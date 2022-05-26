#include "hik_face_rgn_pu.h"
#include "json/json.h"
#include "triggerlistener.h"
#include "global_str_def.h"
#include "log.h"
#include "putree.h"
#include "debug.h"
#include "base64_ex.h"
#include "stream.h"
#include "system.h"
#include "xml/XmlBase.h"

#define GET_YEAR(_time_)          (((_time_)>>26) + 2000) 
#define GET_MONTH(_time_)       (((_time_)>>22) & 15)
#define GET_DAY(_time_)           (((_time_)>>17) & 31)
#define GET_HOUR(_time_)         (((_time_)>>12) & 31)
#define GET_MINUTE(_time_)      (((_time_)>>6) & 63)
#define GET_SECOND(_time_)     (((_time_)>>0) & 63) 

void CALLBACK
facerng_login_cb(LONG userId, DWORD result, LPNET_DVR_DEVICEINFO_V30  deviceInfo, void *pUser)
{
	HikFaceRngPu *pu = (HikFaceRngPu *)pUser;

	if (1 == result)
	{
		LOGI_MSV("Device: %s login success! user ID: %d", pu->ip().c_str(), userId);
		pu->m_login = true;
		pu->writeDevLog(STR_MSV_ON_LINE);
	}

	LOGI_MSV("login reslut: %d, %s", result, pu->ip().c_str());
}

void CALLBACK
facerng_alarm_msg_cb(LONG cmd, NET_DVR_ALARMER *alarmer, char *alarmInfo, DWORD bufLen, void *userData)
{
	HikFaceRngPu *pu = NULL;
	{
		AutoLock locker(HikFaceRngPu::m_userMap._mtx);
		pu = (HikFaceRngPu *)HikFaceRngPu::m_userMap[alarmer->lUserID];
	}
	if (!pu)
	{
		return;
	}
	LOGE_MSV("hik face auth pu alarm type: %0x", cmd);

	if (cmd == COMM_SNAP_MATCH_ALARM)
	{
		NET_VCA_FACESNAP_MATCH_ALARM alarmVcaFaceSnapMatch;
 		memcpy(&alarmVcaFaceSnapMatch, alarmInfo, sizeof(NET_VCA_FACESNAP_MATCH_ALARM));

		SHL_Json::FastWriter writer;
		SHL_Json::Value      value;
		SHL_Json::Value      array;

		value["version"]    = "1.0";
		value["time"]       = sys->currentTime();
		value["deviceName"] = pu->name();
		value["deviceHost"] = pu->host();
		value["devicePort"] = pu->port();
		value["addrcode"]   = pu->addrcode();
		value["level"]      = 1;
		value["channelNo"]  = 1;

		SHL_Json::Value item;
		item["type"]       = TriggerListener::TR_FACE_DETECTTION;
		item["name"]       = pu->name();
		item["code"]       = pu->addrcode();
		item["preset"]     = 1;
		item["channelNo"]  = 1;
		item["roiId"]      = 1;


        int iSimilarity = 10 * alarmVcaFaceSnapMatch.fSimilarity;
        string idname = "";
		if (2 == alarmVcaFaceSnapMatch.byContrastStatus || iSimilarity < 3)
		{
            idname = "unregister";
		}
		else
		{
            //item["id"] = sys->ascii2utf8((char*)alarmVcaFaceSnapMatch.struBlockListInfo.struBlockListInfo.struAttribute.byName);
            idname = (char*)alarmVcaFaceSnapMatch.struBlockListInfo.struBlockListInfo.struAttribute.byName;
		}

        char s8RealSnapPic[256] = { 0 };
		DWORD dwAbsTime = alarmVcaFaceSnapMatch.struSnapInfo.dwAbsTime;
		sprintf(s8RealSnapPic, "%04d%02d%02d%02d%02d%02d_%s.jpg", GET_YEAR(dwAbsTime), GET_MONTH(dwAbsTime), GET_DAY(dwAbsTime),
            GET_HOUR(dwAbsTime), GET_MINUTE(dwAbsTime), GET_SECOND(dwAbsTime), idname.c_str());
		

        FileStream         fileWrite;
        string strFacePath = systm->facePath();
#ifdef WIN32
        std::string   filename = sys->utf2ascii(strFacePath.c_str()) + "/" + s8RealSnapPic;
        fileWrite.openWriteReplace(filename.c_str());
#else
        std::string   filename = strFacePath + "/" + s8RealSnapPic;
        fileWrite.openWriteReplace(sys->ascii2utf8(filename.c_str()).c_str());
#endif
        
        
        if (fileWrite.isopen())
        {
            fileWrite.write(alarmVcaFaceSnapMatch.struSnapInfo.pBuffer1, alarmVcaFaceSnapMatch.struSnapInfo.dwSnapFacePicLen);
            fileWrite.close();
        }

        item["id"] = sys->ascii2utf8(idname);
        item["picName"] = sys->ascii2utf8(s8RealSnapPic);
        item["picPath"] = sys->ascii2utf8(filename);;        
		item["status"]    = "1";
		item["alarmdesc"] = "face rng";		

		value["alarms"].append(item);

		string strResp = writer.write(value);
		pu->notifyAlarm(strResp, NULL, 0);
	}
}

