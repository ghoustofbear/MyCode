/************************************************************************/
/* 20191121 wuqingxiong
增加C2平台接入                                                                     */
/************************************************************************/
#include "ut_net.h"
#include "connection.h"
#include "playbackc1.h"
#include "previewc1.h"
#include "downloadc1.h"
#include "download_bytime.h"
#include "pc_talk.h"
#include "dualtalk.h"
#include "dualtalkEx.h"

#include "hikplatform_conn.h"
#include "hikplatform_preview.h"
#include "hikplatform_record.h"
#include "hikplatform_download.h"
#include "hikplatform_download_bytime.h"

#include "c2platform_conn.h"  
#include "c2platform_preview.h"
#include "c2platform_record.h"
#include "c2platform_download.h"
#include "c2Platform_Dualtalk.h"
#include "c2Platform_DualtalkEx.h"
#include "c2Platform_DownloadByTime.h"

static BOOL g_bInit = FALSE;
static FILE * g_pSdkLogFile = NULL;

#pragma warning(disable : 4786)

#ifdef _WIN32
//int noHIKPLATFORM_JIERU = 1;
#define RUN_DATA   ("./RunData.ini")
#endif

/* 初始化 */
NET_API BOOL NET_Init()
{
    if (g_bInit)
    {
        return TRUE;
    }

#ifdef _WIN32

    extern VOID LoadTiandyPlayDll();

	extern void loadhelmetgxlibrary();

	extern void loadHikplatform();

	extern long loadC2platform(const char* jvm2nativePath); //20200403 wuqingxiong add 增加加载C2平台

#include <winver.h>
#pragma comment(lib, "version.lib")

#ifdef NET_LOGFILE
    gNET_LogFile = fopen("c:\\Debug\\NET_.log", "a");
    handleFile = CreateFile("C:\\Debug\\NET_.txt", GENERIC_READ | GENERIC_WRITE,
                            FILE_SHARE_READ | FILE_SHARE_WRITE, NULL, OPEN_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
#endif
#ifdef _DEBUG
    AllocConsole();
    freopen("CONOUT$", "w", stdout);
#endif

    LoadTiandyPlayDll();
	loadhelmetgxlibrary();
	loadHikplatform();

	loadC2platform(NULL);//20200403 wuqingxiong add 增加加载C2平台

#else

#endif

    if (!SocketInit())
    {
        return FALSE;
    }
    g_bInit = TRUE;

    return TRUE;
}

NET_API VOID NET_Release()
{
    if (!g_bInit)
    {
        return;
    }
#ifdef _WIN32
#ifdef NET_LOGFILE
    if (gNET_LogFile != NULL)
    {
        fclose(gNET_LogFile);
        CloseHandle(handleFile);
    }
#endif
    extern VOID FreeTiandyPlayDll();

    FreeTiandyPlayDll();

	extern void destroyHikplatform();

    //destroyHikplatform();

	extern void destroyC2platform();//20191122 wuqingxiong add 释放资源
	
	destroyC2platform();//20191122 wuqingxiong add 释放资源
#else

#endif

    SocketTerm();

    if (g_pSdkLogFile)
    {
        fclose(g_pSdkLogFile);
        g_pSdkLogFile = NULL;
    }

    g_bInit = FALSE;
}

#ifndef _WIN32
UINT g_nLastErr = 0;
VOID SetLastError(UINT err)
{
    g_nLastErr = err;
}

UINT GetLastError()
{
    return g_nLastErr;
}
#endif

NET_API UINT NET_GetLastError()
{
    return GetLastError();
}

NET_API HANDLE NET_ConnStart(const NET_CONN_PARAM_S *pParam, U32 u32Proto)
{
    if (pParam != NULL)
    {
		if  (0 == u32Proto)
		{
			BasePlatformConn *pConn = new Connection;
			if (pConn)
			{
				if (pConn->Start(pParam))
				{
					return pConn;
				}
				
				delete pConn;
			}
		}
		else if  (1 == u32Proto)
		{
			BasePlatformConn * pConn = new HikPlatformConn;
			if (pConn)
			{
				if (pConn->Start(pParam))
				{
					return pConn;
				}
				
				delete pConn;
			}
		}
		
		else if (2 == u32Proto) //20200402 wuqingxiong add C2平台启动登录
		{
			C2PlatformConn* pConn = new C2PlatformConn;
			if(NULL != pConn)
			{
				if(pConn->Start(pParam))
				{
					return pConn;
				}
				delete pConn;
			}
		}
	}

    return NULL;
}

NET_API BOOL NET_AddStandBy(HANDLE hConn, const char *ip, int port)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn)
	{
		return pConn->AddStandby(ip, port);
	}
  
	return TRUE;
}


