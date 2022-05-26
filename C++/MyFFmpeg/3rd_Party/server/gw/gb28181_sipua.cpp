#include "gb28181_sipua.h"
#include "system.h"
#include "platform.h"
#include "supermgr.h"
#include "tls_error.h"
#include "j2x.h"
#include "superior.h"
#include "putree.h"
#include "gsessmgr.h"
#include "stk/stk_message_rtsp.h"
#include "xml/XmlBase.h"

#define  GB28181_VIDEO_SDP_FORMAT \
    "v=0\r\n" \
    "o=%s 0 0 IN IP4 %s\r\n" \
    "s=Play\r\n" \
    "c=IN IP4 %s\r\n" \
    "t=0 0\r\n" \
    "m=video %d RTP/AVP 96 98\r\n" \
    "a=rtpmap:96 PS/90000\r\n" \
    "a=rtpmap:98 H264/90000\r\n" \
    "a=rtpmap:97 MPEG4/90000\r\n" \
    "y=%s\r\n" \
    "f=-\r\n\r\n"

#define GB28181_AUDIO_SDP_FORMAT \
    "v=0\r\n" \
    "o=%s 0 0 IN IP4 %s\r\n" \
    "s=Play\r\n" \
    "c=IN IP4 %s\r\n" \
    "t=0 0\r\n" \
    "m=audio %d RTP/AVP 8\r\n" \
    "a=rtpmap:8 PCMA/8000\r\n" \
    "a=sendrecv\r\n" \
	"y=%s\r\n" \
	"f=v/////a/1/8/1\r\n\r\n"

#define  GB28181_PLAYBACK_VIDEO_SDP_FORMAT \
    "v=0\r\n" \
    "o=%s 0 0 IN IP4 %s\r\n" \
    "s=Playback\r\n" \
    "c=IN IP4 %s\r\n" \
    "t=%d %d\r\n" \
    "m=video %d RTP/AVP 100\r\n" \
    "a=rtpmap:96 PS/90000\r\n" \
    "a=username:%s\r\n" \
    "a=password:%s\r\n" \
    "a=filesize:%d\r\n" \
    "a=sendonly\r\n" \
    "y=%s\r\n" \
    "f=v/2/4/25/1/a/1/\r\n"



#define  GB28181_DOWNLOAD_VIDEO_SDP_FORMAT \
    "v=0\r\n" \
    "o=%s 0 0 IN IP4 %s\r\n" \
    "s=Download\r\n" \
    "c=IN IP4 %s\r\n" \
    "t=%d %d\r\n" \
    "m=video %d RTP/AVP 100\r\n" \
    "a=rtpmap:96 PS/90000\r\n" \
    "a=username:%s\r\n" \
    "a=password:%s\r\n" \
    "a=filesize:%d\r\n" \
    "a=sendonly\r\n" \
    "y=%s\r\n" \
    "f=\r\n\r\n"




#define GB28181_AUDIOVIDEO_SDP_FORMAT \
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


#define GB28181_RTSP_ECHO_200 \
       "RTSP/1.0 200 OK\r\n" \
       "Server: Unitech RTSP Server\r\n" 

using namespace std;


