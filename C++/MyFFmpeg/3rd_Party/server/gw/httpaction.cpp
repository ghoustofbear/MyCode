#include "httpaction.h"
#include "j2x.h"
#include "json/json.h"
#include "sys.h"
#include "cfg_device.h"
#include "pu.h"
#include "putree.h"
#include "platform.h"

#define PER_SEND_RESOURCE_NUM     (5)
#define PER_SEND_RECORD_NUM       (5)
#define PER_SEND_LOG_NUM          (20)

HttpAction::HttpAction(event_base *eventbase)
	: Bufferev(eventbase)
{
	m_s8ReNum = 0;
	m_iContentLen = 0;
}

HttpAction::~HttpAction()
{

}

void       
HttpAction::onRead(Bufferev *bev, void *data, uint32_t size)
{
	char *src = (char*)data;
	int index = 0;
	for (index = 0; index < size; ++index)
	{
		if (src[index]=='\r' ||
			src[index]=='\n')
		{
			m_s8ReNum++;
		}
		else
		{
			m_s8ReNum = 0;
		}

		if (4 == m_s8ReNum)
		{
			break;
		}
	}

	m_strBuffer += src;
	if (4 == m_s8ReNum)
	{
		stk::HttpMsg httpMsg((const char *)data, index);
		m_iContentLen = httpMsg.GetContentLength_int();

		m_iRecvLen = (size - index - 1);
	}
	else
	{
		m_iRecvLen += size;
	}

	if (m_iRecvLen == m_iContentLen)
	{
		stk::HttpMsg httpMsg(m_strBuffer.c_str(), m_strBuffer.size());
		string method = httpMsg.GetMetthod();

		if ("" != method)
		{
			onRequest(httpMsg);
		}
		else
		{
			onResponse(httpMsg);
		}

		m_strBuffer = "";
		m_s8ReNum = 0;
		m_iContentLen = 0;
		m_iRecvLen = 0;
	}
}

void
HttpAction::onRequest(stk::HttpMsg &httpMsg)
{
	string method = httpMsg.GetMetthod();
	string eventType = httpMsg.GetReqUrl();
	if ("POST" == method)
	{
		if ("/Request_Update_Status" == eventType)
		{
			onUpdate(httpMsg);
		}
		else if ("/KeepAlive" == eventType)
		{
			onHeartbeat(httpMsg);
		}
		else if ("/Request_Resource" == eventType)
		{
			onQueryResourceReq(httpMsg);
		}
		else if ("/Request_History_Video" == eventType)
		{
			onQueryRecordReq(httpMsg);
		}
		else if ("/Request_History_Alarm" == eventType)
		{
			onQueryAlarmLogReq(httpMsg);
		}
	}
}

void
HttpAction::onResponse(stk::HttpMsg &httpMsg)
{

}

int
HttpAction::onUpdate(stk::HttpMsg &httpMsg)
{
	string xml = httpMsg.GetBody();

	string json;
	xml2json(xml.c_str(), json);

	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;

	if (!reader.parse(json, rvalue, false))
	{
		return -1;
	}

	string strType = rvalue["HTTP_XML"]["@EventType"].asString();
	string fromn   = rvalue["HTTP_XML"]["Item"]["@From"].asString();
	string ton     = rvalue["HTTP_XML"]["Item"]["@To"].asString();
	string status  = rvalue["HTTP_XML"]["Item"]["@Status"].asString();
	//string fromhttp = rvalue["HTTP_XML"]["Item"]["@HttpURL"].asString();
	//string fromSip = rvalue["HTTP_XML"]["Item"]["@SipURL"].asString();

	SHL_Json::StreamWriterBuilder writer;
	SHL_Json::Value               value;
	value["HTTP_XML"]["@EventType"] = "Response_Update_Status";
	value["HTTP_XML"]["Item"]["@From"] = ton;
	value["HTTP_XML"]["Item"]["@To"] = fromn;
	value["HTTP_XML"]["Item"]["@Expires"] = std::to_string(3600);

	string jresp;
	json2xml(SHL_Json::writeString(writer, value).c_str(), jresp);

	const char *ps8Header =
		"HTTP/1.1 200 OK\r\n"
		"Host: localhost\r\n"
		"Content-Type: application/xml\r\n"
		"Connection: Keep-Alive\r\n\r\n";

	stk::HttpMsg msg(ps8Header, strlen(ps8Header));
	msg.SetBody(jresp.c_str());
	msg.SetContentLength(strlen(jresp.c_str()));

	string strMsg;
	msg.to_str(strMsg);

	write((char *)strMsg.c_str(), strMsg.size());

	return 0;
}

