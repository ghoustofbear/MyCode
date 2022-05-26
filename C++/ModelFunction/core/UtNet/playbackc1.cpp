#include "playbackc1.h"
#include "connection.h"

#ifdef _WIN32
#include "plaympeg4.h"
//#include " tiandy_play.h"
#include "3rd/tindy/PLAYSDKM4_INTERFACE.h"
#include "3rd/dahua/dhplay.h"
#include "RSPlay.h"
//#include "PublicDef.h"
//#include "3rd/tindy/GlobalTypes.h"

//#pragma comment(lib, "../../lib/PlayCtrl.lib")
//#pragma comment(lib, "../../lib/PlaySdkM4.lib")
#endif

#ifdef _WIN32

struct stHelmetData
{
	U8          u8Type;
	S8 *        s8Data;
	U32         u32Len;
}tagHelmetData;

void CALLBACK SourceBufFun(long nPort, DWORD nBufSize, DWORD dwUser, void *pContext)
{
	RecordPlay *pRecPlay = (RecordPlay *)dwUser;
	pRecPlay->ReqRecordData();
}

void CALLBACK SourceBufFun(UINT message, void* userParam)
{
	RecordPlay *pRecPlay = (RecordPlay *)userParam;

	if (NET_FILE_DATA_NOT_ENOUGH == message)
	{
		pRecPlay->ReqRecordData();

        if( pRecPlay->m_bFileDownloadEnd &&
			pRecPlay->m_pfnRecordPbStatusCb != NULL)
		{
			pRecPlay->m_pfnRecordPbStatusCb(MSG_PLAYBACK_END, pRecPlay->m_pUser);
		}
	}
	else if (FILE_PLAY_END == message)
	{}
}

/* 天地伟业播放控制定时处理函数 */
VOID *PlayCtrlFun(VOID *pArg)
{
	RecordPlay *pRecPlay = (RecordPlay *)pArg;

	while (!pRecPlay->m_bTrdPlayCtrlExit)
	{
		if (/*pRecPlay->m_bDownloadPause*/1)
		{
			if (RecordPlay::FRAME_TYPE_TIANDY == pRecPlay->m_u32FrameType)
			{
				int iState = 0;
                int iRet = PLAYSDK_INTERFACE::TC_GetStreamPlayBufferState(pRecPlay->m_iPlayerID, &iState);
				if(iRet == 0)
				{
					if (iState == -19)
					{
						pRecPlay->ReqRecordData();
						pRecPlay->m_bDownloadPause = FALSE;
						
						if( pRecPlay->m_bFileDownloadEnd &&
							pRecPlay->m_pfnRecordPbStatusCb != NULL)
						{
							pRecPlay->m_pfnRecordPbStatusCb(MSG_PLAYBACK_END, pRecPlay->m_pUser);
						}
					}
				}
			}
			else if (RecordPlay::FRAME_TYPE_DAHUA == pRecPlay->m_u32FrameType)
			{
				if (PLAY_GetSourceBufferRemain(pRecPlay->m_lPort) < 200 * 1024)
				{
					pRecPlay->ReqRecordData();
					pRecPlay->m_bDownloadPause = FALSE;
				}

				DWORD dwBufferRemainSize = PLAY_GetSourceBufferRemain(pRecPlay->m_lPort);
                DWORD dwBufVideoDecSize  = PLAY_GetBufferValue(pRecPlay->m_lPort, BUF_VIDEO_RENDER);
                if (0 == dwBufferRemainSize &&
					0 == dwBufVideoDecSize)
                {
					if( pRecPlay->m_bFileDownloadEnd &&
						pRecPlay->m_pfnRecordPbStatusCb != NULL)
					{
                         pRecPlay->m_pfnRecordPbStatusCb(MSG_PLAYBACK_END, pRecPlay->m_pUser);
					}
                }
			}
			else if (RecordPlay::FRAME_TYPE_HIK == pRecPlay->m_u32FrameType)
			{
				DWORD dwBufRemainSize    = PlayM4_GetSourceBufferRemain(pRecPlay->m_lPort);
				DWORD dwBufVideoDecSize  = PlayM4_GetBufferValue(pRecPlay->m_lPort, BUF_VIDEO_DECODED);
                if (0 == dwBufRemainSize &&
					0 == dwBufVideoDecSize)
				{
				   if( pRecPlay->m_bFileDownloadEnd &&
						pRecPlay->m_pfnRecordPbStatusCb != NULL)
					{
                         pRecPlay->m_pfnRecordPbStatusCb(MSG_PLAYBACK_END, pRecPlay->m_pUser);
					}
				}
			}
		}
		Msleep(40);
	}


	return NULL;
}


VOID *HelmetPlayCtrlFun(VOID *pArg)
{
    RecordPlay *pRecPlay = (RecordPlay *)pArg;
	S32  s32Ret = 0;

	while (!pRecPlay->m_bTrdPlayCtrlExit)
	{
        if (0 == pRecPlay->m_listHelmetData.size())
		{
			Msleep(1);
			continue;
		}

		{
			AutoLock lock(pRecPlay->m_mtxHelmet);

			while(pRecPlay->m_listHelmetData.size() != 0)
			{			
				list<void* >::iterator iter = pRecPlay->m_listHelmetData.begin();			
				
				stHelmetData* pstHelmetData = (stHelmetData*)(*iter);
				if (0 == pstHelmetData->u8Type)
				{
					if(NULL != pRecPlay->m_hPlayerHandle)
					{			
						while(1)
						{
							s32Ret = fnVARender_PumpVideoFrame(pRecPlay->m_hPlayerHandle, pstHelmetData->s8Data, pstHelmetData->u32Len);
							if (VARENDER_E_OK == s32Ret)
							{
								break;
							}
						}	
					}
				}
				else 
				{
					if(NULL != pRecPlay->m_hPlayerHandle)
					{
						fnVARender_PumpAudioFrame(pRecPlay->m_hPlayerHandle, pstHelmetData->s8Data, pstHelmetData->u32Len);
					}					
				}
				
				delete [] pstHelmetData->s8Data;
				delete pstHelmetData;
				
				pRecPlay->m_listHelmetData.erase(iter);
			}
		}
	}

	return NULL;
}

typedef struct
{
	unsigned short  FrameRate;
	unsigned short  Width;
	unsigned short  Height;
} S_header;


