/**
 * @file      cap_video_ff.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-09-11 10:37:54
 *
 * @note
 *  cap_video_ff.h defines
 */
#ifndef __CAP_VIDEO_FF_H__
#define __CAP_VIDEO_FF_H__

extern "C" {
#include "libavcodec/avcodec.h"
#include "libavformat/avformat.h"
#include "libswscale/swscale.h"
#include "libavdevice/avdevice.h"
};

#include "cap_video.h"
#include "c2play.h"

class VideoEncoder;
class FFCapVideo: public CapVideo
{
public:
    FFCapVideo(int type, int width, int height, int gop, int fps, int bps, int mode);
    virtual ~FFCapVideo();

public:
    virtual int        start(HWND wnd);
    virtual void       stop();

protected:
    virtual void       caploop();

public:
    static  int        enumDevice(int &count, cap_device_s *devices);

private:
    AVCodecContext   * m_codecCtx;
    AVFormatContext  * m_fmtCtx;
    AVFrame          * m_yuv;
    AVPicture          m_yuv420;
    int                m_vsize;

    int                m_videoIdx;

    VideoEncoder     * m_encoder;
    uint8_t          * m_frame;
};

#endif
