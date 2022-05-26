#include "cfg_preset.h"
#include "db.h"
#include "observer.h"
#include "sys.h"


#if defined(_WIN32) || (__cplusplus >= 201103L)
#include "json/json.h"
#else
#include "json-0.10.6/json.h"
#endif

static
PresetCfg  cfg(Cfg::DB_EASYSQLITE);
PresetCfg* presetCfg = &cfg;

PresetCfg::PresetCfg(int type)
    : Cfg(type)
{

}

PresetCfg::~PresetCfg()
{

}

int
PresetCfg::add(const string& json)
{
    if (m_db->insert("tbl_preset", json) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_preset", json, Observer::O_ADD);

    return 0;
}

int
PresetCfg::del(const string& json)
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

    std::string str = rvalue["addrcode"].asString();
    int idx = rvalue["idx"].asInt();
    snprintf(cond, 128, "addrcode = '%s' and idx = %d", str.c_str(), idx);
    std::string con(cond);

    if (m_db->deletex("tbl_preset", cond) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_preset", json, Observer::O_DELETE);

    return 0;
}

int
PresetCfg::modify(const string& json)
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
    std::string str = rvalue["addrcode"].asString();
    int idx = rvalue["idx"].asInt();
    snprintf(cond, 128, "addrcode = '%s' and idx = %d", str.c_str(), idx);
    std::string con(cond);

    {
        if (m_db->deletex("tbl_preset", cond) >= 0)
        {
            setChanged();
            notifyObservers("tbl_preset", json, Observer::O_DELETE);
        }
    }

    return add(json);
}

int
PresetCfg::flush(const string& json)
{
    return -1;
}

int
PresetCfg::query(string& json, string condition)
{
    return m_db->query("tbl_preset", json, condition);
}

int
PresetCfg::clear(const string& json)
{
    return -1;
}

int PresetCfg::count(string condition /*= ""*/)
{
	return m_db->count("tbl_preset", condition);
}
