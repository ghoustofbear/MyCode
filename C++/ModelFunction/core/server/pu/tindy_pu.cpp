#include "tindy_pu.h"
#include "tls_error.h"
#include "triggerlistener.h"
#include "bitoperation.h"
#include "global_str_def.h"

#if defined(WIN32) || (__cplusplus >= 201103L)
#include "json/json.h"
#else
#include "json-0.10.6/"
#endif


void  login_cb(int userId, long wParam, void* lParam, void *pUser)
{
    // m_blogin = true;
    TindyPu* pPu = (TindyPu*)pUser;

    ASSERT(pPu);      
    
    switch (LOWORD(wParam))
    {
    case WCM_LOGON_NOTIFY:
    {
        if (lParam == LOGON_SUCCESS)
        {
            pPu->m_login = true;
            //LOGI_MSV("Login success: %s", pPu->m_host);
            pPu->writeDevLog(STR_MSV_ON_LINE);
        }
        else
        {
            pPu->m_login = false;
            //LOGI_MSV("Login failed : %s", pPu->m_host);
        }
        break;
    }
    case WCM_QUERYFILE_FINISHED:
    {
        pPu->m_bFindFinish = true;
        //LOGI_MSV("query finished : %s", pPu->m_host);
        break;
    }
    case WCM_CONNECT_INFO:
    {
        //pPu->m_flagConnect = true;
        break;
    }
    default:
        break;
    }
}


void   alarm_cb(int userId, int chanNo, int alarmState, ALARMTYPE alarmType, void *userData)
{
    TindyPu *pPu = (TindyPu *)userData;

    ASSERT(pPu);
  /*  if (chanNo != pPu->m_chnNum)
    {
        return;
    }*/ //delete wxr2021/07/16

    SHL_Json::FastWriter writer;
    SHL_Json::Value      value;
    

    value["version"]    = "1.0";
    value["time"] = sys->currentTime();
    value["deviceName"] = pPu->name();
    value["deviceHost"] = pPu->host();
    value["devicePort"] = pPu->port();
    value["addrcode"]   = pPu->addrcode();
    value["level"] = 1;

    //U32 u32State = (_iAlarmState == 1) ? ALARM_START : ALARM_STOP; /* 天地1报警，0消警 */

    switch (alarmType)
    {
    //case ALARM_VDO_MOTION:
    //{
    //    SHL_Json::Value item;;
    //    item["type"]      = TriggerListener::TR_DEV_MD;
    //    item["status"]    = alarmState;
    //    item["gucode"]    = pPu->gucode(chanNo);
    //    item["channelNo"] = chanNo;

    //    value["alarms"].append(item);        
    //    break;
    //}

    case ALARM_VDO_LOST:
    {        
        SHL_Json::Value item;;
        item["type"]   = TriggerListener::TR_DEV_VLOSS;
        item["status"] = alarmState;
        item["gucode"]   = pPu->gucode(chanNo);
        item["channelNo"] = chanNo;

        value["alarms"].append(item);
        
        break;
    }

    case ALARM_VDO_INPORT:
    {
        SHL_Json::Value item;;
        item["type"] = TriggerListener::TR_DEV_IO;
        item["status"] = alarmState;
        item["gucode"] = pPu->gucode(chanNo);
        item["channelNo"] = chanNo;

        value["alarms"].append(item);

        break;
    }

    case ALARM_VCA_INFO:
    {
		SHL_Json::Value item;
		int iVcaAlarmInfoIndex = alarmState;
		vca_TAlarmInfo tVcaAlarmInfo = {0};
		int iRet = NetClient_VCAGetAlarmInfo(userId, iVcaAlarmInfoIndex, &tVcaAlarmInfo, sizeof(vca_TAlarmInfo));
		if (iRet < 0)
		{
			break;
		}
		item["status"] = tVcaAlarmInfo.iState;
		//if( tVcaAlarmInfo.iState == 1)
		{
			if (tVcaAlarmInfo.iEventType == 0 || tVcaAlarmInfo.iEventType == 1) 
			{
				item["type"] = TriggerListener::TR_AI_TRIPWIRE;  
			}
			else if (tVcaAlarmInfo.iEventType == 2)
			{
				item["type"] = TriggerListener::TR_AI_PERIMETER;
			}
		}
		item["gucode"] = pPu->gucode(chanNo);
		item["channelNo"] = chanNo;

		value["alarms"].append(item);
//		printf("%d ALARM_VCA EventType=(%d) State=(%d)!\n", chanNo, tVcaAlarmInfo.iState, tVcaAlarmInfo.iState);
        break;
    }
    default:
        return;
    }
    

    string jsonStr = writer.write(value); 
    pPu->notifyAlarm(jsonStr, NULL, 0);
    pPu->writeAlarmLog(jsonStr);
}

