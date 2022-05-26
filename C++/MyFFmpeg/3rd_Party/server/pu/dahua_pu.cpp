#include "dahua_pu.h"
#include "tls_error.h"
#include "triggerlistener.h"
#include "bitoperation.h"
#include "global_str_def.h"

#if defined(_WIN32) || (__cplusplus >= 201103L)
#include "json/json.h"
#else
#include "json-0.10.6/json.h"
#endif


#define MAX_CHANNEL_NUM (256)

#define DHRealcor  (8192)

tmap<long, Pu*> DahuaPu::m_userMap;

int CALLBACK PlateDataCallback(LLONG lAnalyzerHandle,
    DWORD             dwAlarmType,
    void             *pAlarmInfo,
    BYTE             *pBuffer,
    DWORD             dwBufSize,
    LDWORD            dwUser,
    int               nSequence,
    void             *reserved)
{
    return 0;
}

BOOL CALLBACK AlarmDataCallBack(
    LONG              lCommand,
    LLONG             lUserID,
    char             *pBuf,
    DWORD             dwBufLen,
    char             *ps8DVRIP,
    LONG              nDVRPort,
    LDWORD            dwUser)
{
    DahuaPu *pPu = (DahuaPu *)(DahuaPu::m_userMap[lUserID]);

    if (!pPu)
    {
        return FALSE;
    }


    if (!pPu->m_bNewVer)
    {
        return pPu->ReceiveMessage(lUserID, lCommand, ps8DVRIP, nDVRPort, pBuf, dwBufLen);
    }
    else
    {
        return pPu->ReceiveMessageEx(lUserID, lCommand, ps8DVRIP, nDVRPort, pBuf, dwBufLen);
    }

}


void CALLBACK DisConnectFunc(LLONG lLoginID, char *pchDVRIP, LONG nDVRPort, LDWORD dwUser)
{
    DahuaPu * pPu = (DahuaPu *)dwUser;
    
    if (pPu)
    {
        LOGI_MSV("Device: %s:%d connect failed user ID: %d", pPu->ip().c_str(),nDVRPort, lLoginID);

        pPu->m_login = false;

        pPu->writeDevLog(STR_MSV_OFF_LINE);
    }
    else
    {
        return;
    }

}


void CALLBACK AutoConnectFunc(LLONG lLoginID, char *pchDVRIP, LONG nDVRPort, LDWORD dwUser)
{
    DahuaPu *pPu = (DahuaPu *)DahuaPu::m_userMap[lLoginID];

    if (pPu)
    {
        LOGI_MSV("Device: %s:%d AutoConnect success! user ID: %d", pPu->ip().c_str(),nDVRPort, lLoginID);

        pPu->m_login = true;

        pPu->writeDevLog(STR_MSV_RE_ON_LINE);

    }
    else
    {
        return;
    }
    
}





DahuaPu::DahuaPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType,int linktype)
    : Pu(type, addrcode, name, chnNum, user, pswd, host,linktype)
{
    m_protocol   = PROTO_TYPE_DAHUA;
    m_port       = port;
    m_domainType = domainType;
    m_userId     = -1;
    m_lUserSubID = -1;
	m_startmeasure = false;
	m_lastpts = 0;
}

DahuaPu::~DahuaPu()
{
}

int
DahuaPu::start()
{
    if (Pu::start() < 0)
    {
        return -1;
    }

    //chenjiaqi where uninitail
    BOOL res = CLIENT_Init(DisConnectFunc, (LDWORD)this);
    if (FALSE == res)
    {
        LOGI_MSV("Dahua CLIENT_Init return false");
        return -1;
    }

    CLIENT_SetConnectTime(3000, 10);
    CLIENT_SetAutoReconnect(AutoConnectFunc, (LDWORD)this);
    
    m_init = true;
    return 0;
}



void
DahuaPu::stop()
{
    m_init = false;

    logout();

    Pu::stop();
}


int
DahuaPu::login()
{ 
    if (m_login)
        return FALSE;

    NET_PARAM netParam = { 0 };
    netParam.nWaittime            = 3000;
    netParam.nConnectTime         = 1500;
    netParam.nConnectTryNum       = 10;
    netParam.nSubConnectSpaceTime = 10;
    netParam.nPicBufSize          = 4 * 1024 * 1024;

    CLIENT_SetNetworkParam(&netParam);

    char           s8DvrIp[MAX_IPV4_STR_LEN] = { 0 };
    unsigned short u32Port = 0;
    
    if (DOMAIN_PUBLIC == m_domainType)
    {

    }
    else
    {
        strncpy(s8DvrIp, m_ip.c_str(), MAX_IPV4_STR_LEN);
        u32Port = m_port;
    }


    NET_IN_LOGIN_WITH_HIGHLEVEL_SECURITY stInparam;
    memset(&stInparam, 0, sizeof(stInparam));
    stInparam.dwSize = sizeof(stInparam);
    strncpy(stInparam.szIP, m_ip.c_str(), m_ip.size());
    strncpy(stInparam.szUserName, m_user.c_str(), m_user.size());
    strncpy(stInparam.szPassword, m_pswd.c_str(), m_pswd.size());
    stInparam.nPort = m_port    ;
    stInparam.emSpecCap = EM_LOGIN_SPEC_CAP_TCP;
    NET_OUT_LOGIN_WITH_HIGHLEVEL_SECURITY stOutparam;
    memset(&stOutparam, 0, sizeof(stOutparam));
    stOutparam.dwSize = sizeof(stOutparam);

    //注册用户到设备
    m_userId = CLIENT_LoginWithHighLevelSecurity(&stInparam, &stOutparam);
    if (m_userId == 0)
    {
		int res = CLIENT_GetLastError();
        ERRE_MSV("dahua pu login failed\n");
        return -1;
    }

    //chenjiaqi
    CLIENT_SetDVRMessCallBack(AlarmDataCallBack, (LDWORD)this);
    CLIENT_StartListenEx(m_userId);

    {
        AutoLock lock(m_userMap._mtx);
        m_userMap[m_userId] = this;
    }
    LOGI_MSV("Login success! user ID = %lld\n", m_userId);
    m_login = true;

    //int s32ProtoVer;
    //int s32Ret;

    //BOOL bSuccess = CLIENT_QueryDevState(m_userId, DH_DEVSTATE_PROTOCAL_VER, (char*)&s32ProtoVer, sizeof(int), &s32Ret, 1000);
    //if (bSuccess)
    //{
    //    if (s32ProtoVer >= 5)		//Current device is V2.6 baseline or higher.
    //    {
    //        m_bNewVer = true;
    //    }
    //    else
    //    {
    //        m_bNewVer = false;
    //    }
    //}

    //User old interface if device version is below 2.6.
    //If device version is 2.6 or higher then use new interface.
    //if (!m_bNewVer)
    //{
    //    CLIENT_StartListen(m_userId);
    //}
    //else 
    //{
    //    CLIENT_StartListenEx(m_userId);
    //}
    
    //m_lPlateHandle = CLIENT_RealLoadPictureEx(m_userId, 0, EVENT_IVS_ALL, TRUE, PlateDataCallback, (unsigned long)this, NULL);



    //m_lUserSubID = CLIENT_LoginEx2(s8DvrIp,
    //    u32Port,
    //    m_user.c_str(),
    //    m_pswd.c_str(),
    //    EM_LOGIN_SPEC_CAP_TCP,
    //    NULL,
    //    &m_m_devinfo,
    //    &iErr);
    //if (m_lUserSubID <= 0 && iErr != 0)
    //{
    //    LOGI_MSV("sub login, Dahua ip:%s, port:%d, user:%s, password: %s, [ CLIENT_LoginEx ]failed , errorCode = %d\n",
    //             m_ip.c_str(), u32Port, m_user.c_str(), m_pswd.c_str(), iErr);
    //    return FALSE;
    //}

    writeDevLog(STR_MSV_ON_LINE);

    return TRUE;
    
}

