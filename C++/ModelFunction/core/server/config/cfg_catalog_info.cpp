#include "cfg_catalog_info.h"
#include "db.h"
#include "observer.h"
#include "sys.h"
#include "json/json.h"

static
CatalogInfoCfg cfg(Cfg::DB_EASYSQLITE);
CatalogInfoCfg* catalogInfoCfg = &cfg;

CatalogInfoCfg::CatalogInfoCfg(int type)
:Cfg(type)
{

}

CatalogInfoCfg::~CatalogInfoCfg()
{

}


int
CatalogInfoCfg::add(const string& json)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;

	if (!(reader.parse(json, rvalue, false) &&
		!rvalue.isNull() &&
		rvalue.isObject()))
	{
		return -1;
	}

	if (m_db->insert("tbl_catalog_info", json) < 0)
	{
		return -1;
	}

	setChanged();
	notifyObservers("tbl_catalog_info", json, Observer::O_ADD);

	return 0;

}

int CatalogInfoCfg::del(const string& json)
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

	int id = rvalue["id"].asInt();
	snprintf(cond, 128, "id = '%d'", id);
	std::string con(cond);

	if (m_db->deletex("tbl_catalog_info", cond) < 0)
	{
		return -1;
	}

	setChanged();
	notifyObservers("tbl_catalog_info", json, Observer::O_DELETE);

	return 0;
}

int CatalogInfoCfg::modify(const string& json)
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

	if (m_db->update("tbl_catalog_info", writer.write(rvalue)) < 0)
	{
		return -1;
	}

	setChanged();
	notifyObservers("tbl_catalog_info", json, Observer::O_MODIFY);

	return 0;
}

int
CatalogInfoCfg::query(string& json, string condition)
{
	return m_db->query("tbl_catalog_info", json, condition);
}

int
CatalogInfoCfg::flush(const string& json)
{
	return -1;
}

int
CatalogInfoCfg::count(string condition)
{
	return m_db->count("tbl_catalog_info", condition);
}

int
CatalogInfoCfg::clear(const string& json)
{
	return -1;
}
