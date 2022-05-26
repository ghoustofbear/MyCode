#ifndef  _GB28181_SDK_DOWNLOAD_H_
#define  _GB28181_SDK_DOWNLOAD_H_

#include "gb28181_sipua.h"
#include "rtptriggerlistener.h"
#include "c2play.h"

class RTPSessionEx;
class Download : public RTPTriggerListener
{
public:
    Download(GB28181_SipUA *pSipUa);
    ~Download();

    bool start(const char *cameracode, const char *host, int port, const char *savefile, const char *recordfile, uint32_t start, uint32_t end, uint64_t fileSize = 0);
    void stop();

    virtual string     srcip()      { return m_pSipUa->ip(); }
    virtual int        srcport()    { return m_pSipUa->port(); }
    virtual string     destip()     { return ""; }
    virtual int        destport()   { return 0; }

    virtual void       onHeartbeat();
    virtual void       onClose();

    void               playCtrl(int cmd, int param);
    int                downloadPos();
protected:
    static void        stream_cb(const uint8_t * data, uint32_t size, uint8_t type, void *puser);
    static void        status_cb(int code, void* user_data);


public:
    GB28181_SipUA * m_pSipUa;
    RTPSessionEx *  m_rtpSession;
    HANDLE          m_recordHandle;
    FILE*           m_file;

    int             m_iConId;
    int             m_iDialId;
    int             m_cseq;
    int             m_ssrc;

    string          m_strCamCode;
    string          m_host;
    int             m_port;
    uint64_t        m_fileSize;
    uint64_t        m_offset;
};

#endif
