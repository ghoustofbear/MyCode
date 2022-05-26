#include "hs_sipua.h"
#include "system.h"
#include "platform.h"
#include "supermgr.h"
#include "superior.h"
#include "cfg_device.h"
#include "gsession.h"
#include "gsessmgr.h"
#include "tls_error.h"
//#include "pumgr.h"
#include "putree.h"
#include "pu.h"
#include "j2x.h"
#include "osip2/osip_mt.h"
#include "eXosip2/eXosip.h"

#define  HS_VIDEO_SDP_FORMAT \
    "v=0\r\n" \
    "o=- 0 0 IN IP4 %s\r\n" \
    "s=-\r\n" \
    "i=The SIP CALL\r\n" \
    "c=IN IP4 %s\r\n" \
    "t=0 0\r\n" \
    "m=video %d RTP/AVP 100\r\n" \
    "a=rtpmap:100 H.264/90000\r\n" \
    "a=fmtp:100 profile-level-id=42e00a;sprop-parameter-sets=Z0KADJWgUH5A,aM4Ecg==\r\n" \
    "a=sendonly\r\n\r\n"

#define PER_SEND_RESOURCE_NUM     (5)
#define PER_SEND_RECORD_NUM       (5)
#define PER_SEND_LOG_NUM          (20)


HSSipUA::HSSipUA(int transport, string bindIp, int port)
    : SipUA(transport, bindIp, port)
{
    m_protocol = P_GWB_HS;
}

HSSipUA::~HSSipUA()
{

}

int
HSSipUA::regist(const string &host, const string &suph, int supp, const string &pswd, int expires, int &rid, bool first)
{
    osip_message_t * reg = NULL;

    {
        string addrcode = systm->addrcode();

        eXAutolock locker(m_eXcontext);
        if (!first)
        {
            eXosip_clear_authentication_info(m_eXcontext);
            eXosip_add_authentication_info(m_eXcontext, addrcode.c_str(), addrcode.c_str(), pswd.c_str(), "MD5", NULL);
            eXosip_register_build_register(m_eXcontext, rid, expires, &reg);
        }
        else
        {
            string from  = string("sip:") + addrcode + string("@") + host + string(":") + std::to_string(supp);
            string proxy = string("sip:") + suph + string(":") + std::to_string(supp);

            char contact[100] = {0};
            snprintf(contact, 100, "<sip:%s@%s:%d>", addrcode.c_str(), host.c_str(), platform->sport());

            rid = eXosip_register_build_initial_register(m_eXcontext, from.c_str(), proxy.c_str(), contact, expires, &reg);
            if (rid < 0)
            {
                LOGE_MSV("eXosip_register_build_initial_register fail.");

                return -1;
            }

            LOGI_MSV("rid = %d", rid);
        }

        /* osip_list_remove(&reg->contacts, 0); */
        /* char contact[100] = {0}; */
        /* snprintf(contact, 100, "<sip:%s@%s:%d>", addrcode.c_str(), host.c_str(), platform->sport()); */
        /* osip_message_set_contact(reg, contact); */

        if (eXosip_register_send_register(m_eXcontext, rid, reg) != 0)
        {
            eXosip_register_remove(m_eXcontext, rid);
            LOGE_MSV("eXosip_register_send_register failed");

            return -1;
        }
    }

	tevent<std::string, SHL_Json::Value> event;
    int result = 0;

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
        eXAutolock locker(m_eXcontext);
        eXosip_register_remove(m_eXcontext, rid);
    }

    return result;
}

int
HSSipUA::on_notify_response(osip_message_t *sip)
{
    return -1;
}

int
HSSipUA::on_invite_response(osip_message_t *sip)
{
    return -1;
}


int
HSSipUA::on_message_response(osip_message_t *sip)
{
    return -1;
}

int
HSSipUA::on_subscribe_response(osip_message_t *sip)
{
    return -1;
}

int
HSSipUA::on_register(osip_message_t *sip)
{
    return -1;
}

int
HSSipUA::on_notify(osip_message_t *sip)
{
    return -1;
}

