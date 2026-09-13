#ifndef AUDIO_BUFFER_HPP
#define AUDIO_BUFFER_HPP

#include <vector>
#include <cstddef>

// non-interleaved(planar) 오디오 버퍼. 채널별로 독립된 연속 float 배열을 가진다.
class AudioBuffer
{
public:
    AudioBuffer();
    AudioBuffer(int numChannels, size_t numFrames);

    void SetSize(int numChannels, size_t numFrames);
    void Clear();

    int GetNumChannels() const { return m_numChannels; }
    size_t GetNumFrames() const { return m_numFrames; }

    float* GetWritePointer(int channel);
    const float* GetReadPointer(int channel) const;

private:
    int m_numChannels;
    size_t m_numFrames;
    std::vector<std::vector<float>> m_channels;
};

#endif
