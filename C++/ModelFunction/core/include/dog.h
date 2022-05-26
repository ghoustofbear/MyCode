/**
 * @file      wsys.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     window Platform dependent interface definition
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:31:11
 *   
 * @note
 *  wsys.h defines 
 */

#ifndef __DOG_DSGADERSDGASD133434634_H__
#define __DOG_DSGADERSDGASD133434634_H__

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

CM_API void initDog();
CM_API void keepAlive(const char* keyWords);


#endif