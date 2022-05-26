/*
 * @Author: liuxu
 * @Date: 2022-04-01 19:33:47
 * @LastEditors: liuxu
 * @LastEditTime: 2022-04-02 13:57:22
 * @FilePath: \C++\MyFFmpeg\src\MyFFmpeg\FFmpeg_Decode.h
 * @Description: 
 * 
 * Copyright (c) 2022 by liuxu/1056203382@qq.com, All Rights Reserved. 
 */


#ifndef __FFMPEG_DECODE_H
#define __FFMPEG_DECODE_H
extern "C" {
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
#include"libavutil/imgutils.h"
#include"libavformat/avformat.h"
};

#include "decoder.h"

class FFmpeg_VideoDecode:public VideoDecoder
{
private:
    
public:
    FFmpeg_VideoDecode();
    virtual ~FFmpeg_VideoDecode();
     

    virtual int        init(int codec, int avfmt, int width, int height, int fps);
    virtual int        decode(void *src, int ssize, void *dest, int &dsize, int &w, int &h, uint64_t &pts);
    virtual int        convert(int sfmt, int dfmt, void *src, int ssize, void *dest, int &dsize, int &w, int &h);
    
private:
    AVCodecContext   * m_codecCtx;
    SwsContext       * m_swsCtx;
    AVFrame          * m_yuv;
    AVFrame          *m_target_frame;
   
    int                m_target_FrameSize=0;
    AVPixelFormat      m_AVPixelFormat;
};

class FFmpeg_AudioDecoder : public AudioDecoder
{
public:
    FFmpeg_AudioDecoder();
    virtual ~FFmpeg_AudioDecoder();

public:
    virtual int        init(int codec, int sps, int bps, int channel);
    virtual int        decode(void *src, int ssize, void *dest, int &dsize);
    virtual int        resample(AVCodecContext* ctx, AVFrame *pcm, uint8_t** outBuf, int &outSize);

private:
    AVCodecContext   * m_codecCtx;
    AVFrame          * m_pcm;
    SwrContext       * m_actx;
    int                m_sps;
    int                m_bps;
    int                m_channel;
};

#endif