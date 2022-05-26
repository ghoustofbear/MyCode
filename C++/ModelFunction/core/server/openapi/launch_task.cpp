#include "launch_task.h"
#include "stk/stk_message_http.h"
#include "json/json.h"
#include "bufferev.h"
#include "launch_pu.h"
#include"cfg_device.h"
typedef struct
{
	string             type;
	string             buff;
} resp_s;

LaunchTask::LaunchTask(LaunchServer *server)
{
	m_s8ReNum = 0;
	launchSVR = server;
}

LaunchTask::~LaunchTask()
{

}
int
LaunchTask::response(string type, int code, string body)
{
	char buf[100] = { 0 };
	snprintf(buf, sizeof(buf), "HTTP/1.1 %d OK\r\n", code);

	stk::HttpMsg http(buf, strlen(buf));

	if (body.size() > 0)
	{
		http.SetContentType("application/json");
		http.SetBody(body.c_str());
	}

	resp_s resp;
	resp.type = type;
	http.to_str(resp.buff);

	m_bev->push((uint8_t*)resp.buff.c_str(), resp.buff.size());

	m_status = S_CLOSE_WAIT;

	return 0;
}

void LaunchTask::setPu(Pu*pu)
{
	m_pu = pu;
}

void LaunchTask::stop()
{
    Task::stop();

    launchSVR->delBuffevListener(m_bev, this);
}

int LaunchTask::process()
{

	return 0;
}


void LaunchTask::onRead(Bufferev *bev, void *data, uint32_t size)
{
	char *src = (char*)data;
	uint32_t index = 0;
	for (index = 0; index < size; ++index)
	{
		if (src[index] == '\r' ||
			src[index] == '\n')
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
        m_bodyTotal = httpMsg.GetContentLength_int();

        m_bodySize = (size - index - 1);
    }
    else
    {
        m_bodySize += size;

    }

    if (m_bodySize == m_bodyTotal)
    {
        stk::HttpMsg httpMsg(m_strBuffer.c_str(), m_strBuffer.size());
		string str = httpMsg.GetBody();
		string body = sys->utf2ascii(str);
		string url = httpMsg.GetReqUrl();
		string method = httpMsg.GetMetthod();
		if ("/launch" == url && "POST" == method)
		{
			SHL_Json::Reader reader;
			SHL_Json::Value  rvalue;
			SHL_Json::FastWriter writer;
			SHL_Json::Value  value;
			if (reader.parse(body, rvalue, false))
			{
				//TODO:notifyAlarm
				int msg_type      = rvalue["msg_type"].asInt();
				if (201 == msg_type)
				{
					string device_id  = rvalue["data"]["id_device"].asString();
					int online_status = rvalue["data"]["e_online_status"].asInt();
					int fault_status  = rvalue["data"]["e_fault_status"].asInt();
					int ctrl_mode     = rvalue["data"]["e_ctrl_mode"].asInt();
					int ctrl_priority = rvalue["data"]["e_ctrl_priority"].asInt();
					int run_status    = rvalue["data"]["e_run_status"].asInt();
					string chn_json;
					SHL_Json::Value chn_value;
					char cond[512] = { 0 };
					sprintf(cond, "comment='%s'", device_id.c_str());
					deviceCfg->query(chn_json, cond);
					if (reader.parse(chn_json, chn_value, false) && chn_value.isArray() &&chn_value.size() >= 1)
					{
						string parentcode = chn_value[0]["parentCode"].asString();
						string dev_json;
						SHL_Json::Value dev_value;
						sprintf(cond, "addrcode='%s'", parentcode.c_str());
						deviceCfg->query(dev_json, cond);
						if (reader.parse(dev_json, dev_value, false) && dev_value.isArray() &&dev_value.size() >= 1)
						{
							value["version"] = "1.0";
							value["time"] = sys->currentTime();
							value["deviceName"]  = dev_value[0]["name"].asString();
							value["deviceHost"]  = dev_value[0]["host"].asString();
							value["devicePort"]  = dev_value[0]["port"].asInt();
							value["addrcode"]    = dev_value[0]["addrcode"].asString();
							value["clientIp"]    = m_pu->ip();
							value["clientPort"]  = m_pu->port();
							int channelNo        = chn_value[0]["chnNo"].asInt();
							SHL_Json::Value item;
							item["type"]         = TriggerListener::TR_PTZ_CONTROL_ALARM;
							item["gucode"]       = m_pu->gucode(channelNo);
							item["name"]         = m_pu->guname(channelNo);
							item["channelNo"]    = channelNo;
							if (1 == online_status && 0 == run_status && 0 == ctrl_mode)
							{
								item["status"] = 1;
							}
							else
							{
								item["status"] = 0;
							}
							value["alarms"].append(item);
							string json = writer.write(value);
							if (NULL != m_pu)
							{
								m_pu->notifyAlarm(json, NULL, 0);
							}
							response("", 200, json);
						}
					}
				}
			}

		}
        m_strBuffer = "";
        m_s8ReNum = 0;
        m_bodySize = 0;
        m_bodyTotal = 0;

        m_status = S_CLOSED;
	}
}

void LaunchTask::onWrite(Bufferev *bev)
{
	
}

void LaunchTask::onClose(Bufferev *bev)
{

}