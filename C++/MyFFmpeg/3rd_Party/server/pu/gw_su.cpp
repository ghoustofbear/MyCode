#include "gw_su.h"
#include "gwb_pu.h"
#include "tls_error.h"
#include "gsessmgr.h"
#include "gw_pu.h"
#include "json/json.h"
#include "sipua.h"
#include "system.h"
#include "frame.h"
#include "mempool.h"

#define GW_AUDIO_SDP_FORMAT \
    "v=0\r\n" \
    "o=- 0 0 IN IP4 %s\r\n" \
    "s=-\r\n" \
    "i=The SIP CALL\r\n" \
    "c=IN IP4 %s\r\n" \
    "t=0 0\r\n" \
    "m=audio %d RTP/AVP 8\r\n" \
    "a=rtpmap:8 PCMA/8000\r\n" \
    "a=sendrecv\r\n\r\n"

GwSu::GwSu(Pu *pu, string addrcode)
    : Su(pu, addrcode)
{
    m_rtpThread = NULL;
    
}

GwSu::GwSu(Pu *pu, int channelNo)
    : Su(pu, channelNo)
{

}

GwSu::~GwSu()
{
    stop();
}

int 
GwSu::start()
{
    if (!m_pu->online())
    {
        ERRE_MSV("device ip: %s is not on-line.", m_pu->ip().c_str());

        return -1;
    }

    if (!m_rtpThread)
    {
        m_rtpThread = new Thread;
        m_rtpThread->setEntry(threadProc, this, T_PUSH_VIDEO);
        m_rtpThread->start();
    }

    GwBPu *pPu = (GwBPu*)m_pu;
    string resp;

    char sdp[1024] = { 0 };
    m_strSessId = m_addrcode + (":") + std::to_string(0xffff);

    int srcport = gsessMgr->RTPSRCPort(m_strSessId);
    if (srcport < 0)
    {
        return -1;
    }

    ERRE_MSV("GwSu::start(), sess: %s, srcport: %d", m_strSessId.c_str(), srcport);

    sprintf(sdp, GW_AUDIO_SDP_FORMAT, pPu->m_sipua->ip().c_str(), pPu->m_sipua->ip().c_str(), srcport);
    int ret = pPu->m_sipua->invite(systm->name(), pPu->m_sipua->ip(), pPu->m_sipua->port(), m_addrcode, pPu->m_ip, pPu->m_sport, sdp, resp);
    if (ret < 0)
    {
        return -1;
    }

    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(resp, rvalue, false))
    {
        return  -1;
    }
    m_strCallId = rvalue["callid"].asString();
    m_iConId = atoi(rvalue["cid"].asString().c_str());
    m_iDialId = atoi(rvalue["did"].asString().c_str());

    SHL_Json::StreamWriterBuilder writer;
    SHL_Json::Value               value;

    value["fromn"] = systm->addrcode();
    value["fromh"] = pPu->m_sipua->ip();
    value["fromp"] = srcport;
    value["ton"] = m_addrcode;
    value["toh"] = pPu->m_ip;
    value["top"] = pPu->m_port;
    value["destip"] = pPu->m_ip;
    value["destport"] = rvalue["audioport"].asInt();
    value["vcmode"] = 1;

    string json = SHL_Json::writeString(writer, value);
    ret = gsessMgr->addSession(m_strSessId, m_strCallId, json);
    if (ret < 0)
    {
        return -1;
    }

    gsessMgr->setstreamcallback(m_strSessId, stream_cb, this);

    return 0;
}

void
GwSu::stream_cb(const uint8_t * data, uint32_t size, uint8_t type, void *puser)
{
    if (!data || size <= 0 || !puser)
    {
        return;
    }

    GwSu *su = (GwSu *)puser;
    if (su->m_listener)
    {
        if (0 == type)
        {
            //gu->notifyVideoStream((void*)data, size, ENCODE_H264, 'I', 1280, 720, 25, sys->currentTimeMillis());
        }
        else if (1 == type)
        {
            //su->m_listener->onAudioStream((void*)data, size, ENCODE_G711A, 8000, 1, 16, 0);
            AutoLock lock(su->m_listPopAudio._mtx);
            audioframe *frame = new audioframe();
            if (!frame)
            {
                return;
            }

            memcpy(frame->buf, (char*)data, size);
            frame->size = size;
            su->m_listPopAudio.push_back(frame);
        }
    }

    su->traffic(size);
    su->m_lastpts = sys->currentTimeMillis();
}

void 
GwSu::stop()
{
    if (NULL != m_rtpThread)
    {
        m_rtpThread->stop();
        delete m_rtpThread;
        m_rtpThread = NULL;
    }

    GwBPu * pPu = (GwBPu *)m_pu;
    pPu->m_sipua->bye(m_iConId, m_iDialId);

    gsessMgr->setstreamcallback(m_strSessId, NULL, this);
    gsessMgr->RTPStop(m_strCallId);
}

int
GwSu::reset()
{
    return 0;
}

void 
GwSu::doVideoStream(void *stream, uint32_t size)
{
    
}
     
void
GwSu::doAudioStream(void *stream, uint32_t size, int flag)
{
    AutoLock lock(m_listPushAudio._mtx);
    audioframe *frame = new audioframe();
    if (!frame)
    {
        return;
    }

    memcpy(frame->buf, (char*)stream, size);
    frame->size = size;
    m_listPushAudio.push_back(frame);
}

void
GwSu::doStatus(int code)
{

}

void * GwSu::threadProc(ThreadInfo *info)
{
    GwSu *su = (GwSu *)(info->data);

    switch (info->id)
    {
    case T_PUSH_VIDEO:
        su->onPushVideo();
        break;

    default:
        break;
    }

    return NULL;
}

void GwSu::onPushVideo()
{
    while (m_rtpThread->active())
    {
        {
            AutoLock lock(m_listPushAudio._mtx);
            if (!m_listPushAudio.empty())
            {
                audioframe *frame = (*m_listPushAudio.begin());
                gsessMgr->doSendAudio(m_strSessId, (const uint8_t*)frame->buf, frame->size);

                delete frame;
                m_listPushAudio.pop_front();
            }
        }

        {
            AutoLock lock(m_listPopAudio._mtx);
            if (!m_listPopAudio.empty())
            {
                audioframe *frame = (*m_listPopAudio.begin());
                if (m_listener)
                {
                    m_listener->onAudioStream((void*)frame->buf, frame->size, ENCODE_G711A, 8000, 1, 16, 0);
                }

                delete frame;
                m_listPopAudio.pop_front();
            }
        }

        sys->sleep(10);
    }
}