/**
 * @file      socket.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:27:42
 *   
 * @note
 *  socket.h defines 
 */
#ifndef __SOCKET_H__
#define __SOCKET_H__

#include "def.h"
#include "stream.h"

#define C2WATCHDOG_SOCK_FILE   "/tmp/c2watchdog_socket"

class CM_API Host
{
public:
    Host(uint32_t ip, int port);
    Host(string str, int port);
    Host();
    virtual ~Host();

    string             toStr();
    int                fromStr(const string str, int p);

    string             getIPStr();

    int                getPort() { return port; }
    bool               operator==(const Host &h) const { return ip == h.ip && port == h.port; }

    union
    {
        uint32_t       ip;
        uint8_t        ipByte[4];
    };

    uint16_t           port;
};

class Socket : public Stream
{
public:
    Socket();
    virtual ~Socket();

    virtual int        open(Host &, int type = IPPROTO_TCP) = 0;
    virtual int        bind(Host &, int type = IPPROTO_TCP) = 0;
    virtual int        connect() = 0;
    virtual Socket   * accept()  = 0;

    virtual void       setBlocking(bool yes) = 0;
    virtual void       setReuse(bool yes);

    virtual bool       active()                     { return m_sock != -1; }
    virtual void       setReadTimeout (uint32_t ms) { m_readTimeout = ms; }
    virtual void       setWriteTimeout(uint32_t ms) { m_writeTimeout = ms; }

    virtual int        checkTimeout(bool r, bool w) = 0;

    virtual int        select(bool r, bool w, int timeoMs) = 0;

    void               setSocket(int sock) { m_sock = sock; }
    int                getSocket()         { return m_sock; }

    Host &             getHost()       { return m_host; }
    void               setHost(Host h) { m_host = h; }

    virtual int        readFrom(void *buf, int size, sockaddr *sa, int *saLen) = 0;
    virtual int        writeTo(const void *buf, int size, sockaddr *sa = NULL, int saLen = 0) = 0;

    virtual string     getPeerIP() = 0;
    virtual int        getPeerPort() = 0;

    static uint32_t    getIP(string hostname);
//	static bool		   getHostname(char *, unsigned int);

protected:
    int                m_sock;
    Host               m_host;
    struct sockaddr_in m_remoteAddr;

    uint32_t           m_readTimeout;
    uint32_t           m_writeTimeout;
};

#endif
