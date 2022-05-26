#include "cfg_camera_ivaability.h"
#include "db.h"
#include "observer.h"
#include "sys.h"
#include "json/json.h"

static
CameraIvaAbilityCfg  cfg(Cfg::DB_EASYSQLITE);
CameraIvaAbilityCfg* cameraIvaAbilityCfg = &cfg;

CameraIvaAbilityCfg::CameraIvaAbilityCfg(int type)
    : Cfg(type)
{

}

CameraIvaAbilityCfg::~CameraIvaAbilityCfg()
{

}

int
CameraIvaAbilityCfg::add(const string& json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) &&
        !rvalue.isNull() &&
        rvalue.isObject()))
    {
        return -1;
    }

    if (m_db->insert("tbl_camera_ivaability", json) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_camera_ivaability", json, Observer::O_ADD);

    return 0;
}

int
CameraIvaAbilityCfg::del(const string& json)
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
    std::string str_ab = rvalue["ability"].asString();
    snprintf(cond, 128, "addrcode = '%s' and ability = '%s'", str.c_str(), str_ab.c_str());
    std::string con(cond);

    if (m_db->deletex("tbl_camera_ivaability", cond) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_camera_ivaability", json, Observer::O_DELETE);

    return 0;
}

int
CameraIvaAbilityCfg::modify(const string& json)
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

    if (m_db->update("tbl_camera_ivaability", writer.write(rvalue)) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_camera_ivaability", json, Observer::O_MODIFY);

    return 0;
}

int
CameraIvaAbilityCfg::flush(const string& json)
{
    return -1;
}

int
CameraIvaAbilityCfg::query(string& json, string condition)
{
    return m_db->query("tbl_camera_ivaability", json, condition);
}

int
CameraIvaAbilityCfg::count(string condition)
{
    return m_db->count("tbl_camera_ivaability", condition);
}

int
CameraIvaAbilityCfg::clear(const string& json)
{
    return -1;
}
