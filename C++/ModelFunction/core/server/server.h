/**
 * @file      server.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-11-14 11:11:53
 *
 * @note
 *  all server base
 */
#ifndef __SERVER_H__
#define __SERVER_H__
#include "def.h"

class Server
{
    friend class ServMgr;

public:
    enum
    {
        SV_PRIV_NEW,
        SV_PRIV_OLD,
        SV_SIPUA_GWA,
        SV_RTSP_GWA,
        SV_HTTP_GWA,
        SV_SIPUA_HS,
        SV_RTSP_HS,
        SV_WEBSOCK,
        SV_WEB,
    };

     enum 
     {
         P_GWA      = 4,
         P_GWB      = 5,
         P_GWB_HS   = 6,
         P_GWB_HIK  = 7,
         P_GB28181  = 8,
         P_GWB_UT   = 16,
     };


protected:
    Server(int transport, string bindIp, int port);
    virtual ~Server();

public:
    int                type();
    int                status();

    virtual int        port();

    virtual int        start() = 0;
    virtual void       stop()  = 0;

    string             ip()  { return m_bindIp; }

protected:
    virtual void       onTimer();

protected:
    volatile int       m_status;
    int                m_type;

    int                m_transport;                        /* IPPROTO_TCP/IPPROTO_UDP */
    string             m_bindIp;
    int                m_listenPort;
};

#endif
