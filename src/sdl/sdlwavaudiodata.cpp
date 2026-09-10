#include "sdlwavaudiodata.hpp"

SDLWAVAudioData::SDLWAVAudioData(const std::string& filename, bool streamFromFile)
{
    SDL_AudioSpec wavSpec;
    Uint8* wavStart;
    Uint32 wavLength;

    if(SDL_LoadWAV(filename.c_str(), &wavSpec, &wavStart, &wavLength) == NULL)
    {
        throw filename;
    }

    m_pos = wavStart;
    m_start = wavStart;
    m_end = m_start + wavLength;
}

SDLWAVAudioData::~SDLWAVAudioData()
{
    SDL_FreeWAV(m_start);
}

/**
 * Generate samples from the audio data.
 * @param stream: pointer to the buffer to store the samples
 * @param streamLength: length of the buffer
 * @param pos: position in the audio data
 * @param info: information about the sample
 * @return length of the samples generated
 */
size_t SDLWAVAudioData::GenerateSamples(float* stream, size_t streamLength, size_t pos, const SampleInfo& info)
{
    float pitch = (float)info.pitch;
    m_pos = m_start + pos;

    if(m_pos >= m_end || m_pos < m_start) 
    {
        return (size_t)-1;
    }

    // select shorter length between remaining length and requested length
    Uint32 length = (Uint32)streamLength;
    Uint32 lengthLeft = (Uint32)((m_end - m_pos)/pitch);
    length = (length > lengthLeft ? lengthLeft : length);


    Sint16* samples = (Sint16*)m_pos;
    float sampleIndex = 0;

    // normalize volume (32768: max value of Sint16)
    float factor = (float)info.volume * 1.0f / 32768.0f;
    for(Uint32 i = 0; i < length; i++)
    {
        stream[i] = samples[(Uint32)sampleIndex] * factor;
        sampleIndex += pitch;
    }

    m_pos = (Uint8*)(samples + (size_t)sampleIndex);
    return (size_t)(m_pos - m_start);
}

size_t SDLWAVAudioData::GetAudioLength()
{
    return (size_t)(m_end - m_start);
}