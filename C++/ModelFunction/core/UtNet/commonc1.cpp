#include "commonc1.h"
#include "excep.h"

#ifndef _WIN32
#include "uuid.h"
#endif

#define ENABLE_COMMON_DEBUG
#ifdef  ENABLE_COMMON_DEBUG 
#ifdef _WIN32
#define COMMON_DEBUG             printf
#else
#define COMMON_DEBUG(MSG...)     DEBUG("COMMON", MSG)
#endif /* end _WIN32 */
#else
#define COMMON_DEBUG(MSG...)
#endif /* end ENABLE_COMMON_DEBUG */

#include "proto_def.h"

/**
 * @remark
 */

static BOOL g_bInit = FALSE;
S32 InitCommon()
{
	if (g_bInit)
	{
		return 0;
	}

#ifdef _WIN32
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;

	wVersionRequested = MAKEWORD( 2, 0 );
	err = WSAStartup( wVersionRequested, &wsaData );
	if ( err != 0 )
	{
		COMMON_DEBUG("Unable to init sockets\n");
		return -1;
	}

#ifdef _DEBUG
	AllocConsole();
	freopen( "CONOUT$","w",stdout);
	//freopen( "./fstream.log","w+",stdout);
#endif

#else
#endif
	g_bInit = TRUE;

	COMMON_DEBUG("Init success!\n");
	return 0;
}

VOID Msleep(U32 u32MSec)
{
#ifdef _WIN32	
	::Sleep(u32MSec);
#else
	usleep(u32MSec*1000);
#endif
}

S32  GetDateTime(DATETIME_S *pDateTime)
{
	ASSERT(pDateTime);
	time_t ltime;
	struct tm *p_tm;

	ltime = time(NULL);
	p_tm = localtime(&ltime);

	pDateTime->u32Year   = p_tm->tm_year + 1900;
	pDateTime->u32Month  = p_tm->tm_mon + 1;
	pDateTime->u32Day    = p_tm->tm_mday;
	pDateTime->u32Hour   = p_tm->tm_hour;
	pDateTime->u32Minute = p_tm->tm_min;
	pDateTime->u32Second = p_tm->tm_sec;

	return 0;
}

S32  GetDateTime(string &strDatetime)
{
	time_t     ltime;
	struct tm *p_tm;
	S8         s8Time[40];

	ltime = time(NULL);
	p_tm = localtime(&ltime);

	snprintf(s8Time, 40, "%04d-%02d-%02d %02d:%02d:%02d",
			p_tm->tm_year + 1900,
			p_tm->tm_mon + 1,
			p_tm->tm_mday,
			p_tm->tm_hour,
			p_tm->tm_min,
			p_tm->tm_sec);

	strDatetime = s8Time;

	return 0;
}

S32  CompareDateTime(const DATETIME_S *pstTime1, const DATETIME_S *pstTime2)
{
	U64 u64Time1 = 0, u64Time2 = 0;
	
    u64Time1 = pstTime1->u32Year * 370 * DAY_SECOND + pstTime1->u32Month * 32 * DAY_SECOND +
        pstTime1->u32Day * DAY_SECOND + pstTime1->u32Hour * 3600 + pstTime1->u32Minute * 60 + pstTime1->u32Second;
	
	u64Time2 = pstTime2->u32Year * 370 * DAY_SECOND + pstTime2->u32Month * 32 * DAY_SECOND +
        pstTime2->u32Day * DAY_SECOND + pstTime2->u32Hour * 3600 + pstTime2->u32Minute * 60 + pstTime2->u32Second;

    if( u64Time1 == u64Time2 )
    {
        return 0;
    }
    else if( u64Time1 > u64Time2 )
    {
        return 1;
    }
    else
    {
        return  -1;
    }
}

ULONG32 YSFPos = 2;
ULONG32 YSFGas = 2;
struct node
{ 
	ULONG32      d;
	struct node *next;
};
INT32 YSFEncode(ULONG8 *data, ULONG32 readlen, ULONG32 pos, ULONG32 gap)
{ 
	ULONG8 b = 0, mark[8] = { 128 , 64 , 32 , 16 , 8 , 4, 2 , 1};
	struct node *head = NULL, *p = NULL, *q = NULL;
	ULONG32 i = 0, j = 0, k = 0, len = 0; 
	head = NULL;
	for ( i= 0 ; i< readlen ; i++ )
	{
		for( j= 0 ; j< 8 ; j++ )
		{ q = p ;
			p = (struct node *)malloc(sizeof(struct node)) ;
			if(p == NULL) 
			{
				goto FAIL;
			}
			p->d = ( (data[i] & mark[j] ) > 0) ;
			p->next = NULL;
			if ( head == NULL) head = p ; else q->next = p;
		}

	}
	p->next = head;

	len = 8 * readlen;
	pos = pos % len ;
	for ( i = 0 ; i < pos ; i++ ) p = p->next;
	for ( i = 0 ; i < readlen ; i++ )
	{ 
		b= 0;
		for ( j = 0 ; j< 8 ; j++ )
		{ 
			k = gap% len ;
			while ( k-- ) p = p-> next;
			q = p-> next;
			if ( q->d > 0) b = b | mark[j];
			p->next = q->next;
			free(q);
			len--;
		}
		data[i] = b;
	}
	printf("Encode size=%u pos=%u gas=%u\n", readlen, pos, gap);
	return 0;

FAIL:
	p = head;
	while(p)
	{
		q = p->next;
		free(p);
		p = q;
	}
	return -1;
}

