#include "superior.h"
#include "sipua.h"
#include "putree.h"
#include "platform.h"
#include "cfg_device.h"
#include "j2x.h"
#include "cfg_sup_platform.h"
#include "pu.h"
#include "system.h"
#include "gw_pu.h"

Superior::Superior(string addrcode, string name, string user, string pswd, string host, int sipPort, int rtspPort, int httpPort, int expires, string ifnetip)
{
    m_addrcode = addrcode;
    m_name     = name;
    m_user     = user;
    m_pswd     = pswd;

    m_host     = host;
    m_sport    = sipPort;
    m_rport    = rtspPort;
    m_hport    = httpPort;
    m_expires  = expires;
    //m_ifnetip  = ifnetip;

    //for test
    m_ifnetip = platform->ip();

    m_registed = false;
    m_rid      = -1;
    m_subscribedid = -1;
    m_subscribeCatalogDid = -1;
    m_subCatalog = false;
    m_alramtype = 0xff000000;
    m_statustype = 0;

    m_bUnregister = true;

    memset(&m_beginTimeAlarm, 0, sizeof(datetime_s));
    memset(&m_beginTimeAlarm, 0, sizeof(datetime_s));
    memset(&m_beginTimeCatalog, 0, sizeof(datetime_s));
    memset(&m_beginTimeCatalog, 0, sizeof(datetime_s));
}

Superior::~Superior()
{
}

string
Superior::addrcode()
{
    return m_addrcode;
}

string
Superior::host()
{
    return m_host;
}

string
Superior::name()
{
    return m_name;
}

int
Superior::hport()
{
    return m_hport;
}


int
Superior::sport()
{
    return m_sport;
}

int
Superior::status()
{
    return m_status;
}

int
Superior::protocol()
{
    return m_protocol;
}

string
Superior::ifnetip()
{
    return m_ifnetip;
}

bool
Superior::isSame(string host, int port)
{
    return (0 == m_host.compare(host) && port == m_sport);
}

int
Superior::start()
{
    putree->addTriggerListener(this);
    m_registThread.setEntry(threadProc, this, T_REGISTER);
    int ret = m_registThread.start();
    return ret;
}

void
Superior::stop()
{
    putree->delTriggerListener(this);
    m_registThread.stop();
}


void
Superior::timerdriver()
{
    AutoLock lock(m_lsAlarmData._mtx);
    tlist<AlarmInfo>::iterator it = m_lsAlarmData.begin();
    if (it != m_lsAlarmData.end())
    {
        AlarmInfo info = (*it);
        if ("alarm" == info.type)
        {
            push_alarm(info.json);
        }
        else if ("presence" == info.type)
        {
            push_status(info.json);
        }

        m_lsAlarmData.erase(it);
    }
}

void
Superior::onAlarm(string &json, void *data, uint32_t size)
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
    sipUA->push_alarm(rvalue["time"].asString(), alarms, m_subscribedid, this);
    sipUA->push_status(rvalue["time"].asString(), alarms, m_subscribedid, this);
}

void
Superior::onAIYX(string &json, void *data, uint32_t size)
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
    sipUA->push_alarm(rvalue["time"].asString(), alarms, m_subscribedid, this);
}

void
Superior::onAIYC(string &json, void *data, uint32_t size)
{

}

void
Superior::registloop()
{
    time_t pre  = 0;
    time_t cur  = time(NULL);
    time_t intv = 30;

    while (m_registThread.active())
    {
        cur = time(NULL);

        if ((cur-pre<=intv) ||
            !m_bUnregister)
        {
            sys->sleep(1000);

            continue;
        }

        pre = cur;

        if (!m_registed)
        {
            if (0 == sipUA->regist(m_ifnetip, m_host, m_sport, m_pswd, m_expires, m_rid, true))
            {
                if (0 == sipUA->regist(m_ifnetip, m_host, m_sport, m_pswd, m_expires, m_rid, false))
                {
                    m_registed = true; 
                    
                    push_resource();
                }
            }
        }
        else
        {
            if (0 == sipUA->regist(m_ifnetip, m_host, m_sport, m_pswd, m_expires, m_rid, false))
            {
                m_registed = true;
            }
            else
            {
                m_registed = false;
            }
        }
    }
}

