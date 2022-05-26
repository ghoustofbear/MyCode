#include "encoder_x265.h"
#include "tls_error.h"
#include "sys.h"

extern "C"
{
#include "x265.h"
};

X265Encoder::X265Encoder()
{
    m_param = NULL;
    m_enc   = NULL;
    m_pic   = NULL;
}

X265Encoder::~X265Encoder()
{
    m_init = false;

    if (m_pic)
    {
        x265_picture_free(m_pic);
        m_pic = NULL;
    }
}

int
X265Encoder::init(int avfmt, int width, int height, int gop, int fps, int bitrate, int mode)
{
    if (VideoEncoder::init(avfmt, width, height, gop, fps, bitrate, mode) < 0)
    {
        return -1;
    }

    int csp = X265_CSP_I420;
    switch (avfmt)
    {
    case FMT_YUV420P:
        csp = X265_CSP_I420;
        break;

    case FMT_YUV444:
        csp = X265_CSP_I444;
        break;

    case FMT_YV12:
        csp = X265_CSP_I420;
        break;

    case FMT_NV12:
        csp = X265_CSP_NV12;
        break;

    case FMT_RGB24:
        csp = X265_CSP_RGB;
        break;

    case FMT_BGR24:
        csp = X265_CSP_BGR;
        break;

    case FMT_BGRA:
        csp = X265_CSP_BGR;
        break;

    default:
        csp = X265_CSP_I420;
        break;
    }

    m_param = x265_param_alloc();
    x265_param_default(m_param);

    /* write sps,pps before keyframe */
    m_param->bRepeatHeaders = 1;
    m_param->internalCsp    = csp;
    m_param->sourceWidth    = width;
    m_param->sourceHeight   = height;

    /* fps */
    m_param->fpsNum         = fps;
    m_param->fpsDenom       = 1;

    /* gop */
	m_param->bOpenGOP       = 0;
	m_param->keyframeMin    = gop;
	m_param->keyframeMax    = gop;

    /* bitrate */
	m_param->rc.rateControlMode = X265_RC_ABR;
	m_param->rc.bitrate         = bitrate;

    /* b frame */
	m_param->bframes = 3;

    m_enc = x265_encoder_open(m_param);
    if (m_enc == NULL)
    {
        ERRE_PLY("x265_encoder_open err");
        return -1;
    }

    m_pic = x265_picture_alloc();
    x265_picture_init(m_param, m_pic);

    m_init = true;

    return 0;
}

int
X265Encoder::encode(void *src, int ssize, void *dest, int &dsize, uint64_t &pts)
{
    if (!m_init)
    {
        ERRE_PLY("x265 encoder is not inited");
        return -1;
    }

    int w = m_param->sourceWidth;
    int h = m_param->sourceHeight;

    if (X265_CSP_I420 == m_param->internalCsp)
    {
        uint8_t * buf = (uint8_t *)src;

        m_pic->planes[0] = buf;
        m_pic->planes[1] = buf + w * h;
        m_pic->planes[2] = buf + w * h * 5 / 4;
        m_pic->stride[0] = w;
        m_pic->stride[1] = w / 2;
        m_pic->stride[2] = w / 2;
    }
    else if (X265_CSP_I444 == m_param->internalCsp)
    {
        uint8_t * buf = (uint8_t *)src;

        m_pic->planes[0] = buf;
        m_pic->planes[1] = buf + w * h;
        m_pic->planes[2] = buf + w * h * 2;
        m_pic->stride[0] = w;
        m_pic->stride[1] = w;
        m_pic->stride[2] = w;
    }

    x265_nal * nals    = NULL;
    uint32_t   i_nal   = 0;
    uint32_t   i_total = 0;
    if (x265_encoder_encode(m_enc, &nals, &i_nal, m_pic, NULL) < 0)
    {
        ERRE_PLY("x265_encoder_encode err");

        return -1;
    }

    uint32_t offset = 0;
    for (uint32_t i = 0; i < i_nal; i++)
    {
        i_total += nals[i].sizeBytes;
        if (dsize < (int)i_total)
        {
            ERRE_PLY("buffer is too small.");

            return -1;
        }

        memcpy((uint8_t *)dest + offset, nals[i].payload, nals[i].sizeBytes);
        offset += nals[i].sizeBytes;
    }

    dsize = i_total;

    pts = sys->currentTimeMillis();

    return 0;
}
