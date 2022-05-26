#include "socketc1.h"

#ifndef _WIN32
int WSAGetLastError()
{
	return errno;
}
#endif

#if defined(EnableNetErr) && defined(_WIN32)
#define NetErr(...)	RSNetLog("NetErr", __VA_ARGS__)
#else
//#define NetErr(...) ((VOID)0)
#define NetErr //
#endif

#define NetFunErr(sock, e) NetErr("sock=%d, %s err = %d\n", sock, __FUNCTION__, e)
#define NetFunLastErr(sock) \
	do \
	{ \
		int _e = WSAGetLastError(); \
		if (_e != 0 /*&& _e != WSAETIMEDOUT*/ && _e != WSAENOTSOCK) \
			NetFunErr(sock, _e); \
	} while (0)

#define ENABLE_SOCKET_DEBUG
#ifdef  ENABLE_SOCKET_DEBUG 
#ifdef _WIN32
#define SOCKET_DEBUG             printf
#else
#define SOCKET_DEBUG(MSG...)     DEBUG("SOCKET", MSG)
#endif /* end _WIN32 */
#else
#define SOCKET_DEBUG(MSG...)
#endif /* end ENABLE_SOCKET_DEBUG */

BOOL SocketInit()
{
#ifdef _WIN32
	const WORD version = MAKEWORD(2, 2);
	WSADATA wsaData;

	int e = WSAStartup(version, &wsaData);
	if (e == 0)
	{
		if (version == wsaData.wVersion)
			return TRUE;

		NetErr("WSAStartup request version %04x, actual version %04x\n", version, wsaData.wVersion);
		WSACleanup();
	}
// 	else
// 		NetFunErr(0, e);
	return FALSE;
#else
	return TRUE;
#endif
}

VOID SocketTerm()
{
#ifdef _WIN32
	if (WSACleanup() != 0)
	{
		//	NetFunLastErr(0);
	}

#endif
}

SyncSocket::SyncSocket()
{
	mSock = INVALID_SOCKET;
}

SyncSocket::~SyncSocket()
{
	Close();
}

BOOL SyncSocket::Socket(BOOL tcp)
{
	ASSERT(mSock == INVALID_SOCKET);

	mSock = socket(PF_INET, tcp?SOCK_STREAM:SOCK_DGRAM, IPPROTO_IP);
	if (mSock != INVALID_SOCKET)
	{
		SOCKET_DEBUG("create sock = %d\n", mSock);
		return TRUE;
	}

	return FALSE;
}
#if 0
BOOL SyncSocket::Bind(PCSTR ip, WORD port, BOOL tcp)
{
	SocketAddr addr;
	if (!addr.FindAddr(ip, port, tcp))
		return FALSE;

	return Bind(addr);
}

BOOL SyncSocket::Bind(const SocketAddr& addr)
{
	const addrinfo* addrInfo = addr.GetAddr();
	return Bind(addrInfo->ai_addr, addrInfo->ai_addrlen);
}

BOOL SyncSocket::Bind(const sockaddr* addr, int addrlen)
{
	int e = bind(mSock, addr, addrlen);
	if (e == 0)
		return TRUE;

	SOCKET_DEBUG("bind error!\n");

	return FALSE;
}
#endif
BOOL SyncSocket::Connect(PCSTR ip, WORD port, UINT milliseconds, BOOL tcp)
{
	SA_IN servAddr;
	memset(&servAddr, 0, sizeof(SA_IN));

	if (tcp)
	{
		servAddr.sin_family = AF_INET;
		servAddr.sin_port   = htons(port);
#ifdef _WIN32
		servAddr.sin_addr.S_un.S_addr = inet_addr(ip);
#else
		inet_pton(AF_INET, ip, &servAddr.sin_addr);
#endif
	}

	return Connect((SA *)&servAddr, sizeof(servAddr), milliseconds);
}	


BOOL SyncSocket::Connect(const sockaddr* addr, int addrlen, UINT milliseconds)
{
	return SetNonblocking(TRUE) && Connect(addr, addrlen) && Select(TRUE, milliseconds) && SetNonblocking(FALSE) && SelfConnect((sockaddr_in*)addr);
}