void   changeParam_cb(int userId, int chanNo, PARATYPE paramType, STR_Para* strcParam, void *userData)
{

}




UserMap TindyPu::m_userMap;
TindyPu::TindyPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType)
    :Pu(type, addrcode, name, chnNum, user, pswd, host)
{
    m_protocol     = PROTO_TYPE_TIANDY;
    m_port         = port;
    m_domainType   = domainType;
    m_userID       = -1;
    m_startTalkChn = 0;

    m_bFindFinish  = false;
    //m_flagConnect  = false;    
}


TindyPu::~TindyPu()
{

}




long TindyPu::getUserID()
{
    return m_userID;
}

int  TindyPu::start()
{
    if (Pu::start() < 0)
    {
        return -1;
    }

    int resInit = NetClient_Startup_V4(0, m_port, 0);

    if (resInit < 0)
    {
        LOGE_MSV(" SDK initial failed ");
    }

    NetClient_SetNotifyFunction_V4(login_cb, alarm_cb, changeParam_cb, NULL, NULL);

#if  defined(_WIN32) || defined(_WIN64)

    SDK_VERSION td_version = {0};

    NetClient_GetVersion(&td_version);
    char strVer[128] = { 0 };
    sprintf(strVer, "NetSdkClient MV:%d , SV:%d, BV:%d, CV:%s",
        td_version.m_ulMajorVersion,
        td_version.m_ulMinorVersion,
        td_version.m_ulBuilder,
        td_version.m_cVerInfo);

    LOGI_MSV(strVer);

#endif // defined(_WIN32) || defined(_WIN64)

    m_init = true;
    return resInit;

}



void 
TindyPu::stop()
{    
    m_init = false;
    
    logout();
    NetClient_Cleanup();
        
    Pu::stop();
}




int 
TindyPu::login()
{
    if (m_login)
    {
        return 0;
    }
#if 1
    LogonPara logInfo = {0};
    
    strncpy(logInfo.cUserName, m_user.c_str(), sizeof(logInfo.cUserName));
    strncpy(logInfo.cUserPwd, m_pswd.c_str(), sizeof(logInfo.cAccontPasswd));
    strncpy(logInfo.cNvsIP, m_ip.c_str(), sizeof(logInfo.cNvsIP));

    logInfo.iNvsPort = m_port;

    logInfo.iSize = sizeof(logInfo);


    m_userID = NetClient_Logon_V4(0, &logInfo, sizeof(logInfo));

    if (m_userID < 0)
    {
        m_login = false;
        LOGE_MSV("NetClient_Logon_V4 Oper failed");
        return false;
    }

    NetClient_SetNotifyUserData_V4(m_userID, this);


    int wait = 10000;

    while (wait > 0 && !m_login)
    {
        sys->sleep(100);

        wait -= 100;
    }

    if (wait < 0)
    {
        if ( -1 != m_userID)
        {
            NetClient_Logoff(m_userID);

            m_userID = -1;
        }

        return -1;
    }
#else
    strncpy(logInfo.cUserName, m_user.c_str(), sizeof(logInfo.cUserName));
    strncpy(logInfo.cUserPwd, m_pswd.c_str(), sizeof(logInfo.cAccontPasswd));
    strncpy(logInfo.cNvsIP, m_ip.c_str(), sizeof(logInfo.cNvsIP));


    m_userID = NetClient_Logon(0, (char*)m_ip.c_str(), (char*)m_user.c_str(), m_pswd.c_str(), 1024);

#endif

    {
        AutoLock locker(m_userMap._mtx);
        m_userMap[m_userID] = this;      
    }


    return 0;

}




