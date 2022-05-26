#include "gw_pu.h"
#include "gw_sipua.h"
#include "platform.h"
#include "system.h"
#include "j2x.h"

GwPu::GwPu(int type, string addrcode, string name, string host, int sport, int rport, int hport, int rtport)
    : Pu(type, addrcode, name, 0, "", "", host)
{
    m_sport    = sport;
    m_rport    = rport;
    m_hport    = hport;
    m_rtport   = rtport;
    m_port     = m_rport;

    m_sipua    = NULL;
    m_rtspcall = NULL;
    m_tmPrevTime = sys->currentTimeMillis();
    resetMessageCB();
}

GwPu::~GwPu()
{

}

bool
GwPu::validate(int channelNo)
{
    return false;
}

int
GwPu::ptzCtrl(int cmd, int channelNo, int speed, int preset, bool stop)
{
    return -1;
}

int
GwPu::queryRecord(int channelNo, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json)
{
    return -1;
}

bool
GwPu::validate(string addrcode)
{
    return true;
}

int
GwPu::ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop)
{
    Thread *thread = new Thread;
    ParamPtzCtrl_T *param = new ParamPtzCtrl_T(thread,m_tid, T_PTZCTRL, this, addrcode, cmd, speed, preset, stop);
    thread->setEntry(threadProc, param, T_PTZCTRL);
    thread->start();
    thread->detach();

    return 0;
}

int
GwPu::queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode)
{
    Thread *thread = new Thread;
    ParamRecord_T *param = new ParamRecord_T(thread, m_tid, type, this, addrcode, start, end, fromIndex, toIndex, usercode);
    thread->setEntry(threadProc, param, T_RECORDQUERY);
    thread->start();
    thread->detach();

    return 0;
}

int
GwPu::syncTime(datetime_s &dt, const string &tzone)
{
    return 0;
}

int GwPu::queryResource(string addrcode, int fromIndex, int toIndex, string &json)
{
	GWSipUA * pSipUa = (GWSipUA *)m_sipua;

    return pSipUa->query_resource(systm->name(), "", 0, m_addrcode, m_ip, m_sport, addrcode, json, 0, fromIndex, toIndex);
}

int GwPu::subscribeEvent(string addrcode, int mode, int alarmtype, int expires)
{
	GWSipUA * pSipUa = (GWSipUA *)m_sipua;

	return pSipUa->subscribe_event(platform->addrcode(), "", 0, m_addrcode, m_ip, m_sport, addrcode, mode, expires, alarmtype);
}

int        
GwPu::setMessageCallBack(int tid, MessageRspCB cb, void *userdata)
{
    m_tid = tid;
    m_message_cb = cb;
    m_userdata = userdata;
    return 0;
}

void               
GwPu::resetMessageCB()
{
    m_tid = -1;
    m_message_cb = NULL;
    m_userdata = NULL;
}

