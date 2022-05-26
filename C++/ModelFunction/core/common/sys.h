/**
 * @file      sys.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:28:01
 *
 * @note
 *  sys.h defines
 */

#ifndef __SYS_H__
#define __SYS_H__

#include "def.h"

class ThreadInfo;
class Socket;
class CM_API Sys
{
public:
    Sys() {};
    virtual ~Sys() {};

    virtual Socket    * createSocket()     = 0;
    virtual void        sleep(uint32_t ms) = 0;

    virtual int         startThread(ThreadInfo &info)  = 0;
    virtual void		endThread(ThreadInfo &info, int timeout = 0) = 0;
    virtual void        detachThread(ThreadInfo &info) = 0;
    virtual long        currentThreadID() = 0;

    virtual void      * getTLSValue() = 0;
    virtual void        setTLSValue(void *value) = 0;

    virtual uint64_t    currentTimeMillis() = 0;
    virtual string      currentTime() = 0;
    virtual int         setLocalTime(datetime_s &dt, const string &tzone) = 0;
    virtual string      currentTimeMillis2Str(int format = 0) = 0;

    virtual string    & ascii2utf8(string& asc) = 0;
    virtual string    & utf2ascii(string& utf8) = 0;

    virtual string      ascii2utf8(const char* ascii) = 0;
    virtual string      utf2ascii(const char*  utf8) = 0;

    virtual uint32_t	rand();
    virtual string      generateUID() = 0;

    virtual string      strftime(time_t now = 0) = 0;
    virtual string      strftime(datetime_s &) = 0;
    virtual int         strptime(string &s, datetime_s &) = 0;
    virtual int         strptime(uint64_t, datetime_s &) = 0;
    virtual uint64_t    datetime2int(datetime_s &time) = 0;

    virtual string      toString(int      value) = 0;
    virtual string      toString(uint32_t value) = 0;
    virtual string      toString(uint64_t value) = 0;

    virtual bool        copyFile(const char* source, const char* destination) = 0;
    virtual bool        removeFile(const char* source) = 0;

    virtual int         getPid(const char* process_name, const char* user) = 0;
    virtual bool        getCpuUsage(int pid, int &proUsed, int &sysUsed) = 0;
    virtual bool        getMemUsage(int pid, int &pusedM, int &vused, int &totalM, int &freeM, int &virTotalM, int &virFreeM) = 0;
    virtual bool        getDiskUsage(unsigned int &totalM, unsigned int &usedM) = 0;
    virtual bool        getInterfacUsage(const char* interfac, int upspeedKbs, int downspeedKbs) = 0;


    virtual bool        getCpuTime(int pid, unsigned long long &totalTime, unsigned long long &pidUserTime, unsigned long long &sysUserTime) = 0;
    virtual bool        getInterfaceIO(const char* interfac, unsigned long long& recv, unsigned long long& send) = 0;


    /**
    * @brief ¨¨£¤3y¡Á?¡¤?¡ä??D?¨´¨®D??¡À¨º¡Á?¡¤?
    * @param [in] src       ?¡ä¡Á?¡¤?¡ä?
    * @param [in] target    ¨°a¨¨£¤3y¦Ì?¡Á?¡¤?
    * @return ¨¨£¤3yo¨®¦Ì?¡Á?¡¤?¡ä?
    */
    string StrRemoveChar(const string src, char target);
    /**
    * @brief ¨¬???¦Ì¨²index¡ä?3???¦Ì????¡§¡Á?¡¤?¡ä?
    * @param [in] src       ?¡ä¡Á?¡¤?¡ä?
    * @param [in] target    ¨¬????¡ã¦Ì?¡Á¨®¡ä?
    * @param [in] replacer  ¨¬???o¨®¦Ì?¡Á¨®¡ä?
    * @param [in] index     ¦Ì¨²index3???
    * @return true-3¨¦1|,false-¨º¡ì¡ã¨¹
    */
    bool StrFindReplace(string &src, const string target, const string replacer, int index = 1);

    /**
    * @brief ¨¨?¨°a????¡Á?¡¤?¡ä? ¡Áa?¡¥?a 10??????D¨ª
    * @param [in] src       ¨°a¡Áa??¦Ì?¡Á?¡¤?¡ä?
    * @param [in] target    ?-¡À?¦Ì?????(2~36)
    * @return 10??????D¨ª
    */
    uint64_t StrToDecimal(const string src, const int bit);


    int64_t cmpDatatime(datetime_s &value1, datetime_s &value2);
};

typedef void *(*THREAD_FUNC)(ThreadInfo *);

class CM_API ThreadInfo
{
public:
    ThreadInfo()
    {
        id     = 0;
        handle = 0;
        func   = NULL;
        data   = NULL;
    }

public:
    int              id;
    THREAD           handle;
    THREAD_FUNC      func;
    void           * data;
};


class CM_API GnuID
{
public:
    bool            isSame(GnuID &gid);
    bool            isSame(const string str);

    bool            isSet();
    void	        clear();

    void            generate(uint8_t flag = 0);

    string          toStr();
    void	        fromStr(const string str);

    uint8_t         getFlags();

    uint8_t         id[16];
    uint32_t        storeTime;
};

#define RAND(a,b) (((a = 36969 * (a & 65535) + (a >> 16)) << 16) + \
                  (b = 18000 * (b & 65535) + (b >> 16)) )

CM_API extern Sys *sys;
#endif
