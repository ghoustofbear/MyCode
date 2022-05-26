#include "hikplatform_record.h"
#include "hikplatform_conn.h"
#include "commonc1.h"
#include <math.h>
#include "json/json.h"

HikPlatformRecord::HikPlatformRecord(HikPlatformConn * pConn)
    :BaseRecord()
{
    m_bExit = FALSE;
	m_trdRecPlay  = NULL;
	m_bRecordExp  = FALSE;
	m_recPlayerId = -1;
	m_iPlaySpeed  = 0;
	m_bDownloadOk = FALSE;
	m_pConn = pConn;
}

HikPlatformRecord::~HikPlatformRecord()
{

}

void CALLBACK RecordMsgCallBack(VIDEO_INT64 i64PlayHandle, int iMsg, void* pUserData)
{
	HikPlatformRecord * pThis = (HikPlatformRecord *)pUserData;
	
	//12:È¡Á÷Òì³£
	if (12 == iMsg && pThis)
	{
		pThis->m_bRecordExp = TRUE;
	}
	else if (11 == iMsg && pThis)
	{
       pThis->m_bDownloadOk = TRUE;
	}
}

void CALLBACK RecordDecodedDataCallback(VIDEO_INT64 i64PlayHandle, const char* pDataArray, int iDataLen, int iWidth, int iHeight, 
								  int iFrameType, int iTimeStamp, void* pUserData)
{
	
}

void CALLBACK RecordStreamCallBack(VIDEO_INT64 i64PlayHandle, int iStreamDataType, const char* pDataArray, int iDataLen, void* pUserData)
{
	
}

BOOL HikPlatformRecord::Start(const NET_RECPLAY_PARAM_S *pParam)
{
	m_hWnd   = pParam->hWnd;
	m_stBeginTime = pParam->stHikMode.stBegin;
	m_stEndTime   = pParam->stHikMode.stEnd;
	m_strDeviceIp = pParam->stHikMode.ps8DeviceIp;
	m_u16DevicePort = pParam->stHikMode.u16DevicePort;
	m_u16ChnNo      = pParam->stHikMode.u16ChnNo;

    if (NULL == pParam->stHikMode.ps8PlayUrl) 
	{
		string strIp = m_pConn->GetIp();
		int    iPort = m_pConn->GetPort();
		char * strAppKey = m_pConn->GetAppKey();
		char * strAppSec = m_pConn->GetAppSec();
		
		S8 s8Buf[512] = {0};
		sprintf(s8Buf, "http://%s:%d/artemis/api/video/v1/cameras/playbackURLs", strIp.c_str(), iPort);
		
		S8 s8BeginTime[128] = {0};
		S8 s8EndTime[128]   = {0};
		sprintf(s8BeginTime, "%04d-%02d-%02dT%02d:%02d:%02d.000+08:00", m_stBeginTime.u32Year, m_stBeginTime.u32Month, m_stBeginTime.u32Day,
			m_stBeginTime.u32Hour, m_stBeginTime.u32Minute, m_stBeginTime.u32Second);
		sprintf(s8EndTime, "%04d-%02d-%02dT%02d:%02d:%02d.000+08:00", m_stEndTime.u32Year, m_stEndTime.u32Month, m_stEndTime.u32Day,
			m_stEndTime.u32Hour, m_stEndTime.u32Minute, m_stEndTime.u32Second);
		
		std::string strUUid = "";
		
		do 
		{	
			char ipPort[512] = {0};
            string name = "";
            map<string,string>::iterator pos;

            if (m_u16DevicePort == 0)
            {
                name = m_strDeviceIp;
				AutoLock lock(m_pConn->m_mtxDevIdx);
				pos = m_pConn->m_mapNameToDevCode.find(name);
				if (pos == m_pConn->m_mapNameToDevCode.end())
				{
					return FALSE;
				}
            } 
            else
            {
			    sprintf(ipPort, "%s:%d", m_strDeviceIp.c_str(), m_u16DevicePort);
				AutoLock lock(m_pConn->m_mtxDevIdx);
				pos = m_pConn->m_mapIpPortToDevCode.find(ipPort);
				if (pos == m_pConn->m_mapIpPortToDevCode.end())
				{
					return FALSE;
				}
            }

			string strDevCode = pos->second;
			
			char ipDevChn[512] = {0};
			sprintf(ipDevChn, "%s-%d", strDevCode.c_str(), (m_u16ChnNo+1));

			string strCamCode;
			{
				AutoLock lock(m_pConn->m_mtxCamIdx);
				map<string, string>::iterator iter = m_pConn->m_mapIpChnToCameraIndex.find(ipDevChn);
				if (iter == m_pConn->m_mapIpChnToCameraIndex.end())
				{
					return FALSE;
				}
				
				strCamCode = iter->second;
			}
			
			SHL_Json::Value jsonReq;
			jsonReq["cameraIndexCode"] = strCamCode;
			jsonReq["beginTime"]       = s8BeginTime;
			jsonReq["endTime"]         = s8EndTime;
			jsonReq["recordLocation"]  = "1";
			SHL_Json::FastWriter req;
			string strReq = req.write(jsonReq);
			
			char *strResult = HttpPost(s8Buf, strReq.c_str(), strAppKey, strAppSec, 5);
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
			
			SHL_Json::Value dataNode = rootNode["data"];
			SHL_Json::Value listNode = dataNode["list"];
			
			strUUid = dataNode["uuid"].asString();
			m_strUrl = dataNode["url"].asString();
			
		}while(0);
	}
	else
	{
		m_strUrl = pParam->stHikMode.ps8PlayUrl;
	}

	CreateJoinableThread(RecordPlayFun, this, &m_trdRecPlay);

	return TRUE;
}