INT32 YSFDecode(ULONG8  *data, ULONG32 readlen, ULONG32 pos, ULONG32 gap)
{ 
	ULONG8 mark[ 8] = { 128, 64 , 32, 16 , 8 , 4 , 2 , 1};
	ULONG8 * bin = NULL;
	struct node *head = NULL, *p = NULL, *q = NULL;
	ULONG32 i = 0, j = 0, k = 0, t = 0, len = 0, x = 0, y = 0;

	len = 8 * readlen;
	bin = (ULONG8 *)malloc (len * sizeof(ULONG8));
	k = 0;
	for ( i= 0 ; i< readlen ; i++ )
	{ 
		for( j= 0 ; j< 8 ; j++ )
		{ 
			bin[k] = ( ( data[i] & mark[j] ) > 0);
			k++;
		}
		data[i] = 0;
	}


	head = NULL;
	for( i = 0 ; i < len ; i++ )
	{ q = p;
		p = (struct node *) malloc (sizeof(struct node));
		if(p == NULL) {
			goto FAIL;
		}
		p->d = i;
		p->next = NULL;
		if ( head == NULL) head = p; else q-> next = p;
	}
	p->next = head;

	pos = pos % len ;
	for ( i= 0 ; i< pos ; i++ ) p = p-> next;
	k = len;
	for ( i = 0 ; i < k ; i++ )
	{ t = gap % len;
		while ( t-- ) p = p-> next;
		q= p-> next;
		if ( bin[i] > 0)
		{ x= q->d / 8;
			y = q-> d % 8;
			data[x] = data[x] | mark[y];
		}
		p->next = q->next;
		free(q);
		len--;
	}
	free(bin);
	printf("decode size=%u pos=%u gas=%u\n", readlen, pos, gap);
	return 0;

FAIL:
	p = head;
	if(bin) free(bin);
	while(p)
	{
		q = p->next;
		free(p);
		p = q;
	}
	return -1;
}	


S32 CreateJoinableThread(VOID *(*entry) (VOID *), VOID *param, THREAD *thread)
{
	ASSERT(thread);

#ifdef _WIN32
	U32 threadId;
	U32 trd;   
	trd = _beginthreadex(NULL, 0, (THREADENTRY_T)entry, param, 0, &threadId);
	if (0 == thread) {			
		return -1;
	}
	*thread = (HANDLE)trd;

#else
	if (0 != pthread_create(thread, NULL, (THREADENTRY_T)entry, param)) 
	{
		COMMON_DEBUG("pthread_create called error!\n");
		return -1;
	}
#endif

	return 0;
}

S32 CreateDetachThread(VOID *(*entry) (VOID *), VOID *param, THREAD *thread)
{
#ifdef _WIN32
	U32 threadId;
	U32 trd;   
	trd = _beginthreadex(NULL, 0, (THREADENTRY_T)entry, param, 0, &threadId);
	if (0 == trd) {
		return -1;
	} else {
		::CloseHandle((HANDLE)trd);
		return 0;
	}
#else
	THREAD ThreadID;
	pthread_attr_t attr;

	ASSERT( NULL != entry );

	pthread_attr_init( &attr );
	pthread_attr_setscope( &attr, PTHREAD_SCOPE_SYSTEM);           /* 绑定 */
	pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_DETACHED );  /* 分离 */
	if( pthread_create(&ThreadID, &attr, (THREADENTRY_T)entry, param) == 0 )
	{
		if (thread)
			*thread = ThreadID;

		pthread_attr_destroy( &attr );
		return 0;
	}

	pthread_attr_destroy( &attr );
	COMMON_DEBUG("pthread_create called error!\n");
#endif

	return -1;
}

S32 ThreadJoin(THREAD thread)
{
#ifdef _WIN32
	if (NULL == thread)
		return -1;
	::WaitForSingleObject(thread, INFINITE);
	::CloseHandle(thread);
#else
	pthread_join(thread, NULL);
#endif
	return 0;
}

