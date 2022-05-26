/**
 * @file      cfg_sub_platform.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-11-16 12:39:39
 *   
 * @note
 *  cfg_sub_platform.h defines 
 */
#ifndef __CFG_SUB_PLATFORM_H__
#define __CFG_SUB_PLATFORM_H__
#include "observable.h"
#include "cfg.h"

class SuberCfg: public Cfg, public Observable
{
public:
    SuberCfg(int type);
    virtual ~SuberCfg();

    virtual int        add(const string& json);
    virtual int        del(const string& json);
    virtual int        modify(const string& json);

    virtual int        flush(const string& json);
    virtual int        query(string& json, string condition = "");
    virtual int        count(string condition = "");
    virtual int        clear(const string& json);
};

MS_API extern SuberCfg* suberCfg;



//下级平台监控信息
class SuberCamInfoCfg :public Cfg, public Observable
{
public:
	SuberCamInfoCfg(int type);
	virtual ~SuberCamInfoCfg();
	virtual int        add(const string& json);
	virtual int        del(const string& json);
	virtual int        modify(const string& json);

	virtual int        flush(const string& json);
	virtual int        query(string& json, string condition = "");
	virtual int        createTable(const string &table);
	virtual int        deleteTable(const string &table);
};
MS_API extern SuberCamInfoCfg* suberCamInfoCfg;
#endif
