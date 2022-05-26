/**
 * @file      hs_superior.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-11-15 14:05:45
 *   
 * @note
 *  hs_superior.h defines 
 */
#ifndef __GWA_SUPERIOR_H__
#define __GWA_SUPERIOR_H__
#include "superior.h"
#include "thread.h"
#include "httpclient.h"

class GwaSuperior : public Superior 
{
public:
	GwaSuperior(string addrcode, string name, string user, string pswd, string host, int sipPort, int rtspPort,
        int httpPort, int expires, string ifupIp, int bregister, int ivauselocal);
	virtual ~GwaSuperior();

	virtual int        start();
	virtual void       stop();

protected:
    virtual void       timerdriver();

	void               keepaliveloop();
	static void *      threadProc(ThreadInfo *info);

private:
	Thread             m_keepaliveThread;
	bool               m_bUpdate;
	HttpClient       * m_httpClient;
};
#endif
