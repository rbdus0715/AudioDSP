#ifndef I_AUDIO_CONTEXT_HPP
#define I_AUDIO_CONTEXT_HPP

#include "iaudiosource.hpp"

class IAudioContext 
{
public:
    virtual ~IAudioContext() {}
    virtual void PlayAudio(IAudioSource& ao) = 0;
    virtual void StopAudio(IAudioSource& ao) = 0;
    virtual void PauseAudio(IAudioSource& ao) = 0;
};

#endif