void
DahuaPu::logout()
{
    do 
    {
        if (m_userId == -1)
        {
            break;
        }

        {
            CLIENT_Logout(m_userId);
            AutoLock lock(m_userMap._mtx);
            map<long, Pu *>::iterator i = m_userMap.find(m_userId);
            if (i != m_userMap.end())
            {
                m_userMap.erase(i);
            }
        }
        m_userId = -1;

        //if (-1 != m_lUserSubID)
        //{
        //    CLIENT_Logout(m_lUserSubID);
        //    m_lUserSubID = -1;
        //}
        
        

    } while (0);

    if (m_login)
    {
        m_login = false;
        writeDevLog(STR_MSV_OFF_LINE);
    }

    return;
}

bool
DahuaPu::validate(int channelNo)
{
    return true;
    //DH_DEVSTATE_ONLINE
    //CLIENT_QueryRemotDevState

    //int nRetLen     = 0;
    //int chnCount    = 0;
    ////int realChannNo = channelNo + 1;

    ////NET_DEV_CHN_COUNT_INFO stuChn = { sizeof(NET_DEV_CHN_COUNT_INFO) };
    ////stuChn.stuVideoIn.dwSize  = sizeof(stuChn.stuVideoIn);
    ////stuChn.stuVideoOut.dwSize = sizeof(stuChn.stuVideoOut);

    ////if (CLIENT_QueryDevState(m_userId, DH_DEVSTATE_DEV_CHN_COUNT, (char*)&stuChn, stuChn.dwSize, &nRetLen))
    ////{
    ////    chnCount = stuChn.stuVideoIn.nCurLocal;       
    ////}
    ////else
    ////{
    ////    
    ////}

    ////chnCount = m_m_devinfo.nChanNum;

    ////if (realChannNo <= 0 || realChannNo > chnCount)
    ////{
    ////    LOGI_MSV("Dahua validate channelNo");
    ////    return false;
    ////}

    //NET_IN_GET_CAMERA_STATEINFO  inStateInfo = {0};

    //inStateInfo.dwSize      = sizeof(NET_IN_GET_CAMERA_STATEINFO);
    //inStateInfo.bGetAllFlag = FALSE;
    //inStateInfo.nValidNum   = MAX_CHANNEL_NUM;
    //for (int i = 0; i < MAX_CHANNEL_NUM; ++i)
    //{
    //    inStateInfo.nChannels[i] = i;
    //}

    //NET_CAMERA_STATE_INFO    arrStateInfo[MAX_CHANNEL_NUM] = {0};
    //NET_OUT_GET_CAMERA_STATEINFO outStateInfo = {0};
    //
    //outStateInfo.dwSize           = sizeof(NET_OUT_GET_CAMERA_STATEINFO);
    //outStateInfo.nMaxNum          = MAX_CHANNEL_NUM;
    //outStateInfo.pCameraStateInfo = arrStateInfo;
    //
    //CLIENT_QueryDevInfo(m_userId, NET_QUERY_GET_CAMERA_STATE, &inStateInfo, &outStateInfo);

    //chnCount = outStateInfo.nValidNum;
    //if (chnCount > MAX_CHANNEL_NUM || chnCount < 0)
    //{
    //    return false;
    //}
    //else
    //{
    //    for (int i = 0; i < chnCount; ++i)
    //    {
    //        int chn = outStateInfo.pCameraStateInfo[i].nChannel;

    //        if (-1 == chn)
    //        {
    //            continue;
    //        }
    //        
    //        if (chn == channelNo)
    //        {
    //            if (EM_CAMERA_STATE_TYPE_CONNECTED == outStateInfo.pCameraStateInfo->emConnectionState)
    //            {
    //                return true;
    //            }
    //            else
    //            {
    //                LOGI_MSV("dahua channel:%d statecode: %d", channelNo, outStateInfo.pCameraStateInfo->emConnectionState);
    //                return false;
    //            }
    //        }

    //    }
    //}

    //return false;
}

int 
DahuaPu::onCustomCommand(string& json)
{

	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	SHL_Json::Value  element;
	int ret = -1;

	if (!reader.parse(json, rvalue, false))
	{
		ERRE_MSV("SHL_Json parse error");
		return ret;
	}
	string jMethod = rvalue["Method"].asString();
	if (!rvalue.isMember("Method"))
	{
		ERRE_MSV("json error:no member \"Method\"");
		return ret;
	}
	if ("setThermometryPresetInfo" == jMethod)
	{
		return SetRuleConfig(json);
	}
	else if("stopmeasuretemp" == jMethod)//与海康测温保持一致
	{
		int handle = rvalue["handle"].asInt64();
		int presetNo = 0;
		AutoLock locker(temprule_s_mtx);
		vector<temprule_s>::iterator iter;
		if (handle != 0)
		{
			for (iter = m_ruleid.begin(); iter != m_ruleid.end(); iter++)
			{
				if (iter->ruleId == handle)
				{
					m_ruleid.erase(iter);
					break;
				}
			}

			if (m_ruleid.size() == 0)
			{
				m_startmeasure = false;
			}
		}
		else
		{
			for (iter = m_ruleid.begin(); iter != m_ruleid.end();)
			{
				if (iter->presetNo == presetNo)
				{
					iter = m_ruleid.erase(iter);
				}
				else
				{
					iter++;
				}
				if (m_ruleid.size() == 0)
				{
					m_startmeasure = false;
				}
			}
		}
		return 0;
	}

}

 int       
