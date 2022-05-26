#include "gb28181_sipua.h"
#include "eXosip2/eXosip.h"
#include "tls_error.h"
#include "j2x.h"
#include "xml/XmlBase.h"
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

GB28181_SipUA::GB28181_SipUA(string bindIp, int port, char *addrcode)
{
    m_addrcode = addrcode;
    m_bindIp = bindIp;
    m_listenPort = port;
    m_pfnsubscribecb = NULL;
    m_user = NULL;
    m_subsribeAlarmId = -1;
    m_subsribeCatalogId = -1;

    m_pfnStatusCb = NULL;
    m_pfnStatusData = NULL;

    m_bFirstRegister = false;
    m_bRegisted = false;
    m_sipUrl = NULL;
    m_contact = NULL;
    m_SN = 1;
}

GB28181_SipUA::~GB28181_SipUA()
{
    stop();
}

int
GB28181_SipUA::start()
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

    m_dataProThread.setEntry(threadProc, this, T_DATAPROLOOP);
    result += m_dataProThread.start();

    m_cleanHpThread.setEntry(threadProc, this, T_CLEANHPLOOP);
    result += m_cleanHpThread.start();

    return result;
}

void
GB28181_SipUA::stop()
{
    m_eventThread.stop();
    m_dataProThread.stop();
    m_cleanHpThread.stop();

    if (m_eXcontext)
    {
        eXosip_quit(m_eXcontext);
        osip_free(m_eXcontext);
        m_eXcontext = NULL;
    }
}

int
GB28181_SipUA::ack()
{
    osip_message_t *ack;
    eXosip_call_build_ack(m_eXcontext, m_eXevent->tid, &ack);
    eXosip_call_send_ack(m_eXcontext, m_eXevent->tid, ack);

    return 0;
}

