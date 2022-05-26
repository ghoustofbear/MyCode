/**
 * (C) Copyright 2016, xxx Co., Ltd.
 * Zhong caiwang <caiwang213@qq.com>
 * Wed Oct 12 08:44:52 CST 2016
 *
 * Cfg classes define.
 */
#ifndef __CFG_H__
#define __CFG_H__

#include "exports.h"

#define  CONFIG_FILE         "c2msv.ini"
#define  CONFIG_DB           "c2msv.db"
#define  DB_VERSION          "20210427"

class DB;
class Cfg
{
public:
    enum
    {
        DB_EASYSQLITE,
        DB_OTL,
    };

protected:
    Cfg(int type = DB_EASYSQLITE);
    virtual ~Cfg();

    virtual int        add(const string& json)    = 0;
    virtual int        del(const string& json)    = 0;
    virtual int        modify(const string& json) = 0;

    virtual int        flush(const string& json) = 0;
    virtual int        query(string& josn, string condition = "") = 0;
    virtual int        count(string condition = "");
    
protected:
    DB               * m_db;
};

#endif
