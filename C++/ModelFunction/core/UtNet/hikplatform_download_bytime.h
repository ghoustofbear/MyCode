#ifndef  __HIKPLATFORM_DOWNLOADBYTIME_H__
#define  __HIKPLATFORM_DOWNLOADBYTIME_H__

#include "ut_net.h"
#include "hikplatform_download.h"
#include "basedownloadbytime.h"

class HikPlatformConn;

class HikPlatformDownloadByTime : public HikPlatformDownload ,public BaseDownloadByTime
{
#define HIKPLAT_DOWNLOAD_STATUS(code) \
	do \
	{ \
		if (m_fStatusCallback) \
		{ \
			m_fStatusCallback(code, m_pUser); \
		} \
	} while (0);

public:
     HikPlatformDownloadByTime(HikPlatformConn * pConn);
	 ~HikPlatformDownloadByTime();

	 BOOL Start(const NET_RECDOWNLOAD_BYTIME_PARAM_S *pParam);	
	 VOID Stop();
	 U32  GetDownloadPos();
};

#endif
