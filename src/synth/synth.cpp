#include "synth.hpp"
#include <cmath>
constexpr double PI = 3.14159265358979323846;

double NoteToFrequency(int note) {
  return 440 * std::pow(2, ((note - 69) / 12.0));
}

Synth::Synth(int sampleRate, double phase)
    : m_sampleRate(sampleRate), m_phase(phase), m_env(sampleRate, 0.01, 0.3),
      m_volume(0.2) {}

bool Synth::GenerateSamples(AudioBuffer &buffer, size_t numFrames) {
  if (!m_env.IsActive())
    return true;
  double phaseIncrement = 2 * PI * m_frequency / m_sampleRate;
  double sample = 0;
  for (size_t i = 0; i < numFrames; i++) {
    sample = m_amplitude * std::sin(m_phase) * m_env.Next();
    for (int ch = 0; ch < buffer.GetNumChannels(); ch++) {
      buffer.GetWritePointer(ch)[i] += sample;
    }
    m_phase += phaseIncrement;
    if (m_phase >= 2 * PI)
      m_phase -= 2 * PI;
  }

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