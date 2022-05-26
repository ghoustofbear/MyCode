#include "gwb_gu.h"
#include "gwb_pu.h"
#include "sipua.h"
#include "tls_error.h"
#include "session.h"
#include "frame.h"
#include "gumgr.h"
#include "gsessmgr.h"
#include "system.h"
#include "thread.h"
#include "gsessmgr.h"

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

void
GwbRealGu::status_cb(int code, void* userData)
{

}


GwbRealGu::GwbRealGu(Pu *pu, string addrcode, int streamType, int stream)
    : RealGu(pu, addrcode, streamType, stream)
{
    m_stream     = stream;
    m_trdView    = new Thread();
    m_tmRecvTime = sys->currentTimeMillis();
    m_strCallId  = "";
    m_iConId     = -1;
    m_iDialId    = -1;
}

GwbRealGu::~GwbRealGu()
{
}

void*
GwbRealGu::threadProc(ThreadInfo *info)
{
    GwbRealGu *pGu = (GwbRealGu*)info->data;
    if (pGu)
    {
        pGu->previewfun();
    }

    return NULL;
}

void
GwbRealGu::previewfun()
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

        sys->sleep(10);
    }
}

void
GwbRealGu::livestop()
{
    if ("" == m_strCallId)
    {
        return;
    }

    GwBPu * pPu = (GwBPu *)m_pu;
    pPu->m_sipua->bye(m_iConId, m_iDialId);

    gsessMgr->RTPStop(m_strCallId);

    realGuMgr->delGu(this);

    m_strCallId = "";
    m_iConId    = 0;
    m_iDialId   = 0;
}

bool
GwbRealGu::livestart()
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

    GwBPu * pPu = (GwBPu *)m_pu;

    string resp;

    char sdp[1024] = { 0 };

    m_strSessId = m_addrcode + (":") + std::to_string(m_streamType);

    int srcport = gsessMgr->RTPSRCPort(m_strSessId);
    if (srcport < 0)
    {
        return false;
    }

    ERRE_MSV("GwbRealGu::start(), sess: %s, srcport: %d", m_strSessId.c_str(), srcport);

    sprintf(sdp, GW_SDP_FORMAT, pPu->m_sipua->ip().c_str(), pPu->m_sipua->ip().c_str(), srcport);
    int ret = pPu->m_sipua->invite(systm->name(), pPu->m_sipua->ip(), pPu->m_sipua->port(), m_addrcode, pPu->m_ip, pPu->m_sport, sdp, resp);
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
    value["toh"]      = pPu->m_ip;
    value["top"]      = pPu->m_port;
    value["destip"]   = pPu->m_ip;
    value["destport"] = rvalue["videoport"].asInt();
    value["vcmode"]   = ((0 == m_streamType)? "mainstream" : "substream");

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
GwbRealGu::start(int offset)
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
GwbRealGu::stream_cb(const uint8_t * data, uint32_t size, uint8_t type, void *puser)
{
    if (!data || size <= 0 || !puser)
    {
        return;
    }

    GwbRealGu *gu = (GwbRealGu *)puser;

    gu->m_tmRecvTime = sys->currentTimeMillis();

    if (0 == type)
    {
        gu->notifyVideoStream((void*)data, size, ENCODE_H264, 'I', 1280, 720, 25, sys->currentTimeMillis());
    }
    else if (1 == type)
    {
        gu->notifyAudioStream((void*)data, size, ENCODE_G711A, 8000, 1, 16, 0);
    }

    gu->traffic(size);
    gu->m_lastpts = sys->currentTimeMillis();
}

void
GwbRealGu::stop()
{
    gsessMgr->setstreamcallback(m_strSessId, NULL, NULL);

    m_trdView->stop();
    delete m_trdView;

    livestop();

   // gsessMgr->delSession(m_strSessId);
}

void
GwbRealGu::onTimer()
{
    /* comment */
    /* notifyStatus(StreamListener::S_GU_OFFLINE); */

    Gu::onTimer();
}