BOOL RecordDataCallback(HWND hWnd, VOID *pData, U32 u32Size, VOID *pUser)
{   
	UT_FRAME_HEAD_S *pFrameHead = (UT_FRAME_HEAD_S *)pData;
	RecordPlay *pRecPlay = (RecordPlay *)pUser;

	if ( !pFrameHead || !pRecPlay || (pFrameHead->FrameSize > 1024*1024) )
	{
		return FALSE;
	}

	BYTE *pBuffer = (BYTE *)((S8 *)pData + sizeof(UT_FRAME_HEAD_S));

	if (FRAME_TAG_HIK == pFrameHead->FrameTag)
	{
		pRecPlay->m_u32FrameType = RecordPlay::FRAME_TYPE_HIK;

		if (pFrameHead->FrameType  == FRAME_I)
		{
			/* 已经打开 */
			if (-1 != pRecPlay->m_lPort) {
				return TRUE;
			}

			/* 获取播放库未使用的通道号 */
			if (!PlayM4_GetPort(&pRecPlay->m_lPort)) {
				return FALSE;
			}

			if (u32Size > 0)
			{
				/* 设置文件流播放模式 */
				if (!PlayM4_SetStreamOpenMode(pRecPlay->m_lPort, STREAME_FILE)) {
					return FALSE;
				}

				if (!PlayM4_OpenStream(pRecPlay->m_lPort, pBuffer, pFrameHead->FrameSize, 4*1024*1024)) {
					return FALSE;
				}

				if ( !PlayM4_Play(pRecPlay->m_lPort, hWnd) ) {
					return FALSE;
				}

				if (! PlayM4_SetSourceBufCallBack(pRecPlay->m_lPort, 1*1024*1024, SourceBufFun, (DWORD)pUser, NULL))
				{
					return FALSE;
				}

				CreateJoinableThread(PlayCtrlFun, pUser, &pRecPlay->m_trdPlayCtrl);
			}
		}
		else
		{
			if (pFrameHead->FrameSize > 0 && pRecPlay->m_lPort != -1)
			{
				/* 红外测温*/
				PlayM4_RenderPrivateData(pRecPlay->m_lPort, PLAYM4_RENDER_TEM, TRUE);
				PlayM4_RenderPrivateDataEx(pRecPlay->m_lPort, PLAYM4_RENDER_TEM, 
			                       PLAYM4_TEM_REGION_BOX|PLAYM4_TEM_REGION_LINE|PLAYM4_TEM_REGION_POINT, TRUE);

				if (!PlayM4_InputData(pRecPlay->m_lPort, pBuffer, pFrameHead->FrameSize)) {
					pRecPlay->m_bDownloadPause = TRUE;
					printf("PlayM4_InputData error !! %d\n", PlayM4_GetLastError(pRecPlay->m_lPort));
					return FALSE;
				} 				

				if ( PlayM4_GetSourceBufferRemain(pRecPlay->m_lPort) > 4*1024*1024 - 512*1024) {
					printf("buffer is going to sufficient !!!!!!!!\n");
					
					PlayM4_ResetSourceBufFlag(pRecPlay->m_lPort);

					return FALSE;
				}
			}
		}

	}
	else if (FRAME_TAG_DAHUA == pFrameHead->FrameTag) 
	{ 
		pRecPlay->m_u32FrameType = RecordPlay::FRAME_TYPE_DAHUA; 
		if (-1 == pRecPlay->m_lPort)
		{
			if ( PLAY_GetFreePort(&pRecPlay->m_lPort) == FALSE)
			{
				return FALSE;		
			}

			if (PLAY_SetStreamOpenMode(pRecPlay->m_lPort, STREAME_FILE) == FALSE)
			{
				return FALSE;
			}

			if (PLAY_OpenStream(pRecPlay->m_lPort, 0 ,0, 8*1024*1024) == FALSE)
			{
				return FALSE;
			}
			
			if (PLAY_Play(pRecPlay->m_lPort, hWnd) == FALSE)
			{
				return FALSE;
			}

			CreateJoinableThread(PlayCtrlFun, pUser, &pRecPlay->m_trdPlayCtrl);
		}

		if (pFrameHead->FrameSize > 0)
		{
			if (!PLAY_InputData(pRecPlay->m_lPort, pBuffer, pFrameHead->FrameSize))
			{
				pRecPlay->m_bDownloadPause = TRUE;
				return FALSE;
			} 
		}
	}
	else if (FRAME_TAG_TIANDY == pFrameHead->FrameTag)
	{
		pRecPlay->m_u32FrameType = RecordPlay::FRAME_TYPE_TIANDY;
		
		S_header *sheader = (S_header *)pBuffer;
		if (pFrameHead->FrameType  == FRAME_I)
		{
			if (pRecPlay->m_iPlayerID == -1)
			{
				sheader->Height = 2048;
				sheader->Width  = 2448;
                pRecPlay->m_iPlayerID = PLAYSDK_INTERFACE::TC_CreatePlayerFromVoD(hWnd, pBuffer, pFrameHead->FrameSize);
				if (pRecPlay->m_iPlayerID >= 0)
				{
                    PLAYSDK_INTERFACE::TC_Play(pRecPlay->m_iPlayerID);
                    PLAYSDK_INTERFACE::TC_PlayAudio(pRecPlay->m_iPlayerID);
					CreateJoinableThread(PlayCtrlFun, pUser, &pRecPlay->m_trdPlayCtrl);
				}
			}
			else if (pRecPlay->m_iPlayerID > 0)
			{
				if (pRecPlay->m_iDataLen > 0)
				{
                    PLAYSDK_INTERFACE::TC_PutStreamToPlayer(pRecPlay->m_iPlayerID, pRecPlay->m_cDataBuffer, pRecPlay->m_iDataLen);
				}
			}
		}
		else
		{
			if (pRecPlay->m_iPlayerID >= 0)
			{
				if (pRecPlay->m_iDataLen > 0)
				{
                    PLAYSDK_INTERFACE::TC_PutStreamToPlayer(pRecPlay->m_iPlayerID, pRecPlay->m_cDataBuffer, pRecPlay->m_iDataLen);
					pRecPlay->m_iDataLen = 0;
				}
                int iPushStreamStatus = PLAYSDK_INTERFACE::TC_PutStreamToPlayer(pRecPlay->m_iPlayerID, pBuffer, pFrameHead->FrameSize);
				if (iPushStreamStatus == -18)
				{
					/* 保存最后接收的一帧数据 */
					if (pFrameHead->FrameSize < sizeof(pRecPlay->m_cDataBuffer))
					{
						memcpy(pRecPlay->m_cDataBuffer, pBuffer, pFrameHead->FrameSize);
						pRecPlay->m_iDataLen = pFrameHead->FrameSize;
					}
					pRecPlay->m_bDownloadPause = TRUE;
					return FALSE;
				}
			}
		}
	}
	else  if (FRAME_TAG_STD_H264 == pFrameHead->FrameTag)
	{
		pRecPlay->m_u32FrameType = RecordPlay::FRAME_TYPE_STD_H264;
		
		if (pRecPlay->m_iPlayerID == -1)
		{
			pRecPlay->m_iPlayerID = RSPlayCreatePlayInstance(PLAY_NETFILE_TYPE);
			RSPlaySetPlayWnd(pRecPlay->m_iPlayerID, hWnd);
			RSPlaySetCallbackMessageEx(pRecPlay->m_iPlayerID, SourceBufFun, pUser);
			RSPlayStartPlay(pRecPlay->m_iPlayerID);

			pRecPlay->m_dwFirstUTFrameTime = pFrameHead->Time;
		}

		if (-1 != pRecPlay->m_iPlayerID)
		{	
			HRESULT hr = RSPlayInputNetFrame(pRecPlay->m_iPlayerID, pData, u32Size);		
			if (hr == E_OUTOFMEMORY)
				return FALSE;
		}
	}
	else if (FRAME_TAG_HELMET_GX == pFrameHead->FrameTag)
	{
		S32 s32Ret = 0;
		pRecPlay->m_u32FrameType = RecordPlay::FRAME_TYPE_HELMET_GX;

		if (NULL == pRecPlay->m_hPlayerHandle)
		{
			VARENDER_HANDLE * pHandle = (VARENDER_HANDLE *) &(pRecPlay->m_hPlayerHandle);

			if (NULL == fnVARender_Open)
			{
				return FALSE;
			}

			s32Ret = fnVARender_Open(pHandle);
			if (0 == s32Ret)
			{
				fnVARender_SetWindow(pRecPlay->m_hPlayerHandle, hWnd);
				
				fnVARender_StartVideo(pRecPlay->m_hPlayerHandle);
				
				fnVARender_StartAudio(pRecPlay->m_hPlayerHandle);

				fnVARender_SetPlayMode(pRecPlay->m_hPlayerHandle, VARENDER_PLAYMODE_REPLAY);
	            fnVARender_SetSpeed(pRecPlay->m_hPlayerHandle, 0);				

				if (!pRecPlay->m_bHelmetPlayCtrlTime)
				{
					CreateJoinableThread(HelmetPlayCtrlFun, pUser, &pRecPlay->m_hHelmetPlayCtrl);

					pRecPlay->m_tmHelmetBeginGx = ((UINT*)pBuffer)[1];
					pRecPlay->m_bHelmetPlayCtrlTime = true;
				}			
			}
		}
		
		if (NULL != pRecPlay->m_hPlayerHandle)
		{		
			do 
			{
				stHelmetData * pstHelmetData = new stHelmetData;
				if (NULL == pstHelmetData)
				{
					break;
				}

				pstHelmetData->u32Len = pFrameHead->FrameSize;
				pstHelmetData->u8Type = 1;
				if (FRAME_A != pFrameHead->FrameType)
				{
					pstHelmetData->u8Type = 0;
				}

				pstHelmetData->s8Data = new S8[pFrameHead->FrameSize];
				if (NULL == pstHelmetData->s8Data)
				{
					delete pstHelmetData;
					break;
				}
				memcpy(pstHelmetData->s8Data, pBuffer, pFrameHead->FrameSize);
				
				AutoLock lock(pRecPlay->m_mtxHelmet);
				pRecPlay->m_listHelmetData.push_back(pstHelmetData);
			} while (0);
		}
	}
	else {}

	return TRUE;
}     

