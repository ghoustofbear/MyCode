#include "sipua.h"
#include "tevqueue.h"
#include "tls_error.h"
#include "osip2/osip_mt.h"
#include "eXosip2/eXosip.h"
#include "j2x.h"
#include "gsessmgr.h"
#include "platform.h"

int SipUA::m_baseport = 5060;
SipUA::SipUA(int transport, string bindIp, int port)
    : Server(transport, bindIp, port)
{
    m_subscribeAlarmId = -1;
    m_subscribeStatusId = -1;
}

SipUA::~SipUA()
{
	stop();
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

    bool subflag = false;
    if (m_listenPort < 0)
    {
        m_listenPort = baseport();
        subflag = true;
    }

    if (eXosip_listen_addr(m_eXcontext, m_transport, NULL, m_listenPort, AF_INET, 0))
    {
        ERRE_MSV("eXosip_listen_addr failed!");

        eXosip_quit(m_eXcontext);
        osip_free(m_eXcontext);

        return -1;
    }

    char ip[36] = {0};
    eXosip_guess_localip(m_eXcontext, AF_INET, ip, sizeof(ip));
    m_bindIp = ip;

    int result = 0;

    m_eventThread.setEntry(threadProc, this, T_EVENTLOOP);
    result += m_eventThread.start();

    if (subflag)
    {
        return result;
    }

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
SipUA::subscribe(const string &fromn, const string &fromh, int formp, const string &ton, const string &toh, int top,
            const string &event, int expires, const string &xml, string &jresp, int &subscribeId)
{
    int result = 0;
    char from[100];
    char to[100];
    snprintf(from, 100, "<sip:%s@%s:%d>", fromn.c_str(), m_bindIp.c_str(), m_listenPort);
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
        subscribeId = eXosip_subscription_send_initial_request(m_eXcontext, subs);
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
SipUA::refreshsubscribe(const string &fromn, const string &fromh, int formp, const string &ton, const string &toh, int top,
        const string &event, int expires, const string &xml, string &jresp, int &subscribeId)
{
	int result = 0;
	char from[100];
	char to[100];
	snprintf(from, 100, "<sip:%s@%s:%d>", fromn.c_str(), m_bindIp.c_str(), m_listenPort);
	snprintf(to, 100, "<sip:%s@%s:%d>", ton.c_str(), toh.c_str(), top);

	osip_message_t  *subs = NULL;
	{
		eXAutolock locker(m_eXcontext);
        eXosip_subscription_build_refresh_request(m_eXcontext, subscribeId, &subs);
		if (NULL == subs)
		{
			return -1;
		}

		osip_message_set_content_type(subs, "application/xml");
		osip_message_set_body(subs, xml.c_str(), xml.length());

		char tmp[512] = { 0 };
		snprintf(tmp, 10, "%i", expires);
		osip_message_set_expires(subs, tmp);

		osip_message_set_header(subs, "Event", event.c_str());

        eXosip_subscription_send_refresh_request(m_eXcontext, subscribeId, subs);
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
SipUA::message(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top,
               const string &xml, string &xresp)
{
    int result = 0;

    char from[100];
    char to  [100];
    snprintf(from, 100, "<sip:%s@%s:%d>", fromn.c_str(), m_bindIp.c_str(), m_listenPort);
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
SipUA::invite(const string &fromn, const string &fromh, int formp, const string &ton, const string &toh, int top,
              const string &sdp, string &jresp)
{
    osip_message_t *invite;
    char from[100];
    char to[100];
    snprintf(from, 100, "<sip:%s@%s:%d>", fromn.c_str(), m_bindIp.c_str(), m_listenPort);
	snprintf(to, 100, "<sip:%s@%s:%d>", ton.c_str(), toh.c_str(), top);
    char  subject[100] = { 0 };
    snprintf(subject, 100, "%s:0-4-0,%s:1", ton.c_str(), fromn.c_str());
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

        LOGD_SIP(jresp.c_str());
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

    eXAutolock locker(m_eXcontext);
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
}

int
SipUA::notify(const string &fromn, const string &fromh, int formp, const string &ton, const string &toh, int top, const string &event, const string &xml, string &jresp)
{
    int s32Ret = 0;

    char from[100];
    char to[100];
    snprintf(from, 100, "<sip:%s@%s:%d>", fromn.c_str(), m_bindIp.c_str(), m_listenPort);
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
SipUA::push_resource(const string &fromn, const string &fromh, int formp, const string &ton, const string &toh, int top, const string &xml, string &xresp)
{
	int s32Ret = 0;

	char from[100];
	char to[100];
    snprintf(from, 100, "<sip:%s@%s:%d>", fromn.c_str(), fromh.c_str(), formp);
    snprintf(to, 100, "<sip:%s@%s:%d>", ton.c_str(), toh.c_str(), top);

	osip_message_t *request = NULL;
    string strCallID;
    {
        eXAutolock locker(m_eXcontext);
        s32Ret = eXosip_message_build_request(m_eXcontext, &request, "NOTIFY", to, from, NULL);
        if (!request)
        {
            return -1;
        }

        osip_list_remove(&request->contacts, 0);
        osip_message_set_contact(request, from);

        osip_message_set_content_type(request, "application/xml");
        osip_message_set_body(request, xml.c_str(), xml.length());

        s32Ret = eXosip_message_send_request(m_eXcontext, request);
        osip_call_id_t *call_id = osip_message_get_call_id (request);
        strCallID= osip_call_id_get_number(call_id);
    }

#if 1
    //CallEvent call;
    tevent<std::string, SHL_Json::Value> event;
    SHL_Json::Value resp;
    event.init(strCallID, resp);
    m_cqueue.add(event);
    if (!event.wait(5000))
    {
        s32Ret = - 1;
    }
    else
    {
        s32Ret = 0;
    }

    m_cqueue.clear(event);
#endif

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
SipUA::info(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, int did, const string &rtsp, string &jresp)
{
    int result = 0;

    char from[100];
    char to[100];
    snprintf(from, 100, "<sip:%s@%s:%d>", fromn.c_str(), fromh.c_str(), fromp);
    snprintf(to, 100, "<sip:%s@%s:%d>", ton.c_str(), toh.c_str(), top);

    osip_message_t *message = NULL;
    osip_cseq_t *cseq = NULL;
    {
        eXAutolock locker(m_eXcontext);
        eXosip_call_build_info(m_eXcontext, did, &message);
        if (message == NULL)
        {
            return -1;
        }

        //osip_message_set_contact(message, from);
        osip_message_set_content_type(message, "Application/MANSRTSP");
        osip_message_set_body(message, rtsp.c_str(), rtsp.size());
        eXosip_call_send_request(m_eXcontext, did, message);
        cseq = osip_message_get_cseq(message);
    }

#if 0
    string strCseq = cseq->number;
    osip_call_id_t *call_id = osip_message_get_call_id(message);
    string strCallID = osip_call_id_get_number(call_id);

    //CallEvent call;
    tevent<std::string, SHL_Json::Value> event;
    SHL_Json::Value resp;
    event.init(strCallID + strCseq, resp);
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

#endif

    return result;
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
SipUA::doAnswer(int u32Code)
{
    osip_message_t *answer  = NULL;

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

        eXosip_message_send_answer(m_eXcontext, m_eXevent->tid,  200, answer);
    }
    else if (401 == u32Code)
    {
        char *str_url;
        osip_uri_to_str(m_sipUrl, &str_url);

        char realm[24] = {0};
        char tmp[24]   = {0};
        strncpy(tmp, str_url + 4, 10);
        snprintf(realm, 24, "\"%s\"", tmp);
        osip_free(str_url);

        eXosip_message_build_answer(m_eXcontext, m_eXevent->tid, 401, &answer);

        osip_www_authenticate_t *www_auth;

        osip_www_authenticate_init (&www_auth);

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
SipUA::on_register_response(osip_message_t *sip)
{
	SHL_Json::Value resp;

    string cid = osip_call_id_get_number(osip_message_get_call_id(sip));

    resp["code"] = sip->status_code;
	/*if (200 == sip->status_code)
	{
	osip_header_t * header = NULL;
	osip_message_get_expires(sip, 0, &header);
	resp["expires"] = atoi(header->hvalue);
	}*/
    m_cqueue.response(cid, resp);

    return 0;
}

int        
SipUA::on_info_response(osip_message_t *sip)
{
#if 0
    SHL_Json::Value stRsp;
    unsigned short u16DestPort = 0;

    osip_call_id_t *call_id = osip_message_get_call_id(sip);
    string strCallID = osip_call_id_get_number(call_id);
    osip_cseq_t *cseq = osip_message_get_cseq(sip);
    string strCesq = cseq->number;


    if (200 == sip->status_code)
    {
        stRsp["code"] = sip->status_code;
        stRsp["info"] = "success";
    }
    else
    {
        stRsp["code"] = 404;
        stRsp["info"] = "failed";
    }

    m_cqueue.response(strCallID + strCesq, stRsp);
#endif
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
    else if (MSG_IS_INVITE(sip))
    {
        on_invite(sip);
    }
    else if (MSG_IS_BYE(sip))
    {
        on_bye(sip);
    }
    else if (MSG_IS_SUBSCRIBE(sip))
    {
        on_subscribe(sip);
    }
    else if (MSG_IS_MESSAGE(sip))
    {
        on_message(sip);
    }
    else if (MSG_IS_INFO(sip))
    {
        on_info(sip);
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
    else if (MSG_IS_RESPONSE_FOR(sip, "INFO"))
    {

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
    else if ("Request_DVResource" == req.strType)
    {
        wvalue["SIP_XML"]["@EventType"]         = "Request_DVResource";
        wvalue["SIP_XML"]["Item"]["@Code"]      = string((const char*)req.stResouceReq.ps8AddrCode);
        wvalue["SIP_XML"]["Item"]["@FromIndex"] = std::to_string(req.stResouceReq.s32FromIndex);
        wvalue["SIP_XML"]["Item"]["@ToIndex"]   = std::to_string(req.stResouceReq.s32ToIndex);
    }
    else if ("Request_History_Video" == req.strType)
    {
        wvalue["SIP_XML"]["@EventType"] = "Request_History_Video";
        wvalue["SIP_XML"]["Item"]["@Code"]      = string((const char*)req.stRecordReq.ps8ChnAddrCode);
        wvalue["SIP_XML"]["Item"]["@UserCode"]  = string((const char*)req.stRecordReq.ps8UserAddrCode);
        wvalue["SIP_XML"]["Item"]["@Type"]      = std::to_string(req.stRecordReq.u32RecType);
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
        wvalue["SIP_XML"]["Item"]["@Type"]      = std::to_string(req.stAlarmLogReq.u32AlarmType);
        wvalue["SIP_XML"]["Item"]["@BeginTime"] = string((const char*)req.stAlarmLogReq.ps8Begin);
        wvalue["SIP_XML"]["Item"]["@EndTime"]   = string((const char*)req.stAlarmLogReq.ps8End);
        wvalue["SIP_XML"]["Item"]["@Level"]     = std::to_string(req.stAlarmLogReq.u32AlarmLevel);
        wvalue["SIP_XML"]["Item"]["@FromIndex"] = std::to_string(req.stAlarmLogReq.s32FromIndex);
        wvalue["SIP_XML"]["Item"]["@ToIndex"]   = std::to_string(req.stAlarmLogReq.s32ToIndex);
    }
    else if ("Control_Camera" == req.strType)
    {
        wvalue["SIP_XML"]["@EventType"]            = "Control_Camera";
        wvalue["SIP_XML"]["Item"]["@Code"]         = string((const char*)req.stPtzReq.ps8ChnAddrCode);
        wvalue["SIP_XML"]["Item"]["@Command"]      = std::to_string(req.stPtzReq.u32Cmd);
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
    else if ("Notify_Alarm" == req.strType)
    {
        wvalue["SIP_XML"]["@EventType"]         = "Notify_Alarm";
        wvalue["SIP_XML"]["Item"]["@Code"]      = string((const char*)req.stEventRptReq.ps8FrontAddrCode);
        wvalue["SIP_XML"]["Item"]["@Name"]      = string((const char*)req.stEventRptReq.ps8Name);
        wvalue["SIP_XML"]["Item"]["@Type"]      = std::to_string(req.stEventRptReq.u32AlarmType);
        wvalue["SIP_XML"]["Item"]["@StartTime"] = string((const char*)req.stEventRptReq.ps8Begin);
    }
    else if ("Notify_Status" == req.strType)
    {
        wvalue["SIP_XML"]["@EventType"]      = "Notify_Status";
        wvalue["SIP_XML"]["Item"]["@Code"]   = string((const char*)req.stEventRptReq.ps8FrontAddrCode);
        wvalue["SIP_XML"]["Item"]["@Name"]   = string((const char*)req.stEventRptReq.ps8Name);
        wvalue["SIP_XML"]["Item"]["@Status"] = std::to_string(req.stEventRptReq.u32Status);
    }
    else if ("Notify_IVS" == req.strType)
    {

    }
    else if ("Notify_VI" == req.strType)
    {

    }
    else if ("Request_Flow" == req.strType)
    {
		wvalue["SIP_XML"]["@EventType"] = "Request_Flow";
		wvalue["SIP_XML"]["Item"]["@Code"] = (char*)req.stFlowReq.ps8AddrCode;
		wvalue["SIP_XML"]["Item"]["@UserCode"] = (char*)req.stFlowReq.ps8UserAddrCode;
    }
    else if ("Notify_AcessCarPlug" == req.strType)
    {

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

int
SipUA::baseport()
{
    return m_baseport++;
}

void
SipUA::timerdriver()
{
    /* TODO:  <15-11-17, yourname> */

    /////Server::timerdriver();
    
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
        case EXOSIP_CALL_MESSAGE_NEW:
            if (ev->request)
            {
                on_request(ev->request);
            }
            break;

        case EXOSIP_CALL_ACK:
            on_ack(ev->request);
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
