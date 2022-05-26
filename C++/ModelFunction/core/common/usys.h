/**
 * @file      usys.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     linux/unix Platform dependent interface definition
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:29:51
 *
 * @note
 *  usys.h defines
 */

#ifndef __USYS_H__
#define __USYS_H__

#include "sys.h"

class USys : public Sys
{
public:
    USys();
    virtual ~USys();

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
    virtual int         setLocalTime(datetime_s &dt, const string &tzone);
    virtual string      currentTimeMillis2Str(int format = 0);

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
    virtual bool        removeFile(const char* path);

    virtual int         getPid(const char* process_name, const char* user);
    virtual bool        getCpuUsage(int pid, int &proUsed, int &sysUsed);
    virtual bool        getMemUsage(int pid, int &pusedM, int &vused, int &totalM, int &freeM, int &virTotalM, int &virFreeM);
    virtual bool        getDiskUsage(unsigned int &totalM, unsigned int &usedM);
    virtual bool        getInterfacUsage(const char* interface, int upspeedKbs, int downspeedKbs);

    virtual bool        getCpuTime(int pid, unsigned long long &totalTime, unsigned long long &pidUserTime, unsigned long long &sysUserTime);
    virtual bool        getInterfaceIO(const char* interface, unsigned long long& recv, unsigned long long& send);

private:
    const char*         getItems(const char* buffer,int ie);
    char *              deleteSpace(char* str);
};

#endif
