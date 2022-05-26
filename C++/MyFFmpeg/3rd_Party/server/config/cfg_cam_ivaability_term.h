/**
 * @file      cfg_cam_ivaability_term.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-31 08:37:09
 *
 * @note
 *  cfg_group.h defines
 */
#ifndef __CFG_CAM_IVAABILITY_TERM_H__
#define __CFG_CAM_IVAABILITY_TERM_H__
#include "observable.h"
#include "cfg.h"

class CamIvaAbilityTermCfg: public Cfg, public Observable
{
public:
    CamIvaAbilityTermCfg(int type);
    virtual ~CamIvaAbilityTermCfg();

    virtual int        add(const string& json);
    virtual int        del(const string& json);
    virtual int        modify(const string& json);

    virtual int        flush(const string& json);
    virtual int        query(string& json, string condition = "");
    virtual int        count(string condition = "");
    virtual int        clear(const string& json);
};

MS_API extern CamIvaAbilityTermCfg* camIvaAbilityCfg;
#endif
