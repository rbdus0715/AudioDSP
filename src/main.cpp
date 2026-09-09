#include <SDL2/SDL.h>
#include <iostream>
#define FILE_PATH "../assets/audio/test.wav"

struct AudioData {
    Uint8* pos;
    Uint32 length;
};

void MyAudioCallBack(void* userdata, Uint8* stream, int streamLength)
{
	AudioData* audio = (AudioData*)userdata;

	if(audio->length == 0)
	{
		return;
	}

	Uint32 length = (Uint32)streamLength;
	// 하한선
	length = (length > audio->length ? audio->length : length);
	
	// stream에 오디오에 포지션 정보를 카피
	SDL_memcpy(stream, audio->pos, length);

	audio->pos += length;
	audio->length -= length;
}

int main(int argc, char* argv[]) {
    SDL_Init(SDL_INIT_AUDIO);

    SDL_AudioSpec wavSpec;
    Uint8* wavStart;
    Uint32 wavLength;

    if(SDL_LoadWAV(FILE_PATH, &wavSpec, &wavStart, &wavLength) == NULL) {
        std::cerr << "SDL_LOADWAV Error: " << SDL_GetError() << std::endl;
        SDL_Quit();
        return 1;
    }

    AudioData audio;
    audio.pos = wavStart;
	audio.length = wavLength;

	wavSpec.callback = MyAudioCallBack;
	wavSpec.userdata = &audio;

	SDL_AudioDeviceID device = SDL_OpenAudioDevice(NULL, 0, &wavSpec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);

	if(device == 0) {
		std::cerr << "Error : " << SDL_GetError() << std::endl;
		SDL_FreeWAV(wavStart);
		SDL_Quit();
		return 1;
	}

	SDL_PauseAudioDevice(device, 0);
	while(audio.length > 0)
	{
		SDL_Delay(100);
	}

	SDL_CloseAudioDevice(device);
	SDL_FreeWAV(wavStart);
	SDL_Quit();
	return 0;

    
}