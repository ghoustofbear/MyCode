// c2platform_devTalk.cpp: implementation of the c2platform_devTalk class.
//
//////////////////////////////////////////////////////////////////////

#include "c2platform_devTalk.h"
#include "c2platform_conn.h"
#include "c2platform_preview.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern HANDLE m_hLogin;

C2PlatformDevTalk::C2PlatformDevTalk(C2PlatformConn *pConn)
{
	m_devTalkHandle = NULL;
	m_pConn = pConn; 
}

C2PlatformDevTalk::~C2PlatformDevTalk()
{

}

//************************************************************************
//函数名称：C2StatusCallback(int code, void* userdata)
//函数说明：开始对讲状态回调
//函数参数：
//    (in) code 状态码
//    (in) userdata 用户数据
//返回值：void
//************************************************************************/
void C2DevTalkStatusCallback(int code, void* userdata)
{
	C2PlatformDevTalk *pDevTalk = (C2PlatformDevTalk *)userdata;	
}

//************************************************************************
//函数名称：IniDevTalk(int audioType)
//函数说明：初始化设备对讲
//函数参数： 
//    (in) audioType 音频类型 
//返回值：成功返回TRUE/失败返回FALSE
//************************************************************************/
BOOL C2PlatformDevTalk::InitDevTalk(int audioType)
{
	if(NULL != m_pConn)
	{
		m_devTalkHandle = c2_init_dualtalk(audioType);
		return TRUE;
	}
	return FALSE;
}

//************************************************************************
//函数名称：UninitDevTalk()
//函数说明：反初始化设备对讲
//函数参数：无
//返回值：成功返回TRUE/失败返回FALSE
//************************************************************************/
BOOL C2PlatformDevTalk::UninitDevTalk()
{
	if(NULL != m_pConn)
	{
		int iRet = c2_uninit_dualtalk(m_devTalkHandle);
		if(0 == iRet)
		{
			return TRUE;
		}
	}
	return FALSE;
}

//************************************************************************
//函数名称：StartDevTalk(const NET_DTALK_DEV_LIST *pParams)
//函数说明：发送对话请求
//函数参数： 
//    (in) pDevList 对话参数列表
//返回值：成功返回TRUE/失败返回FALSE
//************************************************************************/
BOOL C2PlatformDevTalk::StartDevTalk(const NET_DTALK_DEV_LIST *pParam)
{	
	if((NULL != m_pConn) && (NULL != pParam))
	{
		vector<C2PlatformPreview *> vcPrev = m_pConn->GetPreview();

		for(int i=0; i<pParam->u32DevListNum; i++)
		{
			char* ip = pParam->pDevList[i].ps8Ip;
			int port = pParam->pDevList[i].u16Port;
			int chnNo = pParam->pDevList[i].u8ChnNo;
			
			for(int j=0; j<vcPrev.size(); j++)  //查询是否已存在的视频通道
			{
				if((0 == strcmp(ip, vcPrev[j]->m_prevParam.deviceHost)) &&
					(port == vcPrev[j]->m_prevParam.devicePort) &&
					(chnNo == vcPrev[j]->m_prevParam.channelNo))
				{
					HANDLE hRet = c2_start_dev_dualtalk(m_devTalkHandle, vcPrev[j]->m_hPreview, C2DevTalkStatusCallback, this); 
					if(NULL != hRet)
					{
						return TRUE;
					}					
				}
			}		
			
		}
	}	
	return FALSE;
}

//************************************************************************
//函数名称：StartDevTalk(const NET_DTALK_DEV_LIST *pParams)
//函数说明：发送对话请求
//函数参数： 
//    (in) pParam 对话参数列表
//返回值：成功返回TRUE/失败返回FALSE
//************************************************************************/
BOOL C2PlatformDevTalk::StopDevTalk(const NET_DTALK_DEV_LIST *pParam)
{
	if((NULL != m_pConn) && (NULL != pParam))
	{
		vector<C2PlatformPreview *> vcPrev = m_pConn->GetPreview();
		
		for(int i=0; i<pParam->u32DevListNum; i++)
		{
			char* ip = pParam->pDevList[i].ps8Ip;
			int port = pParam->pDevList[i].u16Port;
			int chnNo = pParam->pDevList[i].u8ChnNo;
			
			for(int j=0; j<vcPrev.size(); j++)  //查询是否已存在的视频通道
			{
				if((0 == strcmp(ip, vcPrev[j]->m_prevParam.deviceHost)) &&
					(port == vcPrev[j]->m_prevParam.devicePort) &&
					(chnNo == vcPrev[j]->m_prevParam.channelNo))
				{
					int iRet = c2_stop_dev_dualtalk(m_devTalkHandle, vcPrev[j]->m_hPreview); 
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