S32 TcpListen(const S8 *host, const S8 *serv, socklen_t *addrlenp)
{
	S32 listenfd = -1;
	const S32 on = 1;
#ifdef _WIN32
	struct sockaddr_in localAddr;

	if ((listenfd = socket (PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
	{
		//		throw SockException("Can`t open socket");
	}

	setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, (const S8 *)&on, sizeof(on));

	U16 u16Port = atoi(serv);
	memset(&localAddr,0,sizeof(localAddr));
	localAddr.sin_family = AF_INET;
	localAddr.sin_port = htons(u16Port);
	if (host)
	{
		localAddr.sin_addr.s_addr = inet_addr(host); 
	}
	else
	{
		localAddr.sin_addr.s_addr = INADDR_ANY;
	}
	
	if( ::bind (listenfd, (sockaddr *)&localAddr, sizeof(localAddr)) == SOCKET_ERROR)
	{
		goto failed;
	}

	if (::listen(listenfd, SOMAXCONN) == SOCKET_ERROR)
	{
		goto failed;
	}

#else
	S32 n;
	struct addrinfo hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
	{
		COMMON_DEBUG("getaddrinfo called error: %s, %s: %s\n", host, serv, gai_strerror(n));
		goto failed;
	}
	ressave = res;

	do 
	{
		listenfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (listenfd < 0)
			continue; 

		setsockopt(listenfd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on));
		if (bind(listenfd, res->ai_addr, res->ai_addrlen) == 0)
			break;

		close(listenfd);
	} 
	while ( (res = res->ai_next) != NULL);

	if (res == NULL)
	{
		COMMON_DEBUG("errno from final socket() or bind(): %s, %s\n", host, serv);
		goto failed;
	}

	if (listen(listenfd, LISTENQ) < 0)
	{
		COMMON_DEBUG("listen called error: %s, %s\n", host, serv);
		goto failed;
	}

	if (addrlenp)
		*addrlenp = res->ai_addrlen;

	freeaddrinfo(ressave);
#endif

	return(listenfd);

failed:
	return -1;
	throw Exception(__FUNCTION__);
}

S32 UdpListen(const S8 *host, const S8 *serv, socklen_t *addrlenp)
{
	S32 sockfd = -1;
#ifdef _WIN32
	struct sockaddr_in localAddr;

	if ((sockfd = socket (PF_INET, SOCK_DGRAM, 0)) == -1)
	{
		return -1;
	}

	U16 u16Port = atoi(serv);
	memset(&localAddr,0,sizeof(localAddr));
	localAddr.sin_family = AF_INET;
	localAddr.sin_port   = htons(u16Port);
	localAddr.sin_addr.s_addr = INADDR_ANY;

	if( ::bind (sockfd, (sockaddr *)&localAddr, sizeof(localAddr)) == SOCKET_ERROR)
	{
		return -1;
	}
#else
	S32 n = 0;
	struct addrinfo hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_flags = AI_PASSIVE;
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
	{
		COMMON_DEBUG("udp_server error for %s, %s: %s",
				host, serv, gai_strerror(n));
		return -1;
	}
	ressave = res;

	do {
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd < 0)
			continue;

		if (bind(sockfd, res->ai_addr, res->ai_addrlen) == 0)
			break;

		close(sockfd);
	} while ( (res = res->ai_next) != NULL);

	if (res == NULL)
	{
		COMMON_DEBUG("udp_server error for %s, %s", host, serv);
		return -1;
	}

	if (addrlenp)
		*addrlenp = res->ai_addrlen;

	freeaddrinfo(ressave);
#endif

	return(sockfd);
}

/**
 * @remark 建立超时TCP连接
 * @host   主机IP,也可以是域名地址
 * @serv   端口号,也可以是服务地址
 * @nsec   超时时间
 */
S32 TcpConnect(const S8 *host, const S8 *serv, S32 nsec)
{
	S32				sockfd = -1;
	S32	            n;
	S32				flags = 0, error;
	socklen_t		len;
	fd_set			rset, wset;
	struct timeval	tval;

#ifdef _WIN32
	struct sockaddr_in servAddr;

	if ((sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
	{
		//throw SockException("Can`t open socket");
	}

	ULONG val = 1;
	ioctlsocket(sockfd, FIONBIO, &val);

	U16 u16Port = atoi(serv);
	memset(&servAddr,0,sizeof(servAddr));
	servAddr.sin_family      = AF_INET;
	servAddr.sin_port        = htons(u16Port);
	servAddr.sin_addr.s_addr = inet_addr(host);

	S32 addrLen = sizeof(servAddr);
	if (connect(sockfd, (SA *)&servAddr, addrLen) != 0 && 
		WSAGetLastError() != WSAEWOULDBLOCK )
	{
		close(sockfd);
		return -1;
	}

	FD_ZERO(&rset);
	FD_SET(sockfd, &rset);
	wset = rset;
	tval.tv_sec = nsec;
	tval.tv_usec = 0;

	if ( (n = select(sockfd + 1, &rset, &wset, NULL, nsec ? &tval : NULL)) == 0) 
	{
		close(sockfd);
		return -1;
	}
	else if (n > 0)
	{
		if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
			len = sizeof(error);
			if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, (char *)&error, &len) < 0) {
				close(sockfd);
				return -1;
			}
			if (error) {
				return -1;
			}
		}
	}
	else 
	{	
		close(sockfd);
		return -1;
	}

	val = 0;
	ioctlsocket(sockfd, FIONBIO, &val);

	return sockfd;
