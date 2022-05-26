// c2Platform_DualtalkEx.cpp: implementation of the c2Platform_DualtalkEx class.
//
//////////////////////////////////////////////////////////////////////

#include "c2Platform_DualtalkEx.h"
#include "c2platform_conn.h"
#include "c2platform_preview.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C2PlatformDualtalkEx::C2PlatformDualtalkEx(C2PlatformConn *pConn, int audioType)
:BaseDualTalkEx()
{
	m_pConn = pConn;
	m_iAudioType = audioType;
	m_devTalkExHandle = NULL;
}

C2PlatformDualtalkEx::~C2PlatformDualtalkEx()
{	
	Stop();
}


// 启动对讲信道 
S32 C2PlatformDualtalkEx::Start()
{
	if(NULL != m_pConn)
	{
		m_devTalkExHandle = c2_init_dualtalk(m_iAudioType);
		if (NULL != m_devTalkExHandle)
		{
			return 0;
		}
	}
	return -1;
}

// 关闭对讲链接 */
VOID C2PlatformDualtalkEx::Stop()
{
	if(NULL != m_pConn && NULL != m_devTalkExHandle)
	{
		int iRet = c2_uninit_dualtalk(m_devTalkExHandle);
		if(0 == iRet)
		{
			//return TRUE;
		}
	}
	//return FALSE;
}


//************************************************************************
//函数名称：C2DevTalkStatus_cb(int code, void* userdata)
//函数说明：开始对讲状态回调
//函数参数：
//    (in) code 状态码
//    (in) userdata 用户数据
//返回值：void
//************************************************************************/
void C2DevTalkStatus_cb(int code, void* userdata)
{
	C2PlatformDualtalkEx *pDevTalk = (C2PlatformDualtalkEx *)userdata;	
}


// 发送对讲申请 */
BOOL C2PlatformDualtalkEx::SendDTalkReq(const NET_DTALK_DEV_LIST * pDevList, U32 u32Type)
{
	if(NULL == m_pConn && NULL == m_devTalkExHandle)
	{
		return FALSE;
	}

	/* 开始对讲 */
	if (0 == u32Type)
	{		
		vector<C2PlatformPreview *> vcPrev = m_pConn->GetPreview();
		
		for(int i=0; i<pDevList->u32DevListNum; i++)
		{
			char* ip = pDevList->pDevList[i].ps8Ip;
			int port = pDevList->pDevList[i].u16Port;
			int chnNo = pDevList->pDevList[i].u8ChnNo;
			
			for(int j=0; j<vcPrev.size(); j++)  //查询是否已存在的视频通道
			{
				C2PlatformPreview * sl = vcPrev[j];
				if((0 == strcmp(ip, vcPrev[j]->m_PViewClient.stParam.ps8DeviceIp )) &&
					(port == vcPrev[j]->m_PViewClient.stParam.u16DevicePort) &&
					(chnNo == vcPrev[j]->m_PViewClient.stParam.u32ChnNO))
				{
					HANDLE hRet = c2_start_dev_dualtalk(m_devTalkExHandle, vcPrev[j]->m_hPreview, pDevList->devTalk_cb, pDevList->userdata); 
					if(NULL != hRet)
					{
						return TRUE;
					}					
				}
			}		
			
		}
	}
	/* 结束对讲 */
	else
	{
		vector<C2PlatformPreview *> vcPrev = m_pConn->GetPreview();
		
		for(int i=0; i<pDevList->u32DevListNum; i++)
		{
			char* ip = pDevList->pDevList[i].ps8Ip;
			int port = pDevList->pDevList[i].u16Port;
			int chnNo = pDevList->pDevList[i].u8ChnNo;
			
			for(int j=0; j<vcPrev.size(); j++)  //查询是否已存在的视频通道
			{
				if((0 == strcmp(ip, vcPrev[j]->m_PViewClient.stParam.ps8DeviceIp )) &&
					(port == vcPrev[j]->m_PViewClient.stParam.u16DevicePort) &&
					(chnNo == vcPrev[j]->m_PViewClient.stParam.u32ChnNO))
				{
					int iRet = c2_stop_dev_dualtalk(m_devTalkExHandle, vcPrev[j]->m_hPreview); 
					if(0 == iRet)
					{
						return TRUE;
					}					
				}
			}	
		}
	}
	
	return FALSE;
}

// 发送对讲数据 */
BOOL C2PlatformDualtalkEx::SendDTalkData(U8 *pu8Data, U32 u32DataSize)
{
	return FALSE;
}
