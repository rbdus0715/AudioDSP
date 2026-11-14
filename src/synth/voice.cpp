#include "voice.hpp"
#include <cmath>

constexpr double PI = 3.14159265358979323846;

static double NoteToFrequency(int note) {
  return 440 * std::pow(2, ((note - 69) / 12.0));
}

static double PolyBlep(double t, double dt) {
  // dt: phase increment for one sample (= freq / sampleRate)
  if (t < dt) {
    t /= dt;
    return t + t - t * t - 1.0;
  }
  if (t > 1.0 - dt) {
    t = (t - 1.0) / dt;
    return t * t + t + t + 1.0;
  }
  return 0.0;
}

Voice::Voice(int sampleRate)
    : m_sampleRate(sampleRate), m_env(sampleRate, 0.01, 0.3, 0.7, 0.3) {}

void Voice::NoteOn(int note, float velocity, uint64_t age) {
  if (!IsActive())
    m_filter.Reset();
  m_age = age;
  m_note = note;
  m_velocity = velocity;
  m_frequency = NoteToFrequency(note);
  m_held = true;
  m_env.NoteOn();
}

void Voice::NoteOff() {
  m_held = false;
  m_env.NoteOff();
}

void Voice::Kill() {
  m_held = false;
  m_env.Kill();
}

void Voice::Render(AudioBuffer &buffer, size_t start, size_t count,
                   double volume) {
  if (!IsActive())
    return;
  double dt = m_frequency / m_sampleRate; // 한 샘플당 위상 증가량 (0~1 단위)
  for (size_t i = start; i < start + count; i++) {
    double osc = m_filter.Process(Oscillate(m_phase, dt));
    double sample = volume * m_velocity * osc * m_env.Next();
    for (int ch = 0; ch < buffer.GetNumChannels(); ch++)
      buffer.GetWritePointer(ch)[i] += sample;
    m_phase += dt;
    if (m_phase >= 1.0)
      m_phase -= 1.0;
  }
}

double Voice::Oscillate(double t, double dt) const {
  switch (m_waveform) {
  case Waveform::SINE:
    return std::sin(2 * PI * t);
  case Waveform::SAW:
    return 2 * t - 1 - PolyBlep(t, dt);
  case Waveform::SQUARE: {
    double v = t < 0.5 ? 1.0 : -1.0;
    v += PolyBlep(t, dt);                       // 0 에서 올라가는 점프
    v -= PolyBlep(std::fmod(t + 0.5, 1.0), dt); // 0.5 에서 내려가는 점프
    return v;
  }
  case Waveform::TRIANGLE:
    return 1 - 4 * std::fabs(t - 0.5);
  }
  return 0.0;
}
