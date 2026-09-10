#include <iostream>
#include <SDL2/SDL.h>

#include "iaudiodevice.hpp"
#include "iaudiocontext.hpp"
#include "audioobject.hpp"

#include "sdl/sdlaudiodevice.hpp"
#include "sdl/sdlaudiocontext.hpp"

#define FILE_PATH "../assets/audio/test.wav"

int main(int argc, char** argv) {
	SDL_Init(SDL_INIT_AUDIO);

	IAudioDevice* device = new SDLAudioDevice();
	IAudioContext* context = new SDLAudioContext();

	IAudioData* data = device->CreateAudioFromFile(FILE_PATH);

	SampleInfo info;
	info.volume = 1.0;
	info.pitch = 1.0;

	AudioObject sound(info, data);

	char in = 0;

	std::cout << "Press 'a' to play, 's' to pause, 'd' to stop, 'q' to quit" << std::endl;
	while(in != 'q') 
	{
		std::cin >> in;
		switch(in)
		{
			case 'a':
				context->PlayAudio(sound);
				break;
			case 's':
				context->PauseAudio(sound);
				break;
			case 'd':
				context->StopAudio(sound);
				break;
		}
	}

	device->ReleaseAudio(data);
	delete context;
	delete device;

	SDL_Quit();
	return 0;
}