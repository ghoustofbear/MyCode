/**
 * @file      cfg_user_group.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-31 16:44:59
 *   
 * @note
 *  cfg_user_group.h defines 
 */
#ifndef __CFG_USER_GROUP_H__
#define __CFG_USER_GROUP_H__
#include "observable.h"
#include "cfg.h"

class UserGroupCfg: public Cfg, public Observable
{
public:
    UserGroupCfg(int type);
    virtual ~UserGroupCfg();

    virtual int        add(const string& json);
    virtual int        del(const string& cond);
    virtual int        modify(const string& json);

    virtual int        flush(const string& json);
    virtual int        query(string& json, string condition = "");
    virtual int        clear(const string& json);
};

MS_API extern UserGroupCfg* usrgrCfg;
#endif
