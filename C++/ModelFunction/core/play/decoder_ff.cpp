#include "decoder_ff.h"
#include "tls_error.h"

class FFCodecInitializer
{
public:
    FFCodecInitializer()
    {
        avcodec_register_all();
    }
};

static FFCodecInitializer ffCodecInitializer;

FFVideoDecoder::FFVideoDecoder()
{
    m_type     = T_FFMPEG;
    m_codecCtx = NULL;
    m_yuv      = NULL;
    m_swsCtx   = NULL;
}

FFVideoDecoder::~FFVideoDecoder()
{
    if (m_yuv)
    {
        av_free(m_yuv);
    }

    if (m_codecCtx)
    {
        avcodec_close(m_codecCtx);
        av_free(m_codecCtx);
    }

    if (m_swsCtx)
    {
        sws_freeContext(m_swsCtx);
        avpicture_free(&m_swsPict);
    }
}

int
FFVideoDecoder::init(int codec, int avfmt, int width, int height, int fps)
{
    VideoDecoder::init(codec, avfmt, width, height, fps);

    m_width  = 0;
    m_height = 0;

    enum AVCodecID codecId;
    switch (codec)
    {
    case CODEC_H264:
        codecId = AV_CODEC_ID_H264;
        break;
    case CODEC_HEVC:
        codecId = AV_CODEC_ID_HEVC;
        break;
    case CODEC_MPEG4:
        codecId = AV_CODEC_ID_MPEG4;
        break;
    case CODEC_MJPEG:
        codecId = AV_CODEC_ID_MJPEG;
        break;
    case CODEC_MJPEGB:
        codecId = AV_CODEC_ID_MJPEGB;
        break;
    default:
        return -1;
    }

    AVCodec *avc = avcodec_find_decoder(codecId);
    if (NULL == avc)
    {
        ERRE_PLY("avcodec_find_decoder fali.");
        return -1;
    }

    m_codecCtx = avcodec_alloc_context3(avc);
    if (NULL == m_codecCtx)
    {
        ERRE_PLY("avcodec_alloc_context3 fali.");
        return -1;
    }

    /**
     * For some codecs, such as msmpeg4 and mpeg4, width and height
     * MUST be initialized there because this information is not
     * available in the bitstream.
     */
    m_codecCtx->width  = width;
    m_codecCtx->height = height;

    LOGT_PLY("frame width:  %d\n", width);
    LOGT_PLY("frame height: %d\n", height);

    if (0 > avcodec_open2(m_codecCtx, avc, NULL))
    {
        ERRE_PLY("avcodec_open2 fali.");
        goto FAIL0;
    }

    m_yuv = av_frame_alloc();
    if (NULL == m_yuv)
    {
        ERRE_PLY("av_frame_alloc fali.");
        goto FAIL1;
    }

    switch (avfmt)
    {
    case FMT_RGB565:
        m_avFormat = AV_PIX_FMT_RGB565LE;
        break;
    case FMT_BGR565:
        m_avFormat = AV_PIX_FMT_BGR565LE;
        break;
    case FMT_RGB24:
        m_avFormat = AV_PIX_FMT_RGB24;
        break;
    case FMT_BGR24:
        m_avFormat = AV_PIX_FMT_BGR24;
        break;
    case FMT_YUV420P:
    case FMT_YV12:
    default:
        m_avFormat = AV_PIX_FMT_YUV420P;
        break;
    }

    return 0;

FAIL1:
    avcodec_close(m_codecCtx);

FAIL0:
    av_free(m_codecCtx);
    m_codecCtx = NULL;

    return -1;
}

int
FFVideoDecoder::decode(void *src, int ssize, void *dest, int &dsize, int &w, int &h, uint64_t &pts)
{
    int      got_picture = 0;
    int      result      = -1;
    AVPacket avpkt;

    av_init_packet(&avpkt);
    avpkt.data = (uint8_t *)src;
    avpkt.size = ssize;

    result = avcodec_decode_video2(m_codecCtx, m_yuv, &got_picture, &avpkt);
    if (result < 0 || !got_picture)
    {
        ERRW_PLY("avcodec_decode_video2 return valud: %d", result);

        return -1;
    }

    w = m_codecCtx->width;
    h = m_codecCtx->height;

    if (m_width != w || m_height != h)
    {
        if (NULL != m_swsCtx)
        {
            sws_freeContext(m_swsCtx);
            avpicture_free(&m_swsPict);
        }

        m_swsCtx = sws_getContext(w, h, m_codecCtx->pix_fmt, w, h, m_avFormat, SWS_FAST_BILINEAR, NULL, NULL, NULL);

        m_avSize = avpicture_fill(&m_swsPict, NULL, m_avFormat, w, h);
        avpicture_alloc(&m_swsPict, m_avFormat, w, h);

        m_width  = w;
        m_height = h;
    }

    AVPicture *ip = (AVPicture *)m_yuv;
    AVPicture *op = &m_swsPict;
    sws_scale(m_swsCtx, ip->data, ip->linesize, 0, h, op->data, op->linesize);

    if (dest && (m_avSize <= dsize))
    {
        uint8_t *yuv = (uint8_t *)dest;
        memcpy(yuv, op->data[0], op->linesize[0] * h);
        if (op->data[1])
        {
            yuv += op->linesize[0] * h;
            memcpy(yuv, op->data[1], op->linesize[1] * h / 2);
        }

        if (op->data[2])
        {
            yuv += op->linesize[1] * h / 2;
            memcpy(yuv, op->data[2], op->linesize[2] * h / 2);
        }
    }

    if (m_vbuf)
    {
        if (m_avSize > m_vsize)
        {
            m_vbuf  = (uint8_t *)realloc(m_vbuf, m_avSize);
            m_vsize = m_avSize;
        }

        uint8_t *yuv = m_vbuf;
        memcpy(yuv, op->data[0], op->linesize[0] * h);
        if (op->data[1])
        {
            yuv += op->linesize[0] * h;
            memcpy(yuv, op->data[1], op->linesize[1] * h / 2);
        }

        if (op->data[2])
        {
            yuv += op->linesize[1] * h / 2;
            memcpy(yuv, op->data[2], op->linesize[2] * h / 2);
        }

        notifyYUV(m_vbuf, m_avSize, w, h);
    }

    dsize = m_avSize;

    return 0;
}