DahuaPu::onRequestConfig(string & jin, string& jout)
 {

	 SHL_Json::Reader reader;
	 SHL_Json::Value  rvalue;
	 SHL_Json::Value  element;
	 int ret = -1;

	 if (!reader.parse(jin, rvalue, false))
	 {
		 ERRE_MSV("SHL_Json parse error");
		 return ret;
	 }
	 string jMethod = rvalue["Method"].asString();
	 if (!rvalue.isMember("Method"))
	 {
		 ERRE_MSV("json error:no member \"Method\"");
		 return ret;
	 }
	 if ("getPointTemp" == jMethod)
	 {
		 int channelNo = rvalue["channelNo"].asInt();
		 int nX = int(rvalue["X"].asFloat()*DHRealcor);
		 int nY = int(rvalue["Y"].asInt()*DHRealcor);
		 int type = rvalue["type"].asInt();
		 ret = OnQuerypointinfo(channelNo, nX, nY, type, jout);
		 return ret;
	 }
	 else if ("getmeasuretemp" == jMethod)
	 {

		 int channelNo = rvalue["chnNo"].asInt();
		 int PresetID = 0;
		 int RuleID = rvalue["ruleId"].asInt();

		 ret = OnQueryiteminfo(RuleID,channelNo, PresetID,jout);
		 return ret;
	 }
	 else if ("startmeasuretemp" == jMethod) //与海康测温保持一致
	 {
		 
		int chnNo = rvalue["chnNo"].asInt();
		int ruleId = rvalue["ruleId"].asInt();
		int presetNo = 0;
		return startmeasuretemp(ruleId, chnNo, presetNo, jout);
	 }
 }


bool
DahuaPu::validate(string gucode)
{
    return true;

    //int channelNo = guchannel(gucode);

    //return validate(channelNo);
}

int
DahuaPu::ptzCtrl(int cmd, int channelNo, int speed, int preset, bool stop)
{
    BOOL res = TRUE;

    if (!m_login)
    {
        ERRE_MSV("device %s:%d is not on-line.", m_host.c_str(), m_port);
        return -1;
    }

    int ptzCmd = -1;
    switch (cmd)
    {
    case PTZ_CMD_UP:
        ptzCmd = DH_PTZ_UP_CONTROL;
        break;
    case PTZ_CMD_DOWN:
        ptzCmd = DH_PTZ_DOWN_CONTROL;
        break;
    case PTZ_CMD_LEFT:
        ptzCmd = DH_PTZ_LEFT_CONTROL;
        break;
    case PTZ_CMD_RIGHT:
        ptzCmd = DH_PTZ_RIGHT_CONTROL;
        break;
    case PTZ_CMD_UPLEFT:
        ptzCmd = DH_EXTPTZ_LEFTTOP;
        break;
    case PTZ_CMD_UPRIGHT:
        ptzCmd = DH_EXTPTZ_RIGHTTOP;
        break;
    case PTZ_CMD_DOWNLEFT:
        ptzCmd = DH_EXTPTZ_LEFTDOWN;
        break;
    case PTZ_CMD_DOWNRIGHT:
        ptzCmd = DH_EXTPTZ_RIGHTDOWN;
        break;
    case PTZ_CMD_ZOOMIN:
        ptzCmd = DH_PTZ_ZOOM_ADD_CONTROL;
        break;
    case PTZ_CMD_ZOOMOUT:
        ptzCmd = DH_PTZ_ZOOM_DEC_CONTROL;
        break;
    case PTZ_CMD_FOCUSNEAR:
        ptzCmd = DH_PTZ_FOCUS_DEC_CONTROL;
        break;
    case PTZ_CMD_FOCUSFAR:
        ptzCmd = DH_PTZ_FOCUS_ADD_CONTROL;
        break;
    case PTZ_CMD_IRISOPEN:
        ptzCmd = DH_PTZ_APERTURE_ADD_CONTROL;
        break;
    case PTZ_CMD_IRISCLOSE:
        ptzCmd = DH_PTZ_APERTURE_DEC_CONTROL;
        break;
    case PTZ_CMD_AUTOSCAN:
        ptzCmd = DH_EXTPTZ_STARTLINESCAN;
        break;

    case PTZ_CMD_SETPRESET:
        
        res = CLIENT_DHPTZControl(m_userId, channelNo , DH_PTZ_POINT_SET_CONTROL, 0, preset,0, FALSE);
        return (res == TRUE ? 0 : -1);

    case PTZ_CMD_CLEARPRESET:
        res = CLIENT_DHPTZControl(m_userId, channelNo , DH_PTZ_POINT_DEL_CONTROL, 0, preset,0, FALSE);
        return (res == TRUE ? 0 : -1);

    case PTZ_CMD_CALLPRESET:
        res = CLIENT_DHPTZControl(m_userId, channelNo , DH_PTZ_POINT_MOVE_CONTROL, 0, preset, 0, FALSE);
        return (res == TRUE ? 0 : -1);

    case PTZ_CMD_RAIN_ON:
        res = CLIENT_DHPTZControl(m_userId, channelNo , DH_PTZ_LAMP_CONTROL, 1, 0, 0, FALSE);
        return (res == TRUE ? 0 : -1);

    case PTZ_CMD_RAIN_OFF:
        res = CLIENT_DHPTZControl(m_userId, channelNo , DH_PTZ_LAMP_CONTROL, 0, 0, 0, FALSE);
        return (res == TRUE ? 0 : -1);
    }

    float rate = 8 / (float)10;
    int actualSpeed = (int)(speed * rate + 0.50);

    if (actualSpeed < 0 || actualSpeed > 7)
    {
        actualSpeed = 5;
    }

    res = CLIENT_DHPTZControl(m_userId, channelNo, ptzCmd, 0,actualSpeed,0,stop );

    LOGI_MSV("channelNo: %d, cmd: %d, actualSpeed: %d, preset: %d, stop: %d, result: %d", channelNo, ptzCmd, actualSpeed, preset, stop, res);

    return (res == TRUE ? 0 : -1);

}

