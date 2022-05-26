#ifndef __GW_SIPUAEX_H__
#define __GW_SIPUAEX_H__

#include "gw_sipua.h"

class HttpClient;

class GWSipUAEx : public GWSipUA
{
public:
	GWSipUAEx(const char *addrcode, const char *rhhost, int rhport, const char *lshost, int lsport);
	
	virtual ~GWSipUAEx();

	virtual int        start();
	virtual void       stop();

	int				   query_resource(const string &ton, const string &toh, int top, const string &code, pfnQueryResourceCallback fnCb, void *pUser);

	int				   query_record(const string &ton, const string &toh, int top, const string &usercode,
		                    const string &begin, const string &end, uint32_t type, pfnRecordCallback fnCb, void *pUser);

	int				   query_alarm(const string &ton, const string &toh, int top, const string &code,
		                   const string &begin, const string &end, uint32_t type, int level, pfnAlarmCallback fnCb, void *pUser);

private:
	HttpClient  * m_httpClient;
};

#endif
