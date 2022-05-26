#ifndef  _GWSDK_PLAYBACK_H_
#define  _GWSDK_PLAYBACK_H_

#include "rtptriggerlistener.h"
#include "c2play.h"
#include "rtspclient.h"
#include "gw_sipua.h"

class RTPSessionEx;
class Playback : public RTPTriggerListener
{
public:
	Playback(SipUA *pSipUa);

	~Playback();

	bool start(const char *url, HWND hWnd);

	void stop();

	virtual string     srcip()      { return m_pSipUa->ip(); }
	virtual int        srcport()    { return m_pSipUa->port(); }
	virtual string     destip()     { return ""; }
	virtual int        destport()   { return 0; }

	virtual void       onHeartbeat();
	virtual void       onClose();

	void               playCtrl(int cmd);

protected:
	static void        stream_cb(const uint8_t * data, uint32_t size, uint8_t type, void *puser);
	static void        status_cb(int code, void* user_data);
	static void        bufferwillfull(int type, int fcnt, void* userdata);
	static void        bufferwillemtpy(int type, int fcnt, void* userdata);

public:
	SipUA * m_pSipUa;
	RtspClient * m_rtspClient;
	HWND      m_hWnd;
	RTPSessionEx * m_rtpSession;
	HANDLE         m_recordHandle;
	long           m_seq;
	int            m_width;
	int            m_height;
	long           m_firstpts;
	int            m_speed;
};

#endif
