#include "cfg_cam_ivaability_term.h"
#include "db.h"
#include "observer.h"
#include "sys.h"
#include "json/json.h"

static
CamIvaAbilityTermCfg  cfg(Cfg::DB_EASYSQLITE);
CamIvaAbilityTermCfg* camIvaAbilityCfg = &cfg;

CamIvaAbilityTermCfg::CamIvaAbilityTermCfg(int type)
    : Cfg(type)
{

}

CamIvaAbilityTermCfg::~CamIvaAbilityTermCfg()
{

}

int
CamIvaAbilityTermCfg::add(const string& json)
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

    if (m_db->insert("tbl_cam_ivaability_term", wjson) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_cam_ivaability_term", wjson, Observer::O_ADD);

    return 0;
}

int
CamIvaAbilityTermCfg::del(const string& json)
{
    return -1;
}

int
CamIvaAbilityTermCfg::modify(const string& json)
{
    return -1;
}

int
CamIvaAbilityTermCfg::flush(const string& json)
{
    return -1;
}

int
CamIvaAbilityTermCfg::query(string& json, string condition)
{
    return m_db->query("tbl_cam_ivaability_term", json, condition);
}

int
CamIvaAbilityTermCfg::count(string condition)
{
    return m_db->count("tbl_cam_ivaability_term", condition);
}

int
CamIvaAbilityTermCfg::clear(const string& json)
{
    if (m_db->deletex("tbl_cam_ivaability_term", json) < 0)
    {
        return -1;
    }
    return 0;
}
