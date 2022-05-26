#include "cfg_alarm_log.h"
#include "db.h"
#include "observer.h"
#include "sys.h"
#include "json/json.h"

static
AlarmLogCfg  cfg(Cfg::DB_EASYSQLITE);
AlarmLogCfg* alarmLogCfg = &cfg;

AlarmLogCfg::AlarmLogCfg(int type)
    : Cfg(type)
{

}

AlarmLogCfg::~AlarmLogCfg()
{

}

int
AlarmLogCfg::add(const string& json)
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

    if (m_db->insert("tbl_alarm_log", wjson) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_alarm_log", wjson, Observer::O_ADD);

    return 0;
}

int
AlarmLogCfg::del(const string& json)
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

    //if (m_db->deletex("tbl_alarm_log", cond) < 0)
    //{
    //    return -1;
    //}

    //setChanged();
    //notifyObservers("tbl_alarm_log", json, Observer::O_DELETE);

    return -1;
}

int
AlarmLogCfg::modify(const string& json)
{
    return -1;
}

int
AlarmLogCfg::flush(const string& json)
{
    return -1;
}

int
AlarmLogCfg::query(string& json, string condition)
{
    return m_db->query("tbl_alarm_log", json, condition);
}

int
AlarmLogCfg::count(string condition)
{
    return m_db->count("tbl_alarm_log", condition);
}

int
AlarmLogCfg::clear(const string& json)
{
    if (m_db->deletex("tbl_alarm_log", json) < 0)
    {
        return -1;
    }
    return 0;
}
