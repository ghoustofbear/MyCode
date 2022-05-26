#include "huadian_pu.h"
#include "rtspclient.h"


HuaDianPu::HuaDianPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType)
    : Pu(type, addrcode, name, chnNum, user, pswd, host)
{
    m_lastTime   = sys->currentTimeMillis();
    m_rtspClient = NULL;
    m_protocol = PROTO_TYPE_HUADIAN;
    m_port       = port;
    m_domainType = domainType;
}

HuaDianPu::~HuaDianPu()
{

}

int
HuaDianPu::start()
{
    if (Pu::start() < 0)
    {
        return -1;
    }

    m_rtspClient = new RtspClient(m_host.c_str(), m_port);

    char strUrl[256] = { 0 };
    sprintf(strUrl, "rtsp://%s:%d/user=admin&password=&channel=1&stream=1.sdp", m_host.c_str(), m_port);
    m_rtspClient->startex(strUrl, this);

    m_init = true;
    return 0;
}

void
HuaDianPu::stop()
{
    if (!m_init)
        return;
    m_init = false;

    logout();

    Pu::stop();
}

int
HuaDianPu::login()
{
    if (m_login)
    {
        return 0;
    }

    bool bret = m_rtspClient->doOption();
    if (bret)
    {
        m_login = true;
    }

    return  (bret) ? 0 : -1;
}

void
HuaDianPu::logout()
{
    if (NULL != m_rtspClient)
    {
        delete m_rtspClient;
        m_rtspClient = NULL;
    }
}

int
HuaDianPu::heartbeat()
{
    if (NULL == m_rtspClient)
    {
        return -1;
    }

    bool bret = m_rtspClient->doOption();
    return (bret) ? 0 : -1;
}

void
HuaDianPu::onTimer()
{
    if (!m_login)
    {
        return;
    }

    time_t tmNow = sys->currentTimeMillis();
    if (tmNow - m_lastTime >= 5000)
    {
        if (heartbeat() < 0)
        {
            m_login = false;
            m_first = false;
        }

        m_lastTime = tmNow;
    }

    Pu::onTimer();
}