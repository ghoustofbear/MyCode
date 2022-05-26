// c2Platform_Dualtalk.cpp: implementation of the c2Platform_Dualtalk class.
//
//////////////////////////////////////////////////////////////////////

#include "c2Platform_Dualtalk.h"
#include "c2platform_conn.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C2PlatformDualtalk::C2PlatformDualtalk(C2PlatformConn *pConn)
:BaseDualTalk()
{
	m_pConn = pConn;
	
	
}

C2PlatformDualtalk::~C2PlatformDualtalk()
{

}

S32 C2PlatformDualtalk::Start()
{
	return 0;
}	

VOID C2PlatformDualtalk::Stop()
{

}

BOOL C2PlatformDualtalk::SendDTalkMsg(const NET_DTALK_MSG_S *pParam, const NET_DTALK_USER_S *pUserList)
{
	return FALSE;
}

BOOL C2PlatformDualtalk::SendDTalkData(U8 *pu8Data, U32 u32DataSize)
{
	return FALSE;
}

BOOL C2PlatformDualtalk::StartCapture()
{
	return FALSE;
}