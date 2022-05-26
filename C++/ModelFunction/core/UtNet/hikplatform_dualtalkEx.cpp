#include "hikplatform_dualtalkEx.h"
#include "hikplatform_conn.h"
#include "commonc1.h"
#include "json/json.h"

void CALLBACK TalkExMsgCallBack(VIDEO_INT64 i64PlayHandle, int iMsg, void* pUserData)
{
    HikPlatformDualtalkEx *ptalkEx = (HikPlatformDualtalkEx*)pUserData;
    switch(iMsg)
    {
    case 12://麦克风拔掉后
        ptalkEx->setBTalkEx(FALSE);
    	break;
    case 10://刚开启对讲，可能去留失败
    	break;
    default:
        int a =0;
        break;
    }
}

HikPlatformDualtalkEx::HikPlatformDualtalkEx(HikPlatformConn *pConn,  int audioType)
    :BaseDualTalkEx()
{
	ASSERT(pConn);
	m_pConn         = pConn;
	m_trdTalkEx     = 0;
	m_iAudioType    = audioType;
	m_bTalkEx       = FALSE;
	m_bExit         = TRUE;
    m_retTalk       = VIDEO_ERR_FAIL;
	
}

HikPlatformDualtalkEx::~HikPlatformDualtalkEx()
{
	Stop();
}

S32 HikPlatformDualtalkEx::Start()
{
	return 0;
}

VOID HikPlatformDualtalkEx::Stop()
{	
	if (m_bExit)
	{
		return;
	}

	AutoLock lock(m_mtxTalkEx);
	Video_StopTalk();

	m_bTalkEx  	  = FALSE;
	m_bExit       = TRUE;
	ThreadJoin(m_trdTalkEx);

	return;
}

BOOL HikPlatformDualtalkEx::SendDTalkReq(const NET_DTALK_DEV_LIST * pParam, U32 u32Type)
{
    if(0 == u32Type)
    {    
        if (m_bExit)
        {
	        ASSERT(pParam);
	        AutoLock lock(m_mtxTalkEx);
	        
	        strcpy(m_s8DeviceIp, pParam->pDevList->ps8Ip);
	        
	        m_talkEx.ps8Ip  = m_s8DeviceIp;
            m_talkEx.u16Port = pParam->pDevList->u16Port;
	        m_talkEx.u8ChnNo = pParam->pDevList->u8ChnNo;
	        CreateJoinableThread(TalkExFun, this, &m_trdTalkEx);
        }
    }
    else
    {
        Stop();
    }
	
	return TRUE;
}

VOID *HikPlatformDualtalkEx::TalkExFun(VOID *pArg)
{
	HikPlatformDualtalkEx* pPView = (HikPlatformDualtalkEx *)pArg;
	pPView->DualtalkLoop();
	
	return 0;
}

VOID HikPlatformDualtalkEx::DualtalkLoop()
{    
    m_bExit = FALSE;
	do 
	{
		//m_bTalkEx = false;
		
        if(m_bTalkEx)
        {
            Msleep(100);
        }
        else
        {
		    BOOL streamOpened = OpenStream();
		    if (streamOpened)
		    {
			    RecvStream();
		    }
		    else
		    {
			    Msleep(100);
		    }            
        }

	}while(!m_bExit);
}

BOOL HikPlatformDualtalkEx::OpenStream()
{
	std::string ip     = m_pConn->GetIp();
	U16 u16Port        = m_pConn->GetPort();
	char * appKey = m_pConn->GetAppKey();
	char * appSec = m_pConn->GetAppSec();
	
	
	S8 s8Buf[512] = {0};
	sprintf(s8Buf, "http://%s:%d/artemis/api/video/v1/cameras/talkURLs", ip.c_str(), u16Port);

    //
    char ipPort[512] = {0};
    std::string name = "";
    if(m_talkEx.u16Port == 0)
    {
        name = m_talkEx.ps8Ip;
    }
    else
    {
	    sprintf(ipPort, "%s:%d", m_talkEx.ps8Ip, m_talkEx.u16Port);
    }


	string strDevCode;
	{
		AutoLock lock(m_pConn->m_mtxDevIdx);
        if(m_talkEx.u16Port == 0)
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
	sprintf(ipDevChn, "%s-%d", strDevCode.c_str(), (m_talkEx.u8ChnNo+1));

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
	jsonReq["transmode"]      = m_iAudioType;
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


VOID HikPlatformDualtalkEx::RecvStream()
{
    int ret = Video_StopTalk();

    m_retTalk = Video_StartTalk(m_strUrl.c_str(), NULL, TalkExMsgCallBack, this); 
    
	if (m_retTalk == VIDEO_ERR_FAIL)
	{
		int iErr = Video_GetLastError();
	}
    else
	{
		//Video_StopTalk();
        m_bTalkEx = TRUE;
        m_bExit = FALSE;
	}
}






















