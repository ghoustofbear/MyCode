//
// Copyright (C) 2010 Piotr Zagawa
//
// Released under BSD License
//

#pragma once

#include <map>
#include <vector>
#include "SqlField.h"


namespace sql
{

class FieldSet
{
private:
    std::vector<Field> _vec;
    std::map<string, Field*> _map;

private:
    void copy(const std::vector<Field>& definition);

public:
    FieldSet(const Field* definition);
    FieldSet(std::vector<Field>& definition);
    FieldSet(const FieldSet& source);

public:
    string toString(const char *contextTableName = NULL) const;
    int count() const;
    const Field* getByIndex(int index) const;
    const Field* getByName(string name) const;
    Field* getByIndex(int index);
    Field* getByName(string name);

public:
    string definitionHash(bool stripIgnoredFields) const;
    string getDefinition(bool stripIgnoredFields) const;
    bool hasIgnoredFields() const;
    static FieldSet* createFromDefinition(string value);

};


//sql eof
};