int
HSSipUA::on_subscribe(osip_message_t *sip)
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
        /* NOTE: HS MESSAGE method has no contact field */
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
            /* NOTE: HS, The IP in the from field is not consistent with the via and contact field */
            url   = osip_from_get_url(from);
            fromn = (p = osip_uri_get_username(url)) ? p : "";
            fromh = (p = osip_uri_get_host(url))     ? p : "";
            fromp = (p = osip_uri_get_port(url))     ? p : "";

            /* break; */
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
#if 0
        Superior *super = superMgr->findSuperior(fromh, atoi(fromp.c_str()));
        if (!super)
        {
            LOGW_SIP(getError());
            code = 404;
            break;
        }
#endif

        osip_body_t * bodies = NULL;
        if (!(-1 != osip_message_get_body(sip, 0, &bodies) && bodies))
        {
            code = 401;
            break;
        }

        if (on_parse_request(sip, response, NULL) < 0)
        {
            code = 401;
            break;
        }
    }
    while (0);

    return -1;
}

int
HSSipUA::on_message(osip_message_t *sip)
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
        /* NOTE: HS MESSAGE method has no contact field */
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
            /* NOTE: HS, The IP in the from field is not consistent with the via and contact field */
            url   = osip_from_get_url(from);
            fromn = (p = osip_uri_get_username(url)) ? p : "";
            fromh = (p = osip_uri_get_host(url))     ? p : "";
            fromp = (p = osip_uri_get_port(url))     ? p : "";

            /* break; */
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
#if 0
        Superior *super = superMgr->findSuperior(fromh, atoi(fromp.c_str()));
        if (!super)
        {
            LOGW_SIP(getError());
            code = 404;
            break;
        }

        osip_to_t * to = NULL;
        to  = osip_message_get_to(sip);
        if (!to)
        {
            code = 401;
            break;
        }

        osip_uri_t * url = osip_to_get_url(to);
        string ton = osip_uri_get_username(url);
        string toh = osip_uri_get_host(url);
        string top = osip_uri_get_port(url);

        if (systm->addrcode() != ton)
        {
            code = 401;
            break;
        }
#endif

        osip_body_t * bodies = NULL;
        if (!(-1 != osip_message_get_body(sip, 0, &bodies) && bodies))
        {
            code = 401;
            break;
        }

        if (on_parse_request(sip, response, NULL) < 0)
        {
            code = 401;
            break;
        }
    }
    while (0);

    osip_message_t * answer = NULL;
    eXosip_message_build_answer(m_eXcontext, m_eXevent->tid, code, &answer);

    osip_message_set_content_type(answer, "application/xml");
    osip_message_set_body(answer, response.c_str(), response.size());

    string str;
    LOGD_SIP("ANSWER:\n%s", osip2str(answer, str).c_str());
    eXosip_message_send_answer(m_eXcontext, m_eXevent->tid,  code, answer);

    return 0;
}

int
HSSipUA::on_invite(osip_message_t *sip)
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
            /* NOTE: HS, The IP in the from field is not consistent with the via and contact field */
            url   = osip_from_get_url(from);
            fromn = (p = osip_uri_get_username(url)) ? p : "";
            fromh = (p = osip_uri_get_host(url))     ? p : "";
            fromp = (p = osip_uri_get_port(url))     ? p : "";

            /* break; */
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


    char srcsdp[500]  = {0};
    char contact[100] = {0};
    do
    {
#if 0
        Superior *super = superMgr->findSuperior(fromh, atoi(fromp.c_str()));
        if (!super)
        {
            LOGW_SIP(getError());
            code = 404;
            break;
            /* TODO:  <19-12-17, set default host and ifupIp> */
        }
#endif

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

        /* Pu *pu = puMgr->findPuByGu(ton); */
        Pu *pu = putree->findPu(ton);
        if (!pu)
        {
            LOGE_SIP(getError());
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

        sdp       = eXosip_get_remote_sdp_from_tid(m_eXcontext, m_eXevent->tid);
        sdp_media = eXosip_get_video_media(sdp);
        if (!sdp_media)
        {
            LOGE_SIP("eXosip_get_video_media fail.");
            code = 401;
            break;
        }

        /* 1:sub stream, 0:main stream, default:1 */
        int              vcmode = 1;
        int              pos    = 0;
        sdp_attribute_t *attr   = NULL;

        while (!osip_list_eol(&(sdp_media->a_attributes), pos))
        {
            attr = (sdp_attribute_t *) osip_list_get(&sdp_media->a_attributes, pos);

            if (0 == strncmp(attr->a_att_field, "videochannel", strlen("videochannel")))
            {
                if (NULL != attr->a_att_value)
                {
                    vcmode = atoi(attr->a_att_value);
                }
                break;
            }

            pos ++;
        }

        sdp_message_free(sdp);

        string sessid  = ton + string(":") + std::to_string(vcmode);
        int    srcport = gsessMgr->RTPSRCPort(sessid);

#if 0
        snprintf(srcsdp, sizeof(srcsdp), HS_VIDEO_SDP_FORMAT, super->ifnetip().c_str(), super->ifnetip().c_str(), srcport, vcmode);
        snprintf(contact, 100, "<sip:%s@%s:%d>", ton.c_str(), super->ifnetip().c_str(), platform->sport());
#else
		snprintf(srcsdp, sizeof(srcsdp), HS_VIDEO_SDP_FORMAT, platform->ip().c_str(), platform->ip().c_str(), srcport, vcmode);
		snprintf(contact, 100, "<sip:%s@%s:%d>", ton.c_str(), platform->ip().c_str(), platform->sport());
#endif

        code = 200;
    }
    while (0);

    osip_message_t *answer = NULL;
    eXosip_call_build_answer(m_eXcontext, m_eXevent->tid, code, &answer);
    if (!answer)
    {
        LOGW_SIP("eXosip_call_build_answer fail!");
        return -1;
    }

    osip_message_set_content_type(answer, "application/SDP");
    osip_message_set_body(answer, srcsdp, strlen(srcsdp));

    osip_list_remove(&answer->contacts, 0);
    osip_message_set_contact(answer, contact);

    eXosip_call_send_answer(m_eXcontext, m_eXevent->tid, code, answer);

    return 0;
}

