#include "hik_face_auth_pu.h"
#include "json/json.h"
#include "triggerlistener.h"
#include "global_str_def.h"
#include "log.h"
#include "putree.h"
#include "debug.h"
#include "base64_ex.h"
#include "stream.h"
#include "system.h"

const string       STRSETCARD_C = "gatewayCardCallback_c";
const string       STRFACEDAT_C = "setFaceParamCallback_c";


void CALLBACK 
flogin_cb(LONG userId, DWORD result, LPNET_DVR_DEVICEINFO_V30  deviceInfo, void *pUser)
{
    HikFaceAuthPu *pu = (HikFaceAuthPu *)pUser;

    if (1 == result)
    {
        LOGI_MSV("Device: %s login success! user ID: %d", pu->ip().c_str(), userId);
        pu->m_login = true;
        pu->writeDevLog(STR_MSV_ON_LINE);
    }

    LOGI_MSV("login reslut: %d, %s", result, pu->ip().c_str());
}

void CALLBACK
alarm_msg_cb(LONG cmd, NET_DVR_ALARMER *alarmer, char *alarmInfo, DWORD bufLen, void *userData)
{
    HikFaceAuthPu *pu = NULL;
    {
        AutoLock locker(HikFaceAuthPu::m_userMap._mtx);
        pu = (HikFaceAuthPu *)HikFaceAuthPu::m_userMap[alarmer->lUserID];
    }
    if (!pu)
    {
        return;
    }
    LOGE_MSV("hik face auth pu alarm type: %0x", cmd);



    SHL_Json::FastWriter writer;
    /* SHL_Json::StreamWriterBuilder writer; */
    SHL_Json::Value      value;
    SHL_Json::Value      array;

    value["version"] = "1.0";
    value["time"] = sys->currentTime();
    value["deviceName"] = pu->name();
    value["deviceHost"] = pu->host();
    value["devicePort"] = pu->port();
    value["addrcode"] = pu->addrcode();
    value["level"] = 1;

    if (cmd == COMM_ID_INFO_ALARM)
    {
        //face detach
        LPNET_DVR_ID_CARD_INFO_ALARM idCardInfo = (LPNET_DVR_ID_CARD_INFO_ALARM)alarmInfo;

        FaceAlarmInfo_S * pIdNewCardInfoEx = new FaceAlarmInfo_S();
        pIdNewCardInfoEx->iType = 0;

        LPNET_DVR_ID_CARD_INFO_ALARM idNewCardInfo = new NET_DVR_ID_CARD_INFO_ALARM();
        memcpy(idNewCardInfo, idCardInfo, sizeof(NET_DVR_ID_CARD_INFO_ALARM));

        if (idCardInfo->dwCapturePicDataLen != 0)
        {
            idNewCardInfo->pCapturePicData = new char[idCardInfo->dwCapturePicDataLen];
            memcpy(idNewCardInfo->pCapturePicData, idCardInfo->pCapturePicData, idCardInfo->dwCapturePicDataLen);
        }
        if (idCardInfo->dwPicDataLen != 0)
        {
            idNewCardInfo->pPicData = new char[idCardInfo->dwPicDataLen];
            memcpy(idNewCardInfo->pPicData, idCardInfo->pPicData, idCardInfo->dwPicDataLen);
        }
        pIdNewCardInfoEx->pDVRIdCardInfo = idNewCardInfo;

        {
            AutoLock locker1(pu->m_alarmList._mtx);
            pu->m_alarmList.push(pIdNewCardInfoEx);
        }
    }
    else if (cmd == COMM_ALARM_ACS)
    {
        NET_DVR_ACS_ALARM_INFO * acsEvent = (NET_DVR_ACS_ALARM_INFO *)alarmInfo;
        if (MAJOR_EVENT == acsEvent->dwMajor)
        {
            FaceAlarmInfo_S * pIdNewCardInfoEx = new FaceAlarmInfo_S();
            pIdNewCardInfoEx->iType = 1;

            NET_DVR_ACS_ALARM_INFO * pNetDvrAcsAlarmInfo = new NET_DVR_ACS_ALARM_INFO();
            memcpy(pNetDvrAcsAlarmInfo, acsEvent, sizeof(NET_DVR_ACS_ALARM_INFO));

            pNetDvrAcsAlarmInfo->pPicData = new char[acsEvent->dwPicDataLen];
            memcpy(pNetDvrAcsAlarmInfo->pPicData, acsEvent->pPicData, acsEvent->dwPicDataLen);

            pIdNewCardInfoEx->pDVRAcsAlarmInfo = pNetDvrAcsAlarmInfo;

            {
                AutoLock locker1(pu->m_alarmList._mtx);
                pu->m_alarmList.push(pIdNewCardInfoEx);
            }         
        }

    }

    return;
}


