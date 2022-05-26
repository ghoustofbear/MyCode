/**
 * @file      encoder_x265.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-04-09 10:51:49
 *
 * @note
 *  encoder_x265.h defines
 */
#ifndef __ENCODER_X265_H__
#define __ENCODER_X265_H__

#include "c2play.h"
#include "encoder.h"

struct x265_param;
struct x265_encoder;
struct x265_picture;

class PLAY_API X265Encoder : public VideoEncoder
{
public:
    X265Encoder();
    virtual ~X265Encoder();

public:
    virtual int        init(int avfmt, int width, int height, int gop, int fps, int bitrate, int mode);
    virtual int        encode(void *src, int ssize, void *dest, int &dsize, uint64_t &pts);

private:
    x265_param       * m_param;
    x265_encoder     * m_enc;
    x265_picture     * m_pic;
};
#endif