int
HttpAction::onHeartbeat(stk::HttpMsg &httpMsg)
{
	const char *ps8Header =
		"HTTP/1.1 200 OK\r\n"
		"Host: localhost\r\n"
		"Content-Type: application/xml\r\n"
		"Connection: Keep-Alive\r\n\r\n";

	string xml = httpMsg.GetBody();

	string json;
	xml2json(xml.c_str(), json);

	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;

	if (!reader.parse(json, rvalue, false))
	{
		return -1;
	}

	string strType = rvalue["HTTP_XML"]["@EventType"].asString();
	string fromn = rvalue["HTTP_XML"]["Item"]["@From"].asString();
	string ton = rvalue["HTTP_XML"]["Item"]["@To"].asString();

	stk::HttpMsg msg(ps8Header, strlen(ps8Header));
	msg.SetContentLength(0);

	string strMsg;
	msg.to_str(strMsg);

	write((char *)strMsg.c_str(), strMsg.size());

	return 0;
}

int
HttpAction::onQueryResourceReq(stk::HttpMsg &httpMsg)
{
    const char *ps8Header =
            "HTTP/1.1 200 OK\r\n"
            "Host: localhost\r\n"
            "Content-Type: application/xml\r\n"
            "Connection: Keep-Alive\r\n\r\n";

    do
    {
        string xml = httpMsg.GetBody();
        string json;
        xml2json(xml.c_str(), json);

        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;
        if (!reader.parse(json, rvalue, false))
        {
            break;
        }

        string addrcode = rvalue["HTTP_XML"]["Item"]["@Code"].asString();
        string fromIndex = rvalue["HTTP_XML"]["Item"]["@FromIndex"].asString();
        string toIndex = rvalue["HTTP_XML"]["Item"]["@ToIndex"].asString();
        int    start = atoi(fromIndex.c_str()) - 1;
        int    number = atoi(toIndex.c_str()) - atoi(fromIndex.c_str()) + 1;

        if (start < 0 || number < 1)
        {
            break;
        }

        char cond[256] = { 0 };
        snprintf(cond, 256, "parentCode='%s' and type!='100';", addrcode.c_str());
        int count = deviceCfg->count(cond);
        if (count <= 0)
        {
            break;
        }

        memset(cond, 0, sizeof(cond));
        number = number > PER_SEND_RESOURCE_NUM ? PER_SEND_RESOURCE_NUM : number;
        snprintf(cond, 256, "parentCode='%s' and type!='100' limit '%d', '%d';", addrcode.c_str(), start, number);

        string resource;
        deviceCfg->query(resource, cond);
        if (resource.empty())
        {
            break;
        }

        if (!(reader.parse(resource, rvalue, false) && rvalue.isArray()))
        {
            break;
        }

        SHL_Json::StreamWriterBuilder writer;
        SHL_Json::Value               wvalue;

        wvalue["HTTP_XML"]["@EventType"] = "Response_Resource";
        wvalue["HTTP_XML"]["SubList"]["@Code"] = addrcode;
        wvalue["HTTP_XML"]["SubList"]["@RealNum"] = std::to_string(count);
        wvalue["HTTP_XML"]["SubList"]["@SubNum"] = std::to_string(rvalue.size());
        wvalue["HTTP_XML"]["SubList"]["@FromIndex"] = fromIndex;
        wvalue["HTTP_XML"]["SubList"]["@ToIndex"] = std::to_string(start + number);

        for (uint32_t i = 0; i < rvalue.size(); ++i)
        {
            wvalue["HTTP_XML"]["SubList"]["Item"][i]["@Code"] = rvalue[i]["addrcode"];
            wvalue["HTTP_XML"]["SubList"]["Item"][i]["@Name"] = rvalue[i]["name"];
            wvalue["HTTP_XML"]["SubList"]["Item"][i]["@Status"] = "0";
            wvalue["HTTP_XML"]["SubList"]["Item"][i]["@DecoderTag"] = "0";
            wvalue["HTTP_XML"]["SubList"]["Item"][i]["@Longitude"] = "0";
            wvalue["HTTP_XML"]["SubList"]["Item"][i]["@Latitude"] = "0";
            wvalue["HTTP_XML"]["SubList"]["Item"][i]["@AreaCode"] = "0";
            wvalue["HTTP_XML"]["SubList"]["Item"][i]["@IP"] = rvalue[i]["host"];
            wvalue["HTTP_XML"]["SubList"]["Item"][i]["@Port"] = std::to_string(rvalue[i]["port"].asInt());
            wvalue["HTTP_XML"]["SubList"]["Item"][i]["@UserName"] = rvalue[i]["username"];
            wvalue["HTTP_XML"]["SubList"]["Item"][i]["@Password"] = rvalue[i]["password"];
            wvalue["HTTP_XML"]["SubList"]["Item"][i]["@InstallDescrip"] = "0";
            wvalue["HTTP_XML"]["SubList"]["Item"][i]["@YZWID"] = "0";
            wvalue["HTTP_XML"]["SubList"]["Item"][i]["@YZWType"] = "0";
            wvalue["HTTP_XML"]["SubList"]["Item"][i]["@VCHNo"] = std::to_string(rvalue[i]["chnNo"].asInt());
            wvalue["HTTP_XML"]["SubList"]["Item"][i]["@DevID"] = "0";
            wvalue["HTTP_XML"]["SubList"]["Item"][i]["@DevType"] = "0";
            wvalue["HTTP_XML"]["SubList"]["Item"][i]["@SubNum"] = std::to_string(rvalue[i]["chnNum"].asInt());
        }

        string jresp;
        json2xml(SHL_Json::writeString(writer, wvalue).c_str(), jresp);

        stk::HttpMsg msg(ps8Header, strlen(ps8Header));
        msg.SetBody(jresp.c_str());
        msg.SetContentLength(strlen(jresp.c_str()));

        string strMsg;
        msg.to_str(strMsg);

        write((char *)strMsg.c_str(), strMsg.size());

        return 0;
    }while(0);

    stk::HttpMsg msg1(ps8Header, strlen(ps8Header));
    msg1.SetContentLength(0);

    string strMsg1;
    msg1.to_str(strMsg1);

    write((char *)strMsg1.c_str(), strMsg1.size());
    return -1;
}

