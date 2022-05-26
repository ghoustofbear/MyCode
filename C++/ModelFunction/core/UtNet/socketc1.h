#ifndef __SOCKET_H__
#define __SOCKET_H__
#include "concourrencyc1.h"

#ifndef _WIN32
int WSAGetLastError();
#endif
BOOL SocketInit();;
VOID SocketTerm();

class SocketAddr
{
#if 0
public:
	SocketAddr()
	{
		mAddrs = NULL;
	}

	~SocketAddr()
	{
		FreeAddr();
	}

	BOOL FindAddr(PCSTR ip, WORD wPort, BOOL tcp)
	{
		FreeAddr();

		char szPort[6];
		snprintf(szPort, 6, "%d", wPort);

		struct addrinfo hints;
		memset(&hints, 0, sizeof(hints));
		hints.ai_family = PF_INET;
		hints.ai_protocol = IPPROTO_IP;
		hints.ai_socktype = (tcp?SOCK_STREAM:SOCK_DGRAM);

		int e = getaddrinfo(ip, szPort, &hints, &mAddrs);
		if (e == 0)
		{
			ASSERT(mAddrs != NULL);
			return TRUE;
		}

		NetFunErr(0, e);
		return FALSE;
	}

	const struct addrinfo* GetAddr() const
	{
		ASSERT(mAddrs != NULL);
		return mAddrs;
	}

protected:
	VOID FreeAddr()
	{
		if (mAddrs != NULL)
			freeaddrinfo(mAddrs);
	}

protected:
	addrinfo* mAddrs;
#endif
};

class TransDataRateStat
{
public:
	TransDataRateStat()
	{
		Tidy();
	}

	VOID TransData(UINT size)
	{
		AutoLock lock(mCS);
		mTransDataSize += size;
	}

	U64 Stat()
	{
		AutoLock lock(mCS);

		time_t now = time(NULL);
		time_t elapse = now - mLastStatTime;
		if (elapse <= 0)
			elapse = 1;

		U64 bitRate = (mTransDataSize << 3) / elapse;
		Tidy();

		return bitRate;
	}

	VOID Clear()
	{
		AutoLock lock(mCS);
		Tidy();
	}

protected:
	VOID Tidy()
	{
		mTransDataSize = 0;
		mLastStatTime = time(NULL);
	}

protected:
	U64 mTransDataSize;
	time_t mLastStatTime;
//	QCriSec mCS;
	Mutex  mCS;
};

#ifdef _WIN32
#else
#define INVALID_SOCKET   (-1)
#endif

class SyncSocket
{
public:
	SyncSocket();
	~SyncSocket();
	BOOL Socket(BOOL tcp);
	BOOL Bind(PCSTR ip, WORD port, BOOL tcp);
	BOOL Bind(const SocketAddr& addr);

	BOOL Bind(const sockaddr* addr, int addrlen);
	BOOL Connect(PCSTR ip, WORD port, UINT milliseconds, BOOL tcp = TRUE);
	BOOL Connect(const sockaddr* addr, int addrlen, UINT milliseconds);
	BOOL SelfConnect(const sockaddr_in* svrAddr);

	VOID KillSocket();
	VOID Close();
	BOOL Select(BOOL send, UINT milliseconds, BOOL *timeout = NULL);
	BOOL Send(VOID *buf, ULONG buflen);
	BOOL Send(VOID *buf0, ULONG buflen0, VOID *buf1, ULONG buflen1);
	BOOL Recv(VOID* buf, int len);
	int  readHeadFlag(char* buf, int nMaxLen, char * pHead,timeval* timeout);
	BOOL SendTo(VOID *head, int headlen, PCSTR ip, WORD port, VOID *data = NULL, int datalen = 0);
	BOOL SendTo(VOID *head, int headlen, const SocketAddr& addr, VOID *data = NULL, int datalen = 0);
	BOOL SendTo(VOID *head, int headlen, const sockaddr* to, int tolen, VOID *data = NULL, int datalen = 0);
	BOOL RecvFrom(VOID* buf, int& len, sockaddr* from = NULL, int* fromlen = NULL);
	BOOL SetNonblocking(BOOL enable);
	BOOL SetSendTimeOut(int milliseconds);
	BOOL SetRecvTimeOut(int milliseconds);
	BOOL SetBroadcast(BOOL enable = TRUE);
	BOOL SetRecvBuf(int buflen);
	BOOL GetRecvBuf(int &buflen);
	S32  GetSocket() {return mSock;};
	BOOL SetSndBuf(int buflen);
	S32  GetSndBuf(int &buflen);

protected:
	BOOL Connect(const sockaddr* addr, int addrlen);

	template <typename T>
	BOOL GetSockopt(int optname, T& optval, int level = SOL_SOCKET)
	{
		int optlen = sizeof(T);
#ifdef _WIN32
		if (getsockopt(mSock, level, optname, (S8 *)&optval, (socklen_t *)&optlen) == 0)
#else
		if (getsockopt(mSock, level, optname, (VOID *)&optval, (socklen_t *)&optlen) == 0)
#endif
			return TRUE;

//		NetFunLastErr(mSock);
		return FALSE;
	}

	template <typename T>
	BOOL SetSockOpt(int optname, const T& optval, int level = SOL_SOCKET)
	{
#ifdef _WIN32
		if (setsockopt(mSock, level, optname, (const S8 *)&optval, sizeof(T)) == 0)
#else
		if (setsockopt(mSock, level, optname, (const VOID *)&optval, sizeof(T)) == 0)
#endif
			return TRUE;

//		NetFunLastErr(mSock);
		return FALSE;
	}
protected:
	int volatile mSock;
};

#endif // __Socket_h__
