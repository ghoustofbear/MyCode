#include "hs_pu.h"
#include "hs_sipua.h"
#include "system.h"

HsPu::HsPu(int type, string addrcode, string name,  string host, int sport, int rport, int hport, int rtport)
    : GwPu(type, addrcode, name, host, sport, rport, hport, rtport)
{
    m_protocol = PROTO_TYPE_GW_B_HS;
}

HsPu::~HsPu()
{

}

int
HsPu::start()
{
    if (Pu::start() < 0)
    {
        return -1;
    }

    if (!m_sipua)
    {
        m_sipua = new HSSipUA(IPPROTO_UDP, "", -1);
        return m_sipua->start();
    }

    m_init = true;
    return 0;
}

void
HsPu::stop()
{
    m_init = false;

    logout();

    Pu::stop();
}

int
HsPu::login()
{
    if (m_login)
    {
        return 0;
    }

    /* int                query_resource(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, */
    /*                                   string &content, int jx = 0, int fromidx = 1, int toidx = 100); */
    string xml;
    ((HSSipUA *)m_sipua)->query_resource(systm->addrcode(), "", 0, m_addrcode, m_ip, m_port, xml);

    return -1;
}

void
HsPu::logout()
{

}

int
HsPu::queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json)
{
    return -1;
}
