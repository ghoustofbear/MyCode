#include "cfg_pub_info.h"
#include "db.h"
#include "json/json.h"

static
PubInfoCfg  cfg(Cfg::DB_EASYSQLITE);
PubInfoCfg* pubInfoCfg = &cfg;


PubInfoCfg::PubInfoCfg(int type)
    : Cfg(type)
{

}

PubInfoCfg::~PubInfoCfg()
{

}

int
PubInfoCfg::add(const string& json)
{
    //return m_db->compress();
    return -1;
}

int
PubInfoCfg::del(const string& json)
{
    return 0;
}

int
PubInfoCfg::modify(const string& json)
{
    return -1;
}

int
PubInfoCfg::flush(const string& json)
{
    return -1;
}

int
PubInfoCfg::query(string& json, string condition)
{
    int ret = -1;
    if (condition == "tbl_video_dev_code")
    {
        ret = m_db->query("tbl_video_dev_code", json);
    }
    else if (condition == "tbl_non_video_dev_code")
    {
        ret = m_db->query("tbl_non_video_dev_code", json);
    }
    else if (condition == "tbl_camera_code")
    {
        ret = m_db->query("tbl_camera_code", json);
    }
    else if (condition == "tbl_protocol_type")
    {
        ret = m_db->query("tbl_protocol_type", json);
    }

    return ret;
}

int
PubInfoCfg::clear(const string& json)
{
    return -1;
}

