#include "ntp.h"
#include "cfg_time.h"
#include "json/json.h"
#include "version.h"

NtpTime::NtpTime()
{
    m_otype = O_MODIFY | O_FLUSH;
    timeCfg->addObserver(this);
}

NtpTime::~NtpTime()
{
    timeCfg->deleteObserver(this);
}

int
NtpTime::init()
{
    string json;
    timeCfg->query(json);

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

    m_allowTimeIp = rvalue[0]["allowTimeIp"].asString();
    m_beTime = rvalue[0]["beTime"].asInt();
    m_doTime = rvalue[0]["doTime"].asInt();
    m_timeInterval = rvalue[0]["timeInterval"].asInt();
    m_ntpId = rvalue[0]["ntpId"].asInt();

    return 0;
}

void
NtpTime::update(const std::string &table, const std::string &json, int type)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) && rvalue.isObject()))
    {
        return;
    }
    m_allowTimeIp = rvalue["allowTimeIp"].asString();
    m_beTime = rvalue["beTime"].asInt();
    m_doTime = rvalue["doTime"].asInt();
    m_timeInterval = rvalue["timeInterval"].asInt();
    m_ntpId = rvalue["ntpId"].asInt();
}

std::string
NtpTime::allowIp()
{
    return m_allowTimeIp;
}

int
NtpTime::beTime()
{
    return m_beTime;
}

int
NtpTime::doTime()
{
    return m_doTime;
}

int
NtpTime::timeInterval()
{
    return m_timeInterval;
}

