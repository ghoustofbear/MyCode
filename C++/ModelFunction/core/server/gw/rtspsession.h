/**
 * @file      grsession.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-12-28 08:47:52
 *   
 * @note
 *  grsession.h defines 
 */
#ifndef __GRSESSION_H__
#define __GRSESSION_H__
#include "recordsession.h"
#include "rtptriggerlistener.h"
#include "stk/stk_message_rtsp.h"
#include "thread.h"
#include "bufferev.h"
#include "tlist.h"

class RTPSessionEx;
class RTSPSession: public RecordSession, public RTPTriggerListener
{
public:
	friend class RTSPServer;

    enum 
    {
        T_PUSH_VIDEO,
        T_REQUEST_ACTION,
    };

    RTSPSession();
    virtual ~RTSPSession();

public:
    int                RTPSRCPort();
    int                RTPStart(string &url);
    int                RTPlay(string scale, string range, string url);
    int                RTPause();
    int                RTPStop();
	void               setbev(Bufferev *bev) { m_bev = bev; }
public:
    virtual int        start(string url);
    virtual void       stop();

    virtual int        stream();
    virtual int        transport();
    virtual string     peerip();                           /* rtsp peer ip */
    virtual int        peerport();                         /* rtsp peer port */
    virtual string     peertime();
    virtual int        streamtype();

public:
    virtual string     srcip();                            /* rtp source ip */
    virtual int        srcport();                          /* rtp source base port */
    virtual string     destip();                           /* rtp destination ip */
    virtual int        destport();                         /* rtp destination base port */

    virtual void       onHeartbeat();

protected:
    virtual void       onOption  (stk::RtspMsg &rtsp) = 0;
    virtual void       onDescribe(stk::RtspMsg &rtsp) = 0;
    virtual void       onSetup   (stk::RtspMsg &rtsp) = 0;
    virtual void       onPlay    (stk::RtspMsg &rtsp) = 0;
    virtual void       onPause   (stk::RtspMsg &rtsp) = 0;
    virtual void       onTeardown(stk::RtspMsg &rtsp) = 0;
    virtual void       onGetParameter(stk::RtspMsg &rtsp) = 0;

protected:
    virtual void       onAudioStream(void *stream, uint32_t size, int encode, int sps, int channels, int bps, uint64_t pts);
    virtual void       onPrivStream (void *stream, uint32_t size, int encode, uint32_t tag);
    virtual void       onStatus(int code);

    void               sendRecordData();
    void               sendHeartbeat();

protected:
    virtual void       onRead(Bufferev *bev, void *data, uint32_t size);
    virtual void       onWrite();
    virtual void       timerdriver();

    void               onPushVideo();
    void               doAction();
    static void *      threadProc(ThreadInfo *info);

public:
    virtual void       onClose();                          /* Bufferev and RTPTriggerListener both */

protected:
    RTPSessionEx        * m_rtpSession;
    string                m_url;
    int                   m_destport;
    uint64_t              m_lastpts;

    Thread              * m_rtpThread;
    int                   m_streamProcoType;
	Bufferev            * m_bev;

    Thread              * m_sendReqThread;

	tqueue<uint8_t *>  m_wque;
	tqueue<uint8_t *>  m_wqueAlive;
	bool               m_sendStatus;
	long               m_bufferSize;

    typedef struct RtspReq
    {
        char  req[1024];
        int   size;
    }RtspReq;

    tlist<RtspReq>   m_sendReqList;

	string           m_strScale;
	string           m_strRange;
	string           m_strUrl;
	bool             m_bOpened;
};
#endif
