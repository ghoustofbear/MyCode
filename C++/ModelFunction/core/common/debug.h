/**
 * @file      debug.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:26:51
 *   
 * @note
 *  debug.h defines 
 */
#ifndef __DEBUG_H__
#define __DEBUG_H__
#include "log4x.h"

#undef  DEBUG
#if defined(_MSC_VER) && (_MSC_VER < 1400)
#define DEBUG(MOD, MSG) printf
#else
#define DEBUG(pszModeName, pszFmt, ...) \
	do {\
		printf("[%6s] [%s] " pszFmt, pszModeName, __FUNCTION__, ##__VA_ARGS__);\
		}while(0)
#endif

#undef  ASSERT
#include <assert.h>
#if defined(_MSC_VER) && (_MSC_VER < 1400)
#define ASSERT(exp)  ((void)((exp)?1:( printf("Assertion failed: [%s], func: [%s]\n"),\
						#exp, __FUNCTION__, abort(), 0 )) )
#elif defined(_MSC_VER) && (_MSC_VER > 1700)
#define ASSERT(exp)  ((void)( (exp)?1:( printf("Assertion failed: [%s], func: [%s], line: [%d], file: [%s]\n", \
						#exp, __FUNCTION__, __LINE__, __FILE__), abort(), 0)) )
#else
#define ASSERT(exp)   assert(exp)
#endif

#if defined(_WIN32) || (__cplusplus >= 201103L)
#define RPTLINE      (std::string("func: [") + __FUNCTION__ + "], line: [" + std::to_string(__LINE__) + "], file: [" +  __FILE__ "]")
#else
#define RPTLINE        (__FUNCTION__)
#endif
  
/* log4x */
#define LOG_LEVEL(key, level) \
do \
{ \
    LOGF_FATAL(key, "set %s debug level %d", key, level); \
    log4x::ilog4x::instance()->setlevel(key, level); \
} while (0)

#define LOG_RELEASE_LEVEL(key, level) \
do \
{ \
    if (!log4x::ilog4x::instance()->isff(key)) \
    { \
        LOG_LEVEL(key, level); \
    } \
} while (0)


#ifdef _DEBUG
/* setlevel > fromfile for debug version */
#define LOG4X_DEBUG_LEVEL(key, level) LOG_LEVEL(key, level)
#else
/* fromfile > setlevel for release version*/
#define LOG4X_DEBUG_LEVEL(key, level) LOG_RELEASE_LEVEL(key, level)
#endif

#define DEBUG_LEVEL_COM(level) LOG4X_DEBUG_LEVEL("com", level)
#define DEBUG_LEVEL_SDK(level) LOG4X_DEBUG_LEVEL("sdk", level)
#define DEBUG_LEVEL_PLY(level) LOG4X_DEBUG_LEVEL("ply", level)
#define DEBUG_LEVEL_MSV(level) LOG4X_DEBUG_LEVEL("msv", level)
#define DEBUG_LEVEL_CAP(level) LOG4X_DEBUG_LEVEL("cap", level)
#define DEBUG_LEVEL_SIP(level) LOG4X_DEBUG_LEVEL("sip", level)


#define LOGT_COM(fmt, ...)     LOGF_TRACE("com", fmt, ##__VA_ARGS__)
#define LOGD_COM(fmt, ...)     LOGF_DEBUG("com", fmt, ##__VA_ARGS__)
#define LOGI_COM(fmt, ...)     LOGF_INFO ("com", fmt, ##__VA_ARGS__)
#define LOGW_COM(fmt, ...)     LOGF_WARN ("com", fmt, ##__VA_ARGS__)
#define LOGE_COM(fmt, ...)     LOGF_ERROR("com", fmt, ##__VA_ARGS__)
#define LOGF_COM(fmt, ...)     LOGF_FATAL("com", fmt, ##__VA_ARGS__)

#define LOGT_SDK(fmt, ...)     LOGF_TRACE("sdk", fmt, ##__VA_ARGS__)
#define LOGD_SDK(fmt, ...)     LOGF_DEBUG("sdk", fmt, ##__VA_ARGS__)
#define LOGI_SDK(fmt, ...)     LOGF_INFO ("sdk", fmt, ##__VA_ARGS__)
#define LOGW_SDK(fmt, ...)     LOGF_WARN ("sdk", fmt, ##__VA_ARGS__)
#define LOGE_SDK(fmt, ...)     LOGF_ERROR("sdk", fmt, ##__VA_ARGS__)
#define LOGF_SDK(fmt, ...)     LOGF_FATAL("sdk", fmt, ##__VA_ARGS__)

#define LOGT_PLY(fmt, ...)     LOGF_TRACE("ply", fmt, ##__VA_ARGS__)
#define LOGD_PLY(fmt, ...)     LOGF_DEBUG("ply", fmt, ##__VA_ARGS__)
#define LOGI_PLY(fmt, ...)     LOGF_INFO ("ply", fmt, ##__VA_ARGS__)
#define LOGW_PLY(fmt, ...)     LOGF_WARN ("ply", fmt, ##__VA_ARGS__)
#define LOGE_PLY(fmt, ...)     LOGF_ERROR("ply", fmt, ##__VA_ARGS__)
#define LOGF_PLY(fmt, ...)     LOGF_FATAL("ply", fmt, ##__VA_ARGS__)

#define LOGT_MSV(fmt, ...)     LOGF_TRACE("msv", fmt, ##__VA_ARGS__)
#define LOGD_MSV(fmt, ...)     LOGF_DEBUG("msv", fmt, ##__VA_ARGS__)
#define LOGI_MSV(fmt, ...)     LOGF_INFO ("msv", fmt, ##__VA_ARGS__)
#define LOGW_MSV(fmt, ...)     LOGF_WARN ("msv", fmt, ##__VA_ARGS__)
#define LOGE_MSV(fmt, ...)     LOGF_ERROR("msv", fmt, ##__VA_ARGS__)
#define LOGF_MSV(fmt, ...)     LOGF_FATAL("msv", fmt, ##__VA_ARGS__)

#define LOGT_SIP(fmt, ...)     LOGF_TRACE("sip", fmt, ##__VA_ARGS__)
#define LOGD_SIP(fmt, ...)     LOGF_DEBUG("sip", fmt, ##__VA_ARGS__)
#define LOGI_SIP(fmt, ...)     LOGF_INFO ("sip", fmt, ##__VA_ARGS__)
#define LOGW_SIP(fmt, ...)     LOGF_WARN ("sip", fmt, ##__VA_ARGS__)
#define LOGE_SIP(fmt, ...)     LOGF_ERROR("sip", fmt, ##__VA_ARGS__)
#define LOGF_SIP(fmt, ...)     LOGF_FATAL("sip", fmt, ##__VA_ARGS__)

#define LOGT_CAP(fmt, ...)     LOGF_TRACE("cap", fmt, ##__VA_ARGS__)
#define LOGD_CAP(fmt, ...)     LOGF_DEBUG("cap", fmt, ##__VA_ARGS__)
#define LOGI_CAP(fmt, ...)     LOGF_INFO ("cap", fmt, ##__VA_ARGS__)
#define LOGW_CAP(fmt, ...)     LOGF_WARN ("cap", fmt, ##__VA_ARGS__)
#define LOGE_CAP(fmt, ...)     LOGF_ERROR("cap", fmt, ##__VA_ARGS__)
#define LOGF_CAP(fmt, ...)     LOGF_FATAL("cap", fmt, ##__VA_ARGS__)

#define LOGT_CLI(fmt, ...)     LOGF_TRACE("cli", fmt, ##__VA_ARGS__)
#define LOGD_CLI(fmt, ...)     LOGF_DEBUG("cli", fmt, ##__VA_ARGS__)
#define LOGI_CLI(fmt, ...)     LOGF_INFO ("cli", fmt, ##__VA_ARGS__)
#define LOGW_CLI(fmt, ...)     LOGF_WARN ("cli", fmt, ##__VA_ARGS__)
#define LOGE_CLI(fmt, ...)     LOGF_ERROR("cli", fmt, ##__VA_ARGS__)
#define LOGF_CLI(fmt, ...)     LOGF_FATAL("cli", fmt, ##__VA_ARGS__)


#endif
