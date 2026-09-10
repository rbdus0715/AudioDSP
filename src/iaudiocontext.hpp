#ifndef I_AUDIO_CONTEXT_HPP
#define I_AUDIO_CONTEXT_HPP

#include "audioobject.hpp"

class IAudioContext 
{
public:
    virtual ~IAudioContext() {}
    virtual void PlayAudio(AudioObject& ao) = 0;
    virtual void StopAudio(AudioObject& ao) = 0;
    virtual void PauseAudio(AudioObject& ao) = 0;
};

#endif