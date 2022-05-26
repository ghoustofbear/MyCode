#include "gw_sipua.h"
#include "system.h"
#include "platform.h"
#include "supermgr.h"
#include "superior.h"
#include "cfg_device.h"
#include "gsession.h"
#include "gsessmgr.h"
#include "tls_error.h"
#include "putree.h"
#include "pu.h"
#include "j2x.h"
#include "osip2/osip_mt.h"
#include "eXosip2/eXosip.h"
#include "gw_pu.h"
#include "gumgr.h"
#include "stk/stk_utility.h"
#include "ut_iva_pu.h"
#include "utiva/typedef.h"
#include "jiyuan_iva_pu.h"
#include "cfg_ivarel.h"
#include "cfg_camera_ivaability.h"
#include "cfg_iva_taskresources.h"

#define  GW_VIDEO_SDP_FORMAT \
    "v=0\r\n" \
    "o=- 0 0 IN IP4 %s\r\n" \
    "s=-\r\n" \
    "i=The SIP CALL\r\n" \
    "c=IN IP4 %s\r\n" \
    "t=0 0\r\n" \
    "m=video %d RTP/AVP 100\r\n" \
    "a=rtpmap:100 H264/90000\r\n" \
    "a=fmtp:100 CIF=1\r\n" \
    "a=fmtp:100 profile-level-id=420028;sprop-parameter-sets=Z0IAKOkBQHsg,aM44gA==\r\n" \
    "a=sendrecv\r\n\r\n"

#define GW_AUDIO_SDP_FORMAT \
    "v=0\r\n" \
    "o=- 0 0 IN IP4 %s\r\n" \
    "s=-\r\n" \
    "i=The SIP CALL\r\n" \
    "c=IN IP4 %s\r\n" \
    "t=0 0\r\n" \
    "m=audio %d RTP/AVP 8\r\n" \
    "a=rtpmap:8 PCMA/8000\r\n" \
    "a=sendrecv\r\n\r\n"


#define GW_AUDIOVIDEO_SDP_FORMAT \
    "v=0\r\n" \
    "o=- 0 0 IN IP4 %s\r\n" \
    "s=-\r\n" \
    "i=The SIP CALL\r\n" \
    "c=IN IP4 %s\r\n" \
    "t=0 0\r\n" \
    "m=audio %d RTP/AVP 8" \
    "a=rtpmap:8 PCMA/8000" \
    "m=video %d RTP/AVP 100\r\n" \
    "a=rtpmap:100 H264/90000\r\n" \
    "a=fmtp:100 profile-level-id=42e00a;sprop-parameter-sets=Z0KADJWgUH5A,aM4Ecg==\r\n" \
    "a=sendrecv\r\n\r\n"


#define PER_SEND_RESOURCE_NUM     (5)
#define PER_SEND_RECORD_NUM       (5)
#define PER_SEND_LOG_NUM          (20)

GWSipUA::GWSipUA(int transport, string bindIp, int port)
    : SipUA(transport, bindIp, port)
{
    m_protocol = P_GWB;
    m_bFirstRegister = false;
    m_bRegisted = false;
    m_sipUrl = NULL;
    m_contact = NULL;
}

GWSipUA::~GWSipUA()
{

}

int
GWSipUA::query_resource(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code,
    string &content, int jx, int fromidx, int toidx)
{
    fromidx = (fromidx < 1) ? 1 : fromidx;
    toidx = (toidx < fromidx) ? (fromidx + 1000) : toidx;

    SIP_REQ_S stReq;
    stReq.strType = "Request_Resource";
    stReq.stResouceReq.ps8AddrCode = (int8_t*)code.c_str();
    stReq.stResouceReq.s32FromIndex = fromidx;
    stReq.stResouceReq.s32ToIndex = toidx;

    bool bFinished = false;
    int s32Ret = -1;

    while (!bFinished)
    {
        string strXml;
        makeXmlReq(stReq, strXml);

        string stReponse;
        s32Ret = message(fromn, fromh, fromp, ton, toh, top,   strXml.c_str(), stReponse);
        if (s32Ret != 0)
        {
            return -1;
        }
        
        if (jx == 0)
        {   
            xml2json(stReponse.c_str(), content);
        }
        else
        {
            content = stReponse;
        }

        bFinished = true;        
    }

    return s32Ret;
}

int 
GWSipUA::query_record(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code,
        const string &begin, const string &end, uint32_t type, string &content, int jx, int fromidx, int toidx)
{
    fromidx = (fromidx < 1) ? 1 : fromidx;
    toidx = (toidx < fromidx) ? (fromidx + 1000) : toidx;

    SIP_REQ_S stReq;

    stReq.strType = "Request_History_Video";
    stReq.stRecordReq.ps8ChnAddrCode  = (int8_t*)ton.c_str();
    stReq.stRecordReq.ps8UserAddrCode = (int8_t*)code.c_str();
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
        s32Ret = message(fromn, fromh, fromp, fromh, toh, top, strXml.c_str(), stReponse);
        if (s32Ret != 0)
        {
            return -1;
        }

        if (jx == 0)
        {
            xml2json(stReponse.c_str(), content);
        }
        else
        {
            content = stReponse;
        }

        bFinished = true;
    }

    return s32Ret;
}

int 
GWSipUA::query_alarm(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code,
    const string &begin, const string &end, uint32_t type, int level, string &content, int jx, int fromidx, int toidx)
{
    SIP_REQ_S stReq;
    stReq.strType = "Request_History_Alarm";
    stReq.stAlarmLogReq.ps8ChnAddrCode  = (int8_t *)ton.c_str();
    stReq.stAlarmLogReq.ps8UserAddrCode = (int8_t *)code.c_str();
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
        s32Ret = message(fromn, fromh, fromp, fromh, toh, top, strXml.c_str(), stReponse);
        if (s32Ret != 0)
        {
            return -1;
        }

        if (jx == 0)
        {
            xml2json(stReponse.c_str(), content);
        }
        else
        {
            content = stReponse;
        }

        bFinished = true;
    }

    return s32Ret;
}

int
GWSipUA::ctrl_camera(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code,
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
    //int s32Ret = message(fromn, fromh, fromp, ton, toh, top, strXml.c_str(), stReponse);
    int s32Ret = message(fromn, fromh, fromp, code, toh, top, strXml.c_str(), stReponse);
    if (s32Ret != 0)
    {
        return -1;
    }

    return 0;
}

int
GWSipUA::subscribe_event(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code,
    int subsType, int expires, int alarmType)
{
    SIP_REQ_S stReq;
    string    strEvent;

    stReq.stSubscribeReq.ps8ChnAddrCode = (int8_t *)code.c_str();
    if (0 == subsType)
    {
        stReq.strType = "Subscribe_Alarm";
        stReq.stSubscribeReq.u32AlarmType = alarmType;
        strEvent = "alarm";

        stReq.stSubscribeReq.u32Expires = expires;

        string strXml;
        makeXmlReq(stReq, strXml);
        string jresp;
        int    s32Ret;

        if (-1 == m_subscribeAlarmId)
        {
            s32Ret = subscribe(fromn, fromh, fromp, ton, toh, top, strEvent, expires, strXml, jresp, m_subscribeAlarmId);
        }
        else
        {
            s32Ret = refreshsubscribe(fromn, fromh, fromp, ton, toh, top, strEvent, expires, strXml, jresp, m_subscribeAlarmId);
        }

        if (s32Ret < 0)
        {
            eXosip_insubscription_remove(m_eXcontext, m_subscribeAlarmId);
            m_subscribeAlarmId = -1;
            return -1;
        }

        if (0 == expires)
        {
            eXosip_insubscription_remove(m_eXcontext, m_subscribeAlarmId);
            m_subscribeAlarmId = -1;
        }
    }
    else if (1 == subsType)
    {
        stReq.strType = "Subscribe_Status";
        strEvent = "presence";

        stReq.stSubscribeReq.u32Expires = expires;

        string strXml;
        makeXmlReq(stReq, strXml);
        string jresp;
        int    s32Ret;

        if (-1 == m_subscribeStatusId)
        {
            s32Ret = subscribe(fromn, fromh, fromp, ton, toh, top, strEvent, expires, strXml, jresp, m_subscribeStatusId);
        }
        else
        {
            s32Ret = refreshsubscribe(fromn, fromh, fromp, ton, toh, top, strEvent, expires, strXml, jresp, m_subscribeStatusId);
        }

        if (s32Ret < 0)
        {
            eXosip_insubscription_remove(m_eXcontext, m_subscribeStatusId);
            m_subscribeStatusId = -1;
            return -1;
        }

        if (0 == expires)
        {
            eXosip_insubscription_remove(m_eXcontext, m_subscribeStatusId);
            m_subscribeStatusId = -1;
        }
    }

    return 0;
}

int 
GWSipUA::push_alarm(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code,
        const string &time, int32_t type, int did)
{

    return 0;
}

int
GWSipUA::push_status(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code,
        int status, int did)
{

    return 0;
}


int
GWSipUA::regist(const string &host, const string &suph, int supp, const string &pswd, int expires, int &rid, bool first)
{
    osip_message_t * reg = NULL;

    {
        string addrcode = systm->addrcode();

        eXAutolock locker(m_eXcontext);
        if (!first)
        {
            eXosip_clear_authentication_info(m_eXcontext);
            
            int reeee = eXosip_add_authentication_info(m_eXcontext, addrcode.c_str(), addrcode.c_str(), pswd.c_str(), "MD5", NULL);
            
            eXosip_register_build_register(m_eXcontext, rid, expires, &reg);
			if (0 == expires)
			{
				osip_message_set_header(reg, "Logout-Reason", "maintenance");
			}
        }
        else
        {
            string from = string("sip:") + addrcode + string("@") + suph + string(":") + std::to_string(supp);
            string proxy = string("sip:") + suph + string(":") + std::to_string(supp);

            char contact[100] = {0};
            snprintf(contact, 100, "<sip:%s@%s:%d>", addrcode.c_str(), host.c_str(), platform->sport());

            rid = eXosip_register_build_initial_register(m_eXcontext, from.c_str(), proxy.c_str(), contact, expires, &reg);
            if (rid < 0)
            {
                LOGE_MSV("eXosip_register_build_initial_register fail.");

                return -1;
            }
           // osip_message_set_authorization(reg, "Capability algorithm=MD5");
            LOGI_MSV("rid = %d", rid);            
        }

        osip_list_remove(&reg->contacts, 0);
        char contact[100] = {0};
        snprintf(contact, 100, "<sip:%s@%s:%d>", addrcode.c_str(), host.c_str(), platform->sport());
        osip_message_set_contact(reg, contact);

        if (eXosip_register_send_register(m_eXcontext, rid, reg) != 0)
        {
            eXosip_register_remove(m_eXcontext, rid);
            LOGE_MSV("eXosip_register_send_register failed");
            return -1;
        }
    }

    tevent<std::string, SHL_Json::Value> event;
    int result  = 0;

    do
    {
        string callid = osip_call_id_get_number(osip_message_get_call_id(reg));
        int    code   = -1;

        SHL_Json::Value resp;
        event.init(callid, resp);

        m_cqueue.add(event);
        if (!event.wait(5000))
        {
            result = -1;
            break;
        }

        code = resp["code"].asInt();
        if (first && 401 != code)
        {
            if (200 == code)
            {
                result = 0;
            }
            else
            {
                result = -1;
            }
            break;
        }

        if (!first && 200 != code)
        {
            result = -1;
            break;
        }
    }
    while (0);

    m_cqueue.clear(event);

    if (result < 0)
    {
        //eXAutolock locker(m_eXcontext);
        //eXosip_register_remove(m_eXcontext, rid);
    }

    return result;
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

    sdp = eXosip_get_remote_sdp_from_tid(m_eXcontext, m_eXevent->tid);
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
    osip_from_t *from = NULL;
    osip_uri_t  *url = NULL;
    from = osip_message_get_from(sip);
    url = osip_from_get_url(from);

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
        m_bFirstRegister = true;
        m_bRegisted = false;
    }
    else if (dest &&
        strcmp("Capability,", osip_authorization_get_auth_type(dest)) == 0)
    {
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


    do
    {
        osip_body_t * bodies = NULL;
        if (!(-1 != osip_message_get_body(sip, 0, &bodies) && bodies))
        {
            code = 200;
            break;
        }

        if (on_parse_request(sip, response, NULL) < 0)
        {
            code = 404;
            break;
        }
    } while (0);

    osip_message_t * answer = NULL;
    eXosip_message_build_answer(m_eXcontext, m_eXevent->tid, code, &answer);
    if ("" != response)
    {
        osip_message_set_content_type(answer, "application/xml");
        osip_message_set_body(answer, response.c_str(), response.size());
    }
    else
    {
        char s8Contact[1024] = {0};
        snprintf(s8Contact, 100, "<sip:%s@%s:%d>", platform->addrcode().c_str(), platform->ip().c_str(), platform->sport());
        if (NULL != answer)
        {
            osip_list_remove(&answer->contacts, 0);
            osip_message_set_contact(answer, s8Contact);
        }
    }

    eXosip_message_send_answer(m_eXcontext, m_eXevent->tid, code, answer);

    return 0;
}

