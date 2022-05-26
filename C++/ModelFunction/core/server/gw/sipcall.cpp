#include "sipcall.h"
#include "gsession.h"
#include "sys.h"

SipCall::SipCall(string &callid, string &fromn, string &fromh, int fromp, string &ton, string &toh, int top)
{
    m_status  = S_UNINIT;
    m_callid  = callid;
    m_fromn   = fromn;
    m_fromh   = fromh;
    m_fromp   = fromp;
    m_ton     = ton;
    m_toh     = toh;
    m_top     = top;

    m_lastpts = sys->currentTimeMillis();
}

SipCall::~SipCall()
{

}
int
SipCall::status()
{
    return m_status;
}

void
SipCall::setmedia(int srcport, string &destip, int destport, int vcmode)
{
    m_srcport  = srcport;
    m_destip   = destip;
    m_destport = destport;
    m_vcmode   = vcmode;
}

string
SipCall::srcip()
{
    return m_toh;
}

int
SipCall::srcport()
{
    return m_srcport;
}

string
SipCall::destip()
{
    return m_destip;
}

int
SipCall::destport()
{
    return m_destport;
}

void
SipCall::onHeartbeat()
{
    m_lastpts = sys->currentTimeMillis();
    m_status  = S_TALKING;
    LOGD_MSV("callid: %s, fromn: %s, fromh: %s, fromp: %d, ton: %s, toh: %s, top: %d, destip: %s, destport: %d, vcmode: %d",
             m_callid.c_str(),
             m_fromn.c_str(),
             m_fromh.c_str(),
             m_fromp,
             m_ton.c_str(),
             m_toh.c_str(),
             m_top,
             m_destip.c_str(),
             m_destport,
             m_vcmode
            );
}

void
SipCall::onClose()
{
    m_status = S_CLOSED;
}

void
SipCall::timerdriver()
{
    uint64_t pts = sys->currentTimeMillis();

    if (pts - m_lastpts > 30 * 1000)
    {
        LOGW_MSV("rtp session timeout. srcip: %s, srcport: %d, destip: %s, destport: %d", srcip().c_str(), srcport(), destip().c_str(), m_destport);
        m_status = S_CLOSED;
    }
}
