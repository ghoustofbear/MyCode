#include "cfg_user_group.h"
#include "db.h"
#include "observer.h"

#if defined(_WIN32) || (__cplusplus >= 201103L)
#include "json/json.h"
#else
#include "json-0.10.6/json.h"
#endif

static
UserGroupCfg  cfg(Cfg::DB_EASYSQLITE);
UserGroupCfg* usrgrCfg = &cfg;

UserGroupCfg::UserGroupCfg(int type)
    : Cfg(type)
{

}

UserGroupCfg::~UserGroupCfg()
{

}

int
UserGroupCfg::add(const string& json)
{
    if (m_db->insert("tbl_user_group", json) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_user_group", json, Observer::O_ADD);

    return 0;
}

int
UserGroupCfg::del(const string& json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;


    if (!(reader.parse(json, rvalue, false) && !rvalue.isNull() && rvalue.isObject()))
    {
        return -1;
    }

    char cond[128] = { 0 };

    std::string uId = rvalue["userId"].asString();
    std::string gId = rvalue["groupId"].asString();
    snprintf(cond, 128, "userId = '%s' and groupId = '%s'", uId.c_str(), gId.c_str());
    std::string con(cond);

    if (m_db->deletex("tbl_user_group", con) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_user_group", json, Observer::O_DELETE);

    return 0;
}

int
UserGroupCfg::modify(const string& json)
{
    return -1;
}

int
UserGroupCfg::flush(const string& json)
{
    return -1;
}

int
UserGroupCfg::query(string& json, string condition)
{
    return m_db->query("tbl_user_group", json, condition);
}

int
UserGroupCfg::clear(const string& json)
{
    return -1;
}