void
GB28181_SipUA::bye(int32_t s32CallId, int32_t s32DialogId)
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
GB28181_SipUA::subscribe(const string &ton, const string &tohost, int top, const string &event, int expires, const string &xml, string &jresp, int subType)
{
    string toh = tohost;
    hostPortConvert(ton, toh, top);

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
        osip_message_set_content_type(subs, "application/MANSCDP+XML");
        osip_message_set_body(subs, xml.c_str(), xml.length());
        if (0 == subType)
        {
            m_subsribeAlarmId = eXosip_subscription_send_initial_request(m_eXcontext, subs);
        }
        else
        {
            m_subsribeCatalogId = eXosip_subscription_send_initial_request(m_eXcontext, subs);
        }
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
        if (0 == subType)        
        {
            eXosip_insubscription_remove(m_eXcontext, m_subsribeAlarmId);
            m_subsribeAlarmId = -1;
        }
        else
        {
            eXosip_insubscription_remove(m_eXcontext, m_subsribeCatalogId);
            m_subsribeCatalogId = -1;
        }
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
GB28181_SipUA::refreshsubscribe(const string &ton, const string &tohost, int top,
const string &event, int expires, const string &xml, string &jresp, int subType)
{
    string toh = tohost;
    hostPortConvert(ton, toh, top);

    int result = 0;
    char from[100];
    char to[100];
    snprintf(from, 100, "<sip:%s@%s:%d>", m_addrcode.c_str(), m_bindIp.c_str(), m_listenPort);
    snprintf(to, 100, "<sip:%s@%s:%d>", ton.c_str(), toh.c_str(), top);

    osip_message_t  *subs = NULL;
    {
        eXAutolock locker(m_eXcontext);

        if (0 == subType)        
        {
            eXosip_subscription_build_refresh_request(m_eXcontext, m_subsribeAlarmId, &subs);
        }
        else
        {
            eXosip_subscription_build_refresh_request(m_eXcontext, m_subsribeCatalogId, &subs);
        }

        osip_message_set_content_type(subs, "application/xml");
        osip_message_set_body(subs, xml.c_str(), xml.length());

        char tmp[512] = { 0 };
        snprintf(tmp, 10, "%i", expires);
        osip_message_set_expires(subs, tmp);

        osip_message_set_header(subs, "Event", event.c_str());

        if (0 == subType)        
        {
            eXosip_subscription_send_refresh_request(m_eXcontext, m_subsribeAlarmId, subs);
        }
        else
        {
            eXosip_subscription_send_refresh_request(m_eXcontext, m_subsribeCatalogId, subs);
        }
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
GB28181_SipUA::invite(const string &ton, const string &tohost, int top,
const string &sdp, string &jresp)
{
    string toh = tohost;
    hostPortConvert(ton, toh, top);

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
        if (200 != resp["code"].asInt())
            result = -1;

        SHL_Json::StreamWriterBuilder writer;
        jresp = SHL_Json::writeString(writer, resp);
    }

    m_cqueue.clear(event);
    return result;
}

void
GB28181_SipUA::setsubsribecb(pfnsubscribecb fncb, void *user)
{
    m_pfnsubscribecb = fncb;
    m_user = user;
}

void
GB28181_SipUA::setStatusCB(pfnsubscribecb fncb, void *user)
{
    m_pfnStatusCb = fncb;
    m_pfnStatusData = user;
}


int
GB28181_SipUA::info(const string &ton, const string &tohost, int top, int did, const string &rtsp, string &jresp)
{
    string toh = tohost;
    hostPortConvert(ton, toh, top);

    int result = 0;

    char from[100];
    char to[100];
    snprintf(from, 100, "<sip:%s@%s:%d>", m_addrcode.c_str(), m_bindIp.c_str(), m_listenPort);
    snprintf(to,   100, "<sip:%s@%s:%d>", ton.c_str(), toh.c_str(), top);

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

    string strCseq   = cseq->number;
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
    return result;
}

int        
GB28181_SipUA::doAnswer(int u32Code)
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
        osip_www_authenticate_set_opaque(www_auth, osip_strdup("\"b72bb85fdb78e\""));
        osip_message_set_expires(answer, "100");

        osip_list_add(&answer->www_authenticates, www_auth, -1);
        eXosip_message_send_answer(m_eXcontext, m_eXevent->tid, 401, answer);
    }

    return 0;
}
int
GB28181_SipUA::message(const string &ton, const string &tohost, int top,
const string &xml, string &xresp)
{
    string toh = tohost;
    hostPortConvert(ton, toh, top);

    int result = 0;

    char from[100];
    char to[100];
    snprintf(from, 100, "<sip:%s@%s:%d>", m_addrcode.c_str(), m_bindIp.c_str(), m_listenPort);
    snprintf(to, 100, "<sip:%s@%s:%d>", ton.c_str(), toh.c_str(), top);

    osip_message_t *message = NULL;
    {
        eXAutolock locker(m_eXcontext);
        eXosip_message_build_request(m_eXcontext, &message, "MESSAGE", to, from, NULL);
        if (message == NULL)
        {
            return -1;
        }

        //osip_message_set_contact(message, from);
        osip_message_set_content_type(message, "Application/MANSCDP+xml");
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

uint32_t
GB28181_SipUA::GetSN()
{
    AutoLock locker(m_mtxSN);
    if (m_SN == 0)
        m_SN = 1;
    return m_SN++;
}

int
GB28181_SipUA::makeXmlReq(const SIP_REQ_S *req, string& xml)
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

int                
GB28181_SipUA::drag_zoom(const string &ton, const string &tohost, int top, const string &code,
    bool zoomIn, int winW, int winH, int midPointX, int midPointY, int lenX, int lenY)
{
    string toh = tohost;
    hostPortConvert(ton, toh, top);

    SIP_REQ_S stReq;
    stReq.strType = "Control_Dragzoom";
    stReq.stDragzoomReq.ps8ChnAddrCode = (int8_t*)code.c_str();
    stReq.stDragzoomReq.u32SN = GetSN();
    stReq.stDragzoomReq.zoomin = zoomIn;
    stReq.stDragzoomReq.winW = winW;
    stReq.stDragzoomReq.winH = winH;
    stReq.stDragzoomReq.midPointX = midPointX;
    stReq.stDragzoomReq.midPointY = midPointY;
    stReq.stDragzoomReq.lengthX = lenX;
    stReq.stDragzoomReq.lengthY = lenY;

    string strXml;
    makeXmlReq(&stReq, strXml);

    string stReponse;
    int s32Ret = message(code, toh, top, strXml.c_str(), stReponse);
    if (s32Ret != 0)
    {
        return -1;
    }

    return 0;
}

int                
GB28181_SipUA::subscribe_envent(const string &ton, const string &tohost, int top, int subType, datetime_s begin, datetime_s end, int expire)
{
    string toh = tohost;
    hostPortConvert(ton, toh, top);

    SIP_REQ_S stReq;
    stReq.strType = "Subscribe";
    stReq.stSubscribeReq.ps8ChnAddrCode = (int8_t*)ton.c_str();
    stReq.stSubscribeReq.u32SN = GetSN();
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
            s32Ret = subscribe(ton, toh, top, strEvent, expire, strXml, stReponse, subType);
        }
        else
        {
            s32Ret = refreshsubscribe(ton, toh, top, strEvent, expire, strXml, stReponse, subType);
        }
    }
    else
    {
        if (m_subsribeCatalogId == -1)
        {
            s32Ret = subscribe(ton, toh, top, strEvent, expire, strXml, stReponse, subType);
        }
        else
        {
            s32Ret = refreshsubscribe(ton, toh, top, strEvent, expire, strXml, stReponse, subType);
        }
    }
    if (s32Ret != 0)
    {
        return -1;
    }

    return 0;
}

