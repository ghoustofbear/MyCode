#include "cfg.h"
#include "sqlitedb.h"
#include "otldb.h"


Cfg::Cfg(int type)
{
    if (type == DB_EASYSQLITE)
        m_db = sqliteDb;
    else
        m_db = NULL;
}

Cfg::~Cfg()
{
    m_db = NULL;
}

int
Cfg::count(string condition)
{
    return 0;
}



