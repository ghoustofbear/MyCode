/**
 * @file      track_sdl.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-03-07 15:38:02
 *
 * @note
 *  track_sdl.h defines
 */
#ifndef __TRACK_SLES_H__
#define __TRACK_SLES_H__
#include "audio_track.h"
#include "tqueue.h"

#include <SLES/OpenSLES.h>
#include <SLES/OpenSLES_Android.h>

class SlesTrack : public AudioTrack
{
public:
    SlesTrack(uint32_t sps, uint8_t channels, uint8_t bps);
    virtual ~SlesTrack();

protected:
    virtual int        init();
    virtual int        openSound();
    virtual int        closeSound();
    virtual int        setVolume(int vol);

    static  void       AudioPlayerCallback(SLAndroidSimpleBufferQueueItf bufferQueueItf, void *context);

protected:
    virtual void       onADPCM(void *data, int size);

    bool               createEngine();
    bool               createAudioPlayer();
    bool               destroyEngine();
    bool               destroyAudioPlayer();


protected:
    struct FraIf_S
    {
        int size;
        char buf[0];
    };
    tqueue<uint8_t *>  m_pcmQu;

    char*              m_tmpBuf;
    const int          m_tmpBufSize = 1024;


private:
    SLObjectItf        m_engineObject;
    SLEngineItf        m_engineEngine;

    SLObjectItf        m_playerObject;
    SLPlayItf          m_playerPlay;
    SLVolumeItf        m_playerVolume;

    SLObjectItf        m_outputMixObject;
    SLAndroidSimpleBufferQueueItf m_playerBufferQueueItf;
    SLPlaybackRateItf  m_playbackRate;

    bool               m_bExitThread;
};

#endif
