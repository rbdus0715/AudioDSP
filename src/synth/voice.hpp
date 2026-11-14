#ifndef VOICE_HPP
#define VOICE_HPP

#include "../audiobuffer.hpp"
#include "./envelope.hpp"
#include "./filter.hpp"
#include <cstddef>
#include <cstdint>

enum class Waveform { SINE, SAW, SQUARE, TRIANGLE };

class Voice {
public:
  explicit Voice(int sampleRate);

  void NoteOn(int note, float velocity, uint64_t age);
  void NoteOff();
  void Kill();
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
  void SetFilter(double cutoffHz, double q) {
    m_filter.SetCoefficients(cutoffHz, q, m_sampleRate);
  }
  void Render(AudioBuffer &buffer, size_t start, size_t count, double volume);

private:
  // t: 위상 0~1, dt: 한 샘플당 위상 증가량 -> 출력 -1~1
  double Oscillate(double t, double dt) const;

  int m_sampleRate;
  Envelope m_env;
  SvfFilter m_filter;
  double m_phase = 0.0; // 0~1 (2PI x)
  double m_frequency = 440.0;
  float m_velocity = 0.0f;
  int m_note = -1;
  bool m_held = false;
  uint64_t m_age = 0;
  Waveform m_waveform = Waveform::SAW;
};

#endif
