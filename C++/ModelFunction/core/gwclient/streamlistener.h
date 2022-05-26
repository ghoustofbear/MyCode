/**
 * @file      streamlistener.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-11-14 09:19:04
 *   
 * @note
 *  streamlistener.h defines 
 */
#ifndef __STREAMLISTENER_H__
#define __STREAMLISTENER_H__
#include "def.h"

class StreamListener
{
public:
    enum
    {
        S_SOK,
        S_PU_OFFLINE,
        S_GU_OFFLINE,
		S_GU_REONLINE,
        S_GU_CLOSED,
    };

    enum
    {
        S_BUFF_FULL,
        S_BUFF_EMPTY,
        S_BUFF_NONE,
    };

public:
    StreamListener() : m_buffStatus(S_BUFF_NONE){}
    virtual void       onVideoStream(void *stream, uint32_t size, int encode, int type, int w, int h, int fps, uint64_t pts) = 0;
    virtual void       onAudioStream(void *stream, uint32_t size, int encode, int sps,  int channels, int bps, uint64_t pts) = 0;
    virtual void       onPrivStream (void *stream, uint32_t size, int encode, uint32_t tag) = 0;
    virtual void       onStatus(int code) = 0;
    /**
    *判断buffer的状态，当满的时候需要暂停送流，
    *防止因缓冲区满而导致丢帧
    */
    int                onBuffStatus() { return m_buffStatus; };

    virtual int        stream()     = 0;                   /* 0:private, 1:standard */
    virtual int        transport()  = 0;
    virtual string     peerip()     = 0;
    virtual int        peerport()   = 0;
    virtual string     peertime()   = 0;
    virtual int        streamtype() = 0;                   /* 0:main stream, 1:sub stream */

    int                m_buffStatus;                       /* buff status, see enum */
};
#endif
