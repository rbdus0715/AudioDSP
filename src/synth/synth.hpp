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

private:
  int m_sampleRate;
  double m_phase;
  double m_amplitude = 0.2;
  double m_frequency = 440;
  int m_currentNote = -1;
  double m_volume;
  Envelope m_env;
};

#endif
