#include "cfg_camera_belong_platform.h"
#include "db.h"
#include "observer.h"
#include "sys.h"
#include "json/json.h"

static
CameraBelongPlatformCfg cfg(Cfg::DB_EASYSQLITE);
CameraBelongPlatformCfg* cameraBelongPlatformCfg = &cfg;

CameraBelongPlatformCfg::CameraBelongPlatformCfg(int type)
:Cfg(type)
{

}

CameraBelongPlatformCfg::~CameraBelongPlatformCfg()
{

}


int
CameraBelongPlatformCfg::add(const string& json)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;

	if (!(reader.parse(json, rvalue, false) &&
		!rvalue.isNull() &&
		rvalue.isObject()))
	{
		return -1;
	}

	if (m_db->insert("tbl_camera_belong_platform", json) < 0)
	{
		return -1;
	}

	setChanged();
	notifyObservers("tbl_camera_belong_platform", json, Observer::O_ADD);

	return 0;

}

int CameraBelongPlatformCfg::del(const string& json)
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

	std::string str = rvalue["platform_addrcode"].asString();
	std::string str1 = rvalue["camera_addrcode"].asString();
	snprintf(cond, 128, "platform_addrcode = '%s' and camera_addrcode = '%s'", str.c_str(), str1.c_str());
	std::string con(cond);

	if (m_db->deletex("tbl_camera_belong_platform", cond) < 0)
	{
		return -1;
	}

	setChanged();
	notifyObservers("tbl_camera_belong_platform", json, Observer::O_DELETE);

	return 0;
}

int CameraBelongPlatformCfg::modify(const string& json)
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

	if (m_db->update("tbl_camera_belong_platform", writer.write(rvalue)) < 0)
	{
		return -1;
	}

	setChanged();
	notifyObservers("tbl_camera_belong_platform", json, Observer::O_MODIFY);

	return 0;
}

int
CameraBelongPlatformCfg::query(string& json, string condition)
{
	return m_db->query("tbl_camera_belong_platform", json, condition);
}

int
CameraBelongPlatformCfg::flush(const string& json)
{
	return -1;
}

int
CameraBelongPlatformCfg::count(string condition)
{
	return m_db->count("tbl_camera_belong_platform", condition);
}

int
CameraBelongPlatformCfg::clear(const string& json)
{
	return -1;
}