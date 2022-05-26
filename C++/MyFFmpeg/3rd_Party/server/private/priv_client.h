/**
 * @file      privclient.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-05-14 13:44:18
 *
 * @note
 *  privclient.h defines
 */
#ifndef __PRIVCLIENT_H__
#define __PRIVCLIENT_H__
#include "client.h"

class PrivClient : public Client
{
public:
    PrivClient(Bufferev* bev, const string &name, const string &userId, const string &addrcode, uint64_t mright, uint64_t oright, string uk);
    virtual ~PrivClient();

protected:
    virtual void       stop();
	virtual bool       buildResponsePacket(string &msg, char *stream, int &size);
};
#endif
