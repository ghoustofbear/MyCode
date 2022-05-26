#include "cfg_system_log.h"
#include "db.h"
#include "observer.h"
#include "sys.h"
#include "json/json.h"

static
SystemLogCfg  cfg(Cfg::DB_EASYSQLITE);
SystemLogCfg* systemLogCfg = &cfg;

SystemLogCfg::SystemLogCfg(int type)
    : Cfg(type)
{

}

SystemLogCfg::~SystemLogCfg()
{

}

int
SystemLogCfg::add(const string& json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) &&
            !rvalue.isNull() &&
            rvalue.isObject()))
    {
        return -1;
    }

    //rvalue["id"] = sys->generateUID();

    SHL_Json::FastWriter writer; 
    std::string      wjson;

    wjson = writer.write(rvalue);

    if (m_db->insert("tbl_system_log", wjson) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_system_log", wjson, Observer::O_ADD);

    return 0;
}

int
SystemLogCfg::del(const string& json)
{
    //SHL_Json::Reader reader;
    //SHL_Json::Value  rvalue;

    //if (!(reader.parse(json, rvalue, false) &&
    //    !rvalue.isNull() &&
    //    rvalue.isObject()))
    //{
    //    return -1;
    //}

    //char cond[128] = { 0 };

    //std::string str = rvalue["id"].asString();
    //snprintf(cond, 128, "id = '%s'", str.c_str());
    //std::string con(cond);

    //if (m_db->deletex("tbl_system_log", cond) < 0)
    //{
    //    return -1;
    //}

    //setChanged();
    //notifyObservers("tbl_system_log", json, Observer::O_DELETE);

    return 0;
}

int
SystemLogCfg::modify(const string& json)
{
    return -1;
}

int
SystemLogCfg::flush(const string& json)
{
    return -1;
}

int
SystemLogCfg::query(string& json, string condition)
{
    return m_db->query("tbl_system_log", json, condition);
}

int
SystemLogCfg::count(string condition)
{
    return m_db->count("tbl_system_log", condition);
}

int
SystemLogCfg::clear(const string& json)
{
    if (m_db->deletex("tbl_system_log", json) < 0)
    {
        return -1;
    }
    return 0;
}