#else
	struct addrinfo	hints, *res, *ressave = NULL;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;

	if ((n = getaddrinfo(host, serv, &hints, &res)) != 0)
	{
		COMMON_DEBUG("getaddrinfo error %s, %s: %s\n", host, serv, gai_strerror(n));
		return -1;
	}

	ressave = res;
	do 
	{
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd < 0)
			continue;

		flags = fcntl(sockfd, F_GETFL, 0);
		fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);

		if ( (n = connect(sockfd, res->ai_addr, res->ai_addrlen)) < 0) {
			if (errno != EINPROGRESS)
			{
				COMMON_DEBUG("Connect error: %s, %s, %s\n", host, serv, strerror(errno));
				close(sockfd);
				sockfd = -1;
				goto done;
			}
		}

		if (n == 0)
			goto done;	/* connect completed immediately */

		FD_ZERO(&rset);
		FD_SET(sockfd, &rset);
		wset = rset;
		tval.tv_sec = nsec;
		tval.tv_usec = 0;

		if ( (n = select(sockfd + 1, &rset, &wset, NULL, nsec ? &tval : NULL)) == 0) 
		{
			/* 超时则尝试下一个 */
			close(sockfd);
			continue;
		}
		else if (n > 0)
		{
			if (FD_ISSET(sockfd, &rset) || FD_ISSET(sockfd, &wset)) {
				len = sizeof(error);
				if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &error, &len) < 0) {
					close(sockfd);
					sockfd = -1;
					goto done;
				}
				if (error) {
					COMMON_DEBUG("Select error: %s, %s, %s\n", host, serv, strerror(error));
					close(sockfd);
					sockfd = -1;
					goto done;
				}
			}

			break;
		}
		else 
		{	
			close(sockfd);
			sockfd = -1;
			goto done;
		}
		close(sockfd);	/* ignore this one */
	} while ( (res = res->ai_next) != NULL);

done:
	if (res == NULL)
	{
		COMMON_DEBUG("Connect error: %s, %s, %s\n", host, serv, strerror(errno));
		sockfd = -1;
	}

	fcntl(sockfd, F_SETFL, flags);
	freeaddrinfo(ressave);
#endif

	return sockfd;
}

S32 UdpConnect(const S8 *host, const S8 *serv)
{
	S32 sockfd = -1;
#ifdef _WIN32
#else
	S32             n;
	struct addrinfo hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
		COMMON_DEBUG("udp_connect error for %s, %s: %s",
				host, serv, gai_strerror(n));
	ressave = res;

	do {
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd < 0)
			continue;

		if (connect(sockfd, res->ai_addr, res->ai_addrlen) == 0)
			break;

		close(sockfd);
	} while ( (res = res->ai_next) != NULL);

	if (res == NULL)
		COMMON_DEBUG("udp_connect error for %s, %s", host, serv);

	freeaddrinfo(ressave);
#endif

	return(sockfd);
}

S32 UdpClient(const S8 *host, const S8 *serv, SA *saptr, socklen_t *lenp)
{
	S32 sockfd = -1;

#ifdef _WIN32
	memset(saptr, 0, sizeof(SA));

	struct sockaddr_in server;
	memset(&server, 0, sizeof(struct sockaddr_in));

	server.sin_family      = AF_INET;
	server.sin_port        = htons(atoi(serv));
	server.sin_addr.s_addr = inet_addr(host);

	memcpy(saptr, &server, sizeof(struct sockaddr_in));
	*lenp = sizeof(struct sockaddr_in);

	return socket(AF_INET, SOCK_DGRAM, 0);
#else
	S32 n;
	struct addrinfo hints, *res, *ressave;

	bzero(&hints, sizeof(struct addrinfo));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_DGRAM;

	if ( (n = getaddrinfo(host, serv, &hints, &res)) != 0)
	{ 
		COMMON_DEBUG("udp_client error for %s, %s: %s",
				host, serv, gai_strerror(n));
	}
	ressave = res;

	do {
		sockfd = socket(res->ai_family, res->ai_socktype, res->ai_protocol);
		if (sockfd >= 0)
			break;
	} while ( (res = res->ai_next) != NULL);

	if (res == NULL)
		COMMON_DEBUG("udp_client error for %s, %s", host, serv);

	memcpy(saptr, res->ai_addr, res->ai_addrlen);
	*lenp = res->ai_addrlen;

	freeaddrinfo(ressave);
#endif

	return(sockfd);
}

