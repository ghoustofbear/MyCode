/**
 * @file      privclient.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-05-14 13:44:18
 *
 * @note
 *  websockclient.h defines
 */
#ifndef __WEBSOCK_CLIENT_H__
#define __WEBSOCK_CLIENT_H__
#include "client.h"

class WebSockClient : public Client
{
public:
    WebSockClient(Bufferev* bev, const string &name, const string &userId, const string &addrcode, uint64_t mright, uint64_t oright, string uk);
    virtual ~WebSockClient();

    virtual void       onAlarm(string &json, void *data, uint32_t size);
	void               onHeartbeat(string &json, void *data, uint32_t size);

protected:
    virtual void       stop();

    virtual void       onRead(Bufferev *bev, void *data, uint32_t size);
	virtual bool       buildResponsePacket(string &msg, char *stream, int &size);
};
#endif
