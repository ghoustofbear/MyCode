#include "cap_audio.h"

CapAudio::CapAudio(int mode, int sps, int channels, int bps, int ssize)
{
    m_mode     = mode;
    m_sps      = sps;
    m_channels = channels;
    m_bps      = bps;
    m_ssize    = ssize;
}

CapAudio::~CapAudio()
{

}

void
CapAudio::setAudioCB(cap_audio_cb audio_cb, void *userdata)
{
    m_audioCB   = audio_cb;
    m_audioData = userdata;
}
