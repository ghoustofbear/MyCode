/**
 * (C) Copyright 2011, RaySharp Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 * Tue Feb 26 16:45:16 PST 2013
 *
 * Common defines.
 */

#ifndef __DEF_H__
#define __DEF_H__

#ifdef _WIN32
#pragma warning( disable: 4251 )
#pragma warning( disable: 4786 )
#pragma warning( disable: 4200)
#pragma warning( disable: 4005)
#pragma warning( disable: 4786)

#include <process.h>    /* _beginthread(ex), _endthread(ex) */
#include <time.h>
#define close        closesocket
#define snprintf    _snprintf
#define strcasecmp   stricmp
#define strncasecmp  strnicmp
//#define llabs        abs
#else
#include <unistd.h>
#include <pthread.h>
#include <semaphore.h>
#include <sys/socket.h>  
#include <linux/rtnetlink.h> /* for NETLINK_ROUTE  */
#include <arpa/inet.h>       /* for inet_ntoa */
#include <net/if.h>          /* for "struct ifconf","struct ifreq" */
#include <sys/un.h>
#include <sys/wait.h>
#include <sys/ioctl.h>
#include <netdb.h>
#include <sys/prctl.h>
#include <sys/time.h>
#endif

//#include <stdlib.h>
//#include <stdio.h>
#include <assert.h>
#include <sys/types.h>   
#include <sys/stat.h>    


#include <errno.h>
#include <fcntl.h>
#include <signal.h>

#include <list>
#include <map>
#include <math.h>
#include <algorithm>  
#include <iostream>
#include <string>
#include <vector>
using namespace std;

#include "typec1.h"
#include "debugc1.h"

// 通用宏定义
#define LISTENQ     500

#ifdef _WIN32
#else
#define TRUE     (1)
#define FALSE    (0)
#define __stdcall 
#define CALLBACK  
#endif

#ifdef _WIN32
#ifdef CONFIG_DLL
#define CM_API __declspec(dllexport)
#else
#define CM_API __declspec(dllimport)
#endif
#else
#define CM_API
#endif

#define MAX_NAME_LEN	    (36)
#define MAX_ADDR_LEN        (24)
#define MAX_IP_STR_LEN      (16)
#define MAX_PORT_STR_LEN    (6)
#define MAX_MAC_LEN         (6)
#define MAX_IPV4_LEN        (4)
#define MAX_IPV6_LEN        (16)
#define MAX_FILENAME_LEN    (128)

#define MAX_URL_LEN         (128)

#define MAX_OBJECTNAME_LEN  (64)
#define MAX_TOURLINETM_NUM    (20)

#define MAX_AIOBJ_NUM       (6)
#define MAX_OBJ_PRESSPLATE  (144)

#define UT_I420			       (1)
#define UT_YV12                (2)
#define UT_YUV422			   (3)
#define UT_UYVY                (4)
#define UT_RGB32               (5)

// 结构体重定义
#ifdef _WIN32
typedef U32 (__stdcall *THREADENTRY_T)(VOID *);
typedef HANDLE                  THREAD;
typedef struct sockaddr_in      SA_STORAGE;
typedef int                     socklen_t;
#else
typedef VOID *(*THREADENTRY_T)(VOID *);
typedef pthread_t               THREAD;
typedef struct sockaddr_storage SA_STORAGE;
#endif
typedef struct sockaddr_in      SA_IN;
typedef struct sockaddr         SA;


#ifndef _DATETIME_
#define _DATETIME_
typedef struct _DATETIME_S_
{
	U32 u32Year;
	U32 u32Month;
	U32 u32Day;
	U32 u32Hour;
	U32 u32Minute;
	U32 u32Second;
} DATETIME_S;
#endif


//搜索录像文件类型
#define RECORD_NORMAL	0x1                   /**< 常规录像 */
#define RECORD_ALARM	0x2                   /**< 报警录像 */
#define RECORD_TIME     0x4                   /**< 定时录像 */
typedef struct _RECFILE_INFO_S_
{
	S32        s32ChnNO;
	U32        u32Type;                       /**< RECORD_NORMAL, RECORD_ALARM, RECORD_TIME */
	U32        u32Size;
	DATETIME_S stStart;
	DATETIME_S stEnd;
	S8         s8FileName[MAX_FILENAME_LEN];
} RECFILE_INFO_S;

/**
 * 通用功能宏 
 * 
 */

/* Ip地址字节转字符串 */
#define INET_NTOP(u8Ip, strIp) \
	do \
	{ \
	if (!strIp) \
		break; \
	snprintf(strIp, MAX_IP_STR_LEN, "%d.%d.%d.%d",  \
				u8Ip[0],  \
				u8Ip[1],  \
				u8Ip[2],  \
				u8Ip[3]); \
	} while (0)

/* Ip地址字符串转字节 */
#define INET_PTON(strIp, u8Ip) \
	do \
	{ \
	if (!strIp) \
		break; \
	U32 u32Ip[MAX_IPV4_LEN] = {0};  \
	sscanf(strIp, "%d.%d.%d.%d", \
				&u32Ip[0],  \
				&u32Ip[1],  \
				&u32Ip[2],  \
				&u32Ip[3]); \
				S32 s32Y = 0; \
				for (s32Y = 0; s32Y < MAX_IPV4_LEN; s32Y++) \
		{ \
		u8Ip[s32Y] = u32Ip[s32Y]; \
		} \
	} while (0)

/* 断言 */
#ifndef C_ASSERT
#define C_ASSERT(e) typedef char __C_ASSERT__[(e)?1:-1]
#endif

#ifndef ENSURE_RETURN_VAL
#define ENSURE_RETURN_VAL(expr, val) \
	do {                             \
	int _condVal=!!(expr);           \
	ASSERT(_condVal);                \
	if(!(_condVal)) return val;      \
	} while (0) 
#endif

#endif
