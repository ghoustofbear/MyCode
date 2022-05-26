#include "hikplatform_preview.h"
#include "hikplatform_conn.h"
#include "commonc1.h"
#include "json/json.h"

HikPlatformPreview::HikPlatformPreview(HikPlatformConn * pConn)
    : BasePView()
{
	m_pConn = pConn;
	m_bExit = FALSE;
	m_PlayerId = -1;
	m_bPreviewEx = false;
	m_tmPrevTime = time(NULL);
	m_hDecCallback = NULL;
	m_hUser        = NULL;
	m_tmVideoTime  = time(NULL);
    //m_PViewClient = NULL;
    m_bSaveRealData = VIDEO_ERR_FAIL;
                      
}

HikPlatformPreview::~HikPlatformPreview()
{
}

void CALLBACK PreviewMsgCallBack(VIDEO_INT64 i64PlayHandle, int iMsg, void* pUserData)
{
	HikPlatformPreview * pThis = (HikPlatformPreview *)pUserData;
	
	//12:È¡Á÷Òì³£
	if (12 == iMsg && pThis)
	{
		pThis->m_bPreviewEx = true;
	}
}

void CALLBACK PreviewDecodedDataCallback(VIDEO_INT64 i64PlayHandle, const char* pDataArray, int iDataLen, int iWidth, int iHeight, 
								  int iFrameType, int iTimeStamp, void* pUserData)
{
    HikPlatformPreview * pThis = (HikPlatformPreview *)pUserData;
	if (pThis && pThis->m_hDecCallback)
	{
		DEC_FRAME_INFO param = {0};
		param.ps8Data    = (S8*)pDataArray;
		param.u32Len     = iDataLen;
		param.s32Height  = iHeight;
		param.s32Width   = iWidth;
		param.u8Type     = UT_YV12;	
		
		((pfnDecDataCallback)(pThis->m_hDecCallback))(&param, pThis->m_hUser);
	}
}

void CALLBACK PreviewStreamCallBack(VIDEO_INT64 i64PlayHandle, int iStreamDataType, const char* pDataArray, int iDataLen, void* pUserData)
{
    HikPlatformPreview * pThis = (HikPlatformPreview *)pUserData;
	if (pThis)
	{
		pThis->m_tmVideoTime = time(NULL);
	}
}

VOID *HikPlatformPreview::PreviewFun(VOID *pArg)
{
	HikPlatformPreview* pPView = (HikPlatformPreview *)pArg;
	pPView->PreviewLoop();
	
	return 0;
}

VOID HikPlatformPreview::PreviewLoop()
{
	do 
	{
		m_bPreviewEx = false;

#if 0
		BOOL bStatus = QueryStatus();
		if (!bStatus)
		{
			Msleep(100);
			continue;
		}
#endif
				
		BOOL streamOpened = OpenStream();
		if (streamOpened)
		{
			RecvStream();
		}
		else
		{
			Msleep(100);
		}	
	}while(!m_bExit);
}

VOID HikPlatformPreview::RecvStream()
{
	VIDEO_PLAY_REQ reqPlay = {0};
	reqPlay.pUserData = this;
	reqPlay.fnMsg     = PreviewMsgCallBack;
	reqPlay.fnStream  = PreviewStreamCallBack;
    
	if (-1 != m_PlayerId)
	{
		Video_StopPreview(m_PlayerId);
		m_PlayerId = -1;
	}

	//m_PlayerId = Video_StartPreview(m_strUrl.c_str(), m_PView.hWnd, &reqPlay); 
	m_PlayerId = Video_StartAsynPreview(m_strUrl.c_str(), m_PView.hWnd, &reqPlay); 
    
	if (m_PlayerId < 0)
	{
		int iErr = Video_GetLastError();
		return;
	}

	do 
	{
		time_t tmNow = time(NULL);
		if (tmNow - m_tmVideoTime >= 10)
		{
			m_tmVideoTime = tmNow;
			break;
		}

		if (m_bPreviewEx)
		{
			break;
		}
		else
		{
			Msleep(100);
			continue;
		}
	} while (!m_bExit);

	if (-1 != m_PlayerId)
	{
		Video_StopPreview(m_PlayerId);
		m_PlayerId = -1;
	}
}

BOOL HikPlatformPreview::QueryStatus()
{
	std::string ip     = m_pConn->GetIp();
	U16 u16Port        = m_pConn->GetPort();
	char * appKey = m_pConn->GetAppKey();
	char * appSec = m_pConn->GetAppSec();
	
	S8 s8Buf[512] = {0};
	sprintf(s8Buf, "http://%s:%d/artemis/api/resource/v1/cameras/indexCode", ip.c_str(), u16Port);
	
    char ipPort[512] = {0};
	sprintf(ipPort, "%s:%d", m_PView.ps8DeviceIp, m_PView.u16DevicePort);

	string strDevCode;
	{
		AutoLock lock(m_pConn->m_mtxDevIdx);
		map<string,string>::iterator pos = m_pConn->m_mapIpPortToDevCode.find(ipPort);
		if (pos == m_pConn->m_mapIpPortToDevCode.end())
		{
			return FALSE;
		}
		
		strDevCode = pos->second;
	}
	
	char ipDevChn[512] = {0};
	sprintf(ipDevChn, "%s-%d", strDevCode.c_str(), (m_PView.u32ChnNO+1));

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
	SHL_Json::FastWriter req;
	string strReq = req.write(jsonReq);
	
	char *strResult = HttpPost(s8Buf, strReq.c_str(), appKey, appSec, 5);
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
	m_strUrl = dataNode["url"].asString();
	string status = dataNode["status"].asString();
	if ("1" != status)
	{
		return FALSE;
	}
	
	return TRUE;
}