#endif


RecordPlay::RecordPlay(Connection *pConn)
    :BaseRecord()
{
	m_pConn  = pConn;
	m_lPort  = -1;
	m_u32Pos = 0;
	m_tmBeginSec = 0;
	m_tmEndSec   = 0;
	m_iPlayerID   = -1;
	m_iDataLen    = 0;
	m_trdPlayCtrl = 0;
	m_bTrdPlayCtrlExit = FALSE;
	m_bDownloadPause   = FALSE;
	m_u32FrameType     = FRAME_TYPE_HIK;
	m_iPlaySpeed       = 0;
	m_dwFirstUTFrameTime = 0;
	m_dwPlayedTime     = 0;
	m_dwPreTagTime     = 0;
	m_bFileDownloadEnd = FALSE;
	m_pfnRecordPbStatusCb = NULL;
	m_pUser = NULL;
	m_hPlayerHandle = NULL;
	m_hHelmetPlayCtrl = NULL;
	m_tmHelmetBeginGx = 0;
	m_bHelmetPlayCtrlTime = false;
}

BOOL RecordPlay::Start(const NET_RECPLAY_PARAM_S *pParam)
{
	ASSERT(pParam);

	m_stRecPlayParam = *pParam;
	m_u32PlayMode    = pParam->u8PlayMode;

	if (REC_PLAY_IP_TIME == m_u32PlayMode)
	{
		ASSERT(pParam->stIpMode.ps8DeviceIp);
		m_strDeviceIp = pParam->stIpMode.ps8DeviceIp;
		if (pParam->stIpMode.ps8SubServIp)
			m_strSubServIp = pParam->stIpMode.ps8SubServIp;

		struct tm timeinfo;
		timeinfo.tm_year = m_stRecPlayParam.stIpMode.stBegin.u32Year  - 1900;
		timeinfo.tm_mon  = m_stRecPlayParam.stIpMode.stBegin.u32Month - 1;
		timeinfo.tm_mday = m_stRecPlayParam.stIpMode.stBegin.u32Day;
		timeinfo.tm_hour = m_stRecPlayParam.stIpMode.stBegin.u32Hour  ;
		timeinfo.tm_min  = m_stRecPlayParam.stIpMode.stBegin.u32Minute;
		timeinfo.tm_sec  = m_stRecPlayParam.stIpMode.stBegin.u32Second;

        m_tmBeginSec     = mktime(&timeinfo);

		timeinfo.tm_year = m_stRecPlayParam.stIpMode.stEnd.u32Year  - 1900;
		timeinfo.tm_mon  = m_stRecPlayParam.stIpMode.stEnd.u32Month - 1;
		timeinfo.tm_mday = m_stRecPlayParam.stIpMode.stEnd.u32Day;
		timeinfo.tm_hour = m_stRecPlayParam.stIpMode.stEnd.u32Hour  ;
		timeinfo.tm_min  = m_stRecPlayParam.stIpMode.stEnd.u32Minute;
		timeinfo.tm_sec  = m_stRecPlayParam.stIpMode.stEnd.u32Second;
		
	    m_tmEndSec       = mktime(&timeinfo);
	}
	else if (REC_PLAY_IP_NAME == m_u32PlayMode)
	{
		ASSERT(pParam->stIpMode.ps8DeviceIp);
		m_strDeviceIp  = pParam->stIpMode.ps8DeviceIp;
		if (pParam->stIpMode.ps8SubServIp)
			m_strSubServIp = pParam->stIpMode.ps8SubServIp;

		m_strFileName = pParam->stIpMode.ps8FileName;
	}
	else if (REC_PLAY_ADDR_URL == m_u32PlayMode)
	{
		m_strFrontAddrCode = pParam->stAddrMode.ps8FrontAddrCode;
		m_strRtspUrl       = pParam->stAddrMode.ps8RtspUrl;
	}
	else if (REC_PLAY_ADDR_TIME == m_u32PlayMode)
	{
		m_strFrontAddrCode = pParam->stAddrMode.ps8FrontAddrCode;

		m_tmBeginSec = m_stRecPlayParam.stAddrMode.stBegin.u32Hour * 3600 + \
            m_stRecPlayParam.stAddrMode.stBegin.u32Minute * 60 + \
            m_stRecPlayParam.stAddrMode.stBegin.u32Second;
		
		DATETIME_S stBegin = {0};
		DATETIME_S stEnd   = {0};
		stBegin.u32Year  = m_stRecPlayParam.stAddrMode.stBegin.u32Year;
		stBegin.u32Month = m_stRecPlayParam.stAddrMode.stBegin.u32Month;
		stBegin.u32Day   = m_stRecPlayParam.stAddrMode.stBegin.u32Day;

		stEnd.u32Year  = m_stRecPlayParam.stAddrMode.stEnd.u32Year;
		stEnd.u32Month = m_stRecPlayParam.stAddrMode.stEnd.u32Month;
		stEnd.u32Day   = m_stRecPlayParam.stAddrMode.stEnd.u32Day;

		/* 跨天 */
		if (CompareDateTime(&stEnd, &stBegin) > 0)
		{
			m_tmEndSec = (m_stRecPlayParam.stAddrMode.stEnd.u32Hour + 24) * 3600 + \
				m_stRecPlayParam.stAddrMode.stEnd.u32Minute * 60 + \
			m_stRecPlayParam.stAddrMode.stEnd.u32Second;
		}
		else
		{
			m_tmEndSec = m_stRecPlayParam.stAddrMode.stEnd.u32Hour * 3600 + \
				m_stRecPlayParam.stAddrMode.stEnd.u32Minute * 60 + \
			m_stRecPlayParam.stAddrMode.stEnd.u32Second;
		}
	}

	m_bAutoReconn = TRUE;
	m_bCanSendMsg = FALSE;
	m_bNeedData   = TRUE;
	m_bReposing   = FALSE;
	m_bNoData     = FALSE;
	m_bExit       = FALSE;
	m_LastKeyFrameTime = 0;
	m_dwPlayedTime     = 0;

	memset(&m_LastRecvFrame, 0, sizeof(m_LastRecvFrame));

	CreateJoinableThread(RecordPlayFun, this, &m_trdRecPlay);
	return TRUE;
}

