#ifndef ENVELOPE_HPP
#define ENVELOPE_HPP

class Envelope {
public:
  enum class Stage { IDLE, ATTACK, SUSTAIN, RELEASE };

  Envelope(int sampleRate, double attackSec, double releaseSec)
      : m_sampleRate(sampleRate), m_attackSec(attackSec),
        m_releaseSec(releaseSec) {}

  void NoteOn() { m_stage = Stage::ATTACK; }
  void NoteOff() {
    if (m_stage != Stage::IDLE)
      m_stage = Stage::RELEASE;
  }
  bool IsActive() const { return m_stage != Stage::IDLE; }
  double Next() {
    switch (m_stage) {
    case Stage::ATTACK:
      m_level += 1 / (m_attackSec * m_sampleRate);
      if (m_level >= 1.0) {
        m_level = 1.0;
        m_stage = Stage::SUSTAIN;
      }
      break;
    case Stage::RELEASE:
      m_level -= 1 / (m_releaseSec * m_sampleRate);
      if (m_level <= 0.0) {
        m_level = 0.0;
        m_stage = Stage::IDLE;
      }
      break;
    default:
      break;
    }
    return m_level;
  }
  void SetAttack(double sec) { m_attackSec = sec; }
  void SetRelease(double sec) { m_releaseSec = sec; }

private:
  int m_sampleRate;
  double m_attackSec;
  double m_releaseSec;
  Stage m_stage = Stage::IDLE;
  double m_level = 0.0;
};

#endif