#include "cfg_ntp.h"
#include "db.h"
#include "observer.h"
#include "sys.h"

#if defined(_WIN32) || (__cplusplus >= 201103L)
#include "json/json.h"
#else
#include "json-0.10.6/json.h"
#endif

static
NtpCfg  cfg(Cfg::DB_EASYSQLITE);
NtpCfg* ntpCfg = &cfg;

NtpCfg::NtpCfg(int type)
    : Cfg(type)
{

}

NtpCfg::~NtpCfg()
{

}

int
NtpCfg::add(const string& json)
{
    return -1;
}

int
NtpCfg::del(const string& json)
{
    return -1;
}

int
NtpCfg::modify(const string& json)
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

    if (m_db->update("tbl_ntp", writer.write(rvalue)) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_ntp", json, Observer::O_MODIFY);

    return 0;
}

int
NtpCfg::flush(const string& json)
{
    return -1;
}

int
NtpCfg::query(string& json, string condition)
{
    return m_db->query("tbl_ntp", json, condition);
}

int
NtpCfg::clear(const string& json)
{
    return -1;
}