VOID HikPlatformRecord::Stop()
{
	m_bRecordExp = TRUE;
    m_bExit      = TRUE;
	ThreadJoin(m_trdRecPlay);
}

VOID *HikPlatformRecord::RecordPlayFun(VOID *pArg)
{
	HikPlatformRecord *pRecPlay = (HikPlatformRecord *)pArg;
	pRecPlay->RecordPlayLoop();
	
	return 0;
}

VOID HikPlatformRecord::RecordPlayLoop()
{
    do 
    {
		m_bRecordExp = FALSE;

		BOOL streamOpened = OpenStream();
		if (streamOpened)
		{
            while(!m_bRecordExp 
			   && !m_bDownloadOk)
			{
				Msleep(100);
				continue;
			}

			break;
		}
		else
		{
			Msleep(100);
		}
    } while (!m_bExit);

	if (-1 != m_recPlayerId)
	{
		Video_StopPlayback(m_recPlayerId);
		m_recPlayerId = -1;
	}
}

BOOL HikPlatformRecord::OpenStream()
{
	if (-1 != m_recPlayerId)
	{
        Video_StopPlayback(m_recPlayerId);
		m_recPlayerId = -1;
	}

	struct tm tmStart;
	memset(&tmStart,0,sizeof(tmStart));
	struct tm tmEnd;
	memset(&tmEnd,0,sizeof(tmEnd));

	tmStart.tm_year = m_stBeginTime.u32Year - 1900;
	tmStart.tm_mon  = m_stBeginTime.u32Month - 1;
	tmStart.tm_mday = m_stBeginTime.u32Day;
	tmStart.tm_hour = m_stBeginTime.u32Hour;
	tmStart.tm_min  = m_stBeginTime.u32Minute;
	tmStart.tm_sec  = m_stBeginTime.u32Second;
	time_t llStartTime = mktime(&tmStart);
	m_tmBeginSec = llStartTime;

	tmEnd.tm_year = m_stEndTime.u32Year - 1900;
	tmEnd.tm_mon  = m_stEndTime.u32Month - 1;
	tmEnd.tm_mday = m_stEndTime.u32Day;
	tmEnd.tm_hour = m_stEndTime.u32Hour;
	tmEnd.tm_min  = m_stEndTime.u32Minute;
	tmEnd.tm_sec  = m_stEndTime.u32Second;
	time_t llEndTime = mktime(&tmEnd);
	m_tmEndSec = llEndTime;

	VIDEO_PLAY_REQ recReq = {0};
	recReq.fnMsg = RecordMsgCallBack;

    m_recPlayerId = Video_StartPlayback(m_strUrl.c_str(), m_hWnd, llStartTime, llEndTime, &recReq);
	if (m_recPlayerId < 0)
	{
		return FALSE;
	}

	return TRUE;
}

