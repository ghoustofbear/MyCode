/**
 * @file      encoder.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-04-09 09:54:27
 *
 * @note
 *  encoder.h defines
 */
#ifndef __ENCODER_H__
#define __ENCODER_H__

#include "tlist.h"
#include "c2play.h"

class PLAY_API VideoEncoder
{
public:
    enum
    {
        FMT_YUV420P,
        FMT_YUV444,
        FMT_YV12,
        FMT_NV12,
        FMT_RGB24,
        FMT_BGR24,
        FMT_BGRA,
    };

public:
    VideoEncoder();
    virtual ~VideoEncoder();

public:
    virtual int        init(int avfmt, int width, int height, int gop, int fps, int bitrate, int mode);
    virtual int        encode(void *src, int ssize, void *dest, int &dsize, uint64_t &pts) = 0;

private:
    int                m_type;
    int                m_avfmt;                            /**< enocde input video format */

    int                m_width;
    int                m_height;
    int                m_gop;
    int                m_fps;
    int                m_bitrate;
    int                m_mode;

    uint8_t          * m_vbuf;
    int                m_vsize;

protected:
    bool               m_init;
};

class PLAY_API AudioEncoder
{
public:
    AudioEncoder();
    virtual ~AudioEncoder();

public:
    virtual int        init();
    virtual int        encode(void *src, int ssize, void *dest, int &dsize, uint64_t &pts) = 0;
};

#endif
