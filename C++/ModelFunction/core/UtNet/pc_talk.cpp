// PcTalk.cpp: implementation of the PcTalk class.
//
//////////////////////////////////////////////////////////////////////

#include "pc_talk.h"
#include "g711c1.h"
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////
TalkClient::TalkClient()
{
	m_s32TalkSockFd = -1;
	m_pS8TalkClietIp = NULL;
}

TalkClient::~TalkClient()
{

}

void TalkClient::Init()
{

}

VOID* TalkClient::ThreadFun(VOID *pArg)
{
	ASSERT( pArg );
	THREAD_ARG_S *pThreadArg = (THREAD_ARG_S *)pArg;
	TalkClient *pTalkClient = (TalkClient *)pThreadArg->pClass;
	U8 u8Func = pThreadArg->u8Func;
	
// 	switch ( u8Func )
// 	{		
// 	default:
// 		break;
// 	}
	delete pThreadArg;	
	
	return NULL;
}

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

PcTalk::PcTalk()
{
	m_szLocalIp = "";
	m_u16LocalPort = 0;
	m_szRemoteIP = "";
	m_u16RemotePort = 0;
	m_pTalkUser = NULL;
	m_pClient = NULL;
	m_pServer = NULL;
	m_s32SockFd = -1;
}

PcTalk::~PcTalk()
{
	if (m_pClient)
	{
		delete m_pClient;
		m_pClient = NULL;
	}
	if (m_pServer)
	{
		delete m_pServer;
		m_pServer = NULL;
	}
}

/** 
 * @brief  初始化对讲连接
 *
 * @param  [in] u8Type  类型：server, client
 * @param  [in] pParam  对讲参数
 *
 * @return 成功：TRUE，失败：FALSE
 *
 * @see    
 */
BOOL PcTalk::IniPcTalk(U8 u8Type, NET_PCTALK_PARAM_S * pParam)
{
	m_fPcTalkCallback = pParam->fPcTalkCallback;
	m_szName = pParam->s8LocalName;

	if (PCTALK_SERVER == u8Type)//server
	{
		if (m_pServer==NULL)
		{
			m_pServer = new UtNetServer;
			m_pServer->m_pPcTalk = this;
			m_pServer->Start(pParam->u16LocalPort);
		}
	}
	else if (PCTALK_CLIENT == u8Type)//client
	{
		if (m_pClient==NULL)
		{
			m_pClient = new UtNetClient;
			m_pClient->m_pPcTalk = this;
			
			if (!m_pClient->TCPConnect(pParam->s8RemoteIp, pParam->u16RemotePort))
			{
				return FALSE;
			}
			m_pClient->Start();
		}
	}

	SetNetParam(pParam->s8LocalIp, pParam->u16LocalPort, pParam->s8RemoteIp, pParam->u16RemotePort);
		
	m_pTalkUser = pParam->pUser;

	return TRUE;
}

/** 
 * @brief  停止对讲
 *
 * @param  无
 *
 * @return 无
 *
 * @see    
 */
VOID PcTalk::StopTalk(S32 s32SockFd)
{
	if (m_pClient)
	{
		m_pClient->Stop();
	}
	if (m_pServer)
	{
		m_pServer->Stop(s32SockFd);
	}
}

/** 
 * @brief  发送对讲消息
 *
 * @param  [in] u8Type  类型：1-请求，2-ACK,3-NAK,4-音频数据，5-停止
 * @param  [in] pData   数据
 * @param  [in] u32Size 数据长度
 *
 * @return 无
 *
 * @see    
 */
VOID PcTalk::SendPcTalkMsg(U8 u8Type, U8 *pData, U32 u32Size)
{
	U8 u8TalkType = 1;
	U32 u32Code = MSG_PCTALK_REQUEST;
	U32 u32SockFd = -1;
	U8 bufIp[MAX_IPV4_LEN] = {0};
	NET_PCTALK_MASHEAD * pMsgHead = (NET_PCTALK_MASHEAD*)pData;

	switch(u8Type)
	{
	case PCTALK_REQUEST://
		SendRequest(pData, u32Size);
		break;
	case PCTALK_ACK://
		SendAck(pData, u32Size);
		break;
	case PCTALK_NAK://
		SendNak(pData, u32Size);
		StopTalk(pMsgHead->u32SockFd);
		break;
	case PCTALK_DATA://
		{
			U8 encBuf[100000] = {0};
			memcpy(encBuf, pMsgHead, sizeof(NET_PCTALK_MASHEAD));
			encode_g711a_audio_data(encBuf+sizeof(NET_PCTALK_MASHEAD), (unsigned char*)pMsgHead+sizeof(NET_PCTALK_MASHEAD), pMsgHead->u32DataLen);
			U32 encLen = pMsgHead->u32DataLen/2;
			SendAudioData((U8*)encBuf, encLen+sizeof(NET_PCTALK_MASHEAD));
		}
		break;
	case PCTALK_STOP://
		SendStop(pData, u32Size);
		StopTalk(pMsgHead->u32SockFd);
		break;
		
	}
	
}


