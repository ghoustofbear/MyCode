// c2platform_pcTalk.h: interface for the c2platform_pcTalk class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_C2PLATFORM_PCTALK_H__A71B44DE_647D_4BD8_BF83_CDFE6F32D7C6__INCLUDED_)
#define AFX_C2PLATFORM_PCTALK_H__A71B44DE_647D_4BD8_BF83_CDFE6F32D7C6__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "ut_net.h" 
#include "c2sdkc1.h"

class C2PlatformPcTalk  
{
public:
	VOID SendPcTalkMsg(U8 u8Type, U8 *pData, U32 u32Size);
	BOOL UnIniPcTalk();
	BOOL IniPcTalk(U8 u8Type, NET_PCTALK_PARAM_S * pParam);
	C2PlatformPcTalk();
	virtual ~C2PlatformPcTalk();

	friend void C2PcTalkStatusCallback(int code, void* userdata);

private:
 
	HANDLE m_hPCTalk;
};

#endif // !defined(AFX_C2PLATFORM_PCTALK_H__A71B44DE_647D_4BD8_BF83_CDFE6F32D7C6__INCLUDED_)
