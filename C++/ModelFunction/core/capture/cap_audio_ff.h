/**
 * @file      cap_audio_ff.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-09-11 10:37:54
 *
 * @note
 *  cap_audio_ff.h defines
 */
#ifndef __CAP_AUDIO_FF_H__
#define __CAP_AUDIO_FF_H__
extern "C" {
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
};

#include "cap_audio.h"
class FFCapAudio: public CapAudio
{
public:
    FFCapAudio(int mode, int sps, int channels, int bps, int ssize);
    virtual ~FFCapAudio();

public:
    virtual int        start();
    virtual void       stop();
private:
};

#endif