void send_message_rsp_cb(string response, int code, int tid, void *userdata)
{
    GB28181_SipUA *sipua = (GB28181_SipUA*)userdata;
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

GB28181_SipUA::GB28181_SipUA(int transport, string bindIp, int port) 
    :SipUA(transport,bindIp,port)
{
    m_protocol = P_GB28181;
    m_SN = 1;
    m_bFirstRegister = false;
    m_bRegisted = false;
    m_sipUrl = NULL;
    m_contact = NULL;
    m_subsribeAlarmId = -1;
    m_subsribeCatalogId = -1;
}

GB28181_SipUA::~GB28181_SipUA()
{

}

int 
GB28181_SipUA::start()
{
    int result = SipUA::start();
    if (result == -1)
    {
        return result;
    }
    m_dataProThread.setEntry(GB28181_threadProc, this, T_DATAPROLOOP);
    result += m_dataProThread.start();
    return result;
}

void 
GB28181_SipUA::stop()
{
    m_eventThread.stop();
    m_dataProThread.stop();
    SipUA::stop();
}

int        
GB28181_SipUA::regist(const string &host, const string &suph, int supp, const string &pswd, int expires, int &rid, bool first)
{
    osip_message_t * reg = NULL;

    {
        string addrcode = systm->addrcode();
        string faddr = platform->addrcode();
        string fip = platform->ip();
        int fport = platform->sport();
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
            string from = string("sip:") + faddr + string("@") + fip + string(":") + std::to_string(fport);
            string proxy = string("sip:") + suph + string(":") + std::to_string(supp);

            char contact[100] = { 0 };
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
        char contact[100] = { 0 };
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
    int result = 0;

    do
    {
        string callid = osip_call_id_get_number(osip_message_get_call_id(reg));
        int    code = -1;

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
    } while (0);

    m_cqueue.clear(event);

    return result;
}

int                
GB28181_SipUA::push_catalog(const string &time, string &xml, int did, Superior *super)
{
    if (!super->m_subCatalog)
    {
        return -1;
    }
    datetime_s alarmT;
    sscanf(time.c_str(), "%d-%d-%dT%d:%d:%d", &alarmT.year, &alarmT.month, &alarmT.day, &alarmT.hour, &alarmT.minute, &alarmT.second);
    int64_t t1 = sys->cmpDatatime(alarmT, super->m_beginTimeCatalog);
    int64_t t2 = sys->cmpDatatime(alarmT, super->m_endTimeCatalog);
    if (sys->cmpDatatime(alarmT, super->m_beginTimeCatalog) < 0 || sys->cmpDatatime(alarmT, super->m_endTimeCatalog) > 0)
    {
        super->m_subCatalog = false;
        return -1;
    }
    string jresp;

    char contact[128] = { 0 };
#if 0
    snprintf(contact, 100, "<sip:%s@%s:%d>", platform->addrcode().c_str(), super->ifnetip().c_str(), platform->sport());
#else
    snprintf(contact, 100, "<sip:%s@%s:%d>", platform->addrcode().c_str(), platform->ip().c_str(), platform->sport());
#endif
    return sipUA->notify("presence", did, xml, jresp, contact);
}

int
GB28181_SipUA::push_alarm(const string &time, string &jalarms, int did, Superior *super)
{
    SHL_Json::Value  rvalue;
    SHL_Json::Reader reader;
    if (!reader.parse(jalarms, rvalue, false))
    {
        return -1;
    }

    datetime_s alarmT; 
    sscanf(time.c_str(), "%d-%d-%d %d:%d:%d", &alarmT.year, &alarmT.month, &alarmT.day, &alarmT.hour, &alarmT.minute, &alarmT.second);
    char alarmtime[128] = { 0 };
    sprintf(alarmtime, "%04d-%02d-%02dT%02d:%02d:%02d", alarmT.year, alarmT.month, alarmT.day, alarmT.hour, alarmT.minute, alarmT.second);

    if (0x0 == super->m_alramtype)
    {
        return -1;
    }

    if (sys->cmpDatatime(alarmT, super->m_beginTimeAlarm) < 0 || sys->cmpDatatime(alarmT, super->m_endTimeAlarm) > 0)
    {
        super->m_alramtype = 0;
        return -1;
    }

    for (uint32_t i = 0; i < rvalue.size(); ++i)
    {
        SHL_Json::StreamWriterBuilder writer;
        SHL_Json::Value               wvalue;

        int level = rvalue[i]["level"].asInt();

        int type = rvalue[i]["type"].asInt();
        if (type == TriggerListener::TR_DEV_STATUS)
        {
            continue;
        }
        wvalue["Notify"]["CmdType"] = "Alarm";
        wvalue["Notify"]["SN"] = to_string(GenerateSN());
        wvalue["Notify"]["DeviceID"] = rvalue[i]["gucode"].asString();
        wvalue["Notify"]["AlarmPriority"] = to_string(level);
        wvalue["Notify"]["AlarmMethod"] = to_string(5);
        wvalue["Notify"]["AlarmTime"] = alarmtime;
        wvalue["Notify"]["Info"]["AlarmType"] = to_string(type);

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
    
}

int
GB28181_SipUA::push_status(const string &code, string &jstatus, int did, Superior *super)
{   
    return 0;
}

int
GB28181_SipUA::push_mediaStatus(const string& gucode, const string& strType, Superior* super)
{

    SHL_Json::StreamWriterBuilder writer;
    SHL_Json::Value               wvalue;


    wvalue["Notify"]["CmdType"] = "MediaStatus";
    wvalue["Notify"]["SN"] = to_string(GenerateSN());
    wvalue["Notify"]["DeviceID"] = gucode;
    wvalue["Notify"]["NotifyType"] = strType;

    string xml, jresp;
    json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);

    if (super == NULL)
    {
        super = superMgr->findSuperiorByGucode(gucode);
    }
    assert(super != NULL);

    return sipUA->message(platform->addrcode(), super->ifnetip(), platform->sport(), super->addrcode(), super->host(), super->sport(), xml, jresp);
}

int
GB28181_SipUA::notify(const string &event, int did, const string &xml, string &jresp, string contact)
{
    eXAutolock locker(m_eXcontext);
    osip_message_t *request = NULL;

    int result = eXosip_insubscription_build_notify(m_eXcontext, did, EXOSIP_SUBCRSTATE_ACTIVE, 0, &request);
    if (OSIP_SUCCESS != result || !request)
    {
        return -1;
    }

    osip_message_set_header(request, "Event", event.c_str());

    osip_message_set_content_type(request, "application/MANSCDP+xml");
    osip_message_set_body(request, xml.c_str(), xml.size());

    osip_list_remove(&request->contacts, 0);
    osip_message_set_contact(request, contact.c_str());

    return eXosip_insubscription_send_request(m_eXcontext, did, request);
}

int
GB28181_SipUA::message(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top,
const string &xml, string &xresp)
{
    int result = 0;

    char from[100];
    char to[100];
    snprintf(from, 100, "<sip:%s@%s:%d>", fromn.c_str(), m_bindIp.c_str(), m_listenPort);
    snprintf(to, 100, "<sip:%s@%s:%d>", ton.c_str(), toh.c_str(), top);

    osip_message_t *message = NULL;
    {
        eXAutolock locker(m_eXcontext);
        eXosip_message_build_request(m_eXcontext, &message, "MESSAGE", to, from, NULL);
        if (message == NULL)
        {
            return -1;
        }
        osip_message_set_contact(message, from);

        osip_message_set_content_type(message, "application/MANSCDP+xml");
        osip_message_set_body(message, xml.c_str(), xml.size());
        eXosip_message_send_request(m_eXcontext, message);
    }

    osip_call_id_t *call_id = osip_message_get_call_id(message);
    string strCallID = osip_call_id_get_number(call_id);

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
        string json = SHL_Json::writeString(writer, resp);
        json2xml(json.c_str(), xresp);
    }

    m_cqueue.clear(event);
    return result;
}

int        
GB28181_SipUA::on_invite(osip_message_t *sip)
{
    string fromn = "";
    string fromh = "";
    string fromp = "";
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


    char srcsdp[2048] = { 0 };
    char contact[512] = { 0 };
    do
    {
        osip_to_t * to = NULL;
        to = osip_message_get_to(sip);
        if (!to)
        {
            LOGE_SIP("osip_message_get_to fail.");
            code = 401;
            break;
        }

        osip_uri_t * url = osip_to_get_url(to);
        string ton = (p = osip_uri_get_username(url)) ? p : "";
        string toh = (p = osip_uri_get_host(url)) ? p : "";
        string top = (p = osip_uri_get_port(url)) ? p : "";

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

        osip_body_t * bodies = NULL;
        if (!(-1 != osip_message_get_body(sip, 0, &bodies) && bodies))
        {
            LOGE_SIP("osip_message_get_body fail.");
            code = 401;
            break;
        }

        sdp_message_t *sdp = NULL;
        sdp_media_t   *sdp_media = NULL;
        sdp_media_t   *sdp_audio = NULL;

        sdp = eXosip_get_remote_sdp_from_tid(m_eXcontext, m_eXevent->tid);
        sdp_media = eXosip_get_video_media(sdp);
        sdp_audio = eXosip_get_audio_media(sdp);

        if (!sdp_media && !sdp_audio)
        {
            LOGE_SIP("eXosip_get_video_media fail.");
            code = 401;
            break;
        }


        string sname = sdp_message_s_name_get(sdp);
        if (sname == "Playback")
        {            
            char * startTimestr = sdp_message_t_start_time_get(sdp, 0);
            char * endTimestr = sdp_message_t_stop_time_get(sdp, 0);
            char * yy = sdp_message_y_ssrc_get(sdp);

            uint64_t startTimeSec = std::atoll(startTimestr);
            uint64_t endTimeSec = std::atoll(endTimestr);

            if (sdp_media && !sdp_audio)
            {
                int              pos = 0;
                sdp_attribute_t *attr = NULL;

                sdp_message_free(sdp);
                
                string sessid = toh + string(":") + top + string(":") + ton + string(":")+yy;
                int    srcport = gsessMgr->RTPSRCPort(sessid, startTimeSec*1000, endTimeSec*1000, 1);
                gsessMgr->setSSRC(sessid, std::atoi(yy));
                
                snprintf(srcsdp, sizeof(srcsdp), GB28181_PLAYBACK_VIDEO_SDP_FORMAT,
                    systm->addrcode().c_str(), platform->ip().c_str(),
                    platform->ip().c_str(),
					startTimeSec, endTimeSec,
                    srcport,
                    systm->addrcode().c_str(),
                    systm->addrcode().c_str(),
                    100000,
                    yy);

                snprintf(contact, 100, "<sip:%s@%s:%d>", ton.c_str(), platform->ip().c_str(), m_listenPort);
                code = 200;
            }
        }
        else if (sname == "Download")
        {
            char * startTimestr = sdp_message_t_start_time_get(sdp, 0);
            char * endTimestr = sdp_message_t_stop_time_get(sdp, 0);
            char * yy = sdp_message_y_ssrc_get(sdp);
            int fileSize = 0;
            uint64_t startTimeSec = std::atoll(startTimestr);
            uint64_t endTimeSec = std::atoll(endTimestr);


            if (sdp_media && !sdp_audio)
            {
                int              pos = 0;
                sdp_attribute_t *attr = NULL;

                while (!osip_list_eol(&(sdp_media->a_attributes), pos))
                {
                    attr = (sdp_attribute_t *)osip_list_get(&sdp_media->a_attributes, pos);
                    if (0 == strncmp(attr->a_att_field, "filesize", strlen("filesize")))
                    {
                        fileSize = atoi(attr->a_att_value);
                    }
                    pos++;
                }

                sdp_message_free(sdp);

                string sessid = toh + string(":") + top + string(":") + ton + string(":")+yy;
                int    srcport = gsessMgr->RTPSRCPort(sessid, startTimeSec * 1000, endTimeSec * 1000, 2);
                gsessMgr->setSSRC(sessid, std::atoi(yy));
                
                snprintf(srcsdp, sizeof(srcsdp), GB28181_DOWNLOAD_VIDEO_SDP_FORMAT,
                    systm->addrcode().c_str(), platform->ip().c_str(),
                    platform->ip().c_str(),
                    startTimestr, endTimestr,
                    srcport,
                    systm->addrcode().c_str(),
                    systm->addrcode().c_str(),
                    fileSize,
                    yy);

                snprintf(contact, 100, "<sip:%s@%s:%d>", ton.c_str(), platform->ip().c_str(), m_listenPort);
                code = 200;
            }
        }
        else if (sname == "Play")
        {
            if (!pu->validate(ton))
            {
                LOGE_SIP(getError());
                code = 404;
                break;
            }

            if (sdp_media && !sdp_audio)
            {
                /* 1:sub stream, 0:main stream, default:1 */
                int              vcmode = 0;
                char* cSsrc = sdp_message_y_ssrc_get(sdp);
                string ssrc(cSsrc);

                sdp_message_free(sdp);

            string vMode = ((0 == vcmode) ? "mainstream" : "substream");
            string sessid = toh + string(":") + top + string(":") + ton + string(":") + vMode;
            int    srcport = gsessMgr->RTPSRCPort(sessid);


#if 0
                snprintf(srcsdp, sizeof(srcsdp), GB28181_VIDEO_SDP_FORMAT, super->ifnetip().c_str(), super->ifnetip().c_str(), srcport, vcmode);
                snprintf(contact, 100, "<sip:%s@%s:%d>", ton.c_str(), super->ifnetip().c_str(), m_listenPort);
#else
                snprintf(srcsdp, sizeof(srcsdp), GB28181_VIDEO_SDP_FORMAT, systm->addrcode().c_str(), platform->ip().c_str(), platform->ip().c_str(), srcport, ssrc.c_str());
                snprintf(contact, 100, "<sip:%s@%s:%d>", ton.c_str(), platform->ip().c_str(), m_listenPort);
#endif

                code = 200;
            }
            if (sdp_audio && !sdp_media)
            {
                int              acmode = 0;
			int              pos = 0;
			sdp_attribute_t *attr = NULL;
			while (!osip_list_eol(&(sdp_audio->a_attributes), pos))
			{
				attr = (sdp_attribute_t *)osip_list_get(&sdp_audio->a_attributes, pos);
				if (0 == strncmp(attr->a_att_field, "sendonly", strlen("sendonly")))
				{
					acmode = 1;
				}
				else if (0 == strncmp(attr->a_att_field, "sendrecv", strlen("sendrecv")))
				{
					acmode = 0;
				}
				pos++;
			}
			char* cSsrc = sdp_message_y_ssrc_get(sdp);
			string ssrc(cSsrc);

                sdp_message_free(sdp);

                string aMode = ((0 == acmode) ? "dualtalk" : "broadcast");
                string sessid = toh + string(":") + top + string(":") + ton + string(":") + aMode;
                int    srcport = gsessMgr->RTPSRCPort(sessid);

#if 0
                snprintf(srcsdp, sizeof(srcsdp), GB28181_AUDIO_SDP_FORMAT, super->ifnetip().c_str(), super->ifnetip().c_str(), srcport);
                snprintf(contact, 100, "<sip:%s@%s:%d>", ton.c_str(), super->ifnetip().c_str(), m_listenPort);
#else
                snprintf(srcsdp, sizeof(srcsdp), GB28181_AUDIO_SDP_FORMAT, systm->addrcode().c_str(), platform->ip().c_str(), platform->ip().c_str(), srcport, ssrc.c_str());
                snprintf(contact, 100, "<sip:%s@%s:%d>", ton.c_str(), platform->ip().c_str(), m_listenPort);
#endif

                code = 200;
            }
        }
    } while (0);

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
GB28181_SipUA::on_ack(osip_message_t *sip)
{
    string fromn = "";
    string fromh = "";
    string fromp = "";
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
    } while (0);

    do
    {
        osip_to_t * to = NULL;
        to = osip_message_get_to(sip);
        if (!to)
        {
            LOGE_SIP("osip_message_get_to fail.");
            break;
        }

        osip_uri_t * url = osip_to_get_url(to);
        string ton = (p = osip_uri_get_username(url)) ? p : "";
        string toh = (p = osip_uri_get_host(url)) ? p : "";
        string top = (p = osip_uri_get_port(url)) ? p : "";

        sdp_message_t *sdp = NULL;
        sdp_media_t   *sdp_media = NULL;
        sdp_media_t   *sdp_audio = NULL;

        sdp = eXosip_get_remote_sdp_from_tid(m_eXcontext, m_eXevent->tid);
        sdp_media = eXosip_get_video_media(sdp);
        sdp_audio = eXosip_get_audio_media(sdp);        
        string sname = sdp_message_s_name_get(sdp);


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
            char* cSsrc = sdp_message_y_ssrc_get(sdp);
            string ssrc(cSsrc);

            sdp_message_free(sdp);

            osip_call_id_t * call_id = osip_message_get_call_id(sip);
            string           callid = (p = osip_call_id_get_number(call_id)) ? p : "";
            string           vMode = ((0 == vcmode) ? "mainstream" : "substream");
            string           sessid ;
            if (sname == "Play")
            {
                sessid= toh + string(":") + top + string(":") + ton + string(":") +vMode;
            }
            else {
                sessid= toh + string(":") + top + string(":") + ton + string(":") +ssrc;
            }

            SHL_Json::StreamWriterBuilder writer;
            SHL_Json::Value               value;

            value["fromn"] = fromn;
            value["fromh"] = fromh;
            value["fromp"] = atoi(fromp.c_str());
            value["ton"] = ton;
            value["toh"] = toh;
            value["top"] = atoi(top.c_str());
            value["destip"] = fromh;
            value["destport"] = destport;
            value["vcmode"] = vMode;
            value["ssrc"] = ssrc;
            

            string json = SHL_Json::writeString(writer, value);
            gsessMgr->RTPStart(sessid, callid, json);
        }
        if (sdp_audio && !sdp_media)
        {
            int destport = atoi(sdp_audio->m_port);

            int              acmode = 0;
			int              pos = 0;
			sdp_attribute_t *attr = NULL;
			while (!osip_list_eol(&(sdp_audio->a_attributes), pos))
			{
				attr = (sdp_attribute_t *)osip_list_get(&sdp_audio->a_attributes, pos);
				if (0 == strncmp(attr->a_att_field, "sendonly", strlen("sendonly")))
				{
					acmode = 1;
				}
				else if (0 == strncmp(attr->a_att_field, "sendrecv", strlen("sendrecv")))
				{
					acmode = 0;
				}
				pos++;
			}
			char* cSsrc = sdp_message_y_ssrc_get(sdp);
			string ssrc(cSsrc);
            sdp_message_free(sdp);

            osip_call_id_t * call_id = osip_message_get_call_id(sip);
            string           callid = (p = osip_call_id_get_number(call_id)) ? p : "";
            string           aMode = ((0 == acmode) ? "dualtalk" : "broadcast");
            string           sessid = toh + string(":") + top + string(":") + ton + string(":") + aMode;

            SHL_Json::StreamWriterBuilder writer;
            SHL_Json::Value               value;

            value["fromn"] = fromn;
            value["fromh"] = fromh;
            value["fromp"] = atoi(fromp.c_str());
            value["ton"] = ton;
            value["toh"] = toh;
            value["top"] = atoi(top.c_str());
            value["destip"] = fromh;
            value["destport"] = destport;
            value["vcmode"] = aMode;
			value["ssrc"] = ssrc;
            string json = SHL_Json::writeString(writer, value);
            gsessMgr->RTPStart(sessid, callid, json);
        }
    } while (0);

    string str;
    LOGW_SIP(osip2str(sip, str).c_str());

    return 0;
}

