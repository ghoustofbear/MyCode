#include "log.h"
#include "json/json.h"
#include "sys.h"
#include "system.h"
#include "onuiupdate.h"

#include "putree.h"

#include "cfg_device_log.h"
#include "cfg_alarm_log.h"
#include "cfg_user_log.h"
#include "cfg_system_log.h"



LogMan::LogMan() :m_ui(NULL)
{

}

LogMan::~LogMan()
{
    m_thread.stop();
}

int
LogMan::start()
{
    m_thread.setEntry(threadProc, this, 1);
    return m_thread.start();
}

int
LogMan::stop()
{
    m_thread.stop();
    return 0;
}

int
LogMan::writeLog(const std::string& table, const std::string json)
{
    LogInfo* info = new LogInfo;
    info->tbl = table;
    info->json = json;
    AutoLock locker(m_wque._mtx);
    m_wque.push(info);
    return 0;
}

void
LogMan::setUi(OnUiUpdate *ui)
{
    m_ui = ui;
}

void *
LogMan::threadProc(ThreadInfo *info)
{
    LogMan * serv = (LogMan *)(info->data);

    switch (info->id)
    {
    case 1:
        serv->writePorc();
        break;
    default:
        break;
    }

    return NULL;
}

void*
LogMan::writePorc()
{
    while (m_thread.active())
    {    
        if (m_wque.size() > 0)
        {
            AutoLock locker(m_wque._mtx);
            LogInfo* info = m_wque.front();
            onWrite(info);
            delete info;
            m_wque.pop();
        }
        else
        {
            sys->sleep(1000);
        }
    }
    return NULL;
}


int  
LogMan::onWrite(LogInfo* info)
{
    int ret = 0;
    if (info->tbl == "tbl_alarm_log")
    {
        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;
        SHL_Json::Value  lvalue;
        SHL_Json::FastWriter writer;
        std::string      wjson;

        if (!(reader.parse(info->json, rvalue, false) &&
            !rvalue.isNull() &&
            rvalue.isObject()))
        {
            return -1;
        }

        if (rvalue["alarms"].isArray())
        {
            lvalue["time"] = rvalue["time"];
            lvalue["name"] = rvalue["deviceName"];
            lvalue["addrcode"] = rvalue["addrcode"];
            lvalue["level"] = rvalue["level"];
            for (int i = 0; i < int(rvalue["alarms"].size()); ++i)
            {
                lvalue["channelNo"] = rvalue["alarms"][i]["channelNo"];
                lvalue["type"]      = rvalue["alarms"][i]["type"];
                lvalue["status"]    = rvalue["alarms"][i]["status"];

                wjson = writer.write(lvalue);
                ret = alarmLogCfg->add(wjson);
            }
        }
    }
    else if (info->tbl == "tbl_user_log")
    {
        ret = userLogCfg->add(info->json);
    }
    else if (info->tbl == "tbl_device_log")
    {
        ret = deviceLogCfg->add(info->json);
    }
    else if (info->tbl == "tbl_system_log")
    {
        ret = systemLogCfg->add(info->json);
    }
    return ret;
}


void
LogMan::onAlarm(string &json, void *data, uint32_t size)
{
    //writeLog();

    //if (m_ui && table == "tbl_device_log")
    //{
    //    m_ui->onUpdate(json, NULL, 0);
    //}
}

void
LogMan::onAIYX(string &json, void *data, uint32_t size)
{

}

void
LogMan::onAIYC(string &json, void *data, uint32_t size)
{

}

void
LogMan::onTimer()
{
    string timeStr = sys->currentTime();
    int yyyy = 0;
    int mm = 0;
    int dd = 0;
    int hh = 0;
    int MM = 0;
    int ss = 0;

    sscanf(timeStr.c_str(), "%4d-%2d-%2d %2d:%2d:%2d", &yyyy, &mm, &dd, &hh, &MM, &ss);

    if (hh == 23 && MM == 59 && ss == 59)
    {
        int tmp = (mm - 1) * 30 + dd - systm->logHoldTime();
        if (tmp <= 0)
        {
            yyyy -= 1;
            tmp += 360;
        }
            

        char        szTime[40];
        snprintf(szTime, 40, "%04d-%02d-%02d 01-01-01", yyyy, tmp/30 + 1, tmp%30);
        string json = string("time<") + string(szTime);

        
        LOGF_MSV("start clear log, current time:%s cond: %s",timeStr.c_str(), json.c_str());

        deviceLogCfg->clear(json);
        alarmLogCfg->clear(json);
        systemLogCfg->clear(json);
        userLogCfg->clear(json);
    }

}


