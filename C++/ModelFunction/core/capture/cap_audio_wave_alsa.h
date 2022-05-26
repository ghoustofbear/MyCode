#ifndef  _CAP_AUDIO_WAVE_ALSA_H_
#define  _CAP_AUDIO_WAVE_ALSA_H_

#include "cap_audio.h"
#include "thread.h"

#ifdef UNIX

#include <alsa/asoundlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>

class AudioEncoder;
class WaveCapAudioAlsa : public CapAudio
{
public:
    WaveCapAudioAlsa(int mode, int sps, int channels, int bps, int ssize);
    virtual ~WaveCapAudioAlsa();

    enum 
    {
        T_CAP_AUDIO,
    };

public:
    virtual int        start();
    virtual void       stop();

    void               capture();

protected:
    static void *      threadProc(ThreadInfo *info);

private:
    bool               opendevice(int mode = 0);
    bool               initdeviceparam();

private:
    AudioEncoder     * m_g711a;
    AudioEncoder     * m_g711u;
    void             * m_g722;

    uint8_t          * m_g711ab;
    uint8_t          * m_g711ub;

    Thread           * m_capThread;
    snd_pcm_t        * m_handle;
    snd_pcm_uframes_t     m_frame;
};
#endif

#endif