int                
GB28181_SipUA::ctrl_camera(const string &ton, const string &tohost, int top, const string &code, int cmd, int param1, int param2, int param3)
{
    string toh = tohost;
    hostPortConvert(ton, toh, top);

    SIP_REQ_S stReq;
    stReq.strType = "Control_Devctrl";
    stReq.stDevctrlReq.ps8ChnAddrCode = (int8_t*)code.c_str();
    stReq.stDevctrlReq.u32SN = GetSN();

    uint64_t ptznum = 0;// = sys->StrToDecimal(ptzstr, 16);

    uint8_t b1 = 0xa5;
    uint8_t b2 = 0x0f;

    *((uint8_t*)(&ptznum) + (8 - 1)) = 0xa5;                    /** <指令的首字节*/
    *((uint8_t*)(&ptznum) + (8 - 2)) = 0x0f;                    /** <组合码 1 ,高 4 位是版本信息(本标准的版本号是 1.0 ,版本信息为 0H),低 4 位是校验位*/
    *((uint8_t*)(&ptznum) + (8 - 3)) = 0;                       /** <地址的低 8 位*/
    *((uint8_t*)(&ptznum) + (8 - 4)) = cmd;                     /** <指令码 */
    *((uint8_t*)(&ptznum) + (8 - 5)) = param1 * 0xff / 10;      /** <数据 1(水平控制速度相对值 / 聚焦速度) */
    *((uint8_t*)(&ptznum) + (8 - 6)) = (cmd == 0x81 || cmd ==0x82||cmd == 0x83)?param2: param2 * 0xff / 10;      /** <数据 2(垂直控制速度相对值 / 光圈速度/预置位号) */
    *((uint8_t*)(&ptznum) + (8 - 7)) = (param3 * 0xf / 10) << 4;/** <组合码 2 ,高 4 位是数据 3 ,低 4 位是地址的高 4 位(变倍控制速度相对值) / */
    *((uint8_t*)(&ptznum) + (8 - 8)) = (b1 + b2 + cmd + param1 + param2 + (param3 << 4)) % 256;  /** <校验码 / */


    char str[64] = { 0 };
    sprintf(str, "%llX", ptznum);
    stReq.stDevctrlReq.ps8PTZCmd = (int8_t *)str;
    string strXml;
    makeXmlReq(&stReq, strXml);

    string stReponse;
    int s32Ret = message(code, toh, top, strXml.c_str(), stReponse);
    if (s32Ret != 0)
    {
        return -1;
    }

    return 0;
}

int
GB28181_SipUA::query_resource(const string &ton, const string &tohost, int top, const string &code, pfnQueryResourceCallback fnCb, void *pUser)
{
    string toh = tohost;
    hostPortConvert(ton, toh, top);

    SIP_REQ_S *pstReq =  new SIP_REQ_S;
    pstReq->strType = "Query_Catalog";
    pstReq->stResouceReq.ps8ChnAddrCode = (int8_t*)code.c_str();
    pstReq->stResouceReq.u32SN = GetSN();
    pstReq->context.cb = fnCb;
    pstReq->context.userdata = pUser;
    pstReq->context.generateTime = sys->currentTimeMillis();
    pstReq->context.listJson = new list < string >;
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
    s32Ret = message(ton, toh, top, strXml.c_str(), stReponse);
    if (s32Ret != 0)
    {
        return -1;
    }
    return 0;
}

