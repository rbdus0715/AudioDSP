#pragma once

#include "pluginterfaces/base/funknown.h"
#include "pluginterfaces/vst/vsttypes.h"
#include <algorithm>

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
  kParamDecay = 3,
  kParamSustain = 4,
  kParamWaveform = 5,
};

// v1: volume, attack, release
// v2: + decay, sustain
// v3: + waveform (int32)
const Steinberg::int32 kStateVersion = 3;
struct ParamRange {
  double min, max, def;
  double ToPlain(double normalized) const {
    return min + (max - min) * normalized;
  };
  double ToNormalized(double plain) const {
    return (plain - min) / (max - min);
  }
};
constexpr ParamRange kVolumeRange{0.0, 1.0, 0.2};
constexpr ParamRange kAttackRange{1e-9, 1.0, 0.01};
constexpr ParamRange kReleaseRange{1e-9, 1.0, 0.3};
constexpr ParamRange kDecayRange{0.001, 1.0, 0.3};
constexpr ParamRange kSustainRange{0.0, 1.0, 0.7};

// 파형 선택 (이산 파라미터). 순서는 Synth::Waveform 과 같아야 함
constexpr Steinberg::int32 kWaveformCount = 4; // Sine, Saw, Square, Triangle
constexpr Steinberg::int32 kWaveformDefault = 1; // Saw

// 범위 밖 값(깨진 파일, 미래 버전)을 막아줌
inline Steinberg::int32 ClampWaveform(Steinberg::int32 index) {
  return std::clamp(index, Steinberg::int32(0), kWaveformCount - 1);
}
// VST3 규칙: index = min(stepCount, normalized * (stepCount + 1))
inline Steinberg::int32 WaveformFromNormalized(double normalized) {
  return ClampWaveform(static_cast<Steinberg::int32>(normalized * kWaveformCount));
}
inline double WaveformToNormalized(Steinberg::int32 index) {
  return ClampWaveform(index) / static_cast<double>(kWaveformCount - 1);
}
} // namespace AudioDSP