NET_API BOOL NET_DelStandBy(HANDLE hConn, const char *ip, int port)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn)
	{
		return pConn->DelStandby(ip, port);
	}
	return TRUE;
}

NET_API VOID NET_ConnStop(HANDLE *hConn)
{
    if (hConn != NULL && *hConn != NULL)
    {
			BasePlatformConn *pConn = (BasePlatformConn *)(*hConn);
			pConn->Stop();
			
			delete pConn;
			*hConn = NULL;
    }
}

NET_API BOOL NET_GetLoginRsp(HANDLE hConn, LOGIN_RSP_S *pRsp)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn)
	{
		return pConn->GetLoginRsp(pRsp);
	}

	return TRUE;
}

NET_API BOOL NET_GetUserList(HANDLE hConn, const NET_USER_PARAM_S *pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		return pConn->GetUserList(pParam);
	}

	return TRUE;
}

NET_API BOOL NET_GetDeviceList(HANDLE hConn, const NET_DEVICE_PARAM_S *pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		return pConn->GetDeviceList(pParam);
	}

	return TRUE;
}

NET_API HANDLE NET_StartPreview(HANDLE hConn, const NET_PVIEW_PARAM_S *pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		return pConn->StartPreview(pParam);
	}
    return NULL;
}

NET_API VOID NET_StopPreview(HANDLE  hConn, HANDLE *hPView)
{
    BasePlatformConn *pConn = (BasePlatformConn *)hConn;
    if (pConn && hPView && *hPView)
    {
	    pConn->StopPreview(hPView);
        *hPView = NULL;
    }
}

NET_API BOOL NET_RequestIFrame(HANDLE  hConn, const NET_REQI_PARAM_S *pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		return pConn->RequestIFrame(pParam);
	}

	return TRUE;

}

NET_API BOOL NET_PreviewSwitch(HANDLE hConn, const NET_SWITCH_PARAM_S* pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		return pConn->PreviewSwitch(pParam);
	}
	return TRUE;
}

/**
 * 非线程安全，调用者要对传入的hPView进行保护
 */
NET_API BOOL NET_GetPreviewRsp(HANDLE hPView, PVIEW_RSP_S *pRsp)
{
	if (hPView)
	{
		PreviewClient *pPViewClient = (PreviewClient*)hPView;
		BasePView     *pPView       = pPViewClient->pPView;
		
		return pPView->GetResponse(pRsp);
	}

	return TRUE;
}

NET_API BOOL NET_SaveRealData(HANDLE hPView, S8 *ps8FileName)
{
	if (hPView)
	{
		PreviewClient *pPViewClient = (PreviewClient*)hPView;
		BasePView     *pPView       = pPViewClient->pPView;
		
		return pPView->SaveRealData(ps8FileName);
	}

	return TRUE;
}

NET_API BOOL NET_StopSaveRealData(HANDLE hPView)
{
	if (hPView)
	{
		PreviewClient *pPViewClient = (PreviewClient*)hPView;
		BasePView     *pPView       = pPViewClient->pPView;
		
		return pPView->StopSaveRealData();
	}

	return TRUE;

}


NET_API BOOL NET_CapturePicture(HANDLE hPView, S8 *ps8PicFileName, U32 u32CaptueMode)
{
	if (hPView)
	{
		PreviewClient *pPViewClient = (PreviewClient*)hPView;
		BasePView     *pPView       = pPViewClient->pPView;
		
		return pPView->CapturePicture(ps8PicFileName, u32CaptueMode);
	}
	
    return FALSE;
}


NET_API BOOL NET_PTZControl(HANDLE hConn, U32 u32Cmd, const NET_PTZ_PARAM_S *pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	
	if (pConn && pParam)
	{
		return pConn->PTZControl(u32Cmd, pParam);
	}
	
    return FALSE;
}

NET_API BOOL NET_PTZControlEx(HANDLE hConn, U32 u32Cmd, const NET_PTZ_PARAM_S *pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		return pConn->PTZControlEx(u32Cmd, pParam);
	}

    return FALSE;
}

