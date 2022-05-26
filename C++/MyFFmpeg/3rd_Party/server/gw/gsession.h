/**
 * @file      gsession.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     guo wang session
 * @author    caiwang213@qq.com
 * @date      2017-06-29 15:35:46
 *
 * @note
 *  gsession.h defines
 */
#ifndef __GSESSION_H__
#define __GSESSION_H__
#include "session.h"
#include "tmap.h"

class RTPSessionEx;
class SipCall;
class Su;
class Pu;

class GSession: public Session, public tmap<string, SipCall *>
{
public:

	friend class GSessMgr;
	friend class GwSu;

	enum
	{
		S_SESS_NORMAL,
		S_SESS_CLOSED,
	};

	friend void       sessdatacallback(const uint8_t * data, uint32_t size, uint8_t type, void *puser);

    GSession(string sessid);
    GSession(string sessid, uint64_t startTime, uint64_t endTime, int);


    virtual ~GSession();

public:
    int                RTPSRCPort();

    int                RTPStart(string &callid, string &json);
    int                RTPStop (string &callid);
    int                RTPPlay(string &callid, string scale, string range);
    int                RTPPause(string &callid);

    int                setSSRC(uint32_t ssrc);

	int               addDestination(string &callid, string &json);

    bool               hasCall(string &callid);

    virtual int        start(string json, int mode=0);
    virtual void       stop();

    void               timerdriver();
public:
    virtual int        stream();                           /* 0:private, 1:standard */
    virtual int        transport();
    virtual string     peerip();
    virtual int        peerport();
    virtual string     peertime();
    virtual int        streamtype();                       /* 0:main stream, 1:sub stream */

	void               setstreamcallback(void *pfnstreamcallback, void *puser);
	int                status()  { return m_status; }

protected:
	virtual void       onVideoStream(void *stream, uint32_t size, int encode, int type, int w, int h, int fps, uint64_t pts, uint32_t tag);
    virtual void       onAudioStream(void *stream, uint32_t size, int encode, int sps, int channels, int bps, uint64_t pts);
	virtual void       onPrivStream(void *stream, uint32_t size, int encode, uint32_t tag, int type = 'I');
    virtual void       onStatus(int code);


    virtual void       onRead(Bufferev *bev, void *data, uint32_t size){};
    virtual void       onWrite(Bufferev *bev){};
    virtual void       onClose(Bufferev *bev){};

protected:
    SipCall          * addSipCall(string &callid, string &fromn, string &fromh, int fromp, string &ton, string &toh, int top);
    int                delSipCall(string &callid);

private:
    string             m_addrcode;
    int                m_streamtype;
    int                m_streamProtoType;           //0:private stream  1:standard stream

    RTPSessionEx     * m_rtpSession;
    volatile int       m_status;
	string             m_vcmode;                    //mainstream,substream,dualtalk,broadcast 
	Su               * m_su;
    Pu               * m_pu;


    int                m_playmode; //0:preview  1:playback
    int                m_ssrc;
    uint64_t           m_startTime;
    uint64_t           m_endTime;
};

#endif
