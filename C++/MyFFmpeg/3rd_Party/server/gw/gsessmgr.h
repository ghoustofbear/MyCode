/**
 * @file      gsessmgr.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-12-20 14:13:00
 *
 * @note
 *  gsessmgr.h defines
 */
#ifndef __GSESSMGR_H__
#define __GSESSMGR_H__

#include "tmap.h"

class Thread;
class ThreadInfo;
class GSession;


const string &GetMediaStream();

const string &GetSipProtocol();

const string &GetCatalogType();

class GSessMgr : public tmap<string, GSession *>
{
public:

	enum
	{
		T_TIMER,
	};

	enum
	{
		S_SESS_NORMAL,
		S_SESS_CLOSED,
	};

    GSessMgr();
    virtual ~GSessMgr();

public:
    int                RTPSRCPort(string &sessid);
    int                RTPSRCPort(string &sessid, uint64_t startTime, uint64_t endTime, int playbackmode);


    int                RTPStart(string &sessid, string &callid, string &json);
    int                RTPStop (string &callid);
    int                RTPPlay(string &callid, string scale, string range);
    int                RTPPause(string &callid);
    int                setSSRC(string &sessid, uint32_t ssrc);
    

	int                addSession(string &sessid, string &callid, string &json);
    int                delSession(string &sessid);

    void               timerdriver();

	typedef void(*pfnstreamcallback)(const uint8_t * data, uint32_t size, uint8_t type, void *puser);
	void               setstreamcallback(string &sessid, pfnstreamcallback fnstreamcb, void *puser);

	void               doSendAudio(string sessid, const uint8_t * data, uint32_t size);

private:
    void               InitMediaStream();
protected:
    int                addSession(string &sessid);
    int                addSession(string &sessid, uint64_t startTime, uint64_t endTime, int);


	static void *      threadProc(ThreadInfo *info);
private:
	Thread           * m_timerThread;
};

extern GSessMgr * gsessMgr;

#endif