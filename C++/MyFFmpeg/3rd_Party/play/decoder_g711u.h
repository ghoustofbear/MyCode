/**
 * @file      g711decoder.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-09-11 10:37:54
 *
 * @note
 *  decoder_g711u.h defines
 */
#ifndef __DECODER_G711U_H__
#define __DECODER_G711U_H__

#include "decoder.h"
class G711UDecoder : public AudioDecoder
{
public:
    G711UDecoder();
    virtual ~G711UDecoder();

public:
    virtual int        decode(void *src, int ssize, void *dest, int &dsize);

private:
};

#endif
