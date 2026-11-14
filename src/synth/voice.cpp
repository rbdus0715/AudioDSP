#include "voice.hpp"
#include <cmath>

constexpr double PI = 3.14159265358979323846;
// Vital 오실레이터 level 기본값 (0.707^2). 필터는 이 크기를 전제로 동작
constexpr double kOscLevel = 0.5;

static double NoteToFrequency(double note) {
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
    : m_sampleRate(sampleRate), m_env(sampleRate, 0.01, 0.3, 0.7, 0.3),
      m_filterEnv(sampleRate, 0.01, 0.3, 0.0, 0.3), m_cutoff(sampleRate, 136.0),
      m_resonance(sampleRate, 0.0), m_filterEnvAmount(sampleRate, 0.0),
      m_keytrack(sampleRate, 0.0), m_blend(sampleRate, 0.0),
      m_drive(sampleRate, 0.0) {}

void Voice::SnapParameters() {
  m_cutoff.Snap();
  m_resonance.Snap();
  m_filterEnvAmount.Snap();
  m_keytrack.Snap();
  m_blend.Snap();
  m_drive.Snap();
}

void Voice::NoteOn(int note, float velocity, uint64_t age) {
  if (!IsActive()) {
    m_filter.Reset();
    SnapParameters();
  }
  m_age = age;
  m_note = note;
  m_velocity = velocity;
  m_frequency = NoteToFrequency(note);
  m_held = true;
  m_env.NoteOn();
  m_filterEnv.NoteOn();
}

void Voice::NoteOff() {
  m_held = false;
  m_env.NoteOff();
  m_filterEnv.NoteOff();
}

void Voice::Kill() {
  m_held = false;
  m_env.Kill();
  m_filterEnv.NoteOff();
}

void Voice::Render(AudioBuffer &buffer, size_t start, size_t count,
                   double volume) {
  if (!IsActive())
    return;
  double dt = m_frequency / m_sampleRate; // 한 샘플당 위상 증가량 (0~1 단위)
  for (size_t i = start; i < start + count; i++) {
    double cutoff = m_cutoff.Next() + m_keytrack.Next() * (m_note - 60) +
                    m_filterEnvAmount.Next() * m_filterEnv.Next();
    double q = 0.5 * std::pow(32.0, m_resonance.Next());
    m_filter.SetCoefficients(NoteToFrequency(cutoff), q, m_sampleRate);
    m_filter.SetBlend(m_blend.Next());
    m_filter.SetDrive(m_drive.Next());
    double osc = m_filter.Process(kOscLevel * Oscillate(m_phase, dt)) / kOscLevel;
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
