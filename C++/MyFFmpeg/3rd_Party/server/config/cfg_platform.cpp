#include "cfg_platform.h"
#include "db.h"
#include "observer.h"

#include "json/json.h"

static
PlatformCfg  cfg(Cfg::DB_EASYSQLITE);
PlatformCfg* platCfg = &cfg;

PlatformCfg::PlatformCfg(int type)
    : Cfg(type)
{

}

PlatformCfg::~PlatformCfg()
{

}

int
PlatformCfg::add(const string& json)
{
    return -1;
}

int
PlatformCfg::del(const string& json)
{
    return -1;
}

int
PlatformCfg::modify(const string& json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) &&
            !rvalue.isNull() &&
            rvalue.isObject()))
    {
        return -1;
    }

    SHL_Json::FastWriter writer; 

    if (m_db->update("tbl_platform", writer.write(rvalue)) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_platform", json, Observer::O_MODIFY);

    return 0;
}

int
PlatformCfg::flush(const string& json)
{
    return -1;
}

int
PlatformCfg::query(string& json, string condition)
{
    return m_db->query("tbl_platform", json, condition);
}

int
PlatformCfg::clear(const string& json)
{
    return -1;
}
