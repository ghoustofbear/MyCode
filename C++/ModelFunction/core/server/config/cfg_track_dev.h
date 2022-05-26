/**
 * @file      cfg_track_dev.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-29 16:07:06
 *   
 * @note
 *  cfg_track_dev.h defines 
 */
#ifndef __CFG_TRACK_DEV_H__
#define __CFG_TRACK_DEV_H__
#include "observable.h"
#include "cfg.h"

class TrackDevCfg: public Cfg, public Observable
{
public:
    TrackDevCfg(int type);
    virtual ~TrackDevCfg();

    virtual int        add(const string& json);
    virtual int        del(const string& json);
    virtual int        modify(const string& json);

    virtual int        flush(const string& json);
    virtual int        query(string& json, string condition = "");
    virtual int        clear(const string& json);
};

MS_API extern TrackDevCfg* trackDevCfg;
#endif
