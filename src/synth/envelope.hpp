#ifndef ENVELOPE_HPP
#define ENVELOPE_HPP

class Envelope {
public:
  enum class Stage { IDLE, ATTACK, DECAY, SUSTAIN, RELEASE };

  Envelope(int sampleRate, double attackSec, double decaySec,
           double sustainLevel, double releaseSec)
      : m_sampleRate(sampleRate), m_attackSec(attackSec), m_decaySec(decaySec),
        m_sustainLevel(sustainLevel), m_releaseSec(releaseSec) {}

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
        m_stage = Stage::DECAY;
      }
      break;
    case Stage::DECAY:
      m_level -= (1.0 - m_sustainLevel) / (m_decaySec * m_sampleRate);
      if (m_level <= m_sustainLevel) {
        m_level = m_sustainLevel;
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
    case Stage::SUSTAIN:
      m_level = m_sustainLevel;
      break;
    default:
      break;
    }
    return m_level;
  }
  void SetAttack(double sec) { m_attackSec = sec; }
  void SetRelease(double sec) { m_releaseSec = sec; }
  void SetDecay(double sec) { m_decaySec = sec; }
  void SetSustain(double level) { m_sustainLevel = level; }

private:
  int m_sampleRate;
  double m_attackSec;
  double m_releaseSec;
  double m_decaySec;
  double m_sustainLevel;
  Stage m_stage = Stage::IDLE;
  double m_level = 0.0;
};

#endif