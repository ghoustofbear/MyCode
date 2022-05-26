/**
 * @file      cap_audio.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-03-27 17:40:49
 *
 * @note
 *  cap_audio.h defines
 */
#ifndef __CAP_AUDIO_H__
#define __CAP_AUDIO_H__
#include "c2cap.h"

class CapAudio
{
public:
    CapAudio(int mode, int sps, int channels, int bps, int ssize);
    virtual ~CapAudio();

public:
    void               setAudioCB(cap_audio_cb audio_cb, void *userdata);

public:
    virtual int        start() = 0;
    virtual void       stop()  = 0;

protected:
    cap_audio_cb       m_audioCB;
    void             * m_audioData;

    int                m_mode;
    int                m_sps;
    int                m_channels;
    int                m_bps;
    int                m_ssize;
};
#endif
