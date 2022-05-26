#include "huadian_gu.h"
#include "pu.h"
#include "tls_error.h"
#include "rtspclient.h"
#include "gsessmgr.h"
#include "json/json.h"
#include "system.h"
#include "platform.h"
#include "gumgr.h"
#include "PsProcessApi.h"

void 
HuaDianGu::pspackdatacallback(uint8_t *data, int size, void* userdata, int type, uint64_t pts, uint64_t frameTime)
{
    HuaDianGu * gu = (HuaDianGu*)userdata;
    if (gu)
    {
        gu->notifyPrivStream((void *)data, size, ENCODE_H264, TAG_HIK_N);
    }
}

HuaDianGu::HuaDianGu(Pu * pu, int channelNo, string subHost, int subPort, int streamType, int streamProtoType)
    : RealGu(pu, channelNo, subHost, subPort, streamType, streamProtoType)
{
    m_rtspClient = new RtspClient("0.0.0.0", 6666);
    m_psHandle = createpsinstance(PS_PACK);
    setpsstreamcallback(m_psHandle, pspackdatacallback, this);
    m_bok = false;
    m_lastHeartTime = sys->currentTimeMillis();
}

HuaDianGu::HuaDianGu(Pu *pu, string addrcode, int streamType, int streamProtoType)
    : RealGu(pu, addrcode, streamType, streamProtoType)
{
    m_rtspClient = new RtspClient("0.0.0.0", 6666);
    m_psHandle = createpsinstance(PS_PACK);
    m_bok = false;
    m_lastHeartTime = sys->currentTimeMillis();
}

HuaDianGu::~HuaDianGu()
{

}

int 
HuaDianGu::start(int offset)
{
    if (!m_pu->online())
    {
        ERRE_MSV("device ip: %s is not on-line.", m_pu->ip().c_str());
        return -1;
    }

    char strUrl[256] = { 0 };
    sprintf(strUrl, "rtsp://%s:%d/user=admin&password=&channel=%d&stream=%d.sdp", m_pu->host().c_str(), m_pu->port(), m_channelNo, m_streamType);
    bool bret = m_rtspClient->startex(strUrl, this);
    if (!bret)
    {
        return -1;
    }

    bret = m_rtspClient->doOption();
    if (!bret)
    {
        return -1;
    }

    bret = m_rtspClient->doDescribe();
    if (!bret)
    {
        return -1;
    }

    char sessid[256] = { 0 };
    sprintf(sessid, "%0x_%d_%d", this, m_channelNo, m_streamType);
    string strSessId = sessid;
    int32_t s32BasePort = gsessMgr->RTPSRCPort(strSessId);
    int32_t s32DestPort = 0;

    bret = m_rtspClient->doSetup("RTP/AVP", "unicast", s32BasePort, s32DestPort);
    if (!bret)
    {
        return -1;
    }

    SHL_Json::StreamWriterBuilder writer;
    SHL_Json::Value               value;

    value["fromn"]  = systm->addrcode();
    value["fromh"]  = platform->ip();
    value["fromp"]  = m_pu->port();
    value["ton"]    = m_addrcode;
    value["toh"]    = m_pu->host();
    value["top"]    = m_pu->port();
    value["destip"] = m_pu->host();
    value["destport"] = s32DestPort;
    value["vcmode"] = "mainstream";
    value["rtcpchk"] = "no";

    string json = SHL_Json::writeString(writer, value);

    int iRet = gsessMgr->addSession(strSessId, strSessId, json);
    if (iRet < 0)
    {
        return -1;
    }

    gsessMgr->setstreamcallback(strSessId, stream_cb, this);

    m_rtspClient->doPlay("1", "clock=0-");

    realGuMgr->addGu(this);

    m_bok = true;

    return 0;
}


void
HuaDianGu::stream_cb(const uint8_t * data, uint32_t size, uint8_t type, void *puser)
{
    if (!data || size <= 0 || !puser)
    {
        return;
    }

    HuaDianGu *gu = (HuaDianGu *)puser;
    if (gu)
    {      
        gu->notifyVideoStream((void*)data, size, ENCODE_H264, 'I', 1920, 1080, 25, sys->currentTimeMillis());

        inputstreamdata(gu->m_psHandle, (uint8_t*)data, size, 0, 0, 0);

        gu->traffic(size);
        gu->m_lastpts = sys->currentTimeMillis();
    }
}

void
HuaDianGu::stop()
{
    realGuMgr->delGu(this);

    if (m_rtspClient)
    {
        m_rtspClient->doTeardown();
    }

    char sessid[256] = { 0 };
    sprintf(sessid, "%0x_%d_%d", this, m_channelNo, m_streamType);
    string strSessId = sessid;

    gsessMgr->delSession(strSessId);

    if (m_psHandle)
    {
        destorypsinstance(m_psHandle);
        m_psHandle = NULL;
    }

    m_bok = false;
}

void
HuaDianGu::onTimer()
{
    time_t tmNow = sys->currentTimeMillis();
    if (tmNow - m_lastHeartTime >= 30 * 1000)
    {
        if (m_pu->online())
        {
            m_rtspClient->doGetParameter();
        }

        m_lastHeartTime = tmNow;
    }

    Gu::onTimer();
}