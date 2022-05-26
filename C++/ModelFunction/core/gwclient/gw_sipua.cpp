#include "gw_sipua.h"
#include "tls_error.h"
#include "j2x.h"
#include "osip2/osip_mt.h"
#include "eXosip2/eXosip.h"
#include "stk/stk_utility.h"
#include <string>

GWSipUA::GWSipUA(string bindIp, int port, char *addrcode)
    : SipUA(bindIp, port, addrcode)
{
    m_bFirstRegister = false;
    m_bRegisted = false;
    m_sipUrl = NULL;
    m_contact = NULL;
    m_bSubStatus = false;
    m_bSubAlarm  = false;
}


GWSipUA::~GWSipUA()
{
    stop();
}

int
GWSipUA::query_resource(const string &ton, const string &toh, int top, const string &code, pfnQueryResourceCallback fnCb, void *pUser)
{
    int fromidx = 1;
    int toidx = 1000;

    SIP_REQ_S stReq;
    stReq.strType = "Request_Resource";
	stReq.stResouceReq.ps8AddrCode = (int8_t*)code.c_str();
    stReq.stResouceReq.s32FromIndex = fromidx;
    stReq.stResouceReq.s32ToIndex = toidx;

    bool bFinished = false;
    int s32Ret = 1;

    while (!bFinished)
    {
        string strXml;
        makeXmlReq(stReq, strXml);

        string stReponse;
        s32Ret = message(ton, toh, top, strXml.c_str(), stReponse);
        if (s32Ret != 0)
        {
            return -1;
        }
        
        string content;
        xml2json(stReponse.c_str(), content);

        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;
        if (!reader.parse(content, rvalue, false))
        {
            return -1;
        }
        
        int realNum = atoi(rvalue["SIP_XML"]["SubList"]["@RealNum"].asString().c_str());
        if (toidx > realNum)
        {
            toidx = realNum;
        }

        int toIndex = atoi(rvalue["SIP_XML"]["SubList"]["@ToIndex"].asString().c_str());
        if (toIndex < toidx)
        {
            stReq.stResouceReq.s32FromIndex = toIndex + 1;
            stReq.stResouceReq.s32ToIndex = toidx;
        }
        else
        {
            bFinished = true;
            s32Ret = 1;
        }

        if (NULL != fnCb)
        {
            fnCb((char*)content.c_str(), content.size(), pUser);
        }
    }

    return s32Ret;
}

int 
GWSipUA::query_record(const string &ton, const string &toh, int top, const string &usercode,
                    const string &begin, const string &end, uint32_t type, pfnRecordCallback fnCb, void *pUser)
{
    int fromidx = 1;
    int toidx = 1000;

    SIP_REQ_S stReq;

    stReq.strType = "Request_History_Video";
    stReq.stRecordReq.ps8ChnAddrCode  = (int8_t*)ton.c_str();
    stReq.stRecordReq.ps8UserAddrCode = (int8_t*)usercode.c_str();
    stReq.stRecordReq.ps8Begin        = (int8_t*)begin.c_str();
    stReq.stRecordReq.ps8End          = (int8_t*)end.c_str();
    stReq.stRecordReq.u32RecType      = type;
    stReq.stRecordReq.s32FromIndex    = fromidx;
    stReq.stRecordReq.s32ToIndex      = toidx;

    bool bFinished = false;
    int  s32Ret = -1;
    while (!bFinished)
    {
        string strXml;
        makeXmlReq(stReq, strXml);

        string stReponse;
        s32Ret = message(ton, toh, top, strXml.c_str(), stReponse);
        if (s32Ret != 0)
        {
            return -1;
        }

        string content;
        xml2json(stReponse.c_str(), content);

        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;
        if (!reader.parse(content, rvalue, false))
        {
            return -1;
        }

        int realNum = atoi(rvalue["SIP_XML"]["SubList"]["@RealNum"].asString().c_str());
        if (toidx > realNum)
        {
            toidx = realNum;
        }

        int toIndex = atoi(rvalue["SIP_XML"]["SubList"]["@ToIndex"].asString().c_str());
        if (toIndex < toidx)
        {
            stReq.stRecordReq.s32FromIndex = toIndex + 1;
            stReq.stRecordReq.s32ToIndex = toidx;
        }
        else
        {
            bFinished = true;
            s32Ret = 1;
        }

        if (NULL != fnCb)
        {
            fnCb((char*)content.c_str(), content.size(), pUser);
        }
    }

    return s32Ret;
}

