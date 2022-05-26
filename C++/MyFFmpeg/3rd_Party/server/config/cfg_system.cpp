#include "cfg_system.h"
#include "db.h"
#include "observer.h"
#include "sys.h"
#include "json/json.h"

static
SystemCfg  cfg(Cfg::DB_EASYSQLITE);
SystemCfg* sysCfg = &cfg;

SystemCfg::SystemCfg(int type)
    : Cfg(type)
{

}

SystemCfg::~SystemCfg()
{

}

int
SystemCfg::add(const string& json)
{
    return -1;
}

int
SystemCfg::del(const string& json)
{
    return -1;
}

int
SystemCfg::modify(const string& json)
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

    if (m_db->update("tbl_system", writer.write(rvalue)) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_system", json, Observer::O_MODIFY);

    return 0;
}

int
SystemCfg::flush(const string& json)
{
    return -1;
}

int
SystemCfg::query(string& json, string condition)
{
    return m_db->query("tbl_system", json, condition);
}

int
SystemCfg::clear(const string& json)
{
    return -1;
}
