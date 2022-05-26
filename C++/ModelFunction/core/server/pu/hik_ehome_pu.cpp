#include "hik_ehome_pu.h"
#include "tls_error.h"
#include "triggerlistener.h"
#include "bitoperation.h"
#include "global_str_def.h"
#include "system.h"
#include "j2x.h"
#include "stream.h"
#include "3rd/hikehome/HCISUPPublic.h"
#include "3rd/hikehome/HCISUPCMS.h"
#include "platform.h"
#include <fstream>
#if defined(_WIN32) || (__cplusplus >= 201103L)
#include "json/json.h"
#else
#include "json-0.10.6/json.h"
#endif
string HikEhomePu::ExternalHost = "0.0.0.0";
int    HikEhomePu::ExternalPort = 8000;
HikEhomePu *EHome_Pu = NULL;
bool  HikEhomePu::m_isInit = false;
HikEhomePu::HikEhomePu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType)
    : Pu(type, addrcode, name, chnNum, user, pswd, host)
{
    m_protocol = PROTO_TYPE_EHOME;
    m_port = port;
    m_domainType = domainType;
	m_bStartCMS = FALSE;
	m_lListenCMS = -1;
	EHome_Pu = this;
}

HikEhomePu::~HikEhomePu()
{
	/*去初始化库*/
	NET_ECMS_Fini();
	NET_ESTREAM_Fini();
	EHome_Pu = NULL;
}

//iUserID [OUT] 用户ID，由SDK分配
BOOL CALLBACK HikEhomePu::RegisterCallBack(LONG iUserID, DWORD dwDataType, void *pOutBuffer, DWORD dwOutLen, void *pInBuffer, DWORD dwInLen, void *pUser)
{
	NET_EHOME_DEV_REG_INFO_V12 *pDevInfo = (NET_EHOME_DEV_REG_INFO_V12 *)pOutBuffer;//设备注册信息结构体
	NET_EHOME_SERVER_INFO *pServInfo = (NET_EHOME_SERVER_INFO *)pInBuffer;//服务信息结构体
	HikEhomePu *pu = (HikEhomePu *)pUser;
	string deviceID = (char*)pDevInfo->struRegInfo.byDeviceID;//获取摄像机的deviceID
	string devProtocolVersion = (char*)pDevInfo->struRegInfo.byDevProtocolVersion;;//获取设备支持的ISUP协议版本 2.0
	if (ENUM_DEV_ON == dwDataType)    //设备上线   
	{
		ERRI_MSV("EHome Device Online:DeviceID:%s\tProtocolVersion:%s\tUserID%d\n", deviceID.c_str(), devProtocolVersion.c_str(), iUserID);
		if (pDevInfo != NULL)
		{
			pServInfo->dwTimeOutCount = 6;//心跳超时次数
			pServInfo->dwKeepAliveSec = 15;//心跳间隔
			{
				AutoLock locker(pu->m_deviceIDToUserId._mtx);
				pu->m_deviceIDToUserId[deviceID] = iUserID;
			}
		}
	}

	else if (ENUM_DEV_OFF == dwDataType)    //设备下线   
	{
		ERRI_MSV("EHome Device OFFline:DeviceID:%s\tProtocolVersion:%s\tUserID%d\n", deviceID.c_str(), devProtocolVersion.c_str(), iUserID);
		map<string, LONG>::iterator i;
		string deviceID;
		for (i = pu->m_deviceIDToUserId.begin(); i != pu->m_deviceIDToUserId.end();)
		{
			if (iUserID == i->second)
			{
				deviceID = i->first;
				pu->m_deviceIDToUserId.erase(i);
				break;
			}
			else
			{
				i++;
			}
		}

		map<string, LONG>::iterator j;
		for (j = pu->m_deviceIDToTalkHandle.begin(); j != pu->m_deviceIDToTalkHandle.end(); ++j)
		{
			if (deviceID == j->first)
			{
				if (-1 != j->second)
				{
					//停止语音对讲
					NET_ECMS_StopVoiceTalk(j->second);
				}
				j->second = -1;
				pu->m_deviceIDToTalkHandle.erase(j);
		      break;
			}
		}

		NET_ECMS_ForceLogout(iUserID);
	}
	return TRUE;
}

