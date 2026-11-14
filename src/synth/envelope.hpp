#ifndef ENVELOPE_HPP
#define ENVELOPE_HPP
#include <cmath>

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
      m_level = m_startValue +
                (1.0 - m_startValue) * Curve(m_position, m_attackPower);
      if (m_position >= 1.0) {
        m_level = 1.0;
        EnterStage(Stage::DECAY);
      }
      break;
    case Stage::DECAY:
      m_position += 1.0 / (m_decaySec * m_sampleRate);
      m_level = 1.0 - (1.0 - m_sustainLevel) * Curve(m_position, m_decayPower);
      if (m_position >= 1.0) {
        m_level = m_sustainLevel;
        EnterStage(Stage::SUSTAIN);
      }
      break;
    case Stage::RELEASE:
      m_position += 1.0 / (m_releaseSec * m_sampleRate);
      m_level = m_startValue * (1.0 - Curve(m_position, m_releasePower));
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

  static double Curve(double x, double power) {
    if (std::fabs(power) < 0.01)
      return x;
    // if x:0 -> 0
    // if x:1 -> 1
    return (std::exp(power * x) - 1.0) / (std::exp(power) - 1.0);
  }

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

  // power member
  double m_attackPower = 0.0;
  double m_decayPower = -2.0;
  double m_releasePower = -2.0;
};

#endif