int
HSSipUA::on_ack(osip_message_t *sip)
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
            /* NOTE: HS, The IP in the from field is not consistent with the via and contact field */
            url   = osip_from_get_url(from);
            fromn = (p = osip_uri_get_username(url)) ? p : "";
            fromh = (p = osip_uri_get_host(url))     ? p : "";
            fromp = (p = osip_uri_get_port(url))     ? p : "";

            /* break; */
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

        sdp       = eXosip_get_remote_sdp_from_tid(m_eXcontext, m_eXevent->tid);
        sdp_media = eXosip_get_video_media(sdp);
        if (!sdp_media)
        {
            LOGE_SIP("eXosip_get_video_media fail.");
            break;
        }

        int destport = atoi(sdp_media->m_port);

        /* 1: sub stream, 0: main stream,default: 1 */
        int             vcmode = 1;
        int             pos    = 0;
        sdp_attribute_t *attr  = NULL;

        while (!osip_list_eol(&(sdp_media->a_attributes), pos))
        {
            attr = (sdp_attribute_t *) osip_list_get(&sdp_media->a_attributes, pos);

            if (0 == strncmp(attr->a_att_field, "videochannel", strlen("videochannel")))
            {
                if (NULL != attr->a_att_value)
                {
                    vcmode = atoi(attr->a_att_value);
                }
                break;
            }

            pos ++;
        }

        sdp_message_free(sdp);

        osip_call_id_t * call_id = osip_message_get_call_id(sip);
        string           callid  = (p = osip_call_id_get_number(call_id)) ? p : "";
        string           sessid  = ton + string(":") + std::to_string(vcmode);

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
        value["vcmode"]   = vcmode;

		string json = SHL_Json::writeString(writer, value);
        gsessMgr->RTPStart(sessid, callid, json);
    }
    while (0);

    string str;
    LOGW_SIP(osip2str(sip, str).c_str());

    return 0;
}

int
HSSipUA::on_bye(osip_message_t *sip)
{
    char *p = NULL;

    osip_call_id_t * call_id = osip_message_get_call_id(sip);
    string           callid  = (p = osip_call_id_get_number(call_id)) ? p : "";

    gsessMgr->RTPStop(callid);

    return 0;
}

