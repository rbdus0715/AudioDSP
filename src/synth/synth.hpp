#ifndef SYNTH_HPP
#define SYNTH_HPP

#include "../iaudiosource.hpp"
#include "./voice.hpp"
#include <cstdint>
#include <vector>

// 보이스들을 관리하는 쪽 (폴리포니).
// 공유 파라미터(파형, ADSR, 볼륨)를 받아서 모든 보이스에 뿌려줌.
// Vital: synthesis/framework/voice_handler.cpp 의 VoiceHandler
class Synth : public IAudioSource {
public:
  using Waveform = ::Waveform; // Synth::Waveform 으로도 계속 쓸 수 있게
  static constexpr int kMaxPolyphony = 8;
  static constexpr int kNumVoices = kMaxPolyphony + 1;

  explicit Synth(int sampleRate);

  bool GenerateSamples(AudioBuffer &buffer, size_t numFrames) override;
  void NoteOn(int note, float velocity);
  void NoteOff(int note);
  void Render(AudioBuffer &buffer, size_t start, size_t count);

  void SetVolume(double volume);
  void SetAttack(double sec);
  void SetDecay(double sec);
  void SetSustain(double level);
  void SetRelease(double sec);
  void SetWaveform(Waveform w);

private:
  // 새 음에 쓸 보이스 고르기
  Voice &GrabVoice();
  template <typename Pred> Voice *FindOldest(Pred pred) {
    Voice *oldest = nullptr;
    for (Voice &v : m_voices)
      if (pred(v) && (!oldest || v.GetAge() < oldest->GetAge()))
        oldest = &v;
    return oldest;
  }

  std::vector<Voice> m_voices;
  double m_volume = 0.2;
  uint64_t m_noteCount = 0;
};

#endif
