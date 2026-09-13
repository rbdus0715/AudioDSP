#ifndef SDL_WAV_AUDIO_DATA_HPP
#define SDL_WAV_AUDIO_DATA_HPP

#include "iaudiocontext.hpp"
#include <SDL2/SDL.h>
#include <string>

class SDLWAVAudioData: public IAudioData
{
public:
    SDLWAVAudioData(const std::string& filename, bool streamFromFile);
    virtual ~SDLWAVAudioData();
    virtual size_t GenerateSamples(AudioBuffer& buffer, size_t numFrames, size_t pos, const SampleInfo& info);
    virtual size_t GetAudioLength();
private:
    Uint8* m_rawStart;
    Sint16* m_samples;
    int m_numChannels;
    size_t m_numFrames;

    SDLWAVAudioData(SDLWAVAudioData& other) {(void) other;}
    void operator=(const SDLWAVAudioData& other) {(void) other;}
};

#endif