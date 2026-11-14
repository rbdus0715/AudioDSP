#include "processor.hpp"
#include "base/source/fstreamer.h"
#include "cids.h"
#include "pluginterfaces/vst/ivstevents.h"
#include <algorithm>
#include <cmath>
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
  m_synth->SetCutoff(m_cutoff);
  m_synth->SetResonance(m_resonance);
  m_synth->SetFilterBlend(m_filterBlend);
  m_synth->SetFilterAttack(m_filterAttack);
  m_synth->SetFilterDecay(m_filterDecay);
  m_synth->SetFilterSustain(m_filterSustain);
  m_synth->SetFilterRelease(m_filterRelease);
  m_synth->SetFilterEnvAmount(m_filterEnvAmount);
  m_synth->SetKeytrack(m_keytrack);
  m_synth->SetFilterDrive(m_filterDrive);
  m_synth->SetLfoRate(std::pow(2.0, m_lfoRate));
  m_synth->SetLfoShape(static_cast<Lfo::Shape>(m_lfoShape));
  m_synth->SetLfoToCutoff(m_lfoToCutoff);
  m_synth->SetLfoToPitch(m_lfoToPitch);
  for (int32 slot = 0; slot < kModSlots; slot++)
    ApplyModSlot(slot);
  m_synth->SnapParameters();
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

      Vst::ParamID id = queue->getParameterId();
      if (IsModParam(id)) {
        int32 slot = (id - kParamModFirst) / kModFieldsPerSlot;
        int32 field = (id - kParamModFirst) % kModFieldsPerSlot;
        if (field == 0)
          m_modSource[slot] = IndexFromNormalized(value, kModSourceCount);
        else if (field == 1)
          m_modDest[slot] = IndexFromNormalized(value, kModDestCount);
        else
          m_modAmount[slot] = kModAmountRange.ToPlain(value);
        ApplyModSlot(slot);
        continue;
      }

      switch (id) {
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
      case kParamCutoff:
        m_cutoff = kCutoffRange.ToPlain(value);
        m_synth->SetCutoff(m_cutoff);
        break;
      case kParamResonance:
        m_resonance = kResonanceRange.ToPlain(value);
        m_synth->SetResonance(m_resonance);
        break;
      case kParamFilterBlend:
        m_filterBlend = kFilterBlendRange.ToPlain(value);
        m_synth->SetFilterBlend(m_filterBlend);
        break;
      case kParamFilterAttack:
        m_filterAttack = kFilterAttackRange.ToPlain(value);
        m_synth->SetFilterAttack(m_filterAttack);
        break;
      case kParamFilterDecay:
        m_filterDecay = kFilterDecayRange.ToPlain(value);
        m_synth->SetFilterDecay(m_filterDecay);
        break;
      case kParamFilterSustain:
        m_filterSustain = kFilterSustainRange.ToPlain(value);
        m_synth->SetFilterSustain(m_filterSustain);
        break;
      case kParamFilterRelease:
        m_filterRelease = kFilterReleaseRange.ToPlain(value);
        m_synth->SetFilterRelease(m_filterRelease);
        break;
      case kParamFilterEnvAmount:
        m_filterEnvAmount = kFilterEnvAmountRange.ToPlain(value);
        m_synth->SetFilterEnvAmount(m_filterEnvAmount);
        break;
      case kParamKeytrack:
        m_keytrack = kKeytrackRange.ToPlain(value);
        m_synth->SetKeytrack(m_keytrack);
        break;
      case kParamFilterDrive:
        m_filterDrive = kFilterDriveRange.ToPlain(value);
        m_synth->SetFilterDrive(m_filterDrive);
        break;
      case kParamLfoRate:
        m_lfoRate = kLfoRateRange.ToPlain(value);
        m_synth->SetLfoRate(std::pow(2.0, m_lfoRate));
        break;
      case kParamLfoShape:
        m_lfoShape = LfoShapeFromNormalized(value);
        m_synth->SetLfoShape(static_cast<Lfo::Shape>(m_lfoShape));
        break;
      case kParamLfoToCutoff:
        m_lfoToCutoff = kLfoToCutoffRange.ToPlain(value);
        m_synth->SetLfoToCutoff(m_lfoToCutoff);
        break;
      case kParamLfoToPitch:
        m_lfoToPitch = kLfoToPitchRange.ToPlain(value);
        m_synth->SetLfoToPitch(m_lfoToPitch);
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
  double cutoff = kCutoffRange.def;
  double resonance = kResonanceRange.def;
  if (version >= 4) {
    if (!s.readDouble(cutoff) || !s.readDouble(resonance))
      return kResultFalse;
  }
  double blend = kFilterBlendRange.def;
  if (version >= 5) {
    if (!s.readDouble(blend))
      return kResultFalse;
  }
  double filterAttack = kFilterAttackRange.def;
  double filterDecay = kFilterDecayRange.def;
  double filterSustain = kFilterSustainRange.def;
  double filterRelease = kFilterReleaseRange.def;
  double filterEnvAmount = kFilterEnvAmountRange.def;
  double keytrack = kKeytrackRange.def;
  if (version >= 6) {
    if (!s.readDouble(filterAttack) ||
        !s.readDouble(filterDecay) ||
        !s.readDouble(filterSustain) ||
        !s.readDouble(filterRelease) ||
        !s.readDouble(filterEnvAmount) ||
        !s.readDouble(keytrack))
      return kResultFalse;
  }
  double drive = kFilterDriveRange.def;
  if (version >= 7) {
    if (!s.readDouble(drive))
      return kResultFalse;
  }
  double lfoRate = kLfoRateRange.def;
  int32 lfoShape = kLfoShapeDefault;
  double lfoToCutoff = kLfoToCutoffRange.def;
  double lfoToPitch = kLfoToPitchRange.def;
  if (version >= 8) {
    if (!s.readDouble(lfoRate) || !s.readInt32(lfoShape) ||
        !s.readDouble(lfoToCutoff) || !s.readDouble(lfoToPitch))
      return kResultFalse;
  }
  int32 modSource[kModSlots] = {};
  int32 modDest[kModSlots] = {};
  double modAmount[kModSlots] = {};
  if (version >= 9) {
    for (int32 slot = 0; slot < kModSlots; slot++)
      if (!s.readInt32(modSource[slot]) || !s.readInt32(modDest[slot]) ||
          !s.readDouble(modAmount[slot]))
        return kResultFalse;
  }
  m_volume = volume;
  m_attack = attack;
  m_release = release;
  m_decay = decay;
  m_sustain = sustain;
  m_waveform = ClampWaveform(waveform);
  m_cutoff = cutoff;
  m_resonance = resonance;
  m_filterBlend = blend;
  m_filterAttack = filterAttack;
  m_filterDecay = filterDecay;
  m_filterSustain = filterSustain;
  m_filterRelease = filterRelease;
  m_filterEnvAmount = filterEnvAmount;
  m_keytrack = keytrack;
  m_filterDrive = drive;
  m_lfoRate = lfoRate;
  m_lfoShape = ClampLfoShape(lfoShape);
  m_lfoToCutoff = lfoToCutoff;
  m_lfoToPitch = lfoToPitch;
  for (int32 slot = 0; slot < kModSlots; slot++) {
    m_modSource[slot] = ClampIndex(modSource[slot], kModSourceCount);
    m_modDest[slot] = ClampIndex(modDest[slot], kModDestCount);
    m_modAmount[slot] = modAmount[slot];
  }
  if (m_synth) {
    m_synth->SetVolume(m_volume);
    m_synth->SetAttack(m_attack);
    m_synth->SetRelease(m_release);
    m_synth->SetDecay(m_decay);
    m_synth->SetSustain(m_sustain);
    m_synth->SetWaveform(static_cast<Synth::Waveform>(m_waveform));
    m_synth->SetCutoff(m_cutoff);
    m_synth->SetResonance(m_resonance);
    m_synth->SetFilterBlend(m_filterBlend);
    m_synth->SetFilterAttack(m_filterAttack);
    m_synth->SetFilterDecay(m_filterDecay);
    m_synth->SetFilterSustain(m_filterSustain);
    m_synth->SetFilterRelease(m_filterRelease);
    m_synth->SetFilterEnvAmount(m_filterEnvAmount);
    m_synth->SetKeytrack(m_keytrack);
    m_synth->SetFilterDrive(m_filterDrive);
    m_synth->SetLfoRate(std::pow(2.0, m_lfoRate));
    m_synth->SetLfoShape(static_cast<Lfo::Shape>(m_lfoShape));
    m_synth->SetLfoToCutoff(m_lfoToCutoff);
    m_synth->SetLfoToPitch(m_lfoToPitch);
    for (int32 slot = 0; slot < kModSlots; slot++)
      ApplyModSlot(slot);
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
  s.writeDouble(m_cutoff);
  s.writeDouble(m_resonance);
  s.writeDouble(m_filterBlend);
  s.writeDouble(m_filterAttack);
  s.writeDouble(m_filterDecay);
  s.writeDouble(m_filterSustain);
  s.writeDouble(m_filterRelease);
  s.writeDouble(m_filterEnvAmount);
  s.writeDouble(m_keytrack);
  s.writeDouble(m_filterDrive);
  s.writeDouble(m_lfoRate);
  s.writeInt32(m_lfoShape);
  s.writeDouble(m_lfoToCutoff);
  s.writeDouble(m_lfoToPitch);
  for (int32 slot = 0; slot < kModSlots; slot++) {
    s.writeInt32(m_modSource[slot]);
    s.writeInt32(m_modDest[slot]);
    s.writeDouble(m_modAmount[slot]);
  }
  return kResultOk;
}

void SynthProcessor::ApplyModSlot(int32 slot) {
  if (!m_synth)
    return;
  m_synth->SetModSource(slot, static_cast<ModSource>(m_modSource[slot]));
  m_synth->SetModDest(slot, static_cast<ModDest>(m_modDest[slot]));
  m_synth->SetModAmount(slot, m_modAmount[slot]);
}

} // namespace AudioDSP