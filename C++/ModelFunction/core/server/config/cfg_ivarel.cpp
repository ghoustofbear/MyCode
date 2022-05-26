#include "cfg_ivarel.h"
#include "db.h"
#include "observer.h"
#include "sys.h"
#include "json/json.h"

static
IVArelCfg  cfg(Cfg::DB_EASYSQLITE);
IVArelCfg* ivarelCfg = &cfg;

IVArelCfg::IVArelCfg(int type)
    : Cfg(type)
{

}

IVArelCfg::~IVArelCfg()
{

}

int
IVArelCfg::add(const string& json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) &&
        !rvalue.isNull() &&
        rvalue.isObject()))
    {
        return -1;
    }

    if (m_db->insert("tbl_camerareliva", json) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_camerareliva", json, Observer::O_ADD);

    return 0;
}

int
IVArelCfg::del(const string& json)
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

    std::string str = rvalue["camera_addrcode"].asString();
    snprintf(cond, 128, "camera_addrcode = '%s'", str.c_str(), str.c_str());
    std::string con(cond);

    if (m_db->deletex("tbl_camerareliva", cond) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_camerareliva", json, Observer::O_DELETE);

    return 0;
}

int
IVArelCfg::modify(const string& json)
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

    if (m_db->update("tbl_camerareliva", writer.write(rvalue)) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_camerareliva", json, Observer::O_MODIFY);

    return 0;
}

int
IVArelCfg::flush(const string& json)
{
    return -1;
}

int
IVArelCfg::query(string& json, string condition)
{
    return m_db->query("tbl_camerareliva", json, condition);
}

int
IVArelCfg::count(string condition)
{
    return m_db->count("tbl_camerareliva", condition);
}

int
IVArelCfg::clear(const string& json)
{
    return -1;
}
