#include "sdlwavaudiodata.hpp"

SDLWAVAudioData::SDLWAVAudioData(const std::string& filename, bool streamFromFile)
{
    SDL_AudioSpec wavSpec;
    Uint8* wavStart;
    Uint32 wavLength;

    if(SDL_LoadWAV(filename.c_str(), &wavSpec, &wavStart, &wavLength) == NULL)
    {
        throw filename;
    }
    Uint8 bitDepth = SDL_AUDIO_BITSIZE(wavSpec.format);
    if(bitDepth != 16) {
        throw std::runtime_error("Unsupported bit depth: " + std::to_string(bitDepth));
    }
    m_rawStart = wavStart;
    m_samples = (Sint16*)wavStart;
    m_numChannels = wavSpec.channels;
    m_numFrames = wavLength / (sizeof(Sint16) * (size_t)m_numChannels);
}

SDLWAVAudioData::~SDLWAVAudioData()
{
    SDL_FreeWAV(m_rawStart);
}

/**
 * Generate samples from the audio data.
 * @param buffer: planar 오디오 버퍼. 파일은 인터리브 상태이므로 여기서 디인터리브해서 채운다.
 * @param numFrames: 요청한 프레임(채널 묶음) 개수
 * @param pos: 오디오 데이터 내 위치 (프레임 단위)
 * @param info: information about the sample
 * @return 다음 호출에 쓸 위치 (프레임 단위), 끝났으면 (size_t)-1
 */
size_t SDLWAVAudioData::GenerateSamples(AudioBuffer& buffer, size_t numFrames, size_t pos, const SampleInfo& info)
{
    float pitch = (float)info.pitch;

    if(pos >= m_numFrames)
    {
        return (size_t)-1;
    }

    // select shorter length between remaining length and requested length
    size_t framesLeft = (size_t)((float)(m_numFrames - pos) / pitch);
    size_t framesToGenerate = (numFrames > framesLeft ? framesLeft : numFrames);

    // normalize volume (32768: max value of Sint16)
    float factor = (float)info.volume * 1.0f / 32768.0f;
    float sampleIndex = (float)pos;

    for(size_t i = 0; i < framesToGenerate; i++)
    {
        size_t frame = (size_t)sampleIndex;
        for(int ch = 0; ch < m_numChannels; ch++)
        {
            buffer.GetWritePointer(ch)[i] = m_samples[frame * (size_t)m_numChannels + (size_t)ch] * factor;
        }
        sampleIndex += pitch;
    }

    return (size_t)sampleIndex;
}

size_t SDLWAVAudioData::GetAudioLength()
{
    return m_numFrames;
}