#ifndef AUDIO_OBJECT_HPP
#define AUDIO_OBJECT_HPP

#include "iaudiodata.hpp"
#include "sampleinfo.hpp"

class AudioObject
{
public:
    AudioObject(const SampleInfo& info, IAudioData* data);
    bool GenerateSamples(float* stream, size_t streamLength);
    void SetPos(double pos);

private:
    size_t m_audioPos;
    size_t m_audioLength;
    SampleInfo m_sampleInfo;
    IAudioData* m_audioData;

    size_t PosToAbsolute(double pos);
};

#endif