int
GB28181_SipUA::on_bye(osip_message_t *sip)
{
	char *p = NULL;

	osip_call_id_t * call_id = osip_message_get_call_id(sip);
	string         callid = (p = osip_call_id_get_number(call_id)) ? p : "";

	gsessMgr->RTPStop(callid);

	return 0;
}

int        
GB28181_SipUA::on_invite_response(osip_message_t *sip)
{
    SHL_Json::Value stRsp;
    unsigned short u16DestPort = 0;

    uint32_t u32DestVideoPort = 0;
    uint32_t u32DestAudioPort = 0;
    string ssrc = "";
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
    ssrc = sdp_message_y_ssrc_get(sdp);
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
    stRsp["code"] = sip->status_code;
    stRsp["callid"] = strCallID;
    stRsp["cid"] = m_eXevent->cid;
    stRsp["did"] = m_eXevent->did;
    stRsp["videoport"] = u32DestVideoPort;
    stRsp["audioport"] = u32DestAudioPort;
    stRsp["ssrc"] = ssrc;
    m_cqueue.response(strCallID, stRsp);

    return 0;
}

int        
GB28181_SipUA::on_message_response(osip_message_t *sip)
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
GB28181_SipUA::on_register(osip_message_t *sip)
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
        //stk_clrquote(s8Username);
        //stk_clrquote(s8Response);
        string strUsername = s8Username;
        string strResponse = s8Response;

        doAnswer(200);
        m_bRegisted = true;

        return 0;
    }

    if (m_bFirstRegister)
    {
        std::cout << "on_register:401" << std::endl;
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
        //stk_clrquote(s8Username);
        //stk_clrquote(s8Response);
        string strUsername = s8Username;
        string strResponse = s8Response;

        std::cout << "on_register:200" << std::endl;
        doAnswer(200);
        m_bRegisted = true;
    }

    return 0;
}

