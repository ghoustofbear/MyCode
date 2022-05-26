#include "sipua.h"
#include "tevqueue.h"
#include "tls_error.h"
#include "osip2/osip_mt.h"
#include "eXosip2/eXosip.h"
#include "j2x.h"


SipUA::SipUA(string bindIp, int port, char *addrcode)
{
	m_addrcode = addrcode;
	m_bindIp = bindIp;
	m_listenPort = port; 
	m_pfnsubscribecb = NULL;
	m_user = NULL;
	m_subsribeId = -1;
}

SipUA::~SipUA()
{
    
}

void
SipUA::setsubsribecb(pfnsubscribecb fncb, void *user)
{
    m_pfnsubscribecb = fncb;
    m_user = user;
}

int
SipUA::start()
{
    m_eXcontext = eXosip_malloc();
    if (eXosip_init(m_eXcontext))
    {
        ERRE_MSV("eXosip_init failed!");

        return -1;
    }

    if (eXosip_listen_addr(m_eXcontext, IPPROTO_UDP, NULL, m_listenPort, AF_INET, 0))
    {
        ERRE_MSV("eXosip_listen_addr failed!");

        eXosip_quit(m_eXcontext);
        osip_free(m_eXcontext);

        return -1;
    }

    int result = 0;

    m_eventThread.setEntry(threadProc, this, T_EVENTLOOP);
    result += m_eventThread.start();

    return result;
}

void
SipUA::stop()
{
    m_eventThread.stop();

    if (m_eXcontext)
    {
        eXosip_quit(m_eXcontext);
        osip_free(m_eXcontext);
        m_eXcontext = NULL;
    }
}

int
SipUA::subscribe(const string &ton, const string &toh, int top,
                 const string &event, int expires, const string &xml, string &jresp)
{
    int result = 0;
    char from[100];
    char to[100];
    snprintf(from, 100, "<sip:%s@%s:%d>", m_addrcode.c_str(), m_bindIp.c_str(), m_listenPort);
    snprintf(to, 100, "<sip:%s@%s:%d>", ton.c_str(), toh.c_str(), top);

    osip_message_t  *subs = NULL;
    {
        eXAutolock locker(m_eXcontext);
        eXosip_subscription_build_initial_subscribe(m_eXcontext, &subs, to, from, NULL, event.c_str(), expires);
        if (!subs)
        {
            return -1;
        }
        osip_message_set_content_type(subs, "application/xml");
        osip_message_set_body(subs, xml.c_str(), xml.length());
		m_subsribeId = eXosip_subscription_send_initial_request(m_eXcontext, subs);
    }

    osip_call_id_t *call_id = osip_message_get_call_id(subs);
    string strCallID = osip_call_id_get_number(call_id);

    //CallEvent call;
	tevent<std::string, SHL_Json::Value> Tent;
	SHL_Json::Value resp;
    Tent.init(strCallID, resp);
    m_cqueue.add(Tent);
    if (!Tent.wait(5000))
    {
        eXosip_insubscription_remove(m_eXcontext, m_subsribeId);
        m_subsribeId = -1;
        result = -1;
    }
    else
    {
		SHL_Json::StreamWriterBuilder writer;
		jresp = SHL_Json::writeString(writer, resp);
    }

    m_cqueue.clear(Tent);
    return result;
}

