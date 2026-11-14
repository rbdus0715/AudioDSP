#ifndef SMOOTHED_VALUE_HPP
#define SMOOTHED_VALUE_HPP

#include <cmath>

class SmoothedValue {
public:
  static constexpr double kSmoothCutoff = 5.0;

  SmoothedValue(double sampleRate, double value) {
    constexpr double kPi = 3.14159265358979323846;
    m_decay = std::exp(-2.0 * kPi * kSmoothCutoff / sampleRate);
    m_target = m_current = value;
  }

  void Set(double target) { m_target = target; }
  void SetHard(double value) { m_target = m_current = value; }
  void Snap() { m_current = m_target; }

  double Next() {
    m_current = m_target + (m_current - m_target) * m_decay;
    return m_current;
  }

private:
  double m_decay;
  double m_target;
  double m_current;
};

#endif