int
GB28181_SipUA::on_subscribe(osip_message_t *sip)
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

        if (on_parse_subscribe(sip, response, super) < 0)
        {
            code = 401;
            break;
        }
    } while (0);
    //send_message_rsp_cb(response, code, m_eXevent->tid, this);
    return 0;
}


int
GB28181_SipUA::on_info(osip_message_t *sip)
{
    string fromn = "";
    string fromh = "";
    string fromp = "";
    string response = "";
    string rtspCSeq = "";

    int    code = 200;
    char * p = NULL;
    char contact[512] = { 0 };

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

        osip_call_id_t * call_id = osip_message_get_call_id(sip);
        string           callid = (p = osip_call_id_get_number(call_id)) ? p : "";


        osip_to_t * to = NULL;
        to = osip_message_get_to(sip);
        if (!to)
        {
            LOGE_SIP("osip_message_get_to fail.");
            code = 401;
            break;
        }

        osip_uri_t * url = osip_to_get_url(to);
        string ton = (p = osip_uri_get_username(url)) ? p : "";
        string toh = (p = osip_uri_get_host(url)) ? p : "";
        string top = (p = osip_uri_get_port(url)) ? p : "";

        snprintf(contact, 100, "<sip:%s@%s:%d>", ton.c_str(), platform->ip().c_str(), m_listenPort);


        stk::RtspMsg rtspMsg(bodies->body, bodies->length);
        rtspCSeq = rtspMsg.GetCSeq();
        string Metthod = rtspMsg.GetMetthod();
        string sessid = rtspMsg.GetSession();

        if (Metthod == "PLAY")
        {
            string range = rtspMsg.GetRange();
            string Scale = rtspMsg.GetScale();

            gsessMgr->RTPPlay(callid, Scale, range);
        }
        else if (Metthod == "PAUSE")
        {
            //pause
            gsessMgr->RTPPause(callid);
        }
        else if (Metthod == "TEARDOWN")
        {

        }
    } while (0);

    
    stk::RtspMsg resp;
    resp.parse(GB28181_RTSP_ECHO_200, strlen(GB28181_RTSP_ECHO_200));
    resp.SetCSeq(rtspCSeq.c_str());
    //resp.SetSession(sessid.c_str());
    //resp.SetRange(range.c_str());
    //resp.SetScale(scale.c_str());
    resp.to_str(response);



    osip_message_t * answer = NULL;
    eXosip_call_build_answer(m_eXcontext, m_eXevent->tid, code, &answer);
    if ("" != response && answer != NULL)
    {
        osip_message_set_content_type(answer, "Application/MANSRTSP");
        osip_message_set_body(answer, response.c_str(), response.size());
    }
    else
    {
        char s8Contact[1024] = { 0 };
        snprintf(s8Contact, 100, "<sip:%s@%s:%d>", platform->addrcode().c_str(), platform->ip().c_str(), platform->sport());
        if (NULL != answer)
        {
            osip_list_remove(&answer->contacts, 0);
            osip_message_set_contact(answer, s8Contact);
        }
    }

    //osip_list_remove(&answer->contacts, 0);
    //osip_message_set_contact(answer, contact);
    eXosip_call_send_answer(m_eXcontext, m_eXevent->tid, code, answer);

    return 0;
}

int
GB28181_SipUA::on_message(osip_message_t *sip)
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
            code = 404;
            break;
        }

        Superior *super = superMgr->findSuperior(fromn);
        Pu *pPu = putree->findPu(fromn);
        if (!super && !pPu)
        {
            LOGW_SIP(getError());
            code = 404;
            break;
        }


        code = 200;

        if (on_parse_requestEx(sip, response, super, fromh,atoi(fromp.c_str()),pPu) < 0)
        {
            code = 404;
            break;
        }
    } while (0);
    send_message_rsp_cb(response, code, m_eXevent->tid, this);

    return 0;
}int
GB28181_SipUA::on_parse_subscribe(osip_message_t *sip, string &xml, Superior *super)
{
    osip_body_t   * bodies = NULL;
    osip_header_t * header = NULL;
    osip_message_get_body(sip, 0, &bodies);
    osip_message_get_expires(sip, 0, &header);

    string json;
    xml2json(bodies->body, json);

    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    SHL_Json::StreamWriterBuilder writer;
    SHL_Json::Value  wvalue;
    if (!reader.parse(json, rvalue, false))
    {
        LOGE_SIP("parse err: %s", json.c_str());
        return -1;
    }

    string eventType;
    string cmdType;


    SHL_Json::Value::Members member = rvalue.getMemberNames();
    for (SHL_Json::Value::Members::iterator iter = member.begin(); iter != member.end(); ++iter)
    {
        eventType = (*iter);
        if ("Query" == eventType)
        {
            string cmdType = rvalue["Query"]["CmdType"].asString();
            if ("Alarm" == cmdType || "Catalog" == cmdType)
            {
                return on_subscribe(json, xml, super, m_eXevent->did, atoi(header->hvalue));
            }
        }
    }

    osip_message_t *answer = NULL;
    eXosip_insubscription_build_answer(m_eXcontext, m_eXevent->tid, 404, &answer);
    eXosip_insubscription_send_answer(m_eXcontext, m_eXevent->tid, 404, answer);
    return -1;
}

int
GB28181_SipUA::on_parse_requestEx(osip_message_t *sip, string &xml, Superior *super, string fromh, int fromp, Pu *pu)
{
    osip_body_t   * bodies = NULL;
    osip_header_t * header = NULL;
    osip_message_get_body(sip, 0, &bodies);
    osip_message_get_expires(sip, 0, &header);

    string json;
    xml2json(bodies->body, json);

    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    SHL_Json::StreamWriterBuilder writer;
    SHL_Json::Value  wvalue;
    if (!reader.parse(json, rvalue, false))
    {
        LOGE_SIP("parse err: %s", json.c_str());
        return -1;
    }

    string eventType;
    string cmdType;


    SHL_Json::Value::Members member = rvalue.getMemberNames();
    for (SHL_Json::Value::Members::iterator iter = member.begin(); iter != member.end(); ++iter)
    {
        eventType = (*iter);
        if ("Query" == eventType)
        {
            cmdType = rvalue["Query"]["CmdType"].asString();
			if ("Alarm" == cmdType)
            {
                return on_subscribe(json, xml, super, m_eXevent->did, atoi(header->hvalue));
            }
            else if ("RecordInfo" == cmdType)
            {
                rvalue["ip"] = fromh;
                rvalue["port"] = fromp;
                return on_RecordInfo(SHL_Json::writeString(writer, rvalue), xml, super);
            }
			else
            {
                rvalue["ip"] = fromh;
                rvalue["port"] = fromp;
                return on_resource(SHL_Json::writeString(writer, rvalue), xml, super);
            }
        }
        else if ("Control" == eventType)
        {
            cmdType = rvalue["Control"]["CmdType"].asString();
            if ("DeviceControl" == cmdType)
            {
                return on_deviceCtrl(json, xml, super);
            }
            else if ("DragZoomIn" == cmdType || "DragZoomOut" == cmdType)
            {
                return on_dragZoom(json, xml, super);
            }
        }
        else if ("Notify" == eventType)
        {
            cmdType = rvalue["Notify"]["CmdType"].asString();
            if ("Keepalive" == cmdType)
            {
                SHL_Json::StreamWriterBuilder writer;
                SHL_Json::Value               wvalue;

                wvalue["Response"]["Variable"] = "KeepAlive";
                wvalue["Response"]["Result"] = "0";

                json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);

                if (pu)
                {
                    pu->UpdateAliveTime();
                }
                return 0;
            }
        }
        else if ("Response" == eventType)
        {
            cmdType = rvalue["Response"]["CmdType"].asString();
            if ("Catalog" == cmdType)
            {
                return on_catalogRsp(json, xml, super);
            }
            else if ("RecordInfo" == cmdType)
            {
                return on_recordRsp(json, xml, super);
            }
        }
    }



    return -1;
}


