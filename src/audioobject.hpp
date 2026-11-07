#ifndef AUDIO_OBJECT_HPP
#define AUDIO_OBJECT_HPP

#include "iaudiodata.hpp"
#include "sampleinfo.hpp"
#include "audiobuffer.hpp"
#include "dsp/dspchain.hpp"
#include "iaudiosource.hpp"

class AudioObject : public IAudioSource
{
public:
    AudioObject(const SampleInfo& info, IAudioData* data);
    bool GenerateSamples(AudioBuffer& buffer, size_t numFrames) override;
    void Rewind() override { SetPos(0.0); }
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