#ifndef AUDIO_OBJECT_HPP
#define AUDIO_OBJECT_HPP

#include "iaudiodata.hpp"
#include "sampleinfo.hpp"
#include "audiobuffer.hpp"
#include "dsp/dspchain.hpp"

class AudioObject
{
public:
    AudioObject(const SampleInfo& info, IAudioData* data);
    bool GenerateSamples(AudioBuffer& buffer, size_t numFrames);
    void SetPos(double pos);
    DSPChain& GetDSPChain() { return m_dspChain; }

private:
    size_t m_audioPos;
    size_t m_audioLength;
    SampleInfo m_sampleInfo;
    IAudioData* m_audioData;
    DSPChain m_dspChain;

    size_t PosToAbsolute(double pos);
};

#endif