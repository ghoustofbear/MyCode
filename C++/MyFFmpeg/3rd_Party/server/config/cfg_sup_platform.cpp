#include "cfg_sup_platform.h"
#include "db.h"
#include "observer.h"
#include "sys.h"
#include "json/json.h"

static
SuperCfg  cfg(Cfg::DB_EASYSQLITE);
SuperCfg* superCfg = &cfg;

SuperCfg::SuperCfg(int type)
    : Cfg(type)
{

}

SuperCfg::~SuperCfg()
{

}

int
SuperCfg::add(const string& json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) &&
            !rvalue.isNull() &&
            rvalue.isObject()))
    {
        return -1;
    }

    if (m_db->insert("tbl_sup_platform", json) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_sup_platform", json, Observer::O_ADD);

    return 0;
}

int
SuperCfg::del(const string& json)
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
    snprintf(cond, 128, "addrcode = '%s'", str.c_str());
    std::string con(cond);

    if (m_db->deletex("tbl_sup_platform", cond) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_sup_platform", json, Observer::O_DELETE);

    return 0;
}

int
SuperCfg::modify(const string& json)
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

    if (m_db->update("tbl_sup_platform", writer.write(rvalue)) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_sup_platform", json, Observer::O_MODIFY);

    return 0;
}

int
SuperCfg::flush(const string& json)
{
    return -1;
}

int
SuperCfg::query(string& json, string condition)
{
    return m_db->query("tbl_sup_platform", json, condition);
}


int
SuperCfg::clear(const string& json)
{
    return -1;
}