/*预览回调函数，转发实时码流*/
void CALLBACK PreviewDataCallback(LONG iPreviewHandle, NET_EHOME_PREVIEW_CB_MSG *pPreviewCBMsg, void *pUserData)
{
	char *deviceID = (char *)pUserData;
	HikEhomeRealGu *gu = NULL;
	for (list<RealGu *>::iterator i = EHome_Pu->m_realGu.begin(); i != EHome_Pu->m_realGu.end(); ++i)
	{
		if (deviceID == (*i)->name())
		{
			gu = (HikEhomeRealGu*)(*i);
		}
	}
	if (NULL != gu)
	{
		gu->m_lRealHandle = iPreviewHandle;//保存预览句柄
		gu->m_deviceID = deviceID;
		void* data = pPreviewCBMsg->pRecvdata;
		int size = pPreviewCBMsg->dwDataLen;

		//inputstreamdata(pGu->psparser, (uint8_t*)data, size, 0,0,0);

		gu->notifyPrivStream((void *)data, size, ENCODE_H264, TAG_HIK_N);
		gu->traffic(size);
		gu->m_lastpts = sys->currentTimeMillis();
	}
}

/*预览连接请求的响应回调函数*/
BOOL CALLBACK PreviewNewlinkCallback(LONG lPreviewHandle, NET_EHOME_NEWLINK_CB_MSG *pNewLinkCBMsg, void *pUserData)
{
	NET_EHOME_PREVIEW_DATA_CB_PARAM struDataCB = { 0 };//预览回调参数
	struDataCB.fnPreviewDataCB = PreviewDataCallback;  //预览回调函数
	char *deviceID = new char[256];
	memcpy(deviceID, pNewLinkCBMsg->szDeviceID, 256);
	struDataCB.pUserData = deviceID;
	struDataCB.byStreamFormat = 0;//封装格式：0- PS 
	/*SMS注册回调函数接收来自设备的实时码流*/
	if (NET_ESTREAM_SetPreviewDataCB(lPreviewHandle, &struDataCB))
	{
		ERRI_MSV("NET_ESTREAM_SetPreviewDataCB succeed");
		return TRUE;
	}
	else
	{
		ERRE_MSV("NET_ESTREAM_SetPreviewDataCB failed, error code: %d\n", NET_ESTREAM_GetLastError());
		return FALSE;
	}
}

/*异常回调函数*/
void CALLBACK PreviewExceptionCallback(DWORD    dwType, LONG      iUserID, LONG      iHandle, void     *pUser)
{
	DWORD dwError = NET_ESTREAM_GetLastError();
	switch (dwType)
	{
	case EHOME_PREVIEW_EXCEPTION://预览异常
		ERRE_MSV("Preview exception, handle=%d, Error:%d", iHandle, dwError);
		break;
	case EHOME_PLAYBACK_EXCEPTION://回放异常
		ERRE_MSV("Playback exception, handle=%d, Error:%d", iHandle, dwError);
		break;
	case EHOME_AUDIOTALK_EXCEPTION://语音对讲取流异常
		ERRE_MSV("Dualtalk exception, handle=%d, Error:%d", iHandle, dwError);
		break;
	case EHOME_CMSALARM_EXCEPTION://报警接收异常
		ERRE_MSV("Alarm exception, handle=%d, Error:%d", iHandle, dwError);
		break;
	default:
		ERRE_MSV("Unknown exception, handle=%d, Error:%d", iHandle, dwError);
		break;
	}
}

