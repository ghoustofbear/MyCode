#include "cfg_iva_lmt.h"
#include "db.h"
#include "observer.h"
#include "sys.h"
#include "json/json.h"

static
IvaLmtCfg  cfg(Cfg::DB_EASYSQLITE);
IvaLmtCfg* ivalmtCfg = &cfg;

IvaLmtCfg::IvaLmtCfg(int type)
    : Cfg(type)
{

}

IvaLmtCfg::~IvaLmtCfg()
{

}

int
IvaLmtCfg::add(const string& json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) &&
        !rvalue.isNull() &&
        rvalue.isObject()))
    {
        return -1;
    }

    if (m_db->insert("tbl_iva_lmt", json) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_iva_lmt", json, Observer::O_ADD);

    return 0;
}

int
IvaLmtCfg::del(const string& json)
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

    std::string str = rvalue["iva_addr"].asString();
    snprintf(cond, 128, "iva_addr = '%s'", str.c_str(), str.c_str());
    std::string con(cond);

    if (m_db->deletex("tbl_iva_lmt", cond) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_iva_lmt", json, Observer::O_DELETE);

    return 0;
}

int
IvaLmtCfg::modify(const string& json)
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

    if (m_db->update("tbl_iva_lmt", writer.write(rvalue)) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_iva_lmt", json, Observer::O_MODIFY);

    return 0;
}

int
IvaLmtCfg::flush(const string& json)
{
    return -1;
}

int
IvaLmtCfg::query(string& json, string condition)
{
    return m_db->query("tbl_iva_lmt", json, condition);
}

int
IvaLmtCfg::count(string condition)
{
    return m_db->count("tbl_iva_lmt", condition);
}

int
IvaLmtCfg::clear(const string& json)
{
    return -1;
}
