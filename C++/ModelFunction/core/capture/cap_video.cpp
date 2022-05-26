#include "cap_video.h"

CapVideo::CapVideo(int type, int width, int height, int gop, int fps, int bps, int mode)
{
    m_videoCB   = NULL;
    m_videoData = NULL;

    m_type      = type;
    m_width     = width;
    m_height    = height;
    m_gop       = gop;
    m_fps       = fps;
    m_bps       = bps;
    m_mode      = mode;

    m_render    = NULL;
    m_vbuf      = NULL;
}

CapVideo::~CapVideo()
{

}

void
CapVideo::setVideoCB(cap_video_cb video_cb, void *userdata)
{
    m_videoCB   = video_cb;
    m_videoData = userdata;
}

void *
CapVideo::threadProc(ThreadInfo *info)
{
    CapVideo *cap = (CapVideo*)(info->data);

    cap->caploop();

    return NULL;
}
