#include "gw_sipuaex.h"
#include "httpclient.h"

GWSipUAEx::GWSipUAEx(const char *addrcode, const char *rhhost, int rhport, const char *lshost, int lsport)
	: GWSipUA((char*)lshost, lsport, (char*)addrcode)
{
	char url[128] = { 0 };
	sprintf(url, "http://%s:%d", rhhost, rhport);
	m_httpClient = new HttpClient(url, this);
}

GWSipUAEx::~GWSipUAEx()
{
	
}

int GWSipUAEx::start()
{
	int ret = SipUA::start();
	if (0 == ret)
	{
		bool bret = m_httpClient->start();
		if (bret)
		{
			return 0;
		}

		return -1;
	}

	return ret;
}

void GWSipUAEx::stop()
{
	if (NULL != m_httpClient)
	{
		delete m_httpClient;
		m_httpClient = NULL;
	}
}

int GWSipUAEx::query_resource(const string &ton, const string &toh, int top, const string &code, pfnQueryResourceCallback fnCb, void *pUser)
{
	string xml;

	int fromidx = 1;
	int toidx = 1000;
	bool bFinished = false;
	int tempToIdx = toidx;

	while (!bFinished)
	{
		int ret = m_httpClient->query_resource(ton.c_str(), fromidx, tempToIdx, xml);

		SHL_Json::Reader reader;
		SHL_Json::Value  rvalue;
		if (!reader.parse(xml, rvalue, false))
		{
			break;
		}

		int realNum = atoi(rvalue["HTTP_XML"]["SubList"]["@RealNum"].asString().c_str());
		if (toidx > realNum)
		{
			toidx = realNum;
		}

		int toIndex = atoi(rvalue["HTTP_XML"]["SubList"]["@ToIndex"].asString().c_str());
		if (toIndex < toidx)
		{
			fromidx = toIndex + 1;
			tempToIdx = toidx;
		}
		else
		{
			bFinished = true;
		}

		if (fnCb)
		{
			fnCb((char*)xml.c_str(), xml.size(), pUser);
		}
	}

	return 1;
}

int GWSipUAEx::query_record(const string &ton, const string &toh, int top, const string &usercode, const string &begin, const string &end, uint32_t type, pfnRecordCallback fnCb, void *pUser)
{
	string xml;

	int fromidx = 1;
	int toidx = 1000;
	bool bFinished = false;
	int tempToIdx = toidx;

	while (!bFinished)
	{
		int ret = m_httpClient->query_record(ton.c_str(), begin.c_str(), end.c_str(), 0, fromidx, tempToIdx, xml);

		SHL_Json::Reader reader;
		SHL_Json::Value  rvalue;
		if (!reader.parse(xml, rvalue, false))
		{
			break;
		}

		int realNum = atoi(rvalue["HTTP_XML"]["SubList"]["@RealNum"].asString().c_str());
		if (toidx > realNum)
		{
			toidx = realNum;
		}

		int toIndex = atoi(rvalue["HTTP_XML"]["SubList"]["@ToIndex"].asString().c_str());
		if (toIndex < toidx)
		{
			fromidx = toIndex + 1;
			tempToIdx = toidx;
		}
		else
		{
			bFinished = true;
		}

		if (fnCb)
		{
			fnCb((char*)xml.c_str(), xml.size(), pUser);
		}
	}

	return 1;
}

int GWSipUAEx::query_alarm(const string &ton, const string &toh, int top, const string &code, const string &begin, const string &end, uint32_t type, int level, pfnAlarmCallback fnCb, void *pUser)
{
	string xml;

	int fromidx = 1;
	int toidx = 1000;
	bool bFinished = false;
	int tempToIdx = toidx;

	while (!bFinished)
	{
		int ret = m_httpClient->query_alarm(ton.c_str(), begin.c_str(), end.c_str(), type, level, fromidx, tempToIdx, xml);

		SHL_Json::Reader reader;
		SHL_Json::Value  rvalue;
		if (!reader.parse(xml, rvalue, false))
		{
			break;
		}

		int realNum = atoi(rvalue["HTTP_XML"]["SubList"]["@RealNum"].asString().c_str());
		if (toidx > realNum)
		{
			toidx = realNum;
		}

		int toIndex = atoi(rvalue["HTTP_XML"]["SubList"]["@ToIndex"].asString().c_str());
		if (toIndex < toidx)
		{
			fromidx = toIndex + 1;
			tempToIdx = toidx;
		}
		else
		{
			bFinished = true;
		}

		if (fnCb)
		{
			fnCb((char*)xml.c_str(), xml.size(), pUser);
		}
	}

	return 1;
}
