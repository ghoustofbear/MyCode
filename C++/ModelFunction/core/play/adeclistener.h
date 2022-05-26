/**
 * @file      decoderlistener.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-09-14 10:07:38
 *
 * @note
 *  adeclistener.h defines
 */
#ifndef __ADECLISTENER_H__
#define __ADECLISTENER_H__

class ADecListener
{
public:
    virtual void       onADPCM(void *data, int size) = 0;
};

#endif
