#ifndef  __BASEDOWNLOADBYTIME_H__
#define  __BASEDOWNLOADBYTIME_H__

#include "ut_net.h"

class BaseDownloadByTime
{
public:
    BaseDownloadByTime(){}
    virtual ~BaseDownloadByTime(){}

	/* 开始下载 */
	virtual BOOL Start(const NET_RECDOWNLOAD_BYTIME_PARAM_S * pParam) = 0;

	/* 结束下载 */
	virtual VOID Stop() = 0;

	/* 获取下载进度 */
	virtual U32  GetDownloadPos() = 0;

	
protected: 
	
private:

};

#endif 
