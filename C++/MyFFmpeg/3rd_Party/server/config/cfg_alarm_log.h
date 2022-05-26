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
#ifndef __CFG_ALARM_LOG_H__
#define __CFG_ALARM_LOG_H__
#include "observable.h"
#include "cfg.h"

class AlarmLogCfg: public Cfg, public Observable
{
public:
    AlarmLogCfg(int type);
    virtual ~AlarmLogCfg();

    virtual int        add(const string& json);
    virtual int        del(const string& json);
    virtual int        modify(const string& json);

    virtual int        flush(const string& json);
    virtual int        query(string& json, string condition = "");
    virtual int        count(string condition = "");
    virtual int        clear(const string& json);
};

MS_API extern AlarmLogCfg* alarmLogCfg;
#endif
