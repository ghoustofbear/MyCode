#include "audio_track.h"
AudioTrack::AudioTrack(uint32_t sps, uint8_t channels, uint8_t bps)
{
    m_sps      = sps;
    m_channels = channels;
    m_bps      = bps;
    m_init     = false;
}

AudioTrack::~AudioTrack()
{

}
