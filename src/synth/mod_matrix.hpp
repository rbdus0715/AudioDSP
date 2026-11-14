#ifndef MOD_MATRIX_HPP
#define MOD_MATRIX_HPP

// 소스 -> 목적지 x 양. 슬롯마다 연결 하나.
// Vital: ModulationConnection (source, destination, amount)
enum class ModSource { NONE, LFO, FILTER_ENV, AMP_ENV, VELOCITY, NOTE, COUNT };
enum class ModDest {
  NONE,
  CUTOFF,
  RESONANCE,
  PITCH,
  VOLUME,
  DRIVE,
  BLEND,
  LFO_RATE,
  COUNT
};

constexpr int kNumModSlots = 4;

// amount 1 일 때 목적지가 움직이는 양 (Vital 처럼 파라미터 전체 범위 기준)
inline double ModDestScale(ModDest d) {
  switch (d) {
  case ModDest::CUTOFF:
    return 128.0; // st
  case ModDest::RESONANCE:
    return 1.0;
  case ModDest::PITCH:
    return 24.0; // st
  case ModDest::VOLUME:
    return 1.0;
  case ModDest::DRIVE:
    return 20.0; // dB
  case ModDest::BLEND:
    return 2.0;
  case ModDest::LFO_RATE:
    return 7.0; // octaves
  default:
    return 0.0;
  }
}

#endif
