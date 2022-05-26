/**
 * (C) Copyright 2013, XXXX Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Feb 26 16:45:16 PST 2013
 *
 * type defines.
 */

#ifndef __TYPE_H__
#define __TYPE_H__

typedef unsigned char           U8;
typedef unsigned char           UCHAR;
typedef unsigned short          U16;
typedef unsigned int            U32;
typedef unsigned long           ULONG;

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
//typedef ULONGLONG               U64;
//typedef LONGLONG                S64;
//songhongliang
typedef unsigned long long      U64;
typedef long long               S64;

typedef DWORD                   LDWORD;


//typedef char                    int8_t;
typedef unsigned char           uint8_t;
typedef short                   int16_t;
typedef unsigned short          uint16_t;
typedef int                     int32_t;
typedef unsigned int            uint32_t;
typedef __int64                 int64_t;
typedef unsigned __int64        uint64_t;
#else
typedef unsigned long long      U64;
typedef long long               S64;
typedef void                    VOID;
typedef int                     BOOL;

typedef long                    LONG;
typedef unsigned int            DWORD;
typedef void* 			        HANDLE;
typedef unsigned int            HWND;
typedef const char*             PCSTR;
typedef char*                   PCHAR;
typedef char                    CHAR;
typedef long                    LDWORD;
typedef wchar_t                 WCHAR;
#endif

typedef char                    S8;
typedef short                   S16;
typedef int                     S32;

typedef float                   FLOAT;
typedef double                  DOUBLE;

//typedef unsigned long           SIZE_T;

typedef unsigned char           BYTE;
typedef unsigned short          WORD;
typedef unsigned int            UINT;

typedef LONG *                  PLONG;
typedef PLONG                   LPLONG;

typedef U8                      ULONG8;
typedef U32                     ULONG32;
typedef S32                     INT32;
typedef long                    LLONG;



#endif 
