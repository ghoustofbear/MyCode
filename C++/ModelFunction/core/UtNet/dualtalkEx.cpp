#include "dualtalkEx.h"
#include "connection.h"

#ifdef _WIN32
#include "RSPlay.h"
#include "audio_record.h"
#include "audio_helmet_gx.h"
//#pragma comment(lib, "../../lib/RSPlay.lib")
#endif

#define ENABLE_DUALTALKEX_DEBUG
#ifdef  ENABLE_DUALTALKEX_DEBUG 
#ifdef _WIN32
#define DUALTALK_EX_DEBUG             printf
#else
#define DUALTALK_EX_DEBUG(MSG...)     DEBUG("DUALTALKEX", MSG)
#endif /* end _WIN32 */
#else
#define DUALTALK_EX_DEBUG(MSG...)
#endif /* end ENABLE_DUALTALKEX_DEBUG */

DualtalkEx::DualtalkEx(Connection *pConn,  int audioType)
    :BaseDualTalkEx()
{
	ASSERT(pConn);
	m_pConn         = pConn;
	m_bExit         = FALSE;
	m_trdTalk       = 0;
	m_bOpened       = FALSE;
	m_iAudioType    = audioType;
	
	m_iPlayerAudio  = -1;
	m_audioRecord   = NULL;
	m_audioHelmetGx = NULL;
    m_hMuxAVF       = NULL;
    m_bTalked       = false;
    m_curChannel    = -1;

    m_muxInitParam.nSubpackageTime=60;
    m_muxInitParam.pUser=NULL;
    m_muxInitParam.callback =NULL;
    m_muxInitParam.strFilePath = new S8[1024];
    memset(m_muxInitParam.strFilePath,0,1024);
    m_bMuxInited = false;
}

DualtalkEx::~DualtalkEx()
{
	Stop();
    delete [] m_muxInitParam.strFilePath;
}

S32 DualtalkEx::Start()
{
	return StartDualtalkLoop();
}

VOID DualtalkEx::Stop()
{
	m_bExit    = TRUE;
	m_bOpened  = FALSE;
	
	m_DTalkSock.KillSocket();
	ThreadJoin(m_trdTalk);

	StopSound();
    CloseMux();
}

VOID DualtalkEx::StopSound()
{
#ifdef _WIN32
	if (3 == m_iAudioType)
	{
		if (m_audioRecord)
		{
			m_audioRecord->stop();

			delete m_audioRecord;
			m_audioRecord = NULL;
		}
	}
	else if(2 == m_iAudioType ||
		1 == m_iAudioType)
	{
		if (-1 != m_iPlayerAudio)
		{
			RSPlayRegisterSetDualCallback(m_iPlayerAudio, NULL, (DWORD)this);
			RSPlayStopDualTalk(m_iPlayerAudio);
			RSPlayDestroyPlayInstance(m_iPlayerAudio);
			m_bOpened      = FALSE;
			m_iPlayerAudio = -1;
		}	
	}
	else if(4 == m_iAudioType)
	{
       if (m_audioHelmetGx)
	   {
		   m_audioHelmetGx->Stop();

		   delete m_audioHelmetGx;
		   m_audioHelmetGx = NULL;
	   }
	}


#endif
}

BOOL DualtalkEx::OpenSound()
{
#ifdef _WIN32
	if (m_bOpened)
	{
		return TRUE;
	}

	HRESULT hRet = S_OK;

	if (1 == m_iAudioType)
	{
		if (-1 == m_iPlayerAudio)
		{
			m_iPlayerAudio = RSPlayCreatePlayInstance(PLAY_DUALTALK_TYPE);
			AudioTalkConfig_t talk;
			talk.iAmr_format = 0;
			talk.iAmr_mode   = 0;
			talk.iFormat     = 1;     /* 1:g711_a; 2:g711_u*/
			talk.iRate       = 8000;
			talk.iWidth      = 16;
			hRet = RSPlayStartDualTalk(m_iPlayerAudio, &talk);
			if (S_OK == hRet)
			{
				m_bOpened = TRUE;
			}
			
			RSPlayRegisterSetDualCallback(m_iPlayerAudio, AudioDataCallback, (DWORD)this);
		}
	}
	else if (2 == m_iAudioType)
	{
		if (-1 == m_iPlayerAudio)
		{
			m_iPlayerAudio = RSPlayCreatePlayInstance(PLAY_DUALTALK_TYPE);
			AudioTalkConfig_t talk;
			talk.iAmr_format = 0;
			talk.iAmr_mode   = 0;
			talk.iFormat     = 2;     /* 1:g711_a; 2:g711_u*/
			talk.iRate       = 8000;
			talk.iWidth      = 16;
			hRet = RSPlayStartDualTalk(m_iPlayerAudio, &talk);
			if (S_OK == hRet)
			{
				m_bOpened = TRUE;
			}
			
			RSPlayRegisterSetDualCallback(m_iPlayerAudio, AudioDataCallback, (DWORD)this);
		}
	}
	else if (3 == m_iAudioType)
	{
		if (!m_audioRecord)
		{
			m_audioRecord = new AudioRecord(3, 1, 16000, 16, 1280);
			m_audioRecord->setAudioDataCallback(AudioDataCallbackEx, this);

			m_audioRecord->start();
		}
	}
	else if (4 == m_iAudioType)
	{
        if (NULL == m_audioHelmetGx)
		{
            m_audioHelmetGx = new AudioHelmetGx();

			m_audioHelmetGx->SetAudioDataCallback(AudioDataCallbackExEx, this);

			m_audioHelmetGx->Start();
		}
	}

	return (S_OK == hRet) ? TRUE : FALSE;
#else
   return TRUE;
#endif
}