int
GWSipUA::on_subscribe(osip_message_t *sip)
{
    string fromn    = "";
    string fromh    = "";
    string fromp    = "";
    string response = "";
    int    code     = 200;
    char * p        = NULL;

    /* find 1st: contact, 2nd: from, 3rd: via */
    do
    {
        osip_contact_t * contact = NULL;
        osip_uri_t     * url     = NULL;
        osip_message_get_contact(sip, 0, &contact);
        if (contact)
        {
            url   = osip_from_get_url(contact);
            fromn = (p = osip_uri_get_username(url)) ? p : "";
            fromh = (p = osip_uri_get_host(url))     ? p : "";
            fromp = (p = osip_uri_get_port(url))     ? p : "";

            break;
        }

        osip_from_t *from = osip_message_get_from(sip);
        if (from)
        {
            url   = osip_from_get_url(from);
            fromn = (p = osip_uri_get_username(url)) ? p : "";
            fromh = (p = osip_uri_get_host(url))     ? p : "";
            fromp = (p = osip_uri_get_port(url))     ? p : "";

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
    }
    while (0);

    do
    {
        Superior *super = superMgr->findSuperior(fromn);
        if (!super)
        {
            LOGW_SIP(getError());
            code = 404;
            break;
        }

        osip_body_t * bodies = NULL;
        if (!(-1 != osip_message_get_body(sip, 0, &bodies) && bodies))
        {
            code = 401;
            break;
        }

        if (on_parse_request(sip, response, super) < 0)
        {
            code = 401;
            break;
        }
    }
    while (0);

    return -1;
}

int
GWSipUA::on_message(osip_message_t *sip)
{
    string fromn    = "";
    string fromh    = "";
    string fromp    = "";
    string response = "";
    int    code     = 200;
    char * p        = NULL;

    /* find 1st: contact, 2nd: from, 3rd: via */
    do
    {
        osip_contact_t * contact = NULL;
        osip_uri_t     * url     = NULL;
        osip_message_get_contact(sip, 0, &contact);
        if (contact)
        {
            url   = osip_from_get_url(contact);
            fromn = (p = osip_uri_get_username(url)) ? p : "";
            fromh = (p = osip_uri_get_host(url))     ? p : "";
            fromp = (p = osip_uri_get_port(url))     ? p : "";

            break;
        }

        osip_from_t *from = osip_message_get_from(sip);
        if (from)
        {
            url   = osip_from_get_url(from);
            fromn = (p = osip_uri_get_username(url)) ? p : "";
            fromh = (p = osip_uri_get_host(url))     ? p : "";
            fromp = (p = osip_uri_get_port(url))     ? p : "";

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
    }
    while (0);


    do
    {
        Superior *super = superMgr->findSuperior(fromn);
        if (!super)
        {
            LOGW_SIP(getError());
            code = 404;
            break;
        }

        osip_body_t * bodies = NULL;
        if (!(-1 != osip_message_get_body(sip, 0, &bodies) && bodies))
        {
            code = 404;
            break;
        }

        if (on_parse_request(sip, response, super) < 0)
        {
            code = 404;
            break;
        }
        return 0;
    }
    while (0);
    cb_message_rsp(response, code, m_eXevent->tid, this);

    return 0;
}

void cb_message_rsp(string response, int code, int tid, void *userdata)
{
    GWSipUA *sipua = (GWSipUA*)userdata;
    eXosip_t *eXcontext = (eXosip_t*)(sipua->m_eXcontext);
    osip_message_t * answer = NULL;
    eXosip_message_build_answer(eXcontext, tid, code, &answer);
    if ("" != response)
    {
        osip_message_set_content_type(answer, "application/xml");
        osip_message_set_body(answer, response.c_str(), response.size());
    }

    char *s8Contact = NULL;

    eXosip_message_send_answer(eXcontext, tid, code, answer);
}

int
GWSipUA::on_invite(osip_message_t *sip)
{
    string fromn = "";
    string fromh = "";
    string fromp = "";
    int    code  = 200;
    char * p     = NULL;

    /* find 1st: contact, 2nd: from, 3rd: via */
    do
    {
        osip_contact_t * contact = NULL;
        osip_uri_t     * url     = NULL;
        osip_message_get_contact(sip, 0, &contact);
        if (contact)
        {
            url   = osip_from_get_url(contact);
            fromn = (p = osip_uri_get_username(url)) ? p : "";
            fromh = (p = osip_uri_get_host(url))     ? p : "";
            fromp = (p = osip_uri_get_port(url))     ? p : "";

            break;
        }

        osip_from_t *from = osip_message_get_from(sip);
        if (from)
        {
            url   = osip_from_get_url(from);
            fromn = (p = osip_uri_get_username(url)) ? p : "";
            fromh = (p = osip_uri_get_host(url))     ? p : "";
            fromp = (p = osip_uri_get_port(url))     ? p : "";

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
    }
    while (0);


    char srcsdp[1024]  = {0};
    char contact[512] = {0};
    do
    {
        osip_to_t * to = NULL;
        to  = osip_message_get_to(sip);
        if (!to)
        {
            LOGE_SIP("osip_message_get_to fail.");
            code = 401;
            break;
        }

        osip_uri_t * url = osip_to_get_url(to);
        string ton = (p = osip_uri_get_username(url)) ? p : "";
        string toh = (p = osip_uri_get_host(url))     ? p : "";
        string top = (p = osip_uri_get_port(url))     ? p : "";

		if (systm->addrcode() == ton)
		{
			LOGE_SIP(getError());
			code = 404;
			break;
		}

        Pu *pu = putree->findPu(ton);
        if (!pu)
        {
            code = 404;
            break;
        }

        if (!pu->validate(ton))
        {
            LOGE_SIP(getError());
            code = 404;
            break;
        }

        osip_body_t * bodies = NULL;
        if (!(-1 != osip_message_get_body(sip, 0, &bodies) && bodies))
        {
            LOGE_SIP("osip_message_get_body fail.");
            code = 401;
            break;
        }

        sdp_message_t *sdp       = NULL;
        sdp_media_t   *sdp_media = NULL;
		sdp_media_t   *sdp_audio = NULL;

        sdp       = eXosip_get_remote_sdp_from_tid(m_eXcontext, m_eXevent->tid);
        sdp_media = eXosip_get_video_media(sdp);     
		sdp_audio = eXosip_get_audio_media(sdp);

		if (!sdp_media && !sdp_audio)
		{
			LOGE_SIP("eXosip_get_video_media fail.");
			code = 401;
			break;
		}

		if (sdp_media && !sdp_audio)
		{
			/* 1:sub stream, 0:main stream, default:1 */
            int              vcmode = 0;
			int              pos = 0;
			sdp_attribute_t *attr = NULL;

			while (!osip_list_eol(&(sdp_media->a_attributes), pos))
			{
				attr = (sdp_attribute_t *)osip_list_get(&sdp_media->a_attributes, pos);

				if (0 == strncmp(attr->a_att_field, "videochannel", strlen("videochannel")))
				{
					if (NULL != attr->a_att_value)
					{
						vcmode = atoi(attr->a_att_value);
					}
					break;
				}

				pos++;
			}

			sdp_message_free(sdp);

            string vMode = ((0==vcmode) ? "mainstream" : "substream");
            string sessid = toh + string(":") + top + string(":") + ton + string(":") + vMode;
			int    srcport = gsessMgr->RTPSRCPort(sessid);

#if 0
            snprintf(srcsdp, sizeof(srcsdp), GW_VIDEO_SDP_FORMAT, super->ifnetip().c_str(), super->ifnetip().c_str(), srcport, vcmode);
            snprintf(contact, 100, "<sip:%s@%s:%d>", ton.c_str(), super->ifnetip().c_str(), m_listenPort);
#else
            snprintf(srcsdp, sizeof(srcsdp), GW_VIDEO_SDP_FORMAT, platform->ip().c_str(), platform->ip().c_str(), srcport);
            snprintf(contact, 100, "<sip:%s@%s:%d>", ton.c_str(), platform->ip().c_str(), m_listenPort);
#endif

			code = 200;
		}
		if (sdp_audio && !sdp_media)
		{
            int              acmode = 0;
            int              pos = 0;
            sdp_attribute_t *attr = NULL;

            while (!osip_list_eol(&(sdp_media->a_attributes), pos))
            {
                attr = (sdp_attribute_t *)osip_list_get(&sdp_media->a_attributes, pos);
                if (0 == strncmp(attr->a_att_field, "sendonly", strlen("sendonly")))
                {
                    acmode = 1;
                }
                else if (0 == strncmp(attr->a_att_field, "sendrecv", strlen("sendrecv")))
                {
                    acmode = 0;
                }
            }

			sdp_message_free(sdp);

            string aMode = ((0==acmode)? "dualtalk" : "broadcast");
            string sessid = toh + string(":") + top + string(":") + ton + string(":") + aMode;
			int    srcport = gsessMgr->RTPSRCPort(sessid);

#if 0
			snprintf(srcsdp, sizeof(srcsdp), GW_AUDIO_SDP_FORMAT, super->ifnetip().c_str(), super->ifnetip().c_str(), srcport);
			snprintf(contact, 100, "<sip:%s@%s:%d>", ton.c_str(), super->ifnetip().c_str(), m_listenPort);
#else
			snprintf(srcsdp, sizeof(srcsdp), GW_AUDIO_SDP_FORMAT, platform->ip().c_str(), platform->ip().c_str(), srcport);
			snprintf(contact, 100, "<sip:%s@%s:%d>", ton.c_str(), platform->ip().c_str(), m_listenPort);
#endif

			code = 200;
		}		
    }
    while (0);

    osip_message_t *answer = NULL;
    eXosip_call_build_answer(m_eXcontext, m_eXevent->tid, code, &answer);
    if (!answer)
    {
        LOGW_SIP("eXosip_call_build_answer fail!");
        return -1;
    }

    if (0 != strcmp(srcsdp, ""))
    {
        osip_message_set_content_type(answer, "application/SDP");
        osip_message_set_body(answer, srcsdp, strlen(srcsdp));
    }

    osip_list_remove(&answer->contacts, 0);
    osip_message_set_contact(answer, contact);

    eXosip_call_send_answer(m_eXcontext, m_eXevent->tid, code, answer);

    return 0;
}

int
GWSipUA::on_ack(osip_message_t *sip)
{
    string fromn = "";
    string fromh = "";
    string fromp = "";
    char * p     = NULL;

    /* find 1st: contact, 2nd: from, 3rd: via */
    do
    {
        osip_contact_t * contact = NULL;
        osip_uri_t     * url     = NULL;
        osip_message_get_contact(sip, 0, &contact);
        if (contact)
        {
            url   = osip_from_get_url(contact);
            fromn = (p = osip_uri_get_username(url)) ? p : "";
            fromh = (p = osip_uri_get_host(url))     ? p : "";
            fromp = (p = osip_uri_get_port(url))     ? p : "";

            break;
        }

        osip_from_t *from = osip_message_get_from(sip);
        if (from)
        {
            url   = osip_from_get_url(from);
            fromn = (p = osip_uri_get_username(url)) ? p : "";
            fromh = (p = osip_uri_get_host(url))     ? p : "";
            fromp = (p = osip_uri_get_port(url))     ? p : "";

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
    }
    while (0);

    do
    {
        osip_to_t * to = NULL;
        to  = osip_message_get_to(sip);
        if (!to)
        {
            LOGE_SIP("osip_message_get_to fail.");
            break;
        }

        osip_uri_t * url = osip_to_get_url(to);
        string ton = (p = osip_uri_get_username(url)) ? p : "";
        string toh = (p = osip_uri_get_host(url))     ? p : "";
        string top = (p = osip_uri_get_port(url))     ? p : "";

        sdp_message_t *sdp       = NULL;
        sdp_media_t   *sdp_media = NULL;
		sdp_media_t   *sdp_audio = NULL;

        sdp       = eXosip_get_remote_sdp_from_tid(m_eXcontext, m_eXevent->tid);
        sdp_media = eXosip_get_video_media(sdp);
		sdp_audio = eXosip_get_audio_media(sdp);

		if (!sdp_media && !sdp_audio)
		{
			LOGE_SIP("eXosip_get_video_media fail.");
			break;
		}

		if (sdp_media && !sdp_audio)
		{
			int destport = atoi(sdp_media->m_port);

			/* 1: sub stream, 0: main stream,default: 1 */
            int             vcmode = 0;
            int             pos = 0;
			sdp_attribute_t *attr = NULL;

			while (!osip_list_eol(&(sdp_media->a_attributes), pos))
			{
				attr = (sdp_attribute_t *)osip_list_get(&sdp_media->a_attributes, pos);

				if (0 == strncmp(attr->a_att_field, "videochannel", strlen("videochannel")))
				{
					if (NULL != attr->a_att_value)
					{
						vcmode = atoi(attr->a_att_value);
					}
					break;
				}

				pos++;
			}

			sdp_message_free(sdp);

			osip_call_id_t * call_id = osip_message_get_call_id(sip);
			string           callid  = (p = osip_call_id_get_number(call_id)) ? p : "";
            string           vMode   = ((0 == vcmode) ? "mainstream" : "substream");
            string           sessid  = toh + string(":") + top + string(":") + ton + string(":") + vMode;

			SHL_Json::StreamWriterBuilder writer;
			SHL_Json::Value               value;

			value["fromn"]    = fromn;
			value["fromh"]    = fromh;
			value["fromp"]    = atoi(fromp.c_str());
			value["ton"]      = ton;
			value["toh"]      = toh;
			value["top"]      = atoi(top.c_str());
			value["destip"]   = fromh;
			value["destport"] = destport;
            value["vcmode"]   = vMode;

			string json = SHL_Json::writeString(writer, value);
			gsessMgr->RTPStart(sessid, callid, json);
		}
		if (sdp_audio && !sdp_media)
		{
			int destport = atoi(sdp_audio->m_port);

            int              acmode = 0;
            int              pos = 0;
            sdp_attribute_t *attr = NULL;

            while (!osip_list_eol(&(sdp_media->a_attributes), pos))
            {
                attr = (sdp_attribute_t *)osip_list_get(&sdp_media->a_attributes, pos);
                if (0 == strncmp(attr->a_att_field, "sendonly", strlen("sendonly")))
                {
                    acmode = 1;
                }
                else if (0 == strncmp(attr->a_att_field, "sendrecv", strlen("sendrecv")))
                {
                    acmode = 0;
                }
            }
			sdp_message_free(sdp);

			osip_call_id_t * call_id = osip_message_get_call_id(sip);
			string           callid  = (p = osip_call_id_get_number(call_id)) ? p : "";
            string           aMode   = ((0 == acmode) ? "dualtalk" : "broadcast");
            string           sessid = toh + string(":") + top + string(":") + ton + string(":") + aMode;

			SHL_Json::StreamWriterBuilder writer;
			SHL_Json::Value               value;

			value["fromn"]    = fromn;
			value["fromh"]    = fromh;
			value["fromp"]    = atoi(fromp.c_str());
			value["ton"]      = ton;
			value["toh"]      = toh;
			value["top"]      = atoi(top.c_str());
			value["destip"]   = fromh;
			value["destport"] = destport;
            value["vcmode"]   = aMode;

			string json = SHL_Json::writeString(writer, value);
			gsessMgr->RTPStart(sessid, callid, json);
		}
    }
    while (0);

    string str;
    LOGW_SIP(osip2str(sip, str).c_str());

    return 0;
}

int
GWSipUA::on_bye(osip_message_t *sip)
{
    char *p = NULL;

    osip_call_id_t * call_id = osip_message_get_call_id(sip);
    string         callid    = (p = osip_call_id_get_number(call_id)) ? p : "";

    gsessMgr->RTPStop(callid);

    return 0;
}

int
GWSipUA::on_parse_request(osip_message_t *sip, string &xml, Superior *super)
{
    osip_body_t   * bodies = NULL;
    osip_header_t * header = NULL;
    osip_message_get_body(sip, 0, &bodies);
    osip_message_get_expires(sip, 0, &header);

    string json;
    xml2json(bodies->body, json);  

    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(json, rvalue, false))
    {
        LOGE_SIP("parse err: %s", json.c_str());
        return -1;
    }

    string eventType = rvalue["SIP_XML"]["@EventType"].asString();

    if ("Push_Resourse" != eventType)
    {
        //set message call back fun and  save tid
        if (0 != setPuMessageCB(json))
        {
            return -1;
        }
    }

    if ("Request_Resource" == eventType)
    {
        return on_resource(json, xml, super);
    }
    else if ("Request_History_Video" == eventType)
    {
        return on_history_video(json, xml, super);
    }
    else if ("Control_Camera" == eventType)
    {
        return on_ctrl_camera(json, xml, super);
    }
    else if ("Request_History_Alarm" == eventType)
    {
        return on_history_alarm(json, xml, super);
    }
    else if ("Subscribe_Alarm" == eventType)
    {
        return on_subscribe_alarm(json, xml, super, m_eXevent->did, atoi(header->hvalue));
    }
    else if ("Subscribe_Status" == eventType)
    {
        return on_subscribe_status(json, xml, super, m_eXevent->did, atoi(header->hvalue));
    }
    else if ("Request_Flow" == eventType)
    {
        return on_query_flow(json, xml, super);
    }
    else if ("Push_Resourse" == eventType)
    {
        return 0;
    }
    else if ("Alg_Ability_Query" == eventType)
    {
        return onQueryAlgAbility(json, xml, super);
    }
    else if("Alg_Task_Dispatch" == eventType)
    {
        return onTaskDispatch(json, xml, super);
    }
    else if("Request_Alg_Task" == eventType)
    {
        return onReqAllTask(json, xml, super);
    }
    else if("Request_Alg_Area" == eventType)
    {
        return onReqSignalTask(json, xml, super);
    }
    else if("Camera_Snap" == eventType)
    {
        return onCameraSnap(json, xml, super);
    }
    else if ("Notify_Alarm" == eventType)
    {
        if (superMgr)
        {
            return superMgr->push("alarm", json);
        }
    }
    else if ("Notify_Status" == eventType)
    {
        if (superMgr)
        {
            return superMgr->push("presence", json);
        }
    }

    return -1;
}

int
GWSipUA::on_parse_response(osip_message_t *sip, string &json, Superior *super)
{
    return -1;
}

int
GWSipUA::on_resource(const string &json, string &xml, Superior *super)
{
    if (super)
    {
        if (0 == super->on_resource(json, xml))
        {
            int code = 200;
            cb_message_rsp(xml, code, m_eXevent->tid, this);
            return 0;
        }
    }

    return -1;
}

int 
GWSipUA::on_push_resource(const string &json, string &xml, Superior *super)
{
    return 0;
}

int
GWSipUA::on_ctrl_camera(const string &json, string &xml, Superior *super)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(json, rvalue, false))
    {
        return -1;
    }

    string addrcode = rvalue["SIP_XML"]["Item"]["@Code"].asString();
	if (systm->addrcode() == addrcode)
	{
		return -1;
	}
    Pu *pPu = putree->findPu(addrcode);
    if (!pPu)
    {
        LOGE_SIP(getError());
        return -1;
    }

    int cmd = atoi(rvalue["SIP_XML"]["Item"]["@Command"].asString().c_str());
    int speed = atoi(rvalue["SIP_XML"]["Item"]["@CommandPara1"].asString().c_str());
    int preset = atoi(rvalue["SIP_XML"]["Item"]["@CommandPara2"].asString().c_str());

    if (Pu::PROTO_TYPE_GW_B == pPu->protocol()      ||
        Pu::PROTO_TYPE_GW_B_HIK == pPu->protocol()  ||
        Pu::PROTO_TYPE_GW_B_HS == pPu->protocol()   ||
        Pu::PROTO_TYPE_GW_B_UT == pPu->protocol()   ||
        Pu::PROTO_TYPE_GB28181 == pPu->protocol())
    {
        GwPu *pGwPu = (GwPu*)pPu;
        return pGwPu->ptzCtrl(cmd, addrcode, speed, preset, 0);
    }
    else
    {
        int s32PtzCmd = -1;
        int s32Speed = speed;
        int s32Stop = 0;

        switch (cmd)
        {
        case PTZ_CMD_0401:
            s32PtzCmd = Pu::PTZ_CMD_UP;
            s32Stop = 1;
            break;
        case PTZ_CMD_0402:
            s32PtzCmd = Pu::PTZ_CMD_UP;
            s32Stop = 0;
            break;
        case PTZ_CMD_0403:
            s32PtzCmd = Pu::PTZ_CMD_DOWN;
            s32Stop = 1;
            break;
        case PTZ_CMD_0404:
            s32PtzCmd = Pu::PTZ_CMD_DOWN;
            s32Stop = 0;
            break;
        case PTZ_CMD_0503:
            s32PtzCmd = Pu::PTZ_CMD_LEFT;
            s32Stop = 1;
            break;
        case PTZ_CMD_0504:
            s32PtzCmd = Pu::PTZ_CMD_LEFT;
            s32Stop = 0;
            break;
        case PTZ_CMD_0501:
            s32PtzCmd = Pu::PTZ_CMD_RIGHT;
            s32Stop = 1;
            break;
        case PTZ_CMD_0502:
            s32PtzCmd = Pu::PTZ_CMD_RIGHT;
            s32Stop = 0;
            break;
        case PTZ_CMD_0701:
            s32PtzCmd = Pu::PTZ_CMD_UPLEFT;
            s32Stop = 1;
            break;
        case PTZ_CMD_0702:
            s32PtzCmd = Pu::PTZ_CMD_UPLEFT;
            s32Stop = 0;
            break;
        case PTZ_CMD_0801:
            s32PtzCmd = Pu::PTZ_CMD_UPRIGHT;
            s32Stop = 1;
            break;
        case PTZ_CMD_0802:
            s32PtzCmd = Pu::PTZ_CMD_UPRIGHT;
            s32Stop = 0;
            break;
        case PTZ_CMD_0703:
            s32PtzCmd = Pu::PTZ_CMD_DOWNLEFT;
            s32Stop = 1;
            break;
        case PTZ_CMD_0704:
            s32PtzCmd = Pu::PTZ_CMD_DOWNLEFT;
            s32Stop = 0;
            break;
        case PTZ_CMD_0803:
            s32PtzCmd = Pu::PTZ_CMD_DOWNRIGHT;
            s32Stop = 1;
            break;
        case PTZ_CMD_0804:
            s32PtzCmd = Pu::PTZ_CMD_DOWNRIGHT;
            s32Stop = 0;
            break;
        case PTZ_CMD_0303:
            s32PtzCmd = Pu::PTZ_CMD_ZOOMIN;
            s32Stop = 1;
            break;
        case PTZ_CMD_0304:
            s32PtzCmd = Pu::PTZ_CMD_ZOOMIN;
            s32Stop = 0;
            break;
        case PTZ_CMD_0301:
            s32PtzCmd = Pu::PTZ_CMD_ZOOMOUT;
            s32Stop = 1;
            break;
        case PTZ_CMD_0302:
            s32PtzCmd = Pu::PTZ_CMD_ZOOMOUT;
            s32Stop = 0;
            break;
        case PTZ_CMD_0201:
            s32PtzCmd = Pu::PTZ_CMD_FOCUSNEAR;
            s32Stop = 1;
            break;
        case PTZ_CMD_0202:
            s32PtzCmd = Pu::PTZ_CMD_FOCUSNEAR;
            s32Stop = 0;
            break;
        case PTZ_CMD_0203:
            s32PtzCmd = Pu::PTZ_CMD_FOCUSFAR;
            s32Stop = 1;
            break;
        case PTZ_CMD_0204:
            s32PtzCmd = Pu::PTZ_CMD_FOCUSFAR;
            s32Stop = 0;
            break;
        case PTZ_CMD_0103:
            s32PtzCmd = Pu::PTZ_CMD_IRISOPEN;
            s32Stop = 1;
            break;
        case PTZ_CMD_0104:
            s32PtzCmd = Pu::PTZ_CMD_IRISOPEN;
            s32Stop = 0;
            break;
        case PTZ_CMD_0101:
            s32PtzCmd = Pu::PTZ_CMD_IRISCLOSE;
            s32Stop = 1;
            break;
        case PTZ_CMD_0102:
            s32PtzCmd = Pu::PTZ_CMD_IRISCLOSE;
            s32Stop = 0;
            break;
        case PTZ_CMD_0E01:
            s32PtzCmd = Pu::PTZ_CMD_AUTOSCAN;
            s32Stop = 1;
            break;
        case PTZ_CMD_0E02:
            s32PtzCmd = Pu::PTZ_CMD_AUTOSCAN;
            s32Stop = 0;
            break;
        case PTZ_CMD_1001:
            //s32PtzCmd = Pu::PTZ_CMD_CRUISE;
            s32Stop = 1;
            break;
        case PTZ_CMD_1002:
            //s32PtzCmd = Pu::PTZ_CMD_CRUISE;
            s32Stop = 0;
            break;
        case PTZ_CMD_0601:
            s32PtzCmd = Pu::PTZ_CMD_SETPRESET;
            break;
        case PTZ_CMD_0603:
            s32PtzCmd = Pu::PTZ_CMD_CLEARPRESET;
            break;
        case PTZ_CMD_0602:
            s32PtzCmd = Pu::PTZ_CMD_CALLPRESET;
            break;
        case PTZ_CMD_0A01:
            s32PtzCmd = Pu::PTZ_CMD_RAIN_ON;
            break;
        case PTZ_CMD_0A02:
            s32PtzCmd = Pu::PTZ_CMD_RAIN_OFF;
            break;
        case PTZ_CMD_3DXX:
            //s32PtzCmd = Pu::PTZ_CMD_3D_POSITION;
            //s32Speed = s32Param1;
            //s32Stop = s32Param2;
            break;
        case PTZ_CMD_1101:
            s32PtzCmd = Pu::PTZ_CMD_LOCKCTRL;
            break;
        case  PTZ_CMD_1102:
            s32PtzCmd = Pu::PTZ_CMD_UNLOCKCTRL;
            break;
        default:
            break;
        }

        if (0 == pPu->ptzCtrl(s32PtzCmd, addrcode, speed, preset, s32Stop))
        {
            int code = 200;
            cb_message_rsp(xml, code, m_eXevent->tid, this);
            return 0;
        }
        return -1;
    }
}

int
GWSipUA::on_history_video(const string &json, string &xml, Superior *super)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(json, rvalue, false))
    {
        return -1;
    }

    string addrcode  = rvalue["SIP_XML"]["Item"]["@Code"].asString();
	if (systm->addrcode() == addrcode)
	{
		return -1;
	}
    Pu *pu = putree->findPu(addrcode);
    if (!pu)
    {
        LOGE_SIP(getError());
        return -1;
    }

    string fromIndex = rvalue["SIP_XML"]["Item"]["@FromIndex"].asString();
    string toIndex = rvalue["SIP_XML"]["Item"]["@ToIndex"].asString();
    string begin = rvalue["SIP_XML"]["Item"]["@BeginTime"].asString();
    string end = rvalue["SIP_XML"]["Item"]["@EndTime"].asString();
    string type = rvalue["SIP_XML"]["Item"]["@Type"].asString();
    string usercode = rvalue["SIP_XML"]["Item"]["@UserCode"].asString();

    const char *sep = strchr(begin.c_str(), 'T');
    if (!sep)
    {
        LOGE_SIP("invalidate args, begin time: %s, end time: %s", begin.c_str(), end.c_str());
        return -1;
    }

    datetime_s b;
    datetime_s e;

    string date;
    date.assign(begin.c_str(), sep - begin.c_str());

    const char *p = sep + 1;
    sep = strchr(sep, 'Z');
    string time;
    time.assign(p, sep - p);

    sscanf(date.c_str(), "%d-%d-%d", &b.year, &b.month, &b.day);
    sscanf(time.c_str(), "%d:%d:%d", &b.hour, &b.minute, &b.second);

    sep = strchr(end.c_str(), 'T');
    date.assign(end.c_str(), sep - end.c_str());

    p = sep + 1;
    sep = strchr(sep, 'Z');
    time.assign(p, sep - p);

    sscanf(date.c_str(), "%d-%d-%d", &e.year, &e.month, &e.day);
    sscanf(time.c_str(), "%d:%d:%d", &e.hour, &e.minute, &e.second);

    string records;
    int    start = 0;
    int    number = 0;

    start = atoi(fromIndex.c_str()) - 1;
    number = atoi(toIndex.c_str()) - atoi(fromIndex.c_str()) + 1;

    if (start < 0 || number < 1)
    {
        LOGE_SIP("invalidate args, fromIndex: %s, toIndex: %s", fromIndex.c_str(), toIndex.c_str());
        return -1;
    }

    number = number > PER_SEND_RECORD_NUM ? PER_SEND_RECORD_NUM : number;

    SHL_Json::StreamWriterBuilder writer;
    SHL_Json::Value               wvalue;

    if ((Pu::PROTO_TYPE_GW_B == pu->protocol())     ||
        (Pu::PROTO_TYPE_GW_B_HS == pu->protocol())  ||
        (Pu::PROTO_TYPE_GW_B_HIK == pu->protocol()) ||
        (Pu::PROTO_TYPE_GW_B_UT == pu->protocol())  ||
        (Pu::PROTO_TYPE_GB28181 == pu->protocol()))
    {
        if (pu->queryRecord(addrcode, atoi(type.c_str()), b, e, atoi(fromIndex.c_str()), start + number, records, usercode) < 0)
        {
            LOGE_SIP(getError());
            return -1;
        }

        return 0;
    }
    else
    {
        int recordtype = Pu::RECORD_TYPE_ALL;
        uint64_t type_ = strtoll(type.c_str(), NULL, 10);
        if (type_ == 0x02)
        {
            recordtype = Pu::RECORD_TYPE_MD;
        }
        else if (type_ == 0x04)
        {
            recordtype = Pu::RECORD_TYPE_VCOVER;
        }
        else if (type_ == 0x100000)
        {
            recordtype = Pu::RECORD_TYPE_TIMER;
        }
        else if (type_ == 0XFFFFFFFF)
        {
            recordtype = Pu::RECORD_TYPE_ALL;
        }

		if (pu->queryRecord(Pu::GetChanNoByCode(addrcode), recordtype, b, e, atoi(fromIndex.c_str()), start + number, records) < 0)
		{
			LOGE_SIP(getError());
			return -1;
		}

        if (!reader.parse(records, rvalue, false))
        {
            LOGE_SIP("parse err: %s", records.c_str());
            return -1;
        }

        wvalue["SIP_XML"]["@EventType"] = "Response_History_Video";
        wvalue["SIP_XML"]["SubList"]["@Code"] = addrcode;
        wvalue["SIP_XML"]["SubList"]["@RealNum"] = std::to_string(rvalue["realNum"].asInt());
        wvalue["SIP_XML"]["SubList"]["@SubNum"] = std::to_string(rvalue["records"].size());
        wvalue["SIP_XML"]["SubList"]["@FromIndex"] = fromIndex;
        wvalue["SIP_XML"]["SubList"]["@ToIndex"] = std::to_string(start + number);

        for (uint32_t i = 0; i < rvalue["records"].size(); ++i)
        {
#if 0
            string url = string("rtsp://") + super->ifnetip() + string(":") + std::to_string(platform->rport()) + \
                string("@") + rvalue["records"][i]["fileurl"].asString();
#else
			string url = string("rtsp://") + platform->ip() + string(":") + std::to_string(platform->rport()) + \
				string("@") + rvalue["records"][i]["fileurl"].asString();
#endif

            wvalue["SIP_XML"]["SubList"]["Item"][i]["@FileUrl"] = url;
            wvalue["SIP_XML"]["SubList"]["Item"][i]["@FileName"] = rvalue["records"][i]["filename"].asString();
            wvalue["SIP_XML"]["SubList"]["Item"][i]["@BeginTime"] = rvalue["records"][i]["start"].asString();
            wvalue["SIP_XML"]["SubList"]["Item"][i]["@EndTime"] = rvalue["records"][i]["end"].asString();
            wvalue["SIP_XML"]["SubList"]["Item"][i]["@Size"] = std::to_string(rvalue["records"][i]["size"].asInt());
            wvalue["SIP_XML"]["SubList"]["Item"][i]["@DecoderTag"] = "100";
            wvalue["SIP_XML"]["SubList"]["Item"][i]["@Type"] = std::to_string(rvalue["records"][i]["type"].asInt());
        }
    }

    json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);

    int code = 200;
    cb_message_rsp(xml, code, m_eXevent->tid, this);

    return 0;
}

