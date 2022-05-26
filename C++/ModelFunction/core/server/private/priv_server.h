/**
 * @file      privserver.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-05-11 13:47:13
 *
 * @note
 *  privserver.h defines
 */
#ifndef __PRIV_SERVMGR_H__
#define __PRIV_SERVMGR_H__
#include "server.h"
#include "sevent.h"
#include "tlist.h"

class Task;
class PrivServer : public tlist<Task *>, public Sevent, public Server
{
public:
    PrivServer(string bindip, int port);
    virtual ~PrivServer();

public:
    void               onTimer();

public:
    virtual int        start();
    virtual void       stop();

private:
    virtual void       onNewConnect(string srcip, int srcport, string destip, int destport , int fd);
};

extern PrivServer * privSV;
#endif
