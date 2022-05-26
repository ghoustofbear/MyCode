/**
 * @file      encoder_g711a.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-04-27 18:19:28
 *   
 * @note
 *  encoder_g711a.h defines 
 */
#ifndef __ENCODER_G711A_H__
#define __ENCODER_G711A_H__
#include "encoder.h"

class PLAY_API G711AEncoder : public AudioEncoder
{
public:
    G711AEncoder();
    virtual ~G711AEncoder();

public:
    virtual int        encode(void *src, int ssize, void *dest, int &dsize, uint64_t &pts);
};
#endif
