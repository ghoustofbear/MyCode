#include "cfg_dbmgr.h"
#include "db.h"
#include "observer.h"
#include "sys.h"


#if defined(_WIN32) || (__cplusplus >= 201103L)
#include "json/json.h"
#else
#include "json-0.10.6/json.h"
#endif

static
DBMgrCfg  cfg(Cfg::DB_EASYSQLITE);
DBMgrCfg* dbMgrCfg = &cfg;

DBMgrCfg::DBMgrCfg(int type)
    : Cfg(type)
{

}

DBMgrCfg::~DBMgrCfg()
{

}

int
DBMgrCfg::add(const string& json)
{

    return 0;
}

int
DBMgrCfg::del(const string& json)
{
    return 0;
}

int
DBMgrCfg::modify(const string& json)
{

    return 0;
}

int
DBMgrCfg::flush(const string& json)
{
    return -1;
}

int
DBMgrCfg::query(string& json, string condition)
{
    int ret = -1;
    if (json == "resume")
    {
        ret = m_db->resume(condition);
    }
    else if (json == "compress")
    {
        ret = m_db->compress();
    }
    else if (json == "upgrade")
    {
        ret = m_db->dbUpdate();
    }

    return ret;
}

int
DBMgrCfg::clear(const string& json)
{
    return -1;
}
