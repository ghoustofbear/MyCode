#include "commonc1.h"
#include "ut_netserver.h"
#include "pc_talk.h"


#define ENABLE_UTNETSERVER_DEBUG
#ifdef  ENABLE_UTNETSERVER_DEBUG 
#ifdef _WIN32
#define UTNETSERVER_DEBUG             printf 
#else
#define UTNETSERVER_DEBUG(MSG...)     DEBUG("UTNETSERVER", MSG)
#endif /* end _WIN32 */
#else
#define UTNETSERVER_DEBUG(MSG...)
#endif /* end ENABLE_UTNETSERVER_DEBUG */

UtNetServer::UtNetServer()
{
	m_s32TcpListenFd  = -1;
	m_trdTcpListen = 0;
	m_trdManager = 0;
//	m_trdRcvMsg = 0;
	m_bExit        = FALSE;

	m_u16TcpPort = 0;
	m_pPcTalk = NULL;
	m_bStopTalk = TRUE;
	m_bSendSuccess = TRUE;
	m_listConn.clear();
	UTNETSERVER_DEBUG(  "UtNetServer construct!\n" );
}

S32 UtNetServer::Start(U16 u16TcpPort)
{
	m_u16TcpPort = u16TcpPort;

	if (m_u16TcpPort>0)
	{
		TCPServStart();
	}
	
	return 0;
}

S32 UtNetServer::Stop(S32 s32SockFd)
{
	list<SERVER_CONN *>::iterator iter;
	for (iter=m_listConn.begin(); iter!=m_listConn.end(); iter++)
	{
		SERVER_CONN * pConn = *iter;
		if (s32SockFd == pConn->s32SockFd)
		{
			pConn->bStopTalk = TRUE;
			StopRcv(s32SockFd);
		}
	}
	return 0;
}

UtNetServer::~UtNetServer()
{
	Stop();

	m_bExit = TRUE;
	
	if (-1 != m_s32TcpListenFd)
	{
		close(m_s32TcpListenFd);
		m_s32TcpListenFd = -1;
	}
	
	if (m_trdTcpListen)
	{
		ThreadJoin(m_trdTcpListen);
		m_trdTcpListen = 0;
	}

	if (m_trdManager)
	{
		ThreadJoin(m_trdManager);
		m_trdManager = 0;
	}

	UTNETSERVER_DEBUG(  "UtNetServer distruct!\n" );
}

S32 UtNetServer::TCPServStart()
{
	THREAD_ARG_S *pThreadArg = new THREAD_ARG_S;

	/* 启动TCP监听服务 */
	pThreadArg->u8Func   = THREAD_TCPLISTEN;
	pThreadArg->pClass = this;
	pThreadArg->pArg   = NULL;
	if ( CreateJoinableThread( ThreadEntry, pThreadArg, &m_trdTcpListen ) < 0)
	{
		UTNETSERVER_DEBUG(  "start TCPServer failed!\n" );
		delete pThreadArg;
		return -1;
	}
	return 0;
}


S32 UtNetServer::TCPListen()
{
	S8  s8Serv[20] = {0};

#ifdef _WIN32
	_snprintf( s8Serv, sizeof(s8Serv), "%d", m_u16TcpPort );
#else
	snprintf( s8Serv, sizeof(s8Serv), "%d", m_u16TcpPort );
#endif

	m_s32TcpListenFd = TcpListen( NULL, s8Serv, NULL );

	AcceptProcess();

	return 0;
}

S32 UtNetServer::GetPeerIpAddr( struct sockaddr *pstPeerAddr, S32 s32SALen, S8 *ps8PeerIP, S8 *ps8PeerPort)
{
	ASSERT( ps8PeerIP );
	ASSERT( ps8PeerPort );

#ifdef _WIN32
#else
	if ( getnameinfo( pstPeerAddr, (socklen_t)s32SALen, ps8PeerIP, NI_MAXHOST, ps8PeerPort, 
				NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV) != 0  )
		return -1;
#endif

	return 0;
}

