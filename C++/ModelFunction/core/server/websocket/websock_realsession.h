/**
* @file      servent.h
* @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
* @brief     brief
* @author    caiwang213@qq.com
* @date      2017-05-31 11:35:57
*
* @note
*  servent.h defines
*/

#ifndef __WEBSOCK_SESSION_H__
#define __WEBSOCK_SESSION_H__

#include "realsession.h"
#include "client.h"

class WebSockRealSession : public RealSession
{
public:
    WebSockRealSession(Client *client, Bufferev* bev);
    virtual ~WebSockRealSession();

private:
    //virtual void       stop();

    virtual void       onRead(Bufferev *bev, void *data, uint32_t size);

    virtual  void      onResponsePacket(Bufferev *bev, uint8_t * data, uint32_t inlen);
};
#endif