int 
GWSipUA::query_alarm(const string &ton, const string &toh, int top, const string &useraddrcode,
                const string &begin, const string &end, uint32_t type, int level, pfnAlarmCallback fnCb, void *pUser)
{
    int fromidx = 1;
    int toidx = 1000;

    SIP_REQ_S stReq;
    stReq.strType = "Request_History_Alarm";
    stReq.stAlarmLogReq.ps8ChnAddrCode  = (int8_t *)ton.c_str();
    stReq.stAlarmLogReq.ps8UserAddrCode = (int8_t *)useraddrcode.c_str();
    stReq.stAlarmLogReq.ps8Begin        = (int8_t *)begin.c_str();
    stReq.stAlarmLogReq.ps8End          = (int8_t *)end.c_str();
    stReq.stAlarmLogReq.u32AlarmType    = type;
    stReq.stAlarmLogReq.u32AlarmLevel   = level;
    stReq.stAlarmLogReq.s32FromIndex    = fromidx;
    stReq.stAlarmLogReq.s32ToIndex      = toidx;

    bool bFinished = false;
    int  s32Ret = -1;

    while (!bFinished)
    {
        string strXml;
        makeXmlReq(stReq, strXml);

        string stReponse;
        int s32Ret = message(ton, toh, top, strXml.c_str(), stReponse);
        if (s32Ret != 0)
        {
            return -1;
        }

        string content;
        xml2json(stReponse.c_str(), content);

        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;
        if (!reader.parse(content, rvalue, false))
        {
            return -1;
        }

        int realNum = atoi(rvalue["SIP_XML"]["SubList"]["@RealNum"].asString().c_str());
        if (toidx > realNum)
        {
            toidx = realNum;
        }

        int toIndex = atoi(rvalue["SIP_XML"]["SubList"]["@ToIndex"].asString().c_str());
        if (toIndex < toidx)
        {
            stReq.stAlarmLogReq.s32FromIndex = toIndex + 1;
            stReq.stAlarmLogReq.s32ToIndex = toidx;
        }
        else
        {
            bFinished = true;
            s32Ret = 1;
        }

        if (NULL != fnCb)
        {
            fnCb((char*)content.c_str(), content.size(), pUser);
        }
    }

    return s32Ret;
}

int
GWSipUA::ctrl_camera(const string &ton, const string &toh, int top, const string &code,
        int cmd, int param1, int param2, int param3)
{
    SIP_REQ_S stReq;
    stReq.strType = "Control_Camera";
    stReq.stPtzReq.ps8ChnAddrCode = (int8_t *)code.c_str();
    stReq.stPtzReq.u32Cmd    = cmd;
    stReq.stPtzReq.u32Param1 = param1;
    stReq.stPtzReq.u32Param2 = param2;
    stReq.stPtzReq.u32Param3 = param3;

    string strXml;
    makeXmlReq(stReq, strXml);

    string stReponse;
    //int s32Ret = message(ton, toh, top, strXml.c_str(), stReponse);
    int s32Ret = message(code, toh, top, strXml.c_str(), stReponse);
    if (s32Ret != 0)
    {
        return -1;
    }

    return 0;
}