void       
TindyPu::logout()
{
    

    if (-1 != m_userID)
    {
        NetClient_Logoff(m_userID);

        {
            AutoLock locker(m_userMap._mtx);
            map<long, Pu*>::iterator item = m_userMap.find(m_userID);
            m_userMap.erase(m_userID);
        }
 
    }
    
    m_login  = false;
    m_userID = -1;
    writeDevLog(STR_MSV_OFF_LINE);
}





bool TindyPu::validate(int channelNo)
{
#if 0
    int state = 0;
    int res   = -1;
    ChnOnlineState dd;

    if (0 == NetClient_GetDevConfig(m_userID, NET_CLIENT_ASYN_CHNONLINESTATE, channelNo, &dd, sizeof(dd), &res))
    {
        if (1 == state)
        {
            return true;
        }
    }
    
    return false;
#else
    return true;
#endif
}




bool TindyPu::validate(string gucode)
{
	int channelNo = GetChanNoByCode(gucode);

    printf("TindyPu::validate chn:%d\n", channelNo);

    return validate(channelNo);
}




int        
TindyPu::ptzCtrl(int cmd, int channelNo, int speed, int preset, bool stop)
{
    if (!m_login)
    {
        return -1;
    }

    speed = speed * 10;

    if (speed < 0 || speed >100)
    {
        speed = 50;
    }

    int res = -1;

    int ptzCmd = -1;
    switch (cmd)
    {
    case PTZ_CMD_UP:
        ptzCmd = PROTOCOL_MOVE_UP;
        break;
    case PTZ_CMD_DOWN:
        ptzCmd = PROTOCOL_MOVE_DOWN;
        break;
    case PTZ_CMD_LEFT:
        ptzCmd = PROTOCOL_MOVE_LEFT;
        break;
    case PTZ_CMD_RIGHT:
        ptzCmd = PROTOCOL_MOVE_RIGHT;
        break;
    case PTZ_CMD_UPLEFT:
        ptzCmd = PROTOCOL_MOVE_UP_LEFT;
        break;
    case PTZ_CMD_UPRIGHT:
        ptzCmd = PROTOCOL_MOVE_UP_RIGHT;
        break;
    case PTZ_CMD_DOWNLEFT:
        ptzCmd = PROTOCOL_MOVE_DOWN_LEFT;
        break;
    case PTZ_CMD_DOWNRIGHT:
        ptzCmd = PROTOCOL_MOVE_DOWN_RIGHT;
        break;
    case PTZ_CMD_ZOOMIN:
        ptzCmd = PROTOCOL_ZOOMIN;
        break;
    case PTZ_CMD_ZOOMOUT:
        ptzCmd = PROTOCOL_ZOOMOUT;
        break;
    case PTZ_CMD_FOCUSFAR:
        ptzCmd = PROTOCOL_FOCUS_OFF;
        break;
    case PTZ_CMD_FOCUSNEAR:
        ptzCmd = PROTOCOL_FOCUS_ON;
        break;
    case PTZ_CMD_IRISOPEN:
        ptzCmd = PROTOCOL_IRIS_OPEN;
        break;
    case PTZ_CMD_IRISCLOSE:
        ptzCmd = PROTOCOL_IRIS_CLOSE;
        break;
    case PTZ_CMD_AUTOSCAN:
        ptzCmd = (stop != false ? PROTOCOL_AUTO_SCAN_START : PROTOCOL_AUTO_SCAN_STOP);
        break;
    case PTZ_CMD_SETPRESET:
        ptzCmd = PROTOCOL_SET_PRESET;
        res = NetClient_DeviceCtrlEx(m_userID, channelNo, ptzCmd, preset, speed, 0);
        return res;

    case PTZ_CMD_CALLPRESET:
        ptzCmd = PROTOCOL_PRESET;
        res = NetClient_DeviceCtrlEx(m_userID, channelNo, ptzCmd, preset, speed, 0);
        return res;

    case PTZ_CMD_CLEARPRESET:
        ptzCmd = PROTOCOL_DELETE_PRESET;
        res = NetClient_DeviceCtrlEx(m_userID, channelNo, ptzCmd, preset, 0, 0);
        return res;


    case PTZ_CMD_RAIN_ON:
    case PTZ_CMD_RAIN_OFF:
        ptzCmd = (stop != false ? PROTOCOL_RAIN_ON : PROTOCOL_RAIN_OFF);
        break;
    default:
        break;
    }

    if (stop == true)
    {
        ptzCmd = PROTOCOL_MOVE_STOP;
    }
    res = NetClient_DeviceCtrlEx(m_userID, channelNo, ptzCmd, speed, speed,0);

    return res;
}