NET_API BOOL NET_QueryRecord(HANDLE hConn, const NET_QRECORD_PARAM_S *pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		return pConn->QueryRecord(pParam);
	}

    return FALSE;
}

NET_API BOOL NET_QueryRecordEx(HANDLE hConn, const NET_QRECORD_PARAM_EX_S *pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		return pConn->QueryRecordEx(pParam);
	}

    return FALSE;
}

NET_API HANDLE NET_StartRecordPlay(HANDLE hConn, const NET_RECPLAY_PARAM_S *pParam)
{
    if (hConn && pParam)
    {
        return ((BasePlatformConn*)hConn)->StartRecordPaly(pParam);
	}

    return NULL;
}

NET_API VOID NET_StopRecordPlay(HANDLE *hPlayback)
{
    if (hPlayback && *hPlayback)
    {
		BaseRecord *pRecordPlay = (BaseRecord*)*hPlayback;
		pRecordPlay->Stop();
		delete pRecordPlay;
		*hPlayback = NULL;
    }
}

NET_API BOOL NET_RecordPlayCtrl(HANDLE hPlayback, U32 u32Code, VOID *pInVal, U32 u32InSize, VOID *pOutVal, U32 *u32OutSize)
{
    if (hPlayback)
    {
		BaseRecord *pRecordPlay = (BaseRecord *)hPlayback;
		return pRecordPlay->PlayCtrl(u32Code, pInVal, u32InSize, pOutVal, u32OutSize);
    }
	
    return FALSE;
}


NET_API HANDLE NET_StartRecordDownload(HANDLE hConn,  const NET_RECDOWNLOAD_PARAM_S *pParam)
{
    if (hConn && pParam)
    {
        return ((BasePlatformConn*)hConn)->StartRecordDownload(pParam);
	}
	
	return NULL;
}

NET_API VOID NET_StopRecordDownload(HANDLE *hDownload)
{
    if (hDownload && *hDownload)
    {
		BaseDownload *pDownload = (BaseDownload *)*hDownload;
		pDownload->Stop();
		delete pDownload;
		*hDownload = NULL;
	}
}

NET_API S32 NET_GetDownloadPos(HANDLE hDownload)
{
    if (hDownload)
    {
		BaseDownload *pDownload = (BaseDownload *)hDownload;
		if (pDownload)
		{
			return pDownload->GetDownloadPos();
		}
	}
	
    return -1;
}

NET_API HANDLE NET_StartRecordDownloadByTime(HANDLE hConn,  const NET_RECDOWNLOAD_BYTIME_PARAM_S *pParam)
{
    if (hConn && pParam)
    {
        return ((BasePlatformConn*)hConn)->StartRecordDownloadByTime(pParam);
	}
	
	return NULL;
}


NET_API VOID NET_StopRecordDownloadByTime(HANDLE *hDownload)
{
    if (hDownload && *hDownload)
    {
		BaseDownloadByTime *pDownload = (BaseDownloadByTime *)*hDownload;
		
		pDownload->Stop();
		delete pDownload;
		*hDownload = NULL;
	}
}

NET_API BOOL NET_MatrixSwitch(HANDLE hConn, const NET_MATRIX_PARAM_S *pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		return pConn->MatrixSwitch(pParam);
	}
	
    return TRUE;
}

NET_API BOOL NET_DecoderSwitch(HANDLE hConn, const NET_DECODER_PARAM_S * pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		return pConn->DecoderSwitch(pParam);
	}
	
    return TRUE;
}

typedef struct
{
    BasePlatformConn * hConn;

    S32 s32DisOutPort;               /**< 显示输出通道 */
    U8  u8DeviceIp[MAX_IPV4_LEN];    /**< 设备IP地址 */
    U16 u16DevicePort;               /**< 设备端口号 */
    U8  u8SubServIp[MAX_IPV4_LEN];   /**< 二级流媒体服务器地址 */
    U16 u16SubServPort;              /**< 二级流媒体端口号 */
} DecDisplay;