int
GWSipUA::on_history_alarm(const string &json, string &xml, Superior *super)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(json, rvalue, false))
    {
        return -1;
    }

    string addrcode = rvalue["SIP_XML"]["Item"]["@Code"].asString();
    //Pu *pu = putree->findPu(addrcode);
    //if (!pu)
    //{
    //    LOGE_SIP(getError());
    //    return -1;
    //}

    string fromIndex = rvalue["SIP_XML"]["Item"]["@FromIndex"].asString();
    string toIndex = rvalue["SIP_XML"]["Item"]["@ToIndex"].asString();
    string begin = rvalue["SIP_XML"]["Item"]["@BeginTime"].asString();
    string end = rvalue["SIP_XML"]["Item"]["@EndTime"].asString();
    string type = rvalue["SIP_XML"]["Item"]["@Type"].asString();
    string level = rvalue["SIP_XML"]["Item"]["@Level"].asString();
    string usercode = rvalue["SIP_XML"]["Item"]["@UserCode"].asString();

    const char *sep = strchr(begin.c_str(), 'T');
    if (!sep)
    {
        LOGE_SIP("invalidate args, begin time: %s, end time: %s", begin.c_str(), end.c_str());
        return -1;
    }

    datetime_s b;
    datetime_s e;

    string date;
    date.assign(begin.c_str(), sep - begin.c_str());

    const char *p = sep + 1;
    sep = strchr(sep, 'Z');
    string time;
    time.assign(p, sep - p);

    sscanf(date.c_str(), "%d-%d-%d", &b.year, &b.month, &b.day);
    sscanf(time.c_str(), "%d:%d:%d", &b.hour, &b.minute, &b.second);

    sep = strchr(end.c_str(), 'T');
    date.assign(end.c_str(), sep - end.c_str());

    p = sep + 1;
    sep = strchr(sep, 'Z');
    time.assign(p, sep - p);

    sscanf(date.c_str(), "%d-%d-%d", &e.year, &e.month, &e.day);
    sscanf(time.c_str(), "%d:%d:%d", &e.hour, &e.minute, &e.second);

    string records;
    int    start = 0;
    int    number = 0;
    int    realNum = 0;

    start = atoi(fromIndex.c_str()) - 1;
    number = atoi(toIndex.c_str()) - atoi(fromIndex.c_str()) + 1;

    if (start < 0 || number < 1)
    {
        LOGE_SIP("invalidate args, fromIndex: %s, toIndex: %s", fromIndex.c_str(), toIndex.c_str());
        return -1;
    }

    number = number > PER_SEND_RECORD_NUM ? PER_SEND_RECORD_NUM : number;

	if (systm->addrcode() == addrcode)
	{
		return -1;
	}

    Pu *pu = putree->findPu(addrcode);
    if (!pu)
    {
        LOGE_SIP(getError());
        return -1;
    }

    SHL_Json::StreamWriterBuilder writer;
    SHL_Json::Value               wvalue;

    if ((Pu::PROTO_TYPE_GW_B == pu->protocol())     ||
        (Pu::PROTO_TYPE_GW_B_HS == pu->protocol())  ||
        (Pu::PROTO_TYPE_GW_B_HIK == pu->protocol()) ||
        (Pu::PROTO_TYPE_GW_B_UT == pu->protocol())  ||
        (Pu::PROTO_TYPE_GB28181 == pu->protocol()))
    {
        pu->queryAlarm(addrcode, atoi(type.c_str()), atoi(level.c_str()), b, e, atoi(fromIndex.c_str()), start + number, records, usercode);
        return 0;
    }
    else
    {
        int realNum = 0;
		if (pu->queryAlarm(Pu::GetChanNoByCode(addrcode.c_str()), atoi(type.c_str()), atoi(level.c_str()), b, e, atoi(fromIndex.c_str()), start + number, records, realNum) < 0)
        {
            LOGE_SIP(getError());
            return -1;
        }

        if (!reader.parse(records, rvalue, false))
        {
            LOGE_SIP("parse err: %s", records.c_str());
            return -1;
        }

        wvalue["SIP_XML"]["@EventType"] = "Response_History_Alarm";
        wvalue["SIP_XML"]["SubList"]["@Code"] = addrcode;
        wvalue["SIP_XML"]["SubList"]["@RealNum"] = std::to_string(realNum);
        wvalue["SIP_XML"]["SubList"]["@SubNum"] = std::to_string(rvalue.size());
        wvalue["SIP_XML"]["SubList"]["@FromIndex"] = fromIndex;
        wvalue["SIP_XML"]["SubList"]["@ToIndex"] = std::to_string(start + number);

        for (uint32_t i = 0; i < rvalue.size(); ++i)
        {
			wvalue["SIP_XML"]["SubList"]["Item"][i]["@Code"] = addrcode;

			int year, month, day, hour, minute, second;
			sscanf(rvalue[i]["time"].asString().c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
			char time[128] = { 0 };
			sprintf(time, "%04d-%02d-%02dT%02d:%02d:%02dZ", year, month, day, hour, minute, second);

			wvalue["SIP_XML"]["SubList"]["Item"][i]["@BeginTime"] = time;
            wvalue["SIP_XML"]["SubList"]["Item"][i]["@Status"] = std::to_string(rvalue[i]["status"].asInt());

            int alarmtype = 0;
            if (TriggerListener::TR_DEV_VLOSS == rvalue[i]["type"].asInt())
            {
                alarmtype = 0x01;
            }
            else if (TriggerListener::TR_DEV_MD == rvalue[i]["type"].asInt())
            {
                alarmtype = 0x02;
            }
            else if (TriggerListener::TR_DEV_VCOVER == rvalue[i]["type"].asInt())
            {
                alarmtype = 0x04;
            }
            else if (TriggerListener::TR_DEV_STATUS == rvalue[i]["type"].asInt())
            {
                alarmtype = 0x10000;
            }
            wvalue["SIP_XML"]["SubList"]["Item"][i]["@Type"] = std::to_string(alarmtype);
        }

    }
    json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);

    int code = 200;
    cb_message_rsp(xml, code, m_eXevent->tid, this);
    return 0;
}