BOOL HikPlatformPreview::OpenStream()
{
	std::string ip     = m_pConn->GetIp();
	U16 u16Port        = m_pConn->GetPort();
	char * appKey = m_pConn->GetAppKey();
	char * appSec = m_pConn->GetAppSec();

	S8 s8Buf[512] = {0};
	sprintf(s8Buf, "http://%s:%d/artemis/api/video/v1/cameras/previewURLs", ip.c_str(), u16Port);

    //
    char ipPort[512] = {0};
    std::string devIp = m_PView.ps8DeviceIp;
    std::string ip_front3 = devIp.substr(0,3);
    std::string name = "";
    if(ip_front3 == "0.0")
    {
        name = m_PView.ps8FrontAddrCode;
    }
    else
    {
	    sprintf(ipPort, "%s:%d", m_PView.ps8DeviceIp, m_PView.u16DevicePort);
    }


	string strDevCode;
	{
		AutoLock lock(m_pConn->m_mtxDevIdx);
        if(ip_front3 == "0.0")
        {
		    map<string,string>::iterator pos = m_pConn->m_mapNameToDevCode.find(name);
		    if (pos == m_pConn->m_mapNameToDevCode.end())
		    {
			    return FALSE;
		    }
		    
		    strDevCode = pos->second;            
        }
        else
        {        
		    map<string,string>::iterator pos = m_pConn->m_mapIpPortToDevCode.find(ipPort);
		    if (pos == m_pConn->m_mapIpPortToDevCode.end())
		    {
			    return FALSE;
		    }
		    
		    strDevCode = pos->second;
        }
	}
	
	char ipDevChn[512] = {0};
	sprintf(ipDevChn, "%s-%d", strDevCode.c_str(), (m_PView.u32ChnNO+1));

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
	jsonReq["streamType"]      = m_PView.u8Type;
	SHL_Json::FastWriter req;
	string strReq = req.write(jsonReq);

	char *strResult = HttpPost(s8Buf, strReq.c_str(), appKey, appSec, 5);
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
	m_strUrl = dataNode["url"].asString();

	return TRUE;
}

PreviewClient * HikPlatformPreview::Start(const NET_PVIEW_PARAM_S *pParam)
{
	ASSERT(pParam);
	AutoLock lock(m_mtxPView);

	strcpy(m_s8DeviceName, pParam->ps8FrontAddrCode);
	strcpy(m_s8DeviceIp, pParam->ps8DeviceIp);

	m_PViewClient.stParam       = *pParam;
	m_PViewClient.stParam.ps8DeviceIp = m_s8DeviceIp;
	m_PViewClient.stParam.ps8FrontAddrCode = m_s8DeviceName;

	m_PViewClient.pPView        = this;
	m_PViewClient.bNeedKeyFrame = TRUE;

	m_PView = *pParam;	
	m_PView.ps8FrontAddrCode = m_s8DeviceName;
	m_PView.ps8DeviceIp  = m_s8DeviceIp;

	CreateJoinableThread(PreviewFun, this, &m_trdPView);

	return &m_PViewClient;
}

BOOL HikPlatformPreview::Stop(PreviewClient* pPViewClient)
{
	if (m_bExit)
	{
		return TRUE;
	}

	AutoLock lock(m_mtxPView);
	if (-1 != m_PlayerId)
	{
		Video_StopPreview(m_PlayerId);
		m_PlayerId = -1;
	}

	m_bPreviewEx  = TRUE;
	m_bExit       = TRUE;
	ThreadJoin(m_trdPView);

	return TRUE;
}

BOOL HikPlatformPreview::OpenSound()
{
	if (-1 == m_PlayerId)
	{
		return FALSE;
	}

	int ret = Video_SoundCtrl(m_PlayerId, 0);
	int err = Video_GetLastError();
	return TRUE;
}

BOOL HikPlatformPreview::CloseSound()
{
	Video_SoundCtrl(m_PlayerId, 1);

	return TRUE;
}

BOOL HikPlatformPreview::CapturePicture(S8 *ps8PicFileName, U32 u32CaptueMode)
{
	if (-1 == m_PlayerId)
	{
		return FALSE;
	}

	int iRet = Video_PlaySnap(m_PlayerId, ps8PicFileName);
	return (VIDEO_ERR_SUCCESS == iRet) ? TRUE : FALSE;
}

VOID HikPlatformPreview::SetDecDataCallBack(HANDLE hDecCb, HANDLE hUser)
{
   m_hDecCallback = hDecCb;
   m_hUser        = hUser;
}

BOOL HikPlatformPreview::SaveRealData(S8 *ps8FileName)
{
    if (VIDEO_ERR_FAIL != m_bSaveRealData)
    {
        return FALSE; 
    }

	ASSERT(ps8FileName);
    if(m_PlayerId > 0)
    {
//        ps8FileName = "E:\\test.mp4";
        m_bSaveRealData = Video_StartLocalRecord(m_PlayerId, ps8FileName, 1024*1024*1024, 0);  
        if (VIDEO_ERR_FAIL == m_bSaveRealData)
        {
            int err = Video_GetLastError();
            return FALSE; 
        }
        else
        {
            return TRUE;           
        }
    }

    return FALSE;
}

BOOL HikPlatformPreview::StopSaveRealData()
{
    if (VIDEO_ERR_FAIL == m_bSaveRealData)
    {
        return FALSE; 
    }

    int ret = Video_StopLocalRecord(m_PlayerId);

    if (VIDEO_ERR_FAIL == ret)
    {
        return FALSE; 
    }
    else
    {
        return TRUE;
        m_bSaveRealData = VIDEO_ERR_FAIL;
    }

    return FALSE;    
}