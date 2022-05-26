/**
 * @file      log4x.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-11-21 10:08:58
 *
 * @note
 *  log4x.h defines
 */
#ifndef __LOG4X_H__
#define __LOG4X_H__

#define LOG4X_DEFAULT_PATH             ("./log/")
#define LOG4X_DEFAULT_LEVEL            (LOG_LEVEL_DEBUG)
#define LOG4X_DEFAULT_DISPLAY          (true)
#define LOG4X_DEFAULT_OUTFILE          (true)
#define LOG4X_DEFAULT_MONTHDIR         (true)
#define LOG4X_DEFAULT_LIMITSIZE        (100)
#define LOG4X_DEFAULT_FILELINE         (true)
#define LOG4X_DEFAULT_RESERVE          (24*3600)

#define LOG4X_LOG_KEY_MAX              (20)
#define LOG4X_LOG_QUEUE_LIMIT_SIZE     (20000)
#define LOG4X_LOG_BUF_SIZE             (1024*8)
#define LOG4X_LOG_CONTAINER_DEPTH      (5)
#define LOG4X_RESERVE_FILE_COUNT       (7)

#define LOG4X_ALL_SYNCHRONOUS_OUTPUT   (false)
#define LOG4X_ALL_DEBUGOUTPUT_DISPLAY  (false)

#define LOG_FORMAT(key, level, func, file, line, logformat, ...) \
do { \
    log4x::ilog4x::instance()->push(key, level, func, file, line, logformat, ##__VA_ARGS__); \
}while(0)

/* format string */
#define LOGF_TRACE(key, fmt, ...)  LOG_FORMAT(key, LOG_LEVEL_TRACE, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGF_DEBUG(key, fmt, ...)  LOG_FORMAT(key, LOG_LEVEL_DEBUG, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGF_INFO(key,  fmt, ...)  LOG_FORMAT(key, LOG_LEVEL_INFO,  __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGF_WARN(key,  fmt, ...)  LOG_FORMAT(key, LOG_LEVEL_WARN,  __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGF_ERROR(key, fmt, ...)  LOG_FORMAT(key, LOG_LEVEL_ERROR, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)
#define LOGF_FATAL(key, fmt, ...)  LOG_FORMAT(key, LOG_LEVEL_FATAL, __FUNCTION__, __FILE__, __LINE__, fmt, ##__VA_ARGS__)

#define LOGFT(fmt, ...) LOGF_TRACE("main", fmt, ##__VA_ARGS__)
#define LOGFD(fmt, ...) LOGF_DEBUG("main", fmt, ##__VA_ARGS__)
#define LOGFI(fmt, ...) LOGF_INFO ("main", fmt, ##__VA_ARGS__)
#define LOGFW(fmt, ...) LOGF_WARN ("main", fmt, ##__VA_ARGS__)
#define LOGFE(fmt, ...) LOGF_ERROR("main", fmt, ##__VA_ARGS__)
#define LOGFF(fmt, ...) LOGF_FATAL("main", fmt, ##__VA_ARGS__)

#ifdef LOG4X_STATICLIB
#  define LOG4X_API
#elif defined(WIN32) || defined(_WIN32)
#  if defined(LOG4X_EXPORTS)
#    define LOG4X_API  __declspec(dllexport)
#  else
#    define LOG4X_API  __declspec(dllimport)
#  endif
#else
#  define   LOG4X_API
#endif

enum
{
    LOG_LEVEL_TRACE = 0,
    LOG_LEVEL_DEBUG,
    LOG_LEVEL_INFO,
    LOG_LEVEL_WARN,
    LOG_LEVEL_ERROR,
    LOG_LEVEL_FATAL,
};

namespace log4x
{
struct log4x_t;

class LOG4X_API ilog4x
{
public:
    ilog4x() {};
    virtual ~ilog4x() {};

public:
    virtual int        config(const char * path)          = 0;
    virtual int        configFromString(const char * str) = 0;

    virtual int        start() = 0;
    virtual void       stop()  = 0;

    virtual int        push(const char * key, int level, const char * func, const char * file, int line, const char* fmt, ...) = 0;

    virtual int        enable(const char * key, bool enable) = 0;
    virtual int        setpath(const char * key, const char * path) = 0;
    virtual int        setlevel(const char * key, int level) = 0;
    virtual int        setfileLine(const char * key, bool enable) = 0;
    virtual int        setdisplay(const char * key, bool enable) = 0;
    virtual int        setoutFile(const char * key, bool enable) = 0;
    virtual int        setlimit(const char * key, unsigned int limitsize) = 0;
    virtual int        setmonthdir(const char * key, bool enable) = 0;
    virtual int        setReserve(const char * key, unsigned int sec) = 0;

    virtual void       setAutoUpdate(int interval) = 0;
#if 0

    virtual bool       isEnable(const char * key) = 0;

    virtual long       totalCount()  = 0;
    virtual long       totalBytes()  = 0;
    virtual long       totalPush()   = 0;
    virtual long       totalPop()    = 0;
    virtual int        activeCount() = 0;
#endif
    virtual bool       isff(const char * key) = 0;

public:
    static ilog4x    * instance();
    static ilog4x    & reference()
    {
        return *instance();
    }
};

}

#endif
