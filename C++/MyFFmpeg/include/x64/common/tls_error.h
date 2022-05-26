/*
 * @Author: liuxu
 * @Date: 2022-04-02 10:25:06
 * @LastEditors: liuxu
 * @LastEditTime: 2022-04-02 10:25:07
 * @FilePath: \C++\MyFFmpeg\src\MyFFmpeg\tls_error.h
 * @Description: 
 * 
 * Copyright (c) 2022 by liuxu/1056203382@qq.com, All Rights Reserved. 
 */
/**
 * @file      tls_error.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     last error get and set using TLS(Thread Local Storage) for thread safety
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:28:29
 *
 * @note
 *  tls_error.h defines
 */
#ifndef __TLS_ERROR_H__
#define __TLS_ERROR_H__
#include "def.h"

#define ERR_MAX_STRLEN  256
#define ERR_MAX_ARGS    5

#undef  ERROR
#define ERROR(mode, level, fmt, ...) \
    do {\
        setError("[%s] [%s] " fmt, mode, __FUNCTION__, ##__VA_ARGS__);\
        log4x::ilog4x::instance()->push(mode, level, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__); \
    }while(0)



#define ERRT_SDK(fmt, ...)  ERROR("sdk", LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)  
#define ERRD_SDK(fmt, ...)  ERROR("sdk", LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)  
#define ERRI_SDK(fmt, ...)  ERROR("sdk", LOG_LEVEL_INFO,  fmt, ##__VA_ARGS__)  
#define ERRW_SDK(fmt, ...)  ERROR("sdk", LOG_LEVEL_WARN,  fmt, ##__VA_ARGS__)  
#define ERRE_SDK(fmt, ...)  ERROR("sdk", LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)  
#define ERRF_SDK(fmt, ...)  ERROR("sdk", LOG_LEVEL_FATAL, fmt, ##__VA_ARGS__)

#define ERRT_COM(fmt, ...)  ERROR("com", LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)  
#define ERRD_COM(fmt, ...)  ERROR("com", LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)  
#define ERRI_COM(fmt, ...)  ERROR("com", LOG_LEVEL_INFO,  fmt, ##__VA_ARGS__)  
#define ERRW_COM(fmt, ...)  ERROR("com", LOG_LEVEL_WARN,  fmt, ##__VA_ARGS__)  
#define ERRE_COM(fmt, ...)  ERROR("com", LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)  
#define ERRF_COM(fmt, ...)  ERROR("com", LOG_LEVEL_FATAL, fmt, ##__VA_ARGS__)

#define ERRT_PLY(fmt, ...)  ERROR("ply", LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)  
#define ERRD_PLY(fmt, ...)  ERROR("ply", LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)  
#define ERRI_PLY(fmt, ...)  ERROR("ply", LOG_LEVEL_INFO,  fmt, ##__VA_ARGS__)  
#define ERRW_PLY(fmt, ...)  ERROR("ply", LOG_LEVEL_WARN,  fmt, ##__VA_ARGS__)  
#define ERRE_PLY(fmt, ...)  ERROR("ply", LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)  
#define ERRF_PLY(fmt, ...)  ERROR("ply", LOG_LEVEL_FATAL, fmt, ##__VA_ARGS__)

#define ERRT_MSV(fmt, ...)  ERROR("msv", LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)  
#define ERRD_MSV(fmt, ...)  ERROR("msv", LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)  
#define ERRI_MSV(fmt, ...)  ERROR("msv", LOG_LEVEL_INFO,  fmt, ##__VA_ARGS__)  
#define ERRW_MSV(fmt, ...)  ERROR("msv", LOG_LEVEL_WARN,  fmt, ##__VA_ARGS__)  
#define ERRE_MSV(fmt, ...)  ERROR("msv", LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)  
#define ERRF_MSV(fmt, ...)  ERROR("msv", LOG_LEVEL_FATAL, fmt, ##__VA_ARGS__)

#define ERRT_CAP(fmt, ...)  ERROR("cap", LOG_LEVEL_TRACE, fmt, ##__VA_ARGS__)  
#define ERRD_CAP(fmt, ...)  ERROR("cap", LOG_LEVEL_DEBUG, fmt, ##__VA_ARGS__)  
#define ERRI_CAP(fmt, ...)  ERROR("cap", LOG_LEVEL_INFO,  fmt, ##__VA_ARGS__)  
#define ERRW_CAP(fmt, ...)  ERROR("cap", LOG_LEVEL_WARN,  fmt, ##__VA_ARGS__)  
#define ERRE_CAP(fmt, ...)  ERROR("cap", LOG_LEVEL_ERROR, fmt, ##__VA_ARGS__)  
#define ERRF_CAP(fmt, ...)  ERROR("cap", LOG_LEVEL_FATAL, fmt, ##__VA_ARGS__)

typedef struct
{
    int            code;
    int            argc;
    char           key[ERR_MAX_STRLEN];
    char           errmsg[ERR_MAX_STRLEN * 2];

    union
    {
        void     * value_ptr;
        int        value_i;
        double     value_f;
        char       buf[ERR_MAX_STRLEN];
    } args[ERR_MAX_ARGS];
} Error;


CM_API long        setError(const char* fmt, ...);
CM_API long        vsetError(const char* fmt, va_list ap);
CM_API const char* getError();

#endif
