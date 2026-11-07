#ifndef SYNTH_HPP
#define SYNTH_HPP

#include "../iaudiosource.hpp"

class Synth : public IAudioSource
{
public:
    explicit Synth(int sampleRate, double phase=0);

    bool GenerateSamples(AudioBuffer& buffer, size_t numFrames) override;

private:
    int m_sampleRate;
    double m_phase;
};

#endif
