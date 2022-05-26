/**
* @file      servmgr.h
* @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
* @brief     server define
* @author    caiwang213@qq.com
* @date      2017-05-31 11:36:06
*
* @note
*  servmgr.h defines
*
*  void setGuUi(OnUiUpdate *ui, int type = 0)
*  type 0-realstrem 1-playback/download stream
*/

#ifndef __WEBSOCKET_SERVER_H__
#define __WEBSOCKET_SERVER_H__
#include "server.h"
#include "sevent.h"
#include "tlist.h"

class Task;

class WebSocketServer : public tlist<Task *>, public Sevent, public Server
{
public:
    WebSocketServer(string bindip, int port);
    virtual ~WebSocketServer();

public:
    void               onTimer();

public:
    virtual int        start();
    virtual void       stop();

private:
    virtual void       onNewConnect(string srcip, int srcport, string destip, int destport, int fd);
};
extern WebSocketServer * websockSV;
#endif