BOOL DualtalkEx::SendDTalkData(U8 *pu8Data, U32 u32DataSize)
{
	if (!m_pConn->IsLogined())
	{
		return FALSE;
	}
	
	if (0 == m_mapDTalkDev.size())
	{
		return FALSE;
	}
	
	MsgHeadWrap msgHead(MSG_DEVTALK, MSG_DTALK_EX_DATA, u32DataSize);
	return SendMsg(m_DTalkSock, msgHead, pu8Data);
}

BOOL DualtalkEx::SendDTalkReq(const NET_DTALK_DEV_LIST * pDevList, U32 u32Type)
{
	if (!m_pConn->IsLogined() ||
		0 == pDevList->u32DevListNum)
	{
		return FALSE;
	}
	
	U32  u32MsgSize = pDevList->u32DevListNum * sizeof(DTALK_DEV_S);
    U8 * pu8Msg = new U8[u32MsgSize];
	
	do 
	{				
		if (NULL == pu8Msg)
		{
			break;
		}
		
        DTALK_DEV_S * pTalkDevList        = (DTALK_DEV_S *)pu8Msg;
		NET_DTALK_DEV_INFO * pTalkDevInfo = (NET_DTALK_DEV_INFO *)pDevList->pDevList;
		
		S8 s8DevId[128] = {0};
		map<string, DTALK_DEV_S>::iterator pos;
		
		for (S32 s32I = 0; s32I < pDevList->u32DevListNum; ++s32I)
		{
            INET_PTON(pTalkDevInfo[s32I].ps8Ip, pTalkDevList->u8DevIp);
			pTalkDevList->u16DevPort = pTalkDevInfo[s32I].u16Port;
			pTalkDevList->u8ChnNo    = pTalkDevInfo[s32I].u8ChnNo;
			
			sprintf(s8DevId, "%s:%d:%d", pTalkDevInfo[s32I].ps8Ip, 
				pTalkDevInfo[s32I].u16Port, pTalkDevInfo[s32I].u8ChnNo);
            pos = m_mapDTalkDev.find(s8DevId);
			if (0 == u32Type)
			{
				if (pos == m_mapDTalkDev.end())
				{
					//m_mapDTalkDev.insert(make_pair<string ,DTALK_DEV_S>(s8DevId, *pTalkDevList));

					m_mapDTalkDev[s8DevId] = *pTalkDevList;
                    if (m_bMuxInited)
                    {
                        CreateMux();
                    }
                    m_curChannel = pTalkDevList->u8ChnNo;
                    m_bTalked= true;
				}
			}
			else if (1 == u32Type)
			{
				if (pos != m_mapDTalkDev.end())
				{
                    m_bTalked= false;
                    m_curChannel = -1;
					m_mapDTalkDev.erase(pos);
                    if (m_bMuxInited)
                    {
                        CloseMux();
                    }
				}
			}
			
            pTalkDevList++;
		}
		
		MsgHeadWrap msgHead;		
		if (0 == u32Type)
		{
			msgHead.SetMsgHead(MSG_DEVTALK, MSG_DEVTALK_START, u32MsgSize);
		}
		else if (1 == u32Type)
		{
            msgHead.SetMsgHead(MSG_DEVTALK, MSG_DEVTALK_STOP, u32MsgSize);
		}
		
		msgHead.u2Version = m_pConn->m_u32MsgVersion;
		
        BOOL bRet = SendReq(msgHead, pu8Msg);
		if (!bRet)
		{
			break;
		}
		
		delete [] pu8Msg;
		
		return TRUE;		
	} while(0);
	
	delete [] pu8Msg;
	
	return FALSE;
}


