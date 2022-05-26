#include "cfg_camera_plate.h"
#include "db.h"
#include "observer.h"
#include "sys.h"
#include "json/json.h"

static
CameraPlateCfg  cfg(Cfg::DB_EASYSQLITE);
CameraPlateCfg* cameraPlateCfg = &cfg;

CameraPlateCfg::CameraPlateCfg(int type)
    : Cfg(type)
{

}

CameraPlateCfg::~CameraPlateCfg()
{

}

int
CameraPlateCfg::add(const string& json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) &&
        !rvalue.isNull() &&
        rvalue.isObject()))
    {
        return -1;
    }

    if (m_db->insert("tbl_camera_plate", json) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_camera_plate", json, Observer::O_ADD);

    return 0;
}

int
CameraPlateCfg::del(const string& json)
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

    std::string str = rvalue["plate_camaddr"].asString();
    snprintf(cond, 128, "plate_camaddr = '%s'", str.c_str(), str.c_str());
    std::string con(cond);

    if (m_db->deletex("tbl_camera_plate", cond) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_camera_plate", json, Observer::O_DELETE);

    return 0;
}

int
CameraPlateCfg::modify(const string& json)
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

    if (m_db->update("tbl_camera_plate", writer.write(rvalue)) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_camera_plate", json, Observer::O_MODIFY);

    return 0;
}

int
CameraPlateCfg::flush(const string& json)
{
    return -1;
}

int
CameraPlateCfg::query(string& json, string condition)
{
    return m_db->query("tbl_camera_plate", json, condition);
}

int
CameraPlateCfg::count(string condition)
{
    return m_db->count("tbl_camera_plate", condition);
}

int
CameraPlateCfg::clear(const string& json)
{
    return -1;
}
