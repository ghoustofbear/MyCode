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
// �߳�����
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
	THREAD   ThreadID; /* �߳�ID */
	U8       u8Active; /*�߳��Ƿ�*/
	MsgBuf   bufMsg;
	BOOL     bStopTalk;
	BOOL     bSendSuccess;
}SERVER_CONN;

class UtNetServer 
{
public:
	UtNetServer();

	virtual ~UtNetServer();
	
	/**< �������� */
	S32 Start(U16 u16TcpPort);
	S32 Stop(S32 s32SockFd=-1);
		/**< TCP��Ϣ�����뷢�ͺ��� */
	S32 RecvNetMsg( S32 s32SockFd, S8 *ps8Msg, U32 u32TimeOut );
	S32 SendNetMsg( S32 s32SockFd, S8 *ps8Msg, U32 u32TimeOut );

	/*����֮ǰ�Խ��������߳�*/
	void ClearRcvThread();
	/*����NAK��ͬʱ��������*/
	void StopRcv(S32 s32SockFd);
	
private:
	/**< ����TCP���� */
	S32 TCPServStart();
	
	/**< TCP�����߳� */
	S32 TCPListen();

	/* ���� */
	S32 HeartBeatProcess( S32 s32SockFd, S8 *ps8Msg );

	/**< �������ݽ��մ����߳�*/
	S32 RcvMsgProcess(S32 s32SockFd, S8 *PeerIP);
	S32 RcvMsgLoop(SERVER_CONN * pConn);

	/**< ���ݽ��մ����߳� */
	S32 AcceptProcess();
	S32 GetPeerIpAddr( struct sockaddr *pstPeerAddr, S32 s32SALen, S8 *PeerIP, S8 *PeerPort);

	
	static VOID *ThreadEntry(VOID *pArg);
	
public:
	PcTalk   *m_pPcTalk;          /**< �Խ�Э��ָ�� */
	U16       m_u16TcpPort;
	BOOL      m_bStopTalk;
	BOOL      m_bSendSuccess;
	
	
private:	
	THREAD    m_trdTcpListen;
	THREAD    m_trdRcvMsg; 
	THREAD    m_trdManager;/*�����߳�*/
	BOOL      m_bExit;
	S32       m_s32TcpListenFd;   /**< TCP�����׽ӿ� */
	list<SERVER_CONN*> m_listConn;
		

		
};

class UtNetClient
{
public:
	UtNetClient();
	
	virtual ~UtNetClient();
	
	/**< �������� */
	S32 Start();
	S32 Stop();
		/**< TCP��Ϣ�����뷢�ͺ��� */
	S32 RecvNetMsg( S32 s32SockFd, S8 *ps8Msg, U32 u32TimeOut );
	S32 SendNetMsg( S32 s32SockFd, S8 *ps8Msg, U32 u32TimeOut );
	
	/**< ����TCP���� */
	S32 TCPConnect(S8 *pS8ServerIP,U16 u16TcpPort);
	
private:	
	/**< ���ݽ��մ����߳� */
	S32 PcTalkProcess();
	/* ���� */
	S32 HeartBeatProcess( S32 s32SockFd, S8 *ps8Msg );
	
	static VOID *ThreadEntry(VOID *pArg);
	
public:
	PcTalk  * m_pPcTalk;          /**< �Խ�Э��ָ�� */
	S8      * m_pS8Name;          /**< �ͻ������� */
	S8      * m_pS8ServerIp;      /**< ������IP */
	U16       m_u16TcpPort;       /**< �������˿� */
	S32       m_s32TcpConnFd;     /**< TCP�׽��� */
	SyncSocket       m_MainSock;       /**< ����Ϣ��· */
	BOOL      m_bSendSuccess;
	
	
private:
	MsgBuf    m_MsgBuf;
	THREAD    m_trdTcpClient;
	BOOL      m_bExit;		
};
#endif
