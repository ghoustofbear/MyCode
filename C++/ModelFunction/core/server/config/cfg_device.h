/**
 * @file      cfg_device.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-11-16 12:39:30
 *   
 * @note
 *  cfg_device.h defines 
 *  query     return the count(*) of current table
 */
#ifndef __CFG_DEVICE_H__
#define __CFG_DEVICE_H__
#include "observable.h"
#include "cfg.h"

class DeviceCfg: public Cfg, public Observable
{
public:
    DeviceCfg(int type);
    virtual ~DeviceCfg();

    virtual int        add(const string& json);
    virtual int        del(const string& json);
    virtual int        modify(const string& json);

    virtual int        flush(const string& json);
    virtual int        query(string& json, string condition = "");
    virtual int        count(string condition = "");
    virtual int        clear(const string& json);
};

MS_API extern DeviceCfg* deviceCfg;
#endif