int        
Superior::keepaliveLoop()
{ 
    m_keekaliveThread.stop();
    return 0; 
}

void *
Superior::threadProc(ThreadInfo *info)
{
    Superior * super = (Superior *)(info->data);

    switch (info->id)
    {
    case T_REGISTER:
        super->registloop();
        break;
    case T_KEEPALIVE:
        super->keepaliveLoop();
        break;
    default:
        break;
    }

    return NULL;
}

int
Superior::push_resource()
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    char cond[256] = { 0 };
    snprintf(cond, 256, "parentCode='%s';", platform->addrcode().c_str());
    int count = deviceCfg->count(cond);
    if (count <= 0)
    {
        LOGE_SIP(getError());
        return -1;
    }

    vector<string> vecDevice;

    memset(cond, 0, sizeof(cond));
    string xml;
    string jresp;
    int left = count;
    for (int index = 0; index < (count + UT_SUPERIOR_PR_NUM-1) / (UT_SUPERIOR_PR_NUM); ++index)
    {
        int number = left > UT_SUPERIOR_PR_NUM ? UT_SUPERIOR_PR_NUM : left;
        int start = index * UT_SUPERIOR_PR_NUM;
        snprintf(cond, 256, "parentCode='%s' and type!='100' limit '%d', '%d';", platform->addrcode().c_str(), start, number);

        string resource;
        int ret = deviceCfg->query(resource, cond);
        if (ret < 0)
        {
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
        wvalue["SIP_XML"]["Code"] = platform->addrcode();
        wvalue["SIP_XML"]["SubList"]["@SubNum"] = std::to_string(count);

        for (uint32_t i = 0; i < rvalue.size(); ++i)
        {
            wvalue["SIP_XML"]["SubList"]["Item"][i]["@Code"] = rvalue[i]["addrcode"].asString();
            wvalue["SIP_XML"]["SubList"]["Item"][i]["@Name"] = rvalue[i]["name"].asString();
            wvalue["SIP_XML"]["SubList"]["Item"][i]["@Status"] = "1";
            wvalue["SIP_XML"]["SubList"]["Item"][i]["@DecoderTag"] = "100";
            wvalue["SIP_XML"]["SubList"]["Item"][i]["@Longitude"] = "0";
            wvalue["SIP_XML"]["SubList"]["Item"][i]["@Latitude"] = "0";
            wvalue["SIP_XML"]["SubList"]["Item"][i]["@SubNum"] = std::to_string(rvalue[i]["chnNum"].asInt());

            vecDevice.push_back(rvalue[i]["addrcode"].asString());
        }
        left -= rvalue.size();

        json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);

        sipUA->push_resource(platform->addrcode(), m_ifnetip, platform->sport(), m_addrcode, m_host, m_sport, xml, jresp);
    }

    for (vector<string>::iterator iter = vecDevice.begin(); iter != vecDevice.end(); ++iter)
    {
        string addrcode = (*iter);
        snprintf(cond, 256, "parentCode='%s';", addrcode.c_str());
        int count = deviceCfg->count(cond);
        if (count < 0)
        {
            LOGE_SIP(getError());
            return -1;
        }
        else if (count == 0)
        {
            continue;
        }

        memset(cond, 0, sizeof(cond));	
        int left = count;
        for (int index = 0; index < (count + (UT_SUPERIOR_PR_NUM-1)) / UT_SUPERIOR_PR_NUM; ++index)
        {
            string xml, jresp;
            SHL_Json::Reader reader;
            SHL_Json::Value  rvalue;

            int number = left > UT_SUPERIOR_PR_NUM ? UT_SUPERIOR_PR_NUM : left;
            int start = index * UT_SUPERIOR_PR_NUM;
            snprintf(cond, 256, "parentCode='%s' and type!='100' limit '%d', '%d';", addrcode.c_str(), start, number);

            string resource;
            int ret = deviceCfg->query(resource, cond);
            if (ret < 0)
            {
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
            wvalue["SIP_XML"]["Code"] = addrcode;
            wvalue["SIP_XML"]["SubList"]["@SubNum"] = std::to_string(count);

            for (uint32_t i = 0; i < rvalue.size(); ++i)
            {
                wvalue["SIP_XML"]["SubList"]["Item"][i]["@Code"] = rvalue[i]["addrcode"];
                wvalue["SIP_XML"]["SubList"]["Item"][i]["@Name"] = rvalue[i]["name"];
                wvalue["SIP_XML"]["SubList"]["Item"][i]["@Status"] = "1";
                wvalue["SIP_XML"]["SubList"]["Item"][i]["@DecoderTag"] = "100";
                wvalue["SIP_XML"]["SubList"]["Item"][i]["@Longitude"] = "0";
                wvalue["SIP_XML"]["SubList"]["Item"][i]["@Latitude"] = "0";
                wvalue["SIP_XML"]["SubList"]["Item"][i]["@SubNum"] = std::to_string(rvalue[i]["chnNum"].asInt());
            }

            left -= rvalue.size();

            json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);

            sipUA->push_resource(platform->addrcode(), m_ifnetip, platform->sport(), m_addrcode, m_host, m_sport, xml, jresp);
        }
    }
    
    return 0;
}

