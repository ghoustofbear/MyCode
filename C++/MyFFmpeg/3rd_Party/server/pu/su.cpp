#include "su.h"
#include "pu.h"
#include "streamlistener.h"
#include "json/json.h"

/**
 * Su
 */
Su::Su(Pu *pu, int channelNo)
{
    m_pu = pu;
    m_channelNo = channelNo;
    m_lastpts = sys->currentTimeMillis();

    if (channelNo < m_pu->channels())
    {
        m_addrcode = m_pu->gucode(channelNo);
        m_name = m_pu->guname(channelNo);
    }
    m_listener = NULL;
}

Su::Su(Pu *pu, string addrcode)
{
    m_pu = pu;
    m_addrcode = addrcode;
    m_lastpts = sys->currentTimeMillis();
    m_channelNo = pu->guchannel(addrcode);
    m_name = pu->guname(addrcode);
    m_listener = NULL;
}

Su::~Su()
{

}

string
Su::addrcode()
{
    return m_addrcode;
}

string
Su::name()
{
    return m_name;
}

string
Su::puname()
{
    return m_pu->name();;
}

string
Su::puip()
{
    return m_pu->ip();
}

double
Su::bps()
{
    uint64_t now = sys->currentTimeMillis();
    uint64_t elapse = now - m_bpspts;
    if (elapse <= 0)
    {
        elapse = 1000;
    }

    double bitrate = (m_transSize << 3) / ((double)elapse / 1000) / 1000;

    m_transSize = 0;
    m_bpspts = sys->currentTimeMillis();

    return bitrate;
}

int
Su::status()
{
    return m_status;
}

void
Su::setUi(OnUiUpdate *ui)
{
    m_ui = ui;
}

void
Su::updateUi()
{

}

void
Su::traffic(uint32_t size)
{
    m_transSize += size;
}

bool
Su::isSame(string addrcode)
{
    return m_addrcode == addrcode ? true : false;
}

bool
Su::isSame(int chnNo)
{
    return m_channelNo == chnNo ? true : false;
}

void
Su::onTimer()
{
    if (!m_pu->online())
    {
        m_status = StreamListener::S_PU_OFFLINE;
    }

    uint64_t pts = sys->currentTimeMillis();
    uint64_t dlta = pts - m_lastpts;

    if (dlta > 2 * 1000)
    {
        m_status = StreamSpeaker::S_SU_OFFLINE;

        /* log every 1 seconds */
        if (0 == (m_tmcnt * 25) % 1000)
        {
            LOGW_MSV("su receice data timeout. pu: %s-%s-%s, gu: %s-%s",
                m_pu->name().c_str(),
                m_pu->addrcode().c_str(),
                m_pu->host().c_str(),
                m_name.c_str(),
                m_addrcode.c_str());
        }
    }
    else
    {
        m_status = StreamSpeaker::S_SOK;
    }

    m_tmcnt++;
}