int
TindyPu::ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop)
{
	int channelNo = GetChanNoByCode(addrcode);

    return ptzCtrl(cmd, channelNo, speed, preset, stop);
}


void updateRecordString(TindyPu* pu)
{

}

int        
TindyPu::queryRecord(int channelNo, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json)
{
    NVS_FILE_QUERY_EX  query = { 0 };

    query.fileQuery.iType    = 0xFF;
    query.fileQuery.iChannel = channelNo;

    query.fileQuery.struStartTime.iYear   = start.year;
    query.fileQuery.struStartTime.iMonth  = start.month;
    query.fileQuery.struStartTime.iDay    = start.day;
    query.fileQuery.struStartTime.iHour   = start.hour;
    query.fileQuery.struStartTime.iMinute = start.minute;
    query.fileQuery.struStartTime.iSecond = start.second;


    query.fileQuery.struStopTime.iYear   = end.year;
    query.fileQuery.struStopTime.iMonth  = end.month;
    query.fileQuery.struStopTime.iDay    = end.day;
    query.fileQuery.struStopTime.iHour   = end.hour;
    query.fileQuery.struStopTime.iMinute = end.minute;
    query.fileQuery.struStopTime.iSecond = end.second;
  
    query.fileQuery.iPageNo   = 0;
    query.fileQuery.iPageSize = 20;                    
    query.fileQuery.iFiletype = 1;
    query.fileQuery.iDevType  = 0xFF;

    query.iTriggerType = 0x7FFFFFFF;
    query.iTrigger     = 0;


    //NVS_FILE_QUERY fileQuery;
    //memset(&fileQuery, 0, sizeof(fileQuery));

    //fileQuery.iChannel = 0;
    //fileQuery.iFiletype = 1;//0: All 1: AVstream 2: picture
    //fileQuery.iPageNo = 0;
    //fileQuery.iPageSize = 20;

    //fileQuery.struStartTime.iYear = 2018;
    //fileQuery.struStartTime.iMonth = 3;
    //fileQuery.struStartTime.iDay = 7;
    //fileQuery.struStartTime.iHour = 13;
    //fileQuery.struStartTime.iMinute = 0;
    //fileQuery.struStartTime.iSecond = 0;

    //fileQuery.struStopTime.iYear = 2018;
    //fileQuery.struStopTime.iMonth = 3;
    //fileQuery.struStopTime.iDay = 7;
    //fileQuery.struStopTime.iHour = 13;
    //fileQuery.struStopTime.iMinute = 59;
    //fileQuery.struStopTime.iSecond = 59;

    //fileQuery.iType = 255;//255: All 1: Manual record 2: Schedule record 3: Alarm record

    //int currentPage    = 0;


    int wait  = 10000;
    int delta = 100;

    json = "";

    SHL_Json::Value value;
    SHL_Json::Value array;

    int iCount      = 0;
    int iTotalCount = 0;

    do 
    {        
        int res = NetClient_NetFileQueryEx(m_userID, (PNVS_FILE_QUERY)&query, sizeof(NVS_FILE_QUERY_EX));

        //NetClient_Query_V4
        //long dw = GetLastError();


        ++(query.fileQuery.iPageNo);

        if (res < 0)
        {
            return -1;
        }

        while (!m_bFindFinish)
        {
            sys->sleep(delta);

            wait -= delta;

            if (wait <= 0)
            {
                break;
            }
        }

        if (!m_bFindFinish)
        {
            return -1;
        }


        //查询录像文件

        int currentCnt = 0;

        int fres = NetClient_NetFileGetFileCount(m_userID, &iTotalCount, &currentCnt);

        if (fres < 0)
        {
            m_bFindFinish = false;
            return -1;
        }

        if (iTotalCount < fromIndex)
        {
            return -1;
        }

        iCount += currentCnt;

        if (iCount < fromIndex)
        {
            continue;
        }

        int needCount = 0;


        if (iCount > toIndex -  fromIndex + 1)
        {
            needCount = iCount - toIndex + 1;
        }
        else
        {
            needCount = currentCnt;
        }



        NVS_FILE_DATA FileInfo = { 0 };
 
        for (int i = 0; i < needCount; i++)
        {
            if (0 == NetClient_NetFileGetQueryfile(m_userID, i, &FileInfo))
            {
                SHL_Json::Value record;

                record["filename"]  = FileInfo.cFileName;
                record["fileurl"]   = string("//") + m_host + string(":") + std::to_string(m_port) + string("/") + string(FileInfo.cFileName);
                record["channelNo"] = (int)channelNo;
                record["size"]      = (int)FileInfo.iFileSize;
                record["index"]     = iCount - currentCnt + i + 1;


                if (1 == FileInfo.iType || 2 == FileInfo.iType)
                {
                    /* UT Normal */
                    record["type"] = 1;
                }
                else if (3 == FileInfo.iType)
                {
                    record["type"] = 3;
                }

                datetime_s start;
                datetime_s end;

                start.year   = FileInfo.struStartTime.iYear;
                start.month  = FileInfo.struStartTime.iMonth;
                start.day    = FileInfo.struStartTime.iDay;
                start.hour   = FileInfo.struStartTime.iHour;
                start.minute = FileInfo.struStartTime.iMinute;
                start.second = FileInfo.struStartTime.iSecond;

                record["start"] = sys->strftime(start);

                end.year   = FileInfo.struStoptime.iYear;
                end.month  = FileInfo.struStoptime.iMonth;
                end.day    = FileInfo.struStoptime.iDay;
                end.hour   = FileInfo.struStoptime.iHour;
                end.minute = FileInfo.struStoptime.iMinute;
                end.second = FileInfo.struStoptime.iSecond;

                record["end"] = sys->strftime(end);

                array.append(record);
            }
        }



        m_bFindFinish = false;

    } while (iCount < iTotalCount);


    value["records"] = array;
    value["realNum"] = iCount;
    value["fromIndex"] = fromIndex;
    value["toIndex"] = fromIndex + array.size();

    SHL_Json::StreamWriterBuilder writer;
    json = SHL_Json::writeString(writer, value);

    return 0;

}

int
TindyPu::queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode)
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
TindyPu::syncTime(datetime_s &dt, const string &tzone)
{
    return -1;
}

void
TindyPu::timerdriver()
{
    Pu::onTimer();
}


