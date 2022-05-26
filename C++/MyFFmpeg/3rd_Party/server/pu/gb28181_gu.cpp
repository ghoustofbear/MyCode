#include "gb28181_gu.h"
#include "gb28181_pu.h"
#include "sipua.h"
#include "tls_error.h"
#include "session.h"
#include "frame.h"
#include "gumgr.h"
#include "gsessmgr.h"
#include "system.h"
#include "thread.h"
#include "gb28181_sipua.h"
#include "rtpsession.h"

#define  GW_SDP_FORMAT \
        "v=0\r\n" \
        "o=- 0 0 IN IP4 %s\r\n" \
        "s=-\r\n" \
        "c=IN IP4 %s\r\n" \
        "t=0 0\r\n" \
        "m=video %d RTP/AVP 100\r\n" \
        "a=rtpmap:100 H264/90000\r\n" \
        "a=fmtp:100 CIF=1;4CIF=1;F=1;K=1\r\n" \
        "a=sendrecv\r\n"


#define  GB28181_VIDEO_SDP_FORMAT \
    "v=0\r\n" \
    "o=%s 0 0 IN IP4 %s\r\n" \
    "s=Play\r\n" \
    "c=IN IP4 %s\r\n" \
    "t=0 0\r\n" \
    "m=video %d RTP/AVP 96 98\r\n" \
    "a=rtpmap:96 PS/90000\r\n" \
    "a=rtpmap:98 H264/90000\r\n" \
    "a=rtpmap:97 MPEG4/90000\r\n" \
    "y=%s\r\n" \
    "f=-\r\n\r\n"

void
Gb28181bRealGu::status_cb(int code, void* userData)
{

}


Gb28181bRealGu::Gb28181bRealGu(Pu *pu, string addrcode, int streamType, int stream)
    : RealGu(pu, addrcode, streamType, stream)
{
    m_stream     = stream;
    m_trdView    = new Thread();
    m_tmRecvTime = sys->currentTimeMillis();
    m_strCallId  = "";
    m_iConId     = -1;
    m_iDialId    = -1;
}

Gb28181bRealGu::~Gb28181bRealGu()
{
}

void*
Gb28181bRealGu::threadProc(ThreadInfo *info)
{
    Gb28181bRealGu *pGu = (Gb28181bRealGu*)info->data;
    if (pGu)
    {
        pGu->previewfun();
    }

    return NULL;
}

void
Gb28181bRealGu::previewfun()
{
    uint64_t sysNowTime = sys->currentTimeMillis();
    bool bok = false;

    while (m_trdView->active())
    {
        sysNowTime = sys->currentTimeMillis();

        if (!bok)
        {
            bok = livestart();
        }

        if (sysNowTime - m_tmRecvTime >= 5 * 1000)
        {
            m_tmRecvTime = sysNowTime;
            bok = false;
        }

        sys->sleep(500);
    }
}

void
Gb28181bRealGu::livestop()
{
    if ("" == m_strCallId)
    {
        return;
    }

    GB28181Pu * pPu = (GB28181Pu *)m_pu;
    pPu->m_sipua->bye(m_iConId, m_iDialId);

    gsessMgr->RTPStop(m_strCallId);

    realGuMgr->delGu(this);
}

bool
Gb28181bRealGu::livestart()
{
    if (!m_pu->online())
    {
        ERRE_MSV("device ip: %s is not on-line.", m_pu->ip().c_str());

        return false;
    }

    livestop();

    /**
      * 1,send invite request
      * 2,new rtp session
      * 3,transport stream
      *
      */

    GB28181Pu * pPu = (GB28181Pu *)m_pu;

    string resp;

    char sdp[1024] = { 0 };

    m_strSessId = m_addrcode + (":") + std::to_string(m_streamType)+":preview";

    int srcport = gsessMgr->RTPSRCPort(m_strSessId);
    if (srcport < 0)
    {
        return false;
    }

    ERRE_MSV("Gb28181bRealGu::start(), sess: %s, srcport: %d", m_strSessId.c_str(), srcport);

    //snprintf(srcsdp, sizeof(srcsdp), GB28181_VIDEO_SDP_FORMAT, systm->addrcode().c_str(), platform->ip().c_str(), platform->ip().c_str(), srcport, ssrc.c_str());
    snprintf(sdp, sizeof(sdp), GB28181_VIDEO_SDP_FORMAT, systm->addrcode().c_str(), pPu->m_sipua->ip().c_str(), pPu->m_sipua->ip().c_str(), srcport, GenerateSSRC(systm->addrcode()).c_str());
    
    int port = pPu->m_sipua->port();
    int sport = pPu->sport();
    int ret = pPu->m_sipua->invite(systm->name(), pPu->m_sipua->ip(), pPu->m_sipua->port(), m_addrcode, pPu->ip(), pPu->sport(), sdp, resp);
    if (ret < 0)
    {
        return false;
    }

    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(resp, rvalue, false))
    {
        return false;
    }
    m_strCallId = rvalue["callid"].asString();
    m_iConId = atoi(rvalue["cid"].asString().c_str());
    m_iDialId = atoi(rvalue["did"].asString().c_str());

    SHL_Json::StreamWriterBuilder writer;
    SHL_Json::Value               value;

    value["fromn"]    = systm->addrcode();
    value["fromh"]    = pPu->m_sipua->ip();
    value["fromp"]    = srcport;
    value["ton"]      = m_addrcode;
    value["toh"]      = pPu->ip();
    value["top"]      = pPu->port();
    value["destip"]   = pPu->ip();
    value["destport"] = rvalue["videoport"].asInt();
    value["vcmode"]   = ((0 == m_streamType) ? "mainstream" : "substream");
    //value["ssrc"]     = rvalue["ssrc"];

    string json = SHL_Json::writeString(writer, value);

    ret = gsessMgr->addSession(m_strSessId, m_strCallId, json);
    if (ret < 0)
    {
        return false;
    }

    gsessMgr->setstreamcallback(m_strSessId, stream_cb, this);

    realGuMgr->addGu(this);

    return true;
}

int
Gb28181bRealGu::start(int offset)
{
    if (m_trdView->active())
    {
        return 0;
    }

    m_trdView->setEntry(threadProc, this, 0);
    m_trdView->start();

    return 0;
}

void        
Gb28181bRealGu::stream_cb(const uint8_t * data, uint32_t size, uint8_t type, void *puser)
{
    if (!data || size <= 0 || !puser)
    {
        return;
    }

    Gb28181bRealGu *gu = (Gb28181bRealGu *)puser;

    gu->m_tmRecvTime = sys->currentTimeMillis();

    if (0 == type)
    {
        gu->notifyVideoStream((void*)data, size, ENCODE_H264, 'I', 1280, 720, 25, sys->currentTimeMillis());
    }
    else if (1 == type)
    {
        gu->notifyAudioStream((void*)data, size, ENCODE_G711A, 8000, 1, 16, 0);
    }
    else if(2 == type)
    {
        gu->notifyVideoStream((void*)data, size, ENCODE_H264, 'I', 1280, 720, 25, sys->currentTimeMillis(), TAG_HIK);
    }

    gu->traffic(size);
    gu->m_lastpts = sys->currentTimeMillis();
}

void
Gb28181bRealGu::stop()
{
    gsessMgr->setstreamcallback(m_strSessId, NULL, NULL);

    m_trdView->stop();
    delete m_trdView;

    livestop();
}

void
Gb28181bRealGu::onTimer()
{
    /* comment */
    /* notifyStatus(StreamListener::S_GU_OFFLINE); */

    Gu::onTimer();
}
