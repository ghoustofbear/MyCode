#include "rtspsession.h"
//#include "pumgr.h"
#include "putree.h"
#include "pu.h"
#include "socket.h"
#include "bufferev.h"
#include "frame.h"
#include "mempool.h"
#include "rtpsession.h"
#include "tls_error.h"

#define RECORD_BUFFER_MIN  (512*1024)

RTSPSession::RTSPSession()
    :RecordSession(NULL)
{
    char sSession[20] = { 0 };
    snprintf(sSession, 20, "%p", this);
    
    m_sessid = sSession;
    m_rtpSession = NULL;
    m_lastpts = sys->currentTimeMillis();

	m_strScale = "1";
	m_strRange = "";
	m_strUrl = "";
	m_bOpened = false;

    m_rtpThread = new Thread;
    m_rtpThread->setEntry(threadProc, this, T_PUSH_VIDEO);
    m_rtpThread->start();

    m_sendReqThread = new Thread;
    m_sendReqThread->setEntry(threadProc, this, T_REQUEST_ACTION);
    m_sendReqThread->start();

    m_obtainType = 1;
    m_streamProcoType = 1;
}

RTSPSession::~RTSPSession()
{ 
    ERRE_MSV("RTSPSession::~RTSPSession begin, this: %0x!!!", this);

    stop();

    if (NULL != m_rtpThread)
    {
        m_rtpThread->stop();
        delete m_rtpThread;
        m_rtpThread = NULL;
    }

    if (NULL != m_sendReqThread)
    {
        m_sendReqThread->stop();
        delete m_sendReqThread;
        m_sendReqThread = NULL;
    }

    if (m_rtpSession)
    {
        m_rtpSession->stop();
        delete m_rtpSession;
        m_rtpSession = NULL;
    }

    ERRE_MSV("RTSPSession::~RTSPSession ok, this: %0x!!!", this);
}

int
RTSPSession::start(string url)
{
	if (m_bOpened)
	{
		return 0;
	}

    do
    {
        char *host = (char *)url.c_str() + 7;
        char *p    = strchr(host, ':');
        if (!p)
        {
            ERRE_MSV("invalidate url: %s", url.c_str());
            break;
        }

        string puip;
        puip.assign(host, p - host);

        char *port = p + 1;
        p = strchr(port, '/');
        if (!p)
        {
            ERRE_MSV("invalidate url: %s", url.c_str());
            break;
        }

        string puport;
        puport.assign(port, p - port);

        Pu *pu = putree->findPu(puip, atoi(puport.c_str()));
        if (!pu)
        {
            LOGE_MSV(getError());
            break;
        }

        if (pu->addRecordGu(url, m_streamProcoType, this) < 0)
        {
            LOGE_MSV(getError());
            break;
        }

        m_time = sys->currentTime();

		m_bOpened = true;
        return 0;
    }
    while (0);

    return -1;
}

void
RTSPSession::stop()
{
    do
    {
        char *host = (char *)m_url.c_str() + 7;
        char *p    = strchr(host, ':');
        if (!p)
        {
            LOGW_MSV("invalidate m_url: %s", m_url.c_str());
            break;
        }

        string puip;
        puip.assign(host, p - host);

        char *port = p + 1;
        p = strchr(port, '/');
        if (!p)
        {
            LOGW_MSV("invalidate m_url: %s", m_url.c_str());
            break;
        }

        string puport;
        puport.assign(port, p - port);

        Pu *pu = putree->findPu(puip, atoi(puport.c_str()));
        if (!pu)
        {
            LOGW_MSV(getError());
            break;
        }

        ERRE_MSV("rtsp stop %s !!", puip.c_str());
        pu->delRecordGu(this);

		m_bOpened = false;
    }
    while (0);
}

int
RTSPSession::stream()
{
    return 0;
}

int
RTSPSession::transport()
{
    return 0;
}

string
RTSPSession::peerip()
{
    return "0.0.0.0";
}

int
RTSPSession::peerport()
{
    return 0;
}

string
RTSPSession::peertime()
{
    /* return m_time; */
    return "";
}

int
RTSPSession::streamtype()
{
    return 0;
}

void
RTSPSession::onAudioStream(void *stream, uint32_t size, int encode, int sps, int channels, int bps, uint64_t pts)
{
}

