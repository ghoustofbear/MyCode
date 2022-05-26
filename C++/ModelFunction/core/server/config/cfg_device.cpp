#include "cfg_device.h"
#include "db.h"
#include "observer.h"
#include "sys.h"
#include "json/json.h"

static
DeviceCfg  cfg(Cfg::DB_EASYSQLITE);
DeviceCfg* deviceCfg = &cfg;

DeviceCfg::DeviceCfg(int type)
    : Cfg(type)
{

}

DeviceCfg::~DeviceCfg()
{

}

int
DeviceCfg::add(const string& json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) &&
            !rvalue.isNull() &&
            rvalue.isObject()))
    {
        return -1;
    }

    if (m_db->insert("tbl_device", json) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_device", json, Observer::O_ADD);

    return 0;
}

int
DeviceCfg::del(const string& json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) &&
        !rvalue.isNull() &&
        rvalue.isObject()))
    {
        return -1;
    }

    char cond[128] = { 0 };

    std::string str = rvalue["addrcode"].asString();
    snprintf(cond, 128, "addrcode = '%s' or parentCode = '%s'", str.c_str(), str.c_str());
    std::string con(cond);

    if (m_db->deletex("tbl_device", cond) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_device", json, Observer::O_DELETE);

    return 0;
}

int
DeviceCfg::modify(const string& json)
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

    if (m_db->update("tbl_device", writer.write(rvalue)) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_device", json, Observer::O_MODIFY);

    return 0;
}

int
DeviceCfg::flush(const string& json)
{
    return -1;
}

int
DeviceCfg::query(string& json, string condition)
{
    return m_db->query("tbl_device", json, condition);
}

int
DeviceCfg::count(string condition)
{
    return m_db->count("tbl_device", condition);
}

int
DeviceCfg::clear(const string& json)
{
    return -1;
}
