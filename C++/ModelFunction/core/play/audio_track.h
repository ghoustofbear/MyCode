/**
 * @file      audio_track.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-03-06 17:38:19
 *
 * @note
 *  audio_track.h defines
 */
#ifndef __AUDIO_TRACK_H__
#define __AUDIO_TRACK_H__
#include "def.h"
#include "adeclistener.h"

class AudioTrack : public ADecListener 
{
public:
    AudioTrack(uint32_t sps, uint8_t channels, uint8_t bps);
    virtual ~AudioTrack();

    virtual int        openSound() = 0;
    virtual int        closeSound() = 0;
    virtual int        setVolume(int vol) = 0;

protected:
    virtual int        init() = 0;


protected:
    volatile bool      m_init;
    uint32_t           m_sps;
    uint8_t            m_channels;
    uint8_t            m_bps;
};
#endif
