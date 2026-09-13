#ifndef SDL_AUDIO_CONTEXT_HPP
#define SDL_AUDIO_CONTEXT_HPP

#include "../iaudiocontext.hpp"
#include "../audiobuffer.hpp"
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
    int m_numChannels;
    // 내부 처리는 전부 planar. 인터리브 변환은 GenerateSamples 끝에서 한 번만 일어난다.
    AudioBuffer m_buffer;
    // 재생중인 오디오 객체들을 저장하는 벡터
    std::vector<AudioObject*> m_playingAudio;

    bool RemoveAudio(AudioObject& ao);

    // 복사생성자와 대입생성자를 private 메소드로 지정하여
	// 자원 중복 해제 문제와 하나의 오디오 장치를 
	// 여러 객체가 소유한다는 것의 의미의 모호함을 제거
    // https://blog.naver.com/rbdus0715/224157056073
	SDLAudioContext(const SDLAudioContext& other) {(void)other;}
	void operator=(const SDLAudioContext& other) {(void)other;}
};

#endif