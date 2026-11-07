#include <iostream>
#include <SDL2/SDL.h>

#include "iaudiodevice.hpp"
#include "iaudiocontext.hpp"
#include "audioobject.hpp"

#include "sdl/sdlaudiodevice.hpp"
#include "sdl/sdlaudiocontext.hpp"
#include "dsp/delay/basic_delay.hpp"
#include "synth/synth.hpp"
#define FILE_PATH "../assets/audio/test.wav"

int main(int argc, char** argv) {
	SDL_Init(SDL_INIT_AUDIO);

	IAudioDevice* device = new SDLAudioDevice();
	SDLAudioContext* context = new SDLAudioContext();

	IAudioData* data = device->CreateAudioFromFile(FILE_PATH);

	SampleInfo info;
	info.volume = 1.0;
	info.pitch = 1.0;

	AudioObject sound(info, data);

	BasicDelay delay(44100, 0.3, 0.35f, 0.3f);
	sound.GetDSPChain().AddEffect(&delay);

	Synth synth(context->GetSampleRate());
	context->PlayAudio(synth);

	// 숫자 키 1~8 -> 다장조 음계 (C4=60 ... C5=72)
	const int keyToNote[8] = {60, 62, 64, 65, 67, 69, 71, 72};
	int lastNote = -1;

	char in = 0;

	std::cout << "[file]  'a' play, 's' pause, 'd' stop" << std::endl;
	std::cout << "[synth] '1'~'8' note on (C D E F G A B C), '0' note off" << std::endl;
	std::cout << "'q' quit  (Enter after press key)" << std::endl;
	while(in != 'q')
	{
		std::cin >> in;

		if(in >= '1' && in <= '8')
		{
			lastNote = keyToNote[in - '1'];
			synth.NoteOn(lastNote, 1.0f);
			continue;
		}

		switch(in)
		{
			case '0':
				synth.NoteOff(lastNote);
				break;
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