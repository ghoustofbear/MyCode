#include "cfg_user_log.h"
#include "db.h"
#include "observer.h"
#include "sys.h"
#include "json/json.h"

static
UserLogCfg  cfg(Cfg::DB_EASYSQLITE);
UserLogCfg* userLogCfg = &cfg;

UserLogCfg::UserLogCfg(int type)
    : Cfg(type)
{

}

UserLogCfg::~UserLogCfg()
{

}

int
UserLogCfg::add(const string& json)
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

    /* group/GROUP is a keyword to the SQL parser in SQLite */
    if (m_db->insert("tbl_user_log", wjson) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_user_log", wjson, Observer::O_ADD);

    return 0;
}

int
UserLogCfg::del(const string& json)
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

    //if (m_db->deletex("tbl_user_log", cond) < 0)
    //{
    //    return -1;
    //}

    //setChanged();
    //notifyObservers("tbl_user_log", json, Observer::O_DELETE);

    return 0;
}

int
UserLogCfg::modify(const string& json)
{
    return -1;
}

int
UserLogCfg::flush(const string& json)
{
    return -1;
}

int
UserLogCfg::query(string& json, string condition)
{
    return m_db->query("tbl_user_log", json, condition);
}

int
UserLogCfg::count(string condition)
{
    return m_db->count("tbl_user_log", condition);
}

int
UserLogCfg::clear(const string& json)
{
    if (m_db->deletex("tbl_user_log", json) < 0)
    {
        return -1;
    }
    return 0;
}