void *
GwPu::threadProc(ThreadInfo *info)
{
    Param_T *param = (Param_T*)(info->data);
    GwPu * pu = (GwPu *)(param->m_pThis);
    int ret = -1;
    string rspXml = "";

    switch (info->id)
    {
         case T_PTZCTRL:
         {
             ParamPtzCtrl_T *param_t = (ParamPtzCtrl_T*)(param);
             GWSipUA * pSipUa = (GWSipUA *)(pu->m_sipua);
             int cmd = param_t->m_cmd;
             switch (param_t->m_cmd)
             {
             case PTZ_CMD_UP:
                 if (param_t->m_stop)
                     cmd = PTZ_CMD_0401;
                 else
                    cmd = PTZ_CMD_0402; 
                 break;
             case PTZ_CMD_DOWN:
                 if (param_t->m_stop)
                     cmd = PTZ_CMD_0403;
                 else
                     cmd = PTZ_CMD_0404;
                 break;
             case PTZ_CMD_LEFT:
                 if (param_t->m_stop)
                     cmd = PTZ_CMD_0503;
                 else
                     cmd = PTZ_CMD_0504;
                 break;
             case PTZ_CMD_RIGHT:
                 if (param_t->m_stop)
                     cmd = PTZ_CMD_0501;
                 else
                     cmd = PTZ_CMD_0502;
                 break;
             case PTZ_CMD_UPLEFT:
                 if (param_t->m_stop)
                     cmd = PTZ_CMD_0701;
                 else
                     cmd = PTZ_CMD_0702;
                 break;
             case PTZ_CMD_UPRIGHT:
                 if (param_t->m_stop)
                     cmd = PTZ_CMD_0801;
                 else
                     cmd = PTZ_CMD_0802;
                 break;
             case PTZ_CMD_DOWNLEFT:
                 if (param_t->m_stop)
                     cmd = PTZ_CMD_0703;
                 else
                     cmd = PTZ_CMD_0704;
                 break;
             case PTZ_CMD_DOWNRIGHT:
                 if (param_t->m_stop)
                     cmd = PTZ_CMD_0803;
                 else
                     cmd = PTZ_CMD_0804;
                 break;
             case PTZ_CMD_ZOOMIN:
                 if (param_t->m_stop)
                     cmd = PTZ_CMD_0303;
                 else
                     cmd = PTZ_CMD_0304;
                 break;
             case PTZ_CMD_ZOOMOUT:
                 if (param_t->m_stop)
                     cmd = PTZ_CMD_0301;
                 else
                     cmd = PTZ_CMD_0302;
                 break;
             case PTZ_CMD_FOCUSNEAR:
                 if (param_t->m_stop)
                     cmd = PTZ_CMD_0201;
                 else
                     cmd = PTZ_CMD_0202;
                 break;
             case PTZ_CMD_FOCUSFAR:
                 if (param_t->m_stop)
                     cmd = PTZ_CMD_0203;
                 else
                     cmd = PTZ_CMD_0204;
                 break;
             case PTZ_CMD_IRISOPEN:
                 if (param_t->m_stop)
                     cmd = PTZ_CMD_0104;
                 else
                     cmd = PTZ_CMD_0103;
                 break;
             case PTZ_CMD_IRISCLOSE:
                 if (param_t->m_stop)
                     cmd = PTZ_CMD_0101;
                 else
                     cmd = PTZ_CMD_0102;
                 break;
             case PTZ_CMD_RAIN_ON:cmd = PTZ_CMD_0A01;break;
             case PTZ_CMD_RAIN_OFF:cmd = PTZ_CMD_0A02; break;
             case PTZ_CMD_SETPRESET:cmd = PTZ_CMD_0601; break;
             case PTZ_CMD_CLEARPRESET:cmd = PTZ_CMD_0603; break;
             case PTZ_CMD_CALLPRESET:cmd = PTZ_CMD_0602; break;
             default:
                 break;
             }
             ret = pSipUa->ctrl_camera(systm->addrcode(), "", 0, pu->m_addrcode, pu->m_ip, pu->m_sport, param->m_addrcode, cmd, param_t->m_speed, param_t->m_preset, param_t->m_stop);
        
             break;
         }

         case T_ALARMQUERY:
         {
             string rspJson = "";
             ParamAlarm_T *param_t = (ParamAlarm_T*)(param);
             GWSipUA * pSipUa = (GWSipUA *)(pu->m_sipua);

             char starttime[128] = { 0 };
             char endtime[128] = { 0 };

             sprintf(starttime, "%04d-%02d-%02dT%02d:%02d:%02dZ", param_t->m_start.year, param_t->m_start.month, param_t->m_start.day, param_t->m_start.hour, param_t->m_start.minute, param_t->m_start.second);
             sprintf(endtime, "%04d-%02d-%02dT%02d:%02d:%02dZ", param_t->m_end.year, param_t->m_end.month, param_t->m_end.day, param_t->m_end.hour, param_t->m_end.minute, param_t->m_end.second);

             ret = pSipUa->query_alarm(systm->addrcode(), param_t->m_addrcode, pSipUa->port(), param_t->m_addrcode, pu->m_ip, pu->m_sport, param_t->m_usercode, \
                 starttime, endtime, param_t->m_alarmType, param_t->m_level, rspJson, 0, param_t->m_fromIndex, param_t->m_toIndex);
             if (0 == ret)
             {
                 SHL_Json::Reader reader;
                 SHL_Json::Value  rvalue;
                 SHL_Json::StreamWriterBuilder writer;
                 SHL_Json::Value               wvalue;

                 if (!reader.parse(rspJson, rvalue, false))
                 {
                     LOGE_SIP("parse err: %s", rspJson.c_str());
                     break;
                 }

                 wvalue["SIP_XML"]["@EventType"] = "Response_History_Alarm";
                 wvalue["SIP_XML"]["SubList"]["@Code"] = param_t->m_addrcode;

                 string subNum = rvalue["SIP_XML"]["SubList"]["@SubNum"].asString();
                 int iSubNum = atoi(subNum.c_str());
                 wvalue["SIP_XML"]["SubList"]["@RealNum"] = rvalue["SIP_XML"]["SubList"]["@RealNum"].asString();
                 wvalue["SIP_XML"]["SubList"]["@SubNum"] = subNum;
                 wvalue["SIP_XML"]["SubList"]["@FromIndex"] = std::to_string(param_t->m_fromIndex);
                 wvalue["SIP_XML"]["SubList"]["@ToIndex"] = std::to_string(param_t->m_fromIndex + iSubNum);


                 if (iSubNum > 1)
                 {
                     for (int i = 0; i < iSubNum; ++i)
                     {
                         wvalue["SIP_XML"]["SubList"]["Item"][i]["@Code"] = rvalue["SIP_XML"]["SubList"]["Item"][i]["@Code"].asString();
                         wvalue["SIP_XML"]["SubList"]["Item"][i]["@BeginTime"] = rvalue["SIP_XML"]["SubList"]["Item"][i]["@BeginTime"].asString();
                         wvalue["SIP_XML"]["SubList"]["Item"][i]["@Status"] = rvalue["SIP_XML"]["SubList"]["Item"][i]["@Status"].asString();
                         wvalue["SIP_XML"]["SubList"]["Item"][i]["@Type"] = rvalue["SIP_XML"]["SubList"]["Item"][i]["@Type"].asString();
                     }
                 }
                 else if (iSubNum == 1)
                 {
                     wvalue["SIP_XML"]["SubList"]["Item"]["@Code"] = rvalue["SIP_XML"]["SubList"]["Item"]["@Code"].asString();
                     wvalue["SIP_XML"]["SubList"]["Item"]["@BeginTime"] = rvalue["SIP_XML"]["SubList"]["Item"]["@BeginTime"].asString();
                     wvalue["SIP_XML"]["SubList"]["Item"]["@Status"] = rvalue["SIP_XML"]["SubList"]["Item"]["@Status"].asString();
                     wvalue["SIP_XML"]["SubList"]["Item"]["@Type"] = rvalue["SIP_XML"]["SubList"]["Item"]["@Type"].asString();
                 }
                    
                 json2xml(SHL_Json::writeString(writer, wvalue).c_str(), rspXml);
             }

             break;
         }
              
         case T_RECORDQUERY:
         {
             ParamRecord_T *param_t = (ParamRecord_T*)(param);
             if (param_t)
             {
                 GWSipUA * pSipUa = (GWSipUA *)(pu->m_sipua);
                
                 char starttime[128] = { 0 };
                 char endtime[128] = { 0 };

                 sprintf(starttime, "%04d-%02d-%02dT%02d:%02d:%02dZ", param_t->m_start.year, param_t->m_start.month, param_t->m_start.day, param_t->m_start.hour, param_t->m_start.minute, param_t->m_start.second);
                 sprintf(endtime, "%04d-%02d-%02dT%02d:%02d:%02dZ", param_t->m_end.year, param_t->m_end.month, param_t->m_end.day, param_t->m_end.hour, param_t->m_end.minute, param_t->m_end.second);

                 string jsonData;
                 ret = pSipUa->query_record(systm->addrcode(), param_t->m_addrcode, pSipUa->port(), param_t->m_addrcode, pu->m_ip, pu->m_sport, param_t->m_usercode, \
                     starttime, endtime, param_t->m_recordType, jsonData, 0, param_t->m_fromIndex, param_t->m_toIndex);

                 if (0 == ret)
                 {
                     SHL_Json::Reader reader;
                     SHL_Json::Value  rvalue;

                     SHL_Json::StreamWriterBuilder writer;
                     SHL_Json::Value               wvalue;

                     if (!reader.parse(jsonData, rvalue, false))
                     {
                         LOGE_SIP("parse err: %s", jsonData.c_str());
                         break;
                     }

                     wvalue["SIP_XML"]["@EventType"] = "Response_History_Video";
                     wvalue["SIP_XML"]["SubList"]["@Code"] = param_t->m_addrcode;

                     string subNum = rvalue["SIP_XML"]["SubList"]["@SubNum"].asString();
                     int iSubNum = atoi(subNum.c_str());
                     wvalue["SIP_XML"]["SubList"]["@RealNum"]   = rvalue["SIP_XML"]["SubList"]["@RealNum"].asString();
                     wvalue["SIP_XML"]["SubList"]["@SubNum"]    = subNum;
                     wvalue["SIP_XML"]["SubList"]["@FromIndex"] = std::to_string(param_t->m_fromIndex);
                     wvalue["SIP_XML"]["SubList"]["@ToIndex"] = std::to_string(param_t->m_fromIndex + iSubNum);


                     if (iSubNum > 1)
                     {
                         for (int i = 0; i < iSubNum; ++i)
                         {
                             string strUrl = rvalue["SIP_XML"]["SubList"]["Item"][i]["@FileUrl"].asString();
                             string strFileUrl = strUrl.substr(strUrl.find("rtsp:") + 5, strUrl.length());
#if 0
                             string url = string("rtsp://") + super->ifnetip() + string(":") + std::to_string(platform->rport()) + \
                                 string("@") + strFileUrl;
#else
                             string url = string("rtsp://") + platform->ip() + string(":") + std::to_string(platform->rport()) + \
                                 string("@") + strFileUrl;
#endif

                             wvalue["SIP_XML"]["SubList"]["Item"][i]["@FileUrl"]    = url;
                             wvalue["SIP_XML"]["SubList"]["Item"][i]["@FileName"]   = rvalue["SIP_XML"]["SubList"]["Item"][i]["@FileName"].asString();
                             wvalue["SIP_XML"]["SubList"]["Item"][i]["@BeginTime"]  = rvalue["SIP_XML"]["SubList"]["Item"][i]["@BeginTime"].asString();
                             wvalue["SIP_XML"]["SubList"]["Item"][i]["@EndTime"]    = rvalue["SIP_XML"]["SubList"]["Item"][i]["@EndTime"].asString();
                             wvalue["SIP_XML"]["SubList"]["Item"][i]["@Size"]       = rvalue["SIP_XML"]["SubList"]["Item"][i]["@Size"].asString();
                             wvalue["SIP_XML"]["SubList"]["Item"][i]["@DecoderTag"] = "100";
                             wvalue["SIP_XML"]["SubList"]["Item"][i]["@Type"]       = rvalue["SIP_XML"]["SubList"]["Item"][i]["@Type"].asString();
                         }
                     }
                     else if (iSubNum == 1)
                     {
                         string strUrl = rvalue["SIP_XML"]["SubList"]["Item"]["@FileUrl"].asString();
                         string strFileUrl = strUrl.substr(strUrl.find("rtsp:") + 5, strUrl.length());

#if 0
                         string url = string("rtsp://") + super->ifnetip() + string(":") + std::to_string(platform->rport()) + \
                             string("@") + strFileUrl;
#else
                         string url = string("rtsp://") + platform->ip() + string(":") + std::to_string(platform->rport()) + \
                             string("@") + strFileUrl;
#endif

                         wvalue["SIP_XML"]["SubList"]["Item"]["@FileUrl"]     = url;
                         wvalue["SIP_XML"]["SubList"]["Item"]["@FileName"]    = rvalue["SIP_XML"]["SubList"]["Item"]["@FileName"].asString();
                         wvalue["SIP_XML"]["SubList"]["Item"]["@BeginTime"]   = rvalue["SIP_XML"]["SubList"]["Item"]["@BeginTime"].asString();
                         wvalue["SIP_XML"]["SubList"]["Item"]["@EndTime"]     = rvalue["SIP_XML"]["SubList"]["Item"]["@EndTime"].asString();
                         wvalue["SIP_XML"]["SubList"]["Item"]["@Size"]        = rvalue["SIP_XML"]["SubList"]["Item"]["@Size"].asString();
                         wvalue["SIP_XML"]["SubList"]["Item"]["@DecoderTag"]  = "100";
                         wvalue["SIP_XML"]["SubList"]["Item"]["@Type"]        = rvalue["SIP_XML"]["SubList"]["Item"]["@Type"].asString();
                     }

                     string jjj = SHL_Json::writeString(writer, wvalue);
                     json2xml(jjj.c_str(), rspXml);

                     if (pu->m_clienQueryRecord_cb)
                     {
						 pu->m_clienQueryRecord_cb(ret, pu->protocol(), jjj, pu->m_clientUserdata);
                     }
                 }
             }
             break;
         }
                
         default:
         {
             break;
         }
    }

    int code = 200;
    if (0 != ret)
    {
        code = 404;
    }
    if (pu->m_message_cb)
    {
        pu->m_message_cb(rspXml, code, param->m_tid, pu->m_userdata);
    }
    pu->resetMessageCB();
    if (param)
    {
        if (param->m_thread)
        {
            delete param->m_thread;
            param->m_thread = NULL;
        }
        delete param;
        param = NULL;
    }
    return NULL;
}