void 
Superior::setregisterstatus(bool bRegister)
{
    if (!bRegister && m_registed)
    {
        sipUA->regist(m_ifnetip, m_host, m_sport,  m_pswd, 0, m_rid, false);
        m_registed = false;
    }
    m_bUnregister = bRegister;

    char cond[128] = { 0 };
    sprintf(cond, "addrcode='%s';", m_addrcode.c_str());

    string json;
    int ret = superCfg->query(json, cond);
    if (0 == ret)
    {
        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;
        SHL_Json::Value  element;

        if (!reader.parse(json, rvalue, false))
        {
            return;
        }

        if (rvalue.isArray() && (1 == rvalue.size()))
        {
            element = rvalue[0];
            element["isregister"] = (bRegister ? 1 : 0);

            SHL_Json::FastWriter write;
            string jsonStr = write.write(element);

            superCfg->modify(jsonStr);
        }
    }
}

void
Superior::setuseivalocalcfg(bool bUseivalocal)
{
    m_ivauselocal = bUseivalocal;

    char cond[128] = { 0 };
    sprintf(cond, "addrcode='%s';", m_addrcode.c_str());

    string json;
    int ret = superCfg->query(json, cond);
    if (0 == ret)
    {
        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;
        SHL_Json::Value  element;

        if (!reader.parse(json, rvalue, false))
        {
            return;
        }

        if (rvalue.isArray() && (1 == rvalue.size()))
        {
            element = rvalue[0];
            element["ivauselocal"] = (bUseivalocal ? 1 : 0);

            SHL_Json::FastWriter write;
            string jsonStr = write.write(element);

            superCfg->modify(jsonStr);
        }       
    } 
}

int
Superior::push_alarm(const string &json)
{
    if (m_subscribedid <= 0)
    {
        return -1;
    }

    string xml, jresp;
    json2xml(json.c_str(), xml);

    char contact[128] = { 0 };
    snprintf(contact, 100, "<sip:%s@%s:%d>", platform->addrcode().c_str(), platform->ip().c_str(), platform->sport());
    sipUA->notify("alarm", m_subscribedid, xml, jresp, contact);

    return 0;
}

int
Superior::push(const string &type, const string &json)
{
    AutoLock lock(m_lsAlarmData._mtx);

    AlarmInfo info;
    info.type = type;
    info.json = json;
    m_lsAlarmData.push_back(info);

    return 0;
}

int
Superior::push_status(const string &json)
{
    if (m_subscribedid <= 0)
    {
        return -1;
    }

    string xml, jresp;
    json2xml(json.c_str(), xml);

    char contact[128] = { 0 };
    snprintf(contact, 100, "<sip:%s@%s:%d>", platform->addrcode().c_str(), platform->ip().c_str(), platform->sport());

    sipUA->notify("presence", m_subscribedid, xml, jresp, contact);

    return 0;
}

int
Superior::push_resource(const string &json)
{
    string xml, jresp;
    json2xml(json.c_str(), xml);

    sipUA->push_resource(platform->addrcode(), m_ifnetip, platform->sport(), m_addrcode, m_host, m_sport, xml, jresp);

    return 0;
}

