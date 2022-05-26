#include "hikplatform_dualtalk.h"
#include "hikplatform_conn.h"

HikPlatformDualtalk::HikPlatformDualtalk(HikPlatformConn *pConn)
    :BaseDualTalk()
{
	ASSERT(pConn);
	m_pConn = pConn;
	m_bExit   = FALSE;
	m_trdTalk = 0;
	m_bOpened = FALSE;
}

HikPlatformDualtalk::~HikPlatformDualtalk()
{
	
}

VOID HikPlatformDualtalk::SetDTalkParam(
							 U32                 u32CollectFlag,
							 HWND                hLWnd,
							 HWND                hRWnd,
							 HWND                hMWnd,
							 pfnDTalkMsgCallback fDTalkMsgCallback, 
							 pfnRealDataCallback fDTalkDataCallback, 
							 VOID *pUser) 
{ 
	m_u32CollectFlag     = u32CollectFlag;
	m_hLocalWnd          = hLWnd;
	m_hRemoteWnd         = hRWnd;
	m_hMsgWnd            = hMWnd;
	m_fDTalkDataCallback = fDTalkDataCallback;
	m_fMsgCallback       = fDTalkMsgCallback; 
	m_pMsgUser           = pUser;
}

	/* �����Խ��ŵ� */
S32    HikPlatformDualtalk::Start()
{
	return StartDualtalkLoop();
}

	/* �رնԽ����� */
VOID   HikPlatformDualtalk::Stop()
{   

}


	/* ���ͶԽ���Ϣ */
BOOL HikPlatformDualtalk::SendDTalkMsg(const NET_DTALK_MSG_S *pParam, const NET_DTALK_USER_S *pUserList)
{
	return FALSE;
}

	/* ���ͶԽ����� */
BOOL HikPlatformDualtalk::SendDTalkData(U8 *pu8Data, U32 u32DataSize)
{
	return FALSE;
}

	/* ��ʼ���� */
BOOL HikPlatformDualtalk::StartCapture()
{
	return FALSE;
}

S32 HikPlatformDualtalk::StartDualtalkLoop()
{	
	return 0;
}

VOID *HikPlatformDualtalk::ThreadEntry(VOID *pArg)
{
	
	return NULL;	
}

VOID HikPlatformDualtalk::DualtalkLoop()
{
	
}


BOOL HikPlatformDualtalk::OpenStream()
{
    return TRUE;
}
