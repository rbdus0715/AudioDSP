#ifndef ENVELOPE_HPP
#define ENVELOPE_HPP

class Envelope {
public:
  enum class Stage { IDLE, ATTACK, DECAY, SUSTAIN, RELEASE };

  Envelope(int sampleRate, double attackSec, double decaySec,
           double sustainLevel, double releaseSec)
      : m_sampleRate(sampleRate), m_attackSec(attackSec), m_decaySec(decaySec),
        m_sustainLevel(sustainLevel), m_releaseSec(releaseSec) {}
  void EnterStage(Stage stage) {
    m_stage = stage;
    m_position = 0.0;
    m_startValue = m_level;
  }
  void NoteOn() { EnterStage(Stage::ATTACK); }
  void NoteOff() {
    if (m_stage != Stage::IDLE)
      EnterStage(Stage::RELEASE);
  }
  bool IsActive() const { return m_stage != Stage::IDLE; }
  double Next() {
    switch (m_stage) {
    case Stage::ATTACK:
      m_position += 1 / (m_attackSec * m_sampleRate);
      m_level = m_startValue + (1.0 - m_startValue) * m_position;
      if (m_position >= 1.0) {
        m_level = 1.0;
        EnterStage(Stage::DECAY);
      }
      break;
    case Stage::DECAY:
      m_position += 1.0 / (m_decaySec * m_sampleRate);
      m_level = 1.0 - (1.0 - m_sustainLevel) * m_position;
      if (m_position >= 1.0) {
        m_level = m_sustainLevel;
        EnterStage(Stage::SUSTAIN);
      }
      break;
    case Stage::RELEASE:
      m_position += 1.0 / (m_releaseSec * m_sampleRate);
      m_level = m_startValue * (1.0 - m_position);
      if (m_position >= 1.0) {
        m_level = 0.0;
        EnterStage(Stage::IDLE);
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
  double m_position = 0.0;
  double m_startValue = 0.0;
};

#endif