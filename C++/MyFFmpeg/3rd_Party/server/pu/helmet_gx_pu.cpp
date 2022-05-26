#include "helmet_gx_pu.h"
#include "helmet_interface.h"
#include "tls_error.h"
#include "j2x.h"
#include "frame.h"
#include "json/json.h"
#include "su.h"
#include "gu.h"


HelmetGXPu::HelmetGXPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType)
    : Pu(type, addrcode, name, chnNum, user, pswd, host)
{
    m_protocol = PROTO_TYPE_HELMET;
    m_port = port;
    m_domainType = domainType;
    m_chnOnlineStatus = 0;
    m_lUserID = NULL;
}


HelmetGXPu::~HelmetGXPu()
{
    
}


int HelmetGXPu::start()
{ 
    int s32Ret = -1;

    s32Ret = IC_Initialize();

    //IC_EnableConvertCharacter(1);
    

    if ((0 == s32Ret) && Pu::start() < 0)	
    {
        return -1;
    }
    
    m_init = true;

    return s32Ret;
}

void HelmetGXPu::stop()
{
    logout();
    Pu::stop();
}

int HelmetGXPu::login()
{
    if (m_login)
    {
        return  0;
    }


    int s32Ret = -1;

    string mmmm = m_ip + ":" + std::to_string(m_port);
    s32Ret = IC_Open(mmmm.c_str(), m_user.c_str(), m_pswd.c_str(), "system", &m_lUserID);
    if (0 != s32Ret)
    {
        return -1;
    }
    m_login = true;
   
    
#if 1
    //get resource
    int nCount = 500;
    IC_HANDLE puRsc[500] = { NULL };
    s32Ret = IC_ForkPUList(m_lUserID, 0, puRsc, &nCount);
    if (s32Ret != 0)
    {
        return 0;
    }

    AutoLock locker(m_chnHandle._mtx);
    HTCHN_ST* chst = NULL;
    for (int i = 0; i < nCount; i++)
    {
        IC_STR szName;
        IC_STR szPUID;
        s32Ret = IC_GetResourcePUID(puRsc[i], szPUID);
        s32Ret = IC_GetResourceName(puRsc[i], szName);
        if (0 == s32Ret)
        {	
#ifdef _WIN32
            string cname = szName;
#else
            string cname = sys->ascii2utf8(szName);
#endif

            m_chnHandle[cname] = puRsc[i];

            //chst = new HTCHN_ST;
            //chst->hand = puRsc[i];
            //memcpy(chst->name, szName, sizeof(szName));
            //memcpy(chst->puid, szPUID, sizeof(szPUID));
            IC_HANDLE subRes[100] = { NULL };
            int nResCount = 100;
            s32Ret = IC_ForkPUResource(puRsc[i], subRes, &nResCount);
            if (0 == s32Ret)
            {
                for (int index = 0; index < nResCount; index++)
                {
                    IC_STR szResName;
                    IC_STR szResType;
                    s32Ret = IC_GetResourceName(subRes[index], szResName);
                    s32Ret = IC_GetResourceType(subRes[index], szResType);
                    string tmpname = cname + "_" + szResName;
                    m_chnHandle[tmpname] = subRes[index];
                }
            } 
            int adfa = 0;
        }
    }
#endif 

#if 0
    RestartAllVoiceWhenOnline();
#endif

    //updateChannel();
    return 0;
}

void HelmetGXPu::logout()
{
    m_login = false;
    if (NULL != m_lUserID &&
        NULL != IC_Close)
    {
        IC_Close(m_lUserID);
        m_lUserID = NULL;

        AutoLock locker(m_chnHandle._mtx);
        map<string, IC_HANDLE>::iterator i;
        for (i = m_chnHandle.begin(); i != m_chnHandle.end(); ++i)
        {
            m_chnHandle[i->first] = NULL;
        }
    }	

    //StopAllVoiceWhenOffline();
}