int
SipUA::refreshsubscribe(const string &ton, const string &toh, int top,
          const string &event, int expires, const string &xml, string &jresp)
{
	int result = 0;
	char from[100];
	char to[100];
	snprintf(from, 100, "<sip:%s@%s:%d>", m_addrcode.c_str(), m_bindIp.c_str(), m_listenPort);
	snprintf(to, 100, "<sip:%s@%s:%d>", ton.c_str(), toh.c_str(), top);

	osip_message_t  *subs = NULL;
	{
		eXAutolock locker(m_eXcontext);
		
		eXosip_subscription_build_refresh_request(m_eXcontext, m_subsribeId, &subs);

		osip_message_set_content_type(subs, "application/xml");
		osip_message_set_body(subs, xml.c_str(), xml.length());

		char tmp[512] = { 0 };
		snprintf(tmp, 10, "%i", expires);
		osip_message_set_expires(subs, tmp);

		osip_message_set_header(subs, "Event", event.c_str());

		eXosip_subscription_send_refresh_request(m_eXcontext, m_subsribeId, subs);
	}

	osip_call_id_t *call_id = osip_message_get_call_id(subs);
	string strCallID = osip_call_id_get_number(call_id);

	//CallEvent call;
	tevent<std::string, SHL_Json::Value> Tent;
	SHL_Json::Value resp;
    Tent.init(strCallID, resp);
    m_cqueue.add(Tent);
    if (!Tent.wait(5000))
    {
        result = -1;
    }
    else
    {
        SHL_Json::StreamWriterBuilder writer;
        jresp = SHL_Json::writeString(writer, resp);
    }

    m_cqueue.clear(Tent);
    return result;
}

int
SipUA::message(const string &ton, const string &toh, int top,
               const string &xml, string &xresp)
{
    int result = 0;

    char from[100];
    char to  [100];
    snprintf(from, 100, "<sip:%s@%s:%d>", m_addrcode.c_str(), m_bindIp.c_str(), m_listenPort);
    snprintf(to,   100, "<sip:%s@%s:%d>", ton.c_str(), toh.c_str(), top);

    osip_message_t *message = NULL;
    {
        eXAutolock locker(m_eXcontext);
        eXosip_message_build_request(m_eXcontext, &message, "MESSAGE", to, from, NULL);
        if (message == NULL)
        {
            return -1;
        }

        osip_message_set_contact(message, from);
        osip_message_set_content_type(message, "application/xml");
        osip_message_set_body(message, xml.c_str(), xml.size());
        eXosip_message_send_request(m_eXcontext, message);
    }

    osip_call_id_t *call_id = osip_message_get_call_id (message);
    string strCallID        = osip_call_id_get_number(call_id);

    //CallEvent call;
    tevent<std::string, SHL_Json::Value> event;
    SHL_Json::Value resp;
    event.init(strCallID, resp);
    m_cqueue.add(event);
    if (!event.wait(5000))
    {
        result = - 1;
    }
    else
    {
        SHL_Json::StreamWriterBuilder writer;

        string json = SHL_Json::writeString(writer, resp);

        json2xml(json.c_str(), xresp);
    }

    m_cqueue.clear(event);
    return result;    
}
int
SipUA::invite(const string &ton, const string &toh, int top,
              const string &sdp, string &jresp)
{
    osip_message_t *invite;
    char from[100];
    char to[100];
    snprintf(from, 100, "<sip:%s@%s:%d>", m_addrcode.c_str(), m_bindIp.c_str(), m_listenPort);
    snprintf(to, 100, "<sip:%s@%s:%d>", ton.c_str(), toh.c_str(), top);
    char  subject[100] = { 0 };
    snprintf(subject, 100, "%s:0-4-0,%s:1", ton.c_str(), m_addrcode.c_str());
    int result = 0;
    string strCallID;
    {
        eXAutolock locker(m_eXcontext);
        eXosip_call_build_initial_invite(m_eXcontext, &invite, to, from, NULL, subject);
        if (!invite)
        {
            return -1;
        }
        osip_call_id_t *call_id = osip_message_get_call_id(invite);
        strCallID = osip_call_id_get_number(call_id);

        osip_message_set_content_type(invite, "application/SDP");
        osip_message_set_body(invite, sdp.c_str(), sdp.length());
        eXosip_call_send_initial_invite(m_eXcontext, invite);
    }

    //CallEvent call;
    tevent<std::string, SHL_Json::Value> event;
    SHL_Json::Value resp;
    event.init(strCallID, resp);
    m_cqueue.add(event);
    if (!event.wait(5000))
    {
        result = -1;
    }
    else
    {
        SHL_Json::StreamWriterBuilder writer;
        jresp = SHL_Json::writeString(writer, resp);
    }

    m_cqueue.clear(event);
    return result;
}

