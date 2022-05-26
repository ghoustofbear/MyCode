#include "gwa_pu.h"

GwAPu::GwAPu(int type, string addrcode, string name, string host, int sport, int rport, int hport, int rtport)
	: GwBPu(type, addrcode, name, host, sport, rport, hport, rtport)
{
	m_httpClient = NULL;
}

GwAPu::~GwAPu()
{
	stop();
}

int GwAPu::start()
{
	char url[128] = { 0 };
	sprintf(url, "http://%s:%d", m_host.c_str(), m_hport);
	m_httpClient = new HttpClient(url, this);

	GwBPu::start();
	return 0;
}

void GwAPu::stop()
{
	if (NULL != m_httpClient)
	{
		delete m_httpClient;
		m_httpClient = NULL;
	}

	GwBPu::stop();
}

int GwAPu::login()
{
	do 
	{
		if (m_login)
		{
			return 0;
		}

		if (NULL == m_httpClient)
		{
			break;
		}

		string xml;
		int ret = m_httpClient->query_resource(m_addrcode.c_str(), 1, 1000, xml);
		if (ret < 0)
		{
			break;
		}

		m_login = true;
		return 0;
	} while (0);

	return -1;
}

void GwAPu::logout()
{
	stop();
}

int GwAPu::queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode)
{
	do 
	{
		if (false == m_login)
		{
			break;
		}
		if (NULL == m_httpClient)
		{
			break;
		}

		char starttime[128] = { 0 };
		char endtime[128] = { 0 };

		sprintf(starttime, "%04d-%02d-%02dT%02d:%02d:%02dZ", start.year, start.month, start.day, start.hour, start.minute, start.second);
		sprintf(endtime, "%04d-%02d-%02dT%02d:%02d:%02dZ", end.year, end.month, end.day, end.hour, end.minute, end.second);

		int ret = m_httpClient->query_record(addrcode.c_str(), starttime, endtime, 0, 1, 1000, json);
		if (ret < 0)
		{
			break;
		}

        if (m_clienQueryRecord_cb)
        {
			m_clienQueryRecord_cb(0, this->protocol(), json, m_clientUserdata);
        }
		return 0;
	} while (0);

    if (m_clienQueryRecord_cb)
    {
		m_clienQueryRecord_cb(-1, this->protocol(), json, m_clientUserdata);
    }
	return -1;
}

int GwAPu::queryAlarm(string addrcode, int type, int level, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode)
{
	do
	{
		if (false == m_login)
		{
			break;
		}
		if (NULL == m_httpClient)
		{
			break;
		}

		char starttime[128] = { 0 };
		char endtime[128] = { 0 };

		sprintf(starttime, "%04d-%02d-%02dT%02d:%02d:%02dZ", start.year, start.month, start.day, start.hour, start.minute, start.second);
		sprintf(endtime, "%04d-%02d-%02dT%02d:%02d:%02dZ", end.year, end.month, end.day, end.hour, end.minute, end.second);

		int ret = m_httpClient->query_alarm(m_addrcode.c_str(), starttime, endtime, type, 0, level, 1000, json);
		if (ret < 0)
		{
			break;
		}

		return 0;
	} while (0);

	return -1;
}