int
GB28181_SipUA::query_record(const string &ton, const string &tohost, int top, const string &code, int start, int end, int type, pfnRecordCallback fnCb, void *pUser)
{
    string toh = tohost;
    hostPortConvert(ton, toh, top);

    SIP_REQ_S *pstReq = new SIP_REQ_S;
    pstReq->strType = "RecordInfo";
    pstReq->stRecordReq.ps8ChnAddrCode = (int8_t*)code.c_str();
    pstReq->stRecordReq.u32SN = GetSN();
    pstReq->stRecordReq.cb = fnCb;
    pstReq->stRecordReq.userdata = pUser;
    pstReq->stRecordReq.start = start;
    pstReq->stRecordReq.end = end;
    pstReq->stRecordReq.type = type;
    pstReq->context.cb = fnCb;
    pstReq->context.userdata = pUser;
    pstReq->context.generateTime = sys->currentTimeMillis();
    pstReq->context.listJson = new list < string >;
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
    s32Ret = message(ton, toh, top, strXml.c_str(), stReponse);
    if (s32Ret != 0)
    {
        return -1;
    }

    return 0;
}

int
GB28181_SipUA::on_request(osip_message_t *sip)
{
    if (MSG_IS_REGISTER(sip))
    {
        on_register(sip);
    }
    else if (MSG_IS_NOTIFY(sip))
    {
        on_notify(sip);
    }
    else if (MSG_IS_MESSAGE(sip))
    {
        on_message(sip);
    }

    return 0;
}

int
GB28181_SipUA::on_response(osip_message_t *sip)
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
        on_info_response(sip);
    }
    
    return 0;
}

int
GB28181_SipUA::on_register_response(osip_message_t *sip)
{
    SHL_Json::Value resp;

    string cid = osip_call_id_get_number(osip_message_get_call_id(sip));

    resp["code"] = sip->status_code;
    m_cqueue.response(cid, resp);

    return 0;
}

bool                
GB28181_SipUA::hostPortConvert(const string addrcode, string &host, int &port)
{
    map<string, HostPort>::iterator it;
    {
        AutoLock lock(m_mtxHostPort);
        it = m_mapHostPort.find(addrcode);
    }
    if (it != m_mapHostPort.end())
    {
        host = it->second.ip;
        port = it->second.port;
        return true;
    }
    return false;
}

int
GB28181_SipUA::parseXml(const string &toCode, const string &xml, SHL_Json::Value &json)
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

int        
GB28181_SipUA::on_subscribe_response(osip_message_t *sip)
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
GB28181_SipUA::on_parse_notify(osip_message_t *sip, string &xml)
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
        if ("Notify" == eventType)
        {
            cmdType = rvalue["Notify"]["CmdType"].asString();
            if ("Alarm" == cmdType)
            {
                if (m_pfnsubscribecb)
                {
                    m_pfnsubscribecb(json.c_str(), json.size(), m_user);
                }
                SHL_Json::StreamWriterBuilder writer;
                SHL_Json::Value               wvalue;

                wvalue["Response"]["CmdType"] = cmdType;
                wvalue["Response"]["SN"] = rvalue["Notify"]["SN"];
                wvalue["Response"]["DeviceID"] = rvalue["Notify"]["DeviceID"];
                wvalue["Response"]["Result"] = "OK";

                json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);                
                return 0;
            }
        }
        else if ("Response" == eventType)
        {
            cmdType = rvalue["Response"]["CmdType"].asString();
            if ("Catalog" == cmdType)
            {
                if (m_pfnsubscribecb)
                {
                    m_pfnsubscribecb(json.c_str(), json.size(), m_user);
                }
                SHL_Json::StreamWriterBuilder writer;
                SHL_Json::Value               wvalue;

                wvalue["Response"]["CmdType"] = cmdType;
                wvalue["Response"]["SN"] = rvalue["Response"]["SN"];
                wvalue["Response"]["DeviceID"] = rvalue["Response"]["DeviceID"];
                wvalue["Response"]["Result"] = "OK";

                json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);
                return 0;
            }
        }
    }
    return -1;

}