void
SipUA::bye(int32_t s32CallId, int32_t s32DialogId)
{
    if (s32CallId <= 0 ||
        s32DialogId <= 0)
    {
        return;
    }

    eXosip_lock(m_eXcontext);

    osip_message_t *request = NULL;

    eXosip_call_build_info(m_eXcontext, s32DialogId, &request);
    if (NULL == request)
    {
        return;
    }
    osip_call_id_t *call_id = osip_message_get_call_id(request);
    string strCallID = osip_call_id_get_number(call_id);
    osip_message_free(request);

    eXosip_call_terminate(m_eXcontext, s32CallId, s32DialogId);
    eXosip_unlock(m_eXcontext);
}

int
SipUA::notify(const string &ton, const string &toh, int top, const string &event, const string &xml, string &jresp)
{
    int s32Ret = 0;

    char from[100];
    char to[100];
    snprintf(from, 100, "<sip:%s@%s:%d>", m_addrcode.c_str(), m_bindIp.c_str(), m_listenPort);
    snprintf(to,   100, "<sip:%s@%s:%d>", toh.c_str(), toh.c_str(), top);

    osip_message_t *request = NULL;
    {
        eXAutolock locker(m_eXcontext);
        s32Ret = eXosip_message_build_request(m_eXcontext, &request, "NOTIFY", to, from, NULL);
        if (!request)
        {
            return -1;
        }
        osip_message_set_header(request, "Event", event.c_str());
        osip_message_set_content_type(request, "application/xml");
        osip_message_set_body(request, xml.c_str(), xml.length());

        //osip_list_remove(&request->contacts, 0);
        //S8 s8Contact[100] = { 0 };
        //snprintf(s8Contact, 100, "<sip:%s@%s:%s>", m_strAddrCode.c_str(), m_strHost.c_str(), m_strPort.c_str());
        //osip_message_set_contact(request, s8Contact);
        s32Ret = eXosip_message_send_request(m_eXcontext, request);
    }
    return s32Ret;
}

int
SipUA::notify(const string &event, int did, const string &xml, string &jresp, string contact)
{
    eXAutolock locker(m_eXcontext);
    osip_message_t *request = NULL;

    int result = eXosip_insubscription_build_notify(m_eXcontext, did, EXOSIP_SUBCRSTATE_ACTIVE, 0, &request);
    if (OSIP_SUCCESS != result || !request)
    {
        return -1;
    }

    osip_message_set_header(request, "Event", event.c_str());

    osip_message_set_content_type(request, "application/xml");
    osip_message_set_body(request, xml.c_str(), xml.size());

    osip_list_remove(&request->contacts, 0);
    osip_message_set_contact(request, contact.c_str());

    return eXosip_insubscription_send_request(m_eXcontext, did, request);
}

int
SipUA::ack()
{
    osip_message_t *ack;
    eXosip_call_build_ack(m_eXcontext, m_eXevent->tid, &ack);
    eXosip_call_send_ack(m_eXcontext, m_eXevent->tid, ack);

    return 0;
}

int
SipUA::on_register_response(osip_message_t *sip)
{
    SHL_Json::Value resp;

    string cid = osip_call_id_get_number(osip_message_get_call_id(sip));

    resp["code"] = sip->status_code;
    m_cqueue.response(cid, resp);

    return 0;
}

int
SipUA::on_bye_response(osip_message_t *sip)
{
    SHL_Json::Value stRsp;

    osip_call_id_t *call_id = osip_message_get_call_id(sip);
    string strCallID = osip_call_id_get_number(call_id);

    stRsp["code"] = sip->status_code;
    m_cqueue.response(strCallID, stRsp);

    return 0;
}

int
SipUA::on_request(osip_message_t *sip)
{
    if (MSG_IS_REGISTER(sip))
    {
        on_register(sip);
    }
    else if (MSG_IS_NOTIFY(sip))
    {
        on_notify(sip);
    }

    return 0;
}

