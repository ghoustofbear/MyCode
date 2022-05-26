//
// Copyright (C) 2010 Piotr Zagawa
//
// Released under BSD License
//

#pragma once

#include <vector>
#include "SqlCommon.h"
#include "SqlValue.h"
#include "SqlFieldSet.h"


namespace sql
{

class Record
{
private:
    const FieldSet* _fields;
    std::vector<Value> _values;

public:
    Record(const FieldSet* fields);
    Record(Record* record);
    Record(const Record& record);

private:
    friend class RecordSet;

    void initColumnCount(int columns);
    void initColumnValue(int column_index, char* value, field_type type);

    void toSqlInsert(string *outFieldNames, string *outValues);

public:
    int columnCount();
    const Value* getValue(int column_index) const;
    const Value* getValue(string fieldName) const;
    Value* getValue(int column_index);
    Value* getValue(string fieldName);
    Value* getKeyIdValue();
    const Field* fieldByName(string fieldName) const;
    const FieldSet* fields() const;
    bool equalsColumnValue(Record* record, string fieldName);
    bool equalsValues(Record* record);

public:
    string toSqlInsert(string tableName);
    string toSqlUpdate(string tableName);
    string toSqlReplace(string tableName);

    string toSql();
    string toString();

public:
    void setIgnored(int index);
    void setNull(int index);
    void setString(int index, string value);
    void setInteger(int index, integer value);
    void setDouble(int index, double value);
    void setBool(int index, bool value);
    void setTime(int index, time value);

public:
    void setIgnored(string fieldName);
    void setNull(string fieldName);
    void setString(string fieldName, string value);
    void setInteger(string fieldName, integer value);
    void setDouble(string fieldName, double value);
    void setBool(string fieldName, bool value);
    void setTime(string fieldName, time value);

};


//sql eof
};
