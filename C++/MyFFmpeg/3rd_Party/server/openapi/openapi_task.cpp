#include "openapi_task.h"
#include "stk/stk_message_http.h"
#include "json/json.h"
#include "supermgr.h"
#include "bufferev.h"
#include "utiva/typedef.h"
#include "openapi_server.h"
#include "cfg_station.h"

char statDesc[][128] = {
    "��վ", "66kV", "110kV", "220kV", 
    "350kV", "500kV", "35kV������", 
    "750kV������", "Ӫҵ��", "Ӫҵ��",
    "��Ƶ���ƽ̨"
};

OpenAPITask::OpenAPITask()
{
	m_s8ReNum = 0;

}

OpenAPITask::~OpenAPITask()
{

}

void OpenAPITask::stop()
{
    Task::stop();

    openApiSVR->delBuffevListener(m_bev, this);
}

int OpenAPITask::process()
{

	return 0;
}

string& OpenAPITask::replace_all(string &str, const string &oldVal, const string &newVal)
{
	while (true)
	{
		string::size_type pos(0);
		if ((pos = str.find(oldVal)) != string::npos)
		{
			str.replace(pos, oldVal.length(), newVal);
		}
		else
		{
			break;
		}
	}

	return str;
}

void OpenAPITask::onRead(Bufferev *bev, void *data, uint32_t size)
{
	//����http+get+url���͵�json�澯���ݣ����ܳ���1024�ֽڣ�

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
        const char *ps8Header =
            "HTTP/1.1 200 OK\r\n"
            "Host: localhost\r\n";

        stk::HttpMsg httpMsg(m_strBuffer.c_str(), m_strBuffer.size());

        string alarm = httpMsg.GetBody();
        string alarmData = sys->utf2ascii(alarm);

        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;

        if (!reader.parse(alarmData, rvalue, false))
        {
            char *ps8HeaderErr =
                "HTTP/1.1 404 JsonErr\r\n"
                "Host: localhost\r\n";
            bev->write((void*)ps8HeaderErr, strlen(ps8HeaderErr));
            return;
        }

        string strMethod = rvalue["Method"].asString();

        //json��ʽ�澯��������վ
        string ivmData = "";
        SHL_Json::Value item;
        SHL_Json::Value mapList;

        item["TaskId"] = "1";
        item["City"] = "zhuhai";
        item["TransformerSubstation"] = "110kV";
        item["AnalyseResult"] = "1";
        item["AnalyseTime"] = "2020-02-21 11:12:30";
        item["AnalysePictureUrl"] = "http://192.168.80.219/pic/1.jpg";
        item["AlarmState"] = "`����";
        item["AnalyseDesc"] = "�������";

        string jsonStat;
        int ret = stationCfg->query(jsonStat);
        SHL_Json::Reader readerStat;
        SHL_Json::Value  rvalueStat;
        if (readerStat.parse(jsonStat, rvalueStat, false))
        {
            if (rvalueStat.isArray())
            {
                if (!rvalueStat[0]["city"].isNull())
                {
                    item["City"] = rvalueStat[0]["city"].asCString();
                }
                if (!rvalueStat[0]["station_type"].isNull())
                {
                    int istatType = rvalueStat[0]["station_type"].asInt();
                    item["TransformerSubstation"] = statDesc[istatType];
                }
            }
        }

        int AnalyseType = 1;
        int SubAnalyseType = 1;

        if (!rvalue["AnalyseType"].isNull())
        {
            AnalyseType = atoi(rvalue["AnalyseType"].asString().c_str());
        }
        if (!rvalue["SubAnalyseType"].isNull())
        {
            SubAnalyseType = atoi(rvalue["SubAnalyseType"].asString().c_str());
        }

        if (!rvalue["TaskId"].isNull())
        {
            item["TaskId"] = rvalue["TaskId"].asString();
        }

        if (!rvalue["time"].isNull())
        {
            item["AnalyseTime"] = rvalue["time"].asString();
        }
        if (!rvalue["filePath"].isNull())
        {
            item["AnalysePictureUrl"] = rvalue["filePath"].asString();
        }

        mapList = rvalue["mapList"];
        for (int index = 0; index < mapList.size(); ++index)
        {
            SHL_Json::Value ele = mapList[index];
            string defect_type = ele["defect_type"].asString();
            string defect_des = ele["defect_des"].asString();

            if (SUPER_WORKING == AnalyseType)
            {
                item["AnalyseResult"] = std::to_string(4);
                if (1 == (SubAnalyseType & 0x1))
                {
                    if ("��ȫñ����" == defect_type)
                    {
                        item["AnalyseResult"] = std::to_string(0);
                    }
                    else if ("δ����ȫñ" == defect_type)
                    {
                        item["AnalyseResult"] = std::to_string(1);
                    }
                }
                else if (2 == (SubAnalyseType & 0x10))
                {
                    if ("��ȫ����������" == defect_type)
                    {
                        item["AnalyseResult"] = std::to_string(0);
                    }
                    else
                    {
                        item["AnalyseResult"] = std::to_string(2);
                    }
                }
                else if (4 == (SubAnalyseType & 0x100))
                {
                    if ("��װ����" == defect_type)
                    {
                        item["AnalyseResult"] = std::to_string(0);
                    }
                    else if ("δ����װ" == defect_type)
                    {
                        item["AnalyseResult"] = std::to_string(3);
                    }
                }
            }
            else if (SUPER_ACTIVITY == AnalyseType)
            {
                item["AnalyseResult"] = std::to_string(0);
                if (2 == (SubAnalyseType & 0x10))
                {
                    if ("����" == defect_type)
                    {
                        item["AnalyseResult"] = std::to_string(1);
                    }
                }
            }
            else if (SUPER_CLASSIFY == AnalyseType)
            {
                item["AnalyseResult"] = std::to_string(0);
                if ("Խ��/����" == defect_type)
                {
                    item["AnalyseResult"] = std::to_string(1);
                }
            }
            else if (SUPER_INVADE == AnalyseType)
            {
                item["AnalyseResult"] = std::to_string(0);
                if ("Խ��/����" == defect_type)
                {
                    item["AnalyseResult"] = std::to_string(1);
                }
            }
            else if (SUPER_APPEARANCE == AnalyseType)
            {
                item["AnalyseResult"] = defect_type;
            }
            else if (SUPER_FIRE == AnalyseType)
            {
                item["AnalyseResult"] = std::to_string(0);
                if ("����" == defect_type)
                {
                    item["AnalyseResult"] = std::to_string(1);
                }
            }

            item["AlarmState"] = std::to_string(1);
            if ("0" == item["AnalyseResult"].asString())
            {
                item["AlarmState"] = std::to_string(0);
            }
            item["AnalyseDesc"] = defect_type;

            SHL_Json::Value value;
            value["alarms"].append(item);
            value["time"] = item["AnalyseTime"];

            SHL_Json::FastWriter writer;
            ivmData = writer.write(value);

            superMgr->push_alarm(ivmData, NULL, 0);

            bev->write((void*)ps8Header, strlen(ps8Header));
        }

        m_strBuffer = "";
        m_s8ReNum = 0;
        m_bodySize = 0;
        m_bodyTotal = 0;

        m_status = S_CLOSED;
	}
}

void OpenAPITask::onWrite(Bufferev *bev)
{
	
}

void OpenAPITask::onClose(Bufferev *bev)
{

}