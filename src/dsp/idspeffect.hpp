#ifndef IDSPEFFECT_HPP
#define IDSPEFFECT_HPP

#include "../audiobuffer.hpp"
#include <cstddef>

class IDSPEffect
{
public:
    virtual ~IDSPEffect() = default;
    virtual void Process(AudioBuffer& buffer, size_t numFrames) = 0;
    virtual void Reset() = 0;
};

#endif // IDSPEFFECT_HPP
