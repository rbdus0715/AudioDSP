#include "sdlaudiocontext.hpp"

static void SDLAudioContext_AudioCallback(void* userdata, Uint8* streamIn, int length)
{
    SDLAudioContext* context = (SDLAudioContext*)userdata;
    context->GenerateSamples(streamIn, length);
}

SDLAudioContext::SDLAudioContext(): m_numChannels(2)
{
    SDL_AudioSpec spec;

    SDL_zero(spec);
    spec.freq = 44100;
    spec.format = AUDIO_S16SYS;
    spec.channels = (Uint8)m_numChannels;
    spec.samples = 2048;
    spec.callback = SDLAudioContext_AudioCallback;
    spec.userdata = this;

    m_device = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, 0);
    if (m_device == 0) 
    {
        // TODO: proper error handling
        throw SDL_GetError();
    }

    SDL_PauseAudioDevice(m_device, 0);
}

SDLAudioContext::~SDLAudioContext()
{
	SDL_CloseAudioDevice(m_device);
}

void SDLAudioContext::PlayAudio(AudioObject& ao)
{
	SDL_LockAudioDevice(m_device);

	// This prevents duplicates
	RemoveAudio(ao);
	m_playingAudio.push_back(&ao);

	SDL_UnlockAudioDevice(m_device);
}

void SDLAudioContext::PauseAudio(AudioObject& ao)
{
	SDL_LockAudioDevice(m_device);

	RemoveAudio(ao);

	SDL_UnlockAudioDevice(m_device);
}

void SDLAudioContext::StopAudio(AudioObject& ao)
{
	SDL_LockAudioDevice(m_device);

	if(RemoveAudio(ao))
	{
		ao.SetPos(0.0);
	}

	SDL_UnlockAudioDevice(m_device);
}

void SDLAudioContext::GenerateSamples(Uint8* streamIn, int streamInLen)
{
	size_t totalSamples = (size_t)(streamInLen/2);
	size_t numFrames = totalSamples / (size_t)m_numChannels;

	// 무음 처리
	m_buffer.SetSize(m_numChannels, numFrames);
	m_buffer.Clear();

	// 믹싱 (planar 상태로 처리)
	std::vector<AudioObject*>::iterator it = m_playingAudio.begin();
	std::vector<AudioObject*>::iterator end = m_playingAudio.end();
	for(; it != end; ++it)
	{
		if(!(*it)->GenerateSamples(m_buffer, numFrames))
		{
			RemoveAudio(*(*it));
		}
	}

	// 클리핑 방지 + planar -> interleaved 변환 (하드웨어로 넘기는 경계에서 한 번만)
	Sint16* stream = (Sint16*)streamIn;
	for(size_t frame = 0; frame < numFrames; frame++)
	{
		for(int ch = 0; ch < m_numChannels; ch++)
		{
			float val = m_buffer.GetReadPointer(ch)[frame];

			if(val > 1.0f)
			{
				val = 1.0f;
			}
			else if(val < -1.0f)
			{
				val = -1.0f;
			}

			stream[frame * (size_t)m_numChannels + (size_t)ch] = (Sint16)(val * 32767);
		}
	}
}

bool SDLAudioContext::RemoveAudio(AudioObject& ao)
{
	std::vector<AudioObject*>::iterator it = m_playingAudio.begin();
	std::vector<AudioObject*>::iterator end = m_playingAudio.end();

	for(; it != end; ++it)
	{
		if(*it == &ao)
		{
			m_playingAudio.erase(it);
			return true;
		}
	}

	return false;
}