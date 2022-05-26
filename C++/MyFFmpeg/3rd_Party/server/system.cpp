#include "system.h"
#include "cfg_system.h"
#include "json/json.h"
#include "version.h"
#include "onuiupdate.h"
#include "concurrency.h"
#include "sys.h"
#include "triggerlistener.h"

System::System()
{
    m_otype = O_ADD | O_MODIFY | O_DELETE | O_FLUSH;
    sysCfg->addObserver(this);
    m_uimtx = new Mutex;
    m_lastTime = 0;
    m_ui = NULL;
#ifdef UNIX
    m_pid = getpid();
#else

#endif
}

System::~System()
{
    sysCfg->deleteObserver(this);
}

int
System::init()
{
    string json;
    sysCfg->query(json);

    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) && rvalue.isArray()))
    {
        return -1;
    }

    if (1 != rvalue.size())
    {
        return -1;
    }

    m_addrcode = rvalue[0]["addrcode"].asString();
    m_name     = rvalue[0]["name"].asString();
    m_swver    = rvalue[0]["swVersion"].asString();
    m_hwver    = rvalue[0]["hwVersion"].asString();
    m_dbver    = rvalue[0]["dbVersion"].asString();
    m_recycle  = rvalue[0]["recycle"].asInt();
    m_logHoldTime = rvalue[0]["logHoldTime"].asInt();
    m_comment  = rvalue[0]["comment"].asString();
    m_platePath = rvalue[0]["platePath"].asString();
    m_facePath = rvalue[0]["facePath"].asString();
	m_bShowPresetName = rvalue[0]["isShowPresetName"].asInt();
	m_x = rvalue[0]["X"].asInt();
	m_y = rvalue[0]["Y"].asInt();
	m_decoder_name = rvalue[0]["decoder_name"].asString();
	m_decoder_password = rvalue[0]["decoder_password"].asString();


    return 0;
}

void
System::update(const std::string &table, const std::string &json, int type)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) && rvalue.isObject()))
    {
        return;
    }
    m_addrcode = rvalue["addrcode"].asString();
    m_name = rvalue["name"].asString();
    m_swver = rvalue["swVersion"].asString();
    m_hwver = rvalue["hwVersion"].asString();
    m_dbver = rvalue["dbVersion"].asString();
    m_recycle = rvalue["recycle"].asInt();
    m_logHoldTime = rvalue["logHoldTime"].asInt();
    m_comment = rvalue["comment"].asString();
    m_platePath = rvalue["platePath"].asString();
    m_facePath = rvalue["facePath"].asString();
	m_bShowPresetName = rvalue["isShowPresetName"].asInt();
	m_x = rvalue["X"].asInt();
	m_y = rvalue["Y"].asInt();
	m_decoder_name = rvalue["decoder_name"].asString();
	m_decoder_password = rvalue["decoder_password"].asString();
}

std::string
System::addrcode()
{
    return m_addrcode;
}

std::string
System::name()
{
    return m_name;
}

std::string
System::dbver()
{
    return m_dbver;
}

std::string
System::platePath()
{
    return m_platePath;
}

std::string
System::facePath()
{
    return m_facePath;
}


int
System::pport()
{
    return m_pport;
}

int
System::vport()
{
    return m_vport;
}

int
System::iport()
{
    return m_iport;
}

int
System::wport()
{
    return m_wport;
}

int
System::recycle()
{
    return m_recycle;
}

int
System::logHoldTime()
{
    return m_logHoldTime;
}

int
System::bShowPresetName()
{
	return m_bShowPresetName;
}

int
System::X()
{
	return m_x;
}

int
System::Y()
{
	return m_y;
}

std::string
System::decoder_name()
{
	return m_decoder_name;
}

std::string
System::decoder_password()
{
	return m_decoder_password;
}



void
System::setUi(OnUiUpdate *ui)
{
    AutoLock lock(* m_uimtx);
    m_ui = ui;
    m_lastTime =0;
}