bool
HelmetGXPu::validate(int channelNo)
{
	if (channelNo < 0 || channelNo > MAX_UT_CHANNEL_NUM)
    {
        return false;
    }

    return (m_chnOnlineStatus & ((uint64_t)0x1<<channelNo))?true:false;

#if 0
    m_updateChnLastTime = sys->currentTimeMillis();

    string cuame = guname(channelNo);

#ifdef _WIN32
    string cname = sys->utf2ascii(cuame.c_str());
#else
    string cname = cuame;
#endif


    AutoLock locker(m_chnHandle._mtx);
    map<string, IC_HANDLE>::const_iterator i = m_chnHandle.find(cname);
    if (i == m_chnHandle.end())
    {
        return false;
    }

    int nOnlineFlag = 0;    

    int s32Ret = IC_GetPUOnlineFlag(i->second, &nOnlineFlag);

    if (nOnlineFlag == 0)
    {
        //ERRE_MSV("HelmetGXPu device has no ip channel: %d", channelNo);
        return false;
    }
    else
    {
        return true;
    }
#endif
}

bool       
HelmetGXPu::validate(string gucode)
{
	int channelNo = GetChanNoByCode(gucode);
    return validate(channelNo);
}

IC_HANDLE
HelmetGXPu::getHandle(int channelNo, string key)
{
    string cuame = guname(channelNo);
#ifdef _WIN32
    string cname = sys->utf2ascii(cuame.c_str());
#else
    string cname = cuame;
#endif
    AutoLock locker(m_chnHandle._mtx);
    map<string, IC_HANDLE>::const_iterator i = m_chnHandle.find(cname);
    if (i == m_chnHandle.end())
    {
        return NULL;
    }

    if (i->second == NULL)
    {
        return NULL;
    }
    IC_HANDLE ret = NULL;
#if 0
    ret = m_chnHandle[channelNo]->subDev[key];
#else
    int iRet = IC_GetResourceHandle(i->second, key.c_str(), 0, &ret);
    if (iRet != 0)
    {
        return NULL;
    }
#endif

    return ret;
}

int
HelmetGXPu::ptzCtrl(int cmd, int channelNo, int speed, int preset, bool stop)
{
    HANDLE hresource = getHandle(channelNo, "PTZ");
    if (!hresource)
    {
        ERRE_MSV("getHandle PTZ failed");
        return -1;
    }

    IC_SetSpeed(hresource, speed);
    int ret = 0;
    switch (cmd)
    {
    case PTZ_CMD_UP:
        if (stop)
            IC_StopTurn(hresource);
        else
            IC_StartTurnUp(hresource, 0);
        break;
    case PTZ_CMD_DOWN:
        if (stop)
            IC_StopTurn(hresource);
        else
            IC_StartTurnDown(hresource, 0);
        break;
    case PTZ_CMD_LEFT:
        if (stop)
            IC_StopTurn(hresource);
        else
            IC_StartTurnLeft(hresource, 0);
        break;
    case PTZ_CMD_RIGHT:
        if (stop)
            IC_StopTurn(hresource);
        else
            IC_StartTurnRight(hresource, 0);
        break;
    case PTZ_CMD_ZOOMIN:
        if (stop)
            IC_StopPictureZoom(hresource);
        else
            IC_ZoomInPicture(hresource);
        break;
    case PTZ_CMD_ZOOMOUT:
        if (stop)
            IC_StopPictureZoom(hresource);
        else
            IC_ZoomOutPicture(hresource);
        break;
    case PTZ_CMD_FOCUSNEAR:
        if (stop)
            IC_StopFocusMove(hresource);
        else
            IC_MakeFocusNear(hresource);
        break;
    case PTZ_CMD_FOCUSFAR:
        if (stop)
            IC_StopFocusMove(hresource);
        else
            IC_MakeFocusFar(hresource);
        break;
    case PTZ_CMD_IRISOPEN:
        if (stop)
            IC_StopApertureZoom(hresource);
        else
            IC_AugmentAperture(hresource);
        break;
    case PTZ_CMD_IRISCLOSE:
        if (stop)
            IC_StopApertureZoom(hresource);
        else
            IC_MinishAperture(hresource);
        break;
    
    case PTZ_CMD_SETPRESET:
        IC_SetPresetPos(hresource, preset, to_string(preset).c_str());
        break;
    case PTZ_CMD_CALLPRESET:
        IC_MoveToPresetPos(hresource, preset);
        break;

    case PTZ_CMD_AUTOSCAN:
    case PTZ_CMD_RAIN_ON:
    case PTZ_CMD_RAIN_OFF:
    case PTZ_CMD_AUXLIGHT_ON:
    case PTZ_CMD_AUXLIGHT_OFF:
    case PTZ_CMD_UPLEFT:
    case PTZ_CMD_UPRIGHT:
    case PTZ_CMD_DOWNLEFT:
    case PTZ_CMD_DOWNRIGHT:
    case PTZ_CMD_CLEARPRESET:
    default:
        ERRE_MSV("not support direction");
        ret = -1;
        break;
    }
    return ret;
}

