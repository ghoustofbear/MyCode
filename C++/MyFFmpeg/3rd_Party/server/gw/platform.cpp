#include "platform.h"
#include "system.h"
#include "cfg_platform.h"
#include "json/json.h"

Platform::Platform()
{
    m_otype = O_ADD | O_MODIFY | O_DELETE | O_FLUSH;
    platCfg->addObserver(this);
}

Platform::~Platform()
{

}

int
Platform::init()
{
    string json;
    platCfg->query(json);

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

    m_enable   = rvalue[0]["enable"].asBool();
    m_protocol = rvalue[0]["protocol"].asInt();
    m_sport    = rvalue[0]["sipPort"].asInt();
    m_rport    = rvalue[0]["rtspPort"].asInt();
    m_hport    = rvalue[0]["httpPort"].asInt();
    m_rtport   = rvalue[0]["rtpPort"].asInt();
    m_aaaip    = rvalue[0]["aaaServIp"].asString();
    m_sendip   = rvalue[0]["sendIp"].asString();

    return 0;
}

void
Platform::update(const std::string &table, const std::string &json, int type)
{

}

std::string
Platform::addrcode()
{
    return systm->addrcode();
}

int
Platform::sport()
{
    return m_sport;
}

int
Platform::rport()
{
    return m_rport;
}

int
Platform::hport()
{
    return m_hport;
}

int
Platform::rtport()
{
    return m_rtport;
}

string 
Platform::ip()
{
    return m_sendip;
}