S32 UtNetServer::AcceptProcess()
{
	S32         s32Ret = -1;
	S32         s32SockFd;
	sockaddr_in stPeerAddr;
	S32         s32SALen = sizeof(stPeerAddr);
	S8          s8PeerIP[MAX_IP_STR_LEN]     = {0};
	S8          s8PeerPort[MAX_PORT_STR_LEN] = {0};
//	S8          s8PeerIP[NI_MAXHOST]     = {0};
//	S8          s8PeerPort[NI_MAXSERV] = {0};

	while (!m_bExit)
	{
		s32SockFd = accept( m_s32TcpListenFd, (sockaddr *)&stPeerAddr, (socklen_t *)&s32SALen );
		if ( s32SockFd < 0)
		{
			UTNETSERVER_DEBUG(  "accept: %s\n", strerror( errno ) );
			continue;
		}

		GetPeerIpAndPort(s32SockFd, s8PeerIP, MAX_IP_STR_LEN, s8PeerPort );

		RcvMsgProcess(s32SockFd, s8PeerIP);
		
// 		if (s32Ret < 0)
// 		{
// 			UTNETSERVER_DEBUG("accept error!\n");
// 			close( s32SockFd );
// 			s32SockFd = -1;
// 			return -1;
// 		}

	}
	return 0;
}

/** 
 * @brief  创建对讲消息接收和处理线程
 *
 * @param  [in] s32SockFd  socket
 * @param  [in] PeerIP     对方IP
 *
 * @return 成功：0，失败：-1
 *
 * @see    
 */
S32 UtNetServer::RcvMsgProcess(S32 s32SockFd, S8 *PeerIP)
{
	THREAD_ARG_S *pThreadArg = new THREAD_ARG_S;
	
	SERVER_CONN* pConn = new SERVER_CONN;
	pConn->s32SockFd = s32SockFd;
	pConn->s8PeerIp = PeerIP;
	pConn->u8Active = 1;
	pConn->bStopTalk = FALSE;
	pConn->bSendSuccess = TRUE;
	pConn->bufMsg.Init();
	
	THREAD threadId = 0;
	/* 启动TCP监听服务 */
	pThreadArg->u8Func = THREAD_RCVMSG;
	pThreadArg->pClass = this;
	pThreadArg->pArg   = pConn;
	
	m_bStopTalk = FALSE;
	if ( CreateJoinableThread( ThreadEntry, pThreadArg, &threadId ) < 0)
	{
		UTNETSERVER_DEBUG(  "start RvcProcess failed!\n" );
		delete pThreadArg;
		return -1;
	}
	pConn->ThreadID = threadId;
	m_listConn.push_back(pConn);
	return 0;
}

void UtNetServer::StopRcv(S32 s32SockFd)
{
	list<SERVER_CONN *>::iterator iter;
	for (iter=m_listConn.begin(); iter!=m_listConn.end(); iter++)
	{
		SERVER_CONN * pConn = *iter;
		if (s32SockFd == pConn->s32SockFd)
		{
			pConn->bStopTalk = TRUE;
// 			UTNETSERVER_DEBUG("Talk Server ThreadJoin in!\n");
// 			ThreadJoin(pConn->ThreadID);
// 			pConn->ThreadID = 0;
// 			UTNETSERVER_DEBUG("Talk Server ThreadJoin out!\n");
		}
	}
}