typedef enum _STATUS_
{       
	WRITE_STATUS,
	READ_STATUS,
	EXCPT_STATUS,
}IOSTATUS_E;
S32 Select( S32 sockfd, S32 sec, S16 stat )
{
	S32       n = 0; 
	S32       error = errno;
	socklen_t len;
	fd_set    fs;
	struct timeval timeout;

//	ASSERT( sockfd >= 0 );
	if ( sockfd < 0 )
		return -1;

	timeout.tv_sec  = sec;
	timeout.tv_usec = 0;
	FD_ZERO(&fs);
	FD_SET(sockfd, &fs);

	switch(stat)
	{
		case READ_STATUS:
			n = select( sockfd+1, &fs, 0, 0, &timeout );
			break;
		case WRITE_STATUS:
			n = select( sockfd+1, 0, &fs, 0, &timeout );
			break;
		case EXCPT_STATUS:
			n = select( sockfd+1, 0, 0, &fs, &timeout );
			break;
	}

	if (0 == n)
	{
		if ( READ_STATUS == stat )
		{
			COMMON_DEBUG("IO read time out!\n");
		}

		else if ( WRITE_STATUS == stat )
		{
			COMMON_DEBUG("IO write time out!\n");
		}

		return -1;
	}
	else if ( n > 0 )
	{
		if (FD_ISSET(sockfd, &fs))
		{
			len = sizeof(error);
#ifdef _WIN32
			if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR,  (S8 *)&error, &len) < 0)
#else
				if (getsockopt(sockfd, SOL_SOCKET, SO_ERROR,  &error, &len) < 0)
#endif
				{
					COMMON_DEBUG("%s\n", strerror(error));
					return -1;
				}

			if (error)
			{
				COMMON_DEBUG("%s\n", strerror(error));
				return -1;
			}
		}
	}
	else 
	{
		COMMON_DEBUG("%s\n", strerror(error));
		return -1;
	}

	return n;
}


S32 RecvTimeOut( S32 s32SockFd, S8 *pu8Buffer, S32 s32Size, S32 s32Sec)
{           
	S32 s32Ret;
	S16 s16Stat = READ_STATUS;

	if ( Select( s32SockFd, s32Sec, s16Stat) < 0 )
	{
		return -1;
	}

	s32Ret = recv( s32SockFd, pu8Buffer, s32Size, 0);
	if ( s32Ret <= 0 )
	{
		if ( 0 == s32Ret )
		{
			COMMON_DEBUG("Connect rest by peer!\n");
		}
		else
		{
			COMMON_DEBUG("%s\n", strerror(errno));
		}
		return -1;
	}

	return s32Ret;
}

S32 SendTimeOut( S32 s32SockFd, S8 *pu8Buffer, S32 s32Size, S32 s32Sec)
{           
	S32 s32Ret;
	S16 s16Stat = WRITE_STATUS;

	if ( Select( s32SockFd, s32Sec, s16Stat) < 0 )
	{
		return -1;
	}

	s32Ret = send( s32SockFd, pu8Buffer, s32Size, 0);
	if ( s32Ret < 0 )
	{
		COMMON_DEBUG("%s\n", strerror(errno));
		return -1;
	}

	return s32Ret;
}

/**
 * @remark 不带超时的完整性读
 * @param1 
 * @return 接收不完整返回-1, 成功返回0
 */
S32 ReadData( S32 s32SockFd, S8 *pu8Buffer, S32 s32Size)
{
	S32 s32LeftSize = s32Size;
	S32 s32ReadSize = 0;
	S32 s32Ret      = 0;

	ASSERT( pu8Buffer );

	while(s32LeftSize > 0)
	{
		s32Ret = recv( s32SockFd, pu8Buffer + s32ReadSize, s32LeftSize, 0);
		if ( s32Ret <= 0)
		{
			COMMON_DEBUG("%s\n", strerror(errno));
			break;
		}

		s32LeftSize -= s32Ret;
		s32ReadSize += s32Ret;
	}

	if ( s32ReadSize != s32Size )
		return -1;

	return 0;
}

/**
 * @remark 带超时的完整性读
 * @param1 
 * @param4 接收超时的秒数
 * @return 接收不完整返回-1, 成功返回0
 */
S32 ReadData( S32 s32SockFd, S8 *pu8Buffer, S32 s32Size, S32 s32Sec )
{
	S32 s32LeftSize = s32Size;
	S32 s32ReadSize = 0;
	S32 s32Ret      = 0;

	while(s32LeftSize > 0)
	{
		s32Ret = RecvTimeOut( s32SockFd, pu8Buffer + s32ReadSize, s32LeftSize, s32Sec );
		if ( s32Ret < 0)
		{
			break;
		}

		s32LeftSize -= s32Ret;
		s32ReadSize += s32Ret;
	}

	if ( s32ReadSize != s32Size )
	{
		COMMON_DEBUG(" s32ReadSize != s32Size\n" );
		return -1;
	}

	return 0;
}

/**
 * @remark 不带超时保证完整性发送
 * @param1 
 * @return 发送不完整返回-1, 成功返回0
 */
S32 WriteData( S32 s32SockFd, S8 *pu8Buffer, S32 s32Size)
{
	S32 s32LeftSize = s32Size;
	S32 s32ReadSize = 0;
	S32 s32Ret      = 0;

	while( s32LeftSize > 0)
	{
		s32Ret = send( s32SockFd, pu8Buffer + s32ReadSize, s32LeftSize, 0);
		if ( s32Ret < 0)
		{
			COMMON_DEBUG("%s\n", strerror(errno));
			break;
		}

		s32LeftSize -= s32Ret;
		s32ReadSize += s32Ret;
	}

	if ( s32ReadSize != s32Size )
		return -1;

	return 0;
}

/**
 * @remark 带超时保证完整性发送
 * @param1 
 * @param4 发送时的秒数
 * @return 发送不完整返回-1, 成功返回0
 */
