/**
 * @file      g711decoder.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-09-11 10:37:54
 *
 * @note
 *  g711decoder.h defines
 */
#ifndef __DECODER_G711A_H__
#define __DECODER_G711A_H__

#include "decoder.h"
class G711ADecoder : public AudioDecoder
{
public:
    G711ADecoder();
    virtual ~G711ADecoder();

public:
    virtual int        decode(void *src, int ssize, void *dest, int &dsize);

private:
};

#endif