VOID RecordPlay::Stop()
{
	m_bAutoReconn = FALSE;
	m_bExit      = TRUE;
	
	{
		AutoLock lock(m_mtxSock);
		m_sockRecPlay.KillSocket();
	}
	
	for (S32 s32I = 0; s32I < 10; s32I++)
		m_semMsgSendQueue.Post();
	
	ThreadJoin(m_trdRecPlay);
	
	AutoLock lock(m_mtxMsgSendQueue);
	while (!m_lsMsgSendQueue.empty())
	{
		delete [] m_lsMsgSendQueue.front();
		m_lsMsgSendQueue.pop_front();
	}

#ifdef _WIN32
	m_bTrdPlayCtrlExit = TRUE;
	ThreadJoin(m_trdPlayCtrl);
	ThreadJoin(m_hHelmetPlayCtrl);

	if (FRAME_TYPE_HIK == m_u32FrameType)
	{
		if (-1 != m_lPort)
		{
			PlayM4_CloseStreamEx(m_lPort);
			PlayM4_Stop(m_lPort);
			PlayM4_FreePort(m_lPort);
			m_lPort = -1;
		}
	}
	else if (FRAME_TYPE_TIANDY == m_u32FrameType)
	{
		if (-1 != m_iPlayerID)
		{
            PLAYSDK_INTERFACE::TC_Stop(m_iPlayerID);
            PLAYSDK_INTERFACE::TC_DeletePlayer(m_iPlayerID);
			m_iPlayerID = -1;
		}
	}
	else if (FRAME_TYPE_DAHUA == m_u32FrameType)
	{
		if (-1 != m_lPort)
		{
			PLAY_Stop(m_lPort);
			m_lPort = -1;
		}
	}
	else if (FRAME_TYPE_STD_H264 == m_u32FrameType)
	{
		if (-1 != m_iPlayerID)
		{
			RSPlayStopPlay(m_iPlayerID);
			RSPlayDestroyPlayInstance(m_iPlayerID);
			m_iPlayerID = -1;
		}
	}
	else if (FRAME_TYPE_HELMET_GX == m_u32FrameType)
	{
		if (NULL != m_hPlayerHandle)
		{
			fnVARender_StopVideo(m_hPlayerHandle);
			fnVARender_StopAudio(m_hPlayerHandle);
			fnVARender_Close(m_hPlayerHandle);
			m_hPlayerHandle = NULL;
		}
		m_bHelmetPlayCtrlTime = false;
	}
				
#endif
	
}