int
GWSipUA::subscribe_event(const string &ton, const string &toh, int top, const string &code,
    int subsType, int alarmType, int expires)
{
    SIP_REQ_S stReq;
    string    strEvent;

    stReq.stSubscribeReq.ps8ChnAddrCode = (int8_t *)code.c_str();
    if (0 == subsType)
    {
        stReq.strType = "Subscribe_Alarm";
        stReq.stSubscribeReq.u32AlarmType = alarmType;
        strEvent = "alarm";
        if (0 == expires)
        {
            m_bSubAlarm = false;
        }
        else
        {
            m_bSubAlarm = true;
        }
    }
    else if (1 == subsType)
    {
        stReq.strType = "Subscribe_Status";
        strEvent = "presence";
        if (0 == expires)
        {
            m_bSubStatus = false;
        }
        else
        {
            m_bSubStatus = true;
        }
    }
	stReq.stSubscribeReq.u32Expires = expires;

    string strXml;
    makeXmlReq(stReq, strXml);
    string jresp;
	int s32Ret = 0;
	if (-1 == m_subsribeId)
	{
		s32Ret = subscribe(ton, toh, top, strEvent, expires, strXml, jresp);
	}
	else
	{
		s32Ret = refreshsubscribe(ton, toh, top, strEvent, expires, strXml, jresp);
	}

    if (!m_bSubAlarm && !m_bSubStatus)
    {
        eXosip_subscription_remove(m_eXcontext, m_subsribeId);
        m_subsribeId = -1;
    }
	 
    if (s32Ret < 0)
    {
        return -1;
    }

    return 0;
}


int
GWSipUA::on_notify_response(osip_message_t *sip)
{
    SHL_Json::Value stRsp;

    osip_call_id_t *call_id = osip_message_get_call_id(sip);
    string strCallID = osip_call_id_get_number(call_id);

    osip_uri_t *url = NULL;
    osip_to_t  *to = NULL;

    to = osip_message_get_to(sip);
    url = osip_to_get_url(to);

    string strToAddrCode = osip_uri_get_username(url);

    if (200 != sip->status_code)
    {
        goto endover;
    }
    else
    {
        osip_body_t *bodies;

        int iRet = osip_message_get_body(sip, 0, &bodies);
        if (iRet != -1 && bodies != NULL)
        {
            string strXml = bodies->body;

            parseXml(strToAddrCode, strXml, stRsp);
        }
    }

endover:
    stRsp["SIP_XML"]["@code"] = sip->status_code;
    m_cqueue.response(strCallID, stRsp);

    return 0;
}

int
GWSipUA::on_invite_response(osip_message_t *sip)
{
    SHL_Json::Value stRsp;
    unsigned short u16DestPort = 0;

    uint32_t u32DestVideoPort = 0;
    uint32_t u32DestAudioPort = 0;
    osip_call_id_t *call_id = osip_message_get_call_id(sip);
    string strCallID = osip_call_id_get_number(call_id);

    if (200 != sip->status_code)
    {
        goto EndOnCallAnswered;
    }

    sdp_message_t *sdp;
    sdp_media_t   *sdp_video;
    sdp_media_t   *sdp_audio;

    /* 内部动态生成，后面要自己释放 */
    sdp = eXosip_get_remote_sdp_from_tid(m_eXcontext, m_eXevent->tid);
	if (NULL == sdp)
	{
		eXosip_call_terminate_with_reason(m_eXcontext, m_eXevent->cid, m_eXevent->did, "sdp is empty!");
		return -1;
	}
    sdp_video = eXosip_get_video_media(sdp);
    if (sdp_video)
    {
        u32DestVideoPort = atoi(sdp_video->m_port);
    }

    sdp_audio = eXosip_get_audio_media(sdp);
    if (sdp_audio)
    {
        u32DestAudioPort = atoi(sdp_audio->m_port);
    }
    sdp_message_free(sdp);

    ack();

EndOnCallAnswered:
    stRsp["code"]     = sip->status_code;
    stRsp["callid"]   = strCallID;
    stRsp["cid"]      = m_eXevent->cid;
    stRsp["did"]      = m_eXevent->did;
    stRsp["videoport"] = u32DestVideoPort;
    stRsp["audioport"] = u32DestAudioPort;
    m_cqueue.response(strCallID, stRsp);

    return 0;
}

