#include "cfg_track_dev.h"
#include "db.h"
#include "observer.h"
#include "sys.h"


#if defined(_WIN32) || (__cplusplus >= 201103L)
#include "json/json.h"
#else
#include "json-0.10.6/json.h"
#endif

static
TrackDevCfg  cfg(Cfg::DB_EASYSQLITE);
TrackDevCfg* trackDevCfg = &cfg;

TrackDevCfg::TrackDevCfg(int type)
    : Cfg(type)
{

}

TrackDevCfg::~TrackDevCfg()
{

}

int
TrackDevCfg::add(const string& json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) &&
            !rvalue.isNull() &&
            rvalue.isObject()))
    {
        return -1;
    }


    if (m_db->insert("tbl_track_dev", json) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_track_dev", json, Observer::O_ADD);

    return 0;
}

int
TrackDevCfg::del(const string& json)
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

    std::string str = rvalue["tarckCode"].asString();
    snprintf(cond, 128, "tarckCode = '%s'",str.c_str());
    std::string con(cond);
    
    if (m_db->deletex("tbl_track_dev", cond) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_track_dev", json, Observer::O_DELETE);

    return 0;
}

int
TrackDevCfg::modify(const string& json)
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
    std::string      wjson;

    wjson = writer.write(rvalue);
    if (m_db->update("tbl_track_dev", wjson) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_track_dev", wjson, Observer::O_MODIFY);

    return 0;
}

int
TrackDevCfg::flush(const string& json)
{
    return -1;
}

int
TrackDevCfg::query(string& json, string condition)
{
    return m_db->query("tbl_track_dev", json, condition);
}

int
TrackDevCfg::clear(const string& json)
{
    return -1;
}