int
DahuaPu::ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop)
{
	int  channelNo = Pu::GetChanNoByCode(addrcode);

    return ptzCtrl(cmd, channelNo, speed, preset, stop);
}

int
DahuaPu::queryRecord(int channelNo, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json)
{
    SHL_Json::Value value;
    SHL_Json::Value array;

    NET_TIME startTM = { 0 };
    startTM.dwYear   = start.year;
    startTM.dwMonth  = start.month;
    startTM.dwDay    = start.day;
    startTM.dwHour   = start.hour;
    startTM.dwMinute = start.minute;
    startTM.dwSecond = start.second;

    NET_TIME endTM = { 0 };
    endTM.dwYear   = end.year;
    endTM.dwMonth  = end.month;
    endTM.dwDay    = end.day;
    endTM.dwHour   = end.hour;
    endTM.dwMinute = end.minute;
    endTM.dwSecond = end.second;

    DWORD dwstream = 0;
    CLIENT_SetDeviceMode(m_userId, DH_RECORD_STREAM_TYPE, &dwstream);

    LLONG recordHand = CLIENT_FindFile(m_userId, channelNo, 0, NULL, &startTM, &endTM, false, 2000);

    if (0 == recordHand)
    {
        ERRE_MSV("CLIENT_FindFile erro");
        return -1;
    }

    int index = 0;
    while (true)
    {
        NET_RECORDFILE_INFO recordInfo = {0};
        
        int res = CLIENT_FindNextFile(recordHand, &recordInfo);

        if (1 == res)
        {
            index++;

            if (index < fromIndex || index > toIndex)
            {
                continue;
            }

            char filename[128] = { 0 };

            snprintf(filename, 
                128,
                "ch%02d-%04d%02d%02d-%02d%02d%02d-%04d%02d%02d-%02d%02d%02d.dav",
                recordInfo.ch,
                recordInfo.starttime.dwYear,
                recordInfo.starttime.dwMonth,
                recordInfo.starttime.dwDay,
                recordInfo.starttime.dwHour,
                recordInfo.starttime.dwMinute,
                recordInfo.starttime.dwSecond,
                recordInfo.endtime.dwYear,
                recordInfo.endtime.dwMonth,
                recordInfo.endtime.dwDay,
                recordInfo.endtime.dwHour,
                recordInfo.endtime.dwMinute,
                recordInfo.endtime.dwSecond
                );

           ERRE_MSV("filename: %s, real: %s ", filename, recordInfo.filename);

            SHL_Json::Value record;
            record["filename"]  = filename;
            record["fileurl"]   = string("//") + m_host + string(":") + std::to_string(m_port) + string("/") + string(filename);
            record["channelNo"] = (int)channelNo;
            record["size"]      = recordInfo.size * 1024;
            record["index"]     = index;

            /** record["type"] < 0-ËùÓÐÂ¼Ïñ 1-¼Æ»®Â¼Ïñ 2 ÊÖ¶¯Â¼Ïñ 3-±¨¾¯Â¼Ïñ */

            if (1 == recordInfo.bRecType)
            {
                record["type"] = 3;
            }
            else if (0 == recordInfo.bRecType)
            {
                record["type"] = 1;
            }
            else
            {
                record["type"] = 0;
            }

            
            datetime_s start;
            datetime_s end;

            start.year   = recordInfo.starttime.dwYear;
            start.month  = recordInfo.starttime.dwMonth;
            start.day    = recordInfo.starttime.dwDay;
            start.hour   = recordInfo.starttime.dwHour;
            start.minute = recordInfo.starttime.dwMinute;
            start.second = recordInfo.starttime.dwSecond;

            record["start"] = sys->strftime(start);

            end.year   = recordInfo.endtime.dwYear;
            end.month  = recordInfo.endtime.dwMonth;
            end.day    = recordInfo.endtime.dwDay;
            end.hour   = recordInfo.endtime.dwHour;
            end.minute = recordInfo.endtime.dwMinute;
            end.second = recordInfo.endtime.dwSecond;

            record["end"] = sys->strftime(end);
            array.append(record);


        }
        else if (-1 == res)
        {
            ERRE_MSV("CLIENT_FindNextFile error");
            return -1;
        }
        else if (0 == res)
        {
            break;
        }

    }

    CLIENT_FindCloseEx(recordHand);
    
    value["records"]   = array;
    value["realNum"]   = index;
    value["fromIndex"] = fromIndex;
    value["toIndex"]   = fromIndex + array.size();

    SHL_Json::StreamWriterBuilder writer;
    json = SHL_Json::writeString(writer, value);

    return 0;
}



int
DahuaPu::queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode)
{
	int channelNo = Pu::GetChanNoByCode(addrcode);
    int ret = queryRecord(channelNo, type, start, end, fromIndex, toIndex, json);

    if (m_clienQueryRecord_cb)
    {
        m_clienQueryRecord_cb(ret, this->protocol(), json, m_clientUserdata);
    }
    return ret;
}

int
DahuaPu::syncTime(datetime_s &dt, const string &tzone)
{
    if (!m_login)
    {
        return -1;
    }

    NET_TIME stDvrTime =
    {
        dt.year,
        dt.month,
        dt.day,
        dt.hour,
        dt.minute,
        dt.second
    };

    if (!CLIENT_SetupDeviceTime(m_userId, &stDvrTime))
    {
        return -1;
    }
    //write log
        
    return 0;
}
void
DahuaPu::onTimer()
{
	uint64_t pts = sys->currentTimeMillis();
	if (m_startmeasure == true)
	{
		if (pts - m_lastpts >= 3 * 1000)
		{
			m_lastpts = pts;
			SHL_Json::Value      revalue;
			SHL_Json::FastWriter writer;
			SHL_Json::Reader reader;
			string jsonIn,jsonOut;
			vector<temprule_s>::const_iterator iter;
			SHL_Json::Value      value;
			SHL_Json::Value      array;

			revalue["version"] = "1.0";
			revalue["time"] = sys->currentTime();
			revalue["deviceName"] = this->name();
			revalue["deviceHost"] = this->host();
			revalue["devicePort"] = this->port();
			revalue["addrcode"] = this->addrcode();
			revalue["level"] = 1;
			AutoLock locker(temprule_s_mtx);
			for (iter = m_ruleid.cbegin(); iter != m_ruleid.cend(); iter++)
			{
				value["channelNo"] = iter->chnNo;
				value["presetNo"] = 1;
				value["ruleID"] = iter->ruleId;
				jsonIn = writer.write(value);
				int res = OnQueryiteminfo(iter->ruleId, iter->chnNo, iter->presetNo, jsonOut, iter->ruleCalibType);
				SHL_Json::Value itemAlarm;
				if (res == 0)
				{
					SHL_Json::Value      tmpvalue;
					if (!reader.parse(jsonOut, tmpvalue, false))
					{
						ERRE_MSV("SHL_Json parse error");
					}
					else
					{
						itemAlarm["type"] = TriggerListener::TR_AI_THERMOMETRY;
						itemAlarm["ruleCalibType"] = tmpvalue["ruleCalibType"];
						itemAlarm["ruleID"] = iter->ruleId;
						itemAlarm["preset"] = 1;
						itemAlarm["channelNo"] = iter->chnNo;
						if (tmpvalue["ruleCalibType"].asInt() == 1)
						{
							itemAlarm["Maxtemp"] = tmpvalue["pointTemp"];
						}
						else
						{
							itemAlarm["Maxtemp"] = tmpvalue["polygonMaxTemp"];
						}
						revalue["alarms"].append(itemAlarm);
						string json = writer.write(revalue);
						this->notifyAlarm(json, NULL, 0);
						//this->writeAlarmLog(json);
					}

				}
			}
		}
	}
    Pu::onTimer();


}