/** 
 * @brief  获取在线客户信息
 *
 * @param  [in] buf  对讲消息
 *
 * @return 无
 *
 * @see    
 */
VOID PcTalk::ProcessPcTalk(MsgBuf &buf, S32 s32SockFd)
{
	
	U16 u16MsgSubType = buf.GetMsgSubType();
	U8 u8TalkMsgType = 0;
	m_s32SockFd = s32SockFd;
	switch(u16MsgSubType)
	{
	case MSG_PCTALK_REQUEST:
		u8TalkMsgType = PCTALK_REQUEST;
		break;
	case MSG_PCTALK_ACK:
		u8TalkMsgType = PCTALK_ACK;
		break;
	case MSG_PCTALK_NAK:
		u8TalkMsgType = PCTALK_NAK;
		StopTalk(s32SockFd);
		break;
	case MSG_PCTALK_AUDIO:
		u8TalkMsgType = PCTALK_DATA;
		break;
	case MSG_PCTALK_STOP:
		u8TalkMsgType = PCTALK_STOP;
		StopTalk(s32SockFd);
		break;
	case MSG_PCTALK_OFFLINE:
		u8TalkMsgType = PCTALK_OFFLINE;
		StopTalk(s32SockFd);
		break;
	}
	U32 u32Size = buf.GetDataLen();
	if (m_fPcTalkCallback)
	{
		if (MSG_PCTALK_AUDIO == u16MsgSubType)
		{
			U8 decBuf[100000] = {0};
			NET_PCTALK_MASHEAD* pMsgHead = (NET_PCTALK_MASHEAD*)buf.GetData();
			memcpy(decBuf, pMsgHead, sizeof(NET_PCTALK_MASHEAD));			
			decode_g711a_audio_data(decBuf+sizeof(NET_PCTALK_MASHEAD), (unsigned char*)buf.GetData()+sizeof(NET_PCTALK_MASHEAD), pMsgHead->u32DataLen/2);
			U32 u32DecLen = pMsgHead->u32DataLen + sizeof(NET_PCTALK_MASHEAD);

			m_fPcTalkCallback(u8TalkMsgType, s32SockFd, decBuf, u32DecLen, m_pTalkUser);
			
		}
		else if (MSG_PCTALK_OFFLINE == u16MsgSubType)
		{
			ProcessOffLine(s32SockFd);		
		}
		else
			m_fPcTalkCallback(u8TalkMsgType, s32SockFd, buf.GetData(), u32Size, m_pTalkUser);
	}
}

VOID PcTalk::ProcessOffLine(S32 s32SockFd)
{
	U8 u8TalkMsgType = PCTALK_OFFLINE;
	U8 buf[100000] = {0};
	NET_PCTALK_MASHEAD* pMsgHead = new NET_PCTALK_MASHEAD;
	if (pMsgHead)
	{
		pMsgHead->u32SockFd = s32SockFd;
		strcpy(pMsgHead->s8LocalName, m_szName.c_str());
		INET_PTON(m_szRemoteIP.c_str(), pMsgHead->u8LocalIp);
		INET_PTON(m_szLocalIp.c_str(), pMsgHead->u8RemoteIp);
		pMsgHead->u32DataLen = 0;
		
		memcpy(buf, pMsgHead, sizeof(NET_PCTALK_MASHEAD));
		m_fPcTalkCallback(u8TalkMsgType, s32SockFd, buf, sizeof(NET_PCTALK_MASHEAD), m_pTalkUser);
		
		delete pMsgHead;
	}
		
}

/** 
 * @brief  设置网络参数
 *
 * @param  [in] szLocalIp     本地IP
 * @param  [in] u16LocalPort  本地端口
 * @param  [in] szRemoteIp    远方IP
 * @param  [in] u16RemotePort 远方端口
 *
 * @return 无
 *
 * @see    
 */