int
FFVideoDecoder::convert(int sfmt, int dfmt, void *src, int ssize, void *dest, int &dsize, int &w, int &h)
{
    if (!src || !dest)
    {
        ERRE_PLY("invalidate src or dest buffer address");
        return -1;
    }

    AVPixelFormat spixfmt = AV_PIX_FMT_YUV420P;

    switch (sfmt)
    {
    case FMT_YUV420P:
    case FMT_YV12:
        spixfmt = AV_PIX_FMT_YUV420P;
        break;
    default:
        break;

    }

    AVPicture srcpic;
    if (avpicture_fill(&srcpic, (uint8_t *)src, spixfmt, w, h) < 0)
    {
        ERRE_PLY("avpicture_fill fail.");
        return -1;
    }

    if (FMT_YV12 == sfmt)
    {
        /* swap U and V */
        uint8_t * p = srcpic.data[1];
        srcpic.data[1] = srcpic.data[2];
        srcpic.data[2] = p;
    }

    AVPixelFormat dpixfmt = AV_PIX_FMT_RGB24;
    switch (dfmt)
    {
    case FMT_RGB565:
        dpixfmt = AV_PIX_FMT_RGB565;
        break;
    case FMT_BGR565:
        dpixfmt = AV_PIX_FMT_BGR565;
        break;
    case FMT_RGB24:
        dpixfmt = AV_PIX_FMT_RGB24;
        break;
    case FMT_BGR24:
        dpixfmt = AV_PIX_FMT_BGR24;
        break;
    default:
        break;
    }

    AVPicture destpic;
    if (avpicture_fill(&destpic, (uint8_t *)dest, dpixfmt, w, h) < 0)
    {
        ERRE_PLY("avpicture_fill fail.");
        return -1;
    }

    destpic.data[0] = destpic.data[0] + destpic.linesize[0] * (h - 1);
    destpic.data[1] = destpic.data[1] + destpic.linesize[0] * h / 4 - 1;
    destpic.data[2] = destpic.data[2] + destpic.linesize[0] * h / 4 - 1;
    destpic.linesize[0] *= -1;
    destpic.linesize[1] *= -1;
    destpic.linesize[2] *= -1;

    struct SwsContext* swsCtx = NULL;
    /* swsCtx = sws_getContext(w, h, spixfmt, w, h, dpixfmt, SWS_BILINEAR, 0, 0, 0); */
    /* swsCtx = sws_getContext(w, h, spixfmt, w, h, dpixfmt, SWS_POINT, 0, 0, 0); */
    swsCtx = sws_getContext(w, h, spixfmt, w, h, dpixfmt, SWS_FAST_BILINEAR, 0, 0, 0);
    if (!swsCtx)
    {
        ERRE_PLY("sws_getContext fail.");
        return -1;
    }

    if (sws_scale(swsCtx, srcpic.data, srcpic.linesize, 0, h, destpic.data, destpic.linesize) < 0)
    {
        ERRE_PLY("sws_scale fail.");
        sws_freeContext(swsCtx);
        return -1;
    }

    sws_freeContext(swsCtx);

    return 0;
}


FFAudioDecoder::FFAudioDecoder()
{
    m_codec = CODEC_G711A;
    m_codecCtx = NULL;
    m_actx = NULL;
    m_pcm = NULL;
    m_actx = NULL;
    m_sps = 8000;
    m_bps = 16;
    m_channel = 1;
}

FFAudioDecoder::~FFAudioDecoder()
{
    if (m_pcm)
    {
        av_free(m_pcm);
    }

    if (m_codecCtx)
    {
        avcodec_close(m_codecCtx);
        av_free(m_codecCtx);
    }

    if (m_actx)
    {
        swr_close(m_actx);
        swr_free(&m_actx);
    }
}