BOOL DualtalkEx::SendReq(const MsgHeadWrap &msgHead, VOID *reqData)
{
	return SendMsg(m_DTalkSock, msgHead, reqData);
}

#ifdef _WIN32

int CALLBACK AudioDataCallback(int size, const char* data, DWORD dwUser)
{
	DualtalkEx *pDTalk = (DualtalkEx *)dwUser;
	
	pDTalk->SendDTalkData((U8*)(data+40), size-40);
    if(pDTalk->GetTalkChannel() != -1)
    {
	    pDTalk->WriteLocalMuxData((U8*)(data+40), size-40);
    }
	return 0;
}

void AudioDataCallbackEx(int type, void* data, int size, void* userData)
{
	DualtalkEx *pDTalk = (DualtalkEx *)userData;
	
	pDTalk->SendDTalkData((U8*)data, size);
	Sleep(20);	
}

void AudioDataCallbackExEx(void* data, int size, void* userData)
{
    DualtalkEx *pDTalk = (DualtalkEx *)userData;
	
	pDTalk->SendDTalkData((U8*)data, size);
	
}

#endif

VOID DualtalkEx::AutoSendReq()
{
	U32 u32DevNum = m_mapDTalkDev.size();
	if (0 == u32DevNum)
	{
		return;
	}

    NET_DTALK_DEV_LIST param = {0};
	NET_DTALK_DEV_INFO * pDTalkDev = new NET_DTALK_DEV_INFO[u32DevNum];
    
	S32 s32I = 0;

	S8 **ps8DevIp = new S8 *[u32DevNum];
    for (s32I = 0; s32I < u32DevNum; ++s32I)
    {
		ps8DevIp[s32I] = new S8[64];
    }

	s32I = 0;
	for (map<string, DTALK_DEV_S>::iterator iter = m_mapDTalkDev.begin(); iter != m_mapDTalkDev.end(); ++iter, ++s32I)
	{
		DTALK_DEV_S &devTalkInfo = (*iter).second;
		INET_NTOP(devTalkInfo.u8DevIp, ps8DevIp[s32I]);
        pDTalkDev[s32I].ps8Ip     = ps8DevIp[s32I];
		pDTalkDev[s32I].u16Port   = devTalkInfo.u16DevPort;
		pDTalkDev[s32I].u8ChnNo   = devTalkInfo.u8ChnNo;
	}

	param.u32DevListNum = u32DevNum;
	param.pDevList      = pDTalkDev;
	
	SendDTalkReq(&param, 0);

	delete [] pDTalkDev;

    for (s32I = 0; s32I < u32DevNum; ++s32I)
    {
		delete [] ps8DevIp[s32I];
    }
	delete [] ps8DevIp;
}

VOID DualtalkEx::DualtalkLoop()
{	
	do 
	{
		m_bOpened = FALSE;

		if (!OpenSound())
		{
		    Msleep(1000);
            continue;
		}

		if (!m_pConn->IsLogined() || !OpenStream())
		{
			Msleep(1000);
			continue;
		}

	    //OpenSound(); 

		AutoSendReq();
		
		m_bOpened = TRUE;
		int   timeoutNum = 0;
		time_t tStart    = time(NULL);
		while(!m_bExit)
		{
			BOOL timeout = FALSE;
			if (m_DTalkSock.Select(FALSE, 5000, &timeout))
			{
				MsgBuf msgBuf;
				if (RecvMsg(m_DTalkSock, msgBuf))
				{
					timeoutNum = 0;
					switch (msgBuf.GetMsgSubType())
					{
					case MSG_DTALK_HBEAT:
						DUALTALK_EX_DEBUG("DualTalkEx HeartBeart!!!\n");
						break;																				
						
					default :
						m_MsgTransQueue.Rsp(msgBuf);
						break;
					}
					
					continue;
				}	
				else
				{
                    timeout = TRUE;
				}
			}
			
			if (timeout)
			{
				MsgHeadWrap heartbeat(MSG_DEVTALK, MSG_DTALK_HBEAT);
				heartbeat.u2Version = m_pConn->m_u32MsgVersion;
				
				if (++timeoutNum < 3 && SendMsg(m_DTalkSock, heartbeat))
				{
					DUALTALK_EX_DEBUG("Send dualtalkEx heatbeat\n");
					continue;
				}
			}
			
			break;
		}

		m_DTalkSock.Close();
		
	} while (!m_bExit);

	StopSound();
}