int
GWSipUA::on_message_response(osip_message_t *sip)
{
    SHL_Json::Value stRsp;

    osip_call_id_t *call_id = osip_message_get_call_id(sip);
    string strCallID = osip_call_id_get_number(call_id);

    osip_uri_t *url = NULL;
    osip_to_t  *to = NULL;

    to = osip_message_get_to(sip);
    url = osip_to_get_url(to);

    string strToAddrCode = osip_uri_get_username(url);

    if (200 != sip->status_code)
    {
        goto endover;
    }
    else
    {
        osip_body_t *bodies;

        int iRet = osip_message_get_body(sip, 0, &bodies);
        if (iRet != -1 && bodies != NULL)
        {
            string strXml = bodies->body;
            parseXml(strToAddrCode, strXml, stRsp);
        }
    }

endover:
    stRsp["SIP_XML"]["@code"] = sip->status_code;
    m_cqueue.response(strCallID, stRsp);

    return 0;
}


int
GWSipUA::on_subscribe_response(osip_message_t *sip)
{
    SHL_Json::Value stRsp;

    osip_call_id_t *call_id = osip_message_get_call_id(sip);
    string strCallID = osip_call_id_get_number(call_id);

    osip_uri_t *url = NULL;
    osip_to_t  *to = NULL;

    to = osip_message_get_to(sip);
    url = osip_to_get_url(to);

    string strToAddrCode = osip_uri_get_username(url);

    if (200 != sip->status_code)
    {
        goto EndOnCallAnswered;
    }
    else
    {
        osip_body_t *bodies;

        int iRet = osip_message_get_body(sip, 0, &bodies);
        if (iRet != -1 && bodies != NULL)
        {
            string strXml = bodies->body;

            parseXml(strToAddrCode, strXml, stRsp);
        }
    }

EndOnCallAnswered:
    stRsp["SIP_XML"]["@code"] = sip->status_code;
    m_cqueue.response(strCallID, stRsp);

    return 0;
}

int
GWSipUA::on_register(osip_message_t *sip)
{
    /* 获取域 */
    osip_from_t *from = NULL;
    osip_uri_t  *url = NULL;
    from = osip_message_get_from(sip);
    url = osip_from_get_url(from);

    /* 更新域 */
    if (m_sipUrl)
    {
        osip_uri_free(m_sipUrl);
        m_sipUrl = NULL;
    }
    osip_uri_clone(url, &m_sipUrl);

    if (m_contact)
    {
        osip_contact_free(m_contact);
        m_contact = NULL;
    }
    osip_contact_t *contact;
    osip_message_get_contact(sip, 0, &contact);
    osip_contact_clone(contact, &m_contact);

    string strAddrCode = osip_uri_get_username(m_sipUrl);
    string strHost = osip_uri_get_host(m_sipUrl);
    string strPort("");
    char *pPort = osip_uri_get_port(m_sipUrl);
    if (pPort != NULL)
    {
        strPort = pPort;
    }

    osip_authorization_t *dest = NULL;
    osip_message_get_authorization(sip, 0, &dest);
    if (!dest)
    {
        /* 国网B协议 */
        m_bFirstRegister = true;
        m_bRegisted = false;
    }
    else if (dest &&
        strcmp("Capability,", osip_authorization_get_auth_type(dest)) == 0)
    {
        /* 28181 */
        char *auth_type = osip_authorization_get_auth_type(dest);
        printf("auth_type = %s\n", auth_type);
        m_bFirstRegister = true;
    }
    else if (dest &&
        m_bRegisted &&                                                      /* 已经登录 */
        strcmp("Digest", osip_authorization_get_auth_type(dest)) == 0
        )
    {
        /* 刷新注册
        /* 密码验证，更新时间 */
        char s8Username[100] = { 0 };
        char s8Response[100] = { 0 };
        strncpy(s8Username, osip_authorization_get_username(dest), 100);
        strncpy(s8Response, osip_authorization_get_response(dest), 100);

        /* 去掉引号 */
        stk_clrquote(s8Username);
        stk_clrquote(s8Response);
        string strUsername = s8Username;
        string strResponse = s8Response;

        doAnswer(200);
        m_bRegisted = true;

        return 0;
    }

    if (m_bFirstRegister)
    {
        doAnswer(401);
        m_bFirstRegister = false;
    }
    else
    {
        /* 处理第二次注册请求 */

        /* 密码验证，更新时间 */
        char s8Username[100] = { 0 };
        char s8Response[100] = { 0 };
        strncpy(s8Username, osip_authorization_get_username(dest), 100);
        strncpy(s8Response, osip_authorization_get_response(dest), 100);

        /* 去掉引号 */
        stk_clrquote(s8Username);
        stk_clrquote(s8Response);
        string strUsername = s8Username;
        string strResponse = s8Response;

        doAnswer(200);
        m_bRegisted = true;
    }

    return 0;
}