VOID SyncSocket::KillSocket()
{
	if (mSock != INVALID_SOCKET)
	{
//		SOCKET_DEBUG("kill sock = %d\n", mSock);
		close(mSock);
	}
}

VOID SyncSocket::Close()
{
	KillSocket();
	mSock = INVALID_SOCKET;
}

BOOL SyncSocket::Select(BOOL send, UINT milliseconds, BOOL *timeout)
{
	fd_set fds;
	FD_ZERO(&fds);
	FD_SET(mSock, &fds);

	timeval tv;
	tv.tv_sec = (long)(milliseconds / 1000);
	tv.tv_usec = (long)(milliseconds - tv.tv_sec*1000) * 1000;

	fd_set* pFds[2] = { NULL };
	if (send)
		pFds[1] = &fds;
	else
		pFds[0] = &fds;

	int e = select(mSock + 1, pFds[0], pFds[1], NULL, &tv);
	if (e > 0)
	{
		int err = -1;
		socklen_t len = sizeof(err);

#ifdef _WIN32
		if (getsockopt(mSock, SOL_SOCKET, SO_ERROR,  (S8 *)&err, &len) < 0)
#else
			if (getsockopt(mSock, SOL_SOCKET, SO_ERROR,  &err, &len) < 0)
#endif
			{
				return FALSE;
			}

		if (err)
		{
			return FALSE;
		}

		ASSERT(e == 1);
		return TRUE;
	}

	if (timeout != NULL)
		*timeout = (e == 0);

	if (e < 0)
	{
		SOCKET_DEBUG("select error!\n");
	}

	return FALSE;
}

BOOL SyncSocket::Send(VOID *buf, ULONG buflen)
{
	int len = send(mSock, (char*)buf, (int)buflen, 0);
	if (len == (int)buflen)
		return TRUE;

	SOCKET_DEBUG("send error!\n");

	return FALSE;
}

BOOL SyncSocket::Send(VOID *buf0, ULONG buflen0, VOID *buf1, ULONG buflen1)
{
	char* buf = new char[buflen0 + buflen1];
	if (buf == NULL)
		return FALSE;

	if (buflen0)
		memcpy(buf, buf0, buflen0);
	if (buflen1)
		memcpy(buf + buflen0, buf1, buflen1);

	BOOL ret = Send(buf, buflen0 + buflen1);
	delete [] buf;
	return ret;
}

