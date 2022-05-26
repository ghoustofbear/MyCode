#include "cfg_camera_related.h"
#include "db.h"
#include "observer.h"
#include "sys.h"
#include "json/json.h"

static
CameraRelatedCfg cfg(Cfg::DB_EASYSQLITE);
CameraRelatedCfg* cameraRelatedCfg = &cfg;

CameraRelatedCfg::CameraRelatedCfg(int type)
:Cfg(type)
{

}

CameraRelatedCfg::~CameraRelatedCfg()
{

}


int
CameraRelatedCfg::add(const string& json)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;

	if (!(reader.parse(json, rvalue, false) &&
		!rvalue.isNull() &&
		rvalue.isObject()))
	{
		return -1;
	}

	if (m_db->insert("tbl_camera_related", json) < 0)
	{
		return -1;
	}

	setChanged();
	notifyObservers("tbl_camera_related", json, Observer::O_ADD);

	return 0;

}

int CameraRelatedCfg::del(const string& json)
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
	snprintf(cond, 128, "camera_addrcode = '%s'", str.c_str());
	std::string con(cond);

	if (m_db->deletex("tbl_camera_related", cond) < 0)
	{
		return -1;
	}

	setChanged();
	notifyObservers("tbl_camera_related", json, Observer::O_DELETE);

	return 0;
}

int CameraRelatedCfg::modify(const string& json)
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

	if (m_db->update("tbl_camera_related", writer.write(rvalue)) < 0)
	{
		return -1;
	}

	setChanged();
	notifyObservers("tbl_camera_related", json, Observer::O_MODIFY);

	return 0;
}

int
CameraRelatedCfg::query(string& json, string condition)
{
	return m_db->query("tbl_camera_related", json, condition);
}

int
CameraRelatedCfg::flush(const string& json)
{
	return -1;
}

int
CameraRelatedCfg::count(string condition)
{
	return m_db->count("tbl_camera_related", condition);
}

int
CameraRelatedCfg::clear(const string& json)
{
	return -1;
}