S32 DualtalkEx::StartDualtalkLoop()
{
	S32 s32Ret = CreateJoinableThread(ThreadEntry, this, &m_trdTalk);
	if (s32Ret < 0)
	{
		return -1;
	}
	
	return 0;
}

VOID *DualtalkEx::ThreadEntry(VOID *pArg)
{
	ASSERT( pArg );
	DualtalkEx *pTalk = (DualtalkEx *)pArg;
	
	pTalk->DualtalkLoop();
	
	return NULL;	
}

BOOL DualtalkEx::OpenStream()
{
	DTALK_REQ_S stReq;
	stReq.u32UserKey = m_pConn->GetUserKey();
	MsgHeadWrap msgHead(MSG_DEVTALK, MSG_NO_SUBTYPE, sizeof(stReq));
	msgHead.u2Version = m_pConn->m_u32MsgVersion;
	
	MsgBuf buf;
	if (m_DTalkSock.Socket(TRUE) && \
		m_DTalkSock.SetSendTimeOut(SEND_TIMEOUT) \
		&& m_DTalkSock.SetRecvTimeOut(RECV_TIMEOUT) \
		&& m_DTalkSock.SetSndBuf(512*1024) \
		&& m_DTalkSock.SetRecvBuf(256*1024) \
		&& m_DTalkSock.Connect(m_pConn->GetIP(), m_pConn->GetPort(), CONN_TIMEOUT, TRUE) \
		&& SendMsg(m_DTalkSock, msgHead, &stReq) \
		&& RecvMsg(m_DTalkSock, buf) \
		&& buf.ValidRsp(msgHead) \
		&& buf.IsSuccess())
	{
		return TRUE;
	}
	
	m_DTalkSock.Close();
	return FALSE;
}


/*写入数据*/
void DualtalkEx::WriteMuxData(void *data,int len)
{
    if (m_hMuxAVF != NULL)
    {
        MuxAVF_inputData(m_hMuxAVF,data,len,MUX_DATA_REMOTE);
    }
    
}

/*写入本地数据*/
void DualtalkEx::WriteLocalMuxData(void *data,int len)
{
    if (m_hMuxAVF != NULL)
    {
        MuxAVF_inputData(m_hMuxAVF,data,len,MUX_DATA_LOCAL);
    }
    
}

/*创建封装文件*/
BOOL DualtalkEx::CreateMux()
{
    if (m_hMuxAVF == NULL)
    {
        m_hMuxAVF = MuxAVF_creat(m_muxInitParam.strFilePath, m_muxInitParam.nSubpackageTime, m_muxInitParam.callback,m_muxInitParam.pUser);
        if (m_hMuxAVF != NULL)
        {
            return TRUE;
        }
    }
    return FALSE;
}

/*关闭mux*/
void DualtalkEx::CloseMux()
{
    if (m_hMuxAVF != NULL)
    {
        MuxAVF_close(m_hMuxAVF);
        m_hMuxAVF = NULL;
    }
}


/* 初始化封装文件参数*/
BOOL DualtalkEx::InitMuxAVF(_NET_DEV_MUXAVF_PARAM_ *pParam)
{
    if (pParam->strFilePath != NULL && strlen(pParam->strFilePath)<1024)
    {
        memcpy(m_muxInitParam.strFilePath,pParam->strFilePath,strlen(pParam->strFilePath));
        m_muxInitParam.callback=pParam->callback;
        m_muxInitParam.nSubpackageTime=pParam->nSubpackageTime;
        m_muxInitParam.pUser=pParam->pUser;
        m_bMuxInited = true;
        return TRUE;
    }
    else
        return FALSE;
}


/*获取对讲通道*/
int DualtalkEx::GetTalkChannel()
{
    if (m_bTalked)
    {
// 	    U32 u32DevNum = m_mapDTalkDev.size();
// 	    if (u32DevNum <= 0)
// 	    {
// 		    return -1;
// 	    }
//         map<string, DTALK_DEV_S>::iterator iter = m_mapDTalkDev.begin();
//         DTALK_DEV_S &devTalkInfo = (*iter).second;
//         return devTalkInfo.u8ChnNo;
        return m_curChannel;
    }
    else
	    return -1;
}