int
GWSipUA::on_notify(osip_message_t *sip)
{
    string fromn = "";
    string fromh = "";
    string fromp = "";
    string response = "";
    int    code = 200;
    char * p = NULL;

    /* find 1st: contact, 2nd: from, 3rd: via */
    do
    {
        osip_contact_t * contact = NULL;
        osip_uri_t     * url = NULL;
        osip_message_get_contact(sip, 0, &contact);
        if (contact)
        {
            url = osip_from_get_url(contact);
            fromn = (p = osip_uri_get_username(url)) ? p : "";
            fromh = (p = osip_uri_get_host(url)) ? p : "";
            fromp = (p = osip_uri_get_port(url)) ? p : "";

            break;
        }

        osip_from_t *from = osip_message_get_from(sip);
        if (from)
        {
            url = osip_from_get_url(from);
            fromn = (p = osip_uri_get_username(url)) ? p : "";
            fromh = (p = osip_uri_get_host(url)) ? p : "";
            fromp = (p = osip_uri_get_port(url)) ? p : "";

            break;
        }

        osip_via_t * via = NULL;
        osip_message_get_via(sip, 0, &via);
        if (via)
        {
            fromh = (p = osip_via_get_host(via)) ? p : "";
            fromp = (p = osip_via_get_port(via)) ? p : "";
            break;
        }

        code = 404;
    } while (0);

    osip_body_t * bodies = NULL;
    if (!(-1 != osip_message_get_body(sip, 0, &bodies) && bodies))
    {
        //for subscribe notify resp
        code = 200;
    }
    else
    {
        string json;
        xml2json(bodies->body, json);
        if (m_pfnsubscribecb)
        {
            m_pfnsubscribecb(json.c_str(), json.size(), m_user);
        }
    }

    osip_message_t * answer = NULL;
    eXosip_message_build_answer(m_eXcontext, m_eXevent->tid, code, &answer);
    if ("" != response)
    {
        osip_message_set_content_type(answer, "application/xml");
        osip_message_set_body(answer, response.c_str(), response.size());
    }

    eXosip_message_send_answer(m_eXcontext, m_eXevent->tid, code, answer);

    return 0;
}

int 
GWSipUA::query_dev_ability(const string &ton, const string &toh, int top, const string &code, pfnDevAbilityCallback fnCb, void *pUser)
{
	SIP_REQ_S stReq;
	stReq.strType = "Alg_Ability_Query";
	stReq.stResouceReq.ps8AddrCode = (signed char*)ton.c_str();

	string strXml;
	makeXmlReq(stReq, strXml);

	string stReponse;
	int s32Ret = message(ton, toh, top, strXml.c_str(), stReponse);
	if (s32Ret != 0)
	{
		return -1;
	}

	string content;
	xml2json(stReponse.c_str(), content);
	if (NULL != fnCb)
	{
		fnCb((char*)content.c_str(), content.size(), pUser);
	}

	return 0;
}

