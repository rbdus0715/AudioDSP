#include "dspchain.hpp"

void DSPChain::AddEffect(IDSPEffect* effect)
{
    m_effects.push_back(effect);
}

void DSPChain::Process(AudioBuffer& buffer, size_t numFrames)
{
    for(IDSPEffect* effect : m_effects)
    {
        effect->Process(buffer, numFrames);
    }
}
