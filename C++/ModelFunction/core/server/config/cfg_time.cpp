#include "cfg_time.h"
#include "db.h"
#include "observer.h"
#include "sys.h"

#if defined(_WIN32) || (__cplusplus >= 201103L)
#include "json/json.h"
#else
#include "json-0.10.6/json.h"
#endif

static
TimeCfg  cfg(Cfg::DB_EASYSQLITE);
TimeCfg* timeCfg = &cfg;

TimeCfg::TimeCfg(int type)
    : Cfg(type)
{

}

TimeCfg::~TimeCfg()
{

}

int
TimeCfg::add(const string& json)
{
    return -1;
}

int
TimeCfg::del(const string& json)
{
    return -1;
}

int
TimeCfg::modify(const string& json)
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

    if (m_db->update("tbl_time", writer.write(rvalue)) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_time", json, Observer::O_MODIFY);

    return 0;
}

int
TimeCfg::flush(const string& json)
{
    return -1;
}

int
TimeCfg::query(string& json, string condition)
{
    return m_db->query("tbl_time", json, condition);
}

int
TimeCfg::clear(const string& json)
{
    return -1;
}
