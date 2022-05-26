/**
 * (C) Copyright 2013, xxx Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Feb 26 16:45:16 PST 2013
 *
 * Common functions define.
 */

#ifndef __COMM_FUNCS_H__
#define __COMM_FUNCS_H__
#include "defc1.h"

#define DAY_SECOND 86400UL
#define BUFSIZE    8192

#ifndef MIN
#define MIN(x,y) ((x) < (y) ? (x) : (y))
#endif
#ifndef MAX
#define MAX(x,y) ((x) > (y) ? (x) : (y))
#endif

#define TELNET_CMD_GET       1
#define TELNET_CMD_SET       2
#define TELNET_BUF_SIZE      (512)

typedef struct _THREADARG_S_
{
	U8     u8Func;
	VOID * pClass;
	VOID * pArg;
}THREAD_ARG_S;

typedef VOID  Sigfunc( S32 );

/* 通用函数 */
CM_API S32  InitCommon();
CM_API U32  GetRandom(S32 s32Val);
CM_API VOID Msleep(U32 u32MSec);
CM_API S32  GetDateTime(DATETIME_S *pDateTime);
CM_API S32  GetDateTime(string &strDatetime);
CM_API S32  CompareDateTime(const DATETIME_S *pstTime1, const DATETIME_S *pstTime2);

/* 约瑟夫数据不变成 加/解 密 */
CM_API INT32 YSFEncode(ULONG8 *data, ULONG32 readlen, ULONG32 pos, ULONG32 gap);
CM_API INT32 YSFDecode(ULONG8 *data, ULONG32 readlen, ULONG32 pos, ULONG32 gap);


/* 线程函数 */
CM_API S32 CreateJoinableThread(VOID *(*entry)(VOID *), VOID *param, THREAD *thread);
CM_API S32 CreateDetachThread(VOID *(*entry)(VOID *), VOID *param, THREAD *thread);
CM_API S32 ThreadJoin(THREAD thread);

/* 网络函数 */
CM_API S32 Select(S32 sockfd, S32 sec, S16 stat);
CM_API S32 TcpListen(const S8 *host, const S8 *serv, socklen_t *addrlenp);
CM_API S32 TcpConnect(const S8 *host, const S8 *serv, S32 nsec);
CM_API S32 UdpListen(const S8 *host, const S8 *serv, socklen_t *addrlenp);
CM_API S32 UdpConnect(const S8 *host, const S8 *serv);
CM_API S32 UdpClient(const S8 *host, const S8 *serv, SA *saptr, socklen_t *lenp);
CM_API S32 ReadData( S32 s32SockFd, S8 *pu8Buffer, S32 s32Size );            /* 不带超时保证完整性读 */
CM_API S32 ReadData( S32 s32SockFd, S8 *pu8Buffer, S32 s32Size, S32 s32Sec); /* 带超时保证完整性读 */
CM_API S32 WriteData( S32 s32SockFd, S8 *pu8Buffer, S32 s32Size );            /* 不带超时保证完整性读 */
CM_API S32 WriteData( S32 s32SockFd, S8 *pu8Buffer, S32 s32Size, S32 s32Sec); /* 带超时保证完整性读 */

CM_API S32 UdpReadData( S32 s32SockFd, S8 *pu8Buffer, S32 s32Size, SA *pstSA, S32 *s32AddrLen);
CM_API S32 UdpWriteData( S32 s32SockFd, S8 *pu8Buffer, S32 s32Size, SA *pstSA, S32 s32AddrLen);

CM_API S32 GetPeerIpAndPort( S32 s32SockFd, S8 *ps8PeerIp, S32 s32IpLen, S8 *ps8PeerPort, S32 s32PortLen = 6 );
CM_API S32 GetIpAndNetMask( U8 *pu8IpAddr, U8 *pu8NetMask, const S8 *pu8IfName = "eth0" );
CM_API S32 GetGateWay( U8 *pu8GateWay, const S8 *pu8IfName = "eth0" );
CM_API BOOL GetNetLinkState( const S8 *pu8IfName = "eth0" ); /* 只能检测网线是否拔出 */
CM_API S32  GetDnsAddr( U8 *pu8Dns );
CM_API string GetLocalIp(S32 s32Index = 0);
CM_API string GetHostByName(string strName);

/* Unicode 转换成UTF-8 */
CM_API VOID UnicodeToUTF8(char* pOut,WCHAR* pText);

/* GB2312 转换成　Unicode */
CM_API VOID Gb2312ToUnicode(WCHAR* pOut,char *gbBuffer);

/* 把Unicode 转换成 GB2312 */
CM_API VOID UnicodeToGB2312(char* pOut,unsigned short uData);

/* 把UTF-8转换成Unicode */
CM_API VOID UTF8ToUnicode(WCHAR* pOut,char* pText);

CM_API VOID UTF8ToGB2312(string& pOut,char *pText, int pLen);
CM_API VOID GB2312ToUTF8(string& pOut,char *pText, int pLen);

CM_API string asc2utf8(const char *strAsc);
CM_API string utf2asc(const char *strUtf8);

CM_API U64    GetSystemTimeMillis();

CM_API string GenerateUuid();

//分割字符串，考虑中文不被分成乱码，返回分割的左侧长度
CM_API S32 split_name( S8 * name , S32 len );

CM_API S32 TcpSendPackaged_2( S32 s32SockFd, S8 *ps8Buffer, S32 s32Size, S32 s32FrameNO );

#define GET_YEAR(_time_)          (((_time_)>>26) + 2000) 
#define GET_MONTH(_time_)       (((_time_)>>22) & 15)
#define GET_DAY(_time_)           (((_time_)>>17) & 31)
#define GET_HOUR(_time_)         (((_time_)>>12) & 31)
#define GET_MINUTE(_time_)      (((_time_)>>6) & 63)
#define GET_SECOND(_time_)     (((_time_)>>0) & 63) 

#endif