NET_API HANDLE NET_DecoderGetDisplay(HANDLE hConn, const NET_DEC_DIS_PARAM_S *pParam)
{
	DecDisplay *pDisplay = new DecDisplay;
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		pDisplay->hConn = pConn;
		pDisplay->u16DevicePort  = pParam->u16DevicePort;
		pDisplay->u16SubServPort = pParam->u16SubServPort;
		pDisplay->s32DisOutPort  = pParam->u32OutPort;
		INET_PTON(pParam->ps8DeviceIp,  pDisplay->u8DeviceIp);
		INET_PTON(pParam->ps8SubServIp, pDisplay->u8SubServIp);
		
		if (!pConn->DecoderGetDisplay(pParam))
		{
			delete pDisplay;
			
			return NULL;
		}
	}
	return pDisplay;
}

NET_API VOID NET_DecoderReleaseDisplay(HANDLE *hDisplay)
{
	if (hDisplay && *hDisplay)
	{
		DecDisplay *pDisPlay = (DecDisplay *)(*hDisplay);
		
		delete pDisPlay;
		
		*hDisplay = NULL;
	}
}

NET_API BOOL NET_DecoderStartDynamic(HANDLE hDisplay, S32 s32DecChnNo, S32 s32DisWndNo, const NET_DEC_CHN_PARAM_S *pParam)
{
	if (hDisplay)
	{
		DecDisplay *pDisPlay = (DecDisplay *)hDisplay;
		
		return pDisPlay->hConn->DecoderStartDynamic(
			pDisPlay->u8DeviceIp,
			pDisPlay->u16DevicePort,
			pDisPlay->u8SubServIp,
			pDisPlay->u16SubServPort,
			s32DecChnNo,
			pDisPlay->s32DisOutPort,
			s32DisWndNo,
			pParam);
	}	
	
    return FALSE;
}

NET_API BOOL NET_DecoderStopDynamic(HANDLE hConn, const NET_DEC_DIS_PARAM_S *pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		U8  u8DeviceIp[MAX_IPV4_LEN]  = {0};
		U8  u8SubServIp[MAX_IPV4_LEN] = {0};
		INET_PTON(pParam->ps8DeviceIp,  u8DeviceIp);
		INET_PTON(pParam->ps8SubServIp, u8SubServIp);
		
		return pConn->DecoderStopDynamic(
			u8DeviceIp,
			pParam->u16DevicePort,
			u8SubServIp,
			pParam->u16SubServPort,
			pParam->u32OutPort
			);
	}
		
    return FALSE;
}

NET_API BOOL NET_SetWFLinkageCallback(HANDLE hConn, pfnWFLinkageCallback fWFLinkageCallback, const VOID *pUser)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn)
	{
		return pConn->SetWFLingageCallback(fWFLinkageCallback, pUser);
	}
		
    return FALSE;
}

/**
 * @brief  获取在线客户信息
 *
 * @param  [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
 *
 * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
 *
 * @see
 */
NET_API BOOL NET_GetClientList(HANDLE hConn, pfnGetClientCallback fGetClientCallback, VOID *pUser)
{
	BasePlatformConn* pConn = (BasePlatformConn*)hConn;
	if (pConn)
	{
		return pConn->GetClientList(fGetClientCallback, pUser);
	}

    return FALSE;
}

/**
 * @brief  初始化PC对讲服务
 *
 * @param [in] pParam  对讲服务参数
 *
 * @return 成功返回连接句柄(指针), 否则返回NULL
 */
NET_API HANDLE NET_InitPcTalkServer(NET_PCTALK_PARAM_S * pParam)
{
	if (pParam != NULL)
	{
		PcTalk *pTalk = new PcTalk;
		if (pTalk)
		{
			if (pTalk->IniPcTalk(PCTALK_SERVER, pParam))
			{
				return pTalk;
			}
			
			delete pTalk;
		}
	}	
	
    return NULL;
}

/**
 * @brief  初始化PC对讲客户端
 *
 * @param [in] pParam  对讲服务参数
 *
 * @return 成功返回连接句柄(指针), 否则返回NULL
 */
NET_API VOID NET_ReleasePcTalk(HANDLE hPcTalk)
{
	if (hPcTalk != NULL)
	{
		PcTalk *pTalk = (PcTalk *)hPcTalk;
		if (pTalk)
		{
			delete pTalk;
			pTalk = NULL;
		}
	}	
}

/**
 * @brief  初始化PC对讲客户端
 *
 * @param [in] pParam  对讲服务参数
 *
 * @return 成功返回连接句柄(指针), 否则返回NULL
 */
