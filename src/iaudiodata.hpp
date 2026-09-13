#ifndef I_AUDIO_DATA_HPP
#define I_AUDIO_DATA_HPP

#include "sampleinfo.hpp"
#include "audiobuffer.hpp"
#include <cstddef>

class IAudioData
{
public:
    // for polymorphism use virtual keyword
    virtual ~IAudioData() {}
    // pos, numFrames, 반환값 모두 프레임 단위 (샘플 프레임 = 채널 묶음 1개)
    virtual size_t GenerateSamples(
        AudioBuffer& buffer,
        size_t numFrames,
        size_t pos,
        const SampleInfo& info) = 0;
    virtual size_t GetAudioLength() = 0;
};

#endif