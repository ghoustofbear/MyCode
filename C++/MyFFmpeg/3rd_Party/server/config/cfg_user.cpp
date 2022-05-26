#include "cfg_user.h"
#include "db.h"
#include "observer.h"
#include "sys.h"
#include "md5.h"

#if defined(_WIN32) || (__cplusplus >= 201103L)
#include "json/json.h"
#else
#include "json-0.10.6/json.h"
#endif

static
UserCfg  cfg(Cfg::DB_EASYSQLITE);
UserCfg* userCfg = &cfg;

UserCfg::UserCfg(int type)
    : Cfg(type)
{

}

UserCfg::~UserCfg()
{

}

int
UserCfg::add(const string& json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) &&
            !rvalue.isNull() &&
            rvalue.isObject()))
    {
        return -1;
    }


    //char md5[33] = {0};
    //MD5((uint8_t *)rvalue["pswd"].asString().c_str(), (uint8_t *)md5, sizeof(md5));
    //rvalue["pswd"] = md5;
    rvalue["id"]   = sys->generateUID();
    rvalue["addrcode"] = sys->generateUID();

    SHL_Json::FastWriter writer; 
    std::string      wjson;

    wjson = writer.write(rvalue);
    if (m_db->insert("tbl_user", wjson) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_user", wjson, Observer::O_ADD);

    return 0;
}

int
UserCfg::del(const string& json)
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

    std::string str = rvalue["id"].asString();
    snprintf(cond, 128, "id = '%s'",str.c_str());
    std::string con(cond);
    
    if (m_db->deletex("tbl_user", cond) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_user", json, Observer::O_DELETE);

    return 0;
}

int
UserCfg::modify(const string& json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) &&
        !rvalue.isNull() &&
        rvalue.isObject()))
    {
        return -1;
    }

    //bool bp = rvalue["pswd"].isNull();

    //if (!bp)
    //{
    //    char md5[33] = { 0 };
    //    MD5((uint8_t *)rvalue["pswd"].asString().c_str(), (uint8_t *)md5, sizeof(md5));
    //    rvalue["pswd"] = md5;
    //}

    

    //SHL_Json::FastWriter writer;
    //std::string      wjson;

    //wjson = writer.write(rvalue);
    if (m_db->update("tbl_user", json) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_user", json, Observer::O_MODIFY);

    return 0;
}

int
UserCfg::flush(const string& json)
{
    return -1;
}

int
UserCfg::query(string& json, string condition)
{
    return m_db->query("tbl_user", json, condition);
}

int
UserCfg::clear(const string& json)
{
    return -1;
}
