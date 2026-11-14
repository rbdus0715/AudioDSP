#include "voice.hpp"
#include <algorithm>
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
      m_drive(sampleRate, 0.0), m_lfo(sampleRate),
      m_lfoToCutoff(sampleRate, 0.0), m_lfoToPitch(sampleRate, 0.0),
      m_modAmount{SmoothedValue(sampleRate, 0.0), SmoothedValue(sampleRate, 0.0),
                  SmoothedValue(sampleRate, 0.0),
                  SmoothedValue(sampleRate, 0.0)} {}

void Voice::SnapParameters() {
  m_cutoff.Snap();
  m_resonance.Snap();
  m_filterEnvAmount.Snap();
  m_keytrack.Snap();
  m_blend.Snap();
  m_drive.Snap();
  m_lfoToCutoff.Snap();
  m_lfoToPitch.Snap();
  for (SmoothedValue &a : m_modAmount)
    a.Snap();
}

void Voice::NoteOn(int note, float velocity, uint64_t age) {
  if (!IsActive()) {
    m_filter.Reset();
    SnapParameters();
  }
  m_age = age;
  m_note = note;
  m_velocity = velocity;
  m_lfo.Retrigger();
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
  for (size_t i = start; i < start + count; i++) {
    double lfo = m_lfo.Next();
    double filterEnv = m_filterEnv.Next();
    double ampEnv = m_env.Next();

    double source[static_cast<int>(ModSource::COUNT)] = {
        0.0, lfo, filterEnv, ampEnv, m_velocity, m_note / 127.0};
    double mod[static_cast<int>(ModDest::COUNT)] = {};
    for (int s = 0; s < kNumModSlots; s++) {
      double amount = m_modAmount[s].Next();
      ModDest d = m_modDest[s];
      mod[static_cast<int>(d)] += amount * ModDestScale(d) *
                                  source[static_cast<int>(m_modSource[s])];
    }
    auto M = [&](ModDest d) { return mod[static_cast<int>(d)]; };

    m_lfo.SetRate(m_lfoRate * std::pow(2.0, M(ModDest::LFO_RATE)));
    double pitch = m_note + m_lfoToPitch.Next() * lfo + M(ModDest::PITCH);
    double dt = NoteToFrequency(pitch) / m_sampleRate;
    double cutoff = m_cutoff.Next() + m_keytrack.Next() * (m_note - 60) +
                    m_filterEnvAmount.Next() * filterEnv +
                    m_lfoToCutoff.Next() * lfo + M(ModDest::CUTOFF);
    double res = std::clamp(m_resonance.Next() + M(ModDest::RESONANCE), 0.0, 1.0);
    double q = 0.5 * std::pow(32.0, res);
    m_filter.SetCoefficients(NoteToFrequency(cutoff), q, m_sampleRate);
    m_filter.SetBlend(std::clamp(m_blend.Next() + M(ModDest::BLEND), 0.0, 2.0));
    m_filter.SetDrive(std::clamp(m_drive.Next() + M(ModDest::DRIVE), 0.0, 20.0));
    double gain = std::max(0.0, 1.0 + M(ModDest::VOLUME));
    double osc = m_filter.Process(kOscLevel * Oscillate(m_phase, dt)) / kOscLevel;
    double sample = volume * gain * m_velocity * osc * ampEnv;
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
