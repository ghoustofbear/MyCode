/**
 * @file      sqlitedb.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-29 15:43:48
 *
 * @note
 *  sqlitedb.h defines
 */
#ifndef __SQLITEDB_H__
#define __SQLITEDB_H__
#include "db.h"
#include "exports.h"
namespace sql
{
class Database;
}

class SqliteDB: public DB
{
public:
    SqliteDB();
    virtual ~SqliteDB();

protected:
    virtual int        open(const string &db);
    virtual void       close();

    virtual int        insert(const string &table, const string &record);
    virtual int        update(const string &table, const string &record);
    virtual int        deletex(const string &table, const string &json);
    virtual int        query(const string &table, string &json, const string &condition = ""); 
    virtual int        count(const string &table, const string &condition = "");
    virtual int        compress();
    virtual int        resume(const string& dbname);
    virtual int        dbUpdate();
	virtual int        createTable(const string &table,const string &definition);//create table
	virtual int        deleteTable(const string &table, const string  &definition);//delete table

private:
    sql::Database    * m_db;
};

MS_API extern SqliteDB* sqliteDb;

#endif
