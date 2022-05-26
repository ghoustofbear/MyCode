/**
* @file      ssession.h
* @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
* @brief     brief
* @author    wuxuanren@UT.com
* @date      2022-02-08 16:42:26
*
* @note
*  ssession.h defines
*/
#ifndef __CaptaureSession_H__
#define __CaptaureSession_H__
#include "session.h"
#include "3rd/hik/sdk-plat.h"
#include "tevqueue.h"
class Pu;
class Client;
class CaptaureSession : public StreamListener
{
public:
	CaptaureSession();
	virtual ~CaptaureSession();

	virtual int        start(string json, int mode = 0);
	virtual void       stop() ;

	virtual int        stream() ;                   /* 0:private, 1:standard */
	virtual int        transport() ;
	virtual string     peerip() ;
	virtual int        peerport() ;
	virtual string     peertime() ;
	virtual int        streamtype() ;                   /* 0:main stream, 1:sub stream */

protected:
	virtual void       onVideoStream(void *stream, uint32_t size, int encode, int type, int w, int h, int fps, uint64_t pts, uint32_t tag);
	virtual void       onAudioStream(void *stream, uint32_t size, int encode, int sps, int channels, int bps, uint64_t pts) ;
	virtual void       onPrivStream(void *stream, uint32_t size, int encode, uint32_t tag, int type = 'I') ;

	virtual void       onStatus(int code) ;
	int GetSize(char * buffer, long length, int &height, int &weidth);

private:
	
	Pu               * m_pu;
	long              m_playID;
	string            m_deviceHost;
	int               m_devicePort;
	bool              m_captaureflag;
	int               m_streamType;
public:
	uint8_t			 *m_pCapBuf;
	DWORD            dwCapSize;
	DWORD			 bufSize;
	int              m_height;
	int              m_weidth;
	int             m_channelNo;
	tevent<string, long> m_evtCaptaurepicture;

};
#endif