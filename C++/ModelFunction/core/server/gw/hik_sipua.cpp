#include "hik_sipua.h"
#include "cfg_device.h"
#include "debug.h"
#include "j2x.h"
#include "tls_error.h"

#define PER_SEND_RESOURCE_NUM     (5)
HikSipUA::HikSipUA(int transport, string bindip, int port)
	: GWSipUA(transport, bindip, port)
{

}

HikSipUA::~HikSipUA()
{

}

int
HikSipUA::on_resource(const string &json, string &xml, Superior *super)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	if (!reader.parse(json, rvalue, false))
	{
		return -1;
	}

	string addrcode = rvalue["SIP_XML"]["Item"]["@Code"].asString();
	string fromIndex = rvalue["SIP_XML"]["Item"]["@FromIndex"].asString();
	string toIndex = rvalue["SIP_XML"]["Item"]["@ToIndex"].asString();
	int    start = atoi(fromIndex.c_str()) - 1;
	int    number = atoi(toIndex.c_str()) - atoi(fromIndex.c_str()) + 1;

	if (start < 0 || number < 1)
	{
		LOGE_SIP("invalidate args, fromIndex: %s, toIndex: %s", fromIndex.c_str(), toIndex.c_str());
		return -1;
	}

	char cond[256] = { 0 };

	snprintf(cond, 256, "type>=301 and type<=315;");
	int count = deviceCfg->count(cond);
	if (count <= 0)
	{
		LOGE_SIP(getError());
		return -1;
	}

	memset(cond, 0, sizeof(cond));
	number = number > PER_SEND_RESOURCE_NUM ? PER_SEND_RESOURCE_NUM : number;
	snprintf(cond, 256, "type>=301 and type<=315 limit '%d', '%d';", start, number);

	string resource;
	deviceCfg->query(resource, cond);
	if (resource.empty())
	{
		LOGE_SIP(getError());
		return -1;
	}

	if (!(reader.parse(resource, rvalue, false) && rvalue.isArray()))
	{
		LOGE_SIP("parse err: %s", resource.c_str());
		return -1;
	}

	SHL_Json::StreamWriterBuilder writer;
	SHL_Json::Value               wvalue;

	wvalue["SIP_XML"]["@EventType"] = "Response_Resource";
	wvalue["SIP_XML"]["SubList"]["@Code"] = addrcode;
	wvalue["SIP_XML"]["SubList"]["@RealNum"] = std::to_string(count);
	wvalue["SIP_XML"]["SubList"]["@SubNum"] = std::to_string(rvalue.size());
	wvalue["SIP_XML"]["SubList"]["@FromIndex"] = fromIndex;
	wvalue["SIP_XML"]["SubList"]["@ToIndex"] = std::to_string(start + number);

	for (uint32_t i = 0; i < rvalue.size(); ++i)
	{
		wvalue["SIP_XML"]["SubList"]["Item"][i]["@Code"] = rvalue[i]["addrcode"];
		wvalue["SIP_XML"]["SubList"]["Item"][i]["@Name"] = rvalue[i]["name"];
		wvalue["SIP_XML"]["SubList"]["Item"][i]["@Status"] = "0";
		wvalue["SIP_XML"]["SubList"]["Item"][i]["@DecoderTag"] = "0";
		wvalue["SIP_XML"]["SubList"]["Item"][i]["@Longitude"] = "0";
		wvalue["SIP_XML"]["SubList"]["Item"][i]["@Latitude"] = "0";
		wvalue["SIP_XML"]["SubList"]["Item"][i]["@AreaCode"] = "0";
		wvalue["SIP_XML"]["SubList"]["Item"][i]["@IP"] = rvalue[i]["host"];
		wvalue["SIP_XML"]["SubList"]["Item"][i]["@Port"] = std::to_string(rvalue[i]["port"].asInt());
		wvalue["SIP_XML"]["SubList"]["Item"][i]["@UserName"] = rvalue[i]["username"];
		wvalue["SIP_XML"]["SubList"]["Item"][i]["@Password"] = rvalue[i]["password"];
		wvalue["SIP_XML"]["SubList"]["Item"][i]["@InstallDescrip"] = "0";
		wvalue["SIP_XML"]["SubList"]["Item"][i]["@YZWID"] = "0";
		wvalue["SIP_XML"]["SubList"]["Item"][i]["@YZWType"] = "0";
		wvalue["SIP_XML"]["SubList"]["Item"][i]["@VCHNo"] = std::to_string(rvalue[i]["chnNo"].asInt());
		wvalue["SIP_XML"]["SubList"]["Item"][i]["@DevID"] = "0";
		wvalue["SIP_XML"]["SubList"]["Item"][i]["@DevType"] = "0";
		wvalue["SIP_XML"]["SubList"]["Item"][i]["@SubNum"] = std::to_string(rvalue[i]["chnNum"].asInt());
	}

	json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);

	return 0;
}
