/**
 * @file      exports.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-09-15 15:55:51
 *
 * @note
 *  exports.h defines
 */
#ifndef __EXPORTS_H__
#define __EXPORTS_H__

#ifdef _WIN32
#pragma warning(disable: 4996)
#endif

#include <stdint.h>
#include <string>
#include <list>

using namespace std;


#ifdef MS_STATICLIB
#  define MS_API
#elif defined(WIN32) || defined(_WIN32)
#  if defined(MS_EXPORTS)
#    define MS_API  __declspec(dllexport)
#  else
#    define MS_API  __declspec(dllimport)
#  endif
#else
#  define   MS_API
#endif

/* fix warning C4251 */
/* template class MS_API std::basic_string<char, std::char_traits<char>, std::allocator<char>>; */
#endif