S32 WriteData( S32 s32SockFd, S8 *pu8Buffer, S32 s32Size, S32 s32Sec )
{
	S32 s32LeftSize = s32Size;
	S32 s32WriteSize = 0;
	S32 s32Ret      = 0;

	while(s32LeftSize > 0)
	{
		s32Ret = SendTimeOut( s32SockFd, pu8Buffer + s32WriteSize, s32LeftSize, s32Sec );
		if ( s32Ret < 0)
		{
			return -1;
		}

		s32LeftSize -= s32Ret;
		s32WriteSize += s32Ret;
	}

	if ( s32WriteSize != s32Size )
	{
		COMMON_DEBUG("s32WriteSize != s32Size !\n");
		return -1;
	}

	return 0;
}

/**
 * @remarks   UDP接收数据
 * @s32SockFd 
 * @pu8Buffer 
 * @s32Size   
 * @pstSA
 * @s32AddrLen [in,out]
 * @return     返回接收到的字节数, 失败返回-1.
 */
S32 UdpReadData( S32 s32SockFd, S8 *pu8Buffer, S32 s32Size, SA *pstSA, S32 *s32AddrLen)
{
	S32 s32Ret = 0;
	s32Ret = recvfrom( s32SockFd, pu8Buffer, s32Size, 0, pstSA, (socklen_t *)s32AddrLen);
	if ( s32Ret < 0 )	
	{
		COMMON_DEBUG("%s\n", strerror(errno));
	}

	return s32Ret;
}

/**
 * @remarks   UDP发送数据
 * @s32SockFd 
 * @pu8Buffer 
 * @s32Size   
 * @pstSA
 * @return     成功返回0, 失败返回-1.
 */
S32 UdpWriteData( S32 s32SockFd, S8 *pu8Buffer, S32 s32Size, SA *pstSA, S32 s32AddrLen)
{
	S32 s32Ret = 0;
	s32Ret = sendto( s32SockFd, pu8Buffer, s32Size, 0, pstSA, (socklen_t)s32AddrLen);
	if ( s32Ret < 0 )	
	{
		COMMON_DEBUG("s32Size %d, %s\n", s32Size,strerror(errno));
	}

	return s32Ret;
}


/**
 * @remarks   返回与某个套接口关联的远程IP与端口
 * @s32SockFd 
 * @pu8PeerIp   [in,out]
 * @pu8PeerPort [in,out]
 * @return     返回已发送的字节数, 失败返回-1.
 */
S32 GetPeerIpAndPort( S32 s32SockFd, S8 *ps8PeerIp, S32 s32IpLen, S8 *ps8PeerPort, S32 s32PortLen )
{
	ASSERT( ps8PeerIp );
	ASSERT( ps8PeerPort );

	S32 s32Ret = 0;

#if 1 
	if (s32IpLen < MAX_IP_STR_LEN || 
			s32PortLen < MAX_PORT_STR_LEN )
	{
		return -1;
	}

	SA_IN stSA;
	S32 s32SALen = sizeof(SA_IN);
	if ( getpeername(s32SockFd , (SA *)&stSA, (socklen_t *)&s32SALen) < 0 )
	{
		printf("errror!\n");
		return -1;
	}
	S8 *ps8Ip = inet_ntoa(stSA.sin_addr);
	strncpy( ps8PeerIp, ps8Ip, s32IpLen);

	snprintf(ps8PeerPort, s32PortLen, "%d", ntohs(stSA.sin_port));
	COMMON_DEBUG("ps8PeerIp = %s, ps8PeerPort = %s\n", ps8PeerIp, ps8PeerPort);
#else
	struct sockaddr  *pstPeerAddr;
	S8         s8PeerIP[NI_MAXHOST]   = {0};
	S8         s8PeerPort[NI_MAXSERV] = {0};
	socklen_t  nSALen = sizeof(struct sockaddr);

	if ( getnameinfo( pstPeerAddr, nSALen, s8PeerIP, NI_MAXHOST, s8PeerPort, 
				NI_MAXSERV, NI_NUMERICHOST | NI_NUMERICSERV) != 0  )
	{
		return -1;
	}
	strncpy( ps8PeerIp,   s8PeerIP,   s32IpLen);
	strncpy( ps8PeerPort, s8PeerPort, s32PortLen);
	COMMON_DEBUG("ps8PeerIp = %s, ps8PeerPort = %s\n", ps8PeerIp, ps8PeerPort);
#endif

	return s32Ret;
}


string GetLocalIp(S32 s32Index)
{	
	InitCommon();

	char   hname[128];
    struct hostent *hent;
	
    gethostname(hname, sizeof(hname));
	
    hent = gethostbyname(hname);
	if (!hent)
	{
		return string("");
	}

#if 0
	int i;
    for(i = 0; hent->h_addr_list[i]; i++) {
        COMMON_DEBUG("%s\t", inet_ntoa(*(struct in_addr*)(hent->h_addr_list[i])));
    }
#endif
	
	string strIP = inet_ntoa(*(struct in_addr*)(hent->h_addr_list[s32Index]));

	return strIP;
}


