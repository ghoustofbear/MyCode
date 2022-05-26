//
// Copyright (C) 2010 Piotr Zagawa
//
// Released under BSD License
//

#pragma once

#include "sqlite3.h"
#include "SqlCommon.h"


namespace sql
{

class Database
{
public:
    class Trace
    {
    public:
        enum Severity
        {
            Informational,
            Warning,
            Error,
            Fatal,
        };

        virtual void notifyDatabaseTrace(
            Severity severity,
            const char *message
        ) const = 0;
    };

private:
    sqlite3* _handle;
    string _err_msg;
    int _result_open;
    Trace *_tracer;

public:
    Database(Trace *tracer = NULL);
    ~Database(void);

public:
    sqlite3* getHandle() const;
    Trace *getTracer() const;
    string errMsg();
    bool open(string filename);
    void close();
    bool isOpen();

public:
    bool transactionBegin();
    bool transactionCommit();
    bool transactionRollback();

private:
    static void trace(void *context, const char *message);
};


//sql eof
};
