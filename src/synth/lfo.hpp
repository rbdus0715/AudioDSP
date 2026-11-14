#ifndef LFO_HPP
#define LFO_HPP

#include <cmath>

// 저주파 오실레이터. 출력 -1~1. 보이스마다 하나, 노트마다 위상 0 에서 시작
class Lfo {
public:
  enum class Shape { SINE, TRIANGLE, SAW, SQUARE };

  explicit Lfo(double sampleRate) : m_sampleRate(sampleRate) {}

  void SetRate(double hz) { m_rate = hz; }
  void SetShape(Shape s) { m_shape = s; }
  void Retrigger() { m_phase = 0.0; }

  double Next() {
    double t = m_phase;
    m_phase += m_rate / m_sampleRate;
    if (m_phase >= 1.0)
      m_phase -= 1.0;

    constexpr double kPi = 3.14159265358979323846;
    switch (m_shape) {
    case Shape::SINE:
      return std::sin(2.0 * kPi * t);
    case Shape::TRIANGLE:
      return 1.0 - 4.0 * std::fabs(t - 0.5);
    case Shape::SAW:
      return 1.0 - 2.0 * t;
    case Shape::SQUARE:
      return t < 0.5 ? 1.0 : -1.0;
    }
    return 0.0;
  }

private:
  double m_sampleRate;
  double m_rate = 2.0;
  double m_phase = 0.0;
  Shape m_shape = Shape::SINE;
};

#endif