S32 UtNetServer::RcvMsgLoop(SERVER_CONN* pConn)
{
	S32 s32Ret = -1;
	S32 s32SockFd = pConn->s32SockFd;
	S8* pS8PeerIp = pConn->s8PeerIp;
	while (!pConn->bStopTalk)
	{
		if (s32SockFd==-1)
		{
			continue;
		}

		s32Ret =  RecvNetMsg(s32SockFd, (S8*)&(pConn->bufMsg), 5);
		if ( s32Ret < 0 )
		{
			MsgHeadWrap heartbeat(MSG_HEARTBEAT);
			heartbeat.u2Version = MsgHeadV1;
			

			if (-1 == SendNetMsg(s32SockFd, (S8*)&(heartbeat), 5))
			{
				UTNETSERVER_DEBUG("Server don't Rcv PcTalk Msg!\n");
				if (m_pPcTalk)
				{
					pConn->bufMsg.u16MsgType = MSG_PC_TALK;
					pConn->bufMsg.u16MsgSubType = MSG_PCTALK_OFFLINE;
					m_pPcTalk->ProcessPcTalk(pConn->bufMsg, s32SockFd);
				}
				
				Stop(s32SockFd);
				UTNETSERVER_DEBUG("Server RcvLoop out!\n");
				break;
			}
			else
				continue;
		}
		
		WORD wMsgType = pConn->bufMsg.GetMsgType();
		switch(wMsgType)
		{
			/* 心跳 */
		case MSG_HEARTBEAT:
			UTNETSERVER_DEBUG("Server Rcv heartbeat!\n");
			s32Ret = HeartBeatProcess(s32SockFd, (S8*)&pConn->bufMsg);
			break;
			/*  对讲 */
		case MSG_PC_TALK:
			UTNETSERVER_DEBUG("Server Rcv PcTalk Msg!\n");
			if (m_pPcTalk)
			{
				if (pConn->bufMsg.GetMsgSubType() == MSG_PCTALK_REQUEST)
				{
					m_pPcTalk->EditClientList(s32SockFd, pS8PeerIp);
				}
				if (pConn->bufMsg.GetMsgSubType() == MSG_PCTALK_STOP)
				{
					pConn->bStopTalk = TRUE;
					pConn->bSendSuccess = FALSE;
				}
				
				m_pPcTalk->ProcessPcTalk(pConn->bufMsg, s32SockFd);
			}
			break;
			
			
		default:
			s32Ret = -1;
			UTNETSERVER_DEBUG("unkown Msg type!\n");
			break;
		}

	}
	return 0;
}

/** 
 * @remarks  心跳请求处理
 * @return   成功0, 失败-1.
 */
S32 UtNetServer::HeartBeatProcess( S32 s32SockFd, S8 *pMsg )
{
	ASSERT( pMsg );

	MSG_HEADER_S *pMsgHeader = (MSG_HEADER_S *)pMsg;
	pMsgHeader->u16AckFlag = ACK_SUCCESS;
	pMsgHeader->u32MsgSize = 0;

	return SendNetMsg( s32SockFd, pMsg, 5);
}

/** 
 * @remarks    接收请求消息
 * @pMsg       消息内容地址 
 * @u32TimeOut 接收超时值,0为永久阻塞
 * @return     成功0, 失败-1.
 */
S32 UtNetServer::RecvNetMsg( S32 s32SockFd, S8 *pMsg, U32 u32TimeOut )
{
	ASSERT( pMsg );
	S32 s32Ret = -1;	

	if ( !pMsg )
		return -1;

	
	MsgBuf* pBuf = (MsgBuf*)pMsg;

	/* 先接收消息头 */
	s32Ret = ReadData( s32SockFd, (S8 *)pBuf->GetHead(), sizeof(MSG_HEADER_S), u32TimeOut );
	if ( s32Ret < 0 )
		return -1;
	
	/* 再接收消息体 */
	s32Ret = ReadData( s32SockFd, (S8 *)pBuf->GetData(), pBuf->GetDataLen(), u32TimeOut );
	if ( s32Ret < 0 )
		return -1;
	
	s32Ret = ReadData( s32SockFd, (S8 *)pBuf->GetExt(), pBuf->GetExtLen(), u32TimeOut );
	if ( s32Ret < 0 )
		return -1;


	

// 	/* 再接收消息体 */
// 	if ( m_MsgBuf.GetDataLen() + m_MsgBuf.GetExtLen() + m_MsgBuf.GetHeadLen() > MAX_TALK_MSG_LEN )
// 	{
// 		UTNETSERVER_DEBUG( "MsgBody is too long!\n" );
// 		s32Ret = ReadData( s32SockFd, (S8*)m_MsgBuf.GetData(), MAX_TALK_MSG_LEN - m_MsgBuf.GetHeadLen() - m_MsgBuf.GetExtLen(), u32TimeOut );
// 		if ( s32Ret < 0 )
// 			return -1;
// 	}
// 	else
// 	{
// 		s32Ret = ReadData( s32SockFd, (S8*)m_MsgBuf.GetData(), m_MsgBuf.GetDataLen(), u32TimeOut );
// 		if ( s32Ret < 0 )
// 			return -1;
// 	}

	return 0;
}