int
HikEhomePu::start()//must return 0
{
	int res = 0;
	SHL_Json::Reader reader;
	SHL_Json::Value rvalue;
	ifstream in("lmt.json", ios::binary);
	if (!in.is_open())
	{
		ERRE_MSV("open lmt.json failed!");
		res = -1;
	}
	if (!reader.parse(in, rvalue, false))
	{
		ERRE_MSV("SHL_Json parse error");
		res = -1;
	}
	//ASSERT(0 == res);
	if (0 == res)
	{
		ExternalHost = rvalue["External_IP"].asString();
		ExternalPort = rvalue["Ehome_PORT"].asInt();
	}
	if (Pu::start() < 0)
	{
		return -1;
	}
    m_init = TRUE;


	if (m_isInit == false)
	{
		initEhome(m_port, ExternalPort);
	}

    return 0;
}

void
HikEhomePu::stop()
{
	if (!m_init)
	{
		return;
	}
	m_init = false;
	if (-1 != m_lListenCMS)
	{
		if (!NET_ECMS_StopListen(m_lListenCMS))
		{
			ERRE_MSV("NET_ECMS_StopListen failed, error code: %d\n", NET_ECMS_GetLastError());
		}
		m_lListenCMS = -1;
	}
	if (-1 != m_lListenSMS)
	{
		//调用接口后，监听端口的所有链接将被断开
		if (!NET_ESTREAM_StopListenPreview(m_lListenSMS))//停止SMS监听服务并断开与设备的连接
		{
			ERRE_MSV("NET_ESTREAM_StopListenPreview failed, error code: %d\n", NET_ECMS_GetLastError());
		}
		m_lListenSMS = -1;
	}
	logout();
	Pu::stop();
}

int        
HikEhomePu::initEhome(int cport, int sport)
{
	/*初始化库*/
	NET_ECMS_Init();
	NET_ESTREAM_Init();

	//login
	NET_EHOME_CMS_LISTEN_PARAM struCMSListenPara = { 0 };//CMS监听参数
	string ip = "0.0.0.0";
	memcpy(struCMSListenPara.struAddress.szIP, ip.c_str(), sizeof(struCMSListenPara.struAddress.szIP));
	struCMSListenPara.struAddress.wPort = cport;
	struCMSListenPara.fnCB = RegisterCallBack;//注册回调函数
	struCMSListenPara.pUserData = this;
	m_lListenCMS = NET_ECMS_StartListen(&struCMSListenPara);//开启CMS监听服务注册回调函数接收设备注册信息
	if (-1 == m_lListenCMS)
	{
		ERRE_MSV("NET_ECMS_StartListen failed, error code: %d\n", NET_ECMS_GetLastError());
		return -1;
	}
	else
	{
		ERRI_MSV("NET_ECMS_StartListen succeed\n");
	}	
	
	NET_EHOME_LISTEN_PREVIEW_CFG struListen = { 0 };//预览监听参数	
	memcpy(struListen.struIPAdress.szIP, ip.c_str(), sizeof(struListen.struIPAdress.szIP));
	struListen.struIPAdress.wPort = (unsigned short)(HikEhomePu::ExternalPort); //SMS监听端口号
	struListen.fnNewLinkCB = PreviewNewlinkCallback;
	struListen.pUser = NULL;
	struListen.byLinkMode = 0; //监听请求的接入方式 0：TCP，1：UDP
	m_lListenSMS = NET_ESTREAM_StartListenPreview(&struListen);//开启SMS监听服务接收来自设备的连接请求
	if (-1 == m_lListenSMS)
	{
		int code = NET_ESTREAM_GetLastError();
		ERRE_MSV("NET_ESTREAM_StartListenPreview failed, error code: %d\n", code);
		return -1;
	}
	else
	{
		ERRI_MSV("NET_ESTREAM_StartListenPreview succeed\n");
	}
	NET_ESTREAM_SetExceptionCallBack(0, 0, PreviewExceptionCallback, NULL);
	
	return 0;
}

