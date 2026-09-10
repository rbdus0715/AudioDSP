#ifndef SDL_AUDIO_DEVICE_HPP
#define SDL_AUDIO_DEVICE_HPP

#include <SDL2/SDL.h>
#include "iaudiodevice.hpp"

class SDLAudioDevice : public IAudioDevice
{
public:
    virtual IAudioData* CreateAudioFromFile(const std::string& filePath) override;
    virtual void ReleaseAudio(IAudioData* audioData) override;
private:
};

#endif