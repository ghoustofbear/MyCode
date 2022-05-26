/**
 * @file      stk_exports.h
 * @copyright Copyright (c) 2017, UT Technology Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-09-15 17:11:01
 *
 * @note
 *  stk_exports.h defines
 */
#ifndef __STK_EXPORTS_H__
#define __STK_EXPORTS_H__

#ifdef STK_STATICLIB
#  define STK_API
#elif defined(WIN32) || defined(_WIN32)
#  if defined(STK_EXPORTS)
#    define STK_API  __declspec(dllexport)
#  else
#    define STK_API  __declspec(dllimport)
#  endif
#else
#  define   STK_API
#endif

#ifdef _WIN32
#define strcasecmp           _stricmp
#define strncasecmp          _strnicmp
#define snprintf             _snprintf
#endif

#endif
