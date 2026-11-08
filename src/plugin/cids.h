#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"

namespace AudioDSP {

static const Steinberg::FUID kSynthProcessorUID(0x0F9895B3, 0x314B412A,
                                                0x8D75FC1A, 0x268ACAE8);
static const Steinberg::FUID kSynthControllerUID(0x5CF02157, 0x2F6A42E6,
                                                 0x8E60FCCD, 0xA2836701);

#define AudioDSPSynthVST3Category Steinberg::Vst::PlugType::kInstrumentSynth

enum ParamIDs : Steinberg::Vst::ParamID {
  kParamVolume = 0,
  kParamAttack = 1,
  kParamRelease = 2,
};

struct ParamRange {
  double min, max, def;
  double ToPlain(double normalized) const {
    return min + (max - min) * normalized;
  };
};
constexpr ParamRange kVolumeRange{0.0, 1.0, 0.2};
constexpr ParamRange kAttackRange{1e-9, 1.0, 0.01};
constexpr ParamRange kReleaseRange{1e-9, 1.0, 0.3};
} // namespace AudioDSP
