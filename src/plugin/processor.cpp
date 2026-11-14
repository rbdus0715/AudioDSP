#include "processor.hpp"
#include "base/source/fstreamer.h"
#include "cids.h"
#include "pluginterfaces/vst/ivstevents.h"
#include <algorithm>
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
  m_synth->SetDecay(m_decay);
  m_synth->SetSustain(m_sustain);
  m_synth->SetWaveform(static_cast<Synth::Waveform>(m_waveform));
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
      case kParamDecay:
        m_decay = kDecayRange.ToPlain(value);
        m_synth->SetDecay(m_decay);
        break;
      case kParamSustain:
        m_sustain = kSustainRange.ToPlain(value);
        m_synth->SetSustain(m_sustain);
        break;
      case kParamRelease:
        m_release = kReleaseRange.ToPlain(value);
        m_synth->SetRelease(m_release);
        break;
      case kParamWaveform:
        m_waveform = WaveformFromNormalized(value);
        m_synth->SetWaveform(static_cast<Synth::Waveform>(m_waveform));
        break;
      }
    }
  }

  // 버퍼에 렌더링
  m_buffer.Clear();
  int32 rendered = 0;
  int32 numEvents = data.inputEvents ? data.inputEvents->getEventCount() : 0;

  for (int32 i = 0; i < numEvents; i++) {
    Vst::Event e;
    if (data.inputEvents->getEvent(i, e) != kResultOk)
      continue;
    int32 offset = std::clamp(e.sampleOffset, rendered, data.numSamples);
    m_synth->Render(m_buffer, rendered, offset - rendered);
    rendered = offset;

    switch (e.type) {
    case Vst::Event::kNoteOnEvent:
      m_synth->NoteOn(e.noteOn.pitch, e.noteOn.velocity);
      break;
    case Vst::Event::kNoteOffEvent:
      m_synth->NoteOff(e.noteOff.pitch);
      break;
    }
  }
  m_synth->Render(m_buffer, rendered, data.numSamples - rendered);

  // 버퍼 -> DAW 버퍼로 복사
  if (data.numOutputs == 0)
    return kResultOk;
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

  double decay = kDecayRange.def;
  double sustain = kSustainRange.def;
  if (version >= 2) {
    if (!s.readDouble(decay) || !s.readDouble(sustain))
      return kResultFalse;
  }
  int32 waveform = kWaveformDefault;
  if (version >= 3) {
    if (!s.readInt32(waveform))
      return kResultFalse;
  }
  m_volume = volume;
  m_attack = attack;
  m_release = release;
  m_decay = decay;
  m_sustain = sustain;
  m_waveform = ClampWaveform(waveform);
  if (m_synth) {
    m_synth->SetVolume(m_volume);
    m_synth->SetAttack(m_attack);
    m_synth->SetRelease(m_release);
    m_synth->SetDecay(m_decay);
    m_synth->SetSustain(m_sustain);
    m_synth->SetWaveform(static_cast<Synth::Waveform>(m_waveform));
  }
  return kResultOk;
}
tresult PLUGIN_API SynthProcessor::getState(IBStream *state) {
  IBStreamer s(state, kLittleEndian);
  s.writeInt32(kStateVersion);
  s.writeDouble(m_volume);
  s.writeDouble(m_attack);
  s.writeDouble(m_release);
  s.writeDouble(m_decay);
  s.writeDouble(m_sustain);
  s.writeInt32(m_waveform);
  return kResultOk;
}

} // namespace AudioDSP