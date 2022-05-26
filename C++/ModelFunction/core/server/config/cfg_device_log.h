/**
 * @file      cfg_group.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-31 08:37:09
 *
 * @note
 *  cfg_group.h defines
 */
#ifndef __CFG_DEVICE_LOG_H__
#define __CFG_DEVICE_LOG_H__
#include "observable.h"
#include "cfg.h"

class DeviceLogCfg: public Cfg, public Observable
{
public:
    DeviceLogCfg(int type);
    virtual ~DeviceLogCfg();

    virtual int        add(const string& json);
    virtual int        del(const string& json);
    virtual int        modify(const string& json);

    virtual int        flush(const string& json);
    virtual int        query(string& json, string condition = "");
    virtual int        count(string condition = "");
    virtual int        clear(const string& json);
};

MS_API extern DeviceLogCfg* deviceLogCfg;
#endif
