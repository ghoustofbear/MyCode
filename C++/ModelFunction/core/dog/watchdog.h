/**
 * @file      watchdog.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2019-05-31 11:32:59
 *
 * @note
 *  watch dog, use local socket
 */
#ifndef __WATCHDOG_H__
#define __WATCHDOG_H__
#include "sys.h"
#include "tmap.h"

class Thread;
class Socket;
class WatchDog
{
public:
    WatchDog();
    ~WatchDog();

    int                        start();
    int                        stop();
    void                       timeDriver();
	string                     getwatchdogid() { return m_watchdogid; }
#ifdef _WIN32
	struct tm*                 getcurrtime();
#endif

protected:
    void                       loop();
    static  void             * threadProc(ThreadInfo *);

private:
    void                       parseJson();
    int                        rebootApp(string key);

private:
    struct CIS
    {
        string  exeName;
        string  workPath;
        string  key;
        string  argv;
        int     pid;
    };

	int                        m_timeOut;
	int                        m_port;
	string                     m_watchdogid;
    Thread                   * m_thread;
	datetime_s                  m_timeReboot;
	bool                       istimerboot; //定时重启标志

#ifdef _WIN32
    Socket                   * m_sock;
#else
    int                        m_sockFd;
#endif
    tmap<string, uint64_t>     m_programMapLast;
    tmap<string, CIS*>         m_programMapInfo;
};




#endif
