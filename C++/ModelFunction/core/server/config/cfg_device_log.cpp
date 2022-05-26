#include "cfg_device_log.h"
#include "db.h"
#include "observer.h"
#include "sys.h"
#include "json/json.h"

static
DeviceLogCfg  cfg(Cfg::DB_EASYSQLITE);
DeviceLogCfg* deviceLogCfg = &cfg;

DeviceLogCfg::DeviceLogCfg(int type)
    : Cfg(type)
{

}

DeviceLogCfg::~DeviceLogCfg()
{

}

int
DeviceLogCfg::add(const string& json)
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

    if (m_db->insert("tbl_device_log", wjson) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_device_log", wjson, Observer::O_ADD);

    return 0;
}

int
DeviceLogCfg::del(const string& json)
{
    return -1;
}

int
DeviceLogCfg::modify(const string& json)
{
    return -1;
}

int
DeviceLogCfg::flush(const string& json)
{
    return -1;
}

int
DeviceLogCfg::query(string& json, string condition)
{
    return m_db->query("tbl_device_log", json, condition);
}

int
DeviceLogCfg::count(string condition)
{
    return m_db->count("tbl_device_log", condition);
}

int
DeviceLogCfg::clear(const string& json)
{
    if (m_db->deletex("tbl_device_log", json) < 0)
    {
        return -1;
    }
    return 0;
}