BOOL SyncSocket::Recv(VOID* buf, int len)
{
	if (len < 0 || len > 4*1024*1024)
		return FALSE;

	char* recvto = (char*)buf;
	while (len > 0)
	{
		int e = recv(mSock, recvto, len, 0);
		if (e > 0)
		{
			recvto += e;
			len -= e;
		}
		else
		{
			if (e < 0)
			{
				SOCKET_DEBUG("recv error!\n");
			}
			else
			{
				SOCKET_DEBUG("recv 0 bytes  socket=%d\n", mSock);
			}
			
#ifdef _WIN32
			DWORD lastErr = WSAGetLastError();
			
			WSASetLastError(lastErr);
#endif
			return FALSE;
		}
	}
	ASSERT(len == 0);
	
	return TRUE;
}
#if 0
//-------------------------------------------------------------------------
// 尝试读取socket缓冲区以寻找头结束标志
// socket错误返回-1, 只有当select超时才返回0;
// 其他错误返回-2 ;
// 成功返回>0, 为buf的有效长度
// 要求: 1- 严格保证函数在给定时间内返回
//       2- 在第一条允许的范围内保证可以进行断点调试
#define MAX_SIZE_HTTPHEAD 1024
int  SyncSocket::readHeadFlag(char* buf, int nMaxLen, char * pHead,timeval* timeout)
{
	fd_set fd_read;
	int nReturn = 0;
	int  nRecv        = 0;
	int  nHeadLen     = 0;
	int  nRecvAll     = 0;
	int  nRecvLastAll = 0;
	char temp[1024] = {0};

	char szHeadFlag[32] = {0};
	memcpy(szHeadFlag,pHead,sizeof(szHeadFlag));

	// convert time to usec
	timeval tv;
	DWORD   dwStart, dwUsed;  // 记录开始时间和select时间开销
	dwUsed =0;
	dwStart = GetTickCount();
	dwUsed = GetTickCount();

	tv.tv_sec = 2;
	tv.tv_usec = 0;

	//这种判断延时的方式会有0 ~ +3秒以内的误差。
	while((dwUsed-dwStart) <= (DWORD)(timeout->tv_sec*1000 + timeout->tv_usec/1000))
	{	
		FD_ZERO(&fd_read);
		FD_SET(mSock, &fd_read);

		int ret = select(mSock, &fd_read, NULL, NULL, &tv); 
		if (ret <= 0)
		{
			if(ret < 0)
			{
				//TRACE("call select() function error  %d\n", WSAGetLastError());
				return ret;
			}
			dwUsed = GetTickCount();
			continue;
		}

		// 从系统缓冲区里COPY数据包到到应用程序接收缓冲区
		int nRecvOnce = 256;
		if( nMaxLen < 256)
			nRecvOnce = nMaxLen;

		nRecv= recv( mSock, buf+ nRecvAll, nRecvOnce, MSG_PEEK); // 一次读取字节数
		if( nRecv <= 0)
		{
			//TRACE("=== readHeadFlag: select return >0, but recv=%d \n", nRecv);
			nReturn = SOCKET_ERROR;
			dwUsed = GetTickCount();
			if(nRecv < 0)
			{
				//TRACE("=== readHeadFlag: select return >0, but recv=%d, Err=%d ===\n", nRecv, WSAGetLastError());
				return nRecv;
			}
			continue;
		}
		nRecvLastAll = nRecvAll;    // 保存上一次读取后的总长度
		nRecvAll += nRecv;          // 更新目前已经读取的总长度
		if( nMaxLen < nRecvAll)   // 检查外部接收缓冲区长度,如果不够长则返回
		{
			recv( mSock, temp, nRecv, 0); // remove from socket buffer, buf里的数据不效
			nReturn =  -2;
			break;
		}
		buf[nRecvAll] = '\0';

		// 分析接收到的数据中是否有头部结束标志"\r\n\r\n"
		char* pStr = NULL;
		pStr = strstr(buf, szHeadFlag);    // 寻找头结束标志"\r\n\r\n"
		if( NULL == pStr)                  // not found,continue finding
		{
			pStr = strstr(buf,"404 Not Found");
			if (pStr != NULL)
			{
				break;
			}
			recv( mSock, temp, nRecv, 0);     // 本次读出的数据需要全部remove
			continue;
		}
		// 如果buf里已经有了头结束标志
		nHeadLen = pStr - buf + strlen(szHeadFlag);
		// remove socket buf
		recv( mSock, temp, nHeadLen- nRecvLastAll, 0);  
		// 可能的异常情况:经过几次循环读取后,这个长度过大,应将包丢弃并返回错误(这样的包不允许进一步解析)
		if( nHeadLen >= MAX_SIZE_HTTPHEAD)  	
			nReturn =  -2;
		// 这个包首部符合常规要求, 然后返回这个包首部的长度
		nReturn =  nHeadLen;
		break;
	}
	return nReturn;
}

#endif

#if 0
BOOL SyncSocket::SendTo(VOID *head, int headlen, PCSTR ip, WORD port, VOID *data = NULL, int datalen = 0)
{
	SocketAddr addr;
	if (!addr.FindAddr(ip, port, FALSE))
		return FALSE;

	return SendTo(head, headlen, addr, data, datalen);
}

BOOL SendTo(VOID *head, int headlen, const SocketAddr& addr, VOID *data = NULL, int datalen = 0)
{
	const addrinfo* addrInfo = addr.GetAddr();
	return SendTo(head, headlen, addrInfo->ai_addr, addrInfo->ai_addrlen, data, datalen);
}

BOOL SendTo(VOID *head, int headlen, const sockaddr* to, int tolen, VOID *data = NULL, int datalen = 0)
{
	char* buf = new char[headlen + datalen];
	if (buf == NULL)
		return FALSE;

	if (headlen)
		memcpy(buf, head, headlen);
	if (datalen)
		memcpy(buf + headlen, data, datalen);

	int ret = sendto(mSock, buf, headlen + datalen, 0, to, tolen);
	delete [] buf;

	if (ret != headlen + datalen)
		return FALSE;

	return TRUE;
}

