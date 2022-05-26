/**
 * @file      track_wave.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-03-07 15:37:38
 *
 * @note
 *  track_wave.h defines
 */
#ifndef __TRACK_WAVE_H__
#define __TRACK_WAVE_H__

#include "audio_track.h"

#define MAX_WAVEHDR_NUM       (50)
/* #define MAX_WAVEBUF_SIZE      (320) */
#define MAX_WAVEBUF_SIZE      (640)

class WaveTrack : public AudioTrack
{
public:
    WaveTrack(uint32_t sps, uint8_t channels, uint8_t bps);
    virtual ~WaveTrack();


protected:
    virtual int        init();
    virtual int        openSound();
    virtual int        closeSound();
    virtual int        setVolume(int vol);

protected:
    virtual void       onADPCM(void *data, int size);

private:
    HWAVEOUT           m_waveout;
    LPWAVEHDR          m_wavehdrs[MAX_WAVEHDR_NUM];

    int                m_hdridx;
};
#endif
