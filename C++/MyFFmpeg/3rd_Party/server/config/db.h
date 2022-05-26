/**
 * @file      db.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-29 15:40:48
 *   
 * @note
 *  db.h defines 
 */
#ifndef __DB_H__
#define __DB_H__
#include "def.h"
namespace sql{
	class Field;
}
class DB
{
public:
    virtual            ~DB() {};

    virtual int        open(const string &name) = 0;
    virtual void       close()                  = 0;

    virtual int        insert(const string &table, const string &record) = 0;
    virtual int        update(const string &table, const string &record) = 0;
    virtual int        deletex(const string &table, const string &json)  = 0;
    virtual int        query(const string &table, string &json, const string &condition = "") = 0;
    virtual int        count(const string &table, const string &condition = "") = 0;
    virtual int        compress() = 0;
    virtual int        resume(const string &dbname) = 0;
    virtual int        dbUpdate() = 0;
	virtual int        createTable(const string &table, const string &definition) = 0;//create table
	virtual int        deleteTable(const string &table, const string  &definition) = 0;//delete table
};
#endif