int
GWSipUA::on_query_flow(const string &json, string &xml, Superior *super)
{
    SHL_Json::StreamWriterBuilder writer;
    SHL_Json::Value               wvalue;

    uint32_t streamNumRG = 0;
	uint32_t bandWidthRG = 0;
	realGuMgr->bandWidth(streamNumRG, bandWidthRG);

	uint32_t streamNumRRG = 0;
	uint32_t bandWidthRRG = 0;
	recordGuMgr->bandWidth(streamNumRRG, bandWidthRRG);

    wvalue["SIP_XML"]["@EventType"] = "Response_Flow";
	wvalue["SIP_XML"]["Item"]["@Bandwidth"] = std::to_string((streamNumRG + streamNumRRG));
	wvalue["SIP_XML"]["Item"]["@StreamNum"] = std::to_string((bandWidthRG + bandWidthRRG));

    json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);

    return 0;
}

int
GWSipUA::onQueryAlgAbility(const string &json, string &xml, Superior *super)
{  
    SHL_Json::Reader reader;
    SHL_Json::Value  value;
    SHL_Json::Value  rvalue;
    SHL_Json::FastWriter writer;

    if (!reader.parse(json, value, false))
    {
        return -1;
    }

    string addrcode = value["SIP_XML"]["Item"]["@Code"].asString();
    char cond[128] = { 0 };
    string jsonData;
    //判断智能分析规则是否由流媒体本地存储
    if (super->useivalocal())
    {    
        sprintf(cond, "addrcode='%s';", addrcode.c_str());
        int ret = cameraIvaAbilityCfg->query(jsonData, cond);
        if (0 == ret)
        {                            
            if (!reader.parse(jsonData, rvalue, false))
            {
                return -1;
            }

            SHL_Json::Value result;
            result["SIP_XML"]["@EventType"] = "Alg_Ability_Response";
            int algCnt = rvalue.size();
            for (int index = 0; index < algCnt; ++index)
            {
                if (algCnt == 1)
                {
                    result["SIP_XML"]["Item"]["@Code"] = addrcode;
                    result["SIP_XML"]["Item"]["@Ability"] = std::to_string(rvalue[index]["ability"].asInt());
                    result["SIP_XML"]["Item"]["@SubAbility"] = std::to_string(rvalue[index]["subability"].asInt());
                }
                else if (algCnt > 1)
                {
                    result["SIP_XML"]["Item"][index]["@Code"] = addrcode;
                    result["SIP_XML"]["Item"][index]["@Ability"] = std::to_string(rvalue[index]["ability"].asInt());
                    result["SIP_XML"]["Item"][index]["@SubAbility"] = std::to_string(rvalue[index]["subability"].asInt());
                }
            }
            std::string data = writer.write(result);
            printf("%s", data.c_str());
            json2xml(data.c_str(), xml);
        }

        return 0;
    }
    else
    {
        sprintf(cond, "addrcode='%s';", addrcode.c_str());
        int ret = ivarelCfg->query(jsonData, cond);
        if (0 == ret)
        {
            if (!reader.parse(jsonData, rvalue, false))
            {
                return -1;
            }

            if (1 == rvalue.size())
            {
                Pu * pPu = putree->findPu(rvalue["iva_addrcode"].asString());
                if (pPu && Pu::DEV_TYPE_AI == pPu->type() &&
                    Pu::PROTO_TYPE_UT == pPu->protocol())
                {
                    UtIvaPu *utIvaPu = (UtIvaPu*)pPu;
                    return utIvaPu->queryAbility(addrcode, xml);
                }
            }     
        }    
    }

    return -1;
}