BOOL DahuaPu::ReceiveMessageEx(LLONG lLoginID, LONG lCommand, char *pchDVRIP, LONG nDVRPort, char *pBuf, DWORD dwBufLen)
{
    //return TRUE;

    DahuaPu *pPu = (DahuaPu *)DahuaPu::m_userMap[lLoginID];

    if (!pPu)
    {
        return FALSE;
    }


    BYTE  dwAlarm[32] = { 0 };
    DWORD min         = dwBufLen < 32 ? dwBufLen : 32;

    SHL_Json::FastWriter writer;
    SHL_Json::Value      value;

    value["version"]    = "1.0";
    value["time"]       = sys->currentTime();
    value["deviceName"] = pPu->name();
    value["deviceHost"] = pPu->host();
    value["devicePort"] = pPu->port();
    value["addrcode"]   = pPu->addrcode();
    value["level"]      = 1;

    int chanNum = 0;

    switch (lCommand)
    {
    case DH_ALARM_ALARM_EX:
    {        
        memcpy(&dwAlarm, pBuf, min);

        for (chanNum = 0; chanNum < min; ++chanNum)
        {
            if (dwAlarm[chanNum])
            {
                SHL_Json::Value item;
                item["type"]    = TriggerListener::TR_DEV_IO;
                item["channelNo"] = chanNum;
                item["status"]  = 1;
                item["gucode"]  = pPu->gucode(chanNum);
                item["name"]    = pPu->guname(chanNum);
                
                value["alarms"].append(item);
            }
        }
    }
        break;
    case DH_MOTION_ALARM_EX:
    {
        memcpy(&dwAlarm, pBuf, min);

        for (chanNum = 0; chanNum < min; ++chanNum)
        {
            if (dwAlarm[chanNum])
            {
                SHL_Json::Value item;
                item["type"]    = TriggerListener::TR_DEV_MD;
                item["channelNo"] = chanNum;
                item["status"]  = 1;
                item["gucode"]  = pPu->gucode(chanNum);
                item["name"]    = pPu->guname(chanNum);

                value["alarms"].append(item);
            }
        }
    }
        break;
    case DH_VIDEOLOST_ALARM_EX:
    {
        memcpy(&dwAlarm, pBuf, min);

        for (chanNum = 0; chanNum < min; ++chanNum)
        {
            if (dwAlarm[chanNum])
            {
                SHL_Json::Value item;
                item["type"]    = TriggerListener::TR_DEV_VLOSS;
                item["channelNo"] = chanNum;
                item["status"]  = 1;
                item["gucode"]  = pPu->gucode(chanNum);
                item["name"]    = pPu->guname(chanNum);

                value["alarms"].append(item);
            }
        }
    }
        break;
    case DH_DISKFULL_ALARM_EX:
    {
        memcpy(&dwAlarm, pBuf, min);

        for (chanNum = 0; chanNum < min; ++chanNum)
        {
            if (dwAlarm[chanNum])
            {
                SHL_Json::Value item;
                item["type"]    = TriggerListener::TR_DEV_HDD;
                item["channelNo"] = chanNum;
                item["status"]  = 1;
                item["gucode"]  = pPu->gucode(chanNum);
                item["name"]    = pPu->guname(chanNum);

                value["alarms"].append(item);
            }
        }
    }
        break;
	case DH_ALARM_TEMPERATURE:
	{
		if (dwBufLen != sizeof(DWORD))
		{
			return FALSE;
		}
		ALARM_TEMPERATURE_INFO* tempalarm = (ALARM_TEMPERATURE_INFO *)pBuf;
		if (tempalarm->dwSize != 0)
		{
			SHL_Json::Value item;
			item["type"] = TriggerListener::TR_THERMOMETRY_ALARM;
			item["status"] = "1";//告警为1
			item["temperature"] = tempalarm->fTemperature;
			item["channel"] = tempalarm->nChannelID;
			item["gucode"] = pPu->gucode(tempalarm->nChannelID);
			item["name"] = pPu->guname(tempalarm->nChannelID);
		}

	}
		break;
    default:
        break;
    }

    //notify client
    string json      = writer.write(value);
    pPu->notifyAlarm(json, NULL, 0);
    pPu->writeAlarmLog(json);

    return TRUE;
}