int
GB28181_SipUA::on_subscribe(const string &json, string &xml, Superior *super, int did, int expires )
{
    if (did <= 0)
    {
        return -1;
    }

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
        
        string CmdType = rvalue["Query"]["CmdType"].asString();
        if (CmdType == "Alarm")
        {
            if (super->m_subscribedid > 0 &&
                super->m_subscribedid != did)
            {
                eXosip_insubscription_remove(m_eXcontext, super->m_subscribedid);
            }

            super->m_subscribedid = did;
        }
        else
        {
            if (super->m_subscribeCatalogDid > 0 &&
                super->m_subscribeCatalogDid != did)
            {
                eXosip_insubscription_remove(m_eXcontext, super->m_subscribeCatalogDid);
            }

            super->m_subscribeCatalogDid = did;
        }

        string addrcode = rvalue["Query"]["DeviceID"].asString();
        string startTime = rvalue["Query"]["StartTime"].asString();
        string endTime = rvalue["Query"]["EndTime"].asString();

        if (systm->addrcode() != addrcode)
        {
            code = 404;
            break;
        }
        if (CmdType == "Alarm")
        {
            super->m_alramtype = 0xffffffff;
            sscanf(startTime.c_str(), "%04d-%02d-%02dT%02d:%02d:%02d",
                &super->m_beginTimeAlarm.year, &super->m_beginTimeAlarm.month, &super->m_beginTimeAlarm.day,
                &super->m_beginTimeAlarm.hour, &super->m_beginTimeAlarm.minute, &super->m_beginTimeAlarm.second);
            sscanf(endTime.c_str(), "%04d-%02d-%02dT%02d:%02d:%02d",
                &super->m_endTimeAlarm.year, &super->m_endTimeAlarm.month, &super->m_endTimeAlarm.day,
                &super->m_endTimeAlarm.hour, &super->m_endTimeAlarm.minute, &super->m_endTimeAlarm.second);
        }
        else
        {
            super->m_subCatalog = true;
            sscanf(startTime.c_str(), "%04d-%02d-%02dT%02d:%02d:%02d",
                &super->m_beginTimeCatalog.year, &super->m_beginTimeCatalog.month, &super->m_beginTimeCatalog.day,
                &super->m_beginTimeCatalog.hour, &super->m_beginTimeCatalog.minute, &super->m_beginTimeCatalog.second);
            sscanf(endTime.c_str(), "%04d-%02d-%02dT%02d:%02d:%02d",
                &super->m_endTimeCatalog.year, &super->m_endTimeCatalog.month, &super->m_endTimeCatalog.day,
                &super->m_endTimeCatalog.hour, &super->m_endTimeCatalog.minute, &super->m_endTimeCatalog.second);
        }
        SHL_Json::Value  wvalue;
        SHL_Json::StreamWriterBuilder writer;
        if (CmdType == "Alarm")
        {
            wvalue["Response"]["CmdType"] = "Alarm";
            if (0 == expires)
            {
                super->m_alramtype = 0;
            }
        }
        else
        {
            wvalue["Response"]["CmdType"] = "Catalog";
        }

        wvalue["Response"]["SN"] = rvalue["Query"]["SN"];
        wvalue["Response"]["DeviceID"] = addrcode;
        wvalue["Response"]["Result"] = "OK";
        json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);
    } while (0);

    osip_message_t *answer = NULL;
    eXosip_insubscription_build_answer(m_eXcontext, m_eXevent->tid, code, &answer);
    osip_message_set_body(answer, xml.c_str(), xml.length());
    eXosip_insubscription_send_answer(m_eXcontext, m_eXevent->tid, code, answer);
    return 0;
}

int                
GB28181_SipUA::on_resource(const string &json, string &xml, Superior *super)
{
    if (super)
    {
        return super->on_resource(json, xml);
    }

    return -1;
}

uint32_t
GB28181_SipUA::GenerateSN()
{
    AutoLock locker(m_mtxSN);
    if (m_SN == 0)
        m_SN = 1;
    return m_SN++;
}

int                
GB28181_SipUA::on_recordRsp(const string &json, string &xml, Superior *super)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(json, rvalue, false))
    {
        LOGE_SIP("parse err: %s", json.c_str());
        return -1;
    }

    string sn = rvalue["Response"]["SN"].asString();
    int iSn = atoi(sn.c_str());
    int num = atoi(rvalue["Response"]["RecordList"]["@Num"].asString().c_str());
    int sumNum = atoi(rvalue["Response"]["SumNum"].asString().c_str());
    {
        AutoLock lock(m_mtxSN);
        map<uint32_t, GB28181_SIP_REQ_S*>::iterator iter;
        iter = m_mapSN.find(iSn);
        if (iter != m_mapSN.end())
        {
            GB28181_SIP_REQ_S *req = iter->second;
            req->context.listJson->push_back(json);
            req->context.sumNum = sumNum;
            req->context.revNum += num;
        }
    }
    return 0;
}

int                
GB28181_SipUA::on_catalogRsp(const string &json, string &xml, Superior *super )
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(json, rvalue, false))
    {
        LOGE_SIP("parse err: %s", json.c_str());
        return -1;
    }

    string sn = rvalue["Response"]["SN"].asString();
    int iSn = atoi(sn.c_str());
    int num = atoi(rvalue["Response"]["DeviceList"]["@Num"].asString().c_str());
    int sumNum = atoi(rvalue["Response"]["SumNum"].asString().c_str());
    {
        AutoLock lock(m_mtxSN);
        map<uint32_t, GB28181_SIP_REQ_S*>::iterator iter;
        iter = m_mapSN.find(iSn);
        if (iter != m_mapSN.end())
        {
            GB28181_SIP_REQ_S *req = iter->second;
            req->context.listJson->push_back(json);
            req->context.sumNum = sumNum;
            req->context.revNum += num;
        }
    }
    return 0;
}

int
GB28181_SipUA::on_dragZoom(const string &json, string &xml, Superior *super) const
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(json, rvalue, false))
    {
        return -1;
    }

    string addrcode = rvalue["Control"]["DeviceID"].asString();
    string cmdType = rvalue["Control"]["CmdType"].asString();
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

    if (Pu::PROTO_TYPE_GW_B == pPu->protocol() ||
        Pu::PROTO_TYPE_GW_B_HIK == pPu->protocol() ||
        Pu::PROTO_TYPE_GW_B_HS == pPu->protocol() ||
        Pu::PROTO_TYPE_GW_B_UT == pPu->protocol() ||
        Pu::PROTO_TYPE_GB28181 == pPu->protocol())
    {
        //GwPu *pGwPu = (GwPu*)pPu;
        //return pGwPu->ptzCtrl(cmd, addrcode, speed, preset, 0);
    }
    else
    {
        string CmdType = rvalue["Control"]["CmdType"].asString();
        int winW = atoi(rvalue["Control"]["Length"].asString().c_str());
        int winH = atoi(rvalue["Control"]["Width"].asString().c_str());
        int midPointX = atoi(rvalue["Control"]["MidPointX"].asString().c_str());
        int midPointY = atoi(rvalue["Control"]["MidPointY"].asString().c_str());
        int lengthX = atoi(rvalue["Control"]["LengthX"].asString().c_str());
        int lengthY = atoi(rvalue["Control"]["LengthY"].asString().c_str());

        SHL_Json::StreamWriterBuilder writer;
        SHL_Json::Value  wvalue;
        wvalue["Method"] = "ptzPostion3D";
        wvalue["chnNo"] = Pu::GetChanNoByCode(addrcode);
        int X1, Y1, X2, Y2;
        if ("DragZoomIn" == cmdType)
        {
            X1 = midPointX - lengthX / 2;
            Y1 = midPointY - lengthY / 2;
            X2 = midPointX + lengthX / 2;
            Y2 = midPointY + lengthY / 2;
        }
        else
        {
            X1 = midPointX + lengthX / 2;
            Y1 = midPointY + lengthY / 2;
            X2 = midPointX - lengthX / 2;
            Y2 = midPointY - lengthY / 2;
        }
        wvalue["X1"] = X1 * 255 / winW;
        wvalue["Y1"] = Y1 * 255 / winH;
        wvalue["X2"] = X2 * 255 / winW;
        wvalue["Y2"] = Y2 * 255 / winH;

        string jsoncmd = SHL_Json::writeString(writer, wvalue);
        if (0 == pPu->onCustomCommand(jsoncmd));
        {
            return 0;
        }
        return -1;
    }

}

