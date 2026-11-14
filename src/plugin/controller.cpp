#include "controller.hpp"
#include "base/source/fstreamer.h"
#include "cids.h"
#include "pluginterfaces/base/ustring.h"
#include "vstgui/plugin-bindings/vst3editor.h"
#include <string>

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
  parameters.addParameter(new Vst::RangeParameter(
      STR16("Filter Drive"), kParamFilterDrive, STR16("dB"),
      kFilterDriveRange.min, kFilterDriveRange.max, kFilterDriveRange.def));

  parameters.addParameter(new Vst::RangeParameter(
      STR16("LFO Rate"), kParamLfoRate, STR16("2^x Hz"), kLfoRateRange.min,
      kLfoRateRange.max, kLfoRateRange.def));
  auto *lfoShape = new Vst::StringListParameter(STR16("LFO Shape"), kParamLfoShape);
  lfoShape->appendString(STR16("Sine"));
  lfoShape->appendString(STR16("Triangle"));
  lfoShape->appendString(STR16("Saw"));
  lfoShape->appendString(STR16("Square"));
  parameters.addParameter(lfoShape);
  parameters.addParameter(new Vst::RangeParameter(
      STR16("LFO > Cutoff"), kParamLfoToCutoff, STR16("st"),
      kLfoToCutoffRange.min, kLfoToCutoffRange.max, kLfoToCutoffRange.def));
  parameters.addParameter(new Vst::RangeParameter(
      STR16("LFO > Pitch"), kParamLfoToPitch, STR16("st"), kLfoToPitchRange.min,
      kLfoToPitchRange.max, kLfoToPitchRange.def));

  for (int32 slot = 0; slot < kModSlots; slot++) {
    auto title = [slot](const char *field) {
      std::string s = "Mod " + std::to_string(slot + 1) + " " + field;
      return UString128(s.c_str());
    };
    auto *src = new Vst::StringListParameter(title("Source"), ModParamId(slot, 0));
    for (auto s : {STR16("None"), STR16("LFO"), STR16("Filter Env"), STR16("Amp Env"),
                   STR16("Velocity"), STR16("Note")})
      src->appendString(s);
    parameters.addParameter(src);
    auto *dst = new Vst::StringListParameter(title("Dest"), ModParamId(slot, 1));
    for (auto d : {STR16("None"), STR16("Cutoff"), STR16("Resonance"), STR16("Pitch"),
                   STR16("Volume"), STR16("Drive"), STR16("Blend"), STR16("LFO Rate")})
      dst->appendString(d);
    parameters.addParameter(dst);
    parameters.addParameter(new Vst::RangeParameter(
        title("Amount"), ModParamId(slot, 2), STR16(""), kModAmountRange.min,
        kModAmountRange.max, kModAmountRange.def));
  }

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
  setParamNormalized(kParamFilterDrive, kFilterDriveRange.ToNormalized(drive));
  setParamNormalized(kParamLfoRate, kLfoRateRange.ToNormalized(lfoRate));
  setParamNormalized(kParamLfoShape, LfoShapeToNormalized(lfoShape));
  setParamNormalized(kParamLfoToCutoff, kLfoToCutoffRange.ToNormalized(lfoToCutoff));
  setParamNormalized(kParamLfoToPitch, kLfoToPitchRange.ToNormalized(lfoToPitch));
  for (int32 slot = 0; slot < kModSlots; slot++) {
    setParamNormalized(ModParamId(slot, 0), IndexToNormalized(modSource[slot], kModSourceCount));
    setParamNormalized(ModParamId(slot, 1), IndexToNormalized(modDest[slot], kModDestCount));
    setParamNormalized(ModParamId(slot, 2), kModAmountRange.ToNormalized(modAmount[slot]));
  }

  return kResultOk;
}

IPlugView *PLUGIN_API SynthController::createView(FIDString name) {
  if (FIDStringsEqual(name, Vst::ViewType::kEditor))
    return new VSTGUI::VST3Editor(this, "view", "editor.uidesc");
  return nullptr;
}

} // namespace AudioDSP