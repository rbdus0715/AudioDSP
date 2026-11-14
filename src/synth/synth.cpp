#include "synth.hpp"
#include <cmath>

Synth::Synth(int sampleRate) {
  m_voices.reserve(kNumVoices);
  for (int i = 0; i < kNumVoices; i++)
    m_voices.emplace_back(sampleRate);
}

bool Synth::GenerateSamples(AudioBuffer &buffer, size_t numFrames) {
  Render(buffer, 0, numFrames);
  return true;
}

// Vital: VoiceHandler::grabVoice / getVoiceToKill
Voice &Synth::GrabVoice() {
  int sounding = 0;
  for (Voice &v : m_voices)
    if (v.IsActive() && !v.IsKilling())
      sounding++;

  if (sounding >= kMaxPolyphony) {
    Voice *victim = FindOldest([](Voice &v) {
      return v.IsActive() && !v.IsKilling() && !v.IsHeld();
    });
    if (!victim)
      victim = FindOldest([](Voice &v) { return v.IsHeld(); });
    if (victim)
      victim->Kill();
  }

  for (Voice &v : m_voices)
    if (!v.IsActive())
      return v;

  Voice *killing = FindOldest([](Voice &v) { return v.IsKilling(); });
  return killing ? *killing : m_voices[0];
}

void Synth::NoteOn(int note, float velocity) {
  GrabVoice().NoteOn(note, velocity, ++m_noteCount);
}

void Synth::NoteOff(int note) {
  for (Voice &v : m_voices)
    if (v.IsHeld() && v.GetNote() == note)
      v.NoteOff();
}

void Synth::Render(AudioBuffer &buffer, size_t start, size_t count) {
  for (Voice &v : m_voices)
    v.Render(buffer, start, count, m_volume * kVoiceGain);

  for (int ch = 0; ch < buffer.GetNumChannels(); ch++) {
    float *out = buffer.GetWritePointer(ch);
    for (size_t i = start; i < start + count; i++)
      out[i] = std::tanh(out[i]);
  }
}

void Synth::SetVolume(double volume) { m_volume = volume; }
void Synth::SetAttack(double sec) {
  for (Voice &v : m_voices)
    v.SetAttack(sec);
}
void Synth::SetDecay(double sec) {
  for (Voice &v : m_voices)
    v.SetDecay(sec);
}
void Synth::SetSustain(double level) {
  for (Voice &v : m_voices)
    v.SetSustain(level);
}
void Synth::SetRelease(double sec) {
  for (Voice &v : m_voices)
    v.SetRelease(sec);
}
void Synth::SetWaveform(Waveform w) {
  for (Voice &v : m_voices)
    v.SetWaveform(w);
}

void Synth::SetCutoff(double value) {
  for (Voice &v : m_voices)
    v.SetCutoff(value);
}
void Synth::SetResonance(double value) {
  for (Voice &v : m_voices)
    v.SetResonance(value);
}
void Synth::SetFilterBlend(double value) {
  for (Voice &v : m_voices)
    v.SetFilterBlend(value);
}
void Synth::SetFilterEnvAmount(double value) {
  for (Voice &v : m_voices)
    v.SetFilterEnvAmount(value);
}
void Synth::SetKeytrack(double value) {
  for (Voice &v : m_voices)
    v.SetKeytrack(value);
}
void Synth::SetFilterAttack(double value) {
  for (Voice &v : m_voices)
    v.SetFilterAttack(value);
}
void Synth::SetFilterDecay(double value) {
  for (Voice &v : m_voices)
    v.SetFilterDecay(value);
}
void Synth::SetFilterSustain(double value) {
  for (Voice &v : m_voices)
    v.SetFilterSustain(value);
}
void Synth::SetFilterRelease(double value) {
  for (Voice &v : m_voices)
    v.SetFilterRelease(value);
}