int GWSipUA::onTaskDispatch(const string &json, string &xml, Superior *super)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(json, rvalue, false))
    {
        return -1;
    }
   
    string addrcode = rvalue["SIP_XML"]["Item"]["@Code"].asString();

    //上级主站下发的配置数据存在流媒体数据表中
    if (super->useivalocal())
    {     
        SHL_Json::Value  result;
        string taskid = rvalue["SIP_XML"]["Item"]["@TaskId"].asString();
        string taskname = rvalue["SIP_XML"]["Item"]["@TaskName"].asString();
        int preset = 0;
        if (!rvalue["SIP_XML"]["Item"]["@PrePoint"].isNull())
        {
            preset = atoi(rvalue["SIP_XML"]["Item"]["@PrePoint"].asString().c_str());
        }
        int taskprj = 0;
        if (!rvalue["SIP_XML"]["Item"]["@TaskPrj"].isNull())
        {
            taskprj = atoi(rvalue["SIP_XML"]["Item"]["@TaskPrj"].asString().c_str());
        }
        int analysetype = atoi(rvalue["SIP_XML"]["Item"]["@AnalyseType"].asString().c_str());
        int analysesubtype = atoi(rvalue["SIP_XML"]["Item"]["@AnalyseSubType"].asString().c_str());
        int analyseexe = atoi(rvalue["SIP_XML"]["Item"]["@AnalyseExe"].asString().c_str());
        int areanum = atoi(rvalue["SIP_XML"]["Item"]["@AreaNum"].asString().c_str());

        result["taskid"]   = taskid;
        result["taskname"] = taskname;
        result["addrcode"] = addrcode;
        result["preset"]   = preset;
        result["taskprj"]  = taskprj;
        result["analysetype"] = analysetype;
        result["analysesubtype"] = analysesubtype;
        result["analyseexe"] = analyseexe;
        result["areanum"] = areanum;

        SHL_Json::Value  paramRet;
        SHL_Json::Value  paramVal;
        for (int index = 0; index < areanum; ++index)
        {
            if (1 == areanum)
            {
                paramVal["PointNum"] = rvalue["SIP_XML"]["Item"]["Area"]["@PointNum"];
                paramVal["Point1_X"] = rvalue["SIP_XML"]["Item"]["Area"]["@Point1_X"];
                paramVal["Point1_Y"] = rvalue["SIP_XML"]["Item"]["Area"]["@Point1_Y"];
                paramVal["Point2_X"] = rvalue["SIP_XML"]["Item"]["Area"]["@Point2_X"];
                paramVal["Point2_Y"] = rvalue["SIP_XML"]["Item"]["Area"]["@Point2_Y"];
            }
            else
            {
                paramVal["PointNum"] = rvalue["SIP_XML"]["Item"][index]["Area"]["@PointNum"];
                paramVal["Point1_X"] = rvalue["SIP_XML"]["Item"][index]["Area"]["@Point1_X"];
                paramVal["Point1_Y"] = rvalue["SIP_XML"]["Item"][index]["Area"]["@Point1_Y"];
                paramVal["Point2_X"] = rvalue["SIP_XML"]["Item"][index]["Area"]["@Point2_X"];
                paramVal["Point2_Y"] = rvalue["SIP_XML"]["Item"][index]["Area"]["@Point2_Y"];
            }

            paramRet.append(paramVal);
        }  

        SHL_Json::FastWriter writer;
        string paraData = writer.write(paramRet);
        result["param"] = paraData;
       
        std::string data = writer.write(result);

        char cond[128] = { 0 };
        sprintf(cond, "taskid='%s';", taskid.c_str());

        string queryData;
        int ret = ivaTaskResourcesCfg->query(queryData, cond);
        if (0 != ret)
        {
            return -1;
        }
        if ("" != queryData)
        {
            ivaTaskResourcesCfg->modify(data);
        }
        else
        {
            ivaTaskResourcesCfg->add(data);
        }
    }  

    char cond[128] = { 0 };
    string jsonData;
    sprintf(cond, "camera_addrcode='%s'", addrcode.c_str());
    int ret = ivarelCfg->query(jsonData, cond);
    if (0 == ret)
    {
        SHL_Json::Value  ivaVal;
        if (!reader.parse(jsonData, ivaVal, false))
        {
            return -1;
        }

        if (1 == ivaVal.size())
        {
            string ivaAddrcode = ivaVal[0]["iva_addrcode"].asString();
            Pu * pPu = putree->findPu(ivaAddrcode);

            if (pPu && Pu::DEV_TYPE_AI == pPu->type())
            {
                if (Pu::PROTO_TYPE_UT == pPu->protocol())
                {
                    UtIvaPu *utIvaPu = (UtIvaPu*)pPu;
                    return utIvaPu->taskDispatch(json, xml);
                }
                else if (Pu::PROTO_TYPE_JIYUAN == pPu->protocol())
                {
                    JiYuanIvaPu *utIvaPu = (JiYuanIvaPu*)pPu;
                    return utIvaPu->taskDispatch(json, xml);
                }
            }
        }
    }

    return 0;
}

