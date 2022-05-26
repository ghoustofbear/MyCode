// c2platform_pcTalk.cpp: implementation of the c2platform_pcTalk class.
//
//////////////////////////////////////////////////////////////////////
 
#include "c2platform_pcTalk.h"
#include "c2platform_conn.h"

 
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

extern HANDLE m_hLogin;

C2PlatformPcTalk::C2PlatformPcTalk()
{	
	m_hPCTalk = NULL;
}

C2PlatformPcTalk::~C2PlatformPcTalk()
{

}


//************************************************************************
//函数名称：C2PcTalkStatusCallback(int code, void* userdata)
//函数说明：下载状态回调
//函数参数：
//    (in) code 状态码
//    (in) userdata 用户数据
//返回值：void
//************************************************************************/
void C2PcTalkStatusCallback(int code, void* userdata)
{	 
}

//************************************************************************
//函数名称：IniPcTalk(U8 u8Type, NET_PCTALK_PARAM_S *pParam)
//函数说明：初始化对讲设备
//函数参数：
//    (in) u8Type 类型
//    (in) pParam 对象信息
//返回值：成功返回TRUE/失败返回FALSE
//************************************************************************/
BOOL C2PlatformPcTalk::IniPcTalk(U8 u8Type, NET_PCTALK_PARAM_S *pParam)
{	
	return FALSE;
}

//************************************************************************
//函数名称：UnIniPcTalk()
//函数说明：反初始化对讲设备
//函数参数： 无
//返回值：成功返回TRUE/失败返回FALSE
//************************************************************************/
BOOL C2PlatformPcTalk::UnIniPcTalk()
{
	return FALSE;
}

//************************************************************************
//函数名称：SendPcTalkMsg(U8 u8Type, U8 *pData, U32 u32Size)
//函数说明：发送对讲消息
//函数参数： 
//    (in) u8Type 类型：1-请求，2-ACK,3-NAK,4-音频数据，5-停止
//    (in) pData 数据
//    (in) u32Size 数据长度
//返回值：void
//************************************************************************/
VOID C2PlatformPcTalk::SendPcTalkMsg(U8 u8Type, U8 *pData, U32 u32Size)
{ 
	if(NULL != m_hPCTalk)
	{
		HANDLE view = NULL; //aaaaaaaaaaaaaa;
		switch(u8Type)
		{
		case PCTALK_REQUEST://PC对讲请求
			{
				c2_start_dev_dualtalk(m_hPCTalk, view, C2PcTalkStatusCallback, this);
			}			
			break;
		case PCTALK_ACK://接收对讲请求
			 
			break;
		case PCTALK_NAK:// 拒绝对讲请求
			 
			break;
		case PCTALK_DATA://音频数据
			{
				//c2_send_audio(m_hPCTalk, pData, u32Size, 4, 8000, 1, 100, 100);//aaaaaaaaaa
			}
			break;
		case PCTALK_STOP://结束对讲
			{
				//c2_stop_dev_dualtalk(m_hPCTalk, pData); //aaaaaaaaaaaa
			}
			break;
		case PCTALK_OFFLINE:// 对讲断线
			
			break;
		}
	}
}
