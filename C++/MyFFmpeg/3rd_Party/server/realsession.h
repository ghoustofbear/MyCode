/**
 * @file      ssession.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-06-28 16:42:26
 *   
 * @note
 *  ssession.h defines 
 */
#ifndef __SSESSION_H__
#define __SSESSION_H__
#include "session.h"
#include "tqueue.h"

class Pu;
class Client;
class RealSession: public Session
{
public:
    RealSession(Client *client);
    virtual ~RealSession();

    virtual int        start(string json, int mode=0);
    virtual void       stop();

    virtual int        stream();
    virtual int        transport();
    virtual string     peerip();
    virtual int        peerport();
    virtual string     peertime();
    virtual int        streamtype();

protected:
    virtual void       onVideoStream(void *stream, uint32_t size, int encode, int type, int w, int h, int fps, uint64_t pts, uint32_t tag);
    virtual void       onAudioStream(void *stream, uint32_t size, int encode, int sps, int channels, int bps, uint64_t pts);
    virtual void       onPrivStream(void *stream, uint32_t size, int encode, uint32_t tag, int type = 'I');

    virtual void       onStatus(int code);

    virtual void       onRead(Bufferev *bev, void *data, uint32_t size);
    virtual void       onClose(Bufferev *bev);

protected:
    vector<uint8_t>    m_recvBuf;

private:
    Client           * m_client;
    Pu               * m_pu;
    Su               * m_su;
    int                m_streamType;
    int                m_transport;
    int                m_streamProtoType;

    string             m_deviceHost;
    int                m_devicePort;
    string             m_subServHost;
    int                m_subServPort;

    string             m_addrcode;
    uint32_t           m_seq;


    unsigned char      m_pfrbuf[1024 * 1024 * 2];
    unsigned char      m_sfrbuf[1024 * 1024 * 2];
};
#endif
