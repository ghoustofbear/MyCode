/**
 * @file      session.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-06-28 15:49:51
 *   
 * @note
 *  session.h defines 
 */
#ifndef __SESSION_H__
#define __SESSION_H__
#include "streamlistener.h"
#include "buffevlistener.h"
#include "bufferev.h"
#include "concurrency.h"
#include "tlist.h"

class Su;

class Session: public StreamListener, public BuffevListener
{
public:

    Session();
    virtual ~Session();

    virtual int        start(string json, int mode =0) = 0;
    virtual void       stop() = 0;

    int                status();
    void               setbev(Bufferev *bev);
	bool                active() { return m_active && m_bev->active(); }

protected:
    virtual void       onDestory(Bufferev *bev);

    virtual void       onResponsePacket(Bufferev *bev, uint8_t * data, uint32_t inlen);

protected:
    volatile int       m_status;
    volatile bool      m_active;
    string             m_sessid;

    int                m_channelNo;
    int                m_obtainType;
    string             m_time;

    Bufferev         * m_bev;
    Mutex              m_bevMtx;
};


class DSession : public tlist<Su *>, public BuffevListener
{
public:
    enum
    {
        SS_UNINIT,
        SS_STARTING,
        SS_WAITING_LOGIN,
        SS_CONNECTING,
        SS_OPENING,
        SS_RECVING,
        SS_WATTING_CLOSE,
        SS_CLOSED,
    };

public:

    DSession();
    virtual ~DSession();

    virtual int        start(string json) = 0;
    virtual void       stop() = 0;

    void               addStreamSpeaker(Su* speaker);
    void               delStreamSpeaker(Su* speaker);

    int                status();
    void               setbev(Bufferev *bev);

protected:
    void               notifyVideoStream(void *stream, uint32_t size);
    void               notifyAudioStream(void *stream, uint32_t size);
    void               notifyStatus(int code);

    virtual void       onDestory(Bufferev *bev);
    virtual void       onResponsePacket(Bufferev *bev, uint8_t * data, uint32_t inlen);

protected:
    volatile int       m_status;
    volatile bool      m_active;
    string             m_sessid;

    int                m_userId;
    
    string             m_time;

    Bufferev         * m_bev;
    Mutex              m_bevMtx;
};
#endif