int                
GB28181_SipUA::on_deviceCtrl(const string &json, string &xml, Superior *super)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(json, rvalue, false))
    {
        return -1;
    }

    string addrcode = rvalue["Control"]["DeviceID"].asString();
    string ptzcmd = rvalue["Control"]["PTZCmd"].asString();
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
    uint64_t ptznum = sys->StrToDecimal(ptzcmd, 16);
    
    int cmd = (uint8_t)(ptznum >> 8 * (8-4));
    int data5 = (uint8_t)(ptznum >> 8 * (8-5));
    int data6 = (uint8_t)(ptznum >> 8 * (8-6));
    int data7 = (uint8_t)(ptznum >> 8 * (8-7))&0xf0;

    if (Pu::PROTO_TYPE_GW_B == pPu->protocol() ||
        Pu::PROTO_TYPE_GW_B_HIK == pPu->protocol() ||
        Pu::PROTO_TYPE_GW_B_HS == pPu->protocol() ||
        Pu::PROTO_TYPE_GW_B_UT == pPu->protocol() ||
        Pu::PROTO_TYPE_GB28181 == pPu->protocol())
    {
        //GwPu *pGwPu = (GwPu*)pPu;
        //return pGwPu->ptzCtrl(cmd, addrcode, speed, preset, 0);
    }
    else
    {
        int s32PtzCmd = -1;
        int s32Speed = sqrt(data5*data5 + data6*data6) * 10 / 0xff;
        if (s32Speed < 1)
        {
            s32Speed = 1;
        }
        int s32Stop = 0;
        int preset = data6;

        switch (cmd)
        {
        case GB28181_PTZ_CMD_ZOOMOUT:       s32PtzCmd = Pu::PTZ_CMD_ZOOMOUT;        s32Speed = data7;   s32Stop = 0 ; break;
        case GB28181_PTZ_CMD_ZOOMIN:        s32PtzCmd = Pu::PTZ_CMD_ZOOMIN;         s32Speed = data7;   s32Stop = 0 ; break;
        case GB28181_PTZ_CMD_UP:            s32PtzCmd = Pu::PTZ_CMD_UP;                                 s32Stop = 0 ; break;
        case GB28181_PTZ_CMD_DOWN:          s32PtzCmd = Pu::PTZ_CMD_DOWN;                               s32Stop = 0; break;
        case GB28181_PTZ_CMD_LEFT:          s32PtzCmd = Pu::PTZ_CMD_LEFT;                               s32Stop = 0; break;
        case GB28181_PTZ_CMD_RIGHT:         s32PtzCmd = Pu::PTZ_CMD_RIGHT;                              s32Stop = 0; break;
        case GB28181_PTZ_CMD_UPLEFT:        s32PtzCmd = Pu::PTZ_CMD_UPLEFT;                             s32Stop = 0 ; break;
        case GB28181_PTZ_CMD_UPRIGHT:       s32PtzCmd = Pu::PTZ_CMD_UPRIGHT;                            s32Stop = 0 ; break;
        case GB28181_PTZ_CMD_DOWNLEFT:      s32PtzCmd = Pu::PTZ_CMD_DOWNLEFT;                           s32Stop = 0; break;
        case GB28181_PTZ_CMD_DOWNRIGHT:     s32PtzCmd = Pu::PTZ_CMD_DOWNRIGHT;                          s32Stop = 0; break;
        case GB28181_PTZ_CMD_IRISCLOSE:     s32PtzCmd = Pu::PTZ_CMD_IRISCLOSE;      s32Speed = data6;   s32Stop = 0; break;
        case GB28181_PTZ_CMD_IRISOPEN:      s32PtzCmd = Pu::PTZ_CMD_IRISOPEN;       s32Speed = data6;   s32Stop = 0; break;
        case GB28181_PTZ_CMD_FOCUSFAR:      s32PtzCmd = Pu::PTZ_CMD_FOCUSFAR;       s32Speed = data5;   s32Stop = 0; break;
        case GB28181_PTZ_CMD_FOCUSNEAR:     s32PtzCmd = Pu::PTZ_CMD_FOCUSNEAR;      s32Speed = data5;   s32Stop = 0; break;
        case GB28181_PTZ_CMD_ALLSTOP_LENS:  s32PtzCmd = Pu::PTZ_CMD_FOCUSNEAR;      s32Speed = 0;       s32Stop = 1; break;
        case GB28181_PTZ_CMD_ALLSTOP_PTZ:   s32PtzCmd = Pu::PTZ_CMD_UP;             s32Speed = 0;       s32Stop = 1; break;
        case GB28181_PTZ_CMD_SETPRESET:     s32PtzCmd = Pu::PTZ_CMD_SETPRESET;      break;
        case GB28181_PTZ_CMD_CALLPRESET:    s32PtzCmd = Pu::PTZ_CMD_CALLPRESET;    break;
        case GB28181_PTZ_CMD_CLEARPRESET:   s32PtzCmd = Pu::PTZ_CMD_CLEARPRESET;     break;
        default:
            break;
        }

        if (0 == pPu->ptzCtrl(s32PtzCmd, addrcode, s32Speed, preset, s32Stop))
        {
            return 0;
        }
        return -1;
    }
}


int
GB28181_SipUA::on_RecordInfo(const string &json, string &xml, Superior *super)
{
    if (super)
    {
        return super->on_RecordInfo(json, xml);
    }

    
    return -1;
}