NET_API HANDLE NET_InitPcTalkClient(NET_PCTALK_PARAM_S * pParam)
{
	if (pParam != NULL)
	{
		PcTalk *pTalk = new PcTalk;
		if (pTalk)
		{
			if (pTalk->IniPcTalk(PCTALK_CLIENT, pParam))
			{
				return pTalk;
			}
			
			delete pTalk;
		}
	}
	
    return NULL;
}



/**
 * @brief  发送对讲消息
 *
 * @param [in] hConn    已经建立的连接句柄，NET_InitPcTalkServer或NET_InitPcTalkClient的返回值
 * @param [in] u8Type	消息类型：1-申请对讲，2-ACK, 3-NAK, 4-音频数据，5-结束对讲
 * @param [in] pData	数据
 * @param [in] u32Size  数据长度
 *
 * @return VOID
 */
NET_API VOID NET_SendPcTalkMsg(HANDLE hPcTalk, U8 u8Type, VOID *pData, U32 u32Size)
{
	PcTalk* pTalk = (PcTalk*)hPcTalk;
	if (pTalk)
	{
		pTalk->SendPcTalkMsg(u8Type, (U8*)pData, u32Size);
	}
}

NET_API BOOL NET_SynchrTime(HANDLE hConn, const NET_SYNTIME_PARAM_S *pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		return pConn->SynchrTime(pParam);
	}

    return FALSE;
}

NET_API BOOL NET_QueryTime(HANDLE hConn, DATETIME_S *pDateTime)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pDateTime)
	{
		return pConn->QueryTime(pDateTime);
	}	
	
    return FALSE;
}

NET_API BOOL NET_OpenSound(HANDLE hPView)
{
	if (hPView)
	{
		PreviewClient *pPViewClient = (PreviewClient*)hPView;
		BasePView    *pPView          = pPViewClient->pPView;
		
		return pPView->OpenSound();
	}
	
    return FALSE;
}

NET_API BOOL NET_CloseSound(HANDLE hPView)
{
	if (hPView)
	{
		PreviewClient *pPViewClient = (PreviewClient*)hPView;
		BasePView    *pPView          = pPViewClient->pPView;
		
		return pPView->CloseSound();
	}
	
    return FALSE;
}

NET_API BOOL NET_GetDeviceState(HANDLE hConn, U8 u8Ip[], U16 u16Port)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn)
	{
		return pConn->GetDeviceState(u8Ip, u16Port);
	}
	
    return FALSE;
}

NET_API BOOL NET_QueryResource(HANDLE hConn, pfnResourceCallback fResourceCallback, VOID *pUser)
{
	BasePlatformConn* pConn = (BasePlatformConn*)hConn;
	if (pConn)
	{
		return pConn->QueryResource(fResourceCallback, pUser);
	}
	
    return FALSE;
}

NET_API HANDLE NET_InitDualtalk(HANDLE hConn, const NET_DTALK_PARAM_S *pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		return pConn->InitDualtalk(pParam);
	}
	
    return NULL;
}

NET_API VOID NET_UnInitDualtalk(HANDLE hConn, HANDLE *hDTalk)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && hDTalk && *hDTalk)
	{
		pConn->UnInitDualtalk(*hDTalk);
		*hDTalk = NULL;
	}
}

NET_API BOOL NET_SendDTalkMsg(HANDLE hDTalk, const NET_DTALK_MSG_S *pParam, const NET_DTALK_USER_S *pUserList)
{
	if (hDTalk)
	{
		BaseDualTalk *pDTalk = (BaseDualTalk *)hDTalk;
		
		return pDTalk->SendDTalkMsg(pParam, pUserList);
	}
		
    return FALSE;
}

NET_API BOOL NET_SendDTalkData(HANDLE hDTalk, U8 *pu8Data, U32 u32Size)
{
	if (hDTalk)
	{
		BaseDualTalk *pDTalk = (BaseDualTalk *)hDTalk;
		
		return pDTalk->SendDTalkData(pu8Data, u32Size);
	}
	
    return FALSE;
}

NET_API BOOL NET_StartCapture(HANDLE hDTalk)
{
	if (hDTalk)
	{
		BaseDualTalk *pDTalk = (BaseDualTalk *)hDTalk;
		
		return pDTalk->StartCapture();
	}
	
    return FALSE;
}