void CALLBACK
set_card_cb(DWORD dwType, void* lpBuffer, DWORD dwBufLen, void* pUserData)
{
    if (dwType != NET_SDK_CALLBACK_TYPE_STATUS)
    {
        return;
    }

    HikFaceAuthPu * pu = (HikFaceAuthPu *)pUserData;

    DWORD dwStatus = *(DWORD*)lpBuffer;
    printf("SetGatewayCardCallback type: %d, status: %d \n", dwType, dwStatus);
    char szCardNumber[ACS_CARD_NO_LEN + 1] = "\0";

    //strncpy(szCardNumber, (char*)(lpBuffer)+4, ACS_CARD_NO_LEN);
    int resp = 0;

    if (dwStatus == NET_SDK_CALLBACK_STATUS_PROCESSING)
    {


    }
    else if (dwStatus == NET_SDK_CALLBACK_STATUS_FAILED)
    {
        DWORD dwErrCode = *((DWORD*)lpBuffer + 1);
        strncpy(szCardNumber, (char*)(lpBuffer)+8, ACS_CARD_NO_LEN);
        LOGE_MSV("set card number failed, code:%d card no:%s in set_card_cb NET_SDK_CALLBACK_STATUS_FAILED", dwErrCode, szCardNumber);
        resp = FALSE;
        pu->m_evtSetCard.notify(STRSETCARD_C, resp);
    }
    else if (dwStatus == NET_SDK_CALLBACK_STATUS_SUCCESS)
    {
        LOGE_MSV("set card number success in set_card_cb");
        resp = TRUE;
        pu->m_evtSetCard.notify(STRSETCARD_C, resp);

    }
    else if (dwStatus == NET_SDK_CALLBACK_STATUS_EXCEPTION)
    {
        LOGE_MSV("set card number exception in set_card_cb NET_SDK_CALLBACK_STATUS_EXCEPTION");
        resp = FALSE;
        pu->m_evtSetCard.notify(STRSETCARD_C, resp);
    }
    //pu->writeAlarmLog(json);

}

void CALLBACK 
set_face_cb(DWORD dwType, void* lpBuffer, DWORD dwBufLen, void* pUserData)
{
    std::string strInfo;

    HikFaceAuthPu * pu = (HikFaceAuthPu *)pUserData;

    printf("SetFaceParamCallback type: %d \n", dwType);

    int  resp = 0;

    if (dwType == NET_SDK_CALLBACK_TYPE_STATUS)
    {
        DWORD dwStatus = *(DWORD*)lpBuffer;
        char szCardNumber[ACS_CARD_NO_LEN + 1] = "\0";
        strncpy(szCardNumber, (char*)(lpBuffer)+8, ACS_CARD_NO_LEN);


        if (dwStatus == NET_SDK_CALLBACK_STATUS_PROCESSING)
        {
            LOGE_MSV("set face data connect pic doing");
        }
        else if (dwStatus == NET_SDK_CALLBACK_STATUS_FAILED)
        {
            LOGE_MSV("set face data connect failed in set_face_cb NET_SDK_CALLBACK_STATUS_FAILED");
            resp = FALSE;
            pu->m_evtSetFaceData.notify(STRFACEDAT_C, resp);
        }
        else if (dwStatus == NET_SDK_CALLBACK_STATUS_SUCCESS)
        {
            LOGE_MSV("set face data connect success");
            resp = TRUE;
            pu->m_evtSetFaceData.notify(STRFACEDAT_C, resp);
        }
        else if (dwStatus == NET_SDK_CALLBACK_STATUS_EXCEPTION)
        {
            LOGE_MSV("set face data connect exception in set_face_cb NET_SDK_CALLBACK_STATUS_EXCEPTION");
            resp = FALSE;
            pu->m_evtSetFaceData.notify(STRFACEDAT_C, resp);
        }
        else
        {
            LOGE_MSV("set face data connect success");
            resp = TRUE;
            pu->m_evtSetFaceData.notify(STRFACEDAT_C, resp);
        }

    }
    else if (dwType == NET_SDK_CALLBACK_TYPE_DATA)
    {
        NET_DVR_FACE_PARAM_STATUS struCfg = { 0 };
        memcpy(&struCfg, lpBuffer, sizeof(struCfg));

        int i;
        BOOL bSendOk = FALSE;
        for (i = 0; i < sizeof(struCfg.byCardReaderRecvStatus); i++)
        {
            if (struCfg.byCardReaderRecvStatus[i] == 1)
            {
                bSendOk = TRUE;
                break;
            }
        }

        if (bSendOk)
        {

        }
        else
        {
            LOGE_MSV("set face data connect failed in set_face_cb");
            resp = FALSE;
            pu->m_evtSetFaceData.notify(STRFACEDAT_C, resp);
        }
    }
    //pu->writeAlarmLog(json);
}