int
HSSipUA::on_parse_request(osip_message_t *sip, string &xml, Superior *super)
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

    if ("Request_Resource" == eventType)
    {
        return on_resource(json, xml);
    }
    else if ("Request_DVResource" == eventType)
    {
        return on_dv_resource(json, xml);
    }
    else if ("Request_VIResource" == eventType)
    {
        return on_vi_resource(json, xml);
    }
    else if ("Request_AXFResource" == eventType)
    {
        return on_axf_resource(json, xml);
    }
    else if ("Request_ESTResource" == eventType)
    {
        return on_est_resource(json, xml);
    }
    else if ("Request_History_Video" == eventType)
    {
        return on_history_video(json, xml, super);
    }
    else if ("Subscribe_Alarm" == eventType)
    {
        return on_subscribe_alarm(json, xml, super, m_eXevent->did, atoi(header->hvalue));
    }
    else if ("Subscribe_Status" == eventType)
    {
        return on_subscribe_status(json, xml, super, m_eXevent->did, atoi(header->hvalue));
    }
    else if ("Subscribe_IVS" == eventType)
    {
        return on_subscribe_ivs(json, xml, super, m_eXevent->did, atoi(header->hvalue));
    }
    else if ("Subscribe_VI" == eventType)
    {
        return on_subscribe_vi(json, xml, super, m_eXevent->did, atoi(header->hvalue));
    }

    return -1;
}

int
HSSipUA::on_parse_response(osip_message_t *sip, string &json, Superior *super)
{
    return -1;
}

int
HSSipUA::on_resource(const string &json, string &xml, Superior *super)
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

    char cond[256] = {0};
    /* snprintf(cond, 256, "parentCode='%s';", addrcode.c_str()); */
    snprintf(cond, 256, "parentCode='%s' and type!='100';", addrcode.c_str());
    int count = deviceCfg->count(cond);
    if (count <= 0)
    {
        LOGE_SIP(getError());
        return -1;
    }

    memset(cond, 0, sizeof(cond));
    number = number > PER_SEND_RESOURCE_NUM ? PER_SEND_RESOURCE_NUM : number;
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

    wvalue["SIP_XML"]["@EventType"]            = "Response_Resource";
    wvalue["SIP_XML"]["SubList"]["@Code"]      = addrcode;
    wvalue["SIP_XML"]["SubList"]["@RealNum"]   = std::to_string(count);
    wvalue["SIP_XML"]["SubList"]["@SubNum"]    = std::to_string(rvalue.size());
    wvalue["SIP_XML"]["SubList"]["@FromIndex"] = fromIndex;
    wvalue["SIP_XML"]["SubList"]["@ToIndex"]   = std::to_string(start + number);

    for (uint32_t i = 0; i < rvalue.size(); ++i)
    {
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@Code"]           = rvalue[i]["addrcode"];
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@Name"]           = rvalue[i]["name"];
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@Status"]         = "0";
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@DecoderTag"]     = "0";
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@Longitude"]      = "0";
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@Latitude"]       = "0";
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@AreaCode"]       = "0";
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@IP"]             = rvalue[i]["host"];
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@Port"]           = std::to_string(rvalue[i]["port"].asInt());
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@UserName"]       = rvalue[i]["username"];
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@Password"]       = rvalue[i]["password"];
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@InstallDescrip"] = "0";
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@YZWID"]          = "0";
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@YZWType"]        = "0";
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@VCHNo"]          = std::to_string(rvalue[i]["chnNo"].asInt());
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@DevID"]          = "0";
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@DevType"]        = "0";
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@SubNum"]         = std::to_string(rvalue[i]["chnNum"].asInt());
    }

	json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);

    return 0;
}

int
HSSipUA::on_dv_resource(const string &json, string &xml, Superior *super)
{
    return -1;
}

int
HSSipUA::on_vi_resource(const string &json, string &xml, Superior *super)
{
    return -1;
}


int
HSSipUA::on_axf_resource(const string &json, string &xml, Superior *super)
{
    return -1;
}

int
HSSipUA::on_est_resource(const string &json, string &xml, Superior *super)
{
    return -1;
}

int
HSSipUA::on_ctrl_camera(const string &json, string &xml, Superior *super)
{
    return -1;
}