int
GB28181_SipUA::on_info_response(osip_message_t *sip)
{
    SHL_Json::Value stRsp;
    unsigned short u16DestPort = 0;

    osip_call_id_t *call_id = osip_message_get_call_id(sip);
    string strCallID  = osip_call_id_get_number(call_id);
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

    m_cqueue.response(strCallID+strCesq, stRsp);

    return 0;
}

int        
GB28181_SipUA::on_parse_request(osip_message_t *sip, string &xml)
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
        if ("Notify" == eventType)
        {
            cmdType = rvalue["Notify"]["CmdType"].asString();
            if ("Keepalive" == cmdType)
            {
                SHL_Json::StreamWriterBuilder writer;
                SHL_Json::Value               wvalue;

                wvalue["Response"]["Variable"] = "KeepAlive";
                wvalue["Response"]["Result"] = "0";

                json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);
                return 0;
            }
            else if("MediaStatus" == cmdType)
            {
                SHL_Json::StreamWriterBuilder writer;
                SHL_Json::Value               wvalue;

                wvalue["Response"]["Variable"] = "MediaStatus";
                wvalue["Response"]["Result"] = "0";

                json2xml(SHL_Json::writeString(writer, wvalue).c_str(), xml);
                return 99;//shl
            }
        }
        else if ("Response" == eventType)
        {
            cmdType = rvalue["Response"]["CmdType"].asString();
            if ("Catalog" == cmdType)
            {
                string sn = rvalue["Response"]["SN"].asString();
                int iSn = atoi(sn.c_str());
                int num = atoi(rvalue["Response"]["DeviceList"]["@Num"].asString().c_str());
                int sumNum = atoi(rvalue["Response"]["SumNum"].asString().c_str());
                {
                    AutoLock lock(m_mtxSN);
                    map<uint32_t, SIP_REQ_S*>::iterator iter;
                    iter = m_mapSN.find(iSn);
                    if (iter != m_mapSN.end())
                    {
                        SIP_REQ_S *req = iter->second;
                        req->context.listJson->push_back(json);
                        req->context.sumNum = sumNum;
                        req->context.revNum += num;
                    }
                }
                return 0;
            }
            else if ("RecordInfo" == cmdType)
            {
                string sn = rvalue["Response"]["SN"].asString();
                int isn = atoi(sn.c_str());
                {
                    AutoLock lock(m_mtxSN);
                    map<uint32_t, SIP_REQ_S*>::iterator iter;
                    iter = m_mapSN.find(atoi(sn.c_str()));
                    if (iter != m_mapSN.end())
                    {
                        SIP_REQ_S *req = iter->second;
                        req->stRecordReq.cb((char*)json.c_str(), json.length(), req->stRecordReq.userdata);
                    }
                    int a = 0;
                }

                return 0;
            }
        }

    }
    return -1;
}

int        
GB28181_SipUA::on_notify(osip_message_t *sip)
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

        code = 200;

        if (on_parse_notify(sip, response) < 0)
        {
            code = 404;
            break;
        }
    } while (0);

#if 0
    osip_message_t * answer = NULL;
    eXosip_message_build_answer(m_eXcontext, m_eXevent->tid, code, &answer);
    if ("" != response)
    {
        osip_message_set_content_type(answer, "application/MANSCDP+xml");
        osip_message_set_body(answer, response.c_str(), response.size());
    }

    eXosip_message_send_answer(m_eXcontext, m_eXevent->tid, code, answer);
