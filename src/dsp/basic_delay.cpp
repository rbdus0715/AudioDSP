#include "basic_delay.hpp"
#include <algorithm>

BasicDelay::BasicDelay(unsigned int sampleRate, double delayTimeSec, float feedback, float mix, int numChannels):
    m_sampleRate(sampleRate), m_numChannels(numChannels),
    m_feedback(feedback), m_mix(mix)
{
    Resize(delayTimeSec);
}

void BasicDelay::Resize(double delayTimeSec)
{
    size_t delaySamples = (size_t)(delayTimeSec * m_sampleRate);
    m_delayLines.assign((size_t)m_numChannels, std::vector<float>(delaySamples > 0 ? delaySamples : 1, 0.0f));
    m_writePos.assign((size_t)m_numChannels, 0);
}

void BasicDelay::SetDelayTime(double delayTimeSec)
{
    Resize(delayTimeSec);
}

void BasicDelay::SetFeedback(float feedback)
{
    m_feedback = feedback;
}

void BasicDelay::SetMix(float mix)
{
    m_mix = mix;
}

void BasicDelay::Reset()
{
    for(std::vector<float>& line : m_delayLines)
    {
        std::fill(line.begin(), line.end(), 0.0f);
    }
    std::fill(m_writePos.begin(), m_writePos.end(), 0);
}

void BasicDelay::Process(AudioBuffer& buffer, size_t numFrames)
{
    int numChannels = m_numChannels < buffer.GetNumChannels() ? m_numChannels : buffer.GetNumChannels();

    for(int ch = 0; ch < numChannels; ch++)
    {
        float* channelData = buffer.GetWritePointer(ch);
        std::vector<float>& delayLine = m_delayLines[(size_t)ch];
        size_t bufSize = delayLine.size();
        size_t writePos = m_writePos[(size_t)ch];

        for(size_t i = 0; i < numFrames; i++)
        {
            float in = channelData[i];
            float delayed = delayLine[writePos];

            delayLine[writePos] = in + delayed * m_feedback;
            channelData[i] = in * (1.0f - m_mix) + delayed * m_mix;

            writePos = (writePos + 1) % bufSize;
        }
        m_writePos[(size_t)ch] = writePos;
    }
}