NET_API BOOL NET_LinkTransmit(HANDLE hConn, LINKAGE_TRANS_RPT_S* pLinkTrans)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn)
	{
		return pConn->LinkTransmit(pLinkTrans);
	}
	
    return FALSE;
}

NET_API CHN_INFO_S *NET_GetDeviceData(HANDLE hConn, U32 *u32DevNum)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn)
	{
		return pConn->GetDeviceData(u32DevNum);
	}

    return NULL;
}

NET_API BOOL NET_RailCarControl(HANDLE hConn, U32 u32Cmd, const NET_PTZ_PARAM_S *pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		return pConn->RailCarControl(u32Cmd, pParam);
	}
	
    return FALSE;
}

NET_API BOOL NET_ControlRecord(HANDLE hConn, const NET_CHANNEL_PARAM_S *pParam, BOOL bStart)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		return pConn->ControlRecord(pParam, bStart);
	}
	
    return FALSE;
}

NET_API BOOL NET_SetParameter(HANDLE hConn, const NET_PARAMSET_PARAM_S *pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		return pConn->SetParameter(pParam);
	}
	
    return FALSE;
}

NET_API BOOL NET_SetParameterEx(HANDLE hConn, const S8 * pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		return pConn->SetParameterEx(pParam);
	}
	
    return FALSE;
}

NET_API BOOL NET_SetDTalkState(HANDLE hConn, U32 u32DTalkState)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn)
	{
		return pConn->SetDTalkState(u32DTalkState);
	}
	
    return FALSE;
}

NET_API BOOL NET_GetChnPreset(HANDLE hConn, const NET_GETPRESET_PARAM_S *pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		return pConn->GetChnPreset(pParam);
	}
	
    return FALSE;
}

NET_API BOOL NET_SetChnPreset(HANDLE hConn, const NET_SETPRESET_PARAM_S *pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		return pConn->SetChnPreset(pParam);
	}
	
    return FALSE;
}

NET_API BOOL NET_GetChnPresetEx(HANDLE hConn, const NET_GETPRESET_PARAM_S_EX *pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		return pConn->GetChnPresetEx(pParam);
	}
	
    return FALSE;
}

NET_API BOOL NET_SetChnPresetEx(HANDLE hConn, const NET_SETPRESET_PARAM_S_EX *pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		return pConn->SetChnPresetEx(pParam);
	}
	
    return FALSE;
}

NET_API BOOL NET_SetLogToFile()
{
    if (g_pSdkLogFile == NULL)
    {
        g_pSdkLogFile = fopen("PrviateSdk.log", "wb");
    }

    if (g_pSdkLogFile == NULL)
    {
        return FALSE;
    }

    return TRUE;
}

#ifdef _WIN32
NET_API VOID NET_DrawPreset(HANDLE hConn, const NET_PRESET_DRAW * pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam &&
        pParam->hPView && *(pParam->hPView))
	{
		pConn->DrawPreset(pParam);
	}
}

NET_API BOOL NET_RegisterDraw(HANDLE hConn, const NET_REGDRAW_PARAM * pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam &&
        (pParam->hPView) && *(pParam->hPView))
	{
		return pConn->SetRegDraw(pParam);
	}
	
    return FALSE;
}

NET_API BOOL NET_SetDecDataCallback(HANDLE hConn, const NET_DECFRAME_PARAM * pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam &&
        pParam->hPView)
	{
		return pConn->SetDecDataCallBack(pParam);
	}
	
    return FALSE;
}
#endif

#include <stdarg.h>
VOID WriteLog(const S8 * fmt...)
{
    if (g_pSdkLogFile == NULL)
    {
        return;
    }

    S8 s8Buf[512]  = {0};
    S8 s8Info[128] = {0};

    va_list vp;
    va_start(vp, fmt);
    vsprintf(s8Info, fmt, vp);
    va_end(vp);

    fseek(g_pSdkLogFile, 0, SEEK_END);
    long lFileSize = ftell(g_pSdkLogFile);

    /**< 日志文件大小默认为100M,超过则清除 */
    if (lFileSize / 1024 / 1024 / 8 >= 100)
    {
        /**< 日志文件清空 */
        fseek(g_pSdkLogFile, 0, SEEK_SET);
    }

    DATETIME_S time;
    GetDateTime(&time);

    snprintf(s8Buf, 512, "[%04d-%02d-%02d %02d:%02d:%02d] %s \n",
             time.u32Year, time.u32Month, time.u32Day,
             time.u32Hour, time.u32Minute, time.u32Second,
             s8Info);
    fputs(s8Buf, g_pSdkLogFile);
    fflush(g_pSdkLogFile);

}

