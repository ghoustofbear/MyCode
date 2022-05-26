// c2Platform_DownloadByTime.cpp: implementation of the c2Platform_DownloadByTime class.
//
//////////////////////////////////////////////////////////////////////

#include "c2Platform_DownloadByTime.h"
#include "c2platform_conn.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C2PlatformDownloadByTime::C2PlatformDownloadByTime(C2PlatformConn *pConn)
:BaseDownloadByTime()
{
	m_pConn = pConn;
	m_hDownload = NULL; 
	m_pUserData = NULL;
}

C2PlatformDownloadByTime::~C2PlatformDownloadByTime()
{

}

//************************************************************************
//函数名称：C2StreamCallback(HWND wnd, void *data, uint32_t size, void *userdata)
//函数说明：按文件码流数据回调函数
//函数参数：
//    (in) wnd      当前的预览/回放句柄
//    (in) data     存放数据的缓冲区指针, ut_frame_s+音视频数据
//    (in) size     缓冲区大小
//    (in) userdata 用户数据
//返回值：成功返回true/失败返回false
//************************************************************************/
bool C2StreamByTimeCallback(HWND wnd, void *data, uint32_t size, void *userdata)
{
	C2PlatformDownloadByTime *pDownload = (C2PlatformDownloadByTime *)userdata;
	if(NULL != pDownload->m_pfnRecordDownloadDataByTimeCallback)
	{
		if(TRUE == pDownload->m_pfnRecordDownloadDataByTimeCallback(data, size, pDownload->m_pUserData))
		{
			return true;
		}
	}
	return false;
}

//************************************************************************
//函数名称：C2StatusCallback(int code, void* userdata)
//函数说明：按文件下载状态回调
//函数参数：
//    (in) code 状态码
//    (in) userdata 用户数据
//返回值：void
//************************************************************************/
void C2StatusByTimeCallback(int code, void* userdata)
{
	C2PlatformDownloadByTime *pDownload = (C2PlatformDownloadByTime *)userdata;
	if(NULL != pDownload->m_pfnRecordDownloadByTimeStatusCallback)
	{
		int iCode = 0;
		switch(code)
		{
		case CODE_STREAM_END:
			iCode = 2;
			break;	
		}
		pDownload->m_pfnRecordDownloadByTimeStatusCallback(iCode, pDownload->m_pUserData);
	}
}


//************************************************************************
//函数名称：StartByTime(const NET_RECDOWNLOAD_BYTIME_PARAM_S *pParam)
//函数说明：按时间开始下载录像
//函数参数：
//    (in) pParam      录像下载信息结构体
//返回值：成功返回true/失败返回false
//************************************************************************/
BOOL C2PlatformDownloadByTime::Start(const NET_RECDOWNLOAD_BYTIME_PARAM_S *pParam)
{
	if((NULL != m_pConn) && (NULL != pParam))
	{
		c2_download_time_s downloadInfo = {0};
		downloadInfo.savepath = pParam->ps8FileName;
		
		downloadInfo.start.year = pParam->stBegin.u32Year;
		downloadInfo.start.month = pParam->stBegin.u32Month;
		downloadInfo.start.day = pParam->stBegin.u32Day;
		downloadInfo.start.hour = pParam->stBegin.u32Hour;
		downloadInfo.start.minute = pParam->stBegin.u32Minute;
		downloadInfo.start.second = pParam->stBegin.u32Second;
		
		downloadInfo.end.year = pParam->stEnd.u32Year;
		downloadInfo.end.month = pParam->stEnd.u32Month;
		downloadInfo.end.day = pParam->stEnd.u32Day;
		downloadInfo.end.hour = pParam->stEnd.u32Hour;
		downloadInfo.end.minute = pParam->stEnd.u32Minute;
		downloadInfo.end.second = pParam->stEnd.u32Second;
		
		downloadInfo.channelNo = pParam->u32ChnNO;
		downloadInfo.obtainType = 0; //aaaaaa码流  
		downloadInfo.deviceHost = pParam->ps8DeviceIp;
		downloadInfo.devicePort = pParam->u16DevicePort;
		downloadInfo.subServHost = pParam->ps8SubServIp;
		downloadInfo.subServPort = pParam->u16SubServPort;
		
		downloadInfo.addrcode = pParam->ps8FrontAddrCode;
		m_pfnRecordDownloadByTimeStatusCallback = pParam->fStatusCallback;
		m_pfnRecordDownloadDataByTimeCallback = pParam->fRecordDataCallback;
		
		m_pUserData = pParam->pUser;
		
		if(NULL != m_hDownload)
		{
			m_hDownload = NULL;
		}
		m_hDownload = c2_start_downloadByTime(m_pConn->m_hLogin, &downloadInfo, C2StreamByTimeCallback, this, C2StatusByTimeCallback, this);	
		if(NULL != m_hDownload)
		{			
			return TRUE;
		}
	}
	return FALSE;
}

//************************************************************************
//函数名称：Stop()
//函数说明：停止某台设备某个通道的远程下载
//函数参数：无
//返回值：void
//************************************************************************/
void C2PlatformDownloadByTime::Stop()
{
	if((NULL != m_pConn->m_hLogin) && (NULL != m_hDownload))
	{
		c2_stop_download(m_pConn->m_hLogin, m_hDownload);
	}
}

//************************************************************************
//函数名称：GetDownloadPos()
//函数说明：获取当前下载位置
//函数参数：无
//返回值：当前下载位置
//************************************************************************/
U32 C2PlatformDownloadByTime::GetDownloadPos()
{
	if(NULL != m_hDownload)
	{
		long lRet = c2_get_downloadpos(m_hDownload);
		return ((U32)lRet);
	}
	return 0;
}