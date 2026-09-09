#include "audioobject.hpp"

AudioObject::AudioObject(const SampleInfo& info, IAudioData* data)
: m_audioPos(0), m_audioLength(0), m_sampleInfo(info), m_audioData(data) {}

bool AudioObject::GenerateSamples(float* stream, size_t streamLength)
{
    m_audioPos = m_audioData->GenerateSamples(stream, streamLength, m_audioPos, m_sampleInfo);

    // audio buffer is finished.
    if(m_audioPos == (size_t)-1)
    {
        m_audioPos = 0;
        return false;
    }
    return true;
}

void AudioObject::SetPos(double pos)
{
    // clamp position
    if(pos < 0.0)
    {
        pos = 0.0;
    }
    else if (pos > 1.0) 
    {
        pos = 1.0;
    }

    m_audioPos = PosToAbsolute(pos);
}

size_t AudioObject::PosToAbsolute(double pos)
{
    return (size_t)(pos * m_audioLength);
}