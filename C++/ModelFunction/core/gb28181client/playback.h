#ifndef  _GB28181_SDK_PLAYBACK_H_
#define  _GB28181_SDK_PLAYBACK_H_

#include "gb28181_sipua.h"
#include "rtptriggerlistener.h"
#include "c2play.h"
#include "thread.h"

class RTPSessionEx;
class Playback : public RTPTriggerListener
{
public:
    Playback(GB28181_SipUA *pSipUa);

    ~Playback();

    bool start(const char *cameracode, const char *host, int port, uint32_t start, uint32_t end, HWND hWnd);

    void stop();

    virtual string     srcip()      { return m_pSipUa->ip(); }
    virtual int        srcport()    { return m_pSipUa->port(); }
    virtual string     destip()     { return ""; }
    virtual int        destport()   { return 0; }

    virtual void       onHeartbeat();
    virtual void       onClose();

    void               playCtrl(int cmd, int param);
    void               streamCtrl(int cmd, int param);
protected:
    void               timerdriver();
    static void      * threadProc(ThreadInfo *);
    static void        stream_cb(const uint8_t * data, uint32_t size, uint8_t type, void *puser);
    static void        status_cb(int code, void* user_data);
    void               bufferwillfull(int type, int fcnt);
    void               bufferwillemtpy(int type, int fcnt);

public:
    GB28181_SipUA * m_pSipUa;
    HWND            m_hWnd;
    RTPSessionEx *  m_rtpSession;
    HANDLE          m_recordHandle;

    char          * m_frameBuf;

    int            m_iConId;
    int            m_iDialId;
    int            m_cseq;

    string         m_strCamCode;
    string         m_host;
    int            m_port;
    long            m_seq;


    int             m_playState;   // 0-uninit 1-playing 2-pause 3-fast 4-slow
    int             m_streamState; // 0-uninit 1-playing 2-pause 3-fast 4-slow
    int             m_speed;

    Thread          m_thread;
};

#endif