int                
GB28181_SipUA::makeXmlReq(const GB28181_SIP_REQ_S *req, string& xml)
{
    CXmlBase xmlBase1;
    char buf[200 * 1024] = { 0 };
    int bufSize = 200 * 1024;
    int retSize;

    if ("Query_Catalog" == req->strType)
    {
        xmlBase1.CreateRoot("Query");
        xmlBase1.AddNode("CmdType", "Catalog");
        xmlBase1.OutOfElem();

        xmlBase1.AddNode("SN", std::to_string(req->stResouceReq.u32SN));
        xmlBase1.OutOfElem();

        xmlBase1.AddNode("DeviceID", (char*)req->stResouceReq.ps8ChnAddrCode);
        xmlBase1.OutOfElem();
    }
    else if ("Control_Devctrl" == req->strType)
    {
        xmlBase1.CreateRoot("Control");
        xmlBase1.AddNode("CmdType", "DeviceControl");
        xmlBase1.OutOfElem();

        xmlBase1.AddNode("SN", std::to_string(req->stDevctrlReq.u32SN));
        xmlBase1.OutOfElem();

        xmlBase1.AddNode("DeviceID", (char*)req->stDevctrlReq.ps8ChnAddrCode);
        xmlBase1.OutOfElem();

        xmlBase1.AddNode("PTZCmd", (char*)req->stDevctrlReq.ps8PTZCmd);
        xmlBase1.OutOfElem();
    }
    else if ("Subscribe" == req->strType)
    {
        xmlBase1.CreateRoot("Query");

        if (req->stSubscribeReq.u32SubType == 0)
        {
            xmlBase1.AddNode("CmdType", "Alarm");
        }
        else
        {
            xmlBase1.AddNode("CmdType", "Catalog");
        }
        xmlBase1.OutOfElem();

        xmlBase1.AddNode("SN", std::to_string(req->stSubscribeReq.u32SN));
        xmlBase1.OutOfElem();

        xmlBase1.AddNode("DeviceID", (char*)req->stSubscribeReq.ps8ChnAddrCode);
        xmlBase1.OutOfElem();
        if (req->stSubscribeReq.u32SubType == 0)
        {
            xmlBase1.AddNode("StartAlarmPriority", to_string(1));
            xmlBase1.OutOfElem();

            xmlBase1.AddNode("EndAlarmPriority", to_string(4));
            xmlBase1.OutOfElem();

            xmlBase1.AddNode("AlarmMethod", to_string(0));
            xmlBase1.OutOfElem();
        }
        xmlBase1.AddNode("StartTime", (char*)req->stSubscribeReq.ps8StartTime);
        xmlBase1.OutOfElem();

        xmlBase1.AddNode("EndTime", (char*)req->stSubscribeReq.ps8EndTime);
        xmlBase1.OutOfElem();
    }
    else if ("RecordInfo" == req->strType)
    {
        xmlBase1.CreateRoot("Query");
        xmlBase1.AddNode("CmdType", "RecordInfo");
        xmlBase1.OutOfElem();

        xmlBase1.AddNode("SN", std::to_string(req->stRecordReq.u32SN));
        xmlBase1.OutOfElem();

        xmlBase1.AddNode("DeviceID", (char*)req->stRecordReq.ps8ChnAddrCode);
        xmlBase1.OutOfElem();

        xmlBase1.AddNode("StartTime", sys->strftime(req->stRecordReq.start));
        xmlBase1.OutOfElem();

        xmlBase1.AddNode("EndTime", sys->strftime(req->stRecordReq.end));
        xmlBase1.OutOfElem();

        xmlBase1.AddNode("Type", "all");
        xmlBase1.OutOfElem();
    }
    else if ("Control_Dragzoom" == req->strType)
    {
        xmlBase1.CreateRoot("Control");
        xmlBase1.AddNode("CmdType", req->stDragzoomReq.zoomin ? "DragZoomIn" : "DragZoomOut");
        xmlBase1.OutOfElem();

        xmlBase1.AddNode("SN", std::to_string(req->stDragzoomReq.u32SN));
        xmlBase1.OutOfElem();

        xmlBase1.AddNode("DeviceID", (char*)req->stDragzoomReq.ps8ChnAddrCode);
        xmlBase1.OutOfElem();

        xmlBase1.AddNode("Length", std::to_string(req->stDragzoomReq.winW));
        xmlBase1.OutOfElem();

        xmlBase1.AddNode("Width", std::to_string(req->stDragzoomReq.winH));
        xmlBase1.OutOfElem();

        xmlBase1.AddNode("MidPointX", std::to_string(req->stDragzoomReq.midPointX));
        xmlBase1.OutOfElem();

        xmlBase1.AddNode("MidPointY", std::to_string(req->stDragzoomReq.midPointY));
        xmlBase1.OutOfElem();

        xmlBase1.AddNode("LengthX", std::to_string(req->stDragzoomReq.lengthX));
        xmlBase1.OutOfElem();

        xmlBase1.AddNode("LengthY", std::to_string(req->stDragzoomReq.lengthY));
        xmlBase1.OutOfElem();
    }
    xmlBase1.WriteToBuf(buf, bufSize, retSize);
    xml = buf;
    return 0;
}

int GB28181_SipUA::subscribe_envent(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top,
    int subType, datetime_s begin, datetime_s end, int expire)
{
    GB28181_SIP_REQ_S stReq;
    stReq.strType = "Subscribe";
    stReq.stSubscribeReq.ps8ChnAddrCode = (int8_t*)ton.c_str();
    stReq.stSubscribeReq.u32SN = GenerateSN();
    stReq.stSubscribeReq.u32SubType = subType;
    char beginTiem[128] = { 0 };
    char endTime[128] = { 0 };
    sprintf(beginTiem, "%04d-%02d-%02dT%02d:%02d:%02d", begin.year, begin.month, begin.day, begin.hour, begin.minute, begin.second);
    sprintf(endTime, "%04d-%02d-%02dT%02d:%02d:%02d", end.year, end.month, end.day, end.hour, end.minute, end.second);
    stReq.stSubscribeReq.ps8StartTime = (int8_t*)beginTiem;
    stReq.stSubscribeReq.ps8EndTime = (int8_t*)endTime;

    string strXml;
    makeXmlReq(&stReq, strXml);
    string strEvent = "presence";
    string stReponse;
    int s32Ret = 0;
    if (subType == 0)
    {
        if (m_subsribeAlarmId == -1)
        {
            s32Ret = subscribe(fromn, fromh, fromp, ton, toh, top, strEvent, expire, strXml, stReponse, subType);
        }
        else
        {
            s32Ret = refreshsubscribe(fromn, fromh, fromp, ton, toh, top, strEvent, expire, strXml, stReponse, subType);
        }
    }
    else
    {
        if (m_subsribeCatalogId == -1)
        {
            s32Ret = subscribe(fromn, fromh, fromp, ton, toh, top, strEvent, expire, strXml, stReponse, subType);
        }
        else
        {
            s32Ret = refreshsubscribe(fromn, fromh, fromp, ton, toh, top, strEvent, expire, strXml, stReponse, subType);
        }
    }
    if (s32Ret != 0)
    {
        return -1;
    }
    return 0;
}

int GB28181_SipUA::query_record(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code,
    int start, int end, int type, string &content)
{
    GB28181_SIP_REQ_S *pstReq = new GB28181_SIP_REQ_S;
    pstReq->strType = "RecordInfo";
    pstReq->stRecordReq.ps8ChnAddrCode = (int8_t*)code.c_str();
    pstReq->stRecordReq.u32SN = GenerateSN();
    pstReq->stRecordReq.start = start;
    pstReq->stRecordReq.end = end;
    pstReq->stRecordReq.type = type;
    pstReq->context.generateTime = sys->currentTimeMillis();
    pstReq->context.listJson = new list <string>;
    pstReq->context.revNum = 0;
    pstReq->context.sumNum = -1;

    int s32Ret = 0;

    string strXml;
    makeXmlReq(pstReq, strXml);

    string stReponse;
    {
        AutoLock lock(m_mtxSN);
        m_mapSN[pstReq->stRecordReq.u32SN] = pstReq;
    }
    s32Ret = message(fromn, fromh, fromp, ton, toh, top, strXml.c_str(), stReponse);
    if (s32Ret != 0)
    {
        return -1;
    }

    int result = 0;
    //CallEvent call;
    string strCallID = code + to_string(pstReq->stResouceReq.u32SN);
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
        content = SHL_Json::writeString(writer, resp);
    }

    m_cqueue.clear(event);
    return result;
}

