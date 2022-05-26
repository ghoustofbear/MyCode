#include "preset.h"
#include "cfg_preset.h"
#include "json/json.h"
#include "tls_error.h"


Preset::Preset()
{
	m_otype = O_ADD | O_MODIFY | O_DELETE | O_FLUSH;
	presetCfg->addObserver(this);
}

Preset::~Preset()
{
	presetCfg->deleteObserver(this);
}

int Preset::init()
{
	string json;
	presetCfg->query(json);

	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;

	if (!(reader.parse(json, rvalue, false) && rvalue.isArray()))
	{
		return -1;
	}

	
	for (int i = 0; i < rvalue.size(); i++)
	{
		
		PresetInfo pInfo;
		pInfo.addrcode = rvalue[i]["addrcode"].asString();
		pInfo.name = rvalue[i]["name"].asString();
		pInfo.idx = rvalue[i]["idx"].asInt();
		pInfo.type = rvalue[i]["type"].asInt();
		pInfo.isIVS = rvalue[i]["isIVS"].asInt();
		string key = pInfo.addrcode + "_" + to_string(pInfo.idx);
		m_map[key] = pInfo;
	}
	

	return 0;
}

void
Preset::update(const std::string &table, const std::string &json, int type)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	if (!(reader.parse(json, rvalue, false) && rvalue.isObject()))
	{
		return;
	}
	PresetInfo pInfo;
	pInfo.addrcode = rvalue["addrcode"].asString();
	pInfo.name = rvalue["name"].asString();
	pInfo.idx = rvalue["idx"].asInt();
	pInfo.type = rvalue["type"].asInt();
	pInfo.isIVS = rvalue["isIVS"].asInt();
	string key = pInfo.addrcode + "_" + to_string(pInfo.idx);
	if (type == 2)
	{
		m_map.erase(key);
	}
	else
	{
		m_map[key] = pInfo;
	}
	
	
}

string
Preset::getName(string addrcode,int idx)
{
	string key = addrcode + "_" + to_string(idx);
	return m_map[key].name;
}