UserMap HikFaceAuthPu::m_userMap;
HikFaceAuthPu::HikFaceAuthPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType)
    : Pu(type, addrcode, name, chnNum, user, pswd, host)
{
    m_protocol = PROTO_TYPE_HIK;
    m_port = port;
    m_domainType = domainType;
    m_userID = -1;
    m_alarmHandle = -1;
}

HikFaceAuthPu::~HikFaceAuthPu()
{
    
}

int HikFaceAuthPu::start()
{
    if (Pu::start() < 0)
    {
        return -1;
    }

    NET_DVR_Init();                  
    NET_DVR_SetConnectTime(2000, 10);
    NET_DVR_SetReconnect(5000, true);

    uint32_t u32Version = NET_DVR_GetSDKBuildVersion();

    char strTemp[1024] = {0};
    sprintf(strTemp, "HCNetSDK V%d.%d.%d.%d\n",
            (0xff000000 & u32Version)>>24,
            (0x00ff0000 & u32Version)>>16,
            (0x0000ff00 & u32Version)>>8,
            (0x000000ff & u32Version));
    printf(strTemp);

    m_init = TRUE;

    return 0;
}

void HikFaceAuthPu::stop()
{
    if (!m_init)
        return;

    m_init = FALSE;

    logout();
    
    Pu::stop();
}

int HikFaceAuthPu::login()
{
    if (m_login)
        return 0;

    NET_DVR_USER_LOGIN_INFO loginInfo;
    NET_DVR_DEVICEINFO_V40  deviceInfo;
    memset(&loginInfo, 0, sizeof(loginInfo));
    memset(&deviceInfo, 0, sizeof(deviceInfo));

    loginInfo.wPort = m_port;
    loginInfo.bUseAsynLogin = TRUE;
    loginInfo.cbLoginResult = flogin_cb;
    loginInfo.pUser = this;

    strncpy(loginInfo.sDeviceAddress, m_ip.c_str(), sizeof(loginInfo.sDeviceAddress));
    strncpy(loginInfo.sUserName, m_user.c_str(), sizeof(loginInfo.sUserName));
    strncpy(loginInfo.sPassword, m_pswd.c_str(), sizeof(loginInfo.sPassword));

    m_userID = NET_DVR_Login_V40(&loginInfo, &deviceInfo);
    if (m_userID < 0)
    {
        LOGE_MSV("NET_DVR_Login_V30 error, %d", NET_DVR_GetLastError());
        return -1;
    }

	int wait = 10000;
	while (wait > 0 && !m_login)
	{
		sys->sleep(100);
		wait -= 100;
	}

	if (wait <= 0)
	{
		if (-1 != m_userID)
		{
			/* It is recommended to use the NET_DVR_Logout for Hik SDK */
			NET_DVR_Logout(m_userID);
			m_userID = -1;
		}

		return -1;
	}

    NET_DVR_SetDVRMessageCallBack_V50(1, alarm_msg_cb, NULL);

    NET_DVR_SETUPALARM_PARAM struSetupParam = { 0 };
    struSetupParam.dwSize = sizeof(NET_DVR_SETUPALARM_PARAM);
    struSetupParam.byAlarmInfoType = 1;
    struSetupParam.byLevel = 1;

    LONG lHandle = NET_DVR_SetupAlarmChan_V41(m_userID, &struSetupParam);
    if (lHandle < 0)
    {
        printf("NET_DVR_SetupAlarmChan_V41 error, %d\n", NET_DVR_GetLastError());
        NET_DVR_Logout(m_userID);
        return -1;
    }

    {
        AutoLock locker(m_userMap._mtx);
        m_userMap[m_userID] = this;
    }


    return 0;
}

