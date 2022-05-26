/**
 * @file      decoderlistener.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-09-14 10:07:38
 *
 * @note
 *  vdeclistener.h defines
 */
#ifndef __VDECLISTENER_H__
#define __VDECLISTENER_H__

class VDecListener
{
public:
    virtual void       onYUV(void *data, int size, int w, int h) = 0;
    virtual void       onRGB(void *data, int size, int w, int h) = 0;
};

#endif