int GWSipUA::onReqAllTask(const string &json, string &xml, Superior *super)
{
    char cond[128] = { 0 };
    SHL_Json::Reader reader;
    SHL_Json::Value  reqvalue;
    SHL_Json::Value  rvalue;
    SHL_Json::Value  value;
    SHL_Json::FastWriter writer;
    if (!reader.parse(json, reqvalue, false))
    {
        return -1;
    }

    string addrcode = reqvalue["SIP_XML"]["Task"]["@StationCode"].asString();
    int fromIndex = atoi(reqvalue["SIP_XML"]["Task"]["@FromIndex"].asString().c_str());
    int toIndex = atoi(reqvalue["SIP_XML"]["Task"]["@ToIndex"].asString().c_str());

    //hik testDemo fromIndex=0 and toIndex=0
    if (0 == fromIndex && 0 == toIndex)
    {
        fromIndex = 1;
        toIndex = 100;
    }

    int    start = 0;
    int    number = 0;
    int    realNum = 0;

    start = fromIndex - 1;
    number = toIndex - fromIndex + 1;

    number = number > PER_SEND_RECORD_NUM ? PER_SEND_RECORD_NUM : number;

    //判断是否由流媒体本地配置
    if (super->useivalocal())
    {
        string jsonData;
        string realData;
        int ret = ivaTaskResourcesCfg->query(jsonData);
        if (0 == ret)
        {
            if (!reader.parse(jsonData, value, false))
            {
                return -1;
            }

            sprintf(cond, "limit '%d', '%d';", start, number);
            ret = ivaTaskResourcesCfg->query(realData);
            if (0 != ret)
            {
                return -1;
            }
            if (!reader.parse(realData, rvalue, false))
            {
                return -1;
            }

            SHL_Json::Value result;
            result["SIP_XML"]["@EventType"] = "Response_Alg_Task";
            int realNum = value.size();
            int subNum  = rvalue.size();
            
            result["SIP_XML"]["Task"]["@StationCode"] = addrcode;
            result["SIP_XML"]["Task"]["@RealNum"]     = std::to_string(realNum);
            result["SIP_XML"]["Task"]["@SubNum"]      = std::to_string(subNum);
            result["SIP_XML"]["Task"]["@FromIndex"]   = std::to_string(fromIndex);
            result["SIP_XML"]["Task"]["@ToIndex"]     = std::to_string(start + number);

            for (int index = 0; index < subNum; ++index)
            {
                if (subNum == 1)
                {
                    result["SIP_XML"]["Task"]["Item"]["@TaskId"]      = rvalue[index]["taskid"].asString();
                    result["SIP_XML"]["Task"]["Item"]["@TaskName"]    = rvalue[index]["taskname"].asString();
                    result["SIP_XML"]["Task"]["Item"]["@Code"]        = rvalue[index]["addrcode"].asString();
                    result["SIP_XML"]["Task"]["Item"]["@PrePoint"]    = std::to_string(rvalue[index]["preset"].asInt());
                    result["SIP_XML"]["Task"]["Item"]["@TaskPrj"]     = std::to_string(rvalue[index]["taskprj"].asInt());
                    result["SIP_XML"]["Task"]["Item"]["@AnalyseType"] = std::to_string(rvalue[index]["analysetype"].asInt());
                    result["SIP_XML"]["Task"]["Item"]["@AnalyseSubType"] = std::to_string(rvalue[index]["analysesubtype"].asInt());;
                    result["SIP_XML"]["Task"]["Item"]["@AnalyseExe"]   = std::to_string(rvalue[index]["analyseexe"].asInt());
                    result["SIP_XML"]["Task"]["Item"]["@AreaNum"]      = std::to_string(rvalue[index]["areanum"].asInt());

                    string jsonArea = rvalue[index]["param"].asString();
                    SHL_Json::Value  areavalue;
                    if (!reader.parse(jsonArea, areavalue, false))
                    {
                        return -1;
                    }

                    int areasize = areavalue.size();
                    for (int j = 0; j < areasize; ++j)
                    {
                        if (1 == areasize)
                        {
                            result["SIP_XML"]["Task"]["Item"]["Area"]["@PointNum"] = areavalue[j]["PointNum"].asString();
                            result["SIP_XML"]["Task"]["Item"]["Area"]["@Point1_X"] = areavalue[j]["Point1_X"].asString();
                            result["SIP_XML"]["Task"]["Item"]["Area"]["@Point1_Y"] = areavalue[j]["Point1_Y"].asString();
                            result["SIP_XML"]["Task"]["Item"]["Area"]["@Point2_X"] = areavalue[j]["Point2_X"].asString();
                            result["SIP_XML"]["Task"]["Item"]["Area"]["@Point2_Y"] = areavalue[j]["Point2_Y"].asString();
                        }
                        else
                        {
                            result["SIP_XML"]["Task"]["Item"]["Area"][j]["@PointNum"] = areavalue[j]["PointNum"].asString();
                            result["SIP_XML"]["Task"]["Item"]["Area"][j]["@Point1_X"] = areavalue[j]["Point1_X"].asString();
                            result["SIP_XML"]["Task"]["Item"]["Area"][j]["@Point1_Y"] = areavalue[j]["Point1_Y"].asString();
                            result["SIP_XML"]["Task"]["Item"]["Area"][j]["@Point2_X"] = areavalue[j]["Point2_X"].asString();
                            result["SIP_XML"]["Task"]["Item"]["Area"][j]["@Point2_Y"] = areavalue[j]["Point2_Y"].asString();
                        }
                    }
                }
                else if (subNum > 1)
                {
                    result["SIP_XML"]["Task"]["Item"][index]["@TaskId"] = rvalue[index]["taskid"].asString();
                    result["SIP_XML"]["Task"]["Item"][index]["@TaskName"] = rvalue[index]["taskname"].asString();
                    result["SIP_XML"]["Task"]["Item"][index]["@Code"] = rvalue[index]["addrcode"].asString();
                    result["SIP_XML"]["Task"]["Item"][index]["@PrePoint"] = std::to_string(rvalue[index]["preset"].asInt());
                    result["SIP_XML"]["Task"]["Item"][index]["@TaskPrj"] = std::to_string(rvalue[index]["taskprj"].asInt());
                    result["SIP_XML"]["Task"]["Item"][index]["@AnalyseType"] = std::to_string(rvalue[index]["analysetype"].asInt());
                    result["SIP_XML"]["Task"]["Item"][index]["@AnalyseSubType"] = std::to_string(rvalue[index]["analysesubtype"].asInt());;
                    result["SIP_XML"]["Task"]["Item"][index]["@AnalyseExe"] = std::to_string(rvalue[index]["analyseexe"].asInt());
                    result["SIP_XML"]["Task"]["Item"][index]["@AreaNum"] = std::to_string(rvalue[index]["areanum"].asInt());

                    string jsonArea = rvalue[index]["param"].asString();
                    SHL_Json::Value  areavalue;
                    if (!reader.parse(jsonArea, areavalue, false))
                    {
                        return -1;
                    }

                    int areasize = areavalue.size();
                    for (int j = 0; j < areasize; ++j)
                    {
                        if (1 == areasize)
                        {
                            result["SIP_XML"]["Task"]["Item"][index]["Area"]["@PointNum"] = areavalue[j]["PointNum"].asString();
                            result["SIP_XML"]["Task"]["Item"][index]["Area"]["@Point1_X"] = areavalue[j]["Point1_X"].asString();
                            result["SIP_XML"]["Task"]["Item"][index]["Area"]["@Point1_Y"] = areavalue[j]["Point1_Y"].asString();
                            result["SIP_XML"]["Task"]["Item"][index]["Area"]["@Point2_X"] = areavalue[j]["Point2_X"].asString();
                            result["SIP_XML"]["Task"]["Item"][index]["Area"]["@Point2_Y"] = areavalue[j]["Point2_Y"].asString();
                        }
                        else
                        {
                            result["SIP_XML"]["Task"]["Item"][index]["Area"][j]["@PointNum"] = areavalue[j]["PointNum"].asString();
                            result["SIP_XML"]["Task"]["Item"][index]["Area"][j]["@Point1_X"] = areavalue[j]["Point1_X"].asString();
                            result["SIP_XML"]["Task"]["Item"][index]["Area"][j]["@Point1_Y"] = areavalue[j]["Point1_Y"].asString();
                            result["SIP_XML"]["Task"]["Item"][index]["Area"][j]["@Point2_X"] = areavalue[j]["Point2_X"].asString();
                            result["SIP_XML"]["Task"]["Item"][index]["Area"][j]["@Point2_Y"] = areavalue[j]["Point2_Y"].asString();
                        }
                    }
                }
            }
            std::string data = writer.write(result);
            printf("%s", data.c_str());
            json2xml(data.c_str(), xml);   
        }
    }
    else
    {
        if (utIvaPu)
        {
            return utIvaPu->reqAllTask(json, xml);
        }
    }  

    return 0;
}