int
HttpAction::onQueryRecordReq(stk::HttpMsg &httpMsg)
{
    const char *ps8Header =
		"HTTP/1.1 200 OK\r\n"
		"Host: localhost\r\n"
		"Content-Type: application/xml\r\n"
		"Connection: Keep-Alive\r\n\r\n";

	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;

	SHL_Json::StreamWriterBuilder writer;
	SHL_Json::Value               wvalue;

	do
	{
		string xml = httpMsg.GetBody();
		string json;
		xml2json(xml.c_str(), json);
	
		if (!reader.parse(json, rvalue, false))
		{
			break;
		}

		string addrcode = rvalue["HTTP_XML"]["Item"]["@Code"].asString();
		Pu *pu = putree->findPu(addrcode);
		if (!pu)
		{
			break;
		}

		string fromIndex = rvalue["HTTP_XML"]["Item"]["@FromIndex"].asString();
		string toIndex = rvalue["HTTP_XML"]["Item"]["@ToIndex"].asString();
		string begin = rvalue["HTTP_XML"]["Item"]["@BeginTime"].asString();
		string end = rvalue["HTTP_XML"]["Item"]["@EndTime"].asString();
		string type = rvalue["HTTP_XML"]["Item"]["@Type"].asString();

		const char *sep = strchr(begin.c_str(), 'T');
		if (!sep)
		{
			break;
		}

		datetime_s b;
		datetime_s e;

		string date;
		date.assign(begin.c_str(), sep - begin.c_str());

		const char *p = sep + 1;
		sep = strchr(sep, 'Z');
		string time;
		time.assign(p, sep - p);

		sscanf(date.c_str(), "%d-%d-%d", &b.year, &b.month, &b.day);
		sscanf(time.c_str(), "%d:%d:%d", &b.hour, &b.minute, &b.second);

		sep = strchr(end.c_str(), 'T');
		date.assign(end.c_str(), sep - end.c_str());

		p = sep + 1;
		sep = strchr(sep, 'Z');
		time.assign(p, sep - p);

		sscanf(date.c_str(), "%d-%d-%d", &e.year, &e.month, &e.day);
		sscanf(time.c_str(), "%d:%d:%d", &e.hour, &e.minute, &e.second);

		string records;
		int    start = 0;
		int    number = 0;

		start = atoi(fromIndex.c_str()) - 1;
		number = atoi(toIndex.c_str()) - atoi(fromIndex.c_str()) + 1;

		if (start < 0 || number < 1)
		{
			break;
		}

		number = number > PER_SEND_RECORD_NUM ? PER_SEND_RECORD_NUM : number;

		if ((Pu::PROTO_TYPE_GW_B == pu->protocol()) ||
			(Pu::PROTO_TYPE_GW_B_HS == pu->protocol()) ||
			(Pu::PROTO_TYPE_GW_B_HIK == pu->protocol()))
		{
            if (pu->queryRecord(addrcode, 0xFFFF, b, e, atoi(fromIndex.c_str()), (start+number), records, "") < 0)
			{
				wvalue["HTTP_XML"]["@EventType"] = "Response_History_Video";
				wvalue["HTTP_XML"]["SubList"]["@Code"] = addrcode;
				wvalue["HTTP_XML"]["SubList"]["@RealNum"] = std::to_string(0);
				wvalue["HTTP_XML"]["SubList"]["@SubNum"] = std::to_string(0);
				wvalue["HTTP_XML"]["SubList"]["@FromIndex"] = fromIndex;
				wvalue["HTTP_XML"]["SubList"]["@ToIndex"] = std::to_string(start + number);

				break;
			}

			if (!reader.parse(records, rvalue, false))
			{
				break;
			}

			wvalue["HTTP_XML"]["@EventType"] = "Response_History_Video";
			wvalue["HTTP_XML"]["SubList"]["@Code"] = addrcode;

			string subNum = rvalue["HTTP_XML"]["SubList"]["@SubNum"].asString();
			wvalue["HTTP_XML"]["SubList"]["@RealNum"] = rvalue["HTTP_XML"]["SubList"]["@RealNum"].asString();
			wvalue["HTTP_XML"]["SubList"]["@SubNum"] = subNum;
			wvalue["HTTP_XML"]["SubList"]["@FromIndex"] = fromIndex;
			wvalue["HTTP_XML"]["SubList"]["@ToIndex"] = std::to_string(start + number);

			int iSubNum = atoi(subNum.c_str());

			if (iSubNum > 1)
			{
				for (int i = 0; i < iSubNum; ++i)
				{
					string strUrl = rvalue["HTTP_XML"]["SubList"]["Item"][i]["@FileUrl"].asString();
					string strFileUrl = strUrl.substr(strUrl.find("rtsp:") + 5, strUrl.length());
					string url = string("rtsp://") + platform->ip() + string(":") + std::to_string(platform->rport()) + \
						string("@") + strFileUrl;

					wvalue["HTTP_XML"]["SubList"]["Item"][i]["@FileUrl"] = url;
					wvalue["HTTP_XML"]["SubList"]["Item"][i]["@FileName"] = rvalue["HTTP_XML"]["SubList"]["Item"][i]["@FileName"].asString();
					wvalue["HTTP_XML"]["SubList"]["Item"][i]["@BeginTime"] = rvalue["HTTP_XML"]["SubList"]["Item"][i]["@BeginTime"].asString();
					wvalue["HTTP_XML"]["SubList"]["Item"][i]["@EndTime"] = rvalue["HTTP_XML"]["SubList"]["Item"][i]["@EndTime"].asString();
					wvalue["HTTP_XML"]["SubList"]["Item"][i]["@Size"] = rvalue["HTTP_XML"]["SubList"]["Item"][i]["@Size"].asString();
					wvalue["HTTP_XML"]["SubList"]["Item"][i]["@DecoderTag"] = "0";
					wvalue["HTTP_XML"]["SubList"]["Item"][i]["@Type"] = rvalue["HTTP_XML"]["SubList"]["Item"][i]["@Type"].asString();
				}
			}
			else if (iSubNum == 1)
			{
				string strUrl = rvalue["HTTP_XML"]["SubList"]["Item"]["@FileUrl"].asString();
				string strFileUrl = strUrl.substr(strUrl.find("rtsp:") + 5, strUrl.length());
				string url = string("rtsp://") +platform->ip() + string(":") + std::to_string(platform->rport()) + \
					string("@") + strFileUrl;

				wvalue["HTTP_XML"]["SubList"]["Item"]["@FileUrl"] = url;
				wvalue["HTTP_XML"]["SubList"]["Item"]["@FileName"] = rvalue["HTTP_XML"]["SubList"]["Item"]["@FileName"].asString();
				wvalue["HTTP_XML"]["SubList"]["Item"]["@BeginTime"] = rvalue["HTTP_XML"]["SubList"]["Item"]["@BeginTime"].asString();
				wvalue["HTTP_XML"]["SubList"]["Item"]["@EndTime"] = rvalue["HTTP_XML"]["SubList"]["Item"]["@EndTime"].asString();
				wvalue["HTTP_XML"]["SubList"]["Item"]["@Size"] = rvalue["HTTP_XML"]["SubList"]["Item"]["@Size"].asString();
				wvalue["HTTP_XML"]["SubList"]["Item"]["@DecoderTag"] = "0";
				wvalue["HTTP_XML"]["SubList"]["Item"]["@Type"] = rvalue["HTTP_XML"]["SubList"]["Item"]["@Type"].asString();
			}
		}
		else
		{
			if (pu->queryRecord(Pu::GetChanNoByCode(addrcode), 0xFFFF, b, e, atoi(fromIndex.c_str()), (start + number), records) < 0)
			{
				wvalue["HTTP_XML"]["@EventType"] = "Response_History_Video";
				wvalue["HTTP_XML"]["SubList"]["@Code"] = addrcode;
				wvalue["HTTP_XML"]["SubList"]["@RealNum"] = std::to_string(0);
				wvalue["HTTP_XML"]["SubList"]["@SubNum"] = std::to_string(0);
				wvalue["HTTP_XML"]["SubList"]["@FromIndex"] = fromIndex;
				wvalue["HTTP_XML"]["SubList"]["@ToIndex"] = std::to_string(start + number);

				break;
			}

			if (!reader.parse(records, rvalue, false))
			{
				break;
			}

			wvalue["HTTP_XML"]["@EventType"] = "Response_History_Video";
			wvalue["HTTP_XML"]["SubList"]["@Code"] = addrcode;
			wvalue["HTTP_XML"]["SubList"]["@RealNum"] = std::to_string(rvalue["realNum"].asInt());
			wvalue["HTTP_XML"]["SubList"]["@SubNum"] = std::to_string(rvalue["records"].size());
			wvalue["HTTP_XML"]["SubList"]["@FromIndex"] = fromIndex;
			wvalue["HTTP_XML"]["SubList"]["@ToIndex"] = std::to_string(start + number);

			for (uint32_t i = 0; i < rvalue["records"].size(); ++i)
			{
				string url = string("rtsp://") + platform->ip() + string(":") + std::to_string(platform->rport()) + \
					string("@") + rvalue["records"][i]["fileurl"].asString();

				wvalue["HTTP_XML"]["SubList"]["Item"][i]["@FileUrl"] = url;
				wvalue["HTTP_XML"]["SubList"]["Item"][i]["@FileName"] = rvalue["records"][i]["filename"];
				wvalue["HTTP_XML"]["SubList"]["Item"][i]["@BeginTime"] = rvalue["records"][i]["start"];
				wvalue["HTTP_XML"]["SubList"]["Item"][i]["@EndTime"] = rvalue["records"][i]["end"];
				wvalue["HTTP_XML"]["SubList"]["Item"][i]["@Size"] = std::to_string(rvalue["records"][i]["size"].asInt());
				wvalue["HTTP_XML"]["SubList"]["Item"][i]["@DecoderTag"] = "0";
				wvalue["HTTP_XML"]["SubList"]["Item"][i]["@Type"] = std::to_string(rvalue["records"][i]["type"].asInt());
			}
		}

		string jresp;
		json2xml(SHL_Json::writeString(writer, wvalue).c_str(), jresp);

		stk::HttpMsg msg(ps8Header, strlen(ps8Header));
		msg.SetBody(jresp.c_str());
		msg.SetContentLength(jresp.size());

		string strMsg;
		msg.to_str(strMsg);

		write((char *)strMsg.c_str(), strMsg.size());

		return 0;

	} while (0);


	string jresp1;
	json2xml(SHL_Json::writeString(writer, wvalue).c_str(), jresp1);

	stk::HttpMsg msg1(ps8Header, strlen(ps8Header));
	msg1.SetBody(jresp1.c_str());
	msg1.SetContentLength(jresp1.size());

	string strMsg1;
	msg1.to_str(strMsg1);

	write((char *)strMsg1.c_str(), strMsg1.size());
	return -1;
}

