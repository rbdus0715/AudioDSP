#include "synth.hpp"
#include <cmath>
constexpr double PI = 3.14159265358979323846;

double NoteToFrequency(int note) {
  return 440 * std::pow(2, ((note - 69) / 12.0));
}
static double PolyBlep(double t, double dt) {
  // dt: phase incresement for one sample (= freq / sampleRate)
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
Synth::Synth(int sampleRate, double phase)
    : m_sampleRate(sampleRate), m_phase(phase),
      m_env(sampleRate, 0.01, 0.3, 0.7, 0.3), m_volume(0.2) {}

bool Synth::GenerateSamples(AudioBuffer &buffer, size_t numFrames) {
  Render(buffer, 0, numFrames);
  return true;
}

void Synth::NoteOn(int note, float velocity) {
  m_env.NoteOn();
  m_currentNote = note;
  m_frequency = NoteToFrequency(note);
  m_amplitude = m_volume * velocity;
}
void Synth::NoteOff(int note) {
  if (note == m_currentNote) {
    m_env.NoteOff();
  }
}

void Synth::SetVolume(double volume) { m_volume = volume; }
void Synth::SetAttack(double sec) { m_env.SetAttack(sec); }
void Synth::SetRelease(double sec) { m_env.SetRelease(sec); }
void Synth::SetDecay(double sec) { m_env.SetDecay(sec); }
void Synth::SetSustain(double level) { m_env.SetSustain(level); }

void Synth::Render(AudioBuffer &buffer, size_t start, size_t count) {
  if (!m_env.IsActive())
    return;
  double phaseIncrement = 2 * PI * m_frequency / m_sampleRate;
  double sample = 0;
  for (size_t i = start; i < start + count; i++) {
    double dt = m_frequency / m_sampleRate;
    sample = m_amplitude * Oscillate(m_phase / (2 * PI), dt) * m_env.Next();
    for (int ch = 0; ch < buffer.GetNumChannels(); ch++) {
      buffer.GetWritePointer(ch)[i] += sample;
    }
    m_phase += phaseIncrement;
    if (m_phase >= 2 * PI)
      m_phase -= 2 * PI;
  }
}

void Synth::SetWaveform(Waveform w) { m_waveform = w; }

double Synth::Oscillate(double t, double dt) const {
  switch (m_waveform) {
  case Waveform::SINE:
    return std::sin(2 * PI * t);
  case Waveform::SAW:
    return 2 * t - 1 - PolyBlep(t, dt);
  case Waveform::SQUARE: {
    double v = t < 0.5 ? 1 : -1;
    v += PolyBlep(t, dt);
    v -= PolyBlep(std::fmod(t + 0.5, 1.0), dt);
    return v;
  }
  case Waveform::TRIANGLE:
    return 1 - 4 * abs(t - 0.5);
  }
  return 0.0;
}
