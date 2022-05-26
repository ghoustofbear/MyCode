/**
 * @file      cfg_user.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-29 16:07:06
 *   
 * @note
 *  cfg_user.h defines 
 */
#ifndef __CFG_DBMGR_H__
#define __CFG_DBMGR_H__
#include "cfg.h"

class DBMgrCfg: public Cfg
{
public:
    DBMgrCfg(int type);
    virtual ~DBMgrCfg();

    virtual int        add(const string& json);
    virtual int        del(const string& json);
    virtual int        modify(const string& json);

    virtual int        flush(const string& json);
    virtual int        query(string& json, string condition = "");
    virtual int        clear(const string& json);
};

MS_API extern DBMgrCfg* dbMgrCfg;
#endif