int
HikEhomePu::login()
{
	if (m_login)
	{
		return -1;
	}


	m_login = true;

	return 0;
}

void
HikEhomePu::logout()
{
   if (!m_login)
   {
	   return;
   }
   m_login = false;
}

int
HikEhomePu::ptzCtrl(int cmd, int channelNo, int speed, int preset, bool stop)
{
	BOOL res = true;
	string deviceID = guname(channelNo);
	tmap<string, LONG>::iterator pos = m_deviceIDToUserId.find(deviceID);
	if (pos == m_deviceIDToUserId.end())
	{
		return -1;
	}
	LONG m_lUserID = pos->second;
	int ptzCmd = -1;
	switch (cmd)
	{
	case PTZ_CMD_UP:
		ptzCmd = PTZ_UP;
		break;
	case PTZ_CMD_DOWN:
		ptzCmd = PTZ_DOWN;
		break;
	case PTZ_CMD_LEFT:
		ptzCmd = PTZ_LEFT;
		break;
	case PTZ_CMD_RIGHT:
		ptzCmd = PTZ_RIGHT;
		break;
	case PTZ_CMD_UPLEFT:
		ptzCmd = PTZ_UPLEFT;
		break;
	case PTZ_CMD_UPRIGHT:
		ptzCmd = PTZ_UPRIGHT;
		break;
	case PTZ_CMD_DOWNLEFT:
		ptzCmd = PTZ_DOWNLEFT;
		break;
	case PTZ_CMD_DOWNRIGHT:
		ptzCmd = PTZ_DOWNRIGHT;
		break;
	case PTZ_CMD_ZOOMIN:
		ptzCmd = PTZ_ZOOMIN;
		break;
	case PTZ_CMD_ZOOMOUT:
		ptzCmd = PTZ_ZOOMOUT;
		break;
	case PTZ_CMD_FOCUSNEAR:
		ptzCmd = PTZ_FOCUSNEAR;
		break;
	case PTZ_CMD_FOCUSFAR:
		ptzCmd = PTZ_FOCUSFAR;
		break;
	case PTZ_CMD_IRISOPEN:
		ptzCmd = PTZ_IRISSTARTUP;
		break;
	case PTZ_CMD_IRISCLOSE:
		ptzCmd = PTZ_IRISSTOPDOWN;
		break;
	case PTZ_CMD_AUTOSCAN:
		ptzCmd = PTZ_AUTO;
		break;

	case PTZ_CMD_SETPRESET:
		{
			NET_EHOME_PRESET_PARAM param = { 0 };
			param.byPresetCmd = 1;//1：设置预置点    2：清除预置点     3：转到预置点
			param.dwSize = sizeof(NET_EHOME_PRESET_PARAM);
			param.dwPresetIndex = preset;

			NET_EHOME_REMOTE_CTRL_PARAM struCtrlParam = { 0 };
			struCtrlParam.dwSize = sizeof(struCtrlParam);
			long lChannel = channelNo + 1;
			struCtrlParam.lpCondBuffer = &lChannel;
			struCtrlParam.dwCondBufferSize = sizeof(lChannel);
			struCtrlParam.lpInbuffer = &param;
			struCtrlParam.dwInBufferSize = sizeof(param);
			res = NET_ECMS_RemoteControl(m_lUserID, NET_EHOME_PRESET_CTRL, &struCtrlParam);
			return (res == TRUE ? 0 : -1);
		}
	case PTZ_CMD_CLEARPRESET:
		{
			return 0;
		}
	case PTZ_CMD_CALLPRESET:
		{
			NET_EHOME_PRESET_PARAM param = { 0 };
			param.byPresetCmd = 3;//1：设置预置点    2：清除预置点     3：转到预置点
			param.dwSize = sizeof(NET_EHOME_PRESET_PARAM);
			param.dwPresetIndex = preset;

			NET_EHOME_REMOTE_CTRL_PARAM struCtrlParam = { 0 };
			struCtrlParam.dwSize = sizeof(struCtrlParam);
			long lChannel = channelNo + 1;
			struCtrlParam.lpCondBuffer = &lChannel;
			struCtrlParam.dwCondBufferSize = sizeof(lChannel);
			struCtrlParam.lpInbuffer = &param;
			struCtrlParam.dwInBufferSize = sizeof(param);
			res = NET_ECMS_RemoteControl(m_lUserID, NET_EHOME_PRESET_CTRL, &struCtrlParam);
			return (res == TRUE ? 0 : -1);
		}

	case PTZ_CMD_RAIN_ON:
	case PTZ_CMD_RAIN_OFF:
		ptzCmd = PTZ_WIPER;
		return 0;
	case PTZ_CMD_AUXLIGHT_ON:
	case PTZ_CMD_AUXLIGHT_OFF:
		ptzCmd = PTZ_LIGHT;
		return 0;
	case PTZ_CMD_LOCKCTRL:    
	{
		 return 0;
	}

	case PTZ_CMD_UNLOCKCTRL:
	{
		return 0;
	}
	}
	float rate = 7 / (float)10;
	int actualSpeed = (int)(speed * rate + 0.50);

	if (actualSpeed < 0 || actualSpeed > 7)
	{
		actualSpeed = 4;
	}
	NET_EHOME_PTZ_PARAM lpStruParam = { 0 };//PTZ参数
	lpStruParam.byPTZCmd = ptzCmd;
	lpStruParam.byAction = stop;//0:start    1:stop
	lpStruParam.dwSize   = sizeof(lpStruParam);
	lpStruParam.bySpeed  = actualSpeed;

	NET_EHOME_REMOTE_CTRL_PARAM struCtrlParam = { 0 };//云台控制参数
	struCtrlParam.dwSize = sizeof(NET_EHOME_REMOTE_CTRL_PARAM);
    int lChannel = 1;
	struCtrlParam.lpCondBuffer = &lChannel;
	struCtrlParam.dwCondBufferSize = sizeof(lChannel);
	struCtrlParam.lpInbuffer = &lpStruParam;
	struCtrlParam.dwInBufferSize = sizeof(NET_EHOME_PTZ_PARAM);
	res = NET_ECMS_RemoteControl(m_lUserID, NET_EHOME_PTZ_CTRL, &struCtrlParam);
	LOGI_MSV("channelNo: %d, cmd: %d, actualSpeed: %d, preset: %d, stop: %d, result: %d", channelNo + 1, ptzCmd, actualSpeed, preset, stop, res);
    DWORD ret = NET_ECMS_GetLastError();
	return (res == TRUE ? 0 : -1);
}

