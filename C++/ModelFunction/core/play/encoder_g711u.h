/**
 * @file      encoder_g711u.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-04-27 17:50:27
 *
 * @note
 *  encoder_g711u.h defines
 */
#ifndef __ENCODER_G711U_H__
#define __ENCODER_G711U_H__
#include "encoder.h"

class PLAY_API G711UEncoder : public AudioEncoder
{
public:
    G711UEncoder();
    virtual ~G711UEncoder();

public:
    virtual int        encode(void *src, int ssize, void *dest, int &dsize, uint64_t &pts);
};
#endif
