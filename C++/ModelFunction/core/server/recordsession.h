/**
 * @file      recordsession.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-02 11:50:53
 *   
 * @note
 *  recordsession.h defines 
 */
#ifndef __RECORDSESSION_H__
#define __RECORDSESSION_H__
#include "session.h"
#include "tqueue.h"
#include "concurrency.h"
#include "thread.h"

class Pu;
class Client;
class Bufferev;
class RecordSession: public Session
{
public:   
    RecordSession(Client *client);
    virtual ~RecordSession();

    virtual int        start(string json, int mode);
    virtual void       stop();

    virtual int        stream();
    virtual int        transport();
    virtual string     peerip();
    virtual int        peerport();
    virtual string     peertime();
    virtual int        streamtype();
	virtual void       setPausestatus(bool status);
	virtual int        getspeed();
	virtual void       setspeed(int speed);


protected:
    virtual void       onVideoStream(void *stream, uint32_t size, int encode, int type, int w, int h, int fps, uint64_t pts, uint32_t tag);
    virtual void       onAudioStream(void *stream, uint32_t size, int encode, int sps, int channels, int bps, uint64_t pts);
    virtual void       onPrivStream (void *stream, uint32_t size, int encode, uint32_t tag, int type = 'I');
    virtual void       onStatus(int code);
    
    virtual void       onRead(Bufferev *bev, void *data, uint32_t size);
    virtual void       onClose(Bufferev *bev);

	void               sendRecordData();
	void               sendHeartbeat();
    static void *      threadProc(ThreadInfo *info);

	void               clearData();
	int                getframeinterval();
	int                getMaxframecnt();
	int                getMinframecnt();
	int                getframecntpresec();

protected:      
    //long               m_bufferSize;
    //bool               m_sendStatus;
	vector<uint8_t>    m_recvBuf;


    string             m_deviceHost;
    int                m_devicePort;
    string             m_addrcode;


    Thread             m_sendThread;
    Pu*                m_pu;
private:
    Client           * m_client;
    string             m_filename;
    uint32_t           m_seq;
	int                m_rspeed;   //-4:1/16  -3:1/8 -2:1/4 -1:1/2 0:1 1:2 2:4 3:8 4:16
	bool               m_rpause;   //pause:true   continue:false
	int                m_videoframecnt;  //
	
	uint64_t           m_lastSendpts;
	tqueue<uint8_t *>  m_wque;
    
};
#endif
