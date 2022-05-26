/**
* @file      cfg_camera_ivaability.h
* @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
* @brief     brief
* @author    caiwang213@qq.com
* @date      2017-11-16 12:39:30
*
* @note
*  cfg_device.h defines
*  query     return the count(*) of current table
*/
#ifndef __CFG_CAMERA_IVAABILITY_H__
#define __CFG_CAMERA_IVAABILITY_H__
#include "observable.h"
#include "cfg.h"

class CameraIvaAbilityCfg : public Cfg, public Observable
{
public:
    CameraIvaAbilityCfg(int type);
    virtual ~CameraIvaAbilityCfg();

    virtual int        add(const string& json);
    virtual int        del(const string& json);
    virtual int        modify(const string& json);

    virtual int        flush(const string& json);
    virtual int        query(string& json, string condition = "");
    virtual int        count(string condition = "");
    virtual int        clear(const string& json);
};

MS_API extern CameraIvaAbilityCfg* cameraIvaAbilityCfg;
#endif