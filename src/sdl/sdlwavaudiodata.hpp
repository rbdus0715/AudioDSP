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
    SDLWAVAudioData(SDLWAVAudioData& other) = delete;
    void operator=(const SDLWAVAudioData& other) = delete;

    virtual size_t GenerateSamples(float* stream, size_t streamLength, size_t pos, const SampleInfo& info);

private:
    Uint8* m_pos;
    Uint8* m_start;
    Uint8* m_end;

    SDLWAVAudioData(SDLWAVAudioData& other) {(void) other;}
    void operator=(const SDLWAVAudioData& other) {(void) other;}
};

#endif