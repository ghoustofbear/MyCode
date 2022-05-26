/**
 * @file      recordsession.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-02 11:50:53
 *   
 * @note
 *  dualtalksession.h defines 
 */
#ifndef __DUALTALKSESSION_H__
#define __DUALTALKSESSION_H__
#include "session.h"
#include "tqueue.h"
#include "tmap.h"
#include "thread.h"


class Client;
class Bufferev;
class Su;
class Pu;

class DualtalkSession: public DSession
{
public:
	DualtalkSession(Client *client, Bufferev *bev);
    virtual ~DualtalkSession();

    virtual int        start(string json);
    virtual void       stop();

protected:
    virtual void       onRead(Bufferev *bev, void *data, uint32_t size);
    virtual void       onWrite(Bufferev *bev);
    virtual void       onClose(Bufferev *bev);

    virtual void       onVideoStream(void *stream, uint32_t size, int encode, int type, int w, int h, int fps, uint64_t pts, uint32_t tag) {}
    virtual void       onAudioStream(void *stream, uint32_t size, int encode, int sps, int channels, int bps, uint64_t pts);
    virtual void       onPrivStream(void *stream, uint32_t size, int encode, uint32_t tag, int type = 'I') {}
    virtual void       onStatus(int code) {}

    virtual int        stream(){ return 0;}                   /* 0:private, 1:standard */
    virtual int        transport(){ return 0; }
    virtual string     peerip() { return ""; }
    virtual int        peerport() { return 0; }
    virtual string     peertime() { return ""; }
    virtual int        streamtype() { return 0; }

    int                addSu(string host, int port, int chn);
    int                addSu(string addrcode);

	static void      * threadEntry(ThreadInfo *arg);
private:
    Client           * m_client;
    tqueue<uint8_t *>  m_wque;
    tmap<Pu*, Su*>     m_sulist;
    Thread             m_alive;
    bool               m_bConnect;

    char*              m_tmpAudioBuf;
    int                m_tmpAudioBufUsed;
    uint64_t           m_last_send;
};
#endif