BOOL  DahuaPu::ReceiveMessage(LLONG lLoginID, LONG lCommand, char *pchDVRIP, LONG nDVRPort, char *pBuf, DWORD dwBufLen)
{
    //return TRUE;

    DahuaPu *pPu = (DahuaPu *)DahuaPu::m_userMap[lLoginID];

    if (!pPu)
    {
        return FALSE;
    }

    SHL_Json::FastWriter writer;
    SHL_Json::Value      value;


    value["version"]       = "1.0";
    value["time"]          = sys->currentTime();
    value["deviceName"]    = pPu->name();
    value["deviceHost"]    = pPu->host();
    value["devicePort"]    = pPu->port();
    value["addrcode"]      = pPu->addrcode();
    value["level"]         = 1;

    int chanNum = 0;

    switch (lCommand)
    {
    case DH_COMM_ALARM:
    {
        NET_CLIENT_STATE *pState = (NET_CLIENT_STATE *)pBuf;
        if (dwBufLen != sizeof(NET_CLIENT_STATE) || pState == NULL)
        {
            return FALSE;
        }

        //External alarm
        for (chanNum = 0; chanNum < pState->alarminputcount; ++chanNum)
        {
            if (pState->alarm[chanNum])
            {
                SHL_Json::Value item;
                item["type"]    = TriggerListener::TR_DEV_IO;
                item["channelNo"] = chanNum;
                item["status"]  = 1;
                item["gucode"]  = pPu->gucode(chanNum);
                item["name"]    = pPu->guname(chanNum);

                value["alarms"].append(item);
            }
        }

        //Video loss 
        for (chanNum = 0; chanNum < pState->channelcount; ++chanNum)
        {
            if (pState->videolost[chanNum])
            {
                SHL_Json::Value item;
                item["type"]    = TriggerListener::TR_DEV_VLOSS;
                item["channelNo"] = chanNum;
                item["status"]  = 1;
                item["gucode"]    = pPu->gucode(chanNum);
                item["name"]    = pPu->guname(chanNum);

                value["alarms"].append(item);
            }
        }

        //Motion detection
        for (chanNum = 0; chanNum < pState->channelcount; ++chanNum)
        {
            if (pState->motiondection[chanNum])
            {
                SHL_Json::Value item;
                item["type"]    = TriggerListener::TR_DEV_MD;
                item["channelNo"] = chanNum;
                item["status"]  = 1;
                item["gucode"]    = pPu->gucode(chanNum);
                item["name"]    = pPu->guname(chanNum);

                value["alarms"].append(item);
            }
        }
    }
        break;
    case DH_DISK_FULL_ALARM:
    {
        if (dwBufLen != sizeof(DWORD))
        {
            return FALSE;
        }

        DWORD dwDiskFull = *(DWORD*)pBuf;
        if (1 == dwDiskFull)
        {
            SHL_Json::Value item;
            item["type"]    = TriggerListener::TR_DEV_HDD;
            item["channelNo"] = 0;
            item["status"]  = 1;
            item["gucode"]    = pPu->gucode(chanNum);
            item["name"]    = pPu->guname(chanNum);

            value["alarms"].append(item);
        }
    }
        break;
    default:
        break;
    }

    //notify client
    string json = writer.write(value);
    pPu->notifyAlarm(json, NULL, 0);
    pPu->writeAlarmLog(json);

    return TRUE;
}