BOOL RecordPlay::PlayCtrl(U32 u32Code, VOID *pInVal, U32 u32InSize, VOID *pOutVal, U32 *u32OutSize)
{
	switch (u32Code)
	{
		case NET_PLAYSTART:
			return TRUE;

		case NET_PLAYSTOP:	
#ifdef _WIN32
			if (FRAME_TYPE_HIK == m_u32FrameType)
			{
				if (-1 != m_lPort)
				{
					return PlayM4_Stop(m_lPort);
				}
			}
			else if (FRAME_TYPE_TIANDY == m_u32FrameType)
			{
				if (-1 != m_iPlayerID)
				{
                    PLAYSDK_INTERFACE::TC_Stop(m_iPlayerID);
				}
			}
			else if (FRAME_TYPE_DAHUA == m_u32FrameType)
			{
				if (-1 != m_lPort)
				{
					PLAY_Stop(m_iPlayerID);
				}
			}
			else if (FRAME_TYPE_STD_H264 == m_u32FrameType)
			{
				if (-1 != m_iPlayerID)
				{
					return RSPlaySetPlayMode(m_iPlayerID, PLAY_MODE_STOP);
				}
			}
			else if (FRAME_TYPE_HELMET_GX == m_u32FrameType)
			{
				if (NULL != m_hPlayerHandle)
				{
					fnVARender_StopVideo(m_hPlayerHandle);
					fnVARender_StopAudio(m_hPlayerHandle);
					fnVARender_Close(m_hPlayerHandle);
					m_hPlayerHandle = NULL;
				}
			}
#endif
			break;

		case NET_PLAYPAUSE:	{
			AutoLock lock(m_mtxReqData);
			MsgHeadWrap head(MSG_PLAY_FEEDBACK, MSG_PLAY_ENOUGH);
			head.u2Version = m_pConn->m_u32MsgVersion;
			SendMsg(head);
			m_bNeedData = FALSE;

#ifdef _WIN32
			if (FRAME_TYPE_HIK == m_u32FrameType)
			{
				if (-1 != m_lPort)
				{
					return PlayM4_Pause(m_lPort, TRUE);
				}
			}
			else if (FRAME_TYPE_TIANDY == m_u32FrameType)
			{
				if (-1 != m_iPlayerID)
				{
                    PLAYSDK_INTERFACE::TC_Pause(m_iPlayerID);
				}
			}		
			else if (FRAME_TYPE_DAHUA == m_u32FrameType)
			{
				if (-1 != m_lPort)
				{
					return PLAY_Pause(m_lPort, TRUE);
				}
			}
			else if (FRAME_TYPE_STD_H264 == m_u32FrameType)
			{
				if (-1 != m_iPlayerID)
				{
					return RSPlaySetPlayMode(m_iPlayerID, PLAY_MODE_PAUSE);
				}
			}
			else if(FRAME_TYPE_HELMET_GX == m_u32FrameType)
			{
				if (NULL != m_hPlayerHandle)
				{

				}
			}
#endif
			break;
		}

		case NET_PLAYRESTART:
			ReqRecordData();
#ifdef _WIN32
			if (FRAME_TYPE_HIK == m_u32FrameType)
			{
				if (-1 != m_lPort)
				{
					return PlayM4_Pause(m_lPort, FALSE);
				}
			}
			else if (FRAME_TYPE_TIANDY == m_u32FrameType)
			{
				if (-1 != m_iPlayerID)
				{
                    PLAYSDK_INTERFACE::TC_Play(m_iPlayerID);
				}
			}
			else if (FRAME_TYPE_DAHUA == m_u32FrameType)
			{
				if (-1 != m_lPort)
				{
					return PLAY_Pause(m_lPort, FALSE);
				}
			}
			else if (FRAME_TYPE_STD_H264 == m_u32FrameType)
			{
				if (-1 != m_iPlayerID)
				{
					return RSPlaySetPlayMode(m_iPlayerID, PLAY_MODE_NORMAL);
				}
			}
			else if(FRAME_TYPE_HELMET_GX == m_u32FrameType)
			{
				if (NULL != m_hPlayerHandle)
				{
					
				}
			}
#endif
			break;

		case NET_PLAYFAST:	
#ifdef _WIN32
			if (FRAME_TYPE_HIK == m_u32FrameType)
			{
				if (-1 != m_lPort)
				{
					return PlayM4_Fast(m_lPort);
				}
			}
			else if (FRAME_TYPE_TIANDY == m_u32FrameType)
			{
				if( -1 != m_iPlayerID )
				{
					m_iPlaySpeed++;

					if (m_iPlaySpeed > 4)
						m_iPlaySpeed = 4;

					if (m_iPlaySpeed < 0)
                        PLAYSDK_INTERFACE::TC_SlowForward(m_iPlayerID, abs(m_iPlaySpeed));
					else if (m_iPlaySpeed > 0)
                        PLAYSDK_INTERFACE::TC_FastForward(m_iPlayerID, m_iPlaySpeed);
					else
                        PLAYSDK_INTERFACE::TC_Play(m_iPlayerID);
				}
			}
			else if (FRAME_TYPE_DAHUA == m_u32FrameType)
			{
				if (-1 != m_lPort)
				{
					return PLAY_Fast(m_lPort);
				}
			}
			else if (FRAME_TYPE_STD_H264 == m_u32FrameType)
			{
				if (-1 != m_iPlayerID)
				{
					m_iPlaySpeed++;
					
					if (m_iPlaySpeed > 4)
						m_iPlaySpeed = 4;

					if (m_iPlaySpeed < 0)
						RSPlaySetPlayMode(m_iPlayerID, (PLAY_MODE_SLOW - 1) + abs(m_iPlaySpeed));
					else if (m_iPlaySpeed > 0)
						RSPlaySetPlayMode(m_iPlayerID, (PLAY_MODE_FAST_FORWARD2 - 1) + m_iPlaySpeed);
					else
						RSPlaySetPlayMode(m_iPlayerID, PLAY_MODE_NORMAL);	
					
					{
						if (m_iPlaySpeed >= 0)
						{
							MsgHeadWrap head(MSG_PLAY_FEEDBACK, MSG_PLAY_FAST);
							head.u2Version = m_pConn->m_u32MsgVersion;
							head.u8Reserved = m_iPlaySpeed;
							SendMsg(head);
						}
						else
						{
							MsgHeadWrap head(MSG_PLAY_FEEDBACK, MSG_PLAY_SLOW);
							head.u2Version = m_pConn->m_u32MsgVersion;
							head.u8Reserved = 255 + m_iPlaySpeed;
							SendMsg(head);
						}	
					}
				}
			}
			else if(FRAME_TYPE_HELMET_GX == m_u32FrameType)
			{
				m_iPlaySpeed++;
				
				if (m_iPlaySpeed > 4)
					m_iPlaySpeed = 4;
				fnVARender_SetSpeed(m_hPlayerHandle, m_iPlaySpeed);

				break;
			}
#endif
			break;

		case NET_PLAYSLOW:	
#ifdef _WIN32
			if (FRAME_TYPE_HIK == m_u32FrameType)
			{
				if (-1 != m_lPort)
				{
					return PlayM4_Slow(m_lPort);
				}
			}
			else if (FRAME_TYPE_TIANDY == m_u32FrameType)
			{
				if( -1 != m_iPlayerID )
				{
					m_iPlaySpeed--;
					
					if (m_iPlaySpeed < -4)
						m_iPlaySpeed = -4;
					
					if (m_iPlaySpeed > 0)
                        PLAYSDK_INTERFACE::TC_FastForward(m_iPlayerID, m_iPlaySpeed);
					else if (m_iPlaySpeed < 0)
                        PLAYSDK_INTERFACE::TC_SlowForward(m_iPlayerID, abs(m_iPlaySpeed));
					else
                        PLAYSDK_INTERFACE::TC_Play(m_iPlayerID);
				}
			}
			else if (FRAME_TYPE_DAHUA == m_u32FrameType)
			{
				if (-1 != m_lPort)
				{
					return PLAY_Slow(m_lPort);
				}
			}
			else if (FRAME_TYPE_STD_H264 == m_u32FrameType)
			{
				if (-1 != m_iPlayerID)
				{
					m_iPlaySpeed--;
					
					if (m_iPlaySpeed < -4)
						m_iPlaySpeed = -4;
					
					if (m_iPlaySpeed < 0)
						RSPlaySetPlayMode(m_iPlayerID, (PLAY_MODE_SLOW - 1) + abs(m_iPlaySpeed));
					else if (m_iPlaySpeed > 0)
						RSPlaySetPlayMode(m_iPlayerID, (PLAY_MODE_FAST_FORWARD2 - 1) + m_iPlaySpeed);
					else
						RSPlaySetPlayMode(m_iPlayerID, PLAY_MODE_NORMAL);
					{
						if (m_iPlaySpeed >= 0)
						{
							MsgHeadWrap head(MSG_PLAY_FEEDBACK, MSG_PLAY_FAST);
							head.u2Version = m_pConn->m_u32MsgVersion;
							head.u8Reserved = m_iPlaySpeed;
							SendMsg(head);
						}
						else
						{
							MsgHeadWrap head(MSG_PLAY_FEEDBACK, MSG_PLAY_SLOW);
							head.u2Version = m_pConn->m_u32MsgVersion;
							head.u8Reserved = 255 + m_iPlaySpeed;
							SendMsg(head);
						}					
					}
				}
			}
			else if(FRAME_TYPE_HELMET_GX == m_u32FrameType)
			{
				m_iPlaySpeed--;
				
				if (m_iPlaySpeed < -4)
					m_iPlaySpeed = -4;
                fnVARender_SetSpeed(m_hPlayerHandle, m_iPlaySpeed);

				break;
			}

#endif
			break;

		case NET_PLAYNORMAL:	
			ReqRecordData();
#ifdef _WIN32
			if (FRAME_TYPE_HIK == m_u32FrameType)
			{
				if (-1 != m_lPort)
				{
					return PlayM4_SetPlayMode(m_lPort, TRUE);
				}
			}
			else if (FRAME_TYPE_TIANDY == m_u32FrameType)
			{
				if( -1 != m_iPlayerID )
				{
                    PLAYSDK_INTERFACE::TC_Play(m_iPlayerID);
				}
			}
			else if (FRAME_TYPE_DAHUA == m_u32FrameType)
			{
				if (-1 != m_lPort)
				{
					return PLAY_Play(m_lPort, NULL);
				}
			}
			else if (FRAME_TYPE_STD_H264 == m_u32FrameType)
			{
				if (-1 != m_iPlayerID)
				{
					RSPlaySetPlayMode(m_iPlayerID, PLAY_MODE_NORMAL);
				}

				{
					m_iPlaySpeed = 1;
					MsgHeadWrap head(MSG_PLAY_FEEDBACK, MSG_PLAY_FAST);
					head.u2Version = m_pConn->m_u32MsgVersion;
					head.u8Reserved = 1;
					SendMsg(head);
				}
			}
#endif
			break;

		case NET_PLAYSTARTAUDIO:
			{
                OpenSound();
			    break;
			}	
		case NET_PLAYSTOPAUDIO:
			{
				CloseSound();
				break;
			}
		case NET_PLAYSETPOS: {
			U32 u32Pos = *(U32 *)pInVal;
			/* 当前播放文件总时间(秒数) */

			DWORD dwFileSec =  m_tmEndSec - m_tmBeginSec;
			
			/* 当前拖动偏移量(秒数) */
			DWORD dwOffset = (u32Pos * dwFileSec) / 100;
			
			/* 计算重定位时间点 */
			time_t tmReposSec = m_tmBeginSec + dwOffset;
			
			DATETIME_S stReposTime = {0};

			REC_REPOSE_REQ_S stReq;
			memset(&stReq, 0, sizeof(stReq));
	
			if (REC_PLAY_ADDR_TIME == m_u32PlayMode)
			{	
				DWORD dwRemain   = 0;
			
				stReposTime.u32Hour  = tmReposSec / 3600;
				dwRemain = tmReposSec % 3600;
				
				stReposTime.u32Minute =  dwRemain / 60;
				dwRemain = dwRemain % 60;
				
				stReposTime.u32Second = dwRemain;

				stReq.stBegin = stReposTime;

				stReq.stBegin.u32Year  = m_stRecPlayParam.stAddrMode.stBegin.u32Year;
				stReq.stBegin.u32Month = m_stRecPlayParam.stAddrMode.stBegin.u32Month;

				/* 跨天 */
				if (stReq.stBegin.u32Hour >= 24) 
				{
					stReq.stBegin.u32Hour -= 24;
					stReq.stBegin.u32Day   = m_stRecPlayParam.stAddrMode.stEnd.u32Day;
				}
				else
				{
					stReq.stBegin.u32Day   = m_stRecPlayParam.stAddrMode.stBegin.u32Day;
				}
				
				stReq.stEnd = m_stRecPlayParam.stAddrMode.stEnd;
			}
			else
			{
				struct tm * timeinfo;
				timeinfo = localtime(&tmReposSec);
				
				stReposTime.u32Year   = timeinfo->tm_year + 1900;
				stReposTime.u32Month  = timeinfo->tm_mon  + 1;
				stReposTime.u32Day    = timeinfo->tm_mday;
				stReposTime.u32Hour   = timeinfo->tm_hour;
				stReposTime.u32Minute = timeinfo->tm_min;
				stReposTime.u32Second = timeinfo->tm_sec;

				stReq.stBegin = stReposTime;
				
				stReq.stEnd = m_stRecPlayParam.stIpMode.stEnd;
			}
		//	ReposRecordData(&stReq);

			m_dwPreTagTime = time(NULL);
			m_dwPlayedTime = dwOffset;
			m_bFileDownloadEnd = FALSE;          

#ifdef _WIN32
			if (FRAME_TYPE_HIK == m_u32FrameType)
			{
				if (-1 != m_lPort)
				{
					PlayM4_ResetSourceBuffer(m_lPort);
				}
			}
			else if (FRAME_TYPE_TIANDY == m_u32FrameType)
			{
				if( -1 != m_iPlayerID )
				{
                    PLAYSDK_INTERFACE::TC_CleanStreamBuffer(m_iPlayerID);
				}
			}
			else if (FRAME_TYPE_DAHUA == m_u32FrameType)
			{
				if( -1 != m_lPort )
				{
					PLAY_ResetSourceBuffer(m_lPort);
				}
			}
			else if (FRAME_TYPE_HELMET_GX == m_u32FrameType)
			{
				AutoLock lock(m_mtxHelmet);

#if 0
				while(m_listHelmetData.size() != 0)
				{			
					list<void* >::iterator iter = m_listHelmetData.begin();
					stHelmetData* pstHelmetData = (stHelmetData*)(*iter);
					delete [] pstHelmetData->s8Data;
					delete pstHelmetData;
					
					m_listHelmetData.erase(iter);
				}
#endif

				if (NULL != m_hPlayerHandle)
				{
					fnVARender_StopVideo(m_hPlayerHandle);
					fnVARender_StopAudio(m_hPlayerHandle);
					fnVARender_Close(m_hPlayerHandle);
					m_hPlayerHandle = NULL;
				}
			}
#endif
			m_u32Pos = u32Pos;

			ReposRecordData(&stReq);

			break;
		}
		case NET_PLAYSETTIME: {
			REC_REPOSE_REQ_S stReq;
			memset(&stReq, 0, sizeof(stReq));

			stReq.stBegin = *(DATETIME_S *)pInVal;
			
			if (REC_PLAY_ADDR_TIME == m_u32PlayMode)
			{
				stReq.stEnd = m_stRecPlayParam.stAddrMode.stEnd;
			}
			else
			{
				stReq.stEnd = m_stRecPlayParam.stIpMode.stEnd;
			}

			ReposRecordData(&stReq);

#ifdef _WIN32
			if (-1 != m_lPort)
			{
				PlayM4_ResetSourceBuffer(m_lPort);
			}
#endif
			break;
		}
		case NET_PLAYSTEP:
			{
#ifdef _WIN32
            if(FRAME_TYPE_TIANDY == m_u32FrameType)
			{
				if (-1 != m_iPlayerID)
				{
                    PLAYSDK_INTERFACE::TC_StepForward(m_iPlayerID);
				}		
			}
			else if (FRAME_TYPE_DAHUA == m_u32FrameType)
			{
				if (-1 != m_lPort)
				{
					PLAY_OneByOne(m_lPort);
				}
			}
			else if (FRAME_TYPE_HIK == m_u32FrameType)
			{
				if (-1 != m_lPort)
				{
                    PlayM4_OneByOne(m_lPort);
				}
			}
#endif
			break;
			}
		default:
			return FALSE;
	}

	return TRUE;
}