BOOL HikPlatformRecord::PlayCtrl(U32 u32Code, VOID *pInVal, U32 u32InSize, VOID *pOutVal, U32 *u32OutSize)
{
    if (-1 == m_recPlayerId)
	{
		return FALSE;
	}

	switch (u32Code)
	{
	case NET_PLAYSTART:
		return TRUE;
	case NET_PLAYSTOP:

        break;
	case NET_PLAYPAUSE:
        Video_PlayCtrl(m_recPlayerId, 0, 0);
		break;
	case NET_PLAYRESTART:
		{
			Video_PlayCtrl(m_recPlayerId, 1, 0);
			
			int speed = 1;
			if (m_iPlaySpeed >= 0)
			{
				speed = pow(2, m_iPlaySpeed);
			}
			else
			{
				speed = -pow(2, abs(m_iPlaySpeed));
			}
			Video_PlayCtrl(m_recPlayerId, 3, speed);
		}
		break;
	case NET_PLAYFAST:
		{
			m_iPlaySpeed++;
			
			if (m_iPlaySpeed > 4)
				m_iPlaySpeed = 4;
			
			int speed = 1;
			if (m_iPlaySpeed >= 0)
			{
				speed = pow(2, m_iPlaySpeed);
			}
			else
			{
				speed = -pow(2, abs(m_iPlaySpeed));
			}
			Video_PlayCtrl(m_recPlayerId, 3, speed);
		}

		break;
	case NET_PLAYSLOW:
		{
			m_iPlaySpeed--;
			
			if (m_iPlaySpeed < -4)
				m_iPlaySpeed = -4;
			
			int speed = 1;
			if (m_iPlaySpeed >= 0)
			{
				speed = pow(2, m_iPlaySpeed);
			}
			else
			{
				speed = -pow(2, abs(m_iPlaySpeed));
			}
			Video_PlayCtrl(m_recPlayerId, 3, speed);
		}

		break;
	case NET_PLAYNORMAL:
		Video_PlayCtrl(m_recPlayerId, 1, 0);
		break;
	case NET_PLAYSTARTAUDIO:
        OpenSound();
		break;
	case NET_PLAYSTOPAUDIO:
        CloseSound();
		break;
	case NET_PLAYSETPOS:
		{
			U32 u32Pos = *(U32 *)pInVal;		
			DWORD dwFileSec =  m_tmEndSec - m_tmBeginSec;
			DWORD dwOffset = (u32Pos * dwFileSec) / 100;
			time_t tmReposSec = m_tmBeginSec + dwOffset;
			if (-1 != m_recPlayerId)
			{
				Video_PlayCtrl(m_recPlayerId, 2, tmReposSec);
			}
		}
		break;
	case NET_PLAYSETTIME:

		break;
	case NET_PLAYSTEP:
		break;
	default:
		return FALSE;
	}

    return TRUE;
}

BOOL HikPlatformRecord::SetRecPlaybackStatusCb(pfnRecordPlaybackStatusCallback fnRecPbStatusCb, VOID *pUser)
{
	m_pfnRecordPbStatusCb = fnRecPbStatusCb;
	m_pUser = pUser;

	return TRUE;
}

DWORD HikPlatformRecord::GetPlayedTime()
{
	if (-1 == m_recPlayerId)
	{
		return 0;
	}

	DWORD dwPlayTime = Video_GetCurrentPlayTime(m_recPlayerId);

	if (dwPlayTime > m_tmBeginSec)
	{
		return (dwPlayTime-m_tmBeginSec);
	}

	return 0;
}

BOOL HikPlatformRecord::OpenSound()
{
	if (-1 == m_recPlayerId)
	{
		return FALSE;
	}

	Video_SetVolume(m_recPlayerId, 90);

	Video_SoundCtrl(m_recPlayerId, 0);

	return TRUE;
}

BOOL HikPlatformRecord::CloseSound()
{
	Video_SoundCtrl(m_recPlayerId, 1);

	return TRUE;
}