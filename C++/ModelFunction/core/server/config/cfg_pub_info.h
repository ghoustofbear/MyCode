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
#ifndef __CFG_PUB_INFO_H__
#define __CFG_PUB_INFO_H__
#include "cfg.h"

class PubInfoCfg : public Cfg
{
public:
    PubInfoCfg(int type);
    virtual ~PubInfoCfg();

    virtual int        query(string& json, string condition = "");
    //为了方便将此接口改为数据库压缩接口
    virtual int        add(const string& json);
    
private:
    
    virtual int        del(const string& json);
    virtual int        modify(const string& json);

    virtual int        flush(const string& json);
    virtual int        clear(const string& json);
};

MS_API extern PubInfoCfg* pubInfoCfg;
#endif