UserMap HikFaceRngPu::m_userMap;
HikFaceRngPu::HikFaceRngPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType)
	: Pu(type, addrcode, name, chnNum, user, pswd, host)
{
	m_protocol = PROTO_TYPE_HIK;
	m_port = port;
	m_domainType = domainType;
	m_userId = -1;
	m_alarmHandle = -1;
}

HikFaceRngPu::~HikFaceRngPu()
{

}

long
HikFaceRngPu::getUserID()
{
	return m_userId;
}

HIK_DVR_CFG
HikFaceRngPu::getDvrCfg()
{
	return m_dvrCfg;
}

int HikFaceRngPu::start()
{
	if (Pu::start() < 0)
	{
		return -1;
	}

	NET_DVR_Init();
	NET_DVR_SetConnectTime(2000, 10);
	NET_DVR_SetReconnect(5000, true);

	uint32_t u32Version = NET_DVR_GetSDKBuildVersion();

	char strTemp[1024] = { 0 };
	sprintf(strTemp, "HCNetSDK V%d.%d.%d.%d\n",
		(0xff000000 & u32Version) >> 24,
		(0x00ff0000 & u32Version) >> 16,
		(0x0000ff00 & u32Version) >> 8,
		(0x000000ff & u32Version));
	printf(strTemp);

	m_init = TRUE;

	return 0;
}

void HikFaceRngPu::stop()
{
	if (!m_init)
		return;

	m_init = FALSE;

	logout();

	Pu::stop();
}

int HikFaceRngPu::login()
{
	if (m_login)
		return 0;

	NET_DVR_USER_LOGIN_INFO loginInfo;
	NET_DVR_DEVICEINFO_V40  deviceInfo;
	memset(&loginInfo, 0, sizeof(loginInfo));
	memset(&deviceInfo, 0, sizeof(deviceInfo));

	loginInfo.wPort = m_port;
	loginInfo.bUseAsynLogin = TRUE;
	loginInfo.cbLoginResult = facerng_login_cb;
	loginInfo.pUser = this;

	strncpy(loginInfo.sDeviceAddress, m_ip.c_str(), sizeof(loginInfo.sDeviceAddress));
	strncpy(loginInfo.sUserName, m_user.c_str(), sizeof(loginInfo.sUserName));
	strncpy(loginInfo.sPassword, m_pswd.c_str(), sizeof(loginInfo.sPassword));

	m_userId = NET_DVR_Login_V40(&loginInfo, &deviceInfo);
	if (m_userId < 0)
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
		if (-1 != m_userId)
		{
			/* It is recommended to use the NET_DVR_Logout for Hik SDK */
			NET_DVR_Logout(m_userId);
			m_login = false;
			m_userId = -1;
		}

		return -1;
	}

	NET_DVR_SetDVRMessageCallBack_V50(2, facerng_alarm_msg_cb, NULL);

	NET_DVR_SETUPALARM_PARAM struSetupParam = { 0 };
	struSetupParam.dwSize = sizeof(NET_DVR_SETUPALARM_PARAM);
	struSetupParam.byAlarmInfoType = 1;
	struSetupParam.byLevel = 1;

	LONG lHandle = NET_DVR_SetupAlarmChan_V41(m_userId, &struSetupParam);
	if (lHandle < 0)
	{
		printf("NET_DVR_SetupAlarmChan_V41 error, %d\n", NET_DVR_GetLastError());
		NET_DVR_Logout(m_userId);
		m_login = false;
		m_userId = -1;
		return -1;
	}

	DWORD res;
	NET_DVR_GetDVRConfig(m_userId, NET_DVR_GET_DEVICECFG_V40, 0xFFFFFFFF, &m_dvrCfg, sizeof(NET_DVR_DEVICECFG_V40), &res);
	LOGI_MSV("HIKVISION DVR(ip: %s) type is 0x%x", m_ip.c_str(), m_dvrCfg.byDVRType);

	{
		AutoLock locker(m_userMap._mtx);
		m_userMap[m_userId] = this;
	}

	return 0;
}

