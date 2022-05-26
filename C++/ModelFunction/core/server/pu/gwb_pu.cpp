#include "gwb_pu.h"
#include "gw_sipua.h"
#include "system.h"
#include "sipua.h"

GwBPu::GwBPu(int type, string addrcode, string name, string host, int sport, int rport, int hport, int rtport)
    :GwPu(type, addrcode, name, host, sport, rport, hport, rtport)
{
    m_protocol = PROTO_TYPE_GW_B;
}

GwBPu::~GwBPu()
{
	stop();
}

int
GwBPu::start()
{
    if (Pu::start() < 0)
    {
        return -1;
    }

    if (!m_sipua)
    {
        m_sipua = new GWSipUA(IPPROTO_UDP, "", -1);
        m_sipua->start();
    }

    m_init = true;
    return 0;
}

void
GwBPu::stop()
{
    m_init = false;

    logout();

    Pu::stop();
}

int
GwBPu::login()
{
    if (m_login)
    {
        return 0;
    }
    
    string xml;
    int Ret = ((GWSipUA *)m_sipua)->query_resource(systm->name(), "", m_sipua->port(), m_addrcode, m_ip, m_sport, m_addrcode, xml);
    if (Ret < 0)
    {
        m_login = false;
        return -1;
    }

    m_login = true;
    return 0;
}

void
GwBPu::logout()
{
	
}

int
GwBPu::queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode)
{
    GWSipUA *gw = static_cast<GWSipUA*>(m_sipua);
    
	char starttime[128] = { 0 };
	char endtime[128] = { 0 };

	sprintf(starttime, "%04d-%02d-%02dT%02d:%02d:%02dZ", start.year, start.month, start.day, start.hour, start.minute, start.second);
	sprintf(endtime, "%04d-%02d-%02dT%02d:%02d:%02dZ", end.year, end.month, end.day, end.hour, end.minute, end.second);

    int ret = gw->query_record(systm->name(), m_addrcode, m_sipua->port(), addrcode, m_ip, m_sport, usercode, starttime, endtime, type, json, 0, fromIndex, toIndex);

    if (m_clienQueryRecord_cb)
    {
		m_clienQueryRecord_cb(ret, this->protocol(), json, m_clientUserdata);
    }
    if (ret < 0)
	{
		return -1;
	}

    return ret;
}

int
GwBPu::queryAlarm(string addrcode, int type, int level, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode)
{
	GWSipUA *gw = static_cast<GWSipUA*>(m_sipua);

	char starttime[128] = { 0 };
	char endtime[128] = { 0 };

	sprintf(starttime, "%04d-%02d-%02dT%02d:%02d:%02dZ", start.year, start.month, start.day, start.hour, start.minute, start.second);
	sprintf(endtime, "%04d-%02d-%02dT%02d:%02d:%02dZ", end.year, end.month, end.day, end.hour, end.minute, end.second);

    int ret = gw->query_alarm(systm->name(), m_addrcode, m_sipua->port(), addrcode, m_ip, m_sport, usercode, starttime, endtime, type, level, json, 0, fromIndex, toIndex);
	return ret;
}

void
GwBPu::onTimer()
{
    if (m_login)
    {
        uint64_t tmNowTime = sys->currentTimeMillis();

        if (tmNowTime - m_tmPrevTime >= 5 * 1000)
        {
            string xml;
            int Ret = ((GWSipUA *)m_sipua)->query_resource(systm->name(), "", m_sipua->port(), m_addrcode, m_ip, m_sport, m_addrcode, xml, 0, 1, 5);
            if (Ret < 0)
            {
                m_login = false;

                closeAllRealGu();
            }

            m_tmPrevTime = tmNowTime;
        }
    }

    Pu::onTimer();
}