int
HelmetGXPu::ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop)
{
    int channelNo = guchannel(addrcode);
    return ptzCtrl(cmd, channelNo, speed, preset, stop);
}

int
HelmetGXPu::queryRecord(int channelNo, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json)
{
    SHL_Json::Value value;
    SHL_Json::Value array;

    struct tm stmTimeBegin;
    memset(&stmTimeBegin, 0, sizeof(stmTimeBegin));
    stmTimeBegin.tm_year = start.year - 1900;
    stmTimeBegin.tm_mon  = start.month - 1;
    stmTimeBegin.tm_mday = start.day;
    stmTimeBegin.tm_hour = start.hour;
    stmTimeBegin.tm_min  = start.minute;
    stmTimeBegin.tm_sec  = start.second;
    time_t tmTimeBegin   = mktime(&stmTimeBegin);

    struct tm stmTimeEnd;
    memset(&stmTimeEnd, 0, sizeof(stmTimeEnd));
    stmTimeEnd.tm_year = end.year - 1900;
    stmTimeEnd.tm_mon  = end.month - 1;
    stmTimeEnd.tm_mday = end.day;
    stmTimeEnd.tm_hour = end.hour;
    stmTimeEnd.tm_min  = end.minute;
    stmTimeEnd.tm_sec  = end.second;
    time_t tmTimeEnd   = mktime(&stmTimeEnd);



    IC_HANDLE haRes = getHandle(channelNo, "IV");
    if (haRes == NULL)
    {
        ERRE_MSV("HelmetGXPu getHandle VI failed, chn:%d", channelNo);
        return -1;
    }


    int nCount = 200;
    int ret = 0;
    IC_FILE_INFO *pFileInfo = new IC_FILE_INFO[nCount];
    memset(pFileInfo, 0, sizeof(IC_FILE_INFO) * nCount);
 

#if 1
    ret = IC_CSSQueryFile(m_lUserID, tmTimeBegin, tmTimeEnd, 0, NULL, 0, &haRes, 1, 0, pFileInfo, &nCount);
#else
    ret = IC_QueryFile(&haRes, 0, tmTimeBegin, tmTimeEnd, 0, 0, pFileInfo, &nCount);
#endif
    if (ret != 0)
    {
        delete[] pFileInfo;
        return -1;
    }
    
    int index = 0;
    for (index = 0; index < nCount; index++)
    {
        if (index + 1 < fromIndex || index + 1 > toIndex)
        {
            continue;
        }

        SHL_Json::Value record;

        record["filename"] = string(pFileInfo[index].szPath) + string(pFileInfo[index].szName) + string(":") + string(pFileInfo[index].szID);
        record["fileurl"] = string("//") + m_host + string(":") + std::to_string(m_port) + string("/") + string(pFileInfo[index].szName);
        record["channelNo"] = (int)channelNo;
        record["size"]      = pFileInfo[index].nSize;
        record["index"]     = index + 1;
        record["type"]      = 1;
  

        time_t tBegin = pFileInfo[index].un.record.nBeginTime;
        time_t tEnd = pFileInfo[index].un.record.nEndTime;
        struct tm *pFileBegin = localtime(&tBegin);
        struct tm *stFileEnd  = localtime(&tEnd);

        datetime_s start;
        datetime_s end;

        start.year = pFileBegin->tm_year + 1900;
        start.month = pFileBegin->tm_mon;
        start.day = pFileBegin->tm_mday;
        start.hour = pFileBegin->tm_hour;
        start.minute = pFileBegin->tm_min;
        start.second = pFileBegin->tm_sec;
        record["start"] = sys->strftime(start);

        end.year = stFileEnd->tm_year + 1900;
        end.month = stFileEnd->tm_mon;
        end.day = stFileEnd->tm_mday;
        end.hour = stFileEnd->tm_hour;
        end.minute = stFileEnd->tm_min;
        end.second = stFileEnd->tm_sec;
        record["end"] = sys->strftime(end);

        array.append(record);
    }

    value["records"] = array;
    value["realNum"] = nCount;
    value["fromIndex"] = fromIndex;
    value["toIndex"] = fromIndex + array.size();

    SHL_Json::StreamWriterBuilder writer;
    json = SHL_Json::writeString(writer, value);

    return 0;
}