int
HttpAction::onQueryAlarmLogReq(stk::HttpMsg &httpMsg)
{
    const char *ps8Header =
		"HTTP/1.1 200 OK\r\n"
		"Host: localhost\r\n"
		"Content-Type: application/xml\r\n"
		"Connection: Keep-Alive\r\n\r\n";

	string xml = httpMsg.GetBody();
	string json;
	xml2json(xml.c_str(), json);

	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;

	SHL_Json::StreamWriterBuilder writer;
	SHL_Json::Value               wvalue;

	do
	{	
		if (!reader.parse(json, rvalue, false))
		{
			break;
		}

		string addrcode = rvalue["HTTP_XML"]["Item"]["@Code"].asString();
		Pu *pu = putree->findPu(addrcode);
		if (!pu)
		{
			break;
		}

		string fromIndex = rvalue["HTTP_XML"]["Item"]["@FromIndex"].asString();
		string toIndex = rvalue["HTTP_XML"]["Item"]["@ToIndex"].asString();
		string begin = rvalue["HTTP_XML"]["Item"]["@BeginTime"].asString();
		string end = rvalue["HTTP_XML"]["Item"]["@EndTime"].asString();
		string type = rvalue["HTTP_XML"]["Item"]["@Type"].asString();
		string level = rvalue["HTTP_XML"]["Item"]["@Level"].asString();

		const char *sep = strchr(begin.c_str(), 'T');
		if (!sep)
		{
			break;
		}

		datetime_s b;
		datetime_s e;

		string date;
		date.assign(begin.c_str(), sep - begin.c_str());

		const char *p = sep + 1;
		sep = strchr(sep, 'Z');
		string time;
		time.assign(p, sep - p);

		sscanf(date.c_str(), "%d-%d-%d", &b.year, &b.month, &b.day);
		sscanf(time.c_str(), "%d:%d:%d", &b.hour, &b.minute, &b.second);

		sep = strchr(end.c_str(), 'T');
		date.assign(end.c_str(), sep - end.c_str());

		p = sep + 1;
		sep = strchr(sep, 'Z');
		time.assign(p, sep - p);

		sscanf(date.c_str(), "%d-%d-%d", &e.year, &e.month, &e.day);
		sscanf(time.c_str(), "%d:%d:%d", &e.hour, &e.minute, &e.second);

		string records;
		int    start = 0;
		int    number = 0;
		int    realNum = 0;

		start = atoi(fromIndex.c_str()) - 1;
		number = atoi(toIndex.c_str()) - atoi(fromIndex.c_str()) + 1;

		if (start < 0 || number < 1)
		{
			break;
		}

		number = number > PER_SEND_RECORD_NUM ? PER_SEND_RECORD_NUM : number;	

		if ((Pu::PROTO_TYPE_GW_B == pu->protocol()) ||
			(Pu::PROTO_TYPE_GW_B_HS == pu->protocol()) ||
			(Pu::PROTO_TYPE_GW_B_HIK == pu->protocol()))
		{
            if (pu->queryAlarm(addrcode, atoi(type.c_str()), atoi(level.c_str()), b, e, atoi(fromIndex.c_str()), (start+number), records, "") < 0)
			{
				wvalue["HTTP_XML"]["@EventType"] = "Response_History_Alarm";
				wvalue["HTTP_XML"]["SubList"]["@Code"] = addrcode;
				wvalue["HTTP_XML"]["SubList"]["@RealNum"] = std::to_string(0);
				wvalue["HTTP_XML"]["SubList"]["@SubNum"] = std::to_string(0);
				wvalue["HTTP_XML"]["SubList"]["@FromIndex"] = fromIndex;
				wvalue["HTTP_XML"]["SubList"]["@ToIndex"] = std::to_string(start + number);

				break;
			}

			if (!reader.parse(records, rvalue, false))
			{
				break;
			}

			wvalue["HTTP_XML"]["@EventType"] = "Response_History_Alarm";
			wvalue["HTTP_XML"]["SubList"]["@Code"] = addrcode;

			string subNum = rvalue["HTTP_XML"]["SubList"]["@SubNum"].asString();
			wvalue["HTTP_XML"]["SubList"]["@RealNum"] = rvalue["HTTP_XML"]["SubList"]["@RealNum"].asString();
			wvalue["HTTP_XML"]["SubList"]["@SubNum"] = subNum;
			wvalue["HTTP_XML"]["SubList"]["@FromIndex"] = fromIndex;
			wvalue["HTTP_XML"]["SubList"]["@ToIndex"] = std::to_string(start + number);

			int iSubNum = atoi(subNum.c_str());

			if (iSubNum > 1)
			{
				for (int i = 0; i < iSubNum; ++i)
				{
					wvalue["HTTP_XML"]["SubList"]["Item"][i]["@Code"] = rvalue["HTTP_XML"]["SubList"]["Item"][i]["@Code"].asString();
					wvalue["HTTP_XML"]["SubList"]["Item"][i]["@BeginTime"] = rvalue["HTTP_XML"]["SubList"]["Item"][i]["@BeginTime"].asString();
					wvalue["HTTP_XML"]["SubList"]["Item"][i]["@Status"] = rvalue["HTTP_XML"]["SubList"]["Item"][i]["@Status"].asString();
					wvalue["HTTP_XML"]["SubList"]["Item"][i]["@Type"] = rvalue["HTTP_XML"]["SubList"]["Item"][i]["@Type"].asString();
				}
			}
			else if (iSubNum == 1)
			{
				wvalue["HTTP_XML"]["SubList"]["Item"]["@Code"] = rvalue["HTTP_XML"]["SubList"]["Item"]["@Code"].asString();
				wvalue["HTTP_XML"]["SubList"]["Item"]["@BeginTime"] = rvalue["HTTP_XML"]["SubList"]["Item"]["@BeginTime"].asString();
				wvalue["HTTP_XML"]["SubList"]["Item"]["@Status"] = rvalue["HTTP_XML"]["SubList"]["Item"]["@Status"].asString();
				wvalue["HTTP_XML"]["SubList"]["Item"]["@Type"] = rvalue["HTTP_XML"]["SubList"]["Item"]["@Type"].asString();
			}
		}
		else
		{
			int realNum = 0;
			if (pu->queryAlarm(Pu::GetChanNoByCode(addrcode), atoi(type.c_str()), atoi(level.c_str()), b, e, atoi(fromIndex.c_str()), (start + number), records, realNum) < 0)
			{
				wvalue["HTTP_XML"]["@EventType"] = "Response_History_Alarm";
				wvalue["HTTP_XML"]["SubList"]["@Code"] = addrcode;
				wvalue["HTTP_XML"]["SubList"]["@RealNum"] = std::to_string(0);
				wvalue["HTTP_XML"]["SubList"]["@SubNum"] = std::to_string(0);
				wvalue["HTTP_XML"]["SubList"]["@FromIndex"] = fromIndex;
				wvalue["HTTP_XML"]["SubList"]["@ToIndex"] = std::to_string(start + number);

				break;
			}

			if (!reader.parse(records, rvalue, false))
			{
				break;
			}

			wvalue["HTTP_XML"]["@EventType"] = "Response_History_Alarm";
			wvalue["HTTP_XML"]["SubList"]["@Code"] = addrcode;
			wvalue["HTTP_XML"]["SubList"]["@RealNum"] = std::to_string(realNum);
			wvalue["HTTP_XML"]["SubList"]["@SubNum"] = std::to_string(rvalue["records"].size());
			wvalue["HTTP_XML"]["SubList"]["@FromIndex"] = fromIndex;
			wvalue["HTTP_XML"]["SubList"]["@ToIndex"] = std::to_string(start + number);

			for (uint32_t i = 0; i < rvalue["records"].size(); ++i)
			{
				wvalue["HTTP_XML"]["SubList"]["Item"][i]["@Code"] = rvalue["records"][i]["addrcode"];
				wvalue["HTTP_XML"]["SubList"]["Item"][i]["@BeginTime"] = rvalue["records"][i]["start"];
				wvalue["HTTP_XML"]["SubList"]["Item"][i]["@Status"] = std::to_string(rvalue["records"][i]["status"].asInt());
				wvalue["HTTP_XML"]["SubList"]["Item"][i]["@Type"] = std::to_string(rvalue["records"][i]["type"].asInt());
			}
		}

		string jresp;
		json2xml(SHL_Json::writeString(writer, wvalue).c_str(), jresp);

		stk::HttpMsg msg(ps8Header, strlen(ps8Header));
		msg.SetBody(jresp.c_str());
		msg.SetContentLength(jresp.size());

		string strMsg;
		msg.to_str(strMsg);

		write((char *)strMsg.c_str(), strMsg.size());

		return 0;
	} while (0);

	string jresp1;
	json2xml(SHL_Json::writeString(writer, wvalue).c_str(), jresp1);

	stk::HttpMsg msg1(ps8Header, strlen(ps8Header));
	msg1.SetContentLength(jresp1.size());
	msg1.SetBody(jresp1.c_str());

	string strMsg1;
	msg1.to_str(strMsg1);

	write((char *)strMsg1.c_str(), strMsg1.size());
	return -1;
}
