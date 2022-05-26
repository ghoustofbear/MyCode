/**
 * @file      decoder.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-09-11 10:13:20
 *
 * @note
 *  decoder.h defines
 */
#ifndef __DECODER_H__
#define __DECODER_H__
#include "tlist.h"

class VDecListener;
class ADecListener;

class VideoDecoder : public tlist<VDecListener *>
{
public:
    enum
    {
        T_FFMPEG,
        T_AVC,
    };

    enum
    {
        CODEC_H264,
        CODEC_HEVC,
        CODEC_MPEG4,
        CODEC_MJPEG,
        CODEC_MJPEGB,
    };

    enum
    {
        FMT_YUV420P,
        FMT_YV12,
        FMT_RGB24,
        FMT_BGR24,
        FMT_RGB565,
        FMT_BGR565,
    };

    VideoDecoder();
    virtual ~VideoDecoder();

    int                codec();
    int                type();

    void               addDecoderLisntener(VDecListener *listener);
    void               delDecoderLisntener(VDecListener *listener);

    void               notifyYUV(void *data, int size, int w, int h);
    void               notifyRGB(void *data, int size, int w, int h);

    virtual int        init(int codec, int avfmt, int width, int height, int fps);
    virtual int        decode(void *src, int ssize, void *dest, int &dsize, int &w, int &h, uint64_t &pts) = 0;
    virtual int        convert(int sfmt, int dfmt, void *src, int ssize, void *dest, int &dsize, int &w, int &h) = 0;

protected:
    int                m_type;
    int                m_codec;
    int                m_avfmt;                            /**< deocde output video format */

    int                m_width;
    int                m_height;
    int                m_fps;

    uint8_t          * m_vbuf;
    int                m_vsize;
};

class AudioDecoder : public tlist<ADecListener *>
{
public:
    enum
    {
        CODEC_G711A,
        CODEC_G711U,
        CODEC_G722,
        CODEC_G726,
        CODEC_G729,
    };

public:
    AudioDecoder();
    virtual ~AudioDecoder();

public:
    int                codec();
    void               addDecoderLisntener(ADecListener *listener);
    void               delDecoderLisntener(ADecListener *listener);

    void               notifyADPCM(void *data, int size);

    virtual int        init(int codec, int sps, int bps, int channel) { return 0; }
    virtual int        decode(void *src, int ssize, void *dest, int &dsize) = 0;

private:
    uint8_t          * m_abuf;
    int                m_asize;

protected:
    int                m_codec;
};

#endif
