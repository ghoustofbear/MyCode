/**
 * @file      otldb.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-29 15:47:19
 *   
 * @note
 *  connect database using otl
 */
#ifndef __OTLDB_H__
#define __OTLDB_H__
#include "db.h"

class OtlDB: public DB
{
protected:
    OtlDB();
    virtual ~OtlDB();

private:
};
#endif
