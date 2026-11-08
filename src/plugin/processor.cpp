#include "processor.hpp"
#include "base/source/fstreamer.h"
#include "cids.h"
#include "pluginterfaces/vst/ivstevents.h"
#include <cstring>

using namespace Steinberg;

namespace AudioDSP {

SynthProcessor::SynthProcessor() {
  // 이 Processor와 짝이 되는 Controller가 누구인지 알려줌
  setControllerClass(kSynthControllerUID);
}

tresult PLUGIN_API SynthProcessor::initialize(FUnknown *context) {
  tresult result = AudioEffect::initialize(context);
  if (result != kResultOk)
    return result;

  addAudioOutput(STR16("Stereo Out"), Vst::SpeakerArr::kStereo);
  addEventInput(STR16("Event In"), 1);

  return kResultOk;
}

tresult PLUGIN_API SynthProcessor::setupProcessing(Vst::ProcessSetup &setup) {
  // setup.sampleRate          : 샘플레이트 (double)
  // setup.maxSamplesPerBlock  : process() 한 번에 올 수 있는 최대 샘플 수
  m_synth = std::make_unique<Synth>(static_cast<int>(setup.sampleRate));
  m_synth->SetVolume(m_volume);
  m_synth->SetAttack(m_attack);
  m_synth->SetRelease(m_release);
  m_buffer.SetSize(2, setup.maxSamplesPerBlock);

  return AudioEffect::setupProcessing(setup);
}

tresult PLUGIN_API
SynthProcessor::canProcessSampleSize(int32 symbolicSampleSize) {
  return symbolicSampleSize == Vst::kSample32 ? kResultTrue : kResultFalse;
}

tresult PLUGIN_API SynthProcessor::process(Vst::ProcessData &data) {
  if (data.inputParameterChanges) {
    int32 numParams = data.inputParameterChanges->getParameterCount();
    for (int32 i = 0; i < numParams; i++) {
      Vst::IParamValueQueue *queue =
          data.inputParameterChanges->getParameterData(i);
      if (!queue)
        continue;

      Vst::ParamValue value;
      int32 sampleOffset;
      int32 last = queue->getPointCount() - 1;
      if (queue->getPoint(last, sampleOffset, value) != kResultOk)
        continue;

      switch (queue->getParameterId()) {
      case kParamVolume:
        m_volume = kVolumeRange.ToPlain(value);
        m_synth->SetVolume(m_volume);
        break;
      case kParamAttack:
        m_attack = kAttackRange.ToPlain(value);
        m_synth->SetAttack(m_attack);
        break;

      case kParamRelease:
        m_release = kReleaseRange.ToPlain(value);
        m_synth->SetRelease(m_release);
        break;
      }
    }
  }

  // 이벤트: DAW가 보낸 노트를 Synth에 전달
  if (data.inputEvents) {
    int32 count = data.inputEvents->getEventCount();
    for (int32 i = 0; i < count; i++) {
      Vst::Event e;
      if (data.inputEvents->getEvent(i, e) != kResultOk)
        continue;

      switch (e.type) {
      case Vst::Event::kNoteOnEvent:
        m_synth->NoteOn(e.noteOn.pitch, e.noteOn.velocity);
        break;
      case Vst::Event::kNoteOffEvent:
        m_synth->NoteOff(e.noteOff.pitch);
        break;
      }
    }
  }

  // 출력할 곳이 없으면 여기서 끝
  if (data.numSamples == 0 || data.numOutputs == 0)
    return kResultOk;

  // 버퍼에 렌더링
  m_buffer.Clear();
  m_synth->GenerateSamples(m_buffer, data.numSamples);

  // 버퍼 -> DAW 버퍼로 복사
  Vst::AudioBusBuffers &out = data.outputs[0];
  for (int32 ch = 0; ch < out.numChannels; ch++) {
    std::memcpy(out.channelBuffers32[ch], m_buffer.GetReadPointer(ch),
                data.numSamples * sizeof(float));
  }
  out.silenceFlags = 0;

  return kResultOk;
}

tresult PLUGIN_API SynthProcessor::setState(IBStream *state) {
  IBStreamer s(state, kLittleEndian);
  int32 version = 0;
  double volume, attack, release;
  if (!s.readInt32(version) || !s.readDouble(volume) || !s.readDouble(attack) ||
      !s.readDouble(release))
    return kResultFalse;

  m_volume = volume;
  m_attack = attack;
  m_release = release;

  //
  if (m_synth) {
    m_synth->SetVolume(m_volume);
    m_synth->SetAttack(m_attack);
    m_synth->SetRelease(m_release);
  }
  return kResultOk;
}
tresult PLUGIN_API SynthProcessor::getState(IBStream *state) {
  IBStreamer s(state, kLittleEndian);
  s.writeInt32(kStateVersion);
  s.writeDouble(m_volume);
  s.writeDouble(m_attack);
  s.writeDouble(m_release);
  return kResultOk;
}

} // namespace AudioDSP