#ifndef I_AUDIO_DEVICE_HPP
#define I_AUDIO_DEVICE_HPP

#include <string>
#include "iaudiodata.hpp"

class IAudioDevice
{
public:
    virtual ~IAudioDevice() {}
    virtual IAudioData* CreateAudioFromFile(const std::string& filePath) = 0;
    virtual void ReleaseAudio(IAudioData* audioData) = 0;
};

#endif