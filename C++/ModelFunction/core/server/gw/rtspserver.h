/**
 * @file      rtspserver.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-12-26 11:53:29
 *
 * @note
 *  rtspserver.h defines
 */
#ifndef __RTSPSERVER_H__
#define __RTSPSERVER_H__
#include "server.h"
#include "sevent.h"
#include "tlist.h"
#include "rtspsession.h"

class RTSPServer : public Sevent, public Server, public tlist<RTSPSession*>
{
public:
    RTSPServer(int proto, string bindIp, int port);
    virtual ~RTSPServer();

public:
	void               onTimer();
    virtual int        start();
    virtual void       stop();

protected:
    virtual void       onNewConnect(string srcip, int srcport, string destip, int destport , int fd);
private:
};

extern RTSPServer * rtspSV;
#endif