int
HSSipUA::on_history_video(const string &json, string &xml, Superior *super)
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
    string begin     = rvalue["SIP_XML"]["Item"]["@BeginTime"].asString();
    string end       = rvalue["SIP_XML"]["Item"]["@EndTime"].asString();
    string type      = rvalue["SIP_XML"]["Item"]["@Type"].asString();

    int    start     = atoi(fromIndex.c_str()) - 1;
    int    number    = atoi(toIndex.c_str()) - atoi(fromIndex.c_str()) + 1;

    if (start < 0 || number < 1)
    {
        LOGE_SIP("invalidate args, fromIndex: %s, toIndex: %s", fromIndex.c_str(), toIndex.c_str());
        return -1;
    }

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
    /* Pu *pu = puMgr->findPuByGu(addrcode); */
    Pu *pu = putree->findPu(addrcode);
    if (!pu)
    {
        LOGE_SIP(getError());
        return -1;
    }

    number = number > PER_SEND_RECORD_NUM ? PER_SEND_RECORD_NUM : number;
	if (pu->queryRecord(Pu::GetChanNoByCode(addrcode), 0xFFFF, b, e, atoi(fromIndex.c_str()), start + number, records) < 0)
    {
        LOGE_SIP(getError());
        return -1;
    }

    if (!reader.parse(records, rvalue, false))
    {
        LOGE_SIP("parse err: %s", records.c_str());
        return -1;
    }

	SHL_Json::StreamWriterBuilder writer;
	SHL_Json::Value               wvalue;

    wvalue["SIP_XML"]["@EventType"]            = "Response_History_Video";
    wvalue["SIP_XML"]["SubList"]["@Code"]      = addrcode;
    wvalue["SIP_XML"]["SubList"]["@RealNum"]   = std::to_string(rvalue["realNum"].asInt());
    wvalue["SIP_XML"]["SubList"]["@SubNum"]    = std::to_string(rvalue["records"].size());
    wvalue["SIP_XML"]["SubList"]["@FromIndex"] = fromIndex;
    wvalue["SIP_XML"]["SubList"]["@ToIndex"]   = std::to_string(start + number);

    for (uint32_t i = 0; i < rvalue["records"].size(); ++i)
    {
#if 0
        string url = string("rtsp://") + super->ifnetip() + string(":") + std::to_string(platform->rport()) + \
                     string("@") +  rvalue["records"][i]["fileurl"].asString();
#else
		string url = string("rtsp://") + platform->ip() + string(":") + std::to_string(platform->rport()) + \
			string("@") + rvalue["records"][i]["fileurl"].asString();
#endif

        wvalue["SIP_XML"]["SubList"]["Item"][i]["@FileUrl"]    = url;
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@FileName"]   = rvalue["records"][i]["filename"];
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@BeginTime"]  = rvalue["records"][i]["start"];
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@EndTime"]    = rvalue["records"][i]["end"];
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@Size"]       = std::to_string(rvalue["records"][i]["size"].asInt());
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@DecoderTag"] = "0";
        wvalue["SIP_XML"]["SubList"]["Item"][i]["@Type"]       = std::to_string(rvalue["records"][i]["type"].asInt());
    }

	json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);

    return 0;
}