void HikFaceAuthPu::logout()
{

    if (!m_login)
        return;

    if (-1 != m_userID)
    {
        NET_DVR_Logout_V30(m_userID);
    }

    {
        AutoLock locker(m_userMap._mtx);
        map<long, Pu *>::iterator i = m_userMap.find(m_userID);
        if (i != m_userMap.end())
        {
            m_userMap.erase(i);
        }
    }


    if (-1 != m_alarmHandle)
    {
        NET_DVR_CloseAlarmChan_V30(m_alarmHandle);
        m_alarmHandle = -1;
    }

    m_userID = -1;
    m_login = false;
}


void
HikFaceAuthPu::setCardInfo(NET_DVR_CARD_CFG_V50 & netDvrCardInfo)
{
    netDvrCardInfo.byCardType = 1;
    netDvrCardInfo.byLeaderCard = 1;
    netDvrCardInfo.dwPlanTemplate = 1;
    netDvrCardInfo.dwModifyParamType = 32767;
    netDvrCardInfo.byDoorRight[0] = 1;
    netDvrCardInfo.struValid.byEnable = 1;
    netDvrCardInfo.struValid.struBeginTime.wYear = 2018;
    netDvrCardInfo.struValid.struBeginTime.byMonth = 1;
    netDvrCardInfo.struValid.struBeginTime.byDay = 1;
    netDvrCardInfo.struValid.struBeginTime.byHour = 0;
    netDvrCardInfo.struValid.struBeginTime.byMinute = 0;
    netDvrCardInfo.struValid.struBeginTime.bySecond = 0;
    netDvrCardInfo.struValid.struEndTime.wYear = 2037;
    netDvrCardInfo.struValid.struEndTime.byMonth = 12;
    netDvrCardInfo.struValid.struEndTime.byDay = 28;
    netDvrCardInfo.struValid.struEndTime.byHour = 23;
    netDvrCardInfo.struValid.struEndTime.byMinute = 59;
    netDvrCardInfo.struValid.struEndTime.bySecond = 59;
    netDvrCardInfo.byBelongGroup[0] = 1;
    strcpy((char*)netDvrCardInfo.byCardPassword, "2222");
    netDvrCardInfo.wCardRightPlan[0][0] = 1;
    netDvrCardInfo.wCardRightPlan[0][1] = 2;
    netDvrCardInfo.dwMaxSwipeTime = 32;
    netDvrCardInfo.wDepartmentNo = 1;
    netDvrCardInfo.wSchedulePlanNo = 1;
    netDvrCardInfo.bySchedulePlanType = 2;
}

int
HikFaceAuthPu::setCardNo(string name, string Id, int CardValid)
{
    LOGI_MSV("cardName:%s cardNo:%s", name.c_str(), Id.c_str());

    BOOL bOnline = NET_DVR_RemoteControl(m_userID, NET_DVR_CHECK_USER_STATUS, NULL, NULL);
    if (!bOnline)
    {
		ERRE_MSV("SendFaceDataToDev failed, reason as follows: device offline error = %d\n!", NET_DVR_GetLastError());
        return -1;
    }   

    NET_DVR_CARD_CFG_COND struCond = { 0 };
    struCond.dwSize = sizeof(struCond);
    struCond.dwCardNum = 1;
    struCond.byCheckCardNo = 1;
    struCond.wLocalControllerID = 0;

    LONG setCardCfgHandle = NET_DVR_StartRemoteConfig(m_userID,
        NET_DVR_SET_CARD_CFG_V50,
        &struCond,
        sizeof(struCond),
        set_card_cb,
        this);

    if (setCardCfgHandle < 0)
    {
        ERRE_MSV("NET_DVR_StartRemoteConfig failed");
        return -1;
    }

    NET_DVR_CARD_CFG_V50  netDvrCardInfo = { 0 };
    netDvrCardInfo.dwSize = sizeof(NET_DVR_CARD_CFG_V50);
    strncpy((char*)netDvrCardInfo.byName, name.c_str(), sizeof(netDvrCardInfo.byName));
    strncpy((char*)netDvrCardInfo.byCardNo, Id.c_str(), sizeof(netDvrCardInfo.byCardNo));
    netDvrCardInfo.byCardValid = CardValid;
    netDvrCardInfo.dwEmployeeNo = atoi(Id.c_str());
    setCardInfo(netDvrCardInfo);
    BOOL bReturn = NET_DVR_SendRemoteConfig(setCardCfgHandle,
        ENUM_ACS_SEND_DATA,
        (char *)&netDvrCardInfo,
        sizeof(NET_DVR_CARD_CFG_V50));
    if (!bReturn)
    {
        NET_DVR_StopRemoteConfig(setCardCfgHandle);
        setCardCfgHandle = -1;
        ERRE_MSV("NET_DVR_SendRemoteConfig failed, reason as follows:%d", NET_DVR_GetLastError());
        return -1;
    }

    int resp;
    m_evtSetCard.init(STRSETCARD_C, resp);
    if (!m_evtSetCard.wait(2000) || resp == FALSE)
    {
        ERRE_MSV("set carNo timeout or error");
        bReturn = -1;
    }
    else
    {
        bReturn = 0;
    }

    NET_DVR_StopRemoteConfig(setCardCfgHandle);
    return bReturn;
}

