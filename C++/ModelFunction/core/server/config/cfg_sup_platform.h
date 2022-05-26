/**
 * @file      cfg_sup_platform.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-11-16 12:40:01
 *   
 * @note
 *  cfg_sup_platform.h defines 
 */
#ifndef __CFG_SUP_PLATFORM_H__
#define __CFG_SUP_PLATFORM_H__
#include "observable.h"
#include "cfg.h"

class SuperCfg: public Cfg, public Observable
{
public:
    SuperCfg(int type);
    virtual ~SuperCfg();

    virtual int        add(const string& json);
    virtual int        del(const string& json);
    virtual int        modify(const string& json);

    virtual int        flush(const string& json);
    virtual int        query(string& json, string condition = "");
    virtual int        clear(const string& json);
};

MS_API extern SuperCfg* superCfg;
#endif
