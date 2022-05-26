/**
 * @file      climgr.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:35:51
 *   
 * @note
 *  climgr.h defines 
 */
#ifndef __CLIMGR_H__
#define __CLIMGR_H__
#include "def.h"
#include "tlist.h"
#include "thread.h"

class Client;
class Bufferev;

class CliMgr : public tlist<Client *>
{
    enum
    {
        T_COLLECTOR,
        T_TIMER,
    };

public:
    CliMgr();
    virtual ~CliMgr();

public:
    int                startPreview(string &json, string key, Bufferev *bev, uint32_t &code);
    int                startPlayback(string &json, string key, Bufferev *bev, uint32_t &code);
    int                startDownload(string &json, string key, Bufferev *bev, uint32_t &code);


    int                startTalk(string &json, string key, Bufferev *bev, uint32_t &code);

public:
    int                start();
    void               stop();

    Client *           addClient(Bufferev* bev, int type, const string &name, const string &userId, const string &addrcode, uint64_t mright, uint64_t oright, string userKey);
    Client *           findClient(string key);

    void               onTimer();
private:
    void               release();
    void               releaseDead();

    int                collector();

    static void *      threadEntry(ThreadInfo *arg);

private:
    Thread             m_collecThread;
    Thread             m_timerThread;
};

extern CliMgr* cliMgr;
#endif