int
HikFaceAuthPu::addFaceData(const string &picData, string name, string Id)
{
    LOGI_MSV("addFace name:%s id:%s", name.c_str(), Id.c_str());
    BOOL bReturn = FALSE;

    NET_DVR_FACE_PARAM_COND struCond = { 0 };
    struCond.dwSize = sizeof(struCond);
    struCond.dwFaceNum = 1;
    LONG setFaceDataHandle = NET_DVR_StartRemoteConfig(m_userID, NET_DVR_SET_FACE_PARAM_CFG,
        &struCond,
        sizeof(struCond),
        set_face_cb,
        this);
    if (setFaceDataHandle < 0)
    {
        ERRE_MSV("NET_DVR_StartRemoteConfig NET_DVR_SET_FACE_PARAM_CFG failed:%d", NET_DVR_GetLastError());
        return -1;
    }

    NET_DVR_FACE_PARAM_CFG netDvrFaceParam = { 0 };
    memset(&netDvrFaceParam, 0, sizeof(netDvrFaceParam));
    strncpy((char*)netDvrFaceParam.byCardNo, Id.c_str(), sizeof(netDvrFaceParam.byCardNo));
    netDvrFaceParam.byFaceID = 1;
    netDvrFaceParam.byFaceDataType = 1;
    netDvrFaceParam.byEnableCardReader[0] = 1;
    netDvrFaceParam.dwSize = sizeof(NET_DVR_FACE_PARAM_CFG);
    int plen = picData.length();
    netDvrFaceParam.dwFaceLen = plen;
    //netDvrFaceParam.pFaceBuffer = new char[plen];
    netDvrFaceParam.pFaceBuffer = (char*)picData.c_str();

    bReturn = NET_DVR_SendRemoteConfig(setFaceDataHandle,
        ENUM_ACS_INTELLIGENT_IDENTITY_DATA,
        (char *)&netDvrFaceParam,
        sizeof(NET_DVR_FACE_PARAM_CFG));
    if (!bReturn)
    {
        ERRE_MSV("NET_DVR_StartRemoteConfig ENUM_ACS_INTELLIGENT_IDENTITY_DATA failed:%d", NET_DVR_GetLastError());
        NET_DVR_StopRemoteConfig(setFaceDataHandle);
        return -1;
    }

    int resp1;
    m_evtSetFaceData.init(STRFACEDAT_C, resp1);
    if (!m_evtSetFaceData.wait(4000) || resp1 == FALSE)
    {
        ERRE_MSV("NET_DVR_StartRemoteConfig ENUM_ACS_INTELLIGENT_IDENTITY_DATA send failed");
        bReturn = -1;
    }
    else
    {
        bReturn = 0;
    }

    NET_DVR_StopRemoteConfig(setFaceDataHandle);

    printf("\n\n\n\t\tadd face data:%d\n\n\n\n", bReturn);

    return bReturn;
}

int
HikFaceAuthPu::delFaceData(string name, string Id)
{
    NET_DVR_FACE_PARAM_CTRL struDelCfg = { 0 };
    struDelCfg.dwSize = sizeof(NET_DVR_FACE_PARAM_CTRL);
    struDelCfg.byMode = 0;
    strcpy((char*)struDelCfg.struProcessMode.struByCard.byCardNo, Id.c_str());
    struDelCfg.struProcessMode.struByCard.byEnableCardReader[0] = 1;
    struDelCfg.struProcessMode.struByCard.byFaceID[0] = 1;
    return NET_DVR_RemoteControl(m_userID, NET_DVR_DEL_FACE_PARAM_CFG, &struDelCfg, sizeof(struDelCfg));
}

