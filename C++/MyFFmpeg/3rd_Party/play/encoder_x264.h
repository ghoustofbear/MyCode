/**
 * @file      encoder_x264.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-04-09 10:51:49
 *
 * @note
 *  encoder_x264.h defines
 */
#ifndef __ENCODER_X264_H__
#define __ENCODER_X264_H__

#include "encoder.h"

struct x264_t;
struct x264_param_t;
struct x264_picture_t;

class PLAY_API X264Encoder : public VideoEncoder
{
public:
    X264Encoder();
    virtual ~X264Encoder();

public:
    virtual int        init(int avfmt, int width, int height, int gop, int fps, int bitrate, int mode);
    virtual int        encode(void *src, int ssize, void *dest, int &dsize, uint64_t &pts);

private:
    x264_t             * m_enc;
    x264_param_t       * m_param;
    x264_picture_t     * m_pic;

	uint32_t             m_seq;
};
#endif
