#ifndef  _GWSDK_PREVIEW_H_
#define  _GWSDK_PREVIEW_H_

#include "rtptriggerlistener.h"
#include "c2play.h"
#include "gw_sipua.h"

class RTPSessionEx;

class preview : public RTPTriggerListener
{
public:

	friend class SoundTalk;

	preview(SipUA *pSipUa);

	~preview();

	bool  start(const char *cameracode, const char *host, int port, HWND hWnd);

	void  stop();

	virtual string     srcip()      { return m_pSipUa->ip(); }
	virtual int        srcport()    { return m_pSipUa->port(); }
	virtual string     destip()     { return ""; }
	virtual int        destport()   { return 0;  }

	virtual void       onHeartbeat();
	virtual void       onClose();

	string   camCode()   { return m_strCamCode; }

protected:
	static void        stream_cb(const uint8_t * data, uint32_t size, uint8_t type, void *puser);

public:
	SipUA * m_pSipUa;
	RTPSessionEx * m_rtpSession;
	int            m_iConId;
	int            m_iDialId;
	HANDLE         m_realHandle;
	long           m_seq;
	HWND           m_hWnd;
	int            m_width;
	int            m_height;
	string         m_strCamCode;
};

#endif
