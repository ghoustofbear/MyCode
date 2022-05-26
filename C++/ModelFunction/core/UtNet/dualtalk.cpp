#include "dualtalk.h"
#include "connection.h"

#ifdef _WIN32
#include "RSPlay.h"
#include "capture.h"

#endif

#define ENABLE_DUALTALK_DEBUG
#ifdef  ENABLE_DUALTALK_DEBUG 
#ifdef _WIN32
#define DUALTALK_DEBUG             printf
#else
#define DUALTALK_DEBUG(MSG...)     DEBUG("DUALTALK", MSG)
#endif /* end _WIN32 */
#else
#define DUALTALK_DEBUG(MSG...)
#endif /* end ENABLE_DUALTALK_DEBUG */

Dualtalk::Dualtalk(Connection *pConn)
    :BaseDualTalk()
{
	ASSERT(pConn);
	m_pConn = pConn;
	m_bExit   = FALSE;
	m_trdTalk = 0;
	m_bOpened = FALSE;
	
	m_fMsgCallback       = NULL;
	m_fDTalkDataCallback = NULL;
	m_pMsgUser           = NULL;
	m_iPlayerAudio       = -1;
	m_iPlayerVideo       = -1;
	m_bTalking           = FALSE;
	m_u32TalkMode        = 0;

	m_u32CollectFlag     = 0;
	m_hLocalWnd          = NULL;
	m_hRemoteWnd         = NULL;
	m_hMsgWnd            = NULL;
	m_u32TalkType        = 0;
	m_iLocalCamera       = -1;
}

Dualtalk::~Dualtalk()
{
	
}

S32 Dualtalk::Start()
{
	return StartDualtalkLoop();
}

VOID Dualtalk::Stop()
{
	m_bExit    = TRUE;
	m_bOpened  = FALSE;
	m_bTalking = FALSE;
	
	m_DTalkSock.KillSocket();
	ThreadJoin(m_trdTalk);
	
	StopMedia();
}

VOID Dualtalk::StopMedia()
{
#ifdef _WIN32
	if (-1 != m_iPlayerAudio)
	{
		RSPlayStopDualTalk(m_iPlayerAudio);
		RSPlayDestroyPlayInstance(m_iPlayerAudio);
		m_iPlayerAudio = -1;
	}	
	
	if (-1 != m_iPlayerVideo)
	{
		if (m_iLocalCamera == 0)
		{
			CAP_StopCapture(0);
			m_iLocalCamera = -1;
		}
	
		RSPlayDestroyPlayInstance(m_iPlayerVideo);
		m_iPlayerVideo = -1;
	}
#endif
}