VOID RecordPlay::ReqRecordData()
{
	AutoLock lock(m_mtxReqData);
	if (!m_bNoData)
	{
		m_bNeedData = TRUE;
		MsgHeadWrap head(MSG_PLAY_FEEDBACK, MSG_PLAY_INSUFFICIENT);
		head.u2Version = m_pConn->m_u32MsgVersion;
		SendMsg(head);
	}
}

VOID RecordPlay::ReposRecordData(const REC_REPOSE_REQ_S *pReq)
{
	BOOL shouldWait = FALSE;

	{
// 		m_stRecPlayParam.stBegin = pReq->stBegin;
// 		m_stRecPlayParam.stEnd   = pReq->stEnd;

		if (!m_bCanSendMsg)
		{
			memset(&m_LastRecvFrame, 0, sizeof(m_LastRecvFrame));
			m_LastKeyFrameTime = 0;
		}
		else
		{
			MsgHeadWrap head(MSG_PLAY_REPOS, MSG_NO_SUBTYPE, sizeof(*pReq));
			head.u2Version = m_pConn->m_u32MsgVersion;
			shouldWait = SendMsg(head, pReq);
			if (shouldWait)
				printf("repos record %u:%u:%u\n", pReq->stBegin.u32Hour, pReq->stBegin.u32Minute, pReq->stBegin.u32Second);
			AutoLock lock(m_mtxRepos);
			m_bReposing = shouldWait;
		}
	}

	if (shouldWait)
	{
		m_evReposDone.Wait();
	}
}

#if 0
void ReplayRecordData()
{
}
#endif
VOID *RecordPlay::RecordPlayFun(VOID *pArg)
{
	RecordPlay *pRecPlay = (RecordPlay *)pArg;
	pRecPlay->RecordPlayLoop();

	return 0;
}

VOID RecordPlay::RecordPlayLoop()
{
	do 
	{
		if (m_bExit)
		{
			break;
		}

		if (!m_pConn->IsLogined())
		{
			Msleep(100);
			continue;
		}

		//RecordPlayStatusFun(RSNetMsgRecordPlayOpenStream);
		BOOL streamOpened = OpenStream();
		if (streamOpened)
		{
			//RecordPlayStatusFun(RSNetMsgRecordPlayOpenStreamSuccess);

			//mSendWorkDown.Reset();
			CreateJoinableThread(SendFun, this, &m_trdSend);

			{
				AutoLock lock(m_mtxMsgSendQueue);
				m_bCanSendMsg = TRUE;
			}

			Recv();

			if (m_bFileDownloadEnd)
			{
				m_bExit = TRUE;
			}
		}
		else
		{
			//RecordPlayStatusFun(RSNetMsgRecordPlayOpenStreamFail);
			if (m_bAutoReconn)
			{
				Msleep(1000);
				//mStopRecordPlayEvent.Wait(1000);
			}
		}

		if (streamOpened)
		{
			//RecordPlayStatusFun(RSNetMsgRecordPlayClosed);

			{
				AutoLock lock(m_mtxMsgSendQueue);
				m_bCanSendMsg = FALSE;
				{
					AutoLock lock(m_mtxRepos);
					if (m_bReposing)
					{
						memset(&m_LastRecvFrame, 0, sizeof(m_LastRecvFrame));
						m_LastKeyFrameTime = 0;
						m_bNeedData = TRUE;
						m_bReposing = FALSE;
					}
					m_evReposDone.Notify();
				}
			}

			for (S32 s32I = 0; s32I < 10; s32I++)
				m_semMsgSendQueue.Post();
			ThreadJoin(m_trdSend);

			AutoLock lock(m_mtxMsgSendQueue);
			while (!m_lsMsgSendQueue.empty())
			{
				m_semMsgSendQueue.Wait();
				delete [] m_lsMsgSendQueue.front();
				m_lsMsgSendQueue.pop_front();
			}
		}

	} while (m_bAutoReconn);

	//mRecordPlayDownEvent.Set();
}