int
HSSipUA::on_subscribe_alarm(const string &json, string &xml, Superior *super, int did, int expires)
{
	if (super->m_subscribedid > 0)
    {
    //    eXAutolock locker(m_eXcontext);
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

        uint32_t type = atol(rvalue["SIP_XML"]["Item"]["@Type"].asCString());
        /* validate(type) */
        if (type < 0)
        {
            code = 404;
            break;
        }

        super->m_alramtype = type;
    }
    while (0);

    osip_message_t *answer  = NULL;
    eXosip_insubscription_build_answer(m_eXcontext, m_eXevent->tid, code, &answer);
    if (!answer)
    {
        return -1;
    }
    eXosip_insubscription_send_answer(m_eXcontext, m_eXevent->tid,  code, answer);

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
HSSipUA::on_subscribe_status(const string &json, string &xml, Superior *super, int did, int expires)
{
	if (super->m_subscribedid > 0)
    {
      //  eXAutolock locker(m_eXcontext);
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

#if 0
        uint32_t type = atol(rvalue["SIP_XML"]["Item"]["@Type"].asCString());
        /* validate(type) */
        if (type < 0)
        {
            code = 404;
            break;
        }

        super->m_alramtype = type;
#endif
    }
    while (0);

    osip_message_t *answer  = NULL;
    eXosip_insubscription_build_answer(m_eXcontext, m_eXevent->tid, code, &answer);
    if (!answer)
    {
        return -1;
    }
    eXosip_insubscription_send_answer(m_eXcontext, m_eXevent->tid,  code, answer);

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
HSSipUA::on_subscribe_ivs(const string &json, string &xml, Superior *super, int did, int expires)
{
	if (super->m_subscribedid > 0)
    {
    //    eXAutolock locker(m_eXcontext);
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

#if 0
        uint32_t type = atol(rvalue["SIP_XML"]["Item"]["@Type"].asCString());
        /* validate(type) */
        if (type < 0)
        {
            code = 404;
            break;
        }

        super->m_alramtype = type;
#endif
    }
    while (0);

    osip_message_t *answer  = NULL;
    eXosip_insubscription_build_answer(m_eXcontext, m_eXevent->tid, code, &answer);
    if (!answer)
    {
        return -1;
    }
    eXosip_insubscription_send_answer(m_eXcontext, m_eXevent->tid,  code, answer);

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
HSSipUA::on_subscribe_vi(const string &json, string &xml, Superior *super, int did, int expires)
{
	if (super->m_subscribedid > 0)
    {
      //  eXAutolock locker(m_eXcontext);
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

#if 0
        uint32_t type = atol(rvalue["SIP_XML"]["Item"]["@Type"].asCString());
        /* validate(type) */
        if (type < 0)
        {
            code = 404;
            break;
        }

        super->m_alramtype = type;
#endif
    }
    while (0);

    osip_message_t *answer  = NULL;
    eXosip_insubscription_build_answer(m_eXcontext, m_eXevent->tid, code, &answer);
    if (!answer)
    {
        return -1;
    }
    eXosip_insubscription_send_answer(m_eXcontext, m_eXevent->tid,  code, answer);

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
HSSipUA::query_resource(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code,
                        string &content, int jx, int fromidx, int toidx)
{

    return 0;
}

int
HSSipUA::push_alarm(const string &time, string &jalarms, int did, Superior *super)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
    if (!reader.parse(jalarms, rvalue, false))
    {
        return -1;
    }

	SHL_Json::StreamWriterBuilder writer;
	SHL_Json::Value               wvalue;

    wvalue["SIP_XML"]["@EventType"] = "Notify_Alarm";

    for (uint32_t i = 0; i < rvalue.size(); ++i)
    {
        wvalue["SIP_XML"]["Item"][i]["@Code"]      = rvalue[i]["code"];
        wvalue["SIP_XML"]["Item"][i]["@Name"]      = rvalue[i]["name"];
        wvalue["SIP_XML"]["Item"][i]["@Type"]      = to_string(rvalue[i]["type"].asInt());
        wvalue["SIP_XML"]["Item"][i]["@StartTime"] = time;
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

int
HSSipUA::push_status(const string &code, string &jstatus, int did, Superior *super)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
    if (!reader.parse(jstatus, rvalue, false))
    {
        return -1;
    }

	SHL_Json::StreamWriterBuilder writer;
	SHL_Json::Value               wvalue;

    wvalue["SIP_XML"]["@EventType"] = "Notify_Status";

    for (uint32_t i = 0; i < rvalue.size(); ++i)
    {
        wvalue["SIP_XML"]["Item"][i]["@Code"]   = rvalue[i]["code"];
        wvalue["SIP_XML"]["Item"][i]["@Name"]   = rvalue[i]["name"];
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

int
HSSipUA::push_ivs(const string &time, string &jivs, int did)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
    if (!reader.parse(jivs, rvalue, false))
    {
        return -1;
    }

	SHL_Json::StreamWriterBuilder writer;
	SHL_Json::Value               wvalue;

    wvalue["SIP_XML"]["@EventType"] = "Notify_IVS";

    for (uint32_t i = 0; i < rvalue.size(); ++i)
    {
        wvalue["SIP_XML"]["Item"][i]["@Code"]      = rvalue[i]["code"];
        wvalue["SIP_XML"]["Item"][i]["@Name"]      = rvalue[i]["name"];
        wvalue["SIP_XML"]["Item"][i]["@Type"]      = to_string(rvalue[i]["type"].asInt());
        wvalue["SIP_XML"]["Item"][i]["@StartTime"] = time;
    }

    string xml, jresp;
	json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);

    return sipUA->notify("IVS", did, xml, jresp, "");
}

int
HSSipUA::push_vi(const string &time, string &jvi,  int did)
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
        wvalue["SIP_XML"]["Item"][i]["@Code"]      = rvalue[i]["code"];
        wvalue["SIP_XML"]["Item"][i]["@Name"]      = rvalue[i]["name"];
        wvalue["SIP_XML"]["Item"][i]["@Type"]      = to_string(rvalue[i]["type"].asInt());
        wvalue["SIP_XML"]["Item"][i]["@StartTime"] = time;
    }

    string xml, jresp;
	json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);

    return sipUA->notify("IVS", did, xml, jresp, "");
}