void HikFaceRngPu::logout()
{
	if (!m_login)
		return;

	if (-1 != m_userId)
	{
		NET_DVR_Logout_V30(m_userId);
	}

	{
		AutoLock locker(m_userMap._mtx);
		map<long, Pu *>::iterator i = m_userMap.find(m_userId);
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

	m_userId = -1;
	m_login = false;
}

int
HikFaceRngPu::addFaceData(const string &picData, string name, string Id, string IDCard)
{
#if 0
	FILE * file = fopen("test.jpg", "wb");
	fwrite((BYTE *)picData.c_str(), picData.size(), 1, file);
	fclose(file);
#endif

    delFaceData(name, Id);

	long lUploadHandle = -1;

	NET_DVR_FACELIB_COND struFacedataParam = { 0 };
	struFacedataParam.dwSize = sizeof(struFacedataParam);
	strncpy(struFacedataParam.szFDID, "1", NET_SDK_MAX_PICID_LEN);
	struFacedataParam.byCustomFaceLibID = 0;
    struFacedataParam.byCover = 1;

	lUploadHandle = NET_DVR_UploadFile_V40(m_userId, IMPORT_DATA_TO_FACELIB, &struFacedataParam, sizeof(struFacedataParam), NULL, NULL, 0);

	if (lUploadHandle < 0)
	{
		DWORD dwRet = NET_DVR_GetLastError();

        LOGI_MSV("UtFaceAuth::AddFace -- NET_DVR_UploadFile_V40 failed, reason as follows: ip:%s, %d", m_ip.c_str(), dwRet);
		return -1;
	}

	NET_DVR_SEND_PARAM_IN sendParam = { 0 };

	sendParam.pSendData = (BYTE *)picData.c_str();
	sendParam.dwSendDataLen = picData.length();

	sendParam.struTime.wYear = 2018;
	sendParam.struTime.byMonth = 9;
	sendParam.struTime.byDay = 4;
	sendParam.struTime.byHour = 8;
	sendParam.struTime.byMinute = 30;
	sendParam.struTime.bySecond = 30;
	sendParam.byPicType = 1;

	strcpy((char*)sendParam.sPicName, "2.jpg");

	CXmlBase xmlBase1;
	xmlBase1.CreateRoot("FaceAppendData");
	xmlBase1.AddNode("name", name.c_str());
	xmlBase1.OutOfElem();


    if (IDCard != "")
    {
        xmlBase1.AddNode("certificateType", "ID");
        xmlBase1.OutOfElem();
        xmlBase1.AddNode("certificateNumber",IDCard.c_str());
        xmlBase1.OutOfElem();
    }
    
    xmlBase1.AddNode("customHumanID", Id.c_str());
    xmlBase1.OutOfElem();

	char faceAppData[200 * 1024] = { 0 };
	int dwBufLen1 = 200 * 1024;
	int dwReturn1;
	xmlBase1.WriteToBuf(faceAppData, dwBufLen1, dwReturn1);
	xmlBase1.WriteToFile("addtest.xml");

    sendParam.pSendAppendData = (unsigned char*)faceAppData;
	sendParam.dwSendAppendDataLen = dwReturn1;
	sendParam.dwPicMangeNo = 0;

	LONG lSend = NET_DVR_UploadSend(lUploadHandle, &sendParam, NULL);
	int ret = 0;
	if (lSend < 0)
	{
		DWORD dwRet = NET_DVR_GetLastError();
        LOGI_MSV("UtFaceAuth::AddFace -- NET_DVR_UploadFile_V40 failed, reason as follows: ip:%s, %d", m_ip.c_str(), dwRet);
		ret = -1;
	}
	else
	{
		time_t tmPrev = time(NULL);

		while (lSend >= 0)
		{
			time_t tmNow = time(NULL);
			if (tmNow - tmPrev >= 5)
			{
				ret = -1;
				break;
			}

			DWORD dwProgress = 0;
			LONG iStatus = -1;
			iStatus = NET_DVR_GetUploadState(lUploadHandle, &dwProgress);
			//LOGI_MSV("UtFaceAuth::AddFace -- NET_DVR_GetUploadState , iStatus = %ld", iStatus);

			if (1 == iStatus)
			{
				ret = 0;
				break;
			}
			else if ((iStatus >= 3 && iStatus <= 10) || iStatus == 31 || iStatus == -1)
			{
				ret = -1;
				break;
			}
			else if (2 == iStatus)
			{
                sys->sleep(100);
				continue;
			}
			else
			{
				ret = -1;
				break;
			}

            sys->sleep(100);
		}
	}

	if (-1 == ret)
	{
		delFaceData(name, Id);
	}

	NET_DVR_UploadClose(lUploadHandle);
	lUploadHandle = -1;

	return  ret;
}

int
HikFaceRngPu::delFaceData(string name, string Id)
{
	bool bReturn = true;
	DWORD dwBufLen = 10 * 1024;
	char *pBuf = new char[dwBufLen];
	memset(pBuf, 0, dwBufLen);
	int dwXmlLen = 0;

	NET_DVR_XML_CONFIG_INPUT    struInput = { 0 };
	NET_DVR_XML_CONFIG_OUTPUT   struOuput = { 0 };
	struInput.dwSize = sizeof(struInput);
	struOuput.dwSize = sizeof(struOuput);
	char szUrl[256] = { 0 };

	CXmlBase xmlBase;
	xmlBase.CreateRoot("FDSearchDescription");
	xmlBase.SetAttribute("version", "2.0");

	xmlBase.AddNode("searchID", "1");
	xmlBase.OutOfElem();

	xmlBase.AddNode("searchResultPosition", "1");
	xmlBase.OutOfElem();

	xmlBase.AddNode("maxResults", "50");
	xmlBase.OutOfElem();

	xmlBase.AddNode("FDID", "1");
	xmlBase.OutOfElem();

	xmlBase.AddNode("FaceModeList");
	xmlBase.AddNode("FaceMode");
	xmlBase.AddNode("ModeInfo");
	xmlBase.AddNode("similarity", "2");
	xmlBase.OutOfElem();
	xmlBase.AddNode("modeData", "");
	xmlBase.OutOfElem();
	xmlBase.OutOfElem();
	xmlBase.OutOfElem();
	xmlBase.OutOfElem();

	if (!xmlBase.WriteToBuf(pBuf, dwBufLen, dwXmlLen))
	{
		delete[]pBuf;
		pBuf = NULL;
		bReturn = FALSE;
		return bReturn;
	}

	xmlBase.WriteToFile("searchtest.xml");

	sprintf(szUrl, "POST /ISAPI/Intelligent/FDLib/FDSearch\r\n");

	struInput.lpRequestUrl = szUrl;
	struInput.dwRequestUrlLen = strlen(szUrl);
	struInput.lpInBuffer = pBuf;
	struInput.dwInBufferSize = dwXmlLen;
	DWORD dwOutputLen = 1024 * 1024;
	char *pOutBuf = new char[dwOutputLen];
	memset(pOutBuf, 0, dwOutputLen);
	struOuput.lpOutBuffer = pOutBuf;
	struOuput.dwOutBufferSize = dwOutputLen;

	BOOL bRet = NET_DVR_STDXMLConfig(m_userId, &struInput, &struOuput);
	if (!bRet)
	{
		DWORD dwReturn0 = NET_DVR_GetLastError();
		bReturn = FALSE;
		if (pOutBuf != NULL)
		{
			delete[]pOutBuf;
			pOutBuf = NULL;
		}

		if (pBuf != NULL)
		{
			delete[]pBuf;
			pBuf = NULL;
		}
		return bReturn;
	}

	CXmlBase xmlBase1;
	xmlBase1.Parse(pOutBuf);
	xmlBase1.WriteToFile("textface.xml");

	int face_id = -1;
	if (xmlBase1.FindElem("FDSearchResult") && xmlBase1.IntoElem())
	{

		if (xmlBase1.FindElem("MatchList") && xmlBase1.IntoElem())
		{
			do
			{
				if (xmlBase1.FindElem("MatchElement") && xmlBase1.IntoElem())
				{

					if (xmlBase1.FindElem("name"))
					{
						string pName = xmlBase1.GetData().c_str();
						string dName = name;
						if (pName == dName)
						{
							if (xmlBase1.FindElem("PID"))
							{
								face_id = atoi(xmlBase1.GetData().c_str());
							}
						}
					}

					xmlBase1.OutOfElem();
				}
			} while (xmlBase1.NextSibElem());

			xmlBase1.OutOfElem();
		}

		xmlBase1.OutOfElem();
	}
	if (-1 == face_id)
	{
		if (pOutBuf != NULL)
		{
			delete[]pOutBuf;
			pOutBuf = NULL;
		}

		if (pBuf != NULL)
		{
			delete[]pBuf;
			pBuf = NULL;
		}
		return false;
	}

	char szReqUrl[1024] = { 0 };
    sprintf(szReqUrl, "DELETE /ISAPI/Intelligent/FDLib/%s/picture/%d", Id.c_str(), face_id);
    //sprintf(szReqUrl, "DELETE /ISAPI/Intelligent/FDLib/1/picture/%d", face_id);


	struInput.lpRequestUrl = szReqUrl;
	struInput.dwRequestUrlLen = strlen(szReqUrl);

	DWORD dwOutputLen1 = 1024 * 1024;
	char *pOutBuf1 = new char[dwOutputLen1];
	memset(pOutBuf1, 0, dwOutputLen1);
	struOuput.lpOutBuffer = pOutBuf1;
	struOuput.dwOutBufferSize = dwOutputLen1;

	bRet = NET_DVR_STDXMLConfig(m_userId, &struInput, &struOuput);
	if (!bRet)
	{
		DWORD dwReturn0 = NET_DVR_GetLastError();
		bReturn = false;
	}

	if (pOutBuf1 != NULL)
	{
		delete[]pOutBuf1;
		pOutBuf1 = NULL;
	}

	if (pOutBuf != NULL)
	{
		delete[]pOutBuf;
		pOutBuf = NULL;
	}

	if (pBuf != NULL)
	{
		delete[]pBuf;
		pBuf = NULL;
	}

	return bReturn;
}

int
HikFaceRngPu::onCustomCommand(string& json)
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

	if (jMethod == "setFace")
	{
		string picData = rvalue["picData"].asString();
		int outSize;
		string outData = base64_decode(picData.c_str(), picData.size(), outSize);
		int opera = rvalue["opera"].asInt();

        if (rvalue["ID"].isNull() || rvalue["name"].isNull())
        {
            ERRE_MSV("param 'ID' or 'name' error!");
            return -1;
        }

		string ID = rvalue["ID"].asString();
        string fname = rvalue["name"].asString();
        string IDCard = "";
        if (!rvalue["IDCard"].isNull())
        {
            IDCard = rvalue["IDCard"].asString();
            if (IDCard.size() < 8)
            {
                ERRE_MSV("param 'IDCard' Length less than 8!");
                return -1;
            }
        }

		if (0 == opera)
		{
            if (0 != addFaceData(outData, fname, ID, IDCard))
			{
				ERRE_MSV("set face data failed");
				return -1;
			}
		}
		else if (1 == opera)
		{
			delFaceData(fname, ID);
		}
	}
	else if ("CapturePicture" == jMethod)
	{
		return capturePic(json);
	}

	return 0;
}