VOID UTF8ToUnicode(WCHAR* pOut,char *pText)
{
	char* uchar = (char *)pOut;
	
	uchar[1] = ((pText[0] & 0x0F) << 4) + ((pText[1] >> 2) & 0x0F);
	uchar[0] = ((pText[1] & 0x03) << 6) + (pText[2] & 0x3F);

	return;
}

VOID UnicodeToGB2312(char* pOut,unsigned short uData)
{
#ifdef _WIN32
	WideCharToMultiByte(CP_ACP, NULL, (const wchar_t*)&uData, 1, pOut, sizeof(WCHAR), NULL, NULL);
#else
	wcstombs(pOut, (const wchar_t *)&uData, 2);
#endif
	
}

VOID Gb2312ToUnicode(WCHAR* pOut, char *gbBuffer)
{
#ifdef _WIN32
	MultiByteToWideChar(CP_ACP,MB_PRECOMPOSED, gbBuffer, 2, pOut, 1);
#else
	mbstowcs(pOut, gbBuffer, 2);
#endif
}

VOID UnicodeToUTF8(char* pOut, WCHAR* pText)
{
	/* 注意 WCHAR高低字的顺序,低字节在前，高字节在后 */ 
	char* pchar = (char *)pText;

	pOut[0] = (0xE0 | ((pchar[1] & 0xF0) >> 4));
	pOut[1] = (0x80 | ((pchar[1] & 0x0F) << 2)) + ((pchar[1] & 0xC0) >> 6);
	pOut[2] = (0x80 | (pchar[0] & 0x3F));
}

VOID GB2312ToUTF8(string& pOut,char *pText, int pLen)
{
	char buf[4];
	char* rst = new char[pLen + (pLen >> 2) + 2];
	
	memset(buf,0,4);
	memset(rst,0,pLen + (pLen >> 2) + 2);
	
	int i = 0;
	int j = 0;	
	while(i < pLen)
	{
	    /* 如果是英文直接复制就可以 */	
		if( *(pText + i) >= 0)
		{
			rst[j++] = pText[i++];
		}
		else
		{
			WCHAR pbuffer;
			Gb2312ToUnicode(&pbuffer,pText+i);
			
			UnicodeToUTF8(buf,&pbuffer);
			
			unsigned short int tmp = 0;
			tmp = rst[j] = buf[0];
			tmp = rst[j+1] = buf[1];
			tmp = rst[j+2] = buf[2];
			
			
			j += 3;
			i += 2;
		}
	}
	rst[j] = '\0';

	/* 返回结果 */	
	pOut = rst;		
	delete []rst;	
}

VOID UTF8ToGB2312(string &pOut, char *pText, int pLen)
{
	 char * newBuf = new char[pLen];
	 char Ctemp[4];
	 memset(Ctemp,0,4);

	 int i =0;
	 int j = 0;
	 
	 while(i < pLen)
	 {
	 	if(pText[i] > 0)
		{
			newBuf[j++] = pText[i++];			
		}
		else                 
		{
			WCHAR Wtemp;
			UTF8ToUnicode(&Wtemp,pText + i);
		
			UnicodeToGB2312(Ctemp,Wtemp);
		    
			newBuf[j] = Ctemp[0];
			newBuf[j + 1] = Ctemp[1];

			i += 3;    
			j += 2;   
		}
	 }
	 newBuf[j] = '\0';
	 
	 pOut = newBuf;
	 delete []newBuf;
}
#include <sys/timeb.h>
U64 GetSystemTimeMillis()
{
#ifdef _WIN32
	timeb tb;
	ftime(&tb);
	return tb.time*1000 + tb.millitm;
#else
	struct timeval tv;
	gettimeofday(&tv, NULL);
	
	return tv.tv_sec * 1000 + tv.tv_usec/1000;
#endif
}

string GenerateUuid()
{
	string strUuid = "";
	static char buf[64] = {0};
#ifdef _WIN32
	GUID guid;
	if (S_OK == ::CoCreateGuid(&guid))
	{
		snprintf(buf, sizeof(buf)
			, "{%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X}"
			, guid.Data1
			, guid.Data2
			, guid.Data3
			, guid.Data4[0], guid.Data4[1]
			, guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
			, guid.Data4[6], guid.Data4[7]
			);
	}
#else
	uuid_t uuid = {0};
	uuid_generate(uuid);
	uuid_unparse(uuid, buf);
#endif
	strUuid = buf;
	return strUuid;
}

string asc2utf8(const char *strAsc)
{
	static string strUtf8;
#ifdef _WIN32
	// ASCII转Unicode
	int      unicodeLen = MultiByteToWideChar(CP_ACP, 0, strAsc, -1, NULL, 0); 
	wchar_t *pwcUnicode = new wchar_t[unicodeLen+1]; 	
	memset(pwcUnicode, 0, (unicodeLen+1)*sizeof(wchar_t)); 
	MultiByteToWideChar(CP_ACP, 0, strAsc, -1, (LPWSTR)pwcUnicode, unicodeLen); 
	
	// Unicode转UTF-8
	int utf8Len   = WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)pwcUnicode, -1, NULL, 0, NULL, NULL); 
	BYTE *pszUtf8 = new BYTE[utf8Len+1]; 
	memset(pszUtf8, 0, utf8Len + 1); 
	WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)pwcUnicode, -1, (char *)pszUtf8, utf8Len, NULL, NULL); 
	
	strUtf8 = (char *)pszUtf8;
	
	delete []pwcUnicode;
	delete []pszUtf8; 
