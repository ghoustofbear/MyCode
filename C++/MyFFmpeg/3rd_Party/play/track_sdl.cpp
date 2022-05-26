#include "track_sdl.h"
#include "tls_error.h"
#include "mempool.h"
#include "SDL2/SDL.h"

class SDLInitAudio
{
public:
    SDLInitAudio()
    {
        /* Initialize SDL */
        if (SDL_Init(SDL_INIT_AUDIO | SDL_INIT_TIMER) < 0)
        {
            ERRF_PLY("SDL could not initialize! SDL Error: %s\n", SDL_GetError());
        }
    }
};

SDLTrack::SDLTrack(uint32_t sps, uint8_t channels, uint8_t bps)
    : AudioTrack(sps, channels, bps)
{
    m_audioid = -1;
}

SDLTrack::~SDLTrack()
{
    //SDL_PauseAudioDevice(m_audioid, 1);
    //SDL_CloseAudioDevice(m_audioid);
    SDL_PauseAudio(1);
    SDL_CloseAudio();
    AutoLock locker(m_pcmQu._mtx);
    while (!m_pcmQu.empty())
    {
        mempool->free(m_pcmQu.front());
        m_pcmQu.pop();
    }
}

void
mix_audio(void *udata, Uint8 *stream, int len)
{
    SDLTrack *track = (SDLTrack *)udata;

    SDL_memset(stream, 0, len);

    if (track->m_pcmQu.empty())
    {
        return;
    }

    AutoLock locker(track->m_pcmQu._mtx);
    uint8_t * pcm = track->m_pcmQu.front();

    uint32_t size = ((uint32_t)len > mempool->length(pcm) ? mempool->length(pcm) : len);

    SDL_MixAudio(stream, pcm, size, SDL_MIX_MAXVOLUME);

    mempool->free(track->m_pcmQu.front());
    track->m_pcmQu.pop();
}

int
SDLTrack::init()
{
    SDLInitAudio initializer;

    SDL_AudioSpec spec;
    spec.format   = AUDIO_S16SYS;
    spec.freq     = m_sps;
    spec.channels = m_channels;
    spec.silence  = 0;
    spec.samples  = 320;
    spec.callback = mix_audio;
    spec.userdata = this;

    //m_audioid = SDL_OpenAudioDevice(NULL, 0, &spec, NULL, SDL_AUDIO_ALLOW_ANY_CHANGE);
    m_audioid = SDL_OpenAudio(&spec, NULL);
    if (m_audioid < 0)
    {
        ERRW_PLY("can't open audio device. SDL Error: %s", SDL_GetError());
        return -1;
    }

    //SDL_PauseAudioDevice(m_audioid, 0);
    SDL_PauseAudio(0);
    m_init = true;

    return 0;
}

int
SDLTrack::openSound()
{
    return -1;
}

int
SDLTrack::closeSound()
{
    return -1;
}

int
SDLTrack::setVolume(int vol)
{
    return -1;
}

void
SDLTrack::onADPCM(void *data, int size)
{
    if (!m_init)
    {
        init();
    }

    /* TODO:clear pcm queue which size is large then 50:  <27-04-18, yourname> */
    AutoLock locker(m_pcmQu._mtx);
    uint8_t *buf = (uint8_t *)mempool->alloc(size);
    memcpy(buf, data, size);
    m_pcmQu.push(buf);
}
