/**
 * @file      buffevlistener.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-05-11 14:09:35
 *
 * @note
 *  buffevlistener.h defines
 */
#ifndef __BUFFERLISTENER_H__
#define __BUFFERLISTENER_H__
#include "def.h"

class Bufferev;
class BuffevListener
{
public:
    virtual void       onRead(Bufferev *bev, void *data, uint32_t size) = 0;
    virtual void       onWrite(Bufferev *bev) {}
	virtual void       onClose(Bufferev *bev) = 0;
};
#endif
