/*
 * @Author: liuxu
 * @Date: 2022-04-01 19:33:38
 * @LastEditors: liuxu
 * @LastEditTime: 2022-04-02 13:56:52
 * @FilePath: \C++\MyFFmpeg\src\MyFFmpeg\FFmpeg_Decode.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by liuxu/1056203382@qq.com, All Rights Reserved. 
 */
#include "tls_error.h"
#include"FFmpeg_Decode.h"
/**
 * @description: 
 * @param {*}
 * @return {*}
 */
class FFCodecInitializer
{
public:
    FFCodecInitializer()
    {
        avcodec_register_all();
    }
};

static FFCodecInitializer ffCodecInitializer;

FFmpeg_VideoDecode::FFmpeg_VideoDecode()
{
    m_type     = T_FFMPEG;
    m_codecCtx = NULL;
    m_yuv      = NULL;
    m_swsCtx   = NULL;
}

FFmpeg_VideoDecode::~FFmpeg_VideoDecode()
{
    if (m_yuv)
    {
        av_free(m_yuv);
    }
    if(m_target_frame)
    {
        av_free(m_target_frame);
    }
    if (m_codecCtx)
    {
        avcodec_close(m_codecCtx);
        av_free(m_codecCtx);
    }

    if (m_swsCtx)
    {
        sws_freeContext(m_swsCtx);
        // avpicture_free(&m_swsPict);
    }
}
int
FFmpeg_VideoDecode::init(int codec, int avfmt, int width, int height, int fps)
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
        ERRE_PLY("avcodec_open2 fail.");
        goto FAIL0;
    }

    m_yuv = av_frame_alloc();
    if (NULL == m_yuv)
    {
        ERRE_PLY("av_frame_alloc fail.");
        goto FAIL1;
    }
    m_target_frame=av_frame_alloc();
    if(NULL==m_target_frame)
    {
           ERRE_PLY("m_target_frame fail.");
        goto FAIL1;
    }
    switch (avfmt)
    {
    case FMT_RGB565:
        m_AVPixelFormat = AV_PIX_FMT_RGB565LE;
        break;
    case FMT_BGR565:
        m_AVPixelFormat = AV_PIX_FMT_BGR565LE;
        break;
    case FMT_RGB24:
        m_AVPixelFormat = AV_PIX_FMT_RGB24;
        break;
    case FMT_BGR24:
        m_AVPixelFormat = AV_PIX_FMT_BGR24;
        break;
    case FMT_YUV420P:
    case FMT_YV12:
    default:
        m_AVPixelFormat = AV_PIX_FMT_YUV420P;
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
FFmpeg_VideoDecode::decode(void *src, int ssize, void *dest, int &dsize, int &w, int &h, uint64_t &pts)
{
    int      got_picture = 0;
    int      result      = -1;
    AVPacket avpkt;
    
   
    unsigned char*  pTargetFrameBuffer=NULL;
    
    av_init_packet(&avpkt);
    avpkt.data = (uint8_t *)src;
    avpkt.size = ssize;
    result=avcodec_send_packet(m_codecCtx,&avpkt);

    if (result < 0 )
    {
        ERRW_PLY("avcodec_send_packet return valud: %d", result);

        return -1;
    }

    w = m_codecCtx->width;
    h = m_codecCtx->height;

    if (m_width != w || m_height != h)
    {
        if (NULL != m_swsCtx)
        {
            sws_freeContext(m_swsCtx);
          
        }

        m_swsCtx=sws_getContext(w, h, m_codecCtx->pix_fmt, 
		                        w, h, m_AVPixelFormat, SWS_BICUBIC, NULL, NULL, NULL); 
        

    
        m_target_FrameSize=av_image_get_buffer_size(m_codecCtx->pix_fmt, m_codecCtx->width, 
                                                    m_codecCtx->height, 1);
        pTargetFrameBuffer = (uint8_t*)av_malloc(m_target_FrameSize);

        av_image_fill_arrays(m_target_frame->data, m_target_frame->linesize, pTargetFrameBuffer, 
                            m_AVPixelFormat, w, h, 1);

        m_width  = w;
        m_height = h;
    }


     sws_scale(m_swsCtx, m_yuv->data, m_yuv->linesize, 0, h, m_target_frame->data, m_target_frame->linesize);

    if (dest && (m_target_FrameSize <= dsize))
    {
        uint8_t *target = (uint8_t *)dest;
        memcpy(target, m_target_frame->data[0], m_target_frame->linesize[0] * h);
        if (m_target_frame->data[1])
        {
            target += m_target_frame->linesize[0] * h;
            memcpy(target, m_target_frame->data[1], m_target_frame->linesize[1] * h / 2);
        }

        if (m_target_frame->data[2])
        {
            target += m_target_frame->linesize[1] * h / 2;
            memcpy(target, m_target_frame->data[2], m_target_frame->linesize[2] * h / 2);
        }
    }

    if (m_vbuf)
    {
        if (m_target_FrameSize > m_vsize)
        {
            m_vbuf  = (uint8_t *)realloc(m_vbuf, m_target_FrameSize);
            m_vsize = m_target_FrameSize;
        }

        uint8_t *yuv = m_vbuf;
        memcpy(yuv, m_target_frame->data[0], m_target_frame->linesize[0] * h);
        if (m_target_frame->data[1])
        {
            yuv += m_target_frame->linesize[0] * h;
            memcpy(yuv, m_target_frame->data[1], m_target_frame->linesize[1] * h / 2);
        }

        if (m_target_frame->data[2])
        {
            yuv += m_target_frame->linesize[1] * h / 2;
            memcpy(yuv, m_target_frame->data[2], m_target_frame->linesize[2] * h / 2);
        }

        notifyYUV(m_vbuf, m_target_FrameSize, w, h);
    }

    dsize = m_target_FrameSize;

    return 0;
}
int
FFmpeg_VideoDecode::convert(int sfmt, int dfmt, void *src, int ssize, void *dest, int &dsize, int &w, int &h)
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

   
    AVFrame  srcFrame;

    if(av_image_fill_arrays(srcFrame.data,srcFrame.linesize,(uint8_t *)src,spixfmt, w, h,1)<0)
    {
            ERRE_PLY("av_image_fill_arrays fail.");
        return -1;
    }
    if (FMT_YV12 == sfmt)
    {
        /* swap U and V */
        uint8_t * p = srcFrame.data[1];
        srcFrame.data[1] = srcFrame.data[2];
        srcFrame.data[2] = p;
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

   
    AVFrame  destFrame;

    if(av_image_fill_arrays(destFrame.data,destFrame.linesize,(uint8_t *)dest,dpixfmt,w,h,1)<0)
    {
        ERRE_PLY("av_image_fill_arrays fail.");
        return -1;
    }

    destFrame.data[0] = destFrame.data[0] + destFrame.linesize[0] * (h - 1);
    destFrame.data[1] = destFrame.data[1] + destFrame.linesize[0] * h / 4 - 1;
    destFrame.data[2] = destFrame.data[2] + destFrame.linesize[0] * h / 4 - 1;
    destFrame.linesize[0] *= -1;
    destFrame.linesize[1] *= -1;
    destFrame.linesize[2] *= -1;

    struct SwsContext* swsCtx = NULL;

    swsCtx = sws_getContext(w, h, spixfmt, w, h, dpixfmt, SWS_FAST_BILINEAR, 0, 0, 0);
    if (!swsCtx)
    {
        ERRE_PLY("sws_getContext fail.");
        return -1;
    }

    if (sws_scale(swsCtx, srcFrame.data, srcFrame.linesize, 0, h, destFrame.data, destFrame.linesize) < 0)
    {
        ERRE_PLY("sws_scale fail.");
        sws_freeContext(swsCtx);
        return -1;
    }

    sws_freeContext(swsCtx);

    return 0;
}
FFmpeg_AudioDecoder::FFmpeg_AudioDecoder()
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

FFmpeg_AudioDecoder::~FFmpeg_AudioDecoder()
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

int FFmpeg_AudioDecoder::init(int codec, int sps, int bps, int channel)
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