int
HikEhomePu::ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop)
{
	int channelNo = GetChanNoByCode(addrcode);
	return ptzCtrl(cmd, channelNo, speed, preset, stop);
}

int
HikEhomePu::queryRecord(int channelNo, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json)
{
	SHL_Json::Value value;
	SHL_Json::Value array;
	string deviceID = guname(channelNo);
	tmap<string, LONG>::iterator pos = m_deviceIDToUserId.find(deviceID);
	if (pos == m_deviceIDToUserId.end())
	{
		return -1;
	}
	LONG m_lUserID = pos->second;
	//查找视频文件
	NET_EHOME_REC_FILE_COND struFindCond = {0}; 
	memset(&struFindCond, 0, sizeof(struFindCond));
	struFindCond.dwChannel = 1;//通道号
	struFindCond.dwRecType = 0xff;         //全部录像类型
	struFindCond.dwStartIndex = 0;         //搜索起始位置 
	struFindCond.dwMaxFileCountPer = 8;    //单次可搜索的最多文件数量
	//搜索开始时间 
	struFindCond.struStartTime.wYear    = start.year; 
	struFindCond.struStartTime.byMonth  = start.month;
	struFindCond.struStartTime.byDay    = start.day; 
	struFindCond.struStartTime.byHour   = start.hour; 
	struFindCond.struStartTime.byMinute = start.minute;
	struFindCond.struStartTime.bySecond = start.second;
	//搜索结束时间 
	struFindCond.struStopTime.wYear     = end.year; 
	struFindCond.struStopTime.byMonth   = end.month; 
	struFindCond.struStopTime.byDay     = end.day; 
	struFindCond.struStopTime.byHour    = end.hour; 
	struFindCond.struStopTime.byMinute  = end.minute; 
	struFindCond.struStopTime.bySecond  = end.second;
	//开始查找文件
	LONG lFindHandle = NET_ECMS_StartFindFile_V11(m_lUserID, ENUM_SEARCH_RECORD_FILE, &struFindCond, sizeof(struFindCond));
	if (lFindHandle < 0) 
	{             
		ERRE_MSV("NET_ECMS_StartFindFile_V11 failed, error code: %d\n", NET_ECMS_GetLastError());   
		return -1;
	} 
	LONG iRet = -1;
	int index = 0;
	while (1)
	{
		NET_EHOME_REC_FILE struFileInfo = { 0 };
		memset(&struFileInfo, 0, sizeof(NET_EHOME_REC_FILE));
		iRet = NET_ECMS_FindNextFile_V11(lFindHandle, &struFileInfo, sizeof(struFileInfo));//逐个获取检索结果
		if (ENUM_GET_NEXT_STATUS_SUCCESS == iRet)
		{
			index++;
			if (index < fromIndex || index > toIndex)
			{
				continue;
			}
			SHL_Json::Value record;
			record["filename"] = struFileInfo.sFileName;
			record["fileurl"] = string("//") + m_host + string(":") + std::to_string(m_port) + string("/") + string(struFileInfo.sFileName);
			record["channelNo"] = (int)channelNo;
			record["size"] = (int)struFileInfo.dwFileSize;
			record["index"] = index;
			record["locked"] = 0;//EHome查不到，置0
			datetime_s start;
			datetime_s end;
			start.year = struFileInfo.struStartTime.wYear;
			start.month =struFileInfo.struStartTime.byMonth;
			start.day = struFileInfo.struStartTime.byDay;
			start.hour = struFileInfo.struStartTime.byHour;
			start.minute = struFileInfo.struStartTime.byMinute;
			start.second = struFileInfo.struStartTime.bySecond;
			record["start"] = sys->strftime(start);
			end.year = struFileInfo.struStopTime.wYear;
			end.month = struFileInfo.struStopTime.byMonth;
			end.day = struFileInfo.struStopTime.byDay;
			end.hour = struFileInfo.struStopTime.byHour;
			end.minute = struFileInfo.struStopTime.byMinute;
			end.second = struFileInfo.struStopTime.bySecond;

			record["end"] = sys->strftime(end);

			array.append(record);
		}

		else if (ENUM_GET_NETX_STATUS_NEED_WAIT == iRet)
		{
			sys->sleep(1);
			continue;
		}
		else if (ENUM_GET_NEXT_STATUS_FINISH == iRet || ENUM_GET_NETX_STATUS_NO_FILE == iRet)
		{
			NET_ECMS_StopFindFile(lFindHandle);
			break;
		}
		else       
		{
			ERRE_MSV("NET_ECMS_FindNextFile_V11 err\n");         
			NET_ECMS_StopFindFile(lFindHandle);
			break; 
		}
	}
	value["records"] = array;
	value["realNum"] = index;
	value["fromIndex"] = fromIndex;
	value["toIndex"] = fromIndex + array.size();

	SHL_Json::StreamWriterBuilder writer;
	json = SHL_Json::writeString(writer, value);
	return 0;
}
int 
HikEhomePu::queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode)
{
	int channelNo = GetChanNoByCode(addrcode);
	return queryRecord(channelNo, type, start, end, fromIndex, toIndex, json);
}
