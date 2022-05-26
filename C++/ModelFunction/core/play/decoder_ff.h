/**
 * @file      ffdecoder.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-09-11 10:37:54
 *
 * @note
 *  ffdecoder.h defines
 */
#ifndef __FFDECODER_H__
#define __FFDECODER_H__
extern "C" {
#include "libavcodec/avcodec.h"
#include "libswscale/swscale.h"
#include "libswresample/swresample.h"
};

#include "decoder.h"

class FFVideoDecoder : public VideoDecoder
{
public:
    FFVideoDecoder();
    virtual ~FFVideoDecoder();

    virtual int        init(int codec, int avfmt, int width, int height, int fps);
    virtual int        decode(void *src, int ssize, void *dest, int &dsize, int &w, int &h, uint64_t &pts);
    virtual int        convert(int sfmt, int dfmt, void *src, int ssize, void *dest, int &dsize, int &w, int &h);
private:
    AVCodecContext   * m_codecCtx;
    SwsContext       * m_swsCtx;
    AVFrame          * m_yuv;
    AVPicture          m_swsPict;
    int                m_avSize;
    AVPixelFormat      m_avFormat;
};


class FFAudioDecoder : public AudioDecoder
{
public:
    FFAudioDecoder();
    virtual ~FFAudioDecoder();

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
