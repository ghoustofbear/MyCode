#ifndef  _GWSDK_PREVIEW_H_
#define  _GWSDK_PREVIEW_H_

#include "gb28181_sipua.h"
#include "c2play.h"
#include "baseplay.h"

class RTPSessionEx;

class preview : public BasePlay
{
public:

	friend class SoundTalk;
#ifdef _WIN32
    friend void CALLBACK DrawFunCB(long nPort, HDC hDc, LONG nUser);
#endif
    preview(GB28181_SipUA *pSipUa);

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

    virtual void changeWinSize();
    void            DrawRect(int x1, int y1, int x2, int y2, bool enable);

    int             DragZoom(int startX, int startY, int endX, int endY, int winW, int winH);

protected:
	static void        stream_cb(const uint8_t * data, uint32_t size, uint8_t type, void *puser);
    void            DrawRectCb(long nPort, HDC hDc);

public:
    GB28181_SipUA * m_pSipUa;
	RTPSessionEx * m_rtpSession;
	int            m_iConId;
    int            m_iDialId;
	long           m_seq;
	HWND           m_hWnd;
	int            m_width;
	int            m_height;
    string         m_strCamCode;
    long			m_nPort;
    string          m_host;
    int             m_port;
    int             m_rectX1;
    int             m_rectY1;
    int             m_rectX2;
    int             m_rectY2;
    bool            m_rectEnable;
    Mutex           m_muxRect;
    HANDLE          m_playHandle;
    char          * m_frameBuf;
};

#endif
