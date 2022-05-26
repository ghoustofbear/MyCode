// c2platform_devTalk.h: interface for the c2platform_devTalk class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_C2PLATFORM_DEVTALK_H__D5759A9B_4295_427A_B6F5_0C4C5CEBAB67__INCLUDED_)
#define AFX_C2PLATFORM_DEVTALK_H__D5759A9B_4295_427A_B6F5_0C4C5CEBAB67__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ut_net.h" 
#include "c2sdkc1.h"

class C2PlatformConn;
class C2PlatformDevTalk  
{
public:
	BOOL UninitDevTalk();
	BOOL InitDevTalk(int audioType);
 	BOOL StopDevTalk(const NET_DTALK_DEV_LIST *pParam);
 	BOOL StartDevTalk(const NET_DTALK_DEV_LIST *pParam);
	
	friend void C2DevTalkStatusCallback(int code, void* userdata);

	C2PlatformDevTalk(C2PlatformConn *pConn);
	~C2PlatformDevTalk();



private:
	C2PlatformConn* m_pConn;	//连接对象	
	HANDLE m_devTalkHandle;

};

#endif // !defined(AFX_C2PLATFORM_DEVTALK_H__D5759A9B_4295_427A_B6F5_0C4C5CEBAB67__INCLUDED_)