int
SipUA::doAnswer(int u32Code)
{
    osip_message_t *answer = NULL;

    if (!m_sipUrl)
    {
        return -1;
    }

    if (200 == u32Code)
    {
        eXosip_message_build_answer(m_eXcontext, m_eXevent->tid, 200, &answer);

        osip_contact_t *contact = NULL;
        osip_contact_init(&contact);
        //osip_uri_t *url = NULL;
        //osip_uri_clone(m_sipUrl, &url);

        //osip_contact_set_url(contact, url);

        osip_contact_clone(m_contact, &contact);
        osip_contact_param_add(contact, osip_strdup("expires"), osip_strdup("3600"));
        osip_list_add(&answer->contacts, contact, -1);

        eXosip_message_send_answer(m_eXcontext, m_eXevent->tid, 200, answer);
    }
    else if (401 == u32Code)
    {
        char *str_url;
        osip_uri_to_str(m_sipUrl, &str_url);

        char realm[24] = { 0 };
        char tmp[24] = { 0 };
        strncpy(tmp, str_url + 4, 10);
        snprintf(realm, 24, "\"%s\"", tmp);
        osip_free(str_url);

        eXosip_message_build_answer(m_eXcontext, m_eXevent->tid, 401, &answer);

        osip_www_authenticate_t *www_auth;

        osip_www_authenticate_init(&www_auth);

        osip_www_authenticate_set_auth_type(www_auth, osip_strdup("Digest"));
        osip_www_authenticate_set_algorithm_MD5(www_auth);
        osip_www_authenticate_set_realm(www_auth, osip_strdup(realm));
        osip_www_authenticate_set_nonce(www_auth, osip_strdup("\"d56d544323b72bb85fdb78e7759876dd\""));
        osip_www_authenticate_set_opaque(www_auth, osip_strdup("\" \""));
        osip_www_authenticate_set_qop_options(www_auth, osip_strdup("\"auth\""));

        osip_list_add(&answer->www_authenticates, www_auth, -1);
        eXosip_message_send_answer(m_eXcontext, m_eXevent->tid, 401, answer);
    }

    return 0;
}

int
SipUA::on_response(osip_message_t *sip)
{
    if (MSG_IS_RESPONSE_FOR(sip, "INVITE"))
    {
        on_invite_response(sip);
    }
    else if (MSG_IS_RESPONSE_FOR(sip, "NOTIFY"))
    {
        on_notify_response(sip);
    }
    else if (MSG_IS_RESPONSE_FOR(sip, "REGISTER"))
    {
        on_register_response(sip);
    }
    else if (MSG_IS_RESPONSE_FOR(sip, "MESSAGE"))
    {
        on_message_response(sip);
    }
    else if (MSG_IS_RESPONSE_FOR(sip, "SUBSCRIBE"))
    {
        on_subscribe_response(sip);
    }

    return 0;
}

