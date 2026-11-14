#ifndef VOICE_HPP
#define VOICE_HPP

#include "../audiobuffer.hpp"
#include "./envelope.hpp"
#include "./filter.hpp"
#include "./lfo.hpp"
#include "./smoothed_value.hpp"
#include <cstddef>
#include <cstdint>

enum class Waveform { SINE, SAW, SQUARE, TRIANGLE };

class Voice {
public:
  explicit Voice(int sampleRate);

  void NoteOn(int note, float velocity, uint64_t age);
  void NoteOff();
  void Kill();
  // 스무딩 없이 바로 목표값으로
  void SnapParameters();
  // 소리가 나는 중인가 (릴리즈 포함)
  bool IsActive() const { return m_env.IsActive(); }
  // 건반을 누르고 있는 중인가 (릴리즈 중이면 false)
  bool IsHeld() const { return m_held; }
  bool IsKilling() const { return m_env.IsKilling(); }
  int GetNote() const { return m_note; }
  uint64_t GetAge() const { return m_age; }

  // 공유 파라미터
  void SetWaveform(Waveform w) { m_waveform = w; }
  void SetAttack(double sec) { m_env.SetAttack(sec); }
  void SetDecay(double sec) { m_env.SetDecay(sec); }
  void SetSustain(double level) { m_env.SetSustain(level); }
  void SetRelease(double sec) { m_env.SetRelease(sec); }
  void SetFilterBlend(double blend) { m_blend.Set(blend); }
  void SetFilterDrive(double db) { m_drive.Set(db); }
  void SetCutoff(double midiNote) { m_cutoff.Set(midiNote); }
  void SetResonance(double amount) { m_resonance.Set(amount); }
  void SetFilterEnvAmount(double semitones) { m_filterEnvAmount.Set(semitones); }
  void SetKeytrack(double amount) { m_keytrack.Set(amount); }
  void SetFilterAttack(double sec) { m_filterEnv.SetAttack(sec); }
  void SetFilterDecay(double sec) { m_filterEnv.SetDecay(sec); }
  void SetFilterSustain(double level) { m_filterEnv.SetSustain(level); }
  void SetFilterRelease(double sec) { m_filterEnv.SetRelease(sec); }
  void SetLfoRate(double hz) { m_lfo.SetRate(hz); }
  void SetLfoShape(Lfo::Shape s) { m_lfo.SetShape(s); }
  void SetLfoToCutoff(double semitones) { m_lfoToCutoff.Set(semitones); }
  void SetLfoToPitch(double semitones) { m_lfoToPitch.Set(semitones); }
  void Render(AudioBuffer &buffer, size_t start, size_t count, double volume);

private:
  // t: 위상 0~1, dt: 한 샘플당 위상 증가량 -> 출력 -1~1
  double Oscillate(double t, double dt) const;

  int m_sampleRate;
  Envelope m_env;
  SvfFilter m_filter;
  Envelope m_filterEnv;
  SmoothedValue m_cutoff;
  SmoothedValue m_resonance;
  SmoothedValue m_filterEnvAmount;
  SmoothedValue m_keytrack;
  SmoothedValue m_blend;
  SmoothedValue m_drive;
  Lfo m_lfo;
  SmoothedValue m_lfoToCutoff;
  SmoothedValue m_lfoToPitch;
  double m_phase = 0.0; // 0~1 (2PI x)
  float m_velocity = 0.0f;
  int m_note = -1;
  bool m_held = false;
  uint64_t m_age = 0;
  Waveform m_waveform = Waveform::SAW;
};

#endif
