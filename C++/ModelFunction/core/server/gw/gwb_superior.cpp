#include "gwb_superior.h"
#include "gw_sipua.h"
GwbSuperior::GwbSuperior(string addrcode, string name, string user, string pswd, string host, int sipPort, int rtspPort,
    int httpPort, int expires, string ifupIp, int bregister, int ivauselocal)
    :Superior(addrcode, name, user, pswd, host, sipPort, rtspPort, httpPort, expires, ifupIp)
{
    m_protocol     = SipUA::P_GWB;
    m_bUnregister  = bregister;
    m_ivauselocal = ivauselocal;
}

GwbSuperior::~GwbSuperior()
{

}

void
GwbSuperior::timerdriver()
{
    /* TODO:  <15-11-17, yourname> */

    Superior::timerdriver();
}


void
GwbSuperior::onAIYX(string &json, void *data, uint32_t size)
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
	((GWSipUA *)sipUA)->push_ivs(rvalue["time"].asString(), alarms, m_subscribedid);
}

void
GwbSuperior::onAIYC(string &json, void *data, uint32_t size)
{

}