/** 
 * @remarks    发送请求回馈消息
 * @pMsg       消息内容地址 
 * @u32TimeOut 发送超时值,0为永久阻塞
 * @return     成功0, 失败-1.
 */
S32 UtNetServer::SendNetMsg( S32 s32SockFd, S8 *pMsg, U32 u32TimeOut)
{
	m_bSendSuccess = TRUE;//先初始化为TRUE
	S32 s32Ret = -1;

	ASSERT( pMsg );

	MSG_HEADER_S *pMsgHeader = (MSG_HEADER_S *)pMsg;
	pMsgHeader->u6ExtSize = 0;

	if ( u32TimeOut)
		s32Ret = WriteData( s32SockFd, (S8 *)pMsgHeader , sizeof(MSG_HEADER_S) + pMsgHeader->u32MsgSize, u32TimeOut);
	else
		s32Ret = WriteData( s32SockFd, (S8 *)pMsgHeader , sizeof(MSG_HEADER_S) + pMsgHeader->u32MsgSize);

	if ( s32Ret < 0 ) 
	{
		list<SERVER_CONN *>::iterator iter;
		for (iter=m_listConn.begin(); iter!=m_listConn.end(); iter++)
		{
			SERVER_CONN * pConn = *iter;
			if (s32SockFd == pConn->s32SockFd )
			{
// 				if (!pConn->bStopTalk)
// 				{
					pConn->bSendSuccess = FALSE;
//				}
				break;
			}
		}
		
		UTNETSERVER_DEBUG("Talk Server Send Fail!\n");
		return -1;
	}
	return 0;
}

void UtNetServer::ClearRcvThread()
{
	list<SERVER_CONN *>::iterator iter;
	for (iter=m_listConn.begin(); iter!=m_listConn.end(); iter++)
	{
		SERVER_CONN * pConn = *iter;
		if (0==pConn->u8Active && pConn->ThreadID)
		{
			UTNETSERVER_DEBUG("Talk Server ThreadJoin in!\n");
			ThreadJoin(pConn->ThreadID);
			pConn->ThreadID = 0;
			UTNETSERVER_DEBUG("Talk Server ThreadJoin out!\n");
		}
	}
}

VOID *UtNetServer::ThreadEntry(VOID *pArg)
{
	ASSERT( pArg );
	THREAD_ARG_S *pThreadArg = (THREAD_ARG_S *)pArg;
	UtNetServer *pServer = (UtNetServer *)pThreadArg->pClass;
	U8 u8Func = pThreadArg->u8Func;
	SERVER_CONN* pConn = (SERVER_CONN*)pThreadArg->pArg;
	
	switch ( u8Func )
	{
		case  THREAD_TCPLISTEN:
			pServer->TCPListen();
			break;
		case THREAD_RCVMSG:
			pServer->RcvMsgLoop(pConn);
			break;
		default:
			break;
	}
	delete pThreadArg;	

	return NULL;
}

/***************************************************************************************
***************************************************************************************/

