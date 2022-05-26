#include "encoder.h"

VideoEncoder::VideoEncoder()
{
    m_vbuf = NULL;
    m_init = false;
}

VideoEncoder::~VideoEncoder()
{

}

int
VideoEncoder::init(int avfmt, int width, int height, int gop, int fps, int bitrate, int mode)
{
    m_avfmt   = avfmt;
    m_width   = width;
    m_height  = height;
    m_gop     = gop;

    m_fps     = fps;
    m_bitrate = bitrate;
    m_mode    = mode;

#if 0
    m_vsize = 3 * width * height;
    m_vbuf  = (uint8_t *)malloc(m_vsize);
    if (!m_vbuf)
    {
        return -1;
    }
#endif

    return 0;
}

AudioEncoder::AudioEncoder()
{

}

AudioEncoder::~AudioEncoder()
{

}

int
AudioEncoder::init()
{
    return 0;
}