int    DahuaPu::SetRuleConfig(string &jsonin)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	SHL_Json::Value  element;


	if (!reader.parse(jsonin, rvalue, false))
	{
		ERRE_MSV("SHL_Json parse error");
		return -1;
	}
	int RuleAmount  = rvalue["RuleAmount"].asInt(); 
	int presetNo    = 0;
	int chnNo       = rvalue["chnNo"].asInt();
	int ruleId      = rvalue["ruleId"].asInt();
	int enable      = rvalue["enable"].asInt();
	string ruleName = rvalue["ruleName"].asString();//max len 128
	float emissivity = rvalue["emissivity"].asFloat();
	float reflex = rvalue["reflex"].asFloat();
	int distance = rvalue["distance"].asInt();
	int pointCount = rvalue["pointList"].size();
	int ruleCalibType = 0;
	if (!rvalue["ruleCalibType"].isNull())
	{
		 ruleCalibType = rvalue["ruleCalibType"].asInt();
	}
	else
	{
		if(pointCount == 1)
		{
			ruleCalibType = NET_RADIOMETRY_METERTYPE::NET_RADIOMETRY_METERTYPE_SPOT;
		}
		else if (pointCount == 2)
		{
			ruleCalibType = NET_RADIOMETRY_METERTYPE::NET_RADIOMETRY_METERTYPE_LINE;
		}
		else if (pointCount > 2)
		{
			ruleCalibType = NET_RADIOMETRY_METERTYPE::NET_RADIOMETRY_METERTYPE_AREA;
		}
		else
		{
			ruleCalibType = NET_RADIOMETRY_METERTYPE::NET_RADIOMETRY_METERTYPE_UNKNOWN;
		}
	}
	int Period = 3;
	int LocalParameters = 1;

	//GetRuleConfig(ruleId, 0, 0, chnNo);
	CFG_RADIOMETRY_RULE_INFO *stuRuleOutInfo = new CFG_RADIOMETRY_RULE_INFO;
	if (NULL == stuRuleOutInfo)
	{
		return -1;
	}
	memset(stuRuleOutInfo, 0, sizeof(CFG_RADIOMETRY_RULE_INFO));
	char szJsonBufs[40 * 512] = { 0 };
	int nerror = 0;
	BOOL bRets = FALSE;
	bRets = CLIENT_GetNewDevConfig(m_userId, CFG_CMD_THERMOMETRY_RULE, chnNo, szJsonBufs, 40 * 512, &nerror, 3000);
	if (bRets)
	{
		DWORD dwRetLen = 0;
		bRets = CLIENT_ParseData(CFG_CMD_THERMOMETRY_RULE, szJsonBufs, (char*)stuRuleOutInfo, sizeof(CFG_RADIOMETRY_RULE_INFO), &dwRetLen);
	}
	bool bEnable = (enable == 1) ? true : false;

	int m_nRuleAmount = stuRuleOutInfo->nCount;

	if (bEnable == true)
	{
		if (stuRuleOutInfo->nCount <= ruleId - 1)
		{
			stuRuleOutInfo->nCount = m_nRuleAmount + 1;
		}
		int s32Index = ruleId - 1;

		bool bLocalParameters = (LocalParameters == 1) ? true : false;
		stuRuleOutInfo->stRule[s32Index].bEnable = bEnable;
		stuRuleOutInfo->stRule[s32Index].nPresetId = 0;
		stuRuleOutInfo->stRule[s32Index].nRuleId = ruleId;
#ifdef _WIN32
		strncpy(stuRuleOutInfo->stRule[s32Index].szName, (LPCTSTR)ruleName.c_str(), sizeof(stuRuleOutInfo->stRule[0].szName) - 1); //规则名称
#else
		strncpy(stuRuleOutInfo->stRule[s32Index].szName, (const char*)ruleName.c_str(), sizeof(stuRuleOutInfo->stRule[0].szName) - 1); //规则名称
#endif
		stuRuleOutInfo->stRule[s32Index].nMeterType = ruleCalibType;
		stuRuleOutInfo->stRule[s32Index].nSamplePeriod = Period;
		stuRuleOutInfo->stRule[s32Index].nCoordinateCnt = pointCount;
		stuRuleOutInfo->stRule[s32Index].stLocalParameters.bEnable = bLocalParameters;
		stuRuleOutInfo->stRule[s32Index].stLocalParameters.fObjectEmissivity = emissivity;
		stuRuleOutInfo->stRule[s32Index].stLocalParameters.nObjectDistance = distance;
		stuRuleOutInfo->stRule[s32Index].stLocalParameters.nRefalectedTemp = reflex;
		stuRuleOutInfo->stRule[s32Index].emAreaSubType = EM_CFG_AREA_SUBTYPE_UNKNOWN;
		stuRuleOutInfo->stRule[s32Index].stAlarmSetting[0].bEnable = false;
		stuRuleOutInfo->stRule[s32Index].nAlarmSettingCnt = 1;
		stuRuleOutInfo->stRule[s32Index].stAlarmSetting[0].nId = 1;
		stuRuleOutInfo->stRule[s32Index].stAlarmSetting[0].nResultType = 2;
		stuRuleOutInfo->stRule[s32Index].stAlarmSetting[0].nAlarmCondition = 1;
		stuRuleOutInfo->stRule[s32Index].stAlarmSetting[0].fThreshold = 20;
		stuRuleOutInfo->stRule[s32Index].stAlarmSetting[0].fHysteresis = 0.1000001;
		stuRuleOutInfo->stRule[s32Index].stAlarmSetting[0].nDuration = 30;
		for (int j = 0; j < pointCount; j++)
		{
			stuRuleOutInfo->stRule[s32Index].stCoordinates[j].nX = int(rvalue["pointList"][j]["x"].asFloat()*DHRealcor);
			stuRuleOutInfo->stRule[s32Index].stCoordinates[j].nY = int(rvalue["pointList"][j]["y"].asFloat()*DHRealcor);
		}
		char szJsonBuf[512 * 40] = { 0 };
		BOOL bRet = CLIENT_PacketData(CFG_CMD_THERMOMETRY_RULE, stuRuleOutInfo, sizeof(*stuRuleOutInfo), szJsonBuf, sizeof(szJsonBuf));
		if (!bRet)
		{
			DWORD dwError = CLIENT_GetLastError() & 0x7fffffff;
			ERRE_MSV("CLIENT_SetNewDevConfig %d\n", dwError);
			goto error;
		}
		else
		{
			int nerror = 0;
			int nrestart = 0;
			int nChannelID = chnNo;
			bRet = CLIENT_SetNewDevConfig(m_userId, CFG_CMD_THERMOMETRY_RULE, nChannelID, szJsonBuf, sizeof(szJsonBuf), &nerror, &nrestart, 3000); // 热成像测温全局配置
			if (!bRet)
			{
				DWORD dwError = CLIENT_GetLastError() & 0x7fffffff;
				ERRE_MSV("CLIENT_SetNewDevConfig %d\n", dwError);
				goto error;
			}
		}
	}
	else
	{
//		if (stuRuleOutInfo->nCount == ruleId )
		{
			stuRuleOutInfo->nCount = m_nRuleAmount - 1;
			char szJsonBuf[512 * 40] = { 0 };
			BOOL bRet = CLIENT_PacketData(CFG_CMD_THERMOMETRY_RULE, stuRuleOutInfo, sizeof(*stuRuleOutInfo), szJsonBuf, sizeof(szJsonBuf));
			if (!bRet)
			{
				DWORD dwError = CLIENT_GetLastError() & 0x7fffffff;
				ERRE_MSV("CLIENT_SetNewDevConfig %d\n", dwError);
				goto error;
			}
			else
			{
				int nerror = 0;
				int nrestart = 0;
				int nChannelID = chnNo;
				bRet = CLIENT_SetNewDevConfig(m_userId, CFG_CMD_THERMOMETRY_RULE, nChannelID, szJsonBuf, sizeof(szJsonBuf), &nerror, &nrestart, 3000); // 热成像测温全局配置
				if (!bRet)
				{
					DWORD dwError = CLIENT_GetLastError() & 0x7fffffff;
					ERRE_MSV("CLIENT_SetNewDevConfig %d\n", dwError);
					goto error;
				}
			}
		}
		/*else
		{
			ERRE_MSV("is not the last ruile\n");
			goto error;
		}*/
	}
	delete stuRuleOutInfo;
	stuRuleOutInfo = NULL;
	return 0;


error:
	delete stuRuleOutInfo;
	stuRuleOutInfo = NULL;
	return -1;
}

int    DahuaPu::startmeasuretemp(int ruleId, int channelNo, int PresetId,string &jout)
{
	SHL_Json::Value wvalue;
	AutoLock locker(temprule_s_mtx);
	CFG_RADIOMETRY_RULE_INFO *stuRuleOutInfo = new CFG_RADIOMETRY_RULE_INFO;
	if (NULL == stuRuleOutInfo)
	{
		return -1;
	}
	memset(stuRuleOutInfo, 0, sizeof(CFG_RADIOMETRY_RULE_INFO));
	char szJsonBufs[40 * 512] = { 0 };
	int nerror = 0;
	BOOL bRets = FALSE;
	bRets = CLIENT_GetNewDevConfig(m_userId, CFG_CMD_THERMOMETRY_RULE, channelNo, szJsonBufs, 40 * 512, &nerror, 3000);
	if (bRets)
	{
		DWORD dwRetLen = 0;
		bRets = CLIENT_ParseData(CFG_CMD_THERMOMETRY_RULE, szJsonBufs, (char*)stuRuleOutInfo, sizeof(CFG_RADIOMETRY_RULE_INFO), &dwRetLen);
	}
	vector<temprule_s>::const_iterator iter;
	if (ruleId == 0) //规则ID为0获取预置位下所有测温规则
	{
		int m_nRuleAmount = stuRuleOutInfo->nCount;
		for (int i = 0; i < m_nRuleAmount; i++)
		{
			temprule_s ruleinfo;
			if (stuRuleOutInfo->stRule[i].nPresetId == PresetId)
			{
				for (iter = m_ruleid.begin(); iter != m_ruleid.end(); iter++)
				{
					if (iter->ruleId == stuRuleOutInfo->stRule[i].nRuleId)
						break;
				}
				if (iter == m_ruleid.end())
				{
					ruleinfo.chnNo = channelNo;
					ruleinfo.ruleId = stuRuleOutInfo->stRule[i].nRuleId;
					ruleinfo.presetNo = stuRuleOutInfo->stRule[i].nPresetId;
					ruleinfo.ruleCalibType = stuRuleOutInfo->stRule[i].nMeterType;
					m_ruleid.push_back(ruleinfo);
				}
			}
		}
	}
	else
	{
		for (iter = m_ruleid.begin(); iter != m_ruleid.end(); iter++)
		{
			if (iter->ruleId == ruleId)
				break;
		}
		if (iter == m_ruleid.end())
		{
			temprule_s ruleinfo;
			ruleinfo.chnNo = channelNo;
			ruleinfo.ruleId = ruleId;
			ruleinfo.presetNo = PresetId;
			ruleinfo.ruleCalibType = stuRuleOutInfo->stRule[ruleId-1].nMeterType;
			m_ruleid.push_back(ruleinfo);
		}
	}
	wvalue["handle"] = ruleId;
	wvalue["deviceHost"] = this->host();
	wvalue["devicePort"] = this->port();
	SHL_Json::StreamWriterBuilder writer;
	jout = SHL_Json::writeString(writer, wvalue);
	m_startmeasure = true;
	m_lastpts = sys->currentTimeMillis();
	return 0;
}

