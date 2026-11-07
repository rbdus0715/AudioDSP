#ifndef I_AUDIO_SOURCE_HPP
#define I_AUDIO_SOURCE_HPP

#include "audiobuffer.hpp"
#include <cstddef>

// AudioContext가 재생할 수 있는 모든 것(파일 재생, 신스 등)의 공통 인터페이스.
// Vital로 치면 DAW가 호출하는 processBlock()에 해당한다.
class IAudioSource
{
public:
    virtual ~IAudioSource() = default;

    // buffer에 numFrames만큼 소리를 "더한다"(+=). 여러 소스가 한 버퍼에 믹싱되기 때문.
    // 더 이상 낼 소리가 없으면 false를 반환하고, 컨텍스트는 재생 목록에서 뺀다.
    virtual bool GenerateSamples(AudioBuffer& buffer, size_t numFrames) = 0;

    // Stop 시 처음 위치로 되돌리기 등. 기본은 아무것도 안 함.
    virtual void Rewind() {}
};

#endif