#else
	strUtf8 = strAsc;
#endif
	
	return strUtf8; 
}

string utf2asc(const char *strUtf8)
{
	string strAsc = "";
#ifdef _WIN32	
	// 获得转换后的长度，并分配内存	
	DWORD dwUnicodeLen  = MultiByteToWideChar(CP_UTF8, 0, strUtf8, -1, NULL, 0);	
	wchar_t *pwcUnicode = new wchar_t[dwUnicodeLen+1];	
	memset(pwcUnicode, 0, (dwUnicodeLen+1)*sizeof(wchar_t)); 

	// UTF-8转Unicode
	MultiByteToWideChar(CP_UTF8, 0, strUtf8, -1, pwcUnicode, dwUnicodeLen);
	
	// Unicode转ASCII
	int ascLen   = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)pwcUnicode, -1, NULL, 0, NULL, NULL); 
	char *pszASC = new char[ascLen+1]; 
	memset(pszASC, 0, ascLen + 1); 
	WideCharToMultiByte(CP_ACP, 0, (LPWSTR)pwcUnicode, -1, pszASC, ascLen, NULL, NULL); 
	
	// 转为string
	strAsc = pszASC;
	
	// 清除内存	
	delete []pwcUnicode;
	delete []pszASC;
#else
	strAsc = strUtf8;
#endif
	
	return strAsc;	
}

S32 split_name( S8 * name , S32 s32Len )
{
	S32 s32I = 0; 
	
	if (strlen(name) < s32Len)
	{ 		
		return strlen(name); 
	} 
	
	//从第１个字节开始判断 
	while( s32I < s32Len ) 
	{ 
		if ( name[s32I]>>7&1 && name[s32I+1]>>7&1)
			s32I = s32I + 2 ; 
		else 
			s32I = s32I + 1 ; 
	} 

	s32I = s32I > s32Len ? s32I-3 : s32I-1;	
	
	int iRealLen = s32I + 1;
	
	return iRealLen;
}

string GetHostByName(string strName)
{	
	InitCommon();
	
    struct hostent *hent;
	
    hent = gethostbyname(strName.c_str());
	if (!hent)
	{
		COMMON_DEBUG("gethostbyname fail!!\n");
		return string("");
	}
	
#if 0
	int i;
    for(i = 0; hent->h_addr_list[i]; i++) {
        COMMON_DEBUG("%s\t", inet_ntoa(*(struct in_addr*)(hent->h_addr_list[i])));
    }
#endif
	
	string strIP = inet_ntoa(*(struct in_addr*)(hent->h_addr_list[0]));

	return strIP;
}

S32  TcpSendPackaged_2( S32 s32SockFd, S8 *ps8Buffer, S32 s32Size, S32 s32FrameNO )
{
	ASSERT( ps8Buffer );
	ASSERT( s32Size > 0 );

	S32 s32PackageNO  = 0;
	S32 s32PackageNum = ( s32Size + MaxPackSize_2 - 1 ) / MaxPackSize_2;

	if( (s32Size <= 0) || ( s32Size > 2048 * 1024) )
	{
		return -1;
	}

	S32 s32HasSendSize = 0;
	S32 s32LeftSize    = s32Size;

	PACK_HEADER_S stPackHead;
	memset( &stPackHead, 0, sizeof(PACK_HEADER_S) );
	stPackHead.u2Version  = 1;
	stPackHead.u32FrameNo = s32FrameNO;
	stPackHead.u16PackNum = s32PackageNum;
	stPackHead.u8Type     = FramePack;

	while ( s32LeftSize > 0 )
	{
		stPackHead.u16PackNo   = s32PackageNO;
		stPackHead.u16PackSize = (s32LeftSize > MaxPackSize_2) ? MaxPackSize_2 : s32LeftSize;

		/* 发送包头 */
	    if ( send( s32SockFd, (S8 *)&stPackHead, sizeof(PACK_HEADER_S), 0) < 0 )
		{
		//	UTPROTO_DEBUG("Tcp send package header error: %s, s32SockFd: %d\n", strerror(errno), s32SockFd );
			return -1;
		}

		/* 发送数据 */
		if ( s32LeftSize > MaxPackSize_2 )
		{
	        if ( send( s32SockFd, ps8Buffer + s32HasSendSize, MaxPackSize_2, 0) < 0 )
			{
				return -1;
			}
			s32HasSendSize += MaxPackSize_2;
			s32LeftSize    -= MaxPackSize_2;
		}
		else
		{
	        if ( send( s32SockFd, ps8Buffer + s32HasSendSize, s32LeftSize, 0) < 0 )
			{
				return -1;
			}
			s32HasSendSize += s32LeftSize;
			s32LeftSize = 0;
		}
		s32PackageNO ++;
	}

	return 0;
}
