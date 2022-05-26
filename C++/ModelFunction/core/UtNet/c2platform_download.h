// c2platform_download.h: interface for the c2platform_download class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_C2PLATFORM_DOWNLOAD_H__B30B1F7A_05E8_48A7_8463_989251E83EF6__INCLUDED_)
#define AFX_C2PLATFORM_DOWNLOAD_H__B30B1F7A_05E8_48A7_8463_989251E83EF6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ut_net.h" 
#include "c2sdkc1.h"
#include "basedownload.h"

class C2PlatformConn;
class C2PlatformDownload : public BaseDownload
{
public:

	C2PlatformDownload(C2PlatformConn *pConn);
	virtual ~C2PlatformDownload();

	//按文件名开始下载录像
	BOOL Start(const NET_RECDOWNLOAD_PARAM_S *pParam);
	
	//停止远程下载
	void Stop();
	
	//获取当前下载位置
	U32 GetDownloadPos();

	//按文件名下载状态回调
	friend void C2StatusCallback(int code, void* userdata);
	
	//按文件名下载码流回调
    friend bool C2StreamCallback(HWND wnd, void *data, uint32_t size, void *userdata);

private:
	C2PlatformConn *m_pConn; //连接对象	

	pfnRecordDownloadStatusCallback m_pfnRecordDownloadStatusCallback; //下载状态回调函数
	pfnRecordDownloadDataCallback m_pfnRecordDownloadDataCallback; //下载数据状态回调函数 

	HANDLE m_hDownload; //下载句柄
 
	void * m_pUserData;		//按文件下载用户数据
};

#endif // !defined(AFX_C2PLATFORM_DOWNLOAD_H__B30B1F7A_05E8_48A7_8463_989251E83EF6__INCLUDED_)
