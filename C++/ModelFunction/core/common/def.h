/**
 * @file      def.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:27:22
 *
 * @note
 *  def.h defines
 */
#ifndef __DEF_H__
#define __DEF_H__

#ifdef _WIN32
#pragma warning(disable: 4819)                          /**< do not work */
#pragma warning(disable: 4200)
#pragma warning(disable: 4996)
#pragma warning(disable: 4251)
#endif

#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>

#include <iostream>
#include <string>
#include <list>
#include <map>
#include <vector>
#include <queue>
#include <algorithm>
#include <stdexcept>

#ifdef _WIN32
#include <winsock2.h>
#include <windows.h>
#include <mmsystem.h>
#include <time.h>
#else

#include <unistd.h>
#include <netdb.h>
#include <arpa/inet.h>
#endif

#include "type.h"
#include "debug.h"

#ifdef CM_STATICLIB
#  define CM_API
#elif defined(WIN32) || defined(_WIN32)
#  if defined(CM_EXPORTS)
#    define CM_API  __declspec(dllexport)
#  else
#    define CM_API  __declspec(dllimport)
#  endif
#else
#  define   CM_API
#endif

#ifdef _WIN32
#define strcasecmp           _stricmp
#define strncasecmp          _strnicmp
#define snprintf             _snprintf
#if defined(_MSC_VER) && (_MSC_VER > 1700)
#define snprintf_s           _snprintf_s
#define vsnprintf_s          _vsnprintf_s
#else
#define snprintf             _snprintf
#define vsnprintf            _vsnprintf
#endif

#else
#define strcpy_s			 util_strcpy
#define strncpy_s            util_strncpy
#define sprintf_s            util_sprintf
#define snprintf_s           util_snprintf
#endif

#define MAX_IPV4_STR_LEN     (16)
#define MAX_HOST_LEN	     (128)
#define MAX_NAME_LEN	     (128)
#define MAX_ADDR_LEN         (24)

#if 0
#define MAX_PORT_STR_LEN     (6)
#define MAX_MAC_LEN          (6)
#define MAX_IPV4_LEN         (4)
#define MAX_IPV6_LEN         (16)
#define MAX_FILENAME_LEN     (128)
#define MAX_FILEPATH_LEN     (512)
#define MAX_UUID_LEN         (64)

#define MAX_URL_LEN          (128)

#define MAX_OBJECTNAME_LEN   (64)

#define MAX_LINE_SIZE        (1024)
#endif

#endif