void PcTalk::SetNetParam(string szLocalIp, U16 u16LocalPort, string szRemoteIp, U16 u16RemotePort)
{
	m_szLocalIp = szLocalIp;
	m_u16LocalPort = u16LocalPort;
	m_szRemoteIP = szRemoteIp;
	m_u16RemotePort = u16RemotePort;
}

/** 
 * @brief  发送音频数据
 *
 * @param  [in] pData          对讲数据包
 * @param  [in] u32DataLength  数据包长度
 *
 * @return 无
 *
 * @see    
 */
void PcTalk::SendAudioData(U8 *pData, U32 u32DataLength)
{
	WORD  wSubType = MSG_PCTALK_AUDIO;
	NET_PCTALK_MASHEAD* pDataHead = (NET_PCTALK_MASHEAD*)pData;
	U32 u32SockFd = pDataHead->u32SockFd;

	S8 bufSend[10000] = {0};
	MSG_HEADER_S *pMsgHead = (MSG_HEADER_S *)bufSend;
	pMsgHead->u16MsgType = MSG_PC_TALK;
	pMsgHead->u16MsgSubType = wSubType;
	pMsgHead->u32MsgSize = u32DataLength;
	pMsgHead->u2Version = MsgHeadV1;

	memcpy(bufSend+sizeof(MSG_HEADER_S), pData, u32DataLength);

	if (m_pClient && m_pClient->m_s32TcpConnFd!=-1)
	{
		m_pClient->SendNetMsg(m_pClient->m_s32TcpConnFd, (S8*)&bufSend, 3);
	}
	else if (m_pServer)
	{
		if (u32SockFd!=-1)
		{
			m_pServer->SendNetMsg(u32SockFd, (S8*)&bufSend, 3);
		}
	}
	

}

/** 
 * @brief  发送对讲申请报文
 *
 * @param  [in]u8Type 对讲类型：1-对讲，2-喊话
 *
 * @return 无
 *
 * @see    
 */
void PcTalk::SendRequest(U8 *pData, U32 u32DataLength)
{
	WORD  wSubType = MSG_PCTALK_REQUEST;

	S8 bufSend[1000] = {0};
	MSG_HEADER_S *pMsgHead = (MSG_HEADER_S *)bufSend;
	pMsgHead->u16MsgType = MSG_PC_TALK;
	pMsgHead->u16MsgSubType = wSubType;
	pMsgHead->u32MsgSize = u32DataLength;
	pMsgHead->u2Version = MsgHeadV1;
	
	memcpy(bufSend+sizeof(MSG_HEADER_S), pData, u32DataLength);

	if (m_pClient && m_pClient->m_s32TcpConnFd!=-1)
	{
		m_pClient->SendNetMsg(m_pClient->m_s32TcpConnFd, (S8*)&bufSend, 3);
	}
}

/** 
 * @brief  发送结束对讲报文
 *
 * @param  无
 *
 * @return 无
 *
 * @see    
 */
void PcTalk::SendStop(U8 *pData, U32 u32DataLength)
{
	WORD  wSubType = MSG_PCTALK_STOP;
	NET_PCTALK_MASHEAD* pDataHead = (NET_PCTALK_MASHEAD*)pData;
	U32 u32SockFd = pDataHead->u32SockFd;

	S8 bufSend[1000] = {0};
	MSG_HEADER_S *pMsgHead = (MSG_HEADER_S *)bufSend;
	pMsgHead->u16MsgType = MSG_PC_TALK;
	pMsgHead->u16MsgSubType = wSubType;
	pMsgHead->u32MsgSize = u32DataLength;
	pMsgHead->u2Version = MsgHeadV1;
	memcpy(bufSend+sizeof(MSG_HEADER_S), pData, u32DataLength);

	if (m_pClient && m_pClient->m_s32TcpConnFd!=-1)
	{	
		m_pClient->SendNetMsg(m_pClient->m_s32TcpConnFd, (S8*)&bufSend, 3);
	}
	else if (m_pServer)
	{		
		if (u32SockFd!=-1)
		{
			m_pServer->SendNetMsg(u32SockFd, (S8*)&bufSend, 3);
		}
	}

}

/** 
 * @brief  发送ACK(接收申请)
 *
 * @param  [in] u32Code     收到对方报文(即本帧所回复的报文)的命令码
 *
 * @return 无
 *
 * @see    
 */