BOOL RecordPlay::OpenStream()
{
	{
		AutoLock lock(m_mtxSock);
		if (!m_sockRecPlay.Socket(TRUE))
			return FALSE;
	}

	REC_PLAY_REQ_S stReq;
	memset(&stReq, 0, sizeof(stReq));

	stReq.u32UserKey     = m_pConn->GetUserKey();

	if (REC_PLAY_IP_TIME == m_u32PlayMode)
	{
		stReq.u8PlayMode     = REC_PLAY_IP_TIME;
		stReq.stIpMode.stBegin        = m_stRecPlayParam.stIpMode.stBegin;
		stReq.stIpMode.stEnd          = m_stRecPlayParam.stIpMode.stEnd;
		stReq.stIpMode.u32ChnNO       = m_stRecPlayParam.stIpMode.u32ChnNO;
		stReq.stIpMode.u32Type        = m_stRecPlayParam.stIpMode.u32Type;
		stReq.stIpMode.u16DevicePort  = m_stRecPlayParam.stIpMode.u16DevicePort;
		stReq.stIpMode.u16SubServPort = m_stRecPlayParam.stIpMode.u16SubServPort;
		INET_PTON(m_strDeviceIp.c_str(),  stReq.stIpMode.u8DeviceIp);
		INET_PTON(m_strSubServIp.c_str(), stReq.stIpMode.u8SubServIp);
		printf("open record %u:%u:%u\n", stReq.stIpMode.stBegin.u32Hour, stReq.stIpMode.stBegin.u32Minute, stReq.stIpMode.stBegin.u32Second);
	}
	else if (REC_PLAY_IP_NAME == m_u32PlayMode)
	{
		stReq.u8PlayMode = REC_PLAY_IP_NAME;
		strncpy(stReq.stIpMode.s8FileName, m_strFileName.c_str(), MAX_FILENAME_LEN);
		stReq.stIpMode.u16DevicePort  = m_stRecPlayParam.stIpMode.u16DevicePort;
		stReq.stIpMode.u16SubServPort = m_stRecPlayParam.stIpMode.u16SubServPort;
		INET_PTON(m_strDeviceIp.c_str(),  stReq.stIpMode.u8DeviceIp);
		INET_PTON(m_strSubServIp.c_str(), stReq.stIpMode.u8SubServIp);
	}
	else if (REC_PLAY_ADDR_URL == m_u32PlayMode)
	{
		stReq.u8PlayMode = REC_PLAY_ADDR_URL;
		strncpy(stReq.stAddrMode.s8FrontAddrCode, m_strFrontAddrCode.c_str(), MAX_ADDR_LEN);
		strncpy(stReq.stAddrMode.s8RstpUrl, m_strRtspUrl.c_str(), MAX_FILENAME_LEN);
	}
	else if (REC_PLAY_ADDR_TIME == m_u32PlayMode)
	{
		stReq.u8PlayMode = REC_PLAY_ADDR_TIME;
		strncpy(stReq.stAddrMode.s8FrontAddrCode, m_strFrontAddrCode.c_str(), MAX_ADDR_LEN);

		stReq.stAddrMode.stBegin = m_stRecPlayParam.stAddrMode.stBegin;
		stReq.stAddrMode.stEnd   = m_stRecPlayParam.stAddrMode.stEnd;
	}

	{
		AutoLock lock(m_mtxMsgSendQueue);
		if (m_LastKeyFrameTime != 0)
		{
			m_LastKeyFrameTime = 0;
		}
		C_ASSERT(sizeof(stReq.u8ExactFrame) == sizeof(m_LastRecvFrame));
		*(UT_FRAME_HEAD_S *)stReq.u8ExactFrame = m_LastRecvFrame;
	}

	MsgHeadWrap head(MSG_PLAYBACK, MSG_NO_SUBTYPE, sizeof(stReq));
	head.u2Version = m_pConn->m_u32MsgVersion;

	MsgBuf buf;

	if (m_sockRecPlay.SetSendTimeOut(SEND_TIMEOUT) 
			&& m_sockRecPlay.SetRecvTimeOut(RECV_TIMEOUT) 
			&& m_sockRecPlay.SetRecvBuf(64*1024) 
			&& m_sockRecPlay.Connect(m_pConn->GetIP(), m_pConn->GetPort(), CONN_TIMEOUT, TRUE) 
			&& m_pConn->IsLogined() 
			&& ::SendMsg(m_sockRecPlay, head, &stReq) 
			&& RecvMsg(m_sockRecPlay, buf) 
			&& buf.ValidRsp(head) 
			&& buf.IsSuccess())
	{
		return TRUE;
	}

	AutoLock lock(m_mtxSock);
	m_sockRecPlay.Close();

	return FALSE;
}

VOID RecordPlay::Recv()
{
	FrameBuf frameBuf;
	PackBuf  packBuf;

	UINT reqDataTimes = 0;

	if (m_bNeedData)
	{
		ReqRecordData();
		++reqDataTimes;
	}

	do 
	{
		if (!packBuf.RecvPackTcp(m_sockRecPlay))
		{
#ifdef _WIN32
			S32 err = WSAGetLastError();
			if (err != WSAETIMEDOUT)
			{
				break;
			}
#endif

			{
				AutoLock lock(m_mtxRepos);
				if (m_bReposing)
				{
					m_bNeedData = TRUE;
					reqDataTimes = 2;
				}
			}
			if (reqDataTimes >= 2)
			{
				break;
			}

			if (m_bNeedData)
			{
				ReqRecordData();
				++reqDataTimes;
			}
			else
			{
				MsgHeadWrap head(MSG_HEARTBEAT);
				head.u2Version = m_pConn->m_u32MsgVersion;
				SendMsg(head);
			}

			continue;
		}

		reqDataTimes = 0;

		if (packBuf.IsRepos())
		{
			printf("dev(%s,%u) recordplay=%p Repos OK\n",
					m_pConn->GetIP(), m_pConn->GetPort(), this);

			memset(&m_LastRecvFrame, 0, sizeof(m_LastRecvFrame));

			{
				AutoLock lock(m_mtxReqData);
				m_LastKeyFrameTime = 0;
				m_bNeedData = TRUE;
				m_bNoData = FALSE;
				m_bReposing = FALSE;
			}
			m_evReposDone.Notify();
		}
		else if (packBuf.IsNoData())
		{
			//RecordPlayStatusFun(RSNetMsgRecordPlayNoData);

			if (m_stRecPlayParam.fRecordDataCallback)
			{
				if (!m_stRecPlayParam.fRecordDataCallback(m_stRecPlayParam.hWnd, NULL, 0, m_stRecPlayParam.pUser))
				{
					AutoLock lock(m_mtxReqData);
					MsgHeadWrap head(MSG_PLAY_FEEDBACK, MSG_PLAY_ENOUGH);
					head.u2Version = m_pConn->m_u32MsgVersion;
					SendMsg(head);
					m_bNeedData = FALSE;
				}
			}
#ifdef _WIN32
			if (!RecordDataCallback(m_stRecPlayParam.hWnd, NULL, 0, this))
			{
				AutoLock lock(m_mtxReqData);
				MsgHeadWrap head(MSG_PLAY_FEEDBACK, MSG_PLAY_ENOUGH);
				head.u2Version = m_pConn->m_u32MsgVersion;
				SendMsg(head);
				m_bNeedData = FALSE;
			}
#endif

			m_bNoData = TRUE;
			m_bNeedData = FALSE;
		}
		else if (packBuf.IsFileEnd())
		{
			if (m_stRecPlayParam.fRecordDataCallback)
			{
				UT_FRAME_HEAD_S stFramHead;
				memset(&stFramHead, 0, sizeof(UT_FRAME_HEAD_S));
				
				stFramHead.FrameTag  = FRAME_TAG_STD_H264;
				stFramHead.FrameType = FRAME_END;

				m_stRecPlayParam.fRecordDataCallback(0, &stFramHead, sizeof(UT_FRAME_HEAD_S), m_stRecPlayParam.pUser);
			}
		
			m_bFileDownloadEnd = TRUE;
			break;
		}

		if (frameBuf.FrameComplete(packBuf))
		{
			m_bNoData = FALSE;
			m_LastRecvFrame = *(UT_FRAME_HEAD_S*)frameBuf.GetFrame();
			if (m_LastRecvFrame.FrameType == FRAME_I)
				m_LastKeyFrameTime = m_LastRecvFrame.Time;

			if (m_stRecPlayParam.fRecordDataCallback)
			{
				if (!m_stRecPlayParam.fRecordDataCallback(m_stRecPlayParam.hWnd, frameBuf.GetFrame(), 
							frameBuf.GetFrameTotalSize(), m_stRecPlayParam.pUser))
				{
					AutoLock lock(m_mtxReqData);
					if (m_bNeedData)
					{
						MsgHeadWrap head(MSG_PLAY_FEEDBACK, MSG_PLAY_ENOUGH);
						head.u2Version = m_pConn->m_u32MsgVersion;
						SendMsg(head);
						m_bNeedData = FALSE;
					}
				}
			}
#ifdef _WIN32
			if (m_stRecPlayParam.hWnd 
				&& !RecordDataCallback(m_stRecPlayParam.hWnd, frameBuf.GetFrame(), 
				frameBuf.GetFrameTotalSize(), this))
			{
				AutoLock lock(m_mtxReqData);
				if (m_bNeedData)
				{
					MsgHeadWrap head(MSG_PLAY_FEEDBACK, MSG_PLAY_ENOUGH);
					head.u2Version = m_pConn->m_u32MsgVersion;
					SendMsg(head);
					m_bNeedData = FALSE;
				}
			}
#endif
		}
	} while (!m_bExit);

	AutoLock lock(m_mtxSock);
	m_sockRecPlay.Close();
}

