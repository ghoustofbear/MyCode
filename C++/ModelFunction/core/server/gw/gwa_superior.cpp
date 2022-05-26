#include "gwa_superior.h"
#include "gw_sipua.h"
#include "httpclient.h"

GwaSuperior::GwaSuperior(string addrcode, string name, string user, string pswd, string host, int sipPort, int rtspPort,
    int httpPort, int expires, string ifupIp, int bregister, int ivauselocal)
    :Superior(addrcode, name, user, pswd, host, sipPort, rtspPort, httpPort, expires, ifupIp)
{
	m_protocol    = SipUA::P_GWA;
	m_bUpdate     = false;
}

GwaSuperior::~GwaSuperior()
{
	if (NULL != m_httpClient)
	{
		delete m_httpClient;
		m_httpClient = NULL;
	}
}

void
GwaSuperior::timerdriver()
{
    /* TODO:  <15-11-17, yourname> */

    Superior::timerdriver();
}

int
GwaSuperior::start()
{
	char url[128] = { 0 };
	sprintf(url, "http://%s:%d", this->host().c_str(), this->hport());
	m_httpClient = new HttpClient(url, this);
	if (m_httpClient)
	{
		m_httpClient->start();
	}

	Superior::start();

	m_keepaliveThread.setEntry(threadProc, this, T_KEEPALIVE);
	return m_keepaliveThread.start();
}

void 
GwaSuperior::stop()
{
	m_keepaliveThread.stop();

	Superior::stop();
}

void
GwaSuperior::keepaliveloop()
{
	time_t pre = 0;
	time_t cur = time(NULL);
	time_t intv = 60;

	while (m_keepaliveThread.active())
	{
		cur = time(NULL);

		if ((cur - pre <= intv) ||
			(NULL == m_httpClient))
		{
			sys->sleep(1000);

			continue;
		}

		pre = cur;

		if (!m_bUpdate)
		{
			if (m_httpClient->update(addrcode().c_str(), 1) < 0)
			{
				m_bUpdate = false;
			}
			else
			{
				m_bUpdate = true;
			}
		}

		if (m_bUpdate)
		{
			if (m_httpClient->heartbeat(addrcode().c_str()) < 0)
			{
				m_bUpdate = false;
			}
		}

		sys->sleep(500);
	}
}

void*
GwaSuperior::threadProc(ThreadInfo *info)
{
	GwaSuperior * super = (GwaSuperior *)(info->data);

	switch (info->id)
	{
	case T_KEEPALIVE:
		super->keepaliveloop();
		break;

	default:
		break;
	}

	return NULL;
}