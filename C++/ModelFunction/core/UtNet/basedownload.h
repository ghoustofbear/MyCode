#ifndef  __BASEDOWNLOAD_H__
#define  __BASEDOWNLOAD_H__

#include "ut_net.h"

class BaseDownload
{
public:
    BaseDownload(){}
    virtual ~BaseDownload(){}

	/* ��ʼ���� */
	virtual BOOL Start(const NET_RECDOWNLOAD_PARAM_S * pParam) = 0;

	/* �������� */
	virtual VOID Stop() = 0;

	/* ��ȡ���ؽ��� */
	virtual U32  GetDownloadPos() = 0;
	
protected: 
	
private:

};

#endif 