NET_API DWORD NET_GetSDKVersion()
{
#ifdef _WIN32
    VS_FIXEDFILEINFO *pVerInfo = NULL;
    DWORD dwTemp, dwSize, dwHandle = 0;
    BYTE *pData = NULL;
    UINT uLen;

    S8 s8FileName[MAX_NAME_LEN] = {0};

#ifdef _DEBUG
    sprintf(s8FileName, "utnetd.dll");
#else
    sprintf(s8FileName, "utnet.dll");
#endif

    dwSize = GetFileVersionInfoSize((LPTSTR) s8FileName, &dwTemp);
    if (dwSize == 0)
    {
        return -1;
    };

    pData = new BYTE[dwSize];
    if (pData == NULL)
    {
        return -1;
    };

    if (!GetFileVersionInfo((LPTSTR) s8FileName, dwHandle, dwSize, pData))
    {
        delete[] pData;
        return -1;
    }

    if (!VerQueryValue(pData, ("\\"), (void **) &pVerInfo, &uLen))
    {
        delete[] pData;
        return -1;
    }

    DWORD verMS = pVerInfo->dwFileVersionMS;
    DWORD verLS = pVerInfo->dwFileVersionLS;

    int ver[4] = {0};
    ver[0] = HIWORD(verMS);
    ver[1] = LOWORD(verMS);
    ver[2] = HIWORD(verLS);
    ver[3] = LOWORD(verLS);

    delete[] pData;

    DWORD dwVersion = 0;
    dwVersion = (ver[0] << 24) | (ver[1] << 16) | (ver[2] << 8) | (ver[3]);
    return dwVersion;
#endif
}

NET_API DWORD NET_GetPlayedTime(HANDLE hPlayback)
{
    if (hPlayback)
    {
		BaseRecord *pRecPlay = (BaseRecord *)hPlayback;
		return pRecPlay->GetPlayedTime();
    }
	
    return 0;
}

NET_API BOOL NET_SetRecordPlaybackStatusCallback(HANDLE hPlayback, pfnRecordPlaybackStatusCallback fnRecordPbStatusCb, VOID *pUser)
{
    if (hPlayback)
    {
		BaseRecord *pRecPlay = (BaseRecord *)hPlayback;
		return pRecPlay->SetRecPlaybackStatusCb(fnRecordPbStatusCb, pUser);
	}
	
    return FALSE;
}

NET_API BOOL NET_StartIvaAnalyse(HANDLE hConn, const NET_IVA_ANALYSE_PARAM *pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		return pConn->StartIvaAnalyse(pParam);
	}
	
    return FALSE;
}

NET_API BOOL NET_StopIvaAnalyse(HANDLE hConn, const NET_IVA_ANALYSE_PARAM *pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		return pConn->StopIvaAnalyse(pParam);
	}
	
    return FALSE;
}

NET_API HANDLE NET_InitDevTalk(HANDLE hConn,  int audioType)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn == NULL)
	{
        return NULL;
	}
		
	return pConn->InitDevTalk(audioType);
}

NET_API VOID NET_UninitDevTalk(HANDLE *hDevTalk)
{
	if (NULL != hDevTalk &&
        NULL != *hDevTalk)
	{
		BaseDualTalkEx * pDualTalkEx = (BaseDualTalkEx *)(*hDevTalk);
		if (NULL != pDualTalkEx)
		{
			delete pDualTalkEx;
		}
	}
}

NET_API BOOL NET_StartDevTalk(HANDLE hDevTalk, const NET_DTALK_DEV_LIST *pParam)
{
	BaseDualTalkEx * pDualTalkEx = (BaseDualTalkEx *)hDevTalk;
	if (pDualTalkEx && pParam)
	{
		return pDualTalkEx->SendDTalkReq(pParam, 0);
	}
	
    return FALSE;
}

NET_API VOID NET_StopDevTalk(HANDLE hDevTalk, const NET_DTALK_DEV_LIST *pParam)
{
	BaseDualTalkEx * pDualTalkEx = (BaseDualTalkEx *)hDevTalk;
	if (pDualTalkEx && pParam)
	{
		pDualTalkEx->SendDTalkReq(pParam, 1);
	}
}

