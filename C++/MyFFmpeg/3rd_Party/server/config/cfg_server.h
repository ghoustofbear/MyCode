/**
 * @file      cfg_server.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-11-17 15:17:38
 *   
 * @note
 *  cfg_server.h defines 
 */
#ifndef __CFG_SERVER_H__
#define __CFG_SERVER_H__
#include "observable.h"
#include "cfg.h"

class ServerCfg: public Cfg, public Observable
{
public:
    ServerCfg(int type);
    virtual ~ServerCfg();

    virtual int        add(const string& json);
    virtual int        del(const string& json);
    virtual int        modify(const string& json);

    virtual int        flush(const string& json);
    virtual int        query(string& json, string condition = "");
    virtual int        clear(const string& json);
};

MS_API extern ServerCfg* servCfg;
#endif