#endif

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
    int    result = 0;
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

        code = 200;

        result = on_parse_request(sip, response);
        if (result < 0)
        {
            code = 404;
            break;
        }
    } while (0);
    send_message_rsp_cb(response, code, m_eXevent->tid, this);



    //shl send bye
    if (result == 99)
    {
        m_pfnStatusCb("121", 3, m_pfnStatusData);
    }

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

    {
        AutoLock lock(m_mtxHostPort);
        HostPort hp;
        hp.ip = strHost;
        hp.port = atoi(strPort.c_str());
        hp.freshTime = sys->currentTimeMillis();
        m_mapHostPort[strAddrCode] = hp;
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

void               
GB28181_SipUA::cleanHostPort()
{
    while (m_cleanHpThread.active())
    { 
        bool bBreak = false;
        {
            AutoLock lock(m_mtxHostPort);
            map<string, HostPort>::iterator it = m_mapHostPort.begin();
            while (m_mapHostPort.end() != it)
            {
                uint64_t nowTime = sys->currentTimeMillis();
                HostPort hp = it->second;
                if (nowTime - it->second.freshTime >1000 *30*2)
                {
                    m_mapHostPort.erase(it->first);
                    bBreak = true;
                    break;
                }
                ++it;
            }
        }
        if (!bBreak)
        {
            sys->sleep(300);
        }
    }
}

void               
GB28181_SipUA::dataProloop()
{
    while (m_dataProThread.active())
    {
        SIP_REQ_S *req = NULL;
        {
            AutoLock lock(m_mtxSN);
            map<uint32_t, SIP_REQ_S*>::iterator it = m_mapSN.begin();
            for (; it != m_mapSN.end();++it)
            {
                SIP_REQ_S *t = it->second;
                if ("Query_Catalog" == t->strType || "Query_Alarm" == t->strType || "RecordInfo" == t->strType)
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
        if ("Query_Catalog" == req->strType || "Query_Alarm" == req->strType)
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
                }
                string sss = SHL_Json::writeString(writer, wvalue);
                if (count >= sumNum)
                {
                    string strCatalog = SHL_Json::writeString(writer, wvalue);
                    req->context.cb((char*)strCatalog.c_str(), strCatalog.size(), req->context.userdata);
                    break;
                }
            }
            delete req->context.listJson;
        }
        delete req;
    }
}

void
GB28181_SipUA::eventloop()
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
        
#if 0
        static FILE *fp = fopen("evtype.txt", "wb+");
        char btype[64] = { 0 };
        if (ev->response)
            sprintf(btype, "ev->type:%d,code:%d,method:%s\n",
            ev->type, ev->response->status_code, ev->response->cseq->method);
        else if (ev->response && ev->response->cseq)
            sprintf(btype, "ev->type:%d,code:%d\n",
            ev->type, ev->response->status_code);
        else
            sprintf(btype, "ev->type:%d\n",ev->type);

        fwrite(btype, strlen(btype), 1, fp);
        fflush(fp);
#endif
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
            //on_ack(ev->request);
            break;

        case EXOSIP_REGISTRATION_SUCCESS:
        case EXOSIP_REGISTRATION_FAILURE:
        case EXOSIP_CALL_REQUESTFAILURE:
        case EXOSIP_MESSAGE_ANSWERED:
        case EXOSIP_MESSAGE_REQUESTFAILURE:
        case EXOSIP_SUBSCRIPTION_ANSWERED:
        case EXOSIP_NOTIFICATION_ANSWERED:
        case EXOSIP_CALL_MESSAGE_ANSWERED:
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

        //case EXOSIP_CALL_MESSAGE_ANSWERED:
        //    on_bye_response(ev->response);
        //    break;

        default:
            LOGI_MSV("receieved unknown eXosip event (type, did, cid) = (%d, %d, %d)", ev->type, ev->did, ev->cid);
            break;
        }

        eXosip_event_free(ev);
    }
}

void *
GB28181_SipUA::threadProc(ThreadInfo *info)
{
    GB28181_SipUA * ua = (GB28181_SipUA *)(info->data);

    switch (info->id)
    {
    case T_EVENTLOOP:
        ua->eventloop();
        break;
    case T_DATAPROLOOP:
        ua->dataProloop();
        break;
    case T_CLEANHPLOOP:
        ua->cleanHostPort();
        break;
    default:
        break;
    }

    return NULL;
}

string &
GB28181_SipUA::osip2str(osip_message_t *sip, string &str)
{
    return str;
}

eXAutolock::eXAutolock(eXosip_t *&eXosip) : m_eXcontext(eXosip)
{
    eXosip_lock(m_eXcontext);
}

eXAutolock::~eXAutolock()
{
    eXosip_unlock(m_eXcontext);
}