int
Superior::on_resource(const string &json, string &xml)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(json, rvalue, false))
    {
        return -1;
    }

    string addrcode  = rvalue["SIP_XML"]["Item"]["@Code"].asString();
    string fromIndex = rvalue["SIP_XML"]["Item"]["@FromIndex"].asString();
    string toIndex   = rvalue["SIP_XML"]["Item"]["@ToIndex"].asString();
    int    start     = atoi(fromIndex.c_str()) - 1;
    int    number    = atoi(toIndex.c_str()) - atoi(fromIndex.c_str()) + 1;

    if (start < 0 || number < 1)
    {
        LOGE_SIP("invalidate args, fromIndex: %s, toIndex: %s", fromIndex.c_str(), toIndex.c_str());
        return -1;
    }

    if (systm->addrcode() == addrcode)
    {
        char cond[256] = { 0 };
        snprintf(cond, 256, "parentCode='%s' and type!='100';", addrcode.c_str());
        int count = deviceCfg->count(cond);
        if (count <= 0)
        {
            LOGE_SIP(getError());
            return -1;
        }

        memset(cond, 0, sizeof(cond));
        number = number > UT_SUPERIOR_PR_NUM ? UT_SUPERIOR_PR_NUM : number;
        /* snprintf(cond, 256, "parentCode='%s' limit '%d', '%d';", addrcode.c_str(), start, number); */
        snprintf(cond, 256, "parentCode='%s' and type!='100' limit '%d', '%d';", addrcode.c_str(), start, number);

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
        wvalue["SIP_XML"]["SubList"]["@ToIndex"] = std::to_string(start + 1 + number);

        for (uint32_t i = 0; i < rvalue.size(); ++i)
        {
            wvalue["SIP_XML"]["SubList"]["Item"][i]["@Code"] = rvalue[i]["addrcode"].asString();
            wvalue["SIP_XML"]["SubList"]["Item"][i]["@Name"] = rvalue[i]["name"].asString();
            wvalue["SIP_XML"]["SubList"]["Item"][i]["@Status"] = "1";
            wvalue["SIP_XML"]["SubList"]["Item"][i]["@DecoderTag"] = "100";
            wvalue["SIP_XML"]["SubList"]["Item"][i]["@Longitude"] = "0";
            wvalue["SIP_XML"]["SubList"]["Item"][i]["@Latitude"] = "0";
            wvalue["SIP_XML"]["SubList"]["Item"][i]["@SubNum"] = std::to_string(rvalue[i]["chnNum"].asInt());
        }

        json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);
    }
    else
    {
        Pu *pu = putree->findPu(addrcode);
        if (!pu)
        {
            return -1;
        }
        if (Pu::PROTO_TYPE_HIK == pu->protocol())
        {
            char cond[256] = { 0 };
            snprintf(cond, 256, "parentCode='%s' and type!='100';", addrcode.c_str());
            int count = deviceCfg->count(cond);
            if (count <= 0)
            {
                LOGE_SIP(getError());
                return -1;
            }

            memset(cond, 0, sizeof(cond));
            number = number > UT_SUPERIOR_PR_NUM ? UT_SUPERIOR_PR_NUM : number;
            snprintf(cond, 256, "parentCode='%s' and type!='100' limit '%d', '%d';", addrcode.c_str(), start, number);

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
                wvalue["SIP_XML"]["SubList"]["Item"][i]["@Code"] = rvalue[i]["addrcode"].asString();
                wvalue["SIP_XML"]["SubList"]["Item"][i]["@Name"] = rvalue[i]["name"].asString();
                wvalue["SIP_XML"]["SubList"]["Item"][i]["@Status"] = "1";
                wvalue["SIP_XML"]["SubList"]["Item"][i]["@DecoderTag"] = "100";
                wvalue["SIP_XML"]["SubList"]["Item"][i]["@Longitude"] = "0";
                wvalue["SIP_XML"]["SubList"]["Item"][i]["@Latitude"] = "0";
                wvalue["SIP_XML"]["SubList"]["Item"][i]["@SubNum"] = std::to_string(rvalue[i]["chnNum"].asInt());
            }

            json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);
        }
        else if (Pu::PROTO_TYPE_GW_B == pu->protocol())
        {
            string json;
            if (pu->queryResource(addrcode, atoi(fromIndex.c_str()), atoi(toIndex.c_str()), json) < 0)
            {
                return -1;
            }
            json2xml(json.c_str(), xml);
        }
    }

    return 0;
}
