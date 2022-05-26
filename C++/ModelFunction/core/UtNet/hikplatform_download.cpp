#include "hikplatform_download.h"
#include "commonc1.h"

HikPlatformDownload::HikPlatformDownload(HikPlatformConn * pConn)
    :BaseDownload()
{
	m_pConn        = pConn;
	m_pSaveFile    = NULL;
	m_trdDownload  = NULL;
	m_bExit        = FALSE;
	m_bDownloadExp = FALSE;
	m_bDownloadOk  = FALSE;
	m_u64FileSize  = 0;
	m_u64FileLastlen = 0;
	m_u32Pos       = 0;
	m_downloadId   = -1;
	m_downType     = DOWNLOAD_BYFILE;
}

HikPlatformDownload::~HikPlatformDownload()
{

}

void CALLBACK DownloadMsgCallBack(VIDEO_INT64 i64PlayHandle, int iMsg, void* pUserData)
{
	HikPlatformDownload * pThis = (HikPlatformDownload *)pUserData;
	
	//12:取流异常
	if (12 == iMsg && pThis)
	{
		pThis->m_bDownloadExp = TRUE;
	}
	else if (11 == iMsg && pThis)
	{
		pThis->m_bDownloadOk = TRUE;
		pThis->m_u32Pos = 100;
	}
}

void CALLBACK DownloadDecodedDataCallback(VIDEO_INT64 i64PlayHandle, const char* pDataArray, int iDataLen, int iWidth, int iHeight, 
										int iFrameType, int iTimeStamp, void* pUserData)
{
	
}

void CALLBACK DownloadStreamCallBack(VIDEO_INT64 i64PlayHandle, int iStreamDataType, const char* pDataArray, int iDataLen, void* pUserData)
{
	HikPlatformDownload *pThis = (HikPlatformDownload *)pUserData;
	if (pThis)
	{
		if (NULL != pThis->m_pSaveFile)
		{
			if (2 != iStreamDataType &&
				3 != iStreamDataType)
			{
				if (HikPlatformDownload::DOWNLOAD_BYFILE == pThis->m_downType)
				{
					pThis->m_u64FileLastlen += iDataLen;
					if (0 != pThis->m_u64FileSize)
					{
						pThis->m_u32Pos = (100 * pThis->m_u64FileLastlen) / pThis->m_u64FileSize;
					}
				}
							
				fwrite(pDataArray, iDataLen, 1, pThis->m_pSaveFile);
				fflush(pThis->m_pSaveFile);
			}
			else
			{
				pThis->m_u32Pos = 100;
				fclose(pThis->m_pSaveFile);
				pThis->m_pSaveFile = NULL;
				pThis->m_bDownloadOk = TRUE;
			}
		}
	}
}

BOOL HikPlatformDownload::Start(const NET_RECDOWNLOAD_PARAM_S *pParam)
{
	m_strUrl          = pParam->ps8SubServIp;                        //播放URL
	m_stBeginTime     = *(DATETIME_S *)pParam->ps8FrontAddrCode;     //开始时间
	m_stEndTime       = *(DATETIME_S *)pParam->ps8RecFileName;       //结束时间
	m_fStatusCallback = pParam->fStatusCallback;
	m_pUser           = pParam->pUser;
	m_u64FileSize     = pParam->u32FileSize;

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

VOID HikPlatformDownload::Stop()
{
	if (m_bExit)
	{
		return;
	}

	m_bDownloadExp = TRUE;
	m_bDownloadOk  = TRUE;
	m_bExit = TRUE;
	ThreadJoin(m_trdDownload);
}

U32  HikPlatformDownload::GetDownloadPos()
{
	if (m_u32Pos > 100)
		m_u32Pos = 100;
	
	return m_u32Pos;
}

VOID *HikPlatformDownload::RecDownloadLoop(VOID *pArg)
{
	HikPlatformDownload *pDown = (HikPlatformDownload *)pArg;

	pDown->RecDownload();
	
	return 0;
}

VOID HikPlatformDownload::RecDownload()
{
	do 
	{
       BOOL bOpenStream = OpenStream();

       if (bOpenStream)
	   {
           while (!m_bDownloadExp &&
			      !m_bDownloadOk)
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

	CloseStream();

	if (m_bDownloadExp)
	{
		HIKPLAT_DOWNLOAD_STATUS(MSG_DOWNLOAD_FAIL);
	}
	else if (m_bDownloadOk)
	{
		HIKPLAT_DOWNLOAD_STATUS(MSG_DOWNLOAD_OK);
		m_u32Pos = 100;
	}
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, message, wParam, lParam);
}

HWND HikPlatformDownload::CreateVirtualHwnd()
{
	HWND hWnd = NULL;
    WNDCLASS wc;
	char *pszClass      = "HikPlatformDownload";
	wc.cbClsExtra       = 0;
	wc.cbWndExtra       = 0;
	wc.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.hCursor          = LoadCursor(NULL, IDC_ARROW);
	wc.hIcon            = LoadIcon(NULL,IDI_WINLOGO);
	wc.lpfnWndProc      = WndProc;
	wc.lpszClassName    = pszClass;
	wc.hInstance        = NULL;
	wc.lpszMenuName     = NULL;
	wc.style			= CS_HREDRAW | CS_VREDRAW;
    
    RegisterClass(&wc);
	
	hWnd = CreateWindow(pszClass, pszClass, 
		WS_OVERLAPPEDWINDOW, 
		0, 0, 600, 400, NULL,
		NULL, NULL, NULL);
    
	
	return hWnd;
}

BOOL HikPlatformDownload::OpenStream()
{
	if (-1 != m_downloadId)
	{
        Video_StopPlayback(m_downloadId);
		m_downloadId = -1;
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
	recReq.fnMsg     = DownloadMsgCallBack;
	recReq.fnStream  = DownloadStreamCallBack;
	recReq.pUserData = this;
	
	HWND hWnd = CreateVirtualHwnd();

    m_downloadId = Video_StartPlayback(m_strUrl.c_str(), hWnd, llStartTime, llEndTime, &recReq);
	if (m_downloadId < 0)
	{
		return FALSE;
	}

	Video_PlayCtrl(m_downloadId, 3, 16);

	return TRUE;
}

VOID HikPlatformDownload::CloseStream()
{
   if (-1 != m_downloadId)
   {
	   Video_StopPlayback(m_downloadId);
	   m_downloadId = -1;
   }
   if (NULL != m_pSaveFile)
   {
	   fclose(m_pSaveFile);
	   m_pSaveFile = NULL;
   }
}