VOID Dualtalk::SetDTalkParam(
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

BOOL Dualtalk::OpenStream()
{
	DTALK_REQ_S stReq;
	stReq.u32UserKey = m_pConn->GetUserKey();
	MsgHeadWrap msgHead(MSG_DUALTALK, MSG_NO_SUBTYPE, sizeof(stReq));
	msgHead.u2Version = m_pConn->m_u32MsgVersion;
	
	MsgBuf buf;
	if (m_DTalkSock.Socket(TRUE) && \
		m_DTalkSock.SetSendTimeOut(SEND_TIMEOUT) \
		&& m_DTalkSock.SetRecvTimeOut(RECV_TIMEOUT) \
		&& m_DTalkSock.SetSndBuf(512*1024) \
		&& m_DTalkSock.SetRecvBuf(256*1024) \
		&& m_DTalkSock.Connect(m_pConn->GetIP(), m_pConn->GetPort(), CONN_TIMEOUT, TRUE) \
		&& SendMsg(m_DTalkSock, msgHead, &stReq) \
		&& RecvMsg(m_DTalkSock, buf) \
		&& buf.ValidRsp(msgHead) \
		&& buf.IsSuccess())
	{
		if (m_fMsgCallback)
			m_fMsgCallback(DTALK_CONN_OK, NULL, m_pMsgUser);
		return TRUE;
	}
	
	m_DTalkSock.Close();
	return FALSE;
}

BOOL Dualtalk::SendDTalkData(U8 *pu8Data, U32 u32DataSize)
{
	if (!m_pConn->IsLogined() || !m_bOpened)
	{
		return FALSE;
	}
	
	if (!m_bTalking)
	{
		return FALSE;
	}
	
	MsgHeadWrap msgHead(MSG_DUALTALK, MSG_DTALK_DATA, u32DataSize);
	return SendMsg(m_DTalkSock, msgHead, pu8Data);
}

BOOL Dualtalk::StartCapture()
{
#ifdef _WIN32
	if (m_iLocalCamera == 0)
	{
		StopMedia();
		m_iLocalCamera = -1;
	}
	m_iLocalCamera = CAP_Init();
	if (m_iLocalCamera == 0)
	{
		m_iLocalCamera = CAP_StartCapture(0, m_hLocalWnd, CapDataCallback, 0, this);
		if (m_iLocalCamera == 0)
		{
			CAP_SetCapture(true);
		}
	}
	
#endif
	return TRUE;
}

BOOL Dualtalk::SendDTalkMsg(const NET_DTALK_MSG_S *pParam, const NET_DTALK_USER_S *pUserList)
{
	if (!m_pConn->IsLogined() || !m_bOpened)
	{
		return FALSE;
	}
	
	if (m_bTalking && 1 == pParam->u32MsgType)
	{
		if (m_fMsgCallback)
			m_fMsgCallback(DTALK_TALKINKG, NULL, m_pMsgUser);
		return FALSE;
	}
	
	if (!m_bTalking && 2 == pParam->u32MsgType)
	{
		return FALSE;
	}

	/**
	 * 被动方停止对讲方案，1、先推送一个结束帧，结束帧里边包含本用户信息 2、关闭对讲 
	 */
	if (m_bTalking && 
		2 == pParam->u32MsgType && 
		TALK_MODE_PASSIVE== m_u32TalkMode)
	{
		U8 u8CliInfo[sizeof(UT_FRAME_HEAD_S) + MAX_NAME_LEN + MAX_IP_STR_LEN] = {0};

		UT_FRAME_HEAD_S *pFrameHead = (UT_FRAME_HEAD_S *)u8CliInfo;
		pFrameHead->EncodeType          = ENCODE_TYPE_STD_H264;
		pFrameHead->VideoHead.FrameRate = 25;
		pFrameHead->VideoHead.Width     = 640;
		pFrameHead->VideoHead.Height    = 480;
		pFrameHead->VideoHead.System    = SYSTEM_PAL;
		pFrameHead->Time                = 0;
		pFrameHead->Pts                 = 0;
		pFrameHead->VideoHead.Reserved  = 0;	
		pFrameHead->FrameSize           = sizeof(u8CliInfo);
		pFrameHead->FrameType           = FRAME_END;

		memcpy(u8CliInfo + sizeof(UT_FRAME_HEAD_S), pUserList[0].ps8Name, MAX_NAME_LEN);
		memcpy(u8CliInfo + sizeof(UT_FRAME_HEAD_S) + MAX_NAME_LEN, pUserList[0].ps8Ip, MAX_IP_STR_LEN);
		SendDTalkData(u8CliInfo, sizeof(UT_FRAME_HEAD_S) + MAX_NAME_LEN + MAX_IP_STR_LEN);
		/* 先设置标记，后停止音视频播放 */
		m_bTalking = FALSE;
		StopMedia();

		return TRUE;
	}
	
	U32 u32MsgSize = sizeof(DTALK_MSG_S) + pParam->u32UserNum * sizeof(DTALK_USER_S);
	U8 *pu8Msg = new U8[u32MsgSize];
	
	DTALK_MSG_S  *pstDTalkMsg  = (DTALK_MSG_S *)pu8Msg;
	DTALK_USER_S *pstDTalkUser = (DTALK_USER_S *)(pu8Msg + sizeof(DTALK_MSG_S));
	
	strncpy(pstDTalkMsg->s8LocalName, pParam->ps8LocalName, MAX_NAME_LEN);
	INET_PTON(pParam->ps8LocalIp, pstDTalkMsg->u8LocalIp);
	pstDTalkMsg->u32UserSize = pParam->u32UserNum * sizeof(DTALK_USER_S);
	
	pstDTalkMsg->u8ReqType  = pUserList[0].ps8UserAddrCode ? 1 : 0;
	pstDTalkMsg->u8TalkType = pParam->u8TalkType;
	m_u32TalkType           = pParam->u8TalkType;
	S32 s32I = 0;
	for (s32I = 0; s32I < pParam->u32UserNum; s32I++)
	{
		if (pstDTalkMsg->u8ReqType)
		{
			strncpy(pstDTalkUser->s8UserAddrCode, pUserList->ps8UserAddrCode, MAX_ADDR_LEN);
		}
		else
		{
			strncpy(pstDTalkUser->s8Name, pUserList->ps8Name, MAX_NAME_LEN);
			INET_PTON(pUserList->ps8Ip, pstDTalkUser->u8Ip);
		}
		pstDTalkUser++;
		pUserList++;
	}
	
	WORD wSubType = MSG_NO_SUBTYPE;
	switch (pParam->u32MsgType)
	{
	case 1:
		wSubType = MSG_DTALK_REQ;
		break;
	case 2:
		wSubType = MSG_DTALK_STOP;
		/* 先关闭数据发送，再发消息 */
		StopMedia();
		break;
	default:
		break;
	} 
	
	MsgBuf msgRsp;
	MsgHeadWrap msgHead(MSG_DUALTALK, wSubType, u32MsgSize);
	msgHead.u2Version = m_pConn->m_u32MsgVersion;
	
	if (wSubType == MSG_DTALK_REQ)
	{
		BOOL bRet = Transaction(msgHead, pu8Msg, msgRsp, 15000);
		if (!bRet)
		{
			if (m_fMsgCallback )
				m_fMsgCallback(DTALK_NO_ACK, NULL, m_pMsgUser);
			if (pu8Msg)
			{
				delete []pu8Msg;
				pu8Msg = NULL;
			}
			
			return FALSE;
		}
	}
	else
	{
		SendReq(msgHead, pu8Msg);             //直接下发关闭命令
		m_bTalking = FALSE;
		delete []pu8Msg;
		return TRUE;
	}
	
	if (ACK_DTALK_OK == msgRsp.GetAckFlag())
	{
		DTALK_MSG_S *pDTalkMsg = (DTALK_MSG_S *)msgRsp.GetData();
		if (MSG_DTALK_REQ == wSubType)
		{
			if (m_fMsgCallback)
				m_fMsgCallback(DTALK_ACK, pDTalkMsg, m_pMsgUser);
			m_u32TalkMode = TALK_MODE_ACTIVE;
			m_bTalking    = TRUE;
			
#ifdef _WIN32
			if ((0x1 & m_u32CollectFlag) && 
				(0 == m_u32TalkType ||
				2 == m_u32TalkType ||
				3 == m_u32TalkType ||
				5 == m_u32TalkType)
				)
			{
				if (m_iPlayerAudio == -1)
				{
					m_iPlayerAudio = RSPlayCreatePlayInstance(PLAY_DUALTALK_TYPE);
					AudioTalkConfig_t talk;
					talk.iAmr_format = 0;
					talk.iAmr_mode = 0;
					talk.iFormat = 1;
					talk.iRate = 8000;
					talk.iWidth = 16;
					RSPlayStartDualTalk(m_iPlayerAudio, &talk);
					RSPlayRegisterSetDualCallback(m_iPlayerAudio, DualSpeakCallback, (DWORD)this);
				}
			}

			if ((0x2 & m_u32CollectFlag) &&
				(1 == m_u32TalkType ||
				2 == m_u32TalkType ||
				4 == m_u32TalkType ||
				5 == m_u32TalkType)
				)
			{
				if (m_iPlayerVideo == -1)
				{
					m_iPlayerVideo = RSPlayCreatePlayInstance(PLAY_NETSTREAM_TYPE);
					RSPlaySetPlayWnd(m_iPlayerVideo, m_hRemoteWnd);
					RSPlayStartPlay(m_iPlayerVideo);

					m_iLocalCamera = CAP_Init();
					if (m_iLocalCamera == 0)
					{
						m_iLocalCamera = CAP_StartCapture(0, m_hLocalWnd, CapDataCallback, 0, this);
						if (m_iLocalCamera == 0)
						{
							CAP_SetCapture(true);
						}					    
					}			
				}
			}	
#endif
		}
		else
		{
			m_u32TalkMode = 0;
			m_bTalking    = FALSE;
		}
	}
	else if (ACK_DTALK_NAK == msgRsp.GetAckFlag())
	{
		DTALK_MSG_S *pDTalkMsg = (DTALK_MSG_S *)msgRsp.GetData();
		if (m_fMsgCallback)
			m_fMsgCallback(DTALK_NAK, pDTalkMsg, m_pMsgUser);
	}
	else if (ACK_DTALK_NO_ACK == msgRsp.GetAckFlag())
	{
		DTALK_MSG_S *pDTalkMsg = (DTALK_MSG_S *)msgRsp.GetData();
		if (m_fMsgCallback)
			m_fMsgCallback(DTALK_NO_ACK, pDTalkMsg, m_pMsgUser);
	}
	
	delete []pu8Msg;
	return TRUE;
}


BOOL Dualtalk::SendReq(const MsgHeadWrap &msgHead, VOID *reqData)
{
	return SendMsg(m_DTalkSock, msgHead, reqData);
}

#ifdef _WIN32
int CALLBACK DualSpeakCallback(int size, const char* data, DWORD dwUser)
{
	Dualtalk *pDTalk = (Dualtalk *)dwUser;
	/* 作为主动方 */
	if (Dualtalk::TALK_MODE_ACTIVE == pDTalk->m_u32TalkMode)
	{
		pDTalk->SendDTalkData((U8 *)data, size);
	}
	else
	{
		pDTalk->SendDTalkData((U8 *)data, size);
		/* 作为被动方 */
	}
	
	return 0;
}

VOID CapDataCallback(long lDataType, BYTE *pData, long lDataSize, VOID *pUser)
{
	if (lDataSize <= 0)
	{
		return;
	}

	Dualtalk *pDTalk = (Dualtalk *)pUser;

	U8 u8Frame[512*1024] = {0};
	UT_FRAME_HEAD_S *pFrameHead = (UT_FRAME_HEAD_S *)u8Frame;

	pFrameHead->EncodeType          = ENCODE_TYPE_STD_H264;
	pFrameHead->FrameTag            = FRAME_TAG_STD_H264;
	pFrameHead->VideoHead.FrameRate = 25;
	pFrameHead->VideoHead.Width     = 640;
	pFrameHead->VideoHead.Height    = 480;
	pFrameHead->VideoHead.System    = SYSTEM_PAL;
	pFrameHead->Time                = (U64)time(NULL)*1000;
	pFrameHead->Pts                 = 0;
	pFrameHead->VideoHead.Reserved  = 0;	
	pFrameHead->FrameSize           = lDataSize;

	if (0x67 == pData[4])
	{
		pFrameHead->FrameType = FRAME_I;
	}
	else
	{
		pFrameHead->FrameType = FRAME_P;
	}

	memcpy(u8Frame + sizeof(UT_FRAME_HEAD_S), pData, lDataSize);
	/* 作为主动方 */
	if (Dualtalk::TALK_MODE_ACTIVE == pDTalk->m_u32TalkMode)
	{
		
		pDTalk->SendDTalkData(u8Frame, lDataSize + sizeof(UT_FRAME_HEAD_S));
	}
	else
	{
		pDTalk->SendDTalkData(u8Frame, lDataSize + sizeof(UT_FRAME_HEAD_S));
		/* 作为被动方 */
	}
}
#endif

VOID Dualtalk::DualtalkLoop()
{
	do 
	{
		m_bOpened = FALSE;
		if (!m_pConn->IsLogined() || !OpenStream())
		{
			Msleep(100);
			continue;
		}
		
		m_bOpened = TRUE;
		int   timeoutNum = 0;
		time_t tStart    = time(NULL);
		while(!m_bExit)
		{
			BOOL timeout = FALSE;
			if (m_DTalkSock.Select(FALSE, 5000, &timeout))
			{
				MsgBuf msgBuf;
				if (RecvMsg(m_DTalkSock, msgBuf))
				{
					timeoutNum = 0;
					switch (msgBuf.GetMsgSubType())
					{
					case MSG_DTALK_HBEAT:
						break;
						
					case MSG_DTALK_REQ:
						if (ACK_SUCCESS == msgBuf.GetAckFlag())
						{
							DTALK_MSG_S *pDTalkMsg = (DTALK_MSG_S *)msgBuf.GetData();
							m_u32TalkType = pDTalkMsg->u8TalkType;

							if (3 == m_u32TalkType ||
								4 == m_u32TalkType ||
								5 == m_u32TalkType)
							{
								/* 喊话不询问，直接回ACK */
								msgBuf.u16AckFlag = ACK_DTALK_OK;
								m_u32TalkMode = TALK_MODE_PASSIVE;
								m_bTalking    = TRUE;
							}
							else
							{	
								S8 s8FromIp[MAX_IP_STR_LEN] = {0};
								INET_NTOP(pDTalkMsg->u8LocalIp, s8FromIp);
								/* 被动请求消息 */
								if (m_fMsgCallback &&
									m_fMsgCallback(DTALK_REQ, pDTalkMsg, m_pMsgUser) < 0)
								{
									msgBuf.u16AckFlag = ACK_DTALK_NAK;
		
								}
								else
								{
									msgBuf.u16AckFlag = ACK_DTALK_OK;
									m_u32TalkMode = TALK_MODE_PASSIVE;
									m_bTalking    = TRUE;
								}
							}
							msgBuf.u32MsgSize = 0;
							msgBuf.u6ExtSize  = 0;
							SendMsg(m_DTalkSock, msgBuf);
						}
						else
						{
							/* 主动请求回应消息 */
							m_MsgTransQueue.Rsp(msgBuf);
						}
						break;
					case MSG_DTALK_STOP:
						if (ACK_SUCCESS == msgBuf.GetAckFlag())
						{
							StopMedia();
							if (3 == m_u32TalkType ||
								4 == m_u32TalkType ||
								5 == m_u32TalkType)
							{
							}
							else
							{
								DTALK_MSG_S *pDTalkMsg = (DTALK_MSG_S *)msgBuf.GetData();
								S8 s8FromIp[MAX_IP_STR_LEN] = {0};
								INET_NTOP(pDTalkMsg->u8LocalIp, s8FromIp);
								
								/* 被动推送消息 */
								if (m_fMsgCallback &&
									m_fMsgCallback(DTALK_STOP, pDTalkMsg, m_pMsgUser) < 0)
								{
								}
							}
												
							m_bTalking = FALSE;
						}
						
						break;
						
					case MSG_DTALK_DATA:
						{
							if (m_fDTalkDataCallback)
							{
								m_fDTalkDataCallback(m_hRemoteWnd, (U8 *)msgBuf.GetData(), msgBuf.GetDataLen(), m_pMsgUser);
								//break;
							}

							if (!m_bTalking)
							{
								break;
							}
							
							UT_FRAME_HEAD_S *pFrameHead = (UT_FRAME_HEAD_S *)msgBuf.GetData();
							
							if (FRAME_A == pFrameHead->FrameType)
							{
#ifdef _WIN32
								if (m_iPlayerAudio == -1)
								{
									m_iPlayerAudio = RSPlayCreatePlayInstance(PLAY_DUALTALK_TYPE);
									AudioTalkConfig_t talk;
									talk.iAmr_format = 0;
									talk.iAmr_mode = 0;
									talk.iFormat = 1;
									talk.iRate = 8000;
									talk.iWidth = 16;
									RSPlayStartDualTalk(m_iPlayerAudio, &talk);
									
									if (3 == m_u32TalkType ||
										4 == m_u32TalkType ||
										5 == m_u32TalkType)
									{
										RSPlayRegisterSetDualCallback(m_iPlayerAudio, NULL, (DWORD)this);
									}
									else
									{
										RSPlayRegisterSetDualCallback(m_iPlayerAudio, DualSpeakCallback, (DWORD)this);
									}
								}
								else
								{			
									RSPlayInputNetFrame(m_iPlayerAudio, msgBuf.GetData(), msgBuf.GetDataLen());
								}
#endif
							}
							else if (FRAME_END == pFrameHead->FrameType)
							{
								StopMedia();
								if (m_fMsgCallback)
								{
									S8 *ps8FromName = (S8 *)msgBuf.GetData() + sizeof(UT_FRAME_HEAD_S);
									S8 *ps8FromIp   = (S8 *)msgBuf.GetData() + sizeof(UT_FRAME_HEAD_S) + MAX_NAME_LEN;
									DTALK_MSG_S stDTalkMsg = {0};
									stDTalkMsg.u8ReqType  = (U32)(strlen(ps8FromName) == 18);
									strncpy(stDTalkMsg.s8LocalName, ps8FromName, MAX_NAME_LEN);
									INET_PTON(ps8FromIp, stDTalkMsg.u8LocalIp);
									m_fMsgCallback(DTALK_STOP, &stDTalkMsg, m_pMsgUser);
								}
								m_bTalking = FALSE;						
							}
							else
							{
#ifdef _WIN32
#if 0
								static FILE *fVideo = NULL;
								if(fVideo == NULL)
									fVideo = fopen("dualtalk.h264","wb");
								
								fwrite((char *)msgBuf.GetData() + 40, msgBuf.GetDataLen() - 40, 1, fVideo);
#endif
								if (m_iPlayerVideo == -1)
								{
									m_iPlayerVideo = RSPlayCreatePlayInstance(PLAY_NETSTREAM_TYPE);
									RSPlaySetPlayWnd(m_iPlayerVideo, m_hRemoteWnd);
									RSPlayStartPlay(m_iPlayerVideo);
									RSPlayInputNetFrame(m_iPlayerVideo, msgBuf.GetData(), msgBuf.GetDataLen());
								
									PostMessage(m_hMsgWnd, WM_VIDEO_CAPTURE, MSG_CAP_START, 0);
// 									CAP_Init();
// 									CAP_StartCapture(0, m_hLocalWnd, CapDataCallback, 0, this);
// 									CAP_SetCapture(true);
								}
								else
								{
									RSPlayInputNetFrame(m_iPlayerVideo, msgBuf.GetData(), msgBuf.GetDataLen());
								}
#endif
							}
						}
						break;
						
					default :
						m_MsgTransQueue.Rsp(msgBuf);
						break;
					}
					
					/** 
					*	如果服务器一直在发信息（如一直推送音视频数据），
					*  这时timeout很长时间都不会变true，因而发不出心跳，
					*  导致服务器会超时断开 
					*/
					time_t tEclipse = time(NULL);
                    S64 temppp = tEclipse - tStart;
					if (MSG_DTALK_DATA == msgBuf.GetMsgSubType() && 
                        abs(temppp) > 5)
					{
						timeout = TRUE;
						tStart  = tEclipse;
					}
					else
					{		
						continue;
					}
				}		
			}
			
			if (timeout)
			{
				MsgHeadWrap heartbeat(MSG_DUALTALK, MSG_DTALK_HBEAT);
				heartbeat.u2Version = m_pConn->m_u32MsgVersion;
				if (++timeoutNum < 3 && SendMsg(m_DTalkSock, heartbeat))
				{
					DUALTALK_DEBUG("Send dualtalk heatbeat\n");
					continue;
				}
			}
			
			break;
		}
		m_DTalkSock.Close();
		
		StopMedia();
		m_bTalking = FALSE;
			
	} while (!m_bExit);
}

S32 Dualtalk::StartDualtalkLoop()
{
	S32 s32Ret = CreateJoinableThread(ThreadEntry, this, &m_trdTalk);
	if (s32Ret < 0)
	{
		return -1;
	}
	
	return 0;
}

VOID *Dualtalk::ThreadEntry(VOID *pArg)
{
	ASSERT( pArg );
	Dualtalk *pTalk = (Dualtalk *)pArg;
	
	pTalk->DualtalkLoop();
	
	return NULL;	
}