void
System::updateUi()
{
    AutoLock lock(* m_uimtx);
    if (m_ui || size() > 0)
    {
        SHL_Json::StreamWriterBuilder writer;
        SHL_Json::Value               value;

        uint64_t tmp_cur_time = sys->currentTimeMillis();
        uint64_t delta = tmp_cur_time - m_lastTime;
        m_lastTime = tmp_cur_time;
        int proCpuUsed = 0, sysCpuUsed = 0;
        int pMemUsedM = 0, vMemUsed = 0, totalMemM = 0, freeMemM = 0, virTotalMemM = 0, virFreeMemM = 0;
        int downspeedKbs = 0, upspeedKbs = 0;
        unsigned int totalDiskM = 0, freeDiskM = 0;

#ifdef UNIX
        //获取CPU使用情况
        static unsigned long long totalcputime1 = 0;
        static unsigned long long procputime1   = 0;
        static unsigned long long syscputime1   = 0;
        unsigned long long totalcputime2 = 0;
        unsigned long long procputime2   = 0;
        unsigned long long syscputime2   = 0;
        sys->getCpuTime(m_pid, totalcputime2, procputime2, syscputime2);
        if (totalcputime1 != 0)
        {
            proCpuUsed = ((procputime2 - procputime1)*100)/(totalcputime2 - totalcputime1);
            sysCpuUsed = ((syscputime2 - syscputime1)*100)/(totalcputime2 - totalcputime1);
        }
        totalcputime1 = totalcputime2;
        procputime1 = procputime2;
        syscputime1 = syscputime2;

        //获取内存使用情况
        sys->getMemUsage(m_pid, pMemUsedM, vMemUsed, totalMemM, freeMemM, virTotalMemM, virFreeMemM);

        //网卡使用情况
        static unsigned long long recvKb1 = 0;
        static unsigned long long sendKb1 = 0;
        unsigned long long recvKb2 = 0;
        unsigned long long sendKb2 = 0;
        sys->getInterfaceIO("eth0", recvKb2, sendKb2);
        if (recvKb1 != 0)
        {
            downspeedKbs = (recvKb2 - recvKb1)/(delta/1000);
            upspeedKbs = (sendKb2 - sendKb1)/(delta/1000);
        }
        recvKb1 = recvKb2;
        sendKb1 = sendKb2;

        //硬盘使用情况
        sys->getDiskUsage(totalDiskM, freeDiskM);
#else
        //获取CPU使用情况
        static unsigned long long totalcputime1 = 0;
        static unsigned long long procputime1 = 0;
        static unsigned long long syscputime1 = 0;
        unsigned long long totalcputime2 = 0;
        unsigned long long procputime2 = 0;
        unsigned long long syscputime2 = 0;
        sys->getCpuTime(m_pid, totalcputime2, procputime2, syscputime2);
        if (totalcputime1 != 0)
        {
            proCpuUsed = ((procputime2 - procputime1) * 100) / (totalcputime2 - totalcputime1);
            sysCpuUsed = ((syscputime2 - syscputime1) * 100) / (totalcputime2 - totalcputime1);
        }
        totalcputime1 = totalcputime2;
        procputime1 = procputime2;
        syscputime1 = syscputime2;

        //获取内存使用情况
        sys->getMemUsage(m_pid, pMemUsedM, vMemUsed, totalMemM, freeMemM, virTotalMemM, virFreeMemM);

        //网卡使用情况
        static unsigned long long recvKb1 = 0;
        static unsigned long long sendKb1 = 0;
        unsigned long long recvKb2 = 0;
        unsigned long long sendKb2 = 0;
        sys->getInterfaceIO("eth0", recvKb2, sendKb2);
        if (recvKb1 != 0)
        {
            downspeedKbs = (recvKb2 - recvKb1) / (delta / 1000);
            upspeedKbs = (sendKb2 - sendKb1) / (delta / 1000);
        }
        recvKb1 = recvKb2;
        sendKb1 = sendKb2;

        //硬盘使用情况
        sys->getDiskUsage(totalDiskM, freeDiskM);


#endif
        value["version"]     = "v1.0";
        value["type"]        = TriggerListener::TR_SYSTEM_STATUS;
        value["sysCpuUsage"] = sysCpuUsed;
        value["appCpuUsage"] = proCpuUsed;
        value["appMemUsage"] = pMemUsedM;
        value["phyMemTotal"] = totalMemM;
        value["phyMemUsage"] = totalMemM - freeMemM;
        value["virMemTotal"] = virTotalMemM;
        value["virMemUsage"] = virTotalMemM - virFreeMemM;
        value["netSendKbps"] = upspeedKbs;
        value["netRecvKbps"] = downspeedKbs;
        value["diskTotal"] = totalDiskM;
        value["diskUsage"] = totalDiskM - freeDiskM;
        string updateUIstr = SHL_Json::writeString(writer, value);

        if (size() > 0)
            notifyAlarm(updateUIstr, NULL, 0);

        if (m_ui)
            m_ui->onUpdate(updateUIstr, NULL, 0);
    }
}

void System::addTriggerListener(TriggerListener *listener)
{
    AutoLock locker(_mtx);
    list<TriggerListener *>::const_iterator i = find(begin(), end(), listener);
    if (i == end())
    {
        push_back(listener);
    }
}

void System::delTriggerListener(TriggerListener *listener)
{
    AutoLock locker(_mtx);
    list<TriggerListener *>::iterator i = find(begin(), end(), listener);
    if (i != end())
    {
        erase(i);
    }
}

void System::notifyAlarm(string &json, void *data, uint32_t size)
{
    AutoLock locker(_mtx);
    for (std::list<TriggerListener *>::const_iterator i = begin(); i != end(); ++i)
    {
        (*i)->onAlarm(json, data, size);
    }
}