int
HikFaceAuthPu::pushAlarmInfo(FaceAlarmInfo_S* info)
{
    char capturepicpath[256];
    char capturepicname[128];
    char facepicpath[256];
    char facepicname[128];

    SHL_Json::FastWriter writer;
    /* SHL_Json::StreamWriterBuilder writer; */
    SHL_Json::Value      value;
    SHL_Json::Value      array;

    value["version"] = "1.0";
    value["time"] = sys->currentTime();
    value["deviceName"] = name();
    value["deviceHost"] = host();
    value["devicePort"] = port();
    value["addrcode"] = addrcode();
    value["level"] = 1;
    

#ifdef WIN32
     string strFacePath = sys->utf2ascii(systm->facePath());
#else
    string strFacePath = systm->facePath();
#endif


    SHL_Json::Value item;
    if (0 == info->iType)
    {
        NET_DVR_ID_CARD_INFO_ALARM * pIdCardInfo = info->pDVRIdCardInfo;
#ifdef WIN32
        sprintf(capturepicpath, "%s\\%s_capture_%04d%02d%02d%02d%02d%02d.jpg", strFacePath.c_str(), host().c_str(),
            pIdCardInfo->struSwipeTime.wYear, pIdCardInfo->struSwipeTime.byMonth, pIdCardInfo->struSwipeTime.byDay,
            pIdCardInfo->struSwipeTime.byHour, pIdCardInfo->struSwipeTime.byMinute, pIdCardInfo->struSwipeTime.bySecond);

        sprintf(capturepicname, "%s_capture_%04d%02d%02d%02d%02d%02d.jpg", host().c_str(),
            pIdCardInfo->struSwipeTime.wYear, pIdCardInfo->struSwipeTime.byMonth, pIdCardInfo->struSwipeTime.byDay,
            pIdCardInfo->struSwipeTime.byHour, pIdCardInfo->struSwipeTime.byMinute, pIdCardInfo->struSwipeTime.bySecond);

        sprintf(facepicpath, "%s\\%s_idcard_%04d%02d%02d%02d%02d%02d.jpg", strFacePath.c_str(), host().c_str(),
            pIdCardInfo->struSwipeTime.wYear, pIdCardInfo->struSwipeTime.byMonth, pIdCardInfo->struSwipeTime.byDay,
            pIdCardInfo->struSwipeTime.byHour, pIdCardInfo->struSwipeTime.byMinute, pIdCardInfo->struSwipeTime.bySecond);

        sprintf(facepicname, "%s_idcard_%04d%02d%02d%02d%02d%02d.jpg", host().c_str(),
            pIdCardInfo->struSwipeTime.wYear, pIdCardInfo->struSwipeTime.byMonth, pIdCardInfo->struSwipeTime.byDay,
            pIdCardInfo->struSwipeTime.byHour, pIdCardInfo->struSwipeTime.byMinute, pIdCardInfo->struSwipeTime.bySecond);
#else

        sprintf(capturepicpath, "%s/%s_capture_%04d%02d%02d%02d%02d%02d.jpg", strFacePath.c_str(), host().c_str(),
            pIdCardInfo->struSwipeTime.wYear, pIdCardInfo->struSwipeTime.byMonth, pIdCardInfo->struSwipeTime.byDay,
            pIdCardInfo->struSwipeTime.byHour, pIdCardInfo->struSwipeTime.byMinute, pIdCardInfo->struSwipeTime.bySecond);

        sprintf(capturepicname, "%s_capture_%04d%02d%02d%02d%02d%02d.jpg", host().c_str(),
            pIdCardInfo->struSwipeTime.wYear, pIdCardInfo->struSwipeTime.byMonth, pIdCardInfo->struSwipeTime.byDay,
            pIdCardInfo->struSwipeTime.byHour, pIdCardInfo->struSwipeTime.byMinute, pIdCardInfo->struSwipeTime.bySecond);

        sprintf(facepicpath, "%s/%s_idcard_%04d%02d%02d%02d%02d%02d.jpg", strFacePath.c_str(), host().c_str(),
            pIdCardInfo->struSwipeTime.wYear, pIdCardInfo->struSwipeTime.byMonth, pIdCardInfo->struSwipeTime.byDay,
            pIdCardInfo->struSwipeTime.byHour, pIdCardInfo->struSwipeTime.byMinute, pIdCardInfo->struSwipeTime.bySecond);

		sprintf(facepicname, "%s_idcard_%04d%02d%02d%02d%02d%02d.jpg", host().c_str(),
			pIdCardInfo->struSwipeTime.wYear, pIdCardInfo->struSwipeTime.byMonth, pIdCardInfo->struSwipeTime.byDay,
			pIdCardInfo->struSwipeTime.byHour, pIdCardInfo->struSwipeTime.byMinute, pIdCardInfo->struSwipeTime.bySecond);
#endif
	
        if (0 != pIdCardInfo->dwCapturePicDataLen)
        {
            FileStream fs;
            fs.openWriteReplace(capturepicpath);
            fs.write(pIdCardInfo->pCapturePicData, pIdCardInfo->dwCapturePicDataLen);
            fs.close();
        }
        else
        {
            sprintf(capturepicname, "");
        }

        if (0 != pIdCardInfo->dwPicDataLen)
        {
            FileStream fs;
            fs.openWriteReplace(facepicpath);
            fs.write(pIdCardInfo->pPicData, pIdCardInfo->dwPicDataLen);
            fs.close();
        }
        else
        {
            sprintf(facepicname, "");
        }


        string str_idCardNum = (char*)(pIdCardInfo->struIDCardCfg.byIDNum);
        string str_idCardName = (char*)(pIdCardInfo->struIDCardCfg.byName);
        item["idCardNum"] = str_idCardNum;
        item["idCardName"] = str_idCardName;
                    
        char szBirth[64] = { 0 };
        sprintf(szBirth, "%04d-%02d-%02d", pIdCardInfo->struIDCardCfg.struBirth.wYear,
            pIdCardInfo->struIDCardCfg.struBirth.byMonth, pIdCardInfo->struIDCardCfg.struBirth.byDay);
        item["idCardBirth"] = szBirth;
        item["idCardAddress"] = (char*)pIdCardInfo->struIDCardCfg.byAddr;
        item["idCardSex"] = pIdCardInfo->struIDCardCfg.bySex;
        item["idCardNation"] = pIdCardInfo->struIDCardCfg.byNation;
        item["idCardPicName"] = facepicpath;
        item["CapturePicName"] = capturepicpath;

        if (MINOR_PEOPLE_AND_ID_CARD_COMPARE_PASS == pIdCardInfo->dwMinor)
        {
            item["Result"] = 0;
        }
        else if (MINOR_PEOPLE_AND_ID_CARD_COMPARE_FAIL == pIdCardInfo->dwMinor)
        {
            item["Result"] = 3;
        }

        item["type"] = TriggerListener::TR_FACE_IDCARD_DETECTTION;

        value["alarms"].append(item);
        string strResp = writer.write(value);

		writeAlarmLog(strResp);
        notifyAlarm(strResp, NULL, 0);
            
        

        delete[] pIdCardInfo->pCapturePicData;
        delete[] pIdCardInfo->pPicData;
        delete pIdCardInfo;
    }
    else if (1 == info->iType)
    {
        NET_DVR_ACS_ALARM_INFO * pNetDVRAcsAlarm = info->pDVRAcsAlarmInfo;
        std::string strID = sys->utf2ascii((char*)pNetDVRAcsAlarm->struAcsEventInfo.byCardNo);
        if (strID != "")
        {
#ifdef WIN32
			sprintf(capturepicpath, "%s\\%s_capture_%04d%02d%02d%02d%02d%02d.jpg", strFacePath.c_str(), host().c_str(),
				pNetDVRAcsAlarm->struTime.dwYear, pNetDVRAcsAlarm->struTime.dwMonth, pNetDVRAcsAlarm->struTime.dwDay,
				pNetDVRAcsAlarm->struTime.dwHour, pNetDVRAcsAlarm->struTime.dwMinute, pNetDVRAcsAlarm->struTime.dwSecond);

			sprintf(capturepicname, "%s_capture_%04d%02d%02d%02d%02d%02d.jpg", host().c_str(),
				pNetDVRAcsAlarm->struTime.dwYear, pNetDVRAcsAlarm->struTime.dwMonth, pNetDVRAcsAlarm->struTime.dwDay,
				pNetDVRAcsAlarm->struTime.dwHour, pNetDVRAcsAlarm->struTime.dwMinute, pNetDVRAcsAlarm->struTime.dwSecond);
#else
			sprintf(capturepicpath, "%s/%s_capture_%04d%02d%02d%02d%02d%02d.jpg", strFacePath.c_str(), host().c_str(),
				pNetDVRAcsAlarm->struTime.dwYear, pNetDVRAcsAlarm->struTime.dwMonth, pNetDVRAcsAlarm->struTime.dwDay,
				pNetDVRAcsAlarm->struTime.dwHour, pNetDVRAcsAlarm->struTime.dwMinute, pNetDVRAcsAlarm->struTime.dwSecond);

			sprintf(capturepicname, "%s_capture_%04d%02d%02d%02d%02d%02d.jpg", host().c_str(),
				pNetDVRAcsAlarm->struTime.dwYear, pNetDVRAcsAlarm->struTime.dwMonth, pNetDVRAcsAlarm->struTime.dwDay,
				pNetDVRAcsAlarm->struTime.dwHour, pNetDVRAcsAlarm->struTime.dwMinute, pNetDVRAcsAlarm->struTime.dwSecond);
#endif
            if (0 != pNetDVRAcsAlarm->dwPicDataLen)
            {
                FileStream fs;
                fs.openWriteReplace(capturepicpath);
                fs.write(pNetDVRAcsAlarm->pPicData, pNetDVRAcsAlarm->dwPicDataLen);
                fs.close();
            }
            else
            {
                sprintf(capturepicname, "");
            }


            item["ID"] = strID.c_str();
            item["CapturePicName"] = capturepicpath;


            if (MINOR_PEOPLE_AND_ID_CARD_COMPARE_PASS == pNetDVRAcsAlarm->dwMinor)
            {
                item["Result"] = 0;
            }
            else if (MINOR_PEOPLE_AND_ID_CARD_COMPARE_FAIL == pNetDVRAcsAlarm->dwMinor)
            {
                item["Result"] = 3;
            }

            item["type"] = TriggerListener::TR_FACE_DETECTTION;
			item["present"] = 0;
			item["alarmdesc"] = "";
			item["roild"] = 0;
			item["status"] = 1;
			item["picName"] = capturepicpath;
			item["channelNo"] = channels();
            value["alarms"].append(item);
            string strResp = writer.write(value);
			writeAlarmLog(strResp);
            notifyAlarm(strResp, NULL, 0);
            
        }
        delete[] pNetDVRAcsAlarm->pPicData;
        delete pNetDVRAcsAlarm;
    }

    return 0;
}

