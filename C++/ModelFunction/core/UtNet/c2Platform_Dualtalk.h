// c2Platform_Dualtalk.h: interface for the c2Platform_Dualtalk class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_C2PLATFORM_DUALTALK_H__28516A67_3168_4CB0_A28F_03662C79F204__INCLUDED_)
#define AFX_C2PLATFORM_DUALTALK_H__28516A67_3168_4CB0_A28F_03662C79F204__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ut_net.h" 
#include "c2sdkc1.h"
#include "basedualtalk.h"

class C2PlatformConn;
class C2PlatformDualtalk : public BaseDualTalk  
{
public:
	C2PlatformDualtalk(C2PlatformConn *pConn);
	virtual ~C2PlatformDualtalk();

	
	S32 Start();	
	VOID Stop();
	BOOL SendDTalkMsg(const NET_DTALK_MSG_S *pParam, const NET_DTALK_USER_S *pUserList);
	BOOL SendDTalkData(U8 *pu8Data, U32 u32DataSize);
	
	/* 开始捕获 */
	BOOL StartCapture();

	//friend void C2DevTalkStatusCallback(int code, void* userdata);

private:
	C2PlatformConn* m_pConn;	//连接对象	
	HANDLE m_devTalkHandle;

};

#endif // !defined(AFX_C2PLATFORM_DUALTALK_H__28516A67_3168_4CB0_A28F_03662C79F204__INCLUDED_)
