#include "cfg_station.h"
#include "db.h"
#include "observer.h"

#include "json/json.h"

static
StationCfg  cfg(Cfg::DB_EASYSQLITE);
StationCfg* stationCfg = &cfg;

StationCfg::StationCfg(int type)
    : Cfg(type)
{

}

StationCfg::~StationCfg()
{

}

int
StationCfg::add(const string& json)
{
    return -1;
}

int
StationCfg::del(const string& json)
{
    return -1;
}

int
StationCfg::modify(const string& json)
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

    if (m_db->update("tbl_station", writer.write(rvalue)) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_station", json, Observer::O_MODIFY);

    return 0;
}

int
StationCfg::flush(const string& json)
{
    return -1;
}

int
StationCfg::query(string& json, string condition)
{
    return m_db->query("tbl_station", json, condition);
}

int
StationCfg::clear(const string& json)
{
    return -1;
}
