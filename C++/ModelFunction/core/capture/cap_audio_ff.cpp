#include "cap_audio_ff.h"

FFCapAudio::FFCapAudio(int mode, int sps, int channels, int bps, int ssize)
    : CapAudio(mode, sps, channels, bps, ssize)
{

}

FFCapAudio::~FFCapAudio()
{

}

int
FFCapAudio::start()
{
    return 0;
}

void
FFCapAudio::stop()
{
}
