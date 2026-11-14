#ifndef SYNTH_HPP
#define SYNTH_HPP

#include "../iaudiosource.hpp"
#include "./envelope.hpp"

class Synth : public IAudioSource {
public:
  explicit Synth(int sampleRate, double phase = 0);

  bool GenerateSamples(AudioBuffer &buffer, size_t numFrames) override;
  void NoteOn(int note, float velocity);
  void NoteOff(int note);
  void SetVolume(double volume);
  void SetAttack(double sec);
  void SetRelease(double sec);
  void SetDecay(double sec);
  void SetSustain(double level);
  void Render(AudioBuffer &buffer, size_t start, size_t count);

  // wave
  enum class Waveform { SINE, SAW, SQUARE, TRIANGLE };
  void SetWaveform(Waveform w);

private:
  int m_sampleRate;
  double m_phase;
  double m_amplitude = 0.2;
  double m_frequency = 440;
  int m_currentNote = -1;
  double m_volume;
  Envelope m_env;
  double Oscillate(double t, double dt) const; // t: 0~1 -> -1~1
  Waveform m_waveform = Waveform::SAW;
};

#endif
