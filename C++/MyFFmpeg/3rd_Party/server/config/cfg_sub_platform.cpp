#include "cfg_sub_platform.h"
#include "db.h"
#include "observer.h"
#include "sys.h"
#include "json/json.h"
static
SuberCfg  cfg(Cfg::DB_EASYSQLITE);
SuberCfg* suberCfg = &cfg;

////////////////////////////////////////////////////////////////////////////////////
static
SuberCamInfoCfg  camcfg(Cfg::DB_EASYSQLITE);
SuberCamInfoCfg* suberCamInfoCfg = &camcfg;

SuberCfg::SuberCfg(int type)
    : Cfg(type)
{

}

SuberCfg::~SuberCfg()
{

}

int
SuberCfg::add(const string& json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) &&
            !rvalue.isNull() &&
            rvalue.isObject()))
    {
        return -1;
    }

    if (m_db->insert("tbl_sub_platform", json) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_sub_platform", json, Observer::O_ADD);

    return 0;
}

int
SuberCfg::del(const string& json)
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
    snprintf(cond, 128, "addrcode = '%s'", str.c_str());
    std::string con(cond);

    if (m_db->deletex("tbl_sub_platform", cond) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_sub_platform", json, Observer::O_DELETE);

    return 0;
}

int
SuberCfg::modify(const string& json)
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

    if (m_db->update("tbl_sub_platform", writer.write(rvalue)) < 0)
    {
        return -1;
    }

    setChanged();
    notifyObservers("tbl_sub_platform", json, Observer::O_MODIFY);

    return 0;
}

int
SuberCfg::flush(const string& json)
{
    return -1;
}

int
SuberCfg::query(string& json, string condition)
{
    return m_db->query("tbl_sub_platform", json, condition);
}

int
SuberCfg::count(string condition)
{
    return m_db->count("tbl_sub_platform", condition);
}

int
SuberCfg::clear(const string& json)
{
    return -1;
}



SuberCamInfoCfg::SuberCamInfoCfg(int type)
:Cfg(type)
{

}

SuberCamInfoCfg::~SuberCamInfoCfg()
{

}

int
SuberCamInfoCfg::add(const string& json)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;

	if (!(reader.parse(json, rvalue, false) &&
		!rvalue.isNull() &&
		rvalue.isObject()))
	{
		return -1;
	}
	string table = "tbl_subplat_cam_"+ rvalue["sub_platform_addrcode"].asString();
	if (m_db->insert(table, json) < 0)
	{
		return -1;
	}
	setChanged();

    SHL_Json::StreamWriterBuilder writer;


    if (!reader.parse(json, rvalue))
    {
        return -1;
    }

    rvalue["sub_platform"] = 0;
    string jsonData = SHL_Json::writeString(writer, rvalue);

    notifyObservers(table, jsonData, Observer::O_ADD);
	return 0;
}
int
SuberCamInfoCfg::del(const string& json)
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
	string table = "tbl_subplat_cam_" + rvalue["sub_platform_addrcode"].asString();
	std::string str = rvalue["camera_addrcode"].asString();
	snprintf(cond, 128, "camera_addrcode = '%s'", str.c_str());
	std::string con(cond);
	if (m_db->deletex(table, cond) < 0)
	{
		return -1;
	}

	setChanged();

    SHL_Json::StreamWriterBuilder writer;

    if (!reader.parse(json, rvalue))
    {
        return -1;
    }

    rvalue["sub_platform"] = 0;
    string jsonData = SHL_Json::writeString(writer, rvalue);

    notifyObservers(table, jsonData, Observer::O_DELETE);

	return 0;
}
int
SuberCamInfoCfg::modify(const string& json)
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
    string table = "tbl_subplat_cam_" + rvalue["sub_platform_addrcode"].asString();

    if (m_db->update(table, json) < 0)
    {
        return -1;
    }

    setChanged();

    SHL_Json::StreamWriterBuilder writer;
    SHL_Json::Value               wvalue;

    if (!reader.parse(json, rvalue))
    {
        return -1;
    }

    rvalue["sub_platform"] = 0;
    string jsonData = SHL_Json::writeString(writer, rvalue);

    notifyObservers(table, jsonData, Observer::O_MODIFY);

    return 0;
}

int
SuberCamInfoCfg::flush(const string& json)
{
	return 0;
}
int
SuberCamInfoCfg::query(string& json, string condition)//to find all data in a sub plat camera info table,the condition means sub plat addrcode
{
	string table = "tbl_subplat_cam_" + condition;//the table name
	return m_db->query(table, json,"");
}

int
SuberCamInfoCfg::createTable(const string &table)
{
	return m_db->createTable(table,"tbl_sub_platform_camera_info");
}

int
SuberCamInfoCfg::deleteTable(const string &table)
{
	return m_db->deleteTable(table, "tbl_sub_platform_camera_info");
}