int
HikFaceRngPu::ptzCtrl(int cmd, int channelNo, int speed, int preset, bool stop)
{
	BOOL res = true;

	if (!m_login)
	{
		ERRE_MSV("device %s:%d is not on-line.", m_host.c_str(), m_port);
	}

	int ptzCmd = -1;
	switch (cmd)
	{
	case PTZ_CMD_UP:
		ptzCmd = TILT_UP;
		break;
	case PTZ_CMD_DOWN:
		ptzCmd = TILT_DOWN;
		break;
	case PTZ_CMD_LEFT:
		ptzCmd = PAN_LEFT;
		break;
	case PTZ_CMD_RIGHT:
		ptzCmd = PAN_RIGHT;
		break;
	case PTZ_CMD_UPLEFT:
		ptzCmd = UP_LEFT;
		break;
	case PTZ_CMD_UPRIGHT:
		ptzCmd = UP_RIGHT;
		break;
	case PTZ_CMD_DOWNLEFT:
		ptzCmd = DOWN_LEFT;
		break;
	case PTZ_CMD_DOWNRIGHT:
		ptzCmd = DOWN_RIGHT;
		break;
	case PTZ_CMD_ZOOMIN:
		ptzCmd = ZOOM_IN;
		break;
	case PTZ_CMD_ZOOMOUT:
		ptzCmd = ZOOM_OUT;
		break;
	case PTZ_CMD_FOCUSNEAR:
		ptzCmd = FOCUS_NEAR;
		break;
	case PTZ_CMD_FOCUSFAR:
		ptzCmd = FOCUS_FAR;
		break;
	case PTZ_CMD_IRISOPEN:
		ptzCmd = IRIS_OPEN;
		break;
	case PTZ_CMD_IRISCLOSE:
		ptzCmd = IRIS_CLOSE;
		break;
	case PTZ_CMD_AUTOSCAN:
		ptzCmd = PAN_AUTO;
		break;

	case PTZ_CMD_SETPRESET:
		ptzCmd = SET_PRESET;
		res = NET_DVR_PTZPreset_Other(m_userId, channelNo + 1, SET_PRESET, preset);
		return (res == TRUE ? 0 : -1);

	case PTZ_CMD_CLEARPRESET:
		res = NET_DVR_PTZPreset_Other(m_userId, channelNo + 1, CLE_PRESET, preset);
		return (res == TRUE ? 0 : -1);

	case PTZ_CMD_CALLPRESET:
		res = NET_DVR_PTZPreset_Other(m_userId, channelNo + 1, GOTO_PRESET, preset);
		return (res == TRUE ? 0 : -1);

	case PTZ_CMD_RAIN_ON:
	case PTZ_CMD_RAIN_OFF:
		ptzCmd = WIPER_PWRON;
		break;
	case PTZ_CMD_AUXLIGHT_ON:
	case PTZ_CMD_AUXLIGHT_OFF:
		ptzCmd = LIGHT_PWRON;
		break;
	}

	float rate = 7 / (float)10;
	int actualSpeed = (int)(speed * rate + 0.50);

	if (actualSpeed < 0 || actualSpeed > 7)
	{
		actualSpeed = 4;
	}

	res = NET_DVR_PTZControlWithSpeed_Other(m_userId, channelNo + 1, ptzCmd, stop, actualSpeed);
	LOGI_MSV("channelNo: %d, cmd: %d, actualSpeed: %d, preset: %d, stop: %d, result: %d", channelNo + 1, ptzCmd, actualSpeed, preset, stop, res);

	return (res == TRUE ? 0 : -1);
}

