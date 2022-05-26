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
#ifndef __WSYS_H__
#define __WSYS_H__

#include "sys.h"

class CM_API WSys : public Sys
{
public:
    WSys();
    virtual ~WSys();

    virtual Socket    * createSocket();
    virtual void        sleep(uint32_t ms);

    virtual int         startThread(ThreadInfo &info);
    virtual void		endThread(ThreadInfo &info, int timeout = 50000);
    virtual void        detachThread(ThreadInfo &info);
    virtual long        currentThreadID();

    virtual void      * getTLSValue();
    virtual void        setTLSValue(void *value);

    virtual uint64_t    currentTimeMillis();
    virtual string      currentTime();
    virtual string      currentTimeMillis2Str(int format = 0);
    virtual int         setLocalTime(datetime_s &dt, const string &tzone);

    virtual string    & ascii2utf8(string& ascii);
    virtual string    & utf2ascii(string& utf8);

    virtual string      ascii2utf8(const char* ascii);
    virtual string      utf2ascii(const char*  utf8);

    virtual string      generateUID();

    virtual string      strftime(time_t now = 0);
    virtual string      strftime(datetime_s &);
    virtual int         strptime(string &s, datetime_s &);
    virtual int         strptime(uint64_t, datetime_s &);
    virtual uint64_t    datetime2int(datetime_s &time);

    virtual string      toString(int      value);
    virtual string      toString(uint32_t value);
    virtual string      toString(uint64_t value);

    virtual bool        copyFile(const char* source, const char* destination);
    virtual bool        removeFile(const char* source);

    virtual int         getPid(const char* process_name, const char* user) ;
    virtual bool        getCpuUsage(int pid, int &proUsed, int &sysUsed);
    virtual bool        getMemUsage(int pid, int &pusedM, int &vused, int &totalM, int &freeM, int &virTotalM, int &virFreeM);
    virtual bool        getDiskUsage(unsigned int &totalM, unsigned int &usedM);
    virtual bool        getInterfacUsage(const char* interfac, int upspeedKbs, int downspeedKbs);


    virtual bool        getCpuTime(int pid, unsigned long long &totalTime, unsigned long long &pidUserTime, unsigned long long &sysUserTime);
    virtual bool        getInterfaceIO(const char* interfac, unsigned long long& recvKb, unsigned long long& sendKb);

private:
    void              * m_curProcessHandle;
    int                 m_processCount;  
};

#endif