int FFAudioDecoder::init(int codec, int sps, int bps, int channel)
{
    enum AVCodecID codecId;
    switch (codec)
    {
    case CODEC_G711A:
        codecId = AV_CODEC_ID_PCM_ALAW;
        break;
    case CODEC_G711U:
        codecId = AV_CODEC_ID_PCM_MULAW;
        break;
    case CODEC_G722:
        codecId = AV_CODEC_ID_ADPCM_G722;
        break;
    case CODEC_G726:
        codecId = AV_CODEC_ID_ADPCM_G726;
        break;
    case CODEC_G729:
        codecId = AV_CODEC_ID_G729;
        break;
    default:
        ERRE_PLY("no suport decode type");
        return -1;
    }

    AVCodec *avc = avcodec_find_decoder(codecId);
    if (NULL == avc)
    {
        ERRE_PLY("avcodec_find_decoder fali.codecId:%d", codecId);
        return -1;
    }

    m_codecCtx = avcodec_alloc_context3(avc);
    if (NULL == m_codecCtx)
    {
        ERRE_PLY("avcodec_alloc_context3 fali.");
        return -1;
    }    

//    AVCodecParameters *avPara = avcodec_parameters_alloc();
//    avPara->codec_type = AVMEDIA_TYPE_AUDIO;
//    avPara->codec_id = codecId;
//    avPara->format = AV_SAMPLE_FMT_S16;
//    avPara->channels = 1;
//    avPara->sample_rate = 8000;


    m_codecCtx->sample_fmt       = AV_SAMPLE_FMT_S16;
    m_codecCtx->channels         = channel;
    m_codecCtx->sample_rate      = sps;

    if (0 > avcodec_open2(m_codecCtx, avc, NULL))
    {
        ERRE_PLY("avcodec_open2 fali.");
        goto FAIL0;
    }

    m_pcm = av_frame_alloc();
    if (NULL == m_pcm)
    {
        ERRE_PLY("av_frame_alloc fali.");
        goto FAIL1;
    }

    m_bps = bps;
    m_sps = sps;
    m_channel = channel;
    m_codec = codec;

    ERRE_PLY("FFAudioDecoder::init success, codec:%d sps:%d bps:%d chn:%d", codec, sps, bps, channel);

    return 0;

FAIL1:
    avcodec_close(m_codecCtx);

FAIL0:
    av_free(m_codecCtx);
    m_codecCtx = NULL;

    return -1;
}

int
FFAudioDecoder::decode(void *src, int ssize, void *dest, int &dsize)
{
    if (!src || ssize <= 0)
    {
        return -1;
    }

    int      got_pcm = 0;
    int      result      = -1;
    AVPacket avpkt;

    av_init_packet(&avpkt);
    avpkt.data = (uint8_t *)src;
    avpkt.size = ssize;

    result = avcodec_decode_audio4(m_codecCtx, m_pcm, &got_pcm, &avpkt);
    if (result < 0 || !got_pcm)
    {
        ERRW_PLY("avcodec_decode_audio4 return valud: %d", result);
        return -1;
    }

    int out_size = 2*m_sps;
    uint8_t *out_buffer = (uint8_t *)av_malloc(out_size);

    //ERRW_PLY("avframe, sps:%d chl:%d chns:%d",m_pcm->sample_rate, m_pcm->channel_layout, m_pcm->channels);

    if (0 == resample(m_codecCtx, m_pcm, &out_buffer, out_size))
    {
        if (dest && dsize >= out_size)
        {
            memcpy(dest, out_buffer, out_size);
            dsize = out_size;
        }

        notifyADPCM(out_buffer, out_size);
    }

    av_free(out_buffer);

    return 0;
}

int FFAudioDecoder::resample(AVCodecContext* ctx, AVFrame *pcm, uint8_t** outBuf, int &outSize)
{
    if (m_actx == NULL)
    {
        m_actx = swr_alloc();
        m_actx = swr_alloc_set_opts(m_actx,
                                    AV_CH_LAYOUT_MONO,
                                    AV_SAMPLE_FMT_S16,
                                    m_sps,
                                    AV_CH_LAYOUT_MONO,
                                    AV_SAMPLE_FMT_S16,
                                    m_sps,0,0);
        int ires = swr_init(m_actx);
        char errorBuf[1024] = {0};
        av_make_error_string(errorBuf,1024,ires);
        ERRW_PLY("swr_init:%s", errorBuf);
    }

    int len = swr_convert(m_actx, outBuf, outSize, (const uint8_t**)pcm->data,pcm->nb_samples);
    if (len >= 0)
    {
        outSize = av_samples_get_buffer_size(NULL, m_channel, len, AV_SAMPLE_FMT_S16,0);
        return 0;
    }
    else
    {
        ERRW_PLY("swr_convert fail, %d, out:%d", len, outSize);
        return -1;
    }
}




