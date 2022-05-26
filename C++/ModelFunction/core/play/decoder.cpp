#include "decoder.h"
#include "vdeclistener.h"
#include "adeclistener.h"
#include "tls_error.h"


/**
 * VideoDecoder
 */
VideoDecoder::VideoDecoder()
{
    m_codec  = CODEC_H264;
    m_avfmt  = FMT_YUV420P;
    m_width  = 0;
    m_height = 0;
    m_fps    = 25;
    m_vbuf   = NULL;
    m_vsize  = 0;
}

VideoDecoder::~VideoDecoder()
{
    {
        AutoLock locker(_mtx);
        clear();
    }

    if (m_vbuf)
    {
        free(m_vbuf);
    }
}

int
VideoDecoder::type()
{
    return m_type;
}

int
VideoDecoder::codec()
{
    return m_codec;
}

int
VideoDecoder::init(int codec, int avfmt, int width, int height, int fps)
{
    m_codec  = codec;
    m_avfmt  = avfmt;
    m_width  = width;
    m_height = height;
    m_fps    = fps;

    m_vsize = 3 * width * height;
    m_vbuf  = (uint8_t *)malloc(m_vsize);
    if (!m_vbuf)
    {
        ERRE_PLY("malloc failed, size:%d", m_vsize);
        return -1;
    }

    return 0;
}

void
VideoDecoder::addDecoderLisntener(VDecListener *listener)
{
    AutoLock locker(_mtx);
    list<VDecListener *>::const_iterator i = find(begin(), end(), listener);
    if (i == end())
    {
        push_back(listener);
    }
}

void
VideoDecoder::delDecoderLisntener(VDecListener *listener)
{
    AutoLock locker(_mtx);
    list<VDecListener *>::iterator i = find(begin(), end(), listener);
    if (i != end())
    {
        erase(i);
    }
}

void
VideoDecoder::notifyYUV(void *data, int size, int w, int h)
{
    AutoLock locker(_mtx);
    for (list<VDecListener *>::const_iterator i = begin(); i != end(); ++i)
    {
        (*i)->onYUV(data, size, w, h);
    }
}

void
VideoDecoder::notifyRGB(void *data, int size, int w, int h)
{
    AutoLock locker(_mtx);
    for (list<VDecListener *>::const_iterator i = begin(); i != end(); ++i)
    {
        (*i)->onRGB(data, size, w, h);
    }
}

/**
 * AudioDecoder
 */
AudioDecoder::AudioDecoder()
{
    m_abuf  = NULL;
    m_asize = 0;
}

AudioDecoder::~AudioDecoder()
{
    if (m_abuf)
    {
        free(m_abuf);
    }
}

int
AudioDecoder::codec()
{
    return m_codec;
}


void
AudioDecoder::addDecoderLisntener(ADecListener *listener)
{
    AutoLock locker(_mtx);
    list<ADecListener *>::const_iterator i = find(begin(), end(), listener);
    if (i == end())
    {
        push_back(listener);
    }
}

void
AudioDecoder::delDecoderLisntener(ADecListener *listener)
{
    AutoLock locker(_mtx);
    list<ADecListener *>::iterator i = find(begin(), end(), listener);
    if (i != end())
    {
        erase(i);
    }
}

void
AudioDecoder::notifyADPCM(void *data, int size)
{
    AutoLock locker(_mtx);
    for (list<ADecListener *>::const_iterator i = begin(); i != end(); ++i)
    {
        (*i)->onADPCM(data, size);
    }
}