int
SipUA::makeXmlReq(SIP_REQ_S req, string& xml)
{
    SHL_Json::StreamWriterBuilder writer;
    SHL_Json::Value               wvalue;

    if ("Request_Resource" == req.strType)
    {
        wvalue["SIP_XML"]["@EventType"]         = "Request_Resource";
        wvalue["SIP_XML"]["Item"]["@Code"]      = string((const char*)req.stResouceReq.ps8AddrCode);
        wvalue["SIP_XML"]["Item"]["@FromIndex"] = std::to_string(req.stResouceReq.s32FromIndex);
        wvalue["SIP_XML"]["Item"]["@ToIndex"]   = std::to_string(req.stResouceReq.s32ToIndex);
    }
    else if ("Request_History_Video" == req.strType)
    {
        wvalue["SIP_XML"]["@EventType"] = "Request_History_Video";
        wvalue["SIP_XML"]["Item"]["@Code"]      = string((const char*)req.stRecordReq.ps8ChnAddrCode);
        wvalue["SIP_XML"]["Item"]["@UserCode"]  = string((const char*)req.stRecordReq.ps8UserAddrCode);
        wvalue["SIP_XML"]["Item"]["@Type"] = std::to_string(req.stRecordReq.u32RecType);
        wvalue["SIP_XML"]["Item"]["@BeginTime"] = string((const char*)req.stRecordReq.ps8Begin);
        wvalue["SIP_XML"]["Item"]["@EndTime"]   = string((const char*)req.stRecordReq.ps8End);
        wvalue["SIP_XML"]["Item"]["@FromIndex"] = std::to_string(req.stRecordReq.s32FromIndex);
        wvalue["SIP_XML"]["Item"]["@ToIndex"]   = std::to_string(req.stRecordReq.s32ToIndex);
    }
    else if ("Request_History_Alarm" == req.strType)
    {
        wvalue["SIP_XML"]["@EventType"]         = "Request_History_Alarm";
        wvalue["SIP_XML"]["Item"]["@Code"]      = string((const char*)req.stAlarmLogReq.ps8ChnAddrCode);
        wvalue["SIP_XML"]["Item"]["@UserCode"]  = string((const char*)req.stAlarmLogReq.ps8UserAddrCode);
        wvalue["SIP_XML"]["Item"]["@Type"] = std::to_string(req.stAlarmLogReq.u32AlarmType);
        wvalue["SIP_XML"]["Item"]["@BeginTime"] = string((const char*)req.stAlarmLogReq.ps8Begin);
        wvalue["SIP_XML"]["Item"]["@EndTime"]   = string((const char*)req.stAlarmLogReq.ps8End);
        wvalue["SIP_XML"]["Item"]["@Level"] = std::to_string(req.stAlarmLogReq.u32AlarmLevel);
        wvalue["SIP_XML"]["Item"]["@FromIndex"] = std::to_string(req.stAlarmLogReq.s32FromIndex);
        wvalue["SIP_XML"]["Item"]["@ToIndex"] = std::to_string(req.stAlarmLogReq.s32ToIndex);
    }
    else if ("Control_Camera" == req.strType)
    {
        wvalue["SIP_XML"]["@EventType"]            = "Control_Camera";
        wvalue["SIP_XML"]["Item"]["@Code"]         = string((const char*)req.stPtzReq.ps8ChnAddrCode);
        wvalue["SIP_XML"]["Item"]["@Command"] = std::to_string(req.stPtzReq.u32Cmd);
        wvalue["SIP_XML"]["Item"]["@CommandPara1"] = std::to_string(req.stPtzReq.u32Param1);
        wvalue["SIP_XML"]["Item"]["@CommandPara2"] = std::to_string(req.stPtzReq.u32Param2);
        wvalue["SIP_XML"]["Item"]["@CommandPara3"] = std::to_string(req.stPtzReq.u32Param3);
        
    }
    else if ("Subscribe_Status" == req.strType ||
             "Subscribe_IVS"    == req.strType ||
             "Subscribe_VI"     == req.strType)
    {
        wvalue["SIP_XML"]["@EventType"]    = "Subscribe_Status";
        wvalue["SIP_XML"]["Item"]["@Code"] = string((const char*)req.stSubscribeReq.ps8ChnAddrCode);
    }
    else if ("Subscribe_Alarm" == req.strType)
    {
        wvalue["SIP_XML"]["@EventType"]    = "Subscribe_Alarm";
        wvalue["SIP_XML"]["Item"]["@Code"] = string((const char*)req.stSubscribeReq.ps8ChnAddrCode);
        wvalue["SIP_XML"]["Item"]["@Type"] = std::to_string(req.stSubscribeReq.u32AlarmType);
    }
    else if ("Alg_Ability_Query" == req.strType)
    {
        wvalue["SIP_XML"]["@EventType"] = "Alg_Ability_Query";
        wvalue["SIP_XML"]["Item"]["@Code"] = string((const char*)req.stSubscribeReq.ps8ChnAddrCode);
    }
    else if ("Alg_Task_Dispatch" == req.strType)
    {
        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;

        if (!reader.parse((char*)req.stAlgReq.data, rvalue, false))
        {
            return -1;
        }

        wvalue["SIP_XML"]["@EventType"] = "Alg_Task_Dispatch";
        int AnalyseType = atoi(rvalue["Item"][0]["AnalyseType"].asString().c_str());

        wvalue["SIP_XML"]["Item"]["@TaskId"] = rvalue["Item"][0]["TaskId"].asString();
        wvalue["SIP_XML"]["Item"]["@TaskName"] = rvalue["Item"][0]["TaskName"].asString();
        if (15 != AnalyseType)
		{
			wvalue["SIP_XML"]["Item"]["@Code"] = (char*)req.stAlgReq.ps8AddrCode;
		}		
        if(!rvalue["Item"][0]["PrePoint"].isNull())
            wvalue["SIP_XML"]["Item"]["@PrePoint"] = rvalue["Item"][0]["PrePoint"].asString();
        if(!rvalue["Item"][0]["TaskPrj"].isNull())
            wvalue["SIP_XML"]["Item"]["@TaskPrj"] = rvalue["Item"][0]["TaskPrj"].asString();
        wvalue["SIP_XML"]["Item"]["@AnalyseType"] = rvalue["Item"][0]["AnalyseType"].asString();
        wvalue["SIP_XML"]["Item"]["@AnalyseSubType"] = rvalue["Item"][0]["AnalyseSubType"].asString();
        wvalue["SIP_XML"]["Item"]["@AnalyseExe"] = rvalue["Item"][0]["AnalyseExe"].asString();
        if(!rvalue["Item"][0]["AreaNum"].isNull())
            wvalue["SIP_XML"]["Item"]["@AreaNum"] = rvalue["Item"][0]["AreaNum"].asString();

        int areaNum = rvalue["Item"][0]["Area"].size();
        if (1 == areaNum)
        {
            wvalue["SIP_XML"]["Item"]["Area"]["@PointNum"] = rvalue["Item"][0]["Area"][0]["PointNum"].asString();
            wvalue["SIP_XML"]["Item"]["Area"]["@Point1_X"] = rvalue["Item"][0]["Area"][0]["Point1_X"].asString();
            wvalue["SIP_XML"]["Item"]["Area"]["@Point1_Y"] = rvalue["Item"][0]["Area"][0]["Point1_Y"].asString();
            wvalue["SIP_XML"]["Item"]["Area"]["@Point2_X"] = rvalue["Item"][0]["Area"][0]["Point2_X"].asString();
            wvalue["SIP_XML"]["Item"]["Area"]["@Point2_Y"] = rvalue["Item"][0]["Area"][0]["Point2_Y"].asString();
        }
        
        if (2 == AnalyseType ||
            5 == AnalyseType ||
            6 == AnalyseType ||
            9 == AnalyseType)
        {
            wvalue["SIP_XML"]["Item"]["@Threshold"] = rvalue["Item"][0]["Threshold"].asString();
        }
        else if (7 == AnalyseType)
        {
            wvalue["SIP_XML"]["Item"]["@ParkingTime"] = rvalue["Item"][0]["ParkingTime"].asString();
        }
        else if (8 == AnalyseType)
        {
            wvalue["SIP_XML"]["Item"]["@HighThreshold"] = rvalue["Item"][0]["HighThreshold"].asString();
            wvalue["SIP_XML"]["Item"]["@LowThreshold"] = rvalue["Item"][0]["LowThreshold"].asString();
        }
        else if (10 == AnalyseType)
        {
            wvalue["SIP_XML"]["Item"]["@PreviousDay"] = rvalue["Item"][0]["PreviousDay"].asString();
            wvalue["SIP_XML"]["Item"]["@FlutuationTimeNum"] = rvalue["Item"][0]["FlutuationTimeNum"].asString();
            wvalue["SIP_XML"]["Item"]["@FlutuationInterval"] = rvalue["Item"][0]["FlutuationInterval"].asString();
            wvalue["SIP_XML"]["Item"]["Period"]["@FlutuationTimeStart"] = rvalue["Item"][0]["Period"]["FlutuationTimeStart"].asString();
            wvalue["SIP_XML"]["Item"]["Period"]["@FlutuationTimeEnd"]   = rvalue["Item"][0]["Period"]["FlutuationTimeEnd"].asString();
        }
        else if (15 == AnalyseType)
        {
            wvalue["SIP_XML"]["Item"]["Camara"]["@Code"] = (char*)req.stAlgReq.ps8AddrCode;
            wvalue["SIP_XML"]["Item"]["Camara"]["@ColorThresholdLow"] = rvalue["Item"][0]["Camara"][0]["ColorThresholdLow"].asString();
            wvalue["SIP_XML"]["Item"]["Camara"]["@ColorThresholdHigh"] = rvalue["Item"][0]["Camara"][0]["ColorThresholdHigh"].asString();
            wvalue["SIP_XML"]["Item"]["Camara"]["@StriaeThresholdLow"] = rvalue["Item"][0]["Camara"][0]["StriaeThresholdLow"].asString();
            wvalue["SIP_XML"]["Item"]["Camara"]["@StriaeThresholdHigh"] = rvalue["Item"][0]["Camara"][0]["StriaeThresholdHigh"].asString();

            wvalue["SIP_XML"]["Item"]["Camara"]["@BrightThresholdLow"] = rvalue["Item"][0]["Camara"][0]["BrightThresholdLow"].asString();
            wvalue["SIP_XML"]["Item"]["Camara"]["@BrightThresholdHigh"] = rvalue["Item"][0]["Camara"][0]["BrightThresholdHigh"].asString();
            wvalue["SIP_XML"]["Item"]["Camara"]["@ShakeThresholdLow"] = rvalue["Item"][0]["Camara"][0]["ShakeThresholdLow"].asString();
            wvalue["SIP_XML"]["Item"]["Camara"]["@ShakeThresholdHigh"] = rvalue["Item"][0]["Camara"][0]["ShakeThresholdHigh"].asString();

            wvalue["SIP_XML"]["Item"]["Camara"]["@SignalLostThresholdLow"] = rvalue["Item"][0]["Camara"][0]["SignalLostThresholdLow"].asString();
            wvalue["SIP_XML"]["Item"]["Camara"]["@SignalLostThresholdHigh"] = rvalue["Item"][0]["Camara"][0]["SignalLostThresholdHigh"].asString();
            wvalue["SIP_XML"]["Item"]["Camara"]["@PicFreezeThresholdLow"] = rvalue["Item"][0]["Camara"][0]["PicFreezeThresholdLow"].asString();
            wvalue["SIP_XML"]["Item"]["Camara"]["@PicFreezeThresholdHigh"] = rvalue["Item"][0]["Camara"][0]["PicFreezeThresholdHigh"].asString();

            wvalue["SIP_XML"]["Item"]["Camara"]["@ClarityThresholdLow"] = rvalue["Item"][0]["Camara"][0]["ClarityThresholdLow"].asString();
            wvalue["SIP_XML"]["Item"]["Camara"]["@ClarityThresholdHigh"] = rvalue["Item"][0]["Camara"][0]["ClarityThresholdHigh"].asString();
        }
    }
    else if ("Camera_Snap" == req.strType)
    {
        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;

        if (!reader.parse((char*)req.stAlgReq.data, rvalue, false))
        {
            return -1;
        }

        wvalue["SIP_XML"]["@EventType"] = "Camera_Snap";
        wvalue["SIP_XML"]["Item"]["@Code"] = (char*)req.stAlgReq.ps8AddrCode;
        wvalue["SIP_XML"]["Item"]["@PicServer"] = rvalue["Item"][0]["PicServer"].asString();
        wvalue["SIP_XML"]["Item"]["@SnapType"] = rvalue["Item"][0]["SnapType"].asString();
        wvalue["SIP_XML"]["Item"]["@Range"] = rvalue["Item"][0]["Range"].asString();
        wvalue["SIP_XML"]["Item"]["@Interval"] = rvalue["Item"][0]["Interval"].asString();
    }
    else if ("Request_Alg_Area" == req.strType)
    {
        wvalue["SIP_XML"]["@EventType"] = "Request_Alg_Area";
        wvalue["SIP_XML"]["Item"]["@Code"] = (char*)req.stAlgReq.ps8AddrCode;
        wvalue["SIP_XML"]["Item"]["@FromIndex"] = std::to_string(1);
        wvalue["SIP_XML"]["Item"]["@ToIndex"]   = std::to_string(1000);
        wvalue["SIP_XML"]["Item"]["@PrePoint"]  = std::to_string(1);
    }
    else if ("Request_Alg_Task" == req.strType)
    {		
        wvalue["SIP_XML"]["@EventType"] = "Request_Alg_Task";
        wvalue["SIP_XML"]["Item"]["@Code"] = (char*)req.stAlgReq.ps8AddrCode;
        wvalue["SIP_XML"]["Item"]["@FromIndex"] = std::to_string(1);
        wvalue["SIP_XML"]["Item"]["@ToIndex"]   = std::to_string(1000);
    }
	else if ("Request_Flow" == req.strType)
	{
		wvalue["SIP_XML"]["@EventType"] = "Request_Flow";
		wvalue["SIP_XML"]["Item"]["@Code"] = (char*)req.stFlowReq.ps8AddrCode;
		wvalue["SIP_XML"]["Item"]["@UserCode"] = (char*)req.stFlowReq.ps8UserAddrCode;
	}

    json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);

    return 0;
}

