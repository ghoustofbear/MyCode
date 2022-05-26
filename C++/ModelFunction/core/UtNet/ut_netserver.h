/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 * Apr 2011-04-16
 *
 * Media server define.
 */

#ifndef __UT_NET_SERVER_H__
#define __UT_NET_SERVER_H__
#include "defc1.h"
#include "net_msg.h"

#define MAX_TALK_MSG_LEN 10*1024
class PcTalk;
// 线程种类
enum
{
	THREAD_TCPLISTEN,
		THREAD_UDPLISTEN,
		THREAD_RCVMSG,
};

typedef struct
{
	S32      s32SockFd;
	S8     * s8PeerIp;
	THREAD   ThreadID; /* 线程ID */
	U8       u8Active; /*线程是否活动*/
	MsgBuf   bufMsg;
	BOOL     bStopTalk;
	BOOL     bSendSuccess;
}SERVER_CONN;

class UtNetServer 
{
public:
	UtNetServer();

	virtual ~UtNetServer();
	
	/**< 启动服务 */
	S32 Start(U16 u16TcpPort);
	S32 Stop(S32 s32SockFd=-1);
		/**< TCP消息接收与发送函数 */
	S32 RecvNetMsg( S32 s32SockFd, S8 *ps8Msg, U32 u32TimeOut );
	S32 SendNetMsg( S32 s32SockFd, S8 *ps8Msg, U32 u32TimeOut );

	/*清理之前对讲的连接线程*/
	void ClearRcvThread();
	/*发送NAK的同时处理连接*/
	void StopRcv(S32 s32SockFd);
	
private:
	/**< 启动TCP服务 */
	S32 TCPServStart();
	
	/**< TCP监听线程 */
	S32 TCPListen();

	/* 心跳 */
	S32 HeartBeatProcess( S32 s32SockFd, S8 *ps8Msg );

	/**< 启动数据接收处理线程*/
	S32 RcvMsgProcess(S32 s32SockFd, S8 *PeerIP);
	S32 RcvMsgLoop(SERVER_CONN * pConn);

	/**< 数据接收处理线程 */
	S32 AcceptProcess();
	S32 GetPeerIpAddr( struct sockaddr *pstPeerAddr, S32 s32SALen, S8 *PeerIP, S8 *PeerPort);

	
	static VOID *ThreadEntry(VOID *pArg);
	
public:
	PcTalk   *m_pPcTalk;          /**< 对讲协议指针 */
	U16       m_u16TcpPort;
	BOOL      m_bStopTalk;
	BOOL      m_bSendSuccess;
	
	
private:	
	THREAD    m_trdTcpListen;
	THREAD    m_trdRcvMsg; 
	THREAD    m_trdManager;/*管理线程*/
	BOOL      m_bExit;
	S32       m_s32TcpListenFd;   /**< TCP监听套接口 */
	list<SERVER_CONN*> m_listConn;
		

		
};

class UtNetClient
{
public:
	UtNetClient();
	
	virtual ~UtNetClient();
	
	/**< 启动服务 */
	S32 Start();
	S32 Stop();
		/**< TCP消息接收与发送函数 */
	S32 RecvNetMsg( S32 s32SockFd, S8 *ps8Msg, U32 u32TimeOut );
	S32 SendNetMsg( S32 s32SockFd, S8 *ps8Msg, U32 u32TimeOut );
	
	/**< 启动TCP连接 */
	S32 TCPConnect(S8 *pS8ServerIP,U16 u16TcpPort);
	
private:	
	/**< 数据接收处理线程 */
	S32 PcTalkProcess();
	/* 心跳 */
	S32 HeartBeatProcess( S32 s32SockFd, S8 *ps8Msg );
	
	static VOID *ThreadEntry(VOID *pArg);
	
public:
	PcTalk  * m_pPcTalk;          /**< 对讲协议指针 */
	S8      * m_pS8Name;          /**< 客户端名称 */
	S8      * m_pS8ServerIp;      /**< 服务器IP */
	U16       m_u16TcpPort;       /**< 服务器端口 */
	S32       m_s32TcpConnFd;     /**< TCP套接字 */
	SyncSocket       m_MainSock;       /**< 主消息链路 */
	BOOL      m_bSendSuccess;
	
	
private:
	MsgBuf    m_MsgBuf;
	THREAD    m_trdTcpClient;
	BOOL      m_bExit;		
};
#endif
