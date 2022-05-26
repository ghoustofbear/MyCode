#include "hikplatform_download_bytime.h"
#include "hikplatform_conn.h"
#include "commonc1.h"
#include "json/json.h"

HikPlatformDownloadByTime::HikPlatformDownloadByTime(HikPlatformConn * pConn)
				:HikPlatformDownload(pConn),BaseDownloadByTime()
{
    m_downType     = DOWNLOAD_BYTIME;
}

HikPlatformDownloadByTime::~HikPlatformDownloadByTime()
{

}

BOOL HikPlatformDownloadByTime::Start(const NET_RECDOWNLOAD_BYTIME_PARAM_S *pParam)
{
	string strIp = m_pConn->GetIp();
	int    iPort = m_pConn->GetPort();
	char * strAppKey = m_pConn->GetAppKey();
	char * strAppSec = m_pConn->GetAppSec();
	
	S8 s8Buf[512] = {0};
	sprintf(s8Buf, "http://%s:%d/artemis/api/video/v1/cameras/playbackURLs", strIp.c_str(), iPort);
	   
    S8 s8BeginTime[128] = {0};
	S8 s8EndTime[128]   = {0};
	sprintf(s8BeginTime, "%04d-%02d-%02dT%02d:%02d:%02d.000+08:00", pParam->stBegin.u32Year, pParam->stBegin.u32Month, pParam->stBegin.u32Day,
	   pParam->stBegin.u32Hour, pParam->stBegin.u32Minute, pParam->stBegin.u32Second);
	sprintf(s8EndTime, "%04d-%02d-%02dT%02d:%02d:%02d.000+08:00", pParam->stEnd.u32Year, pParam->stEnd.u32Month, pParam->stEnd.u32Day,
	   pParam->stEnd.u32Hour, pParam->stEnd.u32Minute, pParam->stEnd.u32Second);

	std::string strUUid = "";

	do 
	{	
		char ipPort[512] = {0};
        string name = "";
        map<string,string>::iterator pos;
        if (pParam->u16DevicePort == 0)
        {
            string name = pParam->ps8DeviceIp;
			AutoLock lock(m_pConn->m_mtxDevIdx);
			pos = m_pConn->m_mapNameToDevCode.find(name);
			if (pos == m_pConn->m_mapNameToDevCode.end())
			{
				return FALSE;
			}
        } 
        else
        {
		    sprintf(ipPort, "%s:%d", pParam->ps8DeviceIp, pParam->u16DevicePort);
			AutoLock lock(m_pConn->m_mtxDevIdx);
			pos = m_pConn->m_mapIpPortToDevCode.find(ipPort);
			if (pos == m_pConn->m_mapIpPortToDevCode.end())
			{
				return FALSE;
			}
        }

		string strDevCode = pos->second;
		
		char ipDevChn[512] = {0};
		sprintf(ipDevChn, "%s-%d", strDevCode.c_str(), (pParam->u32ChnNO+1));

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
	   
    m_stBeginTime     = pParam->stBegin;     //开始时间
	m_stEndTime       = pParam->stEnd;       //结束时间
    m_fStatusCallback = pParam->fStatusCallback;
    m_pUser           = pParam->pUser;
 
    if (pParam->ps8FileName)
	{
	   m_pSaveFile = fopen(pParam->ps8FileName, "wb");
	   if (!m_pSaveFile)
	   {
		   HIKPLAT_DOWNLOAD_STATUS(MSG_DOWNLOAD_STOREFAIL);
		   return  FALSE;
	   }
	}
 
    CreateJoinableThread(RecDownloadLoop, this, &m_trdDownload);
 
    return TRUE;
}

 VOID HikPlatformDownloadByTime::Stop()
 {
     HikPlatformDownload::Stop();
 }
 U32  HikPlatformDownloadByTime::GetDownloadPos()
 {
     return HikPlatformDownload::GetDownloadPos();
 }