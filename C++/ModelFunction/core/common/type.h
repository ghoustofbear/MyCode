/**
 * @file      type.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:29:27
 *
 * @note
 *  type.h defines
 */

#ifndef __TYPE_H__
#define __TYPE_H__

using namespace std;

#ifdef _WIN32
typedef HANDLE               THREAD;
#else
#include <pthread.h>
typedef void* 			     HANDLE;
typedef unsigned long        HWND;
//typedef void*                HWND;
typedef void*                HDC;
typedef pthread_t            THREAD;
#endif

#ifndef DATETIME_S
#define DATETIME_S
typedef struct
{
    int            year;
    int            month;
    int            day;
    int            hour;
    int            minute;
    int            second;
} datetime_s;
#endif

#endif