int GB28181_SipUA::ctrl_camera(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code,
    int cmd, int param1 , int param2 , int param3 )
{
    GB28181_SIP_REQ_S stReq;
    stReq.strType = "Control_Devctrl";
    stReq.stDevctrlReq.ps8ChnAddrCode = (int8_t*)code.c_str();
    stReq.stDevctrlReq.u32SN = GenerateSN();

    uint64_t ptznum = 0;// = sys->StrToDecimal(ptzstr, 16);

    uint8_t b1 = 0xa5;
    uint8_t b2 = 0x0f;

    *((uint8_t*)(&ptznum) + (8 - 1)) = 0xa5;                    /** <指令的首字节*/
    *((uint8_t*)(&ptznum) + (8 - 2)) = 0x0f;                    /** <组合码 1 ,高 4 位是版本信息(本标准的版本号是 1.0 ,版本信息为 0H),低 4 位是校验位*/
    *((uint8_t*)(&ptznum) + (8 - 3)) = 0;                       /** <地址的低 8 位*/
    *((uint8_t*)(&ptznum) + (8 - 4)) = cmd;                     /** <指令码 */
    *((uint8_t*)(&ptznum) + (8 - 5)) = param1;                  /** <数据 1(水平控制速度相对值 / 聚焦速度) */
    *((uint8_t*)(&ptznum) + (8 - 6)) = param2;                  /** <数据 2(垂直控制速度相对值 / 光圈速度) */
    *((uint8_t*)(&ptznum) + (8 - 7)) = param3 << 4;             /** <组合码 2 ,高 4 位是数据 3 ,低 4 位是地址的高 4 位(变倍控制速度相对值) / */
    *((uint8_t*)(&ptznum) + (8 - 8)) = (b1 + b2 + cmd + param1 + param2 + (param3 << 4)) % 256;  /** <校验码 / */


    char str[64] = { 0 };
    sprintf(str, "%llX", ptznum);
    stReq.stDevctrlReq.ps8PTZCmd = (int8_t *)str;
    string strXml;
    makeXmlReq(&stReq, strXml);

    string stReponse;
    int s32Ret = message(fromn, fromh, fromp, code, toh, top, strXml.c_str(), stReponse);
    if (s32Ret != 0)
    {
        return -1;
    }
    return 0;
}

int
GB28181_SipUA::query_resource(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code,
string &content, int jx, int fromidx, int toidx)
{
    if (fromidx < 1 || toidx < fromidx)
    {
        return -1;
    }
    
    GB28181_SIP_REQ_S *pstReq = new GB28181_SIP_REQ_S;
    pstReq->strType = "Query_Catalog";
    pstReq->stResouceReq.ps8ChnAddrCode = (int8_t*)code.c_str();
    pstReq->stResouceReq.u32SN = GenerateSN();
    pstReq->context.generateTime = sys->currentTimeMillis();
    pstReq->context.listJson = new list < string > ;
    pstReq->context.revNum = 0;
    pstReq->context.sumNum = -1;

    int s32Ret = 0;

    string strXml;
    makeXmlReq(pstReq, strXml);

    string stReponse;
    {
        AutoLock lock(m_mtxSN);
        m_mapSN[pstReq->stResouceReq.u32SN] = pstReq;
    }
    s32Ret = message(fromn, fromh, fromp, ton, toh, top, strXml.c_str(), stReponse);
    if (s32Ret != 0)
    {
        return -1;
    }

    int result = 0;
    //CallEvent call;
    string strCallID = code+to_string(pstReq->stResouceReq.u32SN);
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
        content = SHL_Json::writeString(writer, resp);
    }

    m_cqueue.clear(event);
    return result;
}

void               
GB28181_SipUA::dataProloop()
{
    while (m_dataProThread.active())
    {
        GB28181_SIP_REQ_S *req = NULL;
        {
            AutoLock lock(m_mtxSN);
            map<uint32_t, GB28181_SIP_REQ_S*>::iterator it = m_mapSN.begin();
            for (; it != m_mapSN.end(); ++it)
            {
                GB28181_SIP_REQ_S *t = it->second;
                if ("Query_Catalog" == t->strType || "RecordInfo" == t->strType)
                {
                    if (t->context.revNum == t->context.sumNum)
                    {
                        req = t;
                        m_mapSN.erase(it->first);
                        break;
                    }
                    uint64_t nowTime = sys->currentTimeMillis();
                    if (nowTime - t->context.generateTime > 1000 * 10)
                    {
                        delete t->context.listJson;
                        delete t;
                        m_mapSN.erase(it->first);
                        break;
                    }
                }
                else
                {
                    delete t->context.listJson;
                    delete t;
                    m_mapSN.erase(it->first);
                    break;
                }
            }
        }
        if (!req)
        {
            sys->sleep(200);
            continue;
        }
        if ("Query_Catalog" == req->strType)
        {
            processCatalog(req);
        }
        else if ("RecordInfo" == req->strType)
        {
            processRecord(req);
        }
        delete req;
    }
}

void*
GB28181_SipUA::GB28181_threadProc(ThreadInfo *arg)
{
    GB28181_SipUA * ua = (GB28181_SipUA *)(arg->data);

    switch (arg->id)
    {
    case T_DATAPROLOOP:
        ua->dataProloop();
        break;
    default:
        break;
    }

    return NULL;
}

void
GB28181_SipUA::processCatalog(GB28181_SIP_REQ_S *req)
{
    int count = 0;
    SHL_Json::Value  wvalue;
    SHL_Json::StreamWriterBuilder writer;
    list<string>::iterator it = req->context.listJson->begin();
    for (; it != req->context.listJson->end(); ++it)
    {
        string json = (*it);
        if (json.empty())
        {
            continue;
        }
        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;
        if (!reader.parse(json, rvalue, false))
        {
            LOGE_SIP("parse err: %s", json.c_str());
            continue;
        }
        int num = atoi(rvalue["Response"]["DeviceList"]["@Num"].asString().c_str());
        int sumNum = atoi(rvalue["Response"]["SumNum"].asString().c_str());
        if (0 == count)
        {
            wvalue = rvalue;
            count = num;
        }
        else
        {
            if (1 == num)
            {
                wvalue["Response"]["DeviceList"]["@Num"] = to_string(++count);
                wvalue["Response"]["DeviceList"]["Item"].append(rvalue["Response"]["DeviceList"]["Item"]);
            }
            else
            {
                for (int i = 0; i < num; ++i)
                {
                    wvalue["Response"]["DeviceList"]["@Num"] = to_string(++count);
                    wvalue["Response"]["DeviceList"]["Item"].append(rvalue["Response"]["DeviceList"]["Item"][i]);
                }
            }
            string sss = SHL_Json::writeString(writer, wvalue);
        }
        if (count >= sumNum)
        {
            string devAddr = rvalue["Response"]["DeviceID"].asString();
            string sn = rvalue["Response"]["SN"].asString();
            m_cqueue.response(devAddr + sn, wvalue);
            break;
        }
    }
    delete req->context.listJson;
}

void
GB28181_SipUA::processRecord(GB28181_SIP_REQ_S *req)
{
    int count = 0;
    SHL_Json::Value  wvalue;
    SHL_Json::StreamWriterBuilder writer;
    list<string>::iterator it = req->context.listJson->begin();
    for (; it != req->context.listJson->end(); ++it)
    {
        string json = (*it);
        if (json.empty())
        {
            continue;
        }
        SHL_Json::Reader reader;
        SHL_Json::Value  rvalue;
        if (!reader.parse(json, rvalue, false))
        {
            LOGE_SIP("parse err: %s", json.c_str());
            continue;
        }
        int num = atoi(rvalue["Response"]["RecordList"]["@Num"].asString().c_str());
        int sumNum = atoi(rvalue["Response"]["SumNum"].asString().c_str());
        if (0 == count)
        {
            wvalue = rvalue;
            count = num;
        }
        else
        {
            if (1 == num)
            {
                wvalue["Response"]["RecordList"]["@Num"] = to_string(++count);
                wvalue["Response"]["RecordList"]["Item"].append(rvalue["Response"]["RecordList"]["Item"]);
            }
            else
            {
                for (int i = 0; i < num; ++i)
                {
                    wvalue["Response"]["RecordList"]["@Num"] = to_string(++count);
                    wvalue["Response"]["RecordList"]["Item"].append(rvalue["Response"]["RecordList"]["Item"][i]);
                }
            }
            string sss = SHL_Json::writeString(writer, wvalue);
        }

        if (count >= sumNum)
        {
            string devAddr = rvalue["Response"]["DeviceID"].asString();
            string sn = rvalue["Response"]["SN"].asString();
            m_cqueue.response(devAddr + sn, wvalue);
            break;
        }
    }
    delete req->context.listJson;
}