NET_API VOID NET_SendDataToDevTalk(HANDLE *hDevTalk, U8 * ps8Data, U16 u16Len)
{
	BaseDualTalkEx * pDualTalkEx = (BaseDualTalkEx *)hDevTalk;
	if (pDualTalkEx)
	{
		pDualTalkEx->SendDTalkData(ps8Data, u16Len);
	}
}

NET_API BOOL NET_InitMuxAVFDevTalk(HANDLE hDevTalk, _NET_DEV_MUXAVF_PARAM_ *pParam)
{
	BaseDualTalkEx * pDualTalkEx = (BaseDualTalkEx *)hDevTalk;
	if (pDualTalkEx)
	{
		return pDualTalkEx->InitMuxAVF(pParam);
	}
    return FALSE;
}

NET_API BOOL NET_SetThermometryRuleCfg(HANDLE hConn, const NET_THERM_RULE_LIST* pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		return pConn->SetThermometryRuleCfg(pParam);
	}
	
    return FALSE;
}

NET_API BOOL NET_SetThermDisplayMode(HANDLE hConn, const NET_TERM_SHOWPARAM_S * pParam)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn && pParam)
	{
		return pConn->SetThermDisplayMode(pParam);
	}
	
    return FALSE;
}

NET_API BOOL NET_GetPresetInfo(HANDLE hConn, const S8 * ps8PresetReq, U32 u32Size, S8 ** ps8PresetInfo)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn)
	{
		return pConn->GetPresetInfo(ps8PresetReq, u32Size, ps8PresetInfo);
	}
	
    return FALSE;
}

NET_API BOOL NET_SendHSResource(HANDLE hConn, const S8 * ps8Resource, S32 s32Size)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn)
	{
		return pConn->SendHSResource(ps8Resource, s32Size);
	}
	
    return FALSE;
}

NET_API BOOL NET_SendPresetInfo(HANDLE hConn, const S8 * ps8PresetInfo, U32 us32Size)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn)
	{
		return pConn->SendPresetInfo(ps8PresetInfo, us32Size);
	}

    return FALSE;
}

NET_API BOOL NET_SendCommand(HANDLE hConn, const S8 * ps8Info, S32 s32Size)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn)
	{
		return pConn->SendCommand(ps8Info, s32Size);
	}
	
	return FALSE;
}

NET_API BOOL NET_GetRemoteIvaAlarmData(HANDLE hConn, const S8 * ps8Req, U32 u32Size, S8 ** ps8AlarmInfo)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn)
	{
		return pConn->GetRemoteIvaAlarmData(ps8Req, u32Size, ps8AlarmInfo);
	}
	
	return FALSE;
}

NET_API BOOL NET_GetIvaCruiseList(HANDLE hConn, const S8 * ps8Req, U32 u32Size, S8 ** ps8IvaCruiseList)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn)
	{
		return pConn->GetIvaCruiseList(ps8Req, u32Size, ps8IvaCruiseList);
	}
	
	return FALSE;
}

NET_API BOOL NET_ControlIvaCruise(HANDLE hConn, const S8 * ps8Req, U32 u32Size)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn)
	{
		return pConn->ControlIvaCruise(ps8Req, u32Size);
	}
	
	return FALSE;
}

NET_API BOOL NET_QueryIvaRunList(HANDLE hConn, const S8 * ps8Req, U32 u32Size, S8 ** ps8Result)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn)
	{
		return pConn->QueryIvaRunList(ps8Req, u32Size, ps8Result);
	}
	
	return FALSE;
}

NET_API BOOL NET_QueryChanRecordState(HANDLE hConn, const S8 * ps8Req, U32 u32Size, S8 ** ps8ChanRecordState)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn)
	{
		return pConn->QueryChanRecordState(ps8Req, u32Size, ps8ChanRecordState);
	}
	
	return FALSE;
}

NET_API BOOL NET_GetResponse(HANDLE hConn, const S8 * ps8Req, U32 u32Size, S8 *ps8Rsp,U32 *u8RspSize)
{
	BasePlatformConn *pConn = (BasePlatformConn *)hConn;
	if (pConn)
	{
		return pConn->GetResponse(ps8Req, u32Size, ps8Rsp,*u8RspSize);
	}
	
	return FALSE;
}