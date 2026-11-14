#ifndef PROCESSOR_HPP
#define PROCESSOR_HPP

#include "audiobuffer.hpp"
#include "cids.h"
#include "pluginterfaces/vst/ivstparameterchanges.h"
#include "public.sdk/source/vst/vstaudioeffect.h"
#include "synth/synth.hpp"
#include <memory>

namespace AudioDSP {

class SynthProcessor : public Steinberg::Vst::AudioEffect {
public:
  SynthProcessor();

  static Steinberg::FUnknown *createInstance(void *) {
    return (Steinberg::Vst::IAudioProcessor *)new SynthProcessor();
  }

  // 생성 직후에 입출력 단자(bus) 선언
  Steinberg::tresult PLUGIN_API initialize(Steinberg::FUnknown *context)
      SMTG_OVERRIDE;
  // 재생 준비 - 샘플레이트를 받아서 Synth 생성, 버퍼를 미리 할당
  Steinberg::tresult PLUGIN_API
  setupProcessing(Steinberg::Vst::ProcessSetup &setup) SMTG_OVERRIDE;
  // 32비트 Float만 지원한다고 답
  Steinberg::tresult PLUGIN_API
  canProcessSampleSize(Steinberg::int32 symbolicSampleSize) SMTG_OVERRIDE;
  // 매 블록마다
  Steinberg::tresult PLUGIN_API process(Steinberg::Vst::ProcessData &data)
      SMTG_OVERRIDE;
  // 프로젝트 저장 및 불러오기
  Steinberg::tresult PLUGIN_API setState(Steinberg::IBStream *state)
      SMTG_OVERRIDE;
  Steinberg::tresult PLUGIN_API getState(Steinberg::IBStream *state)
      SMTG_OVERRIDE;

private:
  // Synth 객체가 아닌 포인터 객체로 -> Synth는 생성자에서 샘플레이트 받아야
  // 하는데, Processor가 만들어질 때는 샘플레이트를 모름. setupProcessing에서
  // 알게되니 그때 만들기 위해 포인터로 둠
  std::unique_ptr<Synth> m_synth;
  AudioBuffer m_buffer; // Synth가 쓸 작업용 버퍼
  double m_volume = kVolumeRange.def;
  double m_attack = kAttackRange.def;
  double m_release = kReleaseRange.def;
  double m_decay = kDecayRange.def;
  double m_sustain = kSustainRange.def;
  Steinberg::int32 m_waveform = kWaveformDefault; // 0~3, Synth::Waveform 순서
  double m_cutoff = kCutoffRange.def;
  double m_resonance = kResonanceRange.def;
  double m_filterBlend = kFilterBlendRange.def;
  double m_filterAttack = kFilterAttackRange.def;
  double m_filterDecay = kFilterDecayRange.def;
  double m_filterSustain = kFilterSustainRange.def;
  double m_filterRelease = kFilterReleaseRange.def;
  double m_filterEnvAmount = kFilterEnvAmountRange.def;
  double m_keytrack = kKeytrackRange.def;
  double m_filterDrive = kFilterDriveRange.def;
};

} // namespace AudioDSP
#endif