void
RTSPSession::onPrivStream(void *stream, uint32_t size, int encode, uint32_t tag)
{
}


void
RTSPSession::onWrite()
{
}

void
RTSPSession::onRead(Bufferev *bev, void *data, uint32_t size)
{
    LOGD_MSV((const char *)data);
    
    RtspReq reqRtsp;
    strncpy(reqRtsp.req, (char*)data, size);
    reqRtsp.size = size;

    AutoLock locker(m_sendReqList._mtx);
    m_sendReqList.push_back(reqRtsp);
}

void
RTSPSession::onClose()
{
    ERRE_MSV("RTSPSession::onClose this: %0x !!!", this);
    m_active = false;
}

void
RTSPSession::onStatus(int code)
{
    m_status = code;
}

int
RTSPSession::RTPSRCPort()
{
    if (!m_rtpSession)
    {
        m_rtpSession = new RTPSessionEx();
    }

    return m_rtpSession->srcport();
}

int
RTSPSession::RTPStart(string &url)
{
    if (!m_rtpSession)
    {
        m_rtpSession = new RTPSessionEx();
    }

    if(m_rtpSession->addDestination(destip(), m_destport, this) < 0)
        return -1;

#if 0
    if (start(url) < 0)
    {
        LOGE_MSV(getError());

        return -1;
    }
#endif

    m_url = url;

    LOGI_MSV("RTSPSession::status() !!!");

    switch (status())
    {
    case StreamListener::S_PU_OFFLINE:
        LOGE_MSV("status: S_PU_OFFLINE.");
        return -1;

    case StreamListener::S_GU_OFFLINE:
        LOGE_MSV("status: S_GU_OFFLINE.");
        return -1;

    case StreamListener::S_GU_CLOSED:
        LOGE_MSV("status: S_GU_CLOSED.");
        return -1;

    default:
        break;
    }

    LOGI_MSV("RTSPSession::RTPStart ok!!!");
    return 0;
}

#if 0
int
RTSPSession::RTPlay(string scale, string range)
{
    char *host = (char *)m_url.c_str() + 7;
    char *p    = strchr(host, ':');
    if (!p)
    {
        LOGW_MSV("invalidate m_url: %s", m_url.c_str());
        return -1;
    }

    string puip;
    puip.assign(host, p - host);

    char *port = p + 1;
    p = strchr(port, '/');
    if (!p)
    {
        LOGW_MSV("invalidate m_url: %s", m_url.c_str());
        return -1;
    }

    string puport;
    puport.assign(port, p - port);

    Pu *pu = putree->findPu(puip, atoi(puport.c_str()));
    if (!pu)
    {
        LOGW_MSV(getError());
        return -1;
    }

    return pu->playRecord(this, scale, range);
}
#else

int
RTSPSession::RTPlay(string scale, string range, string url)
{ 
	if ("" == url)
	{
		return -1;
	}

    if (start(url) < 0)
    {
        LOGE_MSV(getError());

        return -1;
    }

    char *host = (char *)m_url.c_str() + 7;
    char *p    = strchr(host, ':');
    if (!p)
    {
        LOGW_MSV("invalidate m_url: %s", m_url.c_str());
        return -1;
    }

    string puip;
    puip.assign(host, p - host);

    char *port = p + 1;
    p = strchr(port, '/');
    if (!p)
    {
        LOGW_MSV("invalidate m_url: %s", m_url.c_str());
        return -1;
    }

    string puport;
    puport.assign(port, p - port);

    Pu *pu = putree->findPu(puip, atoi(puport.c_str()));
    if (!pu)
    {
        LOGW_MSV(getError());
        return -1;
    }

    return pu->playRecord(this, scale, range);
}

#endif

int
RTSPSession::RTPause()
{
    char *host = (char *)m_url.c_str() + 7;
    char *p    = strchr(host, ':');
    if (!p)
    {
        LOGW_MSV("invalidate m_url: %s", m_url.c_str());
        return -1;
    }

    string puip;
    puip.assign(host, p - host);

    char *port = p + 1;
    p = strchr(port, '/');
    if (!p)
    {
        LOGW_MSV("invalidate m_url: %s", m_url.c_str());
        return -1;
    }

    string puport;
    puport.assign(port, p - port);

    Pu *pu = putree->findPu(puip, atoi(puport.c_str()));
    if (!pu)
    {
        LOGW_MSV(getError());
        return -1;
    }

    return pu->pauseRecord(this);
}