int
GWSipUA::do_algorithm_task(const string &ton, const string &toh, int top, const string &code, const char *json, int size)
{
	SIP_REQ_S stReq;
	stReq.strType = "Alg_Task_Dispatch";
	stReq.stAlgReq.ps8AddrCode = (signed char*)ton.c_str();
	stReq.stAlgReq.data = (signed char*)json;

	string strXml;
	makeXmlReq(stReq, strXml);

	string stReponse;
	int s32Ret = message(ton, toh, top, strXml.c_str(), stReponse);
	if (s32Ret != 0)
	{
		return -1;
	}

	return 0;
}

int
GWSipUA::do_picture_snap(const string &ton, const string &toh, int top, const string &code, const char *json, int siz)
{
	SIP_REQ_S stReq;
	stReq.strType = "Camera_Snap";
	stReq.stAlgReq.ps8AddrCode = (signed char*)ton.c_str();
	stReq.stAlgReq.data = (signed char*)json;

	string strXml;
	makeXmlReq(stReq, strXml);

	string stReponse;
	int s32Ret = message(ton, toh, top, strXml.c_str(), stReponse);
	if (s32Ret != 0)
	{
		return -1;
	}
	return 0;
}

int 
GWSipUA::query_dev_rule(const string &ton, const string &toh, int top, const string &code, pfnDevRuleCallback fnCb, void *pUser)
{
	SIP_REQ_S stReq;
    stReq.strType = "Request_Alg_Area";
    stReq.stAlgReq.ps8AddrCode = (signed char*)ton.c_str();

	string strXml;
	makeXmlReq(stReq, strXml);

	string stReponse;
	int s32Ret = message(ton, toh, top, strXml.c_str(), stReponse);
	if (s32Ret != 0)
	{
		return -1;
	}

	string content;
	xml2json(stReponse.c_str(), content);
	if (NULL != fnCb)
	{
		fnCb((char*)content.c_str(), content.size(), pUser);
	}

	return 0;
}

int 
GWSipUA::query_all_task(const string &ton, const string &toh, int top, pfnAllTaskCallback fnCb, void *pUser)
{
    SIP_REQ_S stReq;
    stReq.strType = "Request_Alg_Task";
    stReq.stAlgReq.ps8AddrCode = (signed char*)ton.c_str();

    string strXml;
    makeXmlReq(stReq, strXml);

    string stReponse;
    int s32Ret = message(ton, toh, top, strXml.c_str(), stReponse);
    if (s32Ret != 0)
    {
        return -1;
    }

    string content;
    xml2json(stReponse.c_str(), content);
    if (NULL != fnCb)
    {
        fnCb((char*)content.c_str(), content.size(), pUser);
    }

    return 0;
}

int 
GWSipUA::query_flow(const string &ton, const string &toh, int top, const string &usercode, int &bandWidth, int &streamNum)
{
	SIP_REQ_S stReq;
	stReq.strType = "Request_Flow";
	stReq.stFlowReq.ps8AddrCode = (int8_t *)ton.c_str();
	stReq.stFlowReq.ps8UserAddrCode = (int8_t *)usercode.c_str();

	string strXml;
	makeXmlReq(stReq, strXml);

	string stReponse;
	int s32Ret = message(ton, toh, top, strXml.c_str(), stReponse);
	if (s32Ret != 0)
	{
		return -1;
	}

	string content;
	xml2json(stReponse.c_str(), content);

	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	if (!reader.parse(content, rvalue, false))
	{
		return -1;
	}

	bandWidth = atoi(rvalue["SIP_XML"]["Item"]["@Bandwidth"].asString().c_str());
	streamNum = atoi(rvalue["SIP_XML"]["Item"]["@StreamNum"].asString().c_str());

	return 0;
}