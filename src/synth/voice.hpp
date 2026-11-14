#ifndef VOICE_HPP
#define VOICE_HPP

#include "../audiobuffer.hpp"
#include "./envelope.hpp"
#include <cstddef>

// Voice 와 Synth 가 같이 쓰므로 클래스 밖에 둠
enum class Waveform { SINE, SAW, SQUARE, TRIANGLE };

// 음 하나를 내는 데 필요한 모든 것 (위상 + 음높이 + 엔벨로프 + 벨로시티).
// 파형/ADSR/볼륨 같은 "공유 파라미터"는 Synth 가 모든 보이스에 뿌려줌.
// Vital: synthesis/framework/voice_handler.h 의 Voice
class Voice {
public:
  explicit Voice(int sampleRate);

  void NoteOn(int note, float velocity);
  void NoteOff();
  bool IsActive() const { return m_env.IsActive(); }
  int GetNote() const { return m_note; }

  // 공유 파라미터
  void SetWaveform(Waveform w) { m_waveform = w; }
  void SetAttack(double sec) { m_env.SetAttack(sec); }
  void SetDecay(double sec) { m_env.SetDecay(sec); }
  void SetSustain(double level) { m_env.SetSustain(level); }
  void SetRelease(double sec) { m_env.SetRelease(sec); }

  // buffer 의 [start, start + count) 구간에 이 보이스 소리를 "더함" (+=)
  void Render(AudioBuffer &buffer, size_t start, size_t count, double volume);

private:
  // t: 위상 0~1, dt: 한 샘플당 위상 증가량 -> 출력 -1~1
  double Oscillate(double t, double dt) const;

  int m_sampleRate;
  Envelope m_env;
  double m_phase = 0.0; // 0~1 (2π 아님)
  double m_frequency = 440.0;
  float m_velocity = 0.0f;
  int m_note = -1;
  Waveform m_waveform = Waveform::SAW;
};

#endif
