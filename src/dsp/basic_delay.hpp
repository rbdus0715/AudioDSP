#ifndef BASIC_DELAY_HPP
#define BASIC_DELAY_HPP

#include "idspeffect.hpp"
#include <vector>

class BasicDelay : public IDSPEffect
{
public:
    BasicDelay(unsigned int sampleRate, double delayTimeSec, float feedback, float mix, int numChannels = 2);

    virtual void Process(AudioBuffer& buffer, size_t numFrames) override;
    virtual void Reset() override;

    void SetDelayTime(double delayTimeSec);
    void SetFeedback(float feedback);
    void SetMix(float mix);

private:
    unsigned int m_sampleRate;
    int m_numChannels;
    float m_feedback;
    float m_mix;

    // 채널별 독립 딜레이 라인 + write pointer
    std::vector<std::vector<float>> m_delayLines;
    std::vector<size_t> m_writePos;

    void Resize(double delayTimeSec);
};

#endif