#endif

BOOL SyncSocket::RecvFrom(VOID* buf, int& len, sockaddr* from, int* fromlen)
{
	int e = recvfrom(mSock, (CHAR *)buf, len, 0, from, (socklen_t *)fromlen);
	if (e > 0)
	{
		len = e;
		return TRUE;
	}

	if (e < 0)
	{
#ifdef _WIN32
		//NetFunLastErr(mSock);
		if (WSAGetLastError() == WSAEMSGSIZE)
		{
			len = 0;
			return TRUE;
		}
#endif
	}
// 	else
// 		NetErr("%s recv 0 bytes\n", __FUNCTION__);
	return FALSE;
}


BOOL SyncSocket::SetNonblocking(BOOL enable)
{
#ifdef _WIN32
	ULONG val = (enable?1:0);
	if (ioctlsocket(mSock, FIONBIO, &val) != 0)
		return FALSE;
#else
	int val;
	if((val = fcntl(mSock, F_GETFL, 0)) < 0)
		return FALSE;

	if (enable)
		val |= O_NONBLOCK;
	else
		val &= ~O_NONBLOCK;

	if(fcntl(mSock, F_SETFL, val) < 0)
		return FALSE;
#endif

	return TRUE;
}

BOOL SyncSocket::SetSendTimeOut(int milliseconds)
{
#ifdef _WIN32
	return SetSockOpt(SO_SNDTIMEO, milliseconds);
#else
	struct timeval tv;
	tv.tv_sec = milliseconds/1000;
	tv.tv_usec = (milliseconds%1000)*1000;
	return setsockopt(mSock, SOL_SOCKET, SO_SNDTIMEO, (const char*)&tv, sizeof(tv)) == 0;
#endif
}

BOOL SyncSocket::SetRecvTimeOut(int milliseconds)
{
#ifdef _WIN32
	return SetSockOpt(SO_RCVTIMEO, milliseconds);
#else
	struct timeval tv;
	tv.tv_sec = milliseconds/1000;
	tv.tv_usec = (milliseconds%1000)*1000;
	return setsockopt(mSock, SOL_SOCKET, SO_RCVTIMEO, (const char*)&tv, sizeof(tv)) == 0;
#endif
}

BOOL SyncSocket::SetBroadcast(BOOL enable)
{
	return SetSockOpt(SO_BROADCAST, enable);
}

BOOL SyncSocket::SetRecvBuf(int buflen)
{
	return SetSockOpt(SO_RCVBUF, buflen);
}

BOOL SyncSocket::SetSndBuf(int buflen)
{
	return SetSockOpt(SO_SNDBUF, buflen);
}

BOOL SyncSocket::GetRecvBuf(int &buflen)
{
	return GetSockopt(SO_RCVBUF, buflen);
}

BOOL SyncSocket::GetSndBuf(int &buflen)
{
	return GetSockopt(SO_SNDBUF, buflen);
}

BOOL SyncSocket::Connect(const sockaddr* addr, int addrlen)
{
#ifndef _WIN32
#undef WSAEWOULDBLOCK
#define WSAEWOULDBLOCK EINPROGRESS
#endif
	if (connect(mSock, addr, addrlen) == 0 || WSAGetLastError() == WSAEWOULDBLOCK)
		return TRUE;

	//		NetFunLastErr(mSock);
	return FALSE;
}

BOOL SyncSocket::SelfConnect(const sockaddr_in* svrAddr)
{
	sockaddr_in addr_client;
	socklen_t len = sizeof(sockaddr_in);
	
	if(getsockname(mSock, (struct sockaddr *)&addr_client, &len))
    { 
		return FALSE;
	}
	if(len != sizeof(sockaddr_in))
    { 
		return FALSE;
	}
	
	if(addr_client.sin_port == svrAddr->sin_port &&
	   addr_client.sin_addr.s_addr == svrAddr->sin_addr.s_addr)
    { 
		return FALSE;
	}
	
	return TRUE;
}
