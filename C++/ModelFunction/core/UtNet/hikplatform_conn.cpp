#include "hikplatform_conn.h"
#include "hikplatform_preview.h"
#include "json/json.h"
#include "commonc1.h"
#include "httppostintf.h"

#include "hikplatform_alarm.h"
#include "hikplatform_record.h"
#include "hikplatform_download.h"
#include "hikplatform_download_bytime.h"
#include "hikplatform_dualtalkEx.h"

#ifdef _DEBUG
//#pragma comment(lib, "HttpUtilLibD.lib")
#else
//#pragma comment(lib, "HttpUtilLib.lib")
#endif

//#pragma comment(lib, "VideoSDK.lib")

void loadHikplatform()
{
	Video_Init(NULL);

	InitHttp();
}

void destroyHikplatform()
{
   Video_Fini();
}

HikPlatformConn::HikPlatformConn()
    :BasePlatformConn()
{
    memset(m_s8Ip, 0, sizeof(m_s8Ip));
	m_u16Port = 0;
	memset(m_s8AppKey, 0, sizeof(m_s8AppKey));
	memset(m_s8AppSec, 0, sizeof(m_s8AppSec));
	m_fConnCallback = NULL;
	m_pfnAlarmCallback = NULL;	
    m_mainMsgId = NULL;
	m_bExit = FALSE;
	m_pUser = NULL;
	m_bConn = FALSE;
    m_ps8CamName = "";
}

HikPlatformConn::~HikPlatformConn()
{

}

BOOL HikPlatformConn::Start(const NET_CONN_PARAM_S *pParam)
{
    strcpy(m_s8Ip, pParam->s8Ip);
	m_u16Port = pParam->u16Port;
	strcpy(m_s8AppKey, pParam->ps8Username);
	strcpy(m_s8AppSec, pParam->ps8Password);
	strcpy(m_s8LoalIp, pParam->ps8ClientIp);

	m_tmPrev = time(NULL);
	m_fConnCallback = pParam->fConnCallback;
	m_pfnAlarmCallback = pParam->fAlarmCallback;
	m_pUser  = pParam->pUser;

	HikPlatformAlarm * alarm = HikPlatformAlarm::getInstance();
    alarm->startService("", 6000);

	CreateJoinableThread(MainMsgFun, this, &m_mainMsgId);

	return TRUE;
}

VOID *HikPlatformConn::MainMsgFun(VOID *pArg)
{
	HikPlatformConn *pMainMsg = (HikPlatformConn *)pArg;
	pMainMsg->MainMsgLoop();
	
	return 0;
}

VOID HikPlatformConn::MainMsgLoop()
{
	time_t tmNow = time(NULL);

	do 
	{
		m_tmPrev   = tmNow;
		if (!m_bConn)
		{		
			m_bConn = QueryDefaultOrg();
			if (m_bConn)
			{
				HIKPLAT_CONNECTION_STATUS(MSG_LOGIN_SUCCESS);	
				QueryResource();
				
				//RegisterAlarm();
			}
			else
			{
				if (m_bExit)
				{
					break;
				}

				HIKPLAT_CONNECTION_STATUS(MSG_LOGIN_FAIL);
				Msleep(1000);
				continue;
			}
		}

		do 
		{
			if (m_bExit)
			{
				break;
			}

			tmNow = time(NULL);
			if (tmNow - m_tmPrev >= 120)
			{
				m_tmPrev = tmNow;
				break;
			}
			else
			{
				Msleep(100);
			}
		} while (1);
		
	} while (!m_bExit);
}

