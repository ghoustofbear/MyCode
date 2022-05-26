// c2Platform_DownloadByTime.h: interface for the c2Platform_DownloadByTime class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_C2PLATFORM_DOWNLOADBYTIME_H__E5CD03A4_FB72_43B4_B02D_5879E5914B95__INCLUDED_)
#define AFX_C2PLATFORM_DOWNLOADBYTIME_H__E5CD03A4_FB72_43B4_B02D_5879E5914B95__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ut_net.h" 
#include "c2sdkc1.h"
#include "basedownloadbytime.h"

class C2PlatformConn;
class C2PlatformDownloadByTime : public BaseDownloadByTime 
{
public:

	C2PlatformDownloadByTime(C2PlatformConn *pConn);
	virtual ~C2PlatformDownloadByTime();
	
	//按时间开始下载录像
	BOOL Start(const NET_RECDOWNLOAD_BYTIME_PARAM_S *pParam);
	
	//停止按时间下载
	void Stop();
	
	//获取当前下载位置
	U32 GetDownloadPos();
	
	//按时间下载状态回调
	friend void C2StatusByTimeCallback(int code, void* userdata);
	
	//按时间下载码流回调
	friend bool C2StreamByTimeCallback(HWND wnd, void *data, uint32_t size, void *userdata);

private:
	C2PlatformConn *m_pConn; //连接对象	
	
	pfnRecordDownloadStatusCallback m_pfnRecordDownloadByTimeStatusCallback;  //按照时间下载状态回调函数
	pfnRecordDownloadDataCallback m_pfnRecordDownloadDataByTimeCallback;	//按照时间下载数据回调函数
	
	HANDLE m_hDownload; //下载句柄
	
	void * m_pUserData;	//按时间下载用户数据
};

#endif // !defined(AFX_C2PLATFORM_DOWNLOADBYTIME_H__E5CD03A4_FB72_43B4_B02D_5879E5914B95__INCLUDED_)