#define ENABLE_UTNETCLIENT_DEBUG
#ifdef  ENABLE_UTNETCLIENT_DEBUG 
#ifdef _WIN32
#define UTNETCLIENT_DEBUG             printf 
#else
#define UTNETCLIENT_DEBUG(MSG...)     DEBUG("UTNETCLIENT", MSG)
#endif /* end _WIN32 */
#else
#define UTNETCLIENT_DEBUG(MSG...)
#endif /* end ENABLE_UTNETSERVER_DEBUG */


UtNetClient::UtNetClient()
{
	m_s32TcpConnFd  = -1;
	m_trdTcpClient = 0;
	m_bExit        = FALSE;

	m_pS8ServerIp = NULL;
	m_u16TcpPort = 0;
	m_pPcTalk = NULL;
	m_bSendSuccess = TRUE;
	UTNETCLIENT_DEBUG(  "UtNetClient construct!\n" );
}

S32 UtNetClient::Start()
{
	m_bExit = FALSE;

	THREAD_ARG_S *pThreadArg = new THREAD_ARG_S;
	
	pThreadArg->pClass = this;
	pThreadArg->pArg   = NULL; 
	if ( CreateJoinableThread(ThreadEntry, pThreadArg, &m_trdTcpClient) < 0)
	{
		UTNETCLIENT_DEBUG(  "start TCPClient failed!\n" );
		delete pThreadArg; 
		return -1;
	}
	return 0;
}

S32 UtNetClient::Stop()
{
	m_bExit = TRUE;
	m_MainSock.Close();
	return 0;
}

UtNetClient::~UtNetClient()
{
	Stop();

	if (m_trdTcpClient)
	{
		ThreadJoin(m_trdTcpClient);
		m_trdTcpClient = 0;
	}

	UTNETCLIENT_DEBUG(  "UtNetClient distruct!\n" );
}

S32 UtNetClient::TCPConnect(S8 *pS8ServerIP,U16 u16TcpPort)
{
	m_pS8ServerIp = pS8ServerIP;
	m_u16TcpPort = u16TcpPort;

	if (m_MainSock.Socket(TRUE) \
		&& m_MainSock.Connect(m_pS8ServerIp, m_u16TcpPort, 5000, TRUE))
	{
		UTNETCLIENT_DEBUG( "UtNetClient Connect success\n");
		m_s32TcpConnFd = m_MainSock.GetSocket();
		return TRUE;
	}

	UTNETCLIENT_DEBUG("UtNetClient Connect fail\n");
	
	m_MainSock.Close();
	
	return FALSE;

	return 0;
}

S32 UtNetClient::PcTalkProcess()
{
	S32 s32Ret = -1;
	S32 s32SockFd = m_s32TcpConnFd;
	int timeoutNum = 0;
	while (!m_bExit)
	{
		if (s32SockFd == -1)
		{
			continue;
		}

		s32Ret =  RecvNetMsg(s32SockFd, (S8*)&m_MsgBuf, 5);
		if ( s32Ret < 0 )
		{
			MsgHeadWrap heartbeat(MSG_HEARTBEAT);
			heartbeat.u2Version = MsgHeadV1;

			if (!m_bSendSuccess)
			{
				UTNETSERVER_DEBUG("Server don't Rcv PcTalk Msg!\n");
				if (m_pPcTalk)
				{
					m_MsgBuf.u16MsgType = MSG_PC_TALK;
					m_MsgBuf.u16MsgSubType = MSG_PCTALK_OFFLINE;					
					m_pPcTalk->ProcessPcTalk(m_MsgBuf, s32SockFd);
				}
				Stop();
				break;
			}
			else 
			{
				SendNetMsg(s32SockFd, (S8*)&heartbeat, 5);
				UTNETCLIENT_DEBUG("PC talk client send heatbeat\n");
				continue;
			}
			continue;
		}
		
		switch(m_MsgBuf.GetMsgType())
		{
			/* 心跳 */
		case MSG_HEARTBEAT:
			UTNETCLIENT_DEBUG("Client Rcv heatbeat!\n");
			s32Ret = HeartBeatProcess(s32SockFd, (S8*)&m_MsgBuf);
			break;
			/*  对讲 */
		case MSG_PC_TALK:
			UTNETCLIENT_DEBUG("Client Rcv PcTalk Msg!\n");
			if (m_pPcTalk)
			{
				m_pPcTalk->ProcessPcTalk(m_MsgBuf, s32SockFd);
			}
			break;
			
			
		default:
			s32Ret = -1;
			UTNETCLIENT_DEBUG("unkown Msg type!\n");
			break;
		}
	}
	return 0;
}

