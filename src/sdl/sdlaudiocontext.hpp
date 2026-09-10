#ifndef SDL_AUDIO_CONTEXT_HPP
#define SDL_AUDIO_CONTEXT_HPP

#include "../iaudiocontext.hpp"
#include <vector>
#include <SDL2/SDL.h>

class SDLAudioContext : public IAudioContext
{
public:
    SDLAudioContext();
    virtual ~SDLAudioContext();

    virtual void PlayAudio(AudioObject& ao);
    virtual void StopAudio(AudioObject& ao);
    virtual void PauseAudio(AudioObject& ao);

    void GenerateSamples(Uint8* stream, int streamLen);

private:
    SDL_AudioDeviceID m_device;
    std::vector<float> m_stream;
    std::vector<AudioObject*> m_playingAudio;

    bool RemoveAudio(AudioObject& ao);

    // 복사생성자와 대입생성자를 private 메소드로 지정함으로써
	// 자원 중복 해제 문제와 하나의 오디오 장치를 
	// 여러 객체가 소유한다는 것의 의미의 모호함을 없애기 위함
	SDLAudioContext(const SDLAudioContext& other) {(void)other;}
	void operator=(const SDLAudioContext& other) {(void)other;}
};

#endif