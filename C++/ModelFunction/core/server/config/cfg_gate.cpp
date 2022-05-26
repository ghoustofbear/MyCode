#include "cfg_gate.h"
#include "db.h"
#include "observer.h"
#include "sys.h"
#include "json/json.h"

static
GateCfg  cfg(Cfg::DB_EASYSQLITE);
GateCfg* gateCfg = &cfg;

GateCfg::GateCfg(int type)
    : Cfg(type)
{

}

GateCfg::~GateCfg()
{

}

int
GateCfg::add(const string& json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) &&
        !rvalue.isNull() &&
        rvalue.isObject()))
    {
        return -1;
    }

    if (m_db->insert("tbl_gate", json) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_gate", json, Observer::O_ADD);

    return 0;
}

int
GateCfg::del(const string& json)
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

    std::string str = rvalue["camaddr"].asString();
    snprintf(cond, 128, "camaddr = '%s'", str.c_str(), str.c_str());
    std::string con(cond);

    if (m_db->deletex("tbl_gate", cond) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_gate", json, Observer::O_DELETE);

    return 0;
}

int
GateCfg::modify(const string& json)
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

    if (m_db->update("tbl_gate", writer.write(rvalue)) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_gate", json, Observer::O_MODIFY);

    return 0;
}

int
GateCfg::flush(const string& json)
{
    return -1;
}

int
GateCfg::query(string& json, string condition)
{
    return m_db->query("tbl_gate", json, condition);
}

int
GateCfg::count(string condition)
{
    return m_db->count("tbl_gate", condition);
}

int
GateCfg::clear(const string& json)
{
    return -1;
}
