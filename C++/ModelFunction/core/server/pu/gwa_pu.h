#ifndef _GWA_PU_H_
#define _GWA_PU_H_

#include "gwb_pu.h"
#include "httpclient.h"

class GwAPu : public GwBPu
{
public:
	GwAPu(int type, string addrcode, string name, string host, int sport, int rport, int hport, int rtport);
	~GwAPu();

	virtual int        start();

protected:
	virtual void       stop();
	virtual int        login();
	virtual void       logout();

protected:
    virtual int        queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode);
    virtual int        queryAlarm(string addrcode, int type, int level, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode);

private:
	HttpClient       * m_httpClient;
};

#endif
