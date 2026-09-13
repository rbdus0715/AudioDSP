#include "audiobuffer.hpp"
#include <algorithm>

AudioBuffer::AudioBuffer(): m_numChannels(0), m_numFrames(0) {}

AudioBuffer::AudioBuffer(int numChannels, size_t numFrames): m_numChannels(0), m_numFrames(0)
{
    SetSize(numChannels, numFrames);
}

void AudioBuffer::SetSize(int numChannels, size_t numFrames)
{
    if((int)m_channels.size() != numChannels)
    {
        m_channels.resize((size_t)numChannels);
    }
    for(std::vector<float>& channel : m_channels)
    {
        channel.resize(numFrames);
    }

    m_numChannels = numChannels;
    m_numFrames = numFrames;
}

void AudioBuffer::Clear()
{
    for(std::vector<float>& channel : m_channels)
    {
        std::fill(channel.begin(), channel.end(), 0.0f);
    }
}

float* AudioBuffer::GetWritePointer(int channel)
{
    return m_channels[(size_t)channel].data();
}

const float* AudioBuffer::GetReadPointer(int channel) const
{
    return m_channels[(size_t)channel].data();
}
