#ifndef DSP_CHAIN_HPP
#define DSP_CHAIN_HPP

#include "idspeffect.hpp"
#include <vector>

class DSPChain
{
public:
    void AddEffect(IDSPEffect* effect);
    void Process(AudioBuffer& buffer, size_t numFrames);

private:
    std::vector<IDSPEffect*> m_effects;
};

#endif
