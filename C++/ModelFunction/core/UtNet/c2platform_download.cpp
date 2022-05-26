// c2platform_download.cpp: implementation of the c2platform_download class.
//
//////////////////////////////////////////////////////////////////////
 

#include "c2platform_download.h"
#include "c2platform_conn.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C2PlatformDownload::C2PlatformDownload(C2PlatformConn *pConn)
:BaseDownload()
{
	m_pConn = pConn;
	m_hDownload = NULL;
	m_pUserData = NULL; 
}

C2PlatformDownload::~C2PlatformDownload()
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
bool C2StreamCallback(HWND wnd, void *data, uint32_t size, void *userdata)
{
	C2PlatformDownload *pDownload = (C2PlatformDownload *)userdata;
	if(NULL != pDownload->m_pfnRecordDownloadDataCallback)
	{
		if(TRUE == pDownload->m_pfnRecordDownloadDataCallback(data, size, pDownload->m_pUserData))
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
void C2StatusCallback(int code, void* userdata)
{
	C2PlatformDownload *pDownload = (C2PlatformDownload *)userdata;
	if(NULL != pDownload->m_pfnRecordDownloadStatusCallback)
	{
		int iCode = 0;
		switch(code)
		{
		case CODE_STREAM_END:
			break;	
		}
		pDownload->m_pfnRecordDownloadStatusCallback(iCode, pDownload->m_pUserData);
	}
}



//************************************************************************
//函数名称：Start(const NET_RECDOWNLOAD_PARAM_S *pParam)
//函数说明：按文件名开始下载录像
//函数参数：
//    (in) pParam      录像下载信息结构体
//返回值：成功返回true/失败返回false
//************************************************************************/
BOOL C2PlatformDownload::Start(const NET_RECDOWNLOAD_PARAM_S *pParam)
{
	if((NULL != m_pConn) && (NULL != pParam))
	{
		c2_download_s downloadInfo = {0};
		downloadInfo.savepath = pParam->ps8FileName;
		downloadInfo.filename = pParam->ps8RecFileName;
		downloadInfo.obtainType = 0; //aaaaa码流
		downloadInfo.deviceHost = pParam->ps8DeviceIp;
		downloadInfo.devicePort = pParam->u16DevicePort;
		downloadInfo.subServHost = pParam->ps8SubServIp;
		downloadInfo.subServPort = pParam->u16SubServPort;

		downloadInfo.addrcode = pParam->ps8FrontAddrCode;	//aaaaaa前端地址码
		m_pfnRecordDownloadStatusCallback = pParam->fStatusCallback;
		m_pfnRecordDownloadDataCallback = pParam->fRecordDataCallback;

		m_pUserData = pParam->pUser;
	
		if(NULL != m_hDownload)
		{
			m_hDownload = NULL;
		}
		m_hDownload = c2_start_download(m_pConn->m_hLogin, &downloadInfo, C2StreamCallback, this, C2StatusCallback, this);  

		return TRUE;
	}
	return FALSE;
}

//************************************************************************
//函数名称：Stop()
//函数说明：停止某台设备某个通道的远程下载
//函数参数：无
//返回值：void
//************************************************************************/
void C2PlatformDownload::Stop()
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
U32 C2PlatformDownload::GetDownloadPos()
{
	if(NULL != m_hDownload)
	{
		long lRet = c2_get_downloadpos(m_hDownload);
		return ((U32)lRet);
	}
	return 0;
}