int
HelmetGXPu::queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode)
{
	int channelNo = GetChanNoByCode(addrcode);
    int ret = queryRecord(channelNo, type, start, end, fromIndex, toIndex, json);
    if (m_clienQueryRecord_cb)
    {
		m_clienQueryRecord_cb(ret, this->protocol(), json, m_clientUserdata);
    }
    return ret;
}

int                
HelmetGXPu::parseEvent(const string& json)
{

    printf("\n\n\n\n\n%s\n\n\n\n\n", json.c_str());
    //containt channes;
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(json, rvalue, false))
    {
        ERRW_MSV("parseEvent, paser err");
        return -1;
    }

    string  Type = rvalue["M"]["@Type"].asString();

    if (Type != "Event")
        return 0;

    string e_id = rvalue["M"]["E"]["@ID"].asString();
    if (e_id == "E_PU_Online")
    {
        string guName = rvalue["M"]["E"]["Src"]["Res"]["Name"].asString();
        string cname = sys->utf2ascii(guName.c_str());

        int chnNo = guchannelFromName(guName);
        if (chnNo != -1)
        {

        }

        //Su* csu = getDualSu(chnNo);
        //if (csu)
        //{
        //    ERRW_MSV("start helmet hahahha:");
        //    csu->start();
        //}
        //Gu* cgu = getRealGu(chnNo);
        //if (cgu)
        //{
        //    ERRW_MSV("start helmet hahahha:");
        //    cgu->start();
        //}
    }
    else if (e_id == "E_PU_Offline")
    {        
        int chnNo = rvalue["M"]["E"]["Src"]["Res"]["Idx"].asInt();
        //Su* csu = getDualSu(chnNo);
        //if (csu)
        //{
        //    ERRW_MSV("stop helmet hahahha:");
        //    csu->stop();
        //}
        //Gu* cgu = getRealGu(chnNo);
        //if (cgu)
        //{
        //    ERRW_MSV("start helmet hahahha:");
        //    cgu->stop();
        //}
    }

    return 0;
}

int HelmetGXPu::updateChnStatus()
{
	int chnNum = channels() > MAX_UT_CHANNEL_NUM ? MAX_UT_CHANNEL_NUM : channels();

    AutoLock locker(m_chnHandle._mtx);
    for (int channelNo = 0; channelNo < chnNum; ++channelNo)
    {
        string cuame = guname(channelNo);
#ifdef _WIN32
        string cname = sys->utf2ascii(cuame.c_str());
#else
        string cname = cuame;
#endif
        map<string, IC_HANDLE>::const_iterator i = m_chnHandle.find(cname);
        if (i == m_chnHandle.end())
        {
            m_chnOnlineStatus &= ~((uint64_t)0x1<<channelNo);
            continue;
        }

        int nOnlineFlag = 0;
        IC_GetPUOnlineFlag(i->second, &nOnlineFlag);
        if (nOnlineFlag == 0)
        {
            m_chnOnlineStatus &= ~((uint64_t)0x1<<channelNo);
        }
        else
        {
            m_chnOnlineStatus |= (uint64_t)0x1<<channelNo;
        }
    }
    return 0;
}

void HelmetGXPu::onTimer()
{
    Pu::onTimer();
    
    if (!m_login)
    {
        return;
    }

    char *szEvent = NULL;

    int s32Ret = IC_RecvEventMessage(m_lUserID, &szEvent);
    if (0 == s32Ret)
    {
        //string utf8event = sys->ascii2utf8(szEvent);
        string json;
        xml2json(szEvent, json);
        parseEvent(json);
        IC_ReleaseMessage(m_lUserID, szEvent);
    }
    else if (s32Ret < 0)
    {
        m_first = false;
        logout();
    }
}


