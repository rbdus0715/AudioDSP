#include "controller.hpp"
#include "base/source/fstreamer.h"
#include "cids.h"

using namespace Steinberg;

namespace AudioDSP {

tresult PLUGIN_API SynthController::initialize(FUnknown *context) {
  tresult result = EditControllerEx1::initialize(context);
  if (result != kResultOk)
    return result;

  parameters.addParameter(new Vst::RangeParameter(
      STR16("Volume"), kParamVolume, STR16(""), kVolumeRange.min,
      kVolumeRange.max, kVolumeRange.def));
  parameters.addParameter(new Vst::RangeParameter(
      STR16("Attack"), kParamAttack, STR16("s"), kAttackRange.min,
      kAttackRange.max, kAttackRange.def));
  parameters.addParameter(new Vst::RangeParameter(
      STR16("Release"), kParamRelease, STR16("s"), kReleaseRange.min,
      kReleaseRange.max, kReleaseRange.def));
  parameters.addParameter(new Vst::RangeParameter(
      STR16("Decay"), kParamDecay, STR16("s"), kDecayRange.min, kDecayRange.max,
      kDecayRange.def));
  parameters.addParameter(new Vst::RangeParameter(
      STR16("Sustain"), kParamSustain, STR16(""), kSustainRange.min,
      kSustainRange.max, kSustainRange.def));

  // StringListParameter: stepCount 와 정규화 변환을 알아서 해주고,
  // DAW 에 "Saw" 같은 이름을 보여줌
  auto *wave = new Vst::StringListParameter(STR16("Waveform"), kParamWaveform);
  wave->appendString(STR16("Sine"));
  wave->appendString(STR16("Saw"));
  wave->appendString(STR16("Square"));
  wave->appendString(STR16("Triangle"));
  wave->getInfo().defaultNormalizedValue =
      WaveformToNormalized(kWaveformDefault);
  wave->setNormalized(WaveformToNormalized(kWaveformDefault));
  parameters.addParameter(wave);

  parameters.addParameter(new Vst::RangeParameter(
      STR16("Cutoff"), kParamCutoff, STR16("st"), kCutoffRange.min,
      kCutoffRange.max, kCutoffRange.def));
  parameters.addParameter(new Vst::RangeParameter(
      STR16("Resonance"), kParamResonance, STR16(""), kResonanceRange.min,
      kResonanceRange.max, kResonanceRange.def));
  parameters.addParameter(new Vst::RangeParameter(
      STR16("Filter Blend"), kParamFilterBlend, STR16(""),
      kFilterBlendRange.min, kFilterBlendRange.max, kFilterBlendRange.def));
  parameters.addParameter(new Vst::RangeParameter(
      STR16("Filter Attack"), kParamFilterAttack, STR16("s"), kFilterAttackRange.min, kFilterAttackRange.max,
      kFilterAttackRange.def));
  parameters.addParameter(new Vst::RangeParameter(
      STR16("Filter Decay"), kParamFilterDecay, STR16("s"), kFilterDecayRange.min, kFilterDecayRange.max,
      kFilterDecayRange.def));
  parameters.addParameter(new Vst::RangeParameter(
      STR16("Filter Sustain"), kParamFilterSustain, STR16(""), kFilterSustainRange.min, kFilterSustainRange.max,
      kFilterSustainRange.def));
  parameters.addParameter(new Vst::RangeParameter(
      STR16("Filter Release"), kParamFilterRelease, STR16("s"), kFilterReleaseRange.min, kFilterReleaseRange.max,
      kFilterReleaseRange.def));
  parameters.addParameter(new Vst::RangeParameter(
      STR16("Filter Env Amount"), kParamFilterEnvAmount, STR16("st"), kFilterEnvAmountRange.min, kFilterEnvAmountRange.max,
      kFilterEnvAmountRange.def));
  parameters.addParameter(new Vst::RangeParameter(
      STR16("Keytrack"), kParamKeytrack, STR16(""), kKeytrackRange.min, kKeytrackRange.max,
      kKeytrackRange.def));

  return kResultOk;
}

tresult PLUGIN_API SynthController::terminate() {
  return EditControllerEx1::terminate();
}

tresult PLUGIN_API SynthController::setComponentState(IBStream *state) {
  if (!state)
    return kResultFalse;

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

  setParamNormalized(kParamVolume, kVolumeRange.ToNormalized(volume));
  setParamNormalized(kParamAttack, kAttackRange.ToNormalized(attack));
  setParamNormalized(kParamRelease, kReleaseRange.ToNormalized(release));
  setParamNormalized(kParamDecay, kDecayRange.ToNormalized(decay));
  setParamNormalized(kParamSustain, kSustainRange.ToNormalized(sustain));
  setParamNormalized(kParamWaveform, WaveformToNormalized(waveform));
  setParamNormalized(kParamCutoff, kCutoffRange.ToNormalized(cutoff));
  setParamNormalized(kParamResonance, kResonanceRange.ToNormalized(resonance));
  setParamNormalized(kParamFilterBlend, kFilterBlendRange.ToNormalized(blend));
  setParamNormalized(kParamFilterAttack, kFilterAttackRange.ToNormalized(filterAttack));
  setParamNormalized(kParamFilterDecay, kFilterDecayRange.ToNormalized(filterDecay));
  setParamNormalized(kParamFilterSustain, kFilterSustainRange.ToNormalized(filterSustain));
  setParamNormalized(kParamFilterRelease, kFilterReleaseRange.ToNormalized(filterRelease));
  setParamNormalized(kParamFilterEnvAmount, kFilterEnvAmountRange.ToNormalized(filterEnvAmount));
  setParamNormalized(kParamKeytrack, kKeytrackRange.ToNormalized(keytrack));

  return kResultOk;
}

} // namespace AudioDSP