void PcTalk::SendAck(U8 *pData, U32 u32DataLength)
{
	WORD  wSubType = MSG_PCTALK_ACK;
	NET_PCTALK_MASHEAD* pDataHead = (NET_PCTALK_MASHEAD*)pData;
	U32 u32SockFd = pDataHead->u32SockFd;

	S8 bufSend[1000] = {0};
	MSG_HEADER_S *pMsgHead = (MSG_HEADER_S *)bufSend;
	pMsgHead->u16MsgType = MSG_PC_TALK;
	pMsgHead->u16MsgSubType = wSubType;
	pMsgHead->u32MsgSize = u32DataLength;
	pMsgHead->u2Version = MsgHeadV1;
	memcpy(bufSend+sizeof(MSG_HEADER_S), pData, u32DataLength);
	
	if (m_pClient && m_pClient->m_s32TcpConnFd!=-1)
	{
		m_pClient->SendNetMsg(m_pClient->m_s32TcpConnFd, (S8*)&bufSend, 3);
	}
	else if (m_pServer)
	{
		S8 s8bufIp[MAX_IP_STR_LEN] = {0};
		INET_NTOP(pDataHead->u8RemoteIp, s8bufIp);
		m_szRemoteIP = s8bufIp;

		if (u32SockFd!=-1)
		{
			m_pServer->ClearRcvThread();
			m_pServer->SendNetMsg(u32SockFd, (S8*)bufSend, 3);
		}

	}

}


/** 
 * @brief  发送NAK(拒绝申请)
 *
 * @param  [in] u32Code  收到对方报文(即本帧所回复的报文)的命令码
 * @param  [in] u8Err    错误码(0-手动拒绝)  
 *
 * @return 无
 *
 * @see    
 */
void PcTalk::SendNak(U8 *pData, U32 u32DataLength)
{
	WORD  wSubType = MSG_PCTALK_NAK;
	NET_PCTALK_MASHEAD* pDataHead = (NET_PCTALK_MASHEAD*)pData;
	U32 u32SockFd = pDataHead->u32SockFd;
	S8 bufSend[1000] = {0};
	MSG_HEADER_S *pMsgHead = (MSG_HEADER_S *)bufSend;
	pMsgHead->u16MsgType = MSG_PC_TALK;
	pMsgHead->u16MsgSubType = wSubType;
	pMsgHead->u32MsgSize = u32DataLength;
	pMsgHead->u2Version = MsgHeadV1;
	memcpy(bufSend+sizeof(MSG_HEADER_S), pData, u32DataLength);

	if (m_pClient && m_pClient->m_s32TcpConnFd!=-1)
	{
		m_pClient->SendNetMsg(m_pClient->m_s32TcpConnFd, (S8*)&bufSend, 3);
	}
	else if (m_pServer)
	{
		S8 s8bufIp[MAX_IP_STR_LEN] = {0};
		INET_NTOP(pDataHead->u8RemoteIp, s8bufIp);
		m_szRemoteIP = s8bufIp;
		
		if (u32SockFd!=-1)
		{
			m_pServer->SendNetMsg(u32SockFd, (S8*)&bufSend, 3);
			m_pServer->StopRcv(u32SockFd);
		}		
	}
}

/** 
 * @brief  编辑客户端列表(增加/删除)
 *
 * @param  [in] s32SockFd      
 * @param  [in] pS8ClientIp      
 * @param  [in] bAdd
 *
 * @return 无
 *
 * @see    
 */
void PcTalk::EditClientList(S32 s32SockFd, S8 *pS8ClientIp, BOOL bAdd/* =TRUE */)
{
	if (bAdd)
	{
		TalkClient tkClient;
		tkClient.m_pS8TalkClietIp = pS8ClientIp;
		tkClient.m_s32TalkSockFd = s32SockFd;
		tkClient.Init();
		
		m_listTalkCliet.push_back(tkClient);
	}
	else
	{
		list<TalkClient>::iterator iter;
		for (iter=m_listTalkCliet.begin(); iter!=m_listTalkCliet.end(); iter++)
		{
			if (s32SockFd == iter->m_s32TalkSockFd && !strcmp(iter->m_pS8TalkClietIp, pS8ClientIp))
			{
				m_listTalkCliet.erase(iter);
				break;
			}
		}
	}
	
}

void PcTalk::ClearClientList()
{
	m_listTalkCliet.clear();
}