int
HikFaceAuthPu::onCustomCommand(string& json)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(json, rvalue, false))
    {
        return -1;
    }
    string jMethod = "";
    if (!rvalue["Method"].isNull())
    {
        jMethod = rvalue["Method"].asString();
    }
    else if (!rvalue["command"].isNull())
    {
        jMethod = rvalue["command"].asString();
    }

    SHL_Json::Value itemlist = rvalue["Item"];
    int realNum = itemlist.size();

    if (jMethod == "setFace")
    {
        string picData = rvalue["picData"].asString();
		int outSize;
		string outData = base64_decode(picData.c_str(), picData.size(), outSize);
        int opera = rvalue["opera"].asInt();
        string ID = rvalue["ID"].asString();
        string fname = rvalue["name"].asString();

        if (opera == 0)
        {
            //register face data
            //steps:
            //1 set card number
            //2 add face data

            if (0 != setCardNo(fname, ID, 1))
            {
                ERRE_MSV("set card number failed");
                return -1;
            }

            if (0 != addFaceData(outData, fname, ID))
            {
                ERRE_MSV("set face data failed");
                return -1;
            }
            return 0;
        }
        else if (opera == 1)
        {
            //delete face
            delFaceData(fname, ID);
            if (0 != setCardNo(fname, ID, 0))
            {
                ERRE_MSV("delete card number failed");
                return -1;
            }
            return 0;
        }

    }
    return -1;
}

void
HikFaceAuthPu::onTimer()
{

    {
        AutoLock locker(m_alarmList._mtx);
        while (m_alarmList.size() > 0)
        {
            FaceAlarmInfo_S *info = m_alarmList.front();
            
            pushAlarmInfo(info);

            delete  info;
            m_alarmList.pop();
        }
    }
    
    Pu::onTimer();
}


long
HikFaceAuthPu::getUserID()
{
    return m_userID;
}