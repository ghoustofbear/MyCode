/**
 * @file      cfg_system.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-11-17 15:17:38
 *   
 * @note
 *  cfg_system.h defines 
 */
#ifndef __CFG_TIME_H__
#define __CFG_TIME_H__
#include "observable.h"
#include "cfg.h"

class TimeCfg: public Cfg, public Observable
{
public:
    TimeCfg(int type);
    virtual ~TimeCfg();

    virtual int        add(const string& json);
    virtual int        del(const string& json);
    virtual int        modify(const string& json);

    virtual int        flush(const string& json);
    virtual int        query(string& json, string condition = "");
    virtual int        clear(const string& json);
};

MS_API extern TimeCfg* timeCfg;
#endif
