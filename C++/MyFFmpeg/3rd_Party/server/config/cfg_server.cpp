#include "cfg_server.h"
#include "db.h"
#include "observer.h"
#include "sys.h"
#include "json/json.h"

static
ServerCfg  cfg(Cfg::DB_EASYSQLITE);
ServerCfg* servCfg = &cfg;

ServerCfg::ServerCfg(int type)
    : Cfg(type)
{

}

ServerCfg::~ServerCfg()
{

}

int
ServerCfg::add(const string& json)
{
    /* server/server is a keyword to the SQL parser in SQLite */
    if (m_db->insert("tbl_server", json) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_server", json, Observer::O_ADD);

    return 0;
}

int
ServerCfg::del(const string& json)
{
    return -1;
}

int
ServerCfg::modify(const string& json)
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

    if (m_db->update("tbl_server", writer.write(rvalue)) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_server", json, Observer::O_MODIFY);

    return 0;
}

int
ServerCfg::flush(const string& json)
{
    return -1;
}

int
ServerCfg::query(string& json, string condition)
{
    return m_db->query("tbl_server", json, condition);
}

int
ServerCfg::clear(const string& json)
{
    return -1;
}
