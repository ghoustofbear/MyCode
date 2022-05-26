#include "cfg_iva_taskresources.h"
#include "db.h"
#include "observer.h"
#include "sys.h"
#include "json/json.h"

static
IVATaskResourcesCfg  cfg(Cfg::DB_EASYSQLITE);
IVATaskResourcesCfg* ivaTaskResourcesCfg = &cfg;

IVATaskResourcesCfg::IVATaskResourcesCfg(int type)
    : Cfg(type)
{

}

IVATaskResourcesCfg::~IVATaskResourcesCfg()
{

}

int
IVATaskResourcesCfg::add(const string& json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) &&
        !rvalue.isNull() &&
        rvalue.isObject()))
    {
        return -1;
    }

    if (m_db->insert("tbl_taskresources", json) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_taskresources", json, Observer::O_ADD);

    return 0;
}

int
IVATaskResourcesCfg::del(const string& json)
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

    std::string str = rvalue["taskid"].asString();
    snprintf(cond, 128, "taskid = '%s' ", str.c_str());
    std::string con(cond);

    if (m_db->deletex("tbl_taskresources", cond) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_taskresources", json, Observer::O_DELETE);

    return 0;
}

int
IVATaskResourcesCfg::modify(const string& json)
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

    if (m_db->update("tbl_taskresources", writer.write(rvalue)) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_taskresources", json, Observer::O_MODIFY);

    return 0;
}

int
IVATaskResourcesCfg::flush(const string& json)
{
    return -1;
}

int
IVATaskResourcesCfg::query(string& json, string condition)
{
    return m_db->query("tbl_taskresources", json, condition);
}

int
IVATaskResourcesCfg::count(string condition)
{
    return m_db->count("tbl_taskresources", condition);
}

int
IVATaskResourcesCfg::clear(const string& json)
{
    return -1;
}
