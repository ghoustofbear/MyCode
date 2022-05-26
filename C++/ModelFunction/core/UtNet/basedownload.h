#ifndef  __BASEDOWNLOAD_H__
#define  __BASEDOWNLOAD_H__

#include "ut_net.h"

class BaseDownload
{
public:
    BaseDownload(){}
    virtual ~BaseDownload(){}

	/* 开始下载 */
	virtual BOOL Start(const NET_RECDOWNLOAD_PARAM_S * pParam) = 0;

	/* 结束下载 */
	virtual VOID Stop() = 0;

	/* 获取下载进度 */
	virtual U32  GetDownloadPos() = 0;
	
protected: 
	
private:

};

#endif 
