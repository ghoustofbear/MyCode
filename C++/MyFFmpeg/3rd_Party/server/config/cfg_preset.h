/**
 * @file      cfg_system.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-11-17 15:17:38
 *   
 * @note
 *  cfg_preset.h defines 
 */
#ifndef __CFG_PRESET_H__
#define __CFG_PRESET_H__
#include "observable.h"
#include "cfg.h"

class PresetCfg: public Cfg, public Observable
{
public:
    PresetCfg(int type);
    virtual ~PresetCfg();

    virtual int        add(const string& json);
    virtual int        del(const string& json);
    virtual int        modify(const string& json);

    virtual int        flush(const string& json);
    virtual int        query(string& json, string condition = "");
    virtual int        clear(const string& json);
	virtual int        count(string condition = "");
};

MS_API extern PresetCfg* presetCfg;
#endif
