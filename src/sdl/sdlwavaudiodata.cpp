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