int
RTSPSession::RTPStop()
{
    stop();
    return 0;
}

string
RTSPSession::srcip()
{
    return m_bev->srcip();
}

int
RTSPSession::srcport()
{
    return RTPSRCPort();
}

string
RTSPSession::destip()
{
    if (m_bev)
    {
        return m_bev->peerip();
    }
    else
    {
        return "0.0.0.0";
    }
}

int
RTSPSession::destport()
{
    return m_destport;
}

void
RTSPSession::onHeartbeat()
{
    m_lastpts = sys->currentTimeMillis();
    LOGI_MSV("srcip: %s, srcport: %d, destip: %s, destport: %d", srcip().c_str(), srcport(), destip().c_str(), m_destport);
}

void
RTSPSession::timerdriver()
{
    uint64_t pts = sys->currentTimeMillis();

    if (pts - m_lastpts > 60 * 1000)
    {
        LOGW_MSV("rtp session timeout. srcip: %s, srcport: %d, destip: %s, destport: %d", srcip().c_str(), srcport(), destip().c_str(), m_destport);
        onClose();
    }
}


void * RTSPSession::threadProc(ThreadInfo *info)
{
    RTSPSession * rtsp = (RTSPSession *)(info->data);

    switch (info->id)
    {
    case T_PUSH_VIDEO:
        rtsp->onPushVideo();
        break;

    case T_REQUEST_ACTION:
        rtsp->doAction();
        break;

    default:
        break;
    }

    return NULL;
}

void RTSPSession::doAction()
{
    int sz = 0;
    while (m_sendReqThread->active())
    {
        {
            AutoLock lock(m_sendReqList._mtx);
            sz = m_sendReqList.size();
        }

        if (0 == sz)
        {
            sys->sleep(100);
            continue;
        }

        
        RtspReq msg;
        {         
            msg = *m_sendReqList.begin();
            m_sendReqList.pop_front();
        }

        stk::RtspMsg rtsp(msg.req, msg.size);
        string method = rtsp.GetMetthod();

        if ("OPTIONS" == method)
        {
            onOption(rtsp);
        }
        else if ("DESCRIBE" == method)
        {
            onDescribe(rtsp);
        }
        else if ("SETUP" == method)
        {
            onSetup(rtsp);
        }
        else if ("PLAY" == method)
        {
            onPlay(rtsp);
            m_sendStatus = true;
        }
        else if ("PAUSE" == method)
        {
            onPause(rtsp);
            m_sendStatus = false;
        }
        else if ("ANNOUNCE" == method)
        {
        }
        else if ("TEARDOWN" == method)
        {
            onTeardown(rtsp);
            break;
        }
        else if ("GET_PARAMETER" == method)
        {
            onGetParameter(rtsp);
        }

        sys->sleep(10);
    }
}

void RTSPSession::sendRecordData()
{
    AutoLock locker(m_wque._mtx);

    if (0 == m_wque.size() ||
        false == m_sendStatus)
    {
        return;
    }

    uint8_t *buf = m_wque.front();
    ut_frame_s *hdr = (ut_frame_s *)buf;

    m_bufferSize -= (hdr->size + sizeof(ut_frame_s));
    if (m_bufferSize <= RECORD_BUFFER_MIN)
    {
        m_buffStatus = S_BUFF_EMPTY;
    }

    if (m_rtpSession)
    {
        m_rtpSession->sendByFU(hdr->encode, hdr->type, hdr->buf, hdr->size, hdr->pts);
    }

    mempool->free(m_wque.front());
    m_wque.pop();
}

void RTSPSession::sendHeartbeat()
{
    AutoLock locker(m_wqueAlive._mtx);

    while (m_wqueAlive.size())
    {
        mempool->free(m_wqueAlive.front());
        m_wqueAlive.pop();
    }
}

void RTSPSession::onPushVideo()
{
    while (m_rtpThread->active())
    {
        sendHeartbeat();

		sendRecordData();

        if (m_sendStatus)
		{
		    if (S_BUFF_FULL == m_buffStatus)
		    {
			    RTPause();
		    }
		    else if (S_BUFF_EMPTY == m_buffStatus)
		    {
			    RTPlay(m_strScale, m_strRange, m_strUrl);
		    }
		}

        sys->sleep(1);
    }
}
