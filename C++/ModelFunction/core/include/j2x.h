/**
 * @file      j2x.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-12-01 21:48:35
 *   
 * @note
 *  j2x.h defines 
 */
#ifndef __J2X_H__
#define __J2X_H__
#ifdef J2X_STATICLIB
#  define J2X_API
#elif defined(WIN32) || defined(_WIN32)
#  if defined(J2X_EXPORTS)
#    define J2X_API  __declspec(dllexport)
#  else
#    define J2X_API  __declspec(dllimport)
#  endif
#else
#  define   J2X_API
#endif
#include <string>

J2X_API int xml2json(const char *xml, std::string &json);
J2X_API int json2xml(const char *json, std::string &xml);

#endif
