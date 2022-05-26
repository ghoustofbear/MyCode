#ifndef  __HIKPLATFORM_DOWNLOAD_H__
#define  __HIKPLATFORM_DOWNLOAD_H__

#include "ut_net.h"
#include "VideoSDK.h"
#include "basedownload.h"

class HikPlatformConn;

class HikPlatformDownload : public BaseDownload
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

	enum DOWNLOAD_TYPE
	{
		DOWNLOAD_BYFILE,
		DOWNLOAD_BYTIME,
	};

	friend void CALLBACK DownloadMsgCallBack(VIDEO_INT64 i64PlayHandle, int iMsg, void* pUserData);
	
	friend void CALLBACK DownloadDecodedDataCallback(VIDEO_INT64 i64PlayHandle, const char* pDataArray, int iDataLen, int iWidth, int iHeight, 
		int iFrameType, int iTimeStamp, void* pUserData);
	
	friend void CALLBACK DownloadStreamCallBack(VIDEO_INT64 i64PlayHandle, int iStreamDataType, const char* pDataArray, int iDataLen, void* pUserData);
	
	HikPlatformDownload(HikPlatformConn * pConn);
	virtual ~HikPlatformDownload();
	
	BOOL Start(const NET_RECDOWNLOAD_PARAM_S *pParam);
	
	VOID Stop();
	
	U32  GetDownloadPos();

protected:
	static VOID *RecDownloadLoop(VOID *pArg);

	VOID RecDownload();

private:
    BOOL  OpenStream();

	VOID  CloseStream();

	HWND  CreateVirtualHwnd();

private:	
	BOOL              m_bExit;
	volatile BOOL     m_bDownloadExp;                    /**< 下载异常 */
    volatile BOOL     m_bDownloadOk;                     /**< 下载完毕 */
	__int64           m_downloadId;
	time_t            m_tmBeginSec;
	time_t            m_tmEndSec; 
	U64               m_u64FileSize;
	U64               m_u64FileLastlen;
	U32               m_u32Pos;

protected:
	HikPlatformConn * m_pConn;	
	DOWNLOAD_TYPE     m_downType;
	string            m_strUrl;
	DATETIME_S        m_stBeginTime;
	DATETIME_S        m_stEndTime;
	pfnRecordDownloadStatusCallback  m_fStatusCallback;  /**< 下载状态回调函数指针 */
	VOID            * m_pUser;
	FILE            * m_pSaveFile;
	THREAD            m_trdDownload;
};

#endif
