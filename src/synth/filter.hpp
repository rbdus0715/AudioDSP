#ifndef FILTER_HPP
#define FILTER_HPP

#include <algorithm>
#include <cmath>

// TPT state variable filter (Andrew Simper / Cytomic)
// Vital: synthesis/filters/digital_svf
class SvfFilter {
public:
  void SetCoefficients(double cutoffHz, double q, double sampleRate) {
    constexpr double kPi = 3.14159265358979323846;
    double fc = std::min(cutoffHz, 0.49 * sampleRate);
    double g = std::tan(kPi * fc / sampleRate);
    double k = 1.0 / q;
    m_a1 = 1.0 / (1.0 + g * (g + k));
    m_a2 = g * m_a1;
    m_a3 = g * m_a2;
  }

  double Process(double v0) {
    double v3 = v0 - m_ic2eq;
    double v1 = m_a1 * m_ic1eq + m_a2 * v3;
    double v2 = m_ic2eq + m_a2 * m_ic1eq + m_a3 * v3;
    m_ic1eq = 2.0 * v1 - m_ic1eq;
    m_ic2eq = 2.0 * v2 - m_ic2eq;
    return v2;
  }

  void Reset() { m_ic1eq = m_ic2eq = 0.0; }

private:
  double m_a1 = 0.0, m_a2 = 0.0, m_a3 = 0.0;
  double m_ic1eq = 0.0, m_ic2eq = 0.0;
};

#endif
