//
// Copyright (C) 2010 Piotr Zagawa
//
// Released under BSD License
//

#pragma once

#include "SqlCommon.h"


namespace sql
{

class Field
{
public:
    friend class FieldSet;

    static const char *id;

private:
    string _name;
    field_use _use;
    field_type _type;
    int _index;
    int _flags;
    string _references;

public:
    Field(field_use use);
    Field(string name, field_type type, int flags = flag_none);
    Field(const Field& value);

public:
    void setReferences(string refer);

    bool isKeyIdField() const;
    bool isEndingField() const;

public:
    int getIndex() const;
    string getName() const;
    string getTypeStr() const;
    field_type getType() const;
    bool isPrimaryKey() const;
    bool isAutoincrement() const;
    bool isNotNull() const;
    bool isUnique() const;
    bool isIgnored() const;

public:
    string getDefinition() const;
    static Field* createFromDefinition(string value);

public:
    void setIgnored(bool ignored = true);

};


//sql eof
};
