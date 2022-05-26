#include "cfg_group.h"
#include "db.h"
#include "observer.h"
#include "sys.h"
#include "json/json.h"

static
GroupCfg  cfg(Cfg::DB_EASYSQLITE);
GroupCfg* groupCfg = &cfg;

GroupCfg::GroupCfg(int type)
    : Cfg(type)
{

}

GroupCfg::~GroupCfg()
{

}

int
GroupCfg::add(const string& json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) &&
            !rvalue.isNull() &&
            rvalue.isObject()))
    {
        return -1;
    }

    rvalue["id"] = sys->generateUID();

    SHL_Json::FastWriter writer; 
    std::string      wjson;

    wjson = writer.write(rvalue);

    /* group/GROUP is a keyword to the SQL parser in SQLite */
    if (m_db->insert("tbl_group", wjson) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_group", wjson, Observer::O_ADD);

    return 0;
}

int
GroupCfg::del(const string& json)
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

    std::string str = rvalue["id"].asString();
    snprintf(cond, 128, "id = '%s'", str.c_str());
    std::string con(cond);

    if (m_db->deletex("tbl_group", cond) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_group", json, Observer::O_DELETE);

    return 0;
}

int
GroupCfg::modify(const string& json)
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

    if (m_db->update("tbl_group", writer.write(rvalue)) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_group", json, Observer::O_MODIFY);

    return 0;
}

int
GroupCfg::flush(const string& json)
{
    return -1;
}

int
GroupCfg::query(string& json, string condition)
{
    return m_db->query("tbl_group", json, condition);
}

int
GroupCfg::clear(const string& json)
{
    return -1;
}
