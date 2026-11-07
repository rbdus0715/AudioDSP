#include "synth.hpp"
#include <cmath>
constexpr double PI = 3.14159265358979323846;

Synth::Synth(int sampleRate, double phase): m_sampleRate(sampleRate), m_phase(phase) {}

bool Synth::GenerateSamples(AudioBuffer& buffer, size_t numFrames)
{
    // 440 sin wave
    double amplitude = 0.2;
    double frequency = 440;
    double phaseIncrement = 2 * PI * frequency / m_sampleRate;
    
    double sample = 0;
    for (size_t i=0; i<numFrames; i++) {
        sample = amplitude * std::sin(m_phase);
        for(int ch=0; ch<buffer.GetNumChannels(); ch++) {
            buffer.GetWritePointer(ch)[i] += sample;
        }
        m_phase += phaseIncrement;
        if(m_phase >= 2*PI) m_phase -= 2*PI;
    }

    return true; 
}