int
HikFaceRngPu::ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop)
{
	//addrcode --- 18 bit char , start pointer add 16 
	int channelNo = GetChanNoByCode(addrcode);

	return ptzCtrl(cmd, channelNo, speed, preset, stop);
}

int
HikFaceRngPu::capturePic(string json)
{
	if (m_userId < 0)
	{
		return FALSE;
	}

	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;

	if (!reader.parse(json, rvalue, false))
	{
		ERRE_MSV("SHL_Json parse error");
		return FALSE;
	}

	int chanNo = rvalue["chnNo"].asInt();
	int devPort = rvalue["DevPort"].asInt();
	string devIp = "";
	if (!rvalue["DevIp"].isNull())
	{
		devIp = rvalue["DevIp"].asString();
	}

	int width = 1;
	int height = 1;
	int picSize = getPicSize(chanNo + 1);
	if (picSize < 0)
	{
		ERRE_MSV("HikPu::capturePic failed! getPicSize() failed:addrcode = %s", addrcode().c_str());
		return -1;
	}

	NET_DVR_JPEGPARA struJpegPara;
	memset(&struJpegPara, 0, sizeof(NET_DVR_JPEGPARA));
	struJpegPara.wPicQuality = 0;
	struJpegPara.wPicSize = picSize;
	int iChanShowNum = chanNo + 1; //iChanIndex + g_struDeviceInfo[iDeviceIndex].iStartChan;

#ifdef WIN32
	string tmpplatePath = sys->utf2ascii(systm->facePath());
#else
	string tmpplatePath = systm->facePath();
#endif

	string strCurTime = sys->currentTime();
	int pos = strCurTime.find(' ');
	if (pos >= 0)
	{
		strCurTime.replace(pos, 1, "_");
	}

	for (int i = 0; i < 2; i++)
	{
		pos = strCurTime.find(':');
		if (pos >= 0)
		{
			strCurTime.replace(pos, 1, "-");
		}
	}

	char cFilename[256] = { 0 };
	sprintf(cFilename, "%s/JPEG_%s.jpg", \
		tmpplatePath.c_str(), strCurTime.c_str());

	BOOL ret = NET_DVR_CaptureJPEGPicture(m_userId, iChanShowNum, &struJpegPara, cFilename);
	if (TRUE == ret)
	{
		SHL_Json::FastWriter writer;
		SHL_Json::Value      value;
		SHL_Json::Value      array;

		value["version"] = "1.0";
		value["time"] = sys->currentTime();
		value["deviceName"] = name();
		value["deviceHost"] = host();
		value["devicePort"] = port();
		value["addrcode"] = addrcode();
		value["level"] = 1;


		SHL_Json::Value itemAlarm;
		itemAlarm["type"] = TriggerListener::TR_CAPTURE_PIC;;
		itemAlarm["channelNo"] = chanNo;
		itemAlarm["status"] = "0";
		itemAlarm["gucode"] = "";
		itemAlarm["name"] = "";
		itemAlarm["preset"] = 1;
		itemAlarm["alarmdesc"] = "";
		itemAlarm["roiId"] = 0;
		itemAlarm["picName"] = cFilename;

		itemAlarm["gucode"] = gucode(chanNo);
		itemAlarm["name"] = guname(chanNo);


		value["alarms"].append(itemAlarm);
		string json = writer.write(value);
		notifyAlarm(json, NULL, 0);
		//writeAlarmLog(json);
		return 0;
	}
	else
	{
		ERRE_MSV("NET_DVR_CaptureJPEGPicture failed,errorCode:%d", NET_DVR_GetLastError());
		return -1;
	}
	return -1;
}

int HikFaceRngPu::getPicSize(int chanNo)
{
	if (m_userId < 0)
		return -1;
	NET_DVR_COMPRESSIONCFG_V30 output = { 0 };

	DWORD dwReturn = 0;
	BOOL ret = NET_DVR_GetDVRConfig(m_userId, NET_DVR_GET_COMPRESSCFG_V30, chanNo, &output, sizeof(output), &dwReturn);
	if (ret == FALSE)
	{
		ERRE_MSV("NET_DVR_GetDVRConfig, NET_DVR_GET_COMPRESSCFG_V30 failed, errno:%d", NET_DVR_GetLastError());
		return -1;
	}
	return output.struNormHighRecordPara.byResolution;
}
