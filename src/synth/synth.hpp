#ifndef SYNTH_HPP
#define SYNTH_HPP

#include "../iaudiosource.hpp"
#include "./smoothed_value.hpp"
#include "./voice.hpp"
#include <cstdint>
#include <vector>

class Synth : public IAudioSource {
public:
  using Waveform = ::Waveform; 
  static constexpr int kMaxPolyphony = 8;
  static constexpr int kNumVoices = kMaxPolyphony + 1;
  static constexpr double kVoiceGain = 0.35355339059327373; // 1/√kMaxPolyphony

  explicit Synth(int sampleRate);

  bool GenerateSamples(AudioBuffer &buffer, size_t numFrames) override;
  void NoteOn(int note, float velocity);
  void NoteOff(int note);
  void Render(AudioBuffer &buffer, size_t start, size_t count);
  void SnapParameters();

  void SetVolume(double volume);
  void SetAttack(double sec);
  void SetDecay(double sec);
  void SetSustain(double level);
  void SetRelease(double sec);
  void SetWaveform(Waveform w);
  void SetCutoff(double midiNote);
  void SetResonance(double amount);
  void SetFilterBlend(double blend);
  void SetFilterDrive(double db);
  void SetFilterEnvAmount(double semitones);
  void SetKeytrack(double amount);
  void SetFilterAttack(double sec);
  void SetFilterDecay(double sec);
  void SetFilterSustain(double level);
  void SetFilterRelease(double sec);

private:
  Voice &GrabVoice();
  template <typename Pred> Voice *FindOldest(Pred pred) {
    Voice *oldest = nullptr;
    for (Voice &v : m_voices)
      if (pred(v) && (!oldest || v.GetAge() < oldest->GetAge()))
        oldest = &v;
    return oldest;
  }

  std::vector<Voice> m_voices;
  SmoothedValue m_volume;
  uint64_t m_noteCount = 0;
};

#endif
