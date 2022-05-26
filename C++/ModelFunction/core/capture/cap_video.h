/**
 * @file      cap_video.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-03-27 17:40:49
 *
 * @note
 *  cap_video.h defines
 */

#ifndef __CAP_VIDEO_H__
#define __CAP_VIDEO_H__

#include "c2cap.h"
#include "thread.h"

class Render;
class CapVideo
{
public:
    enum
    {
        ENC_H264,
        ENC_HEVC,
        ENC_VP8,
    };

public:
    CapVideo(int type, int width, int height, int gop, int fps, int bps, int mode);
    virtual ~CapVideo();

public:
    void               setVideoCB(cap_video_cb video_cb, void *userdata);

public:
    virtual int        start(HWND wnd) = 0;
    virtual void       stop() = 0;

protected:
    virtual void       caploop() = 0;

    static  void     * threadProc(ThreadInfo *);

protected:
    cap_video_cb       m_videoCB;
    void             * m_videoData;

    Thread             m_captrd;

    int                m_type;
    int                m_width;
    int                m_height;
    int                m_gop;
    int                m_fps;
    int                m_bps;
    int                m_mode;

    Render           * m_render;
    uint8_t          * m_vbuf;
};
#endif
