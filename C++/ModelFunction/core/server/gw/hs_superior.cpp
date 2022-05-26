#include "hs_superior.h"
#include "hs_sipua.h"
HSSuperior::HSSuperior(string addrcode, string name, string user, string pswd, string host, int sipPort, int rtspPort,
    int httpPort, int expires, string ifupIp, int bregister, int ivauselocal)
    :Superior(addrcode, name, user, pswd, host, sipPort, rtspPort, httpPort, expires, ifupIp)
{
    ////m_protocol   = SipUA::P_GWB_HS;
    m_bUnregister = bregister;
    m_ivauselocal = ivauselocal;
}

HSSuperior::~HSSuperior()
{

}

void
HSSuperior::timerdriver()
{
    /* TODO:  <15-11-17, yourname> */

    Superior::timerdriver();
}


void
HSSuperior::onAIYX(string &json, void *data, uint32_t size)
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
	((HSSipUA *)sipUA)->push_ivs(rvalue["time"].asString(), alarms, m_subscribedid);
}

void
HSSuperior::onAIYC(string &json, void *data, uint32_t size)
{

}
