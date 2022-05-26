/**
 * @file      cap_audio_wave.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-04-27 11:01:30
 *
 * @note
 *  cap_audio_wave.h defines
 */
#ifndef __CAP_AUDIO_WAVE_H__
#define __CAP_AUDIO_WAVE_H__
#include "cap_audio.h"

#define MAX_AUDIO_BUFFERS    (1) 

class AudioEncoder;
class WaveCapAudio: public CapAudio
{
    friend void CALLBACK waveInProc(HWAVEIN hWaveIn, UINT uMsg, DWORD_PTR dwInstance, DWORD_PTR dwParam1, DWORD_PTR dwParam2);
public:
    WaveCapAudio(int mode, int sps, int channels, int bps, int ssize);
    virtual ~WaveCapAudio();

public:
    virtual int        start();
    virtual void       stop();
private:
    HWAVEIN            m_waveIn;
    WAVEHDR            m_waveHdr[MAX_AUDIO_BUFFERS];
    char             * m_waveBuf[MAX_AUDIO_BUFFERS];

    AudioEncoder     * m_g711a;
    AudioEncoder     * m_g711u;
    void             * m_g722;

    uint8_t          * m_g711ab;
    uint8_t          * m_g711ub;
};

#endif