int GWSipUA::onReqSignalTask(const string &json, string &xml, Superior *super)
{
    SHL_Json::Reader     reader;
    SHL_Json::Value      reqvalue;
    SHL_Json::Value      rvalue;
    SHL_Json::FastWriter writer;
    if (!reader.parse(json, reqvalue, false))
    {
        return -1;
    }

    string addrcode = reqvalue["SIP_XML"]["Item"]["@Code"].asString();

    char cond[128] = { 0 };
    string jsonData;
    string realData;

    int fromIndex = atoi(reqvalue["SIP_XML"]["Item"]["@FromIndex"].asString().c_str());
    int toIndex = atoi(reqvalue["SIP_XML"]["Item"]["@ToIndex"].asString().c_str());
    int preset    = 0;
    if (!reqvalue["SIP_XML"]["Item"]["@PrePoint"].isNull())
    {
        preset = atoi(reqvalue["SIP_XML"]["Item"]["@PrePoint"].asString().c_str());
    }

    int    ret     = 0;
    int    start   = 0;
    int    number  = 0;
    int    realNum = 0;
    int    subNum  = 0;
    int    areaNum = 0;

    //hik testDemo fromIndex=0 and toIndex=0
    if (0 == fromIndex && 0 == toIndex)
    {
        fromIndex = 1;
        toIndex = 100;
    }

    start  = fromIndex - 1;
    number = toIndex - fromIndex + 1;

    number = number > PER_SEND_RECORD_NUM ? PER_SEND_RECORD_NUM : number;

    if (super->useivalocal())
    {
        sprintf(cond, "addrcode='%s' and preset='%d';", addrcode.c_str(), preset);
        ret = ivaTaskResourcesCfg->query(jsonData, cond);
        if (0 != ret)
        {
            return -1;
        }

        SHL_Json::Value  rvalue;
        SHL_Json::Value  value;
        if (!reader.parse(jsonData, value, false))
        {
            return -1;
        }
        realNum = value.size();

        memset(cond, 0, sizeof(cond));
        sprintf(cond, "addrcode='%s' and preset='%d' limit '%d','%d';", addrcode.c_str(), preset, start, number);
        ret = ivaTaskResourcesCfg->query(realData, cond);
        if (0 != ret)
        {
            return -1;
        }

        if (!reader.parse(realData, rvalue, false))
        {
            return -1;
        }
        subNum = rvalue.size();

        SHL_Json::Value result;
        result["SIP_XML"]["@EventType"]         = "Response_Alg_Area";
        result["SIP_XML"]["Item"]["@Code"]      = addrcode;
        result["SIP_XML"]["Item"]["@RealNum"]   = std::to_string(realNum);
        result["SIP_XML"]["Item"]["@SubNum"]    = std::to_string(subNum);
        result["SIP_XML"]["Item"]["@FromIndex"] = std::to_string(fromIndex);
        result["SIP_XML"]["Item"]["@ToIndex"]   = std::to_string(start + number);

        for (int index = 0; index < subNum; ++index)
        {
            if (1 == subNum)
            {
                areaNum = atoi(rvalue[index]["areanum"].asString().c_str());
                string jsonArea = rvalue[index]["param"].asString();

                SHL_Json::Value  areavalue;
                if (!reader.parse(jsonArea, areavalue, false))
                {
                    return -1;
                }

                int areasize = areavalue.size();
                for (int j = 0; j < areasize; ++j)
                {
                    if (1 == areasize)
                    {
                        result["SIP_XML"]["Item"]["Area"]["@PointNum"] = areavalue[j]["PointNum"].asString();
                        result["SIP_XML"]["Item"]["Area"]["@Point1_X"] = areavalue[j]["Point1_X"].asString();
                        result["SIP_XML"]["Item"]["Area"]["@Point1_Y"] = areavalue[j]["Point1_Y"].asString();
                        result["SIP_XML"]["Item"]["Area"]["@Point2_X"] = areavalue[j]["Point2_X"].asString();
                        result["SIP_XML"]["Item"]["Area"]["@Point2_Y"] = areavalue[j]["Point2_Y"].asString();
                    }
                    else
                    {
                        result["SIP_XML"]["Item"]["Area"][j]["@PointNum"] = areavalue[j]["PointNum"].asString();
                        result["SIP_XML"]["Item"]["Area"][j]["@Point1_X"] = areavalue[j]["Point1_X"].asString();
                        result["SIP_XML"]["Item"]["Area"][j]["@Point1_Y"] = areavalue[j]["Point1_Y"].asString();
                        result["SIP_XML"]["Item"]["Area"][j]["@Point2_X"] = areavalue[j]["Point2_X"].asString();
                        result["SIP_XML"]["Item"]["Area"][j]["@Point2_Y"] = areavalue[j]["Point2_Y"].asString();
                    }
                }
            }
            else
            {
                int areaNumTemp = atoi(rvalue[index]["areanum"].asString().c_str());
                areaNum += areaNumTemp;

                string jsonArea = rvalue[index]["param"].asString();

                SHL_Json::Value  areavalue;
                if (!reader.parse(jsonArea, areavalue, false))
                {
                    return -1;
                }

                int areasize = areavalue.size();
                for (int j = 0; j < areasize; ++j)
                {
                    if (1 == areasize)
                    {
                        result["SIP_XML"]["Item"]["Area"][index]["@PointNum"] = areavalue[j]["PointNum"].asString();
                        result["SIP_XML"]["Item"]["Area"][index]["@Point1_X"] = areavalue[j]["Point1_X"].asString();
                        result["SIP_XML"]["Item"]["Area"][index]["@Point1_Y"] = areavalue[j]["Point1_Y"].asString();
                        result["SIP_XML"]["Item"]["Area"][index]["@Point2_X"] = areavalue[j]["Point2_X"].asString();
                        result["SIP_XML"]["Item"]["Area"][index]["@Point2_Y"] = areavalue[j]["Point2_Y"].asString();
                    }
                    else
                    {
                        result["SIP_XML"]["Item"][index]["Area"][j]["@PointNum"] = areavalue[j]["PointNum"].asString();
                        result["SIP_XML"]["Item"][index]["Area"][j]["@Point1_X"] = areavalue[j]["Point1_X"].asString();
                        result["SIP_XML"]["Item"][index]["Area"][j]["@Point1_Y"] = areavalue[j]["Point1_Y"].asString();
                        result["SIP_XML"]["Item"][index]["Area"][j]["@Point2_X"] = areavalue[j]["Point2_X"].asString();
                        result["SIP_XML"]["Item"][index]["Area"][j]["@Point2_Y"] = areavalue[j]["Point2_Y"].asString();
                    }
                }
            }
        }

        result["SIP_XML"]["Item"]["@AreaNum"] = std::to_string(areaNum);

        std::string data = writer.write(result);
        printf("%s", data.c_str());
        json2xml(data.c_str(), xml);
    }
    else
    {
        sprintf(cond, "addrcode='%s';", addrcode.c_str());
        int ret = ivarelCfg->query(jsonData, cond);
        if (0 == ret)
        {
            if (!reader.parse(jsonData, rvalue, false))
            {
                return -1;
            }

            if (1 == rvalue.size())
            {
                Pu * pPu = putree->findPu(rvalue["iva_addrcode"].asString());

                if (pPu && Pu::DEV_TYPE_AI == pPu->type())
                {
                    if (Pu::PROTO_TYPE_UT == pPu->protocol())
                    {
                        UtIvaPu *utIvaPu = (UtIvaPu*)pPu;
                        return utIvaPu->reqSignalTask(addrcode, xml);
                    }
                }
            }
        }
    }

    return 0;
}

int GWSipUA::onCameraSnap(const string &json, string &xml, Superior *super)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(json, rvalue, false))
    {
        return -1;
    }

    if (!utIvaPu)
    {
        return -1;
    }
    return utIvaPu->cameraSnap(json, xml);
}