/** 
 * @remarks  心跳请求处理
 * @return   成功0, 失败-1.
 */
S32 UtNetClient::HeartBeatProcess( S32 s32SockFd, S8 *pMsg )
{
	ASSERT( pMsg );

	MSG_HEADER_S *pMsgHeader = (MSG_HEADER_S *)pMsg;
	pMsgHeader->u16AckFlag = ACK_SUCCESS;
	pMsgHeader->u32MsgSize = 0;

	return SendNetMsg( s32SockFd, pMsg, 5);
}

/** 
 * @remarks    接收请求消息
 * @pMsg       消息内容地址 
 * @u32TimeOut 接收超时值,0为永久阻塞
 * @return     成功0, 失败-1.
 */
S32 UtNetClient::RecvNetMsg( S32 s32SockFd, S8 *pMsg, U32 u32TimeOut )
{
	ASSERT( pMsg );
	S32 s32Ret = -1;	

	if ( !pMsg )
		return -1;

	/* 先接收消息头 */
	s32Ret = ReadData( s32SockFd, (S8 *)m_MsgBuf.GetHead(), sizeof(MSG_HEADER_S), u32TimeOut );
	if ( s32Ret < 0 )
		return -1;

	/* 再接收消息体 */
	s32Ret = ReadData( s32SockFd, (S8 *)m_MsgBuf.GetData(), m_MsgBuf.GetDataLen(), u32TimeOut );
	if ( s32Ret < 0 )
		return -1;

	s32Ret = ReadData( s32SockFd, (S8 *)m_MsgBuf.GetExt(), m_MsgBuf.GetExtLen(), u32TimeOut );
	if ( s32Ret < 0 )
		return -1;


	return 0;
}

/** 
 * @remarks    发送请求回馈消息
 * @pMsg       消息内容地址 
 * @u32TimeOut 发送超时值,0为永久阻塞
 * @return     成功0, 失败-1.
 */
S32 UtNetClient::SendNetMsg( S32 s32SockFd, S8 *pMsg, U32 u32TimeOut)
{
	m_bSendSuccess = TRUE;//先初始化为TRUE
	S32 s32Ret = -1;

	ASSERT( pMsg );

	MSG_HEADER_S *pMsgHeader = (MSG_HEADER_S *)pMsg;
	pMsgHeader->u6ExtSize = 0;

	if ( u32TimeOut)
		s32Ret = WriteData( s32SockFd, (S8 *)pMsgHeader , sizeof(MSG_HEADER_S) + pMsgHeader->u32MsgSize, u32TimeOut);
	else
		s32Ret = WriteData( s32SockFd, (S8 *)pMsgHeader , sizeof(MSG_HEADER_S) + pMsgHeader->u32MsgSize);

	if ( s32Ret < 0 ) 
	{
		if (!m_bExit)
		{
			m_bSendSuccess = FALSE;
		}		
		
		UTNETCLIENT_DEBUG("Talk Client Send Fail!\n");
		return -1;
	}
	return 0;
}



VOID *UtNetClient::ThreadEntry(VOID *pArg)
{
	ASSERT( pArg );
	THREAD_ARG_S *pThreadArg = (THREAD_ARG_S *)pArg;
	UtNetClient *pClient = (UtNetClient *)pThreadArg->pClass;
	U8 u8Func = pThreadArg->u8Func;

	pClient->PcTalkProcess();

	delete pThreadArg;	

	return NULL;
}