VOID *RecordPlay::SendFun(VOID *pArg)
{
	RecordPlay *pRecordPlay = (RecordPlay *)pArg;
	pRecordPlay->Send();
	return 0;
}

VOID RecordPlay::Send()
{
	do 
	{
		m_semMsgSendQueue.Wait();

		S8 *msg = NULL;

		{
			AutoLock lock(m_mtxMsgSendQueue);
			if (!m_bCanSendMsg)
				break;

			if (!m_lsMsgSendQueue.empty())
			{
				msg = m_lsMsgSendQueue.front();
				m_lsMsgSendQueue.pop_front();
			}
		}

		if (msg)
		{
			MsgHeadWrap* head = (MsgHeadWrap*)msg;
			VOID *data = NULL;
			if (head->GetDataLen() > 0)
				data = msg + head->GetHeadExtLen();

			printf("dev(%s,%u,ch%d) recordplay=%p SendMsg type=%u, subtype=%u\n",
					m_pConn->GetIP(), m_pConn->GetPort(), this, head->GetMsgType(), head->GetMsgSubType());

			BOOL sendSuccess = ::SendMsg(m_sockRecPlay, *head, data);
			delete [] msg;

			if (!sendSuccess)
			{
				m_sockRecPlay.KillSocket();
				break;
			}
		}
	} while (!m_bExit);
}

BOOL RecordPlay::SendMsg(const MsgHeadWrap &head, const VOID *data)
{
	S8  *msg = new S8[head.GetMsgLen()];
	if (msg != NULL)
	{
		memcpy(msg, &head, head.GetHeadExtLen());
		if (data != NULL)
			memcpy(msg + head.GetHeadExtLen(), data, head.GetDataLen());

		AutoLock lock(m_mtxMsgSendQueue);

		if (!m_bCanSendMsg)
		{
			delete [] msg;
			return FALSE;
		}

		m_lsMsgSendQueue.push_back(msg);
		m_semMsgSendQueue.Post();
	}
	return TRUE;
}

DWORD RecordPlay::GetPlayedTime()
{

#if _WIN32

	DWORD dwCurTime    = time(NULL);
	/* 鼠标拖动进度条延迟处理 */
	if (dwCurTime - m_dwPreTagTime < 5)
	{
        return m_dwPlayedTime;
	}

	DWORD dwPlayTime = 0;
	
	if (FRAME_TYPE_HIK == m_u32FrameType)
	{
		if (-1 != m_lPort)
		{
			PLAYM4_SYSTEM_TIME hik_sys_time = {0};
			PlayM4_GetSystemTime(m_lPort, &hik_sys_time);

			struct tm timeinfo;
			timeinfo.tm_year         = hik_sys_time.dwYear  - 1900;
			timeinfo.tm_mon			 = hik_sys_time.dwMon   - 1;
			timeinfo.tm_mday		 = hik_sys_time.dwDay;
			timeinfo.tm_hour		 = hik_sys_time.dwHour  ;
			timeinfo.tm_min			 = hik_sys_time.dwMin;
			timeinfo.tm_sec			 = hik_sys_time.dwSec;

            time_t tmPlayTimeNow     = mktime(&timeinfo);

			if (tmPlayTimeNow < m_tmBeginSec)
			{
				return  m_dwPlayedTime;
			}

			m_dwPlayedTime = tmPlayTimeNow - m_tmBeginSec;
		}
	}
	else if (FRAME_TYPE_TIANDY == m_u32FrameType)
	{
		if (-1 != m_iPlayerID)
		{
            m_dwPlayedTime = (int)(PLAYSDK_INTERFACE::TC_GetPlayTime(m_iPlayerID) / 1000);
		}
	}
	else if (FRAME_TYPE_DAHUA == m_u32FrameType)
	{
		if (-1 != m_lPort)
		{			
			typedef struct {
                S32 nYear;
				S32 nMonth;
				S32 nDay;
				S32 nHour;
				S32 nMinute;
				S32 nSecond;
			}tTime;

			tTime tmSysTime = {0};
			S32 s32Len = 0;
			PLAY_QueryInfo(m_lPort, PLAY_CMD_GetTime, (char*)&tmSysTime, sizeof(tTime), &s32Len);

			struct tm timeinfo;
			timeinfo.tm_year		 = tmSysTime.nYear  - 1900;
			timeinfo.tm_mon			 = tmSysTime.nMonth - 1;
			timeinfo.tm_mday		 = tmSysTime.nDay;
			timeinfo.tm_hour		 = tmSysTime.nHour  ;
			timeinfo.tm_min			 = tmSysTime.nMinute;
			timeinfo.tm_sec		 	 = tmSysTime.nSecond;

            time_t tmPlayTimeNow     = mktime(&timeinfo);

			if (tmPlayTimeNow < m_tmBeginSec)
			{
				return  m_dwPlayedTime;
			}

			m_dwPlayedTime = tmPlayTimeNow - m_tmBeginSec;
		}
	}
	else if (FRAME_TYPE_STD_H264 == m_u32FrameType)
	{
		if (-1 != m_iPlayerID)
		{
			m_dwPlayedTime = (RSPlayGetCurPlayedTime(m_iPlayerID) - m_dwFirstUTFrameTime) / 1000;
		}
	}
	else if (FRAME_TYPE_HELMET_GX == m_u32FrameType)
	{
		if (NULL != m_hPlayerHandle)
		{
			U32 u32Timestamp = 0;
			fnVARender_GetLatestPlayVideoTimeStamp(m_hPlayerHandle, &u32Timestamp);
            if (u32Timestamp < m_tmHelmetBeginGx)
			{
				return  m_dwPlayedTime;
			}

			m_dwPlayedTime = (u32Timestamp - m_tmHelmetBeginGx)/1000;
		}
	}
	
	return m_dwPlayedTime;
#endif

}

BOOL RecordPlay::SetRecPlaybackStatusCb(pfnRecordPlaybackStatusCallback fnRecPbStatusCb, VOID *pUser)
{
	m_pfnRecordPbStatusCb = fnRecPbStatusCb;
	m_pUser               = pUser;

	return TRUE;
}

BOOL RecordPlay::OpenSound()
{
#ifdef _WIN32	
	if (FRAME_TYPE_HIK == m_u32FrameType)
	{
		if (-1 != m_lPort)
		{
			/* 海康鱼眼摄像机*/
			PlayM4_SetVolume(m_lPort, 0xFFFF);
			return PlayM4_PlaySound(m_lPort);
		}
	}
	else if (FRAME_TYPE_DAHUA == m_u32FrameType)
	{
		if (-1 != m_lPort)
		{
			return PLAY_PlaySound(m_lPort);
		}
	}
	else if (FRAME_TYPE_TIANDY == m_u32FrameType)
	{
		if (-1 != m_iPlayerID)
		{
            int iRet = PLAYSDK_INTERFACE::TC_PlayAudio(m_iPlayerID);
			return iRet > 0 ? TRUE : FALSE;
		}
	}
#endif	
	return FALSE;
}

BOOL RecordPlay::CloseSound()
{
#ifdef _WIN32
	if (FRAME_TYPE_HIK == m_u32FrameType)
	{
		if (-1 != m_lPort)
		{
			return PlayM4_StopSound();
		}
	}
	else if (FRAME_TYPE_DAHUA == m_u32FrameType)
	{
		if (-1 != m_lPort)
		{
			return PLAY_StopSound();
		}
	}
	else if (FRAME_TYPE_TIANDY == m_u32FrameType)
	{
		if (-1 != m_iPlayerID)
		{
            int iRet = PLAYSDK_INTERFACE::TC_StopAudio(m_iPlayerID);
			return iRet > 0 ? TRUE : FALSE;
		}
	}
#endif
	return FALSE;
}