int
GWSipUA::on_subscribe_alarm(const string &json, string &xml, Superior *super, int did, int expires)
{
    if (did <= 0 )
    {
        return -1;
    }
    if (super->m_subscribedid > 0 &&
        super->m_subscribedid != did)
    {
        eXosip_insubscription_remove(m_eXcontext, super->m_subscribedid);
    }

    super->m_subscribedid = did;

    int code = 200;
    do
    {
        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;
        if (!reader.parse(json, rvalue, false))
        {
            code = 404;
            break;
        }

		string addrcode = rvalue["SIP_XML"]["Item"]["@Code"].asString();
        uint32_t type = atol(rvalue["SIP_XML"]["Item"]["@Type"].asString().c_str());

        //for test
        if (systm->addrcode() != addrcode)
        {
            code = 404;
            break;
        }

        if (type < 0)
        {
            code = 404;
            break;
        }

        if (0 == expires)
        {
            super->m_alramtype = 0xff000000;
        }
        else
        {
            super->m_alramtype |= type;
        }
    } while (0);

    osip_message_t *answer = NULL;
    eXosip_insubscription_build_answer(m_eXcontext, m_eXevent->tid, code, &answer);
    if (!answer)
    {
        return -1;
    }
    eXosip_insubscription_send_answer(m_eXcontext, m_eXevent->tid, code, answer);

    if (200 == code)
    {
        osip_message_t *request = NULL;
        eXosip_insubscription_build_notify(m_eXcontext, m_eXevent->did, EXOSIP_SUBCRSTATE_ACTIVE, 0, &request);
        if (!request)
        {
            return -1;
        }

        eXosip_insubscription_send_request(m_eXcontext, m_eXevent->did, request);
    }

    return 0;
}

int
GWSipUA::on_subscribe_status(const string &json, string &xml, Superior *super, int did, int expires)
{
    if (did <= 0 )
    {
        return -1;
    }
    if (super->m_subscribedid > 0 &&
        super->m_subscribedid != did)
    {
        eXosip_insubscription_remove(m_eXcontext, super->m_subscribedid);
    }

    super->m_subscribedid = did;

    int code = 200;
    do
    {
        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;
        if (!reader.parse(json, rvalue, false))
        {
            code = 404;
            break;
        }

        //for test
        string addrcode = rvalue["SIP_XML"]["Item"]["@Code"].asString();
        if (systm->addrcode() != addrcode)
        {
            code = 404;
            break;
        }

        if (0 == expires)
        {
            super->m_statustype = 0;
        }
        else
        {
            super->m_statustype = 1;
        }
    } while (0);

    osip_message_t *answer = NULL;
    eXosip_insubscription_build_answer(m_eXcontext, m_eXevent->tid, code, &answer);
    if (!answer)
    {
        return -1;
    }
    eXosip_insubscription_send_answer(m_eXcontext, m_eXevent->tid, code, answer);

    if (200 == code)
    {
        osip_message_t *request = NULL;
        eXosip_insubscription_build_notify(m_eXcontext, m_eXevent->did, EXOSIP_SUBCRSTATE_ACTIVE, 0, &request);
        if (!request)
        {
            return -1;
        }

        eXosip_insubscription_send_request(m_eXcontext, m_eXevent->did, request);
    }

    return 0;
}

int
GWSipUA::push_alarm (const string &time, string &jalarms, int did, Superior *super)
{
    SHL_Json::Value  rvalue;
    SHL_Json::Reader reader;
    if (!reader.parse(jalarms, rvalue, false))
    {
        return -1;
    }

    int year, month, day, hour, minute, second;
    sscanf(time.c_str(), "%d-%d-%d %d:%d:%d", &year, &month, &day, &hour, &minute, &second);
    char alarmtime[128] = { 0 };
    sprintf(alarmtime, "%04d-%02d-%02dT%02d:%02d:%02dZ", year, month, day, hour, minute, second);

    int type = rvalue[0]["type"].asInt();
    if (type != TriggerListener::TR_DEV_STATUS)
    {
        if (0xff000000 == super->m_alramtype)
        {
            return 0;
        }

        SHL_Json::StreamWriterBuilder writer;
        SHL_Json::Value               wvalue;

        wvalue["SIP_XML"]["@EventType"] = "Notify_Alarm";

		int alarmtye = 0;
        for (uint32_t i = 0; i < rvalue.size(); ++i)
        {
            if ("" == rvalue[i]["gucode"].asString())
            {
                continue;
            }

            wvalue["SIP_XML"]["Item"][i]["@Code"] = rvalue[i]["gucode"].asString();
			wvalue["SIP_XML"]["Item"][i]["@Name"] = rvalue[i]["name"].asString();

			if (TriggerListener::TR_DEV_VLOSS == rvalue[i]["type"].asInt())
			{
				alarmtye = 0x01;
			}
			else if (TriggerListener::TR_DEV_HDD == rvalue[i]["type"].asInt())
			{
				alarmtye = 0x800;
			}
			else if (TriggerListener::TR_DEV_MD == rvalue[i]["type"].asInt())
			{
				alarmtye = 0x02;
			}
			else if (TriggerListener::TR_DEV_VCOVER == rvalue[i]["type"].asInt())
			{
				alarmtye = 0x04;
			}
			else if (TriggerListener::TR_DEV_IO == rvalue[i]["type"].asInt())
			{
				return true;
			}

            wvalue["SIP_XML"]["Item"][i]["@Type"] = to_string(alarmtye);
            wvalue["SIP_XML"]["Item"][i]["@StartTime"] = alarmtime;
        }

        string xml, jresp;
        json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);

        char contact[128] = { 0 };
#if 0
        snprintf(contact, 100, "<sip:%s@%s:%d>", platform->addrcode().c_str(), super->ifnetip().c_str(), platform->sport());
#else
		snprintf(contact, 100, "<sip:%s@%s:%d>", platform->addrcode().c_str(), platform->ip().c_str(), platform->sport());
#endif
        return sipUA->notify("alarm", did, xml, jresp, contact);
    }

    return 0;
}

int
GWSipUA::push_status(const string &code, string &jstatus, int did, Superior *super)
{
    SHL_Json::Value  rvalue;
    SHL_Json::Reader reader;
    if (!reader.parse(jstatus, rvalue, false))
    {
        return -1;
    }

    int type = rvalue[0]["type"].asInt();
    if (type == TriggerListener::TR_DEV_STATUS)
    {
        if (0 == super->m_statustype)
        {
            return 0;
        }

        SHL_Json::StreamWriterBuilder writer;
        SHL_Json::Value               wvalue;

        wvalue["SIP_XML"]["@EventType"] = "Notify_Status";
        for (uint32_t i = 0; i < rvalue.size(); ++i)
        {
			wvalue["SIP_XML"]["Item"][i]["@Code"] = rvalue[i]["code"].asString();
			wvalue["SIP_XML"]["Item"][i]["@Name"] = rvalue[i]["name"].asString();
            wvalue["SIP_XML"]["Item"][i]["@Status"] = to_string(rvalue[i]["status"].asInt());
        }

        string xml, jresp;
        json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);

        char contact[128] = { 0 };
#if 0
        snprintf(contact, 100, "<sip:%s@%s:%d>", platform->addrcode().c_str(), super->ifnetip().c_str(), platform->sport());
#else
		snprintf(contact, 100, "<sip:%s@%s:%d>", platform->addrcode().c_str(), platform->ip().c_str(), platform->sport());
#endif

        return sipUA->notify("presence", did, xml, jresp, contact);
    }

    return 0;
}

int
GWSipUA::push_ivs(const string &time, string &jivs, int did)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(jivs, rvalue, false))
    {
        return -1;
    }

    SHL_Json::StreamWriterBuilder writer;
    SHL_Json::Value               wvalue;

    wvalue["SIP_XML"]["@EventType"] = "Alg_Result_Push";
    SHL_Json::Value item = rvalue[0];
    wvalue["SIP_XML"]["Item"]["@TaskId"]                = item["TaskId"].asString();
    wvalue["SIP_XML"]["Item"]["@City"]                  = item["City"].asString();
    wvalue["SIP_XML"]["Item"]["@TransformerSubstation"] = item["TransformerSubstation"].asString();
    wvalue["SIP_XML"]["Item"]["@AnalyseResult"]         = item["AnalyseResult"].asString();
    wvalue["SIP_XML"]["Item"]["@AnalyseTime"]           = item["AnalyseTime"].asString();
    wvalue["SIP_XML"]["Item"]["@AlarmState"]            = item["AlarmState"].asString();
    wvalue["SIP_XML"]["Item"]["@AnalyseDesc"]           = item["AnalyseDesc"].asString();
    int superType = atoi(item["AnalyseType"].asString().c_str());
    if(superType==SUPER_TEMP_DIFF || superType==SUPER_TEMP_CHANGE)
    {
        if(!item["InfraredPic"].isNull())
            wvalue["SIP_XML"]["Item"]["@InfraredPic"]       = item["InfraredPic"].asString();
        if(!item["VisiblePic"].isNull())
            wvalue["SIP_XML"]["Item"]["@VisiblePic"] = item["VisiblePic"].asString();
    }
    else
    {
        if(!item["AnalysePictureUrl"].isNull())
            wvalue["SIP_XML"]["Item"]["@AnalysePictureUrl"] = item["AnalysePictureUrl"].asString();
    }
    string xml, jresp;
    json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);
    printf("AlarmMSG:%s",SHL_Json::writeString(writer, wvalue).c_str());
    return sipUA->notify("IVS", did, xml, jresp, "");
}

int
GWSipUA::push_vi(const string &time, string &jvi, int did)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(jvi, rvalue, false))
    {
        return -1;
    }

    SHL_Json::StreamWriterBuilder writer;
    SHL_Json::Value               wvalue;

    wvalue["SIP_XML"]["@EventType"] = "Notify_VI";

    for (uint32_t i = 0; i < rvalue.size(); ++i)
    {
        wvalue["SIP_XML"]["Item"][i]["@Code"] = rvalue[i]["code"];
        wvalue["SIP_XML"]["Item"][i]["@Name"] = rvalue[i]["name"];
        wvalue["SIP_XML"]["Item"][i]["@Type"] = to_string(rvalue[i]["type"].asInt());
        wvalue["SIP_XML"]["Item"][i]["@StartTime"] = time;
    }

    string xml, jresp;
    json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);

    return sipUA->notify("IVS", did, xml, jresp, "");
}

int                
GWSipUA::setPuMessageCB(string json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(json, rvalue, false))
    {
        LOGE_SIP("parse err: %s", json.c_str());
        return -1;
    }

    string addrcode = rvalue["SIP_XML"]["Item"]["@Code"].asString();
    if (systm->addrcode() == addrcode)
    {
        return 0;
    }
    Pu *pPu = putree->findPu(addrcode);
    if (!pPu)
    {
        LOGE_SIP(getError());
        return -1;
    }

    if (Pu::PROTO_TYPE_GW_B == pPu->protocol()      ||
        Pu::PROTO_TYPE_GW_B_HIK == pPu->protocol()  ||
        Pu::PROTO_TYPE_GW_B_HS == pPu->protocol()   ||
        Pu::PROTO_TYPE_GW_B_UT == pPu->protocol()   ||
        Pu::PROTO_TYPE_GB28181 == pPu->protocol())
    {
        GwPu *pGwPu = (GwPu*)pPu;
        return pGwPu->setMessageCallBack(m_eXevent->tid,cb_message_rsp,this);
    }
    return 0;
}
