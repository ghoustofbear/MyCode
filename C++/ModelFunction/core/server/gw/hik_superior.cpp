#include "hik_superior.h"
#include "hik_sipua.h"
#include "cfg_device.h"
#include "j2x.h"
#include "system.h"
#include "platform.h"

#define PER_SEND_RESOURCE_NUM (5)

HikSuperior::HikSuperior(string addrcode, string name, string user, string pswd, string host, int sipPort, int rtspPort,
    int httpPort, int expires, string ifupIp, int bregister, int ivauselocal)
    :Superior(addrcode, name, user, pswd, host, sipPort, rtspPort, httpPort, expires, ifupIp)
{
    ////m_protocol   = SipUA::P_GWB_HS;
    m_bUnregister = bregister;
    m_ivauselocal = ivauselocal;
}

HikSuperior::~HikSuperior()
{

}

void
HikSuperior::timerdriver()
{
    /* TODO:  <15-11-17, yourname> */

    Superior::timerdriver();
}


void
HikSuperior::onAIYX(string &json, void *data, uint32_t size)
{
    if (m_subscribedid < 0)
    {
        return;
    }

    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(json, rvalue, false))
    {
        return;
    }

    SHL_Json::StreamWriterBuilder writer;
    string alarms = SHL_Json::writeString(writer, rvalue["alarms"]);
    ((HikSipUA *)sipUA)->push_ivs(rvalue["time"].asString(), alarms, m_subscribedid);
}

void
HikSuperior::onAIYC(string &json, void *data, uint32_t size)
{

}

int
HikSuperior::push_resource()
{
    string xml, jresp;
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    char cond[256] = { 0 };
    snprintf(cond, 256, "type>=301 and type<=315;");
    int start = 0;
    int number = deviceCfg->count(cond);
    if (number <= 0)
    {
       // LOGE_SIP(getError());
        return -1;
    }

    memset(cond, 0, sizeof(cond));
    snprintf(cond, 256, "type>=301 and type<=315 limit '%d', '%d';", start, number);

    string resource;
    deviceCfg->query(resource, cond);
    if (resource.empty())
    {
     //   LOGE_SIP(getError());
        return -1;
    }

    if (!(reader.parse(resource, rvalue, false) && rvalue.isArray()))
    {
        LOGE_SIP("parse err: %s", resource.c_str());
        return -1;
    }

    SHL_Json::StreamWriterBuilder writer;
    SHL_Json::Value               wvalue;

    wvalue["SIP_XML"]["@EventType"] = "Push_Resourse";
    wvalue["SIP_XML"]["Code"] = systm->addrcode();
    wvalue["SIP_XML"]["SubList"]["@SubNum"] = std::to_string(rvalue.size());

    int index = 0;
    for (uint32_t i = 0; i < rvalue.size(); ++i)
    {
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@Code"] = rvalue[i]["addrcode"];
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@Name"] = rvalue[i]["name"];
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@Status"] = "1";
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@DecoderTag"] = "100";
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@Longitude"] = "0";
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@Latitude"] = "0";
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@SubNum"] = std::to_string(0);
    }

    json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);
    sipUA->push_resource(platform->addrcode(), m_ifnetip, platform->sport(), m_addrcode, m_host, m_sport, xml, jresp);

    return 0;
}

int
HikSuperior::on_resource(const string &json, string &xml)
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
       // LOGE_SIP(getError());
        return -1;
    }

    memset(cond, 0, sizeof(cond));
    number = number > PER_SEND_RESOURCE_NUM ? PER_SEND_RESOURCE_NUM : number;
    snprintf(cond, 256, "type>=301 and type<=315 limit '%d', '%d';", start, number);

    string resource;
    deviceCfg->query(resource, cond);
    if (resource.empty())
    {
      //  LOGE_SIP(getError());
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