BOOL HikPlatformConn::QueryDefaultOrg()
{
	S8 s8Buf[512] = {0};
	sprintf(s8Buf, "http://%s:%d/artemis/api/resource/v1/org/rootOrg", m_s8Ip, m_u16Port);

	SHL_Json::Value jsonReq;
	SHL_Json::FastWriter req;
	string strReq = req.write(jsonReq);
	
	char * strResult = HttpPost(s8Buf, strReq.c_str(), m_s8AppKey, m_s8AppSec, 5);   
	if (NULL == strResult)
	{
		return FALSE;
	}

#if 0
    FILE *fp;
    fp = fopen("C:\\Users\\zhangshenyu\\Desktop\\DefaultOrg.txt", "w");
    if(fp ==NULL)
        printf("打开文件DefaultOrg.txt失败\n");
    else
        fprintf(fp, strResult);
    fclose(fp);
#endif

	SHL_Json::Value rootNode;
	SHL_Json::Reader reader;
	if (!reader.parse(strResult, rootNode))
    {
		delete [] strResult;
		return FALSE;
    }
	delete [] strResult;
	
	std::string code = rootNode["code"].asString();
	if ("0" != code)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL HikPlatformConn::QueryResource()
{
    //查询编码设备  
	{
		AutoLock lock(m_mtxDevIdx);
		if (m_mapIpPortToDevCode.size() != 0 && 
			m_mapIpChnToCameraIndex.size() != 0)
		{
			return TRUE;
		}
		m_mapIpPortToDevCode.clear();

		S8 s8Buf[512] = {0};
		sprintf(s8Buf, "http://%s:%d/artemis/api/irds/v1/deviceResource/resources", m_s8Ip, m_u16Port);
		
		SHL_Json::Value jsonReq;
		jsonReq["pageNo"]   = 1;
		jsonReq["pageSize"] = 1000;
        jsonReq["resourceType"] = "encodeDevice";

        SHL_Json::FastWriter req;
		string strReq = req.write(jsonReq);
		
		char * strResult = HttpPost(s8Buf, strReq.c_str(), m_s8AppKey, m_s8AppSec, 5);
		
		if (NULL == strResult)
		{
			return FALSE;
		}

#if 0
    FILE *fp;
    fp = fopen("C:\\Users\\zhangshenyu\\Desktop\\resources.txt", "w");
    if(fp ==NULL)
        printf("打开文件resources.txt失败\n");
    else
        fprintf(fp, strResult);
    fclose(fp);
#endif
		
		SHL_Json::Value rootNode;
		SHL_Json::Reader reader;
		if (!reader.parse(strResult, rootNode))
		{
			delete [] strResult;
			return FALSE;
		}
		delete [] strResult;

		SHL_Json::Value data = rootNode["data"];
		int total = data["total"].asInt();
		SHL_Json::Value devlists = data["list"];
		for (int index = 0; index < devlists.size(); ++index)
		{
            SHL_Json::Value ele = devlists[index];

            string ip = ele["ip"].asString();
			string port = ele["port"].asString();

			string strIpPort = ip+":"+port;
			string strDevCode = ele["indexCode"].asString();

			m_mapIpPortToDevCode[strIpPort] = strDevCode;

            //for test 海康单兵暂无固定ip，需特殊处理
            string strName = ele["name"].asString();
            m_mapNameToDevCode[strName] = strDevCode;
		}
	}

	//查询监控点列表

	AutoLock lock(m_mtxCamIdx);
	m_mapIpChnToCameraIndex.clear();

	S8 s8Buf[512] = {0};
	sprintf(s8Buf, "http://%s:%d/artemis/api/resource/v1/camera/advance/cameraList", m_s8Ip, m_u16Port);
	
	SHL_Json::Value jsonReq;

	for (int index = 1; index < 10; ++index)
	{	
		jsonReq["pageNo"]   = index;
		jsonReq["pageSize"] = 1000;
		
		SHL_Json::FastWriter req;
		string strReq = req.write(jsonReq);
		
		char * strResult = HttpPost(s8Buf, strReq.c_str(), m_s8AppKey, m_s8AppSec, 5);  

		if (NULL == strResult)
		{
			return FALSE;
		}

#if 0
    FILE *fp;
    fp = fopen("C:\\Users\\zhangshenyu\\Desktop\\cameraList.txt", "w");
    if(fp ==NULL)
        printf("打开文件cameraList.txt失败\n");
    else
        fprintf(fp, strResult);
    fclose(fp);
#endif
		
		SHL_Json::Value rootNode;
		SHL_Json::Reader reader;
		if (!reader.parse(strResult, rootNode))
		{
			delete [] strResult;
			return FALSE;
		}
		delete [] strResult;
		
		std::string code = rootNode["code"].asString();
		if ("0" != code)
		{
			return FALSE;
		}	
		
		SHL_Json::Value data = rootNode["data"];
		int total = data["total"].asInt();
		SHL_Json::Value camlists = data["list"];
		for (int index = 0; index < camlists.size(); ++index)
		{
			SHL_Json::Value ele = camlists[index];
			string strCameraCode = ele["cameraIndexCode"].asString();
			string strDevCode    = ele["encodeDevIndexCode"].asString();
			string strChn        = ele["channelNo"].asString();
			
			char info[128] = {0};
			sprintf(info, "%s-%s", strDevCode.c_str(), strChn.c_str());
			map<string,string>::iterator iter = m_mapIpChnToCameraIndex.find(info);
			if (iter == m_mapIpChnToCameraIndex.end())
			{
				m_mapIpChnToCameraIndex[info] = strCameraCode;
			}
		}
	}

	return TRUE;
}

VOID HikPlatformConn::Stop()
{
   m_bExit = TRUE;
   ThreadJoin(m_mainMsgId);
}

BOOL HikPlatformConn::PTZControl(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam)
{
	S8 s8Buf[512] = {0};

	char ipPort[512] = {0};
	sprintf(ipPort, "%s:%d", pParam->ps8DeviceIp, pParam->u16DevicePort);
	string strDevCode;
	{
		AutoLock lock(m_mtxDevIdx);
		map<string,string>::iterator pos = m_mapIpPortToDevCode.find(ipPort);
		if (pos == m_mapIpPortToDevCode.end())
		{
			return FALSE;
		}
		
		strDevCode = pos->second;
	}

	char ipDevChn[512] = {0};
	sprintf(ipDevChn, "%s-%d", strDevCode.c_str(), (pParam->u32ChnNO+1));

	string strCamCode;
	{
		AutoLock lock(m_mtxCamIdx);
		map<string, string>::iterator iter = m_mapIpChnToCameraIndex.find(ipDevChn);
		if (iter == m_mapIpChnToCameraIndex.end())
		{
			return FALSE;
		}
		
		strCamCode = iter->second;
	}

    SHL_Json::Value jsonReq;

	jsonReq["cameraIndexCode"] = strCamCode;
	if (u32Cmd != PTZ_CMD_SETPRESET)
	{
		sprintf(s8Buf, "http://%s:%d/artemis/api/video/v1/ptzs/controlling", m_s8Ip, m_u16Port);
		
		jsonReq["action"]  = pParam->stNormal.u32Flag;
		
		std::string strCommand = "";
		switch (u32Cmd)
		{
		case PTZ_CMD_UP:
			strCommand = "UP";
			break;
		case PTZ_CMD_DOWN:
			strCommand = "DOWN";
			break;
		case PTZ_CMD_LEFT:
			strCommand = "LEFT";
			break;
		case PTZ_CMD_RIGHT:
			strCommand = "RIGHT";
			break;
		case PTZ_CMD_ZOOMIN:
			strCommand = "ZOOM_IN";
			break;
		case PTZ_CMD_ZOOMOUT:
			strCommand = "ZOOM_OUT";
			break;
		case PTZ_CMD_FOCUSNEAR:
			strCommand = "FOCUS_NEAR";
			break;
		case PTZ_CMD_FOCUSFAR:
			strCommand = "FOCUS_FAR";
			break;
		case PTZ_CMD_IRISOPEN:
			strCommand = "IRIS_ENLARGE";
			break;
		case PTZ_CMD_IRISCLOSE:
			strCommand = "IRIS_REDUCE";
			break;
		case PTZ_CMD_SETPRESET:
			strCommand = "SET_PRESET";
			break;
		case PTZ_CMD_CALLPRESET:
			strCommand = "GOTO_PRESET";
			break;
		default:
			return FALSE;
		}
		
		if (pParam->stNormal.u32Speed == 0)
		{
			jsonReq["speed"]        = 1;
		}
		else if (jsonReq["speed"] > 10)
		{
			jsonReq["speed"]        = 100;
		}
		else
		{
			jsonReq["speed"]        = pParam->stNormal.u32Speed * 10;
		}
		
		jsonReq["command"]      = strCommand;
		if (PTZ_CMD_CALLPRESET == u32Cmd)
		{
			jsonReq["speed"]        = 1;
			jsonReq["presetIndex"]  = pParam->stPreset.u32Index;
		}
	}
	else
	{
		sprintf(s8Buf, "http://%s:%d/artemis/api/video/v1/presets/addition", m_s8Ip, m_u16Port);
		S8 s8PresetName[128] = {0};
		sprintf(s8PresetName, "preset%d", pParam->stPreset.u32Index);
		jsonReq["presetName"] = s8PresetName;
		jsonReq["presetIndex"] = pParam->stPreset.u32Index;
	}

	SHL_Json::FastWriter req;
	string strReq = req.write(jsonReq);

	char *strResult = HttpPost(s8Buf, strReq.c_str(), m_s8AppKey, m_s8AppSec, 5);  
	if (NULL == strResult)
	{
		return FALSE;
	}

	SHL_Json::Value rootNode;
	SHL_Json::Reader reader;
	if (!reader.parse(strResult, rootNode))
    {
		delete [] strResult;
		return FALSE;
    }
	delete [] strResult;
	
	std::string code = rootNode["code"].asString();
	if ("0" != code)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL HikPlatformConn::PTZControlEx(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam)
{
	return PTZControl(u32Cmd, pParam);
}

HANDLE HikPlatformConn::StartPreview(const NET_PVIEW_PARAM_S *pParam)
{
    HikPlatformPreview * preview = new HikPlatformPreview(this);

	return preview->Start(pParam);
}

VOID * StopHikPreview(void *hPView)
{
    HikPlatformPreview *pPView = (HikPlatformPreview*)(hPView); 
    
	pPView->Stop(NULL);
	
	delete pPView;

	return NULL;
}

VOID HikPlatformConn::StopPreview(HANDLE *hPView)
{
	PreviewClient      * pPViewClient = (PreviewClient *)(*hPView);
	HikPlatformPreview * pPView= (HikPlatformPreview*)pPViewClient->pPView;

#if 0
	THREAD trd;
	CreateDetachThread(StopHikPreview, pPView, &trd);	
#else        
    pPView->Stop(NULL);
	
    delete pPView;
#endif
    
}

BOOL HikPlatformConn::QueryRecord(const NET_QRECORD_PARAM_S *pParam)
{
    S8 s8Buf[512] = {0};
	sprintf(s8Buf, "http://%s:%d/artemis/api/video/v1/cameras/playbackURLs", m_s8Ip, m_u16Port);
	
    S8 s8BeginTime[128] = {0};
	S8 s8EndTime[128]   = {0};
	sprintf(s8BeginTime, "%04d-%02d-%02dT%02d:%02d:%02d.000+08:00", pParam->stBegin.u32Year, pParam->stBegin.u32Month, pParam->stBegin.u32Day,
		pParam->stBegin.u32Hour, pParam->stBegin.u32Minute, pParam->stBegin.u32Second);
    sprintf(s8EndTime, "%04d-%02d-%02dT%02d:%02d:%02d.000+08:00", pParam->stEnd.u32Year, pParam->stEnd.u32Month, pParam->stEnd.u32Day,
		pParam->stEnd.u32Hour, pParam->stEnd.u32Minute, pParam->stEnd.u32Second);

	std::string strUUid = "";
	do 
	{	
        std::string name ="";
		char ipPort[512] = {0};
	    map<string,string>::iterator pos;

        if (pParam->u16DevicePort == 0)
        {
            name = pParam->ps8DeviceIp;
	        AutoLock lock(m_mtxDevIdx);
	        pos = m_mapNameToDevCode.find(name);
	        if (pos == m_mapNameToDevCode.end())
	        {
		        return FALSE;
	        }
        } 
        else
        {
	        sprintf(ipPort, "%s:%d", pParam->ps8DeviceIp, pParam->u16DevicePort);
	        AutoLock lock(m_mtxDevIdx); 
            pos= m_mapIpPortToDevCode.find(ipPort);
	        if (pos == m_mapIpPortToDevCode.end())
	        {
		        return FALSE;
	        }
        }
		
		string strDevCode = pos->second;
		char ipDevChn[512] = {0};
		sprintf(ipDevChn, "%s-%d", strDevCode.c_str(), (pParam->u32ChnNO+1));

		string strCamCode;
		{
			AutoLock lock(m_mtxCamIdx);
			map<string, string>::iterator iter = m_mapIpChnToCameraIndex.find(ipDevChn);
			if (iter == m_mapIpChnToCameraIndex.end())
			{
				return FALSE;
			}
			
			strCamCode = iter->second;
		}
	
		SHL_Json::Value jsonReq;
		jsonReq["cameraIndexCode"] = strCamCode;
		jsonReq["beginTime"]       = s8BeginTime;
		jsonReq["endTime"]         = s8EndTime;
	//	jsonReq["uuid"]            = strUUid;
		jsonReq["recordLocation"]  = "1";

		SHL_Json::FastWriter req;
		string strReq = req.write(jsonReq);
		
		char *strResult = HttpPost(s8Buf, strReq.c_str(), m_s8AppKey, m_s8AppSec, 5);
		if (NULL == strResult)
		{
			return FALSE;
		}

		SHL_Json::Value rootNode;
		SHL_Json::Reader reader;
		if (!reader.parse(strResult, rootNode))
		{
			delete [] strResult;
			return FALSE;
		}
		delete [] strResult;
		
		std::string code = rootNode["code"].asString();
		if ("0" != code)
		{
			break;
		}

		SHL_Json::Value dataNode = rootNode["data"];
		SHL_Json::Value listNode = dataNode["list"];

		strUUid = dataNode["uuid"].asString();
		std::string recordUrl = dataNode["url"].asString();
	
		for (int index = 0; index < listNode.size(); ++index)
		{
			SHL_Json::Value ele = listNode[index];
			std::string strBeginTime = ele["beginTime"].asString();
			std::string strEndTime = ele["endTime"].asString();
			int sz = ele["size"].asInt();

			S8 s8BeginTime[128] = {0};
			int len = strrchr(strBeginTime.c_str(), '.') - strBeginTime.c_str();
			strncpy(s8BeginTime, strBeginTime.c_str(), len);

			S8 s8EndTime[128] = {0};
			len = strrchr(strEndTime.c_str(), '.') - strEndTime.c_str();
			strncpy(s8EndTime, strEndTime.c_str(), len);

			int year[2], month[2], day[2], hour[2], minute[2], second[2];
            sscanf(s8BeginTime, "%04d-%02d-%02dT%02d:%02d:%02d", &year[0], &month[0], &day[0],
				   &hour[0], &minute[0], &second[0]);
			sscanf(s8EndTime, "%04d-%02d-%02dT%02d:%02d:%02d", &year[1], &month[1], &day[1],
				   &hour[1], &minute[1], &second[1]);

			REC_DETAIL_RSP_S recDetailRsp = {0};
			strcpy(recDetailRsp.s8FileName, recordUrl.c_str());
			recDetailRsp.stBegin.u32Year  = year[0];
			recDetailRsp.stBegin.u32Month = month[0];
			recDetailRsp.stBegin.u32Day   = day[0];
			recDetailRsp.stBegin.u32Hour  = hour[0];
			recDetailRsp.stBegin.u32Minute = minute[0];
			recDetailRsp.stBegin.u32Second = second[0];

			recDetailRsp.stEnd.u32Year  = year[1];
			recDetailRsp.stEnd.u32Month = month[1];
			recDetailRsp.stEnd.u32Day   = day[1];
			recDetailRsp.stEnd.u32Hour  = hour[1];
			recDetailRsp.stEnd.u32Minute = minute[1];
			recDetailRsp.stEnd.u32Second = second[1];

			recDetailRsp.u32Size = sz;
			recDetailRsp.u32ChnNO = pParam->u32ChnNO;
			
			if (pParam->fDetailCallback)
			{
				pParam->fDetailCallback(&recDetailRsp, pParam->pUser);
			}
		}
	} while (0);

	return TRUE;
}

BOOL HikPlatformConn::QueryRecordEx(const NET_QRECORD_PARAM_EX_S *pParamEx)
{
	NET_QRECORD_PARAM_S * pParam = pParamEx->pRecordReq;

    S8 s8Buf[512] = {0};
	sprintf(s8Buf, "http://%s:%d/artemis/api/video/v1/cameras/playbackURLs", m_s8Ip, m_u16Port);
	
    S8 s8BeginTime[128] = {0};
	S8 s8EndTime[128]   = {0};
	sprintf(s8BeginTime, "%04d-%02d-%02dT%02d:%02d:%02d.000+08:00", pParam->stBegin.u32Year, pParam->stBegin.u32Month, pParam->stBegin.u32Day,
		pParam->stBegin.u32Hour, pParam->stBegin.u32Minute, pParam->stBegin.u32Second);
    sprintf(s8EndTime, "%04d-%02d-%02dT%02d:%02d:%02d.000+08:00", pParam->stEnd.u32Year, pParam->stEnd.u32Month, pParam->stEnd.u32Day,
		pParam->stEnd.u32Hour, pParam->stEnd.u32Minute, pParam->stEnd.u32Second);
	
	std::string strUUid = "";
	do 
	{	
        std::string name ="";
		char ipPort[512] = {0};
	    map<string,string>::iterator pos;

        if (pParam->u16DevicePort == 0)
        {
            name = pParam->ps8DeviceIp;
	        AutoLock lock(m_mtxDevIdx);
	        pos = m_mapNameToDevCode.find(name);
	        if (pos == m_mapNameToDevCode.end())
	        {
		        return FALSE;
	        }
        } 
        else
        {
		    sprintf(ipPort, "%s:%d", pParam->ps8DeviceIp, pParam->u16DevicePort);
			AutoLock lock(m_mtxDevIdx);
			pos = m_mapIpPortToDevCode.find(ipPort);
			if (pos == m_mapIpPortToDevCode.end())
			{
				return FALSE;
			}			
        }
		
		string strDevCode = pos->second;

		char ipDevChn[512] = {0};
		sprintf(ipDevChn, "%s-%d", strDevCode.c_str(), (pParam->u32ChnNO+1));

		string strCamCode;
		{
			AutoLock lock(m_mtxCamIdx);
			map<string, string>::iterator iter = m_mapIpChnToCameraIndex.find(ipDevChn);
			if (iter == m_mapIpChnToCameraIndex.end())
			{
				return FALSE;
			}
			
			strCamCode = iter->second;
		}
	
		SHL_Json::Value jsonReq;
		jsonReq["cameraIndexCode"] = strCamCode;
		jsonReq["beginTime"]       = s8BeginTime;
		jsonReq["endTime"]         = s8EndTime;
		//jsonReq["uuid"]            = strUUid;
		jsonReq["recordLocation"]  = "1";
		SHL_Json::FastWriter req;
		string strReq = req.write(jsonReq);
		
		char *strResult = HttpPost(s8Buf, strReq.c_str(), m_s8AppKey, m_s8AppSec, 5);
		if (NULL == strResult)
		{
			return FALSE;
		}
		
		SHL_Json::Value rootNode;
		SHL_Json::Reader reader;
		if (!reader.parse(strResult, rootNode))
		{
			delete [] strResult;
			return FALSE;
		}
		delete [] strResult;
		
		std::string code = rootNode["code"].asString();
		if ("0" != code)
		{
			break;
		}
		
		SHL_Json::Value dataNode = rootNode["data"];
		SHL_Json::Value listNode = dataNode["list"];
		
		strUUid = dataNode["uuid"].asString();
		std::string recordUrl = dataNode["url"].asString();
		
		for (int index = 0; index < listNode.size(); ++index)
		{
			SHL_Json::Value ele = listNode[index];
			std::string strBeginTime = ele["beginTime"].asString();
			std::string strEndTime = ele["endTime"].asString();
			int sz = ele["size"].asInt();
			
			S8 s8BeginTime[128] = {0};
			int len = strrchr(strBeginTime.c_str(), '.') - strBeginTime.c_str();
			strncpy(s8BeginTime, strBeginTime.c_str(), len);
			
			S8 s8EndTime[128] = {0};
			len = strrchr(strEndTime.c_str(), '.') - strEndTime.c_str();
			strncpy(s8EndTime, strEndTime.c_str(), len);
			
			int year[2], month[2], day[2], hour[2], minute[2], second[2];
            sscanf(s8BeginTime, "%04d-%02d-%02dT%02d:%02d:%02d", &year[0], &month[0], &day[0], &hour[0], &minute[0], &second[0]);
			sscanf(s8EndTime, "%04d-%02d-%02dT%02d:%02d:%02d", &year[1], &month[1], &day[1], &hour[1], &minute[1], &second[1]);
			
			REC_DETAIL_RSP_S recDetailRsp = {0};
			strcpy(recDetailRsp.s8FileName, recordUrl.c_str());
			recDetailRsp.stBegin.u32Year  = year[0];
			recDetailRsp.stBegin.u32Month = month[0];
			recDetailRsp.stBegin.u32Day   = day[0];
			recDetailRsp.stBegin.u32Hour  = hour[0];
			recDetailRsp.stBegin.u32Minute = minute[0];
			recDetailRsp.stBegin.u32Second = second[0];
			
			recDetailRsp.stEnd.u32Year  = year[1];
			recDetailRsp.stEnd.u32Month = month[1];
			recDetailRsp.stEnd.u32Day   = day[1];
			recDetailRsp.stEnd.u32Hour  = hour[1];
			recDetailRsp.stEnd.u32Minute = minute[1];
			recDetailRsp.stEnd.u32Second = second[1];
			
			recDetailRsp.u32Size = sz;
			
			if (pParam->fDetailCallback)
			{
				pParam->fDetailCallback(&recDetailRsp, pParam->pUser);
			}
		}
	} while (0);
	
	return TRUE;
}

string HikPlatformConn::FindCameraIndexByIp(string ip, int port, int chn)
{
    char info[128] = {0};
	sprintf(info, "%s-%d-%d", ip.c_str(), port, chn);
	map<string,string>::iterator iter = m_mapIpChnToCameraIndex.find(info);
	if (iter != m_mapIpChnToCameraIndex.end())
	{
		return iter->second;
	}

	return "";
}

BOOL HikPlatformConn::SetDecDataCallBack(const NET_DECFRAME_PARAM *pParam)
{
	HikPlatformPreview *pPView = (HikPlatformPreview *)pParam->hPView;
	if (pPView)
	{
		pPView->SetDecDataCallBack(pParam->pfnDecCb, pParam->hUser);
	}
	else
	{
		return FALSE;
	}

	return TRUE;
}

bool HikPlatformConn::RegisterAlarm()
{
	S8 s8Buf[512] = {0};
	sprintf(s8Buf, "http://%s:%d/artemis/api/eventService/v1/eventSubscriptionByEventTypes", m_s8Ip, m_u16Port);

	S8 s8Req[10*1024] = {0};
	sprintf(s8Req, "\{\"eventTypes\":[131329,131330,131331], \"eventDest\": \"https://%s:%d/eventRcv\"\}", m_s8LoalIp, 6000);

	string strReq = s8Req;
	char *strResult = HttpPost(s8Buf, strReq.c_str(), m_s8AppKey, m_s8AppSec, 5);
	if (NULL == strResult)
	{
		return FALSE;
	}

	SHL_Json::Value rootNode;
	SHL_Json::Reader reader;
	if (!reader.parse(strResult, rootNode))
    {
		delete [] strResult;
		return FALSE;
    }
	delete [] strResult;
	
	std::string code = rootNode["code"].asString();
	if ("0" != code)
	{
		return FALSE;
	}

    HikPlatformAlarm * alarm = HikPlatformAlarm::getInstance();
	alarm->registerAlarm(m_pfnAlarmCallback, this);

	return true;
}


HANDLE   HikPlatformConn::StartRecordPaly(const NET_RECPLAY_PARAM_S *pParam)
{
	HikPlatformRecord *pRecPlay = new HikPlatformRecord(this);
	if (pRecPlay)
	{
		if (pRecPlay->Start(pParam))
		{
			return pRecPlay;
		}
		
		delete pRecPlay;
	}
    return NULL;
}

HANDLE   HikPlatformConn::StartRecordDownload(const NET_RECDOWNLOAD_PARAM_S *pParam)
{
    HikPlatformDownload *pDownload = new HikPlatformDownload(this);			
	if (pDownload)
	{
		if (pDownload->Start(pParam))
		{
			return pDownload;
		}
		
		delete pDownload;
	}

    return NULL;   
}


HANDLE   HikPlatformConn::StartRecordDownloadByTime(const NET_RECDOWNLOAD_BYTIME_PARAM_S *pParam)
{
	HikPlatformDownloadByTime *pDownload = new HikPlatformDownloadByTime(this);
	
	if (pDownload)
	{
		if (pDownload->Start(pParam))
		{
			return pDownload;
		}
		
		delete pDownload;
	}
    return NULL; 
}

HANDLE HikPlatformConn::InitDevTalk(int audioType)
{
    static HikPlatformDualtalkEx * pDualTalkEx = NULL;
    if (NULL != pDualTalkEx)
    {
        return pDualTalkEx;	
    }

    pDualTalkEx = new HikPlatformDualtalkEx(this, audioType);

    if (NULL == pDualTalkEx)
    {	    
        return NULL;
    }
    return pDualTalkEx;			
}