int
SipUA::parseXml(const string &toCode, const string &xml, SHL_Json::Value &json)
{
    string strJson;
    SHL_Json::Reader reader;

    xml2json(xml.c_str(), strJson);
    
    if (!(reader.parse(strJson, json, false)))
    {
        return -1;
    }

    return 0;
}

string &
SipUA::osip2str(osip_message_t *sip, string &str)
{
    str.clear();
    char  * cstr = NULL;
    size_t  size = 0;

    osip_message_to_str(sip, &cstr, &size);

    if (cstr)
    {
        str = cstr;
        osip_free(cstr);
    }

    return str;
}


void
SipUA::timerdriver()
{
    /* TODO:  <15-11-17, yourname> */

}

void
SipUA::eventloop()
{
    string str;
    while (m_eventThread.active())
    {
        eXosip_event_t *ev;

        if (!(ev = eXosip_event_wait(m_eXcontext, 0, 1)))
        {
            osip_usleep(10000);
            continue;
        }

        if (ev->request)
        {
            LOGD_SIP("REQUEST:\n%s", osip2str(ev->request, str).c_str());
        }

        if (ev->response)
        {
            LOGD_SIP("RESPONSE:\n%s", osip2str(ev->response, str).c_str());
        }

        m_eXevent = ev;

        eXAutolock locker(m_eXcontext);

        switch (ev->type)
        {
        case EXOSIP_MESSAGE_NEW:
        case EXOSIP_CALL_INVITE:
        case EXOSIP_CALL_CLOSED:
        case EXOSIP_IN_SUBSCRIPTION_NEW:
        case EXOSIP_SUBSCRIPTION_NOTIFY:
            if (ev->request)
            {
                on_request(ev->request);
            }
            break;

        case EXOSIP_CALL_ACK:
            //on_ack(ev->request);
            break;

        case EXOSIP_REGISTRATION_SUCCESS:
        case EXOSIP_REGISTRATION_FAILURE:
        case EXOSIP_MESSAGE_ANSWERED:
        case EXOSIP_SUBSCRIPTION_ANSWERED:
        case EXOSIP_NOTIFICATION_ANSWERED:
            if (ev->response)
            {
                on_response(ev->response);
            }
            break;

        case EXOSIP_CALL_PROCEEDING:
            //OnCallProceeding(ev->response);
            break;

        case EXOSIP_CALL_ANSWERED:
            on_response(ev->response);
            break;

        case EXOSIP_CALL_MESSAGE_ANSWERED:
            on_bye_response(ev->response);
            break;

        default:
            LOGI_MSV("receieved unknown eXosip event (type, did, cid) = (%d, %d, %d)", ev->type, ev->did, ev->cid);
            break;
        }

        eXosip_event_free(ev);
    }
}

void *
SipUA::threadProc(ThreadInfo *info)
{
    SipUA * ua = (SipUA *)(info->data);

    switch (info->id)
    {
    case T_EVENTLOOP:
        ua->eventloop();
        break;

    default:
        break;
    }

    return NULL;
}

eXAutolock::eXAutolock(eXosip_t *&eXosip): m_eXcontext(eXosip)
{
    eXosip_lock(m_eXcontext);
}

eXAutolock::~eXAutolock()
{
    eXosip_unlock(m_eXcontext);
}