int    DahuaPu::OnQuerypointinfo(int channelNo, int nX, int  nY, int type, string &json)
{
	SHL_Json::Value      value;
	NET_IN_RADIOMETRY_GETPOINTTEMPER stuInGetPointTemper = { sizeof(stuInGetPointTemper) };
	stuInGetPointTemper.dwSize = sizeof(stuInGetPointTemper);
	stuInGetPointTemper.nChannel = channelNo;
	stuInGetPointTemper.stCoordinate.nx = nX;
	stuInGetPointTemper.stCoordinate.ny = nY;
	NET_OUT_RADIOMETRY_GETPOINTTEMPER stuOutPointTemper = { sizeof(stuOutPointTemper) };
	BOOL ret = CLIENT_QueryDevInfo(m_userId, NET_QUERY_DEV_RADIOMETRY_POINT_TEMPER, &stuInGetPointTemper, &stuOutPointTemper, NULL, 1000);
	if (0 != ret)
	{
		value["Measuretype"] = stuOutPointTemper.stPointTempInfo.nMeterType;//测温类型
		value["Unit"] = stuOutPointTemper.stPointTempInfo.nTemperUnit;			//测温单位
		value["Temp"] = stuOutPointTemper.stPointTempInfo.fTemperAver;
	}
	else
	{
		return -1;
	}
	SHL_Json::StreamWriterBuilder writer;
	json = SHL_Json::writeString(writer, value);
	return 0;
}

int    DahuaPu::OnQueryiteminfo(int ruleId, int channelNo, int PresetID, string &json, int ruleCalibType)
{
	SHL_Json::Value      value;
	if (ruleCalibType == 0)
	{
		AutoLock locker(temprule_s_mtx);
		vector<temprule_s>::const_iterator iter;
		for (iter = m_ruleid.begin(); iter != m_ruleid.end(); iter++)
		{
			if (iter->ruleId == ruleId)
			{
				ruleCalibType = iter->ruleCalibType;
				break;
			}
		}
		/*CFG_RADIOMETRY_RULE_INFO *stuRuleOutInfo = new CFG_RADIOMETRY_RULE_INFO;
		if (NULL == stuRuleOutInfo)
		{
			return -1;
		}
		memset(stuRuleOutInfo, 0, sizeof(CFG_RADIOMETRY_RULE_INFO));
		char szJsonBufs[40 * 512] = { 0 };
		int nerror = 0;
		BOOL bRets = FALSE;
		bRets = CLIENT_GetNewDevConfig(m_userId, CFG_CMD_THERMOMETRY_RULE, channelNo, szJsonBufs, 40 * 512, &nerror, 3000);
		if (bRets)
		{
			DWORD dwRetLen = 0;
			bRets = CLIENT_ParseData(CFG_CMD_THERMOMETRY_RULE, szJsonBufs, (char*)stuRuleOutInfo, sizeof(CFG_RADIOMETRY_RULE_INFO), &dwRetLen);
		}
		ruleCalibType = stuRuleOutInfo->stRule[ruleId - 1].nMeterType;*/
	}

	NET_IN_RADIOMETRY_GETTEMPER stuRadioMetryInTemper = { sizeof(stuRadioMetryInTemper) };
	NET_OUT_RADIOMETRY_GETTEMPER stuRadioMetryOutTemper = { sizeof(stuRadioMetryOutTemper) };
	stuRadioMetryInTemper.stCondition.nChannel = channelNo;
	stuRadioMetryInTemper.stCondition.nPresetId = 0;
	stuRadioMetryInTemper.stCondition.nRuleId = ruleId;
	stuRadioMetryInTemper.stCondition.nMeterType = ruleCalibType;

	BOOL res = CLIENT_QueryDevInfo(m_userId, NET_QUERY_DEV_RADIOMETRY_TEMPER, &stuRadioMetryInTemper, &stuRadioMetryOutTemper, NULL, 3000);
	if (0 != res)
	{
		value["ruleCalibType"] = stuRadioMetryOutTemper.stTempInfo.nMeterType;	//测温类型
		if (stuRadioMetryOutTemper.stTempInfo.nMeterType == 1)
		{
			value["pointTemp"] = stuRadioMetryOutTemper.stTempInfo.fTemperAver;
		}
		else
		{
			value["polygonMaxTemp"].asFloat();
			value["polygonAverageTemp"] = stuRadioMetryOutTemper.stTempInfo.fTemperAver;
			value["polygonMaxTemp"] = stuRadioMetryOutTemper.stTempInfo.fTemperMax;
			value["polygonMinTemp"] = stuRadioMetryOutTemper.stTempInfo.fTemperMin;
			value["polygonMidTemp"] = stuRadioMetryOutTemper.stTempInfo.fTemperMid;
			value["polygonStdTemp"] = stuRadioMetryOutTemper.stTempInfo.fTemperStd;
		}
	}
	else
	{
		DWORD dwError = CLIENT_GetLastError() & 0x7fffffff;
		ERRE_MSV("CLIENT_SetNewDevConfig %d\n", dwError);
		return -1;
	}
	SHL_Json::StreamWriterBuilder writer;
	json = SHL_Json::writeString(writer, value);
	return 0;
}


