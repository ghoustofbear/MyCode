/**
 * @file      recordstream.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-02 10:12:51
 *   
 * @note
 *  recordstream.h defines 
 */
#ifndef __RECORDSTREAM_H__
#define __RECORDSTREAM_H__
#include "def.h"
#include "tlist.h"
#include "thread.h"
#include "streamlistener.h"
#include "c2sdk.h"

#define MAX_FRAME_SIZE       (2*1024*1024) 

class Cu;
class Socket;

class RecordStream : public StreamListener
{
public:
    enum
    {
        S_UNINIT,
        S_STARTING,
        S_WAITING_LOGIN,
        S_CONNECTING,
        S_OPENING,
        S_RECVING,
        S_WATTING_CLOSE,
        S_CLOSED,
    };

    RecordStream(Cu *cu);
    virtual ~RecordStream();

    int                start(string json);

    int                status();

    Cu               * getCu();
    string           & getJson();

    long               getPosition();

public:
    virtual void       stop();
    virtual int        streamCtrl(int code, void *val, int valSize) = 0;

    virtual void       timerdriver();

    virtual int        setDecCallback(c2_decode_cb decodeCB, void *userData) { return 0; }

protected:
    virtual int        open()       = 0;
    virtual int        recv()       = 0;

    virtual int        pause()      = 0;
    virtual int        continuex()  = 0;
    virtual int        heartbeat()  = 0;

    virtual void       onReposEnd() = 0;

protected:
    void               recvLoop();

    static  void     * threadProc(ThreadInfo *);

protected:
    volatile int       m_status;
    Thread             m_recvThread;
    Socket           * m_sock;

    Cu               * m_cu;
    string             m_json;

    uint64_t           m_tmcnt;                            /**< timer driver count */

    char               m_frame[MAX_FRAME_SIZE];
    int                m_frameSize;

    int                m_streamPos;

    int                m_streamStatus; //0-uinit 1-play 2-pause

    c2_decode_cb       m_decodeCB;
    void             * m_decodeData;
    int                m_offset;
};

#endif
