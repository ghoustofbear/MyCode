#include "previewc1.h"
#include "connection.h"
#include "fs_rwc1.h"

#ifdef _WIN32
#include "plaympeg4.h"
//#include "3rd/tindy/tiandy_play.h"
#include "3rd/tindy/PLAYSDKM4_INTERFACE.cpp"
//#include "tiandy_play.h"
#include "3rd/dahua/dhplay.h"
#include "RSPlay.h"

//#pragma comment(lib, "../../lib/PlayCtrl.lib")
//#pragma comment(lib, "../../lib/dhplay.lib")
//#pragma comment(lib, "../../lib/RSPlay.lib")
//#pragma comment(lib, "../../lib/PlaySdkM4.lib")

#include <math.h>

struct stHelmetDataPView
{
	U8          u8Type;
	S8 *        s8Data;
	U32         u32Len;
}tagHelmetDataPView;

#endif

BYTE gbyHKHeaderO[40] = {
	0x34, 0x48, 0x4b, 0x48, 0xfe, 0xb3, 0xd0, 0xd6,
	0x08, 0x03, 0x04, 0x20, 0x00, 0x00, 0x00, 0x00,
	0x03, 0x10, 0x01, 0x10, 0x01, 0x10, 0x10, 0x00,
	0x80, 0x3e, 0x00, 0x00, 0xC0, 0x02, 0x40, 0x02,
	0x11, 0x10, 0x00, 0x00, 0x05, 0x00, 0x00, 0x00
};

BYTE gbyHKHeaderN[40] = {
	0x49, 0x4d, 0x4b, 0x48, 0x01, 0x01, 0x00, 0x00,
	0x02, 0x00, 0x00, 0x01, 0x21, 0x72, 0x01, 0x10,
	0x80, 0x3e, 0x00, 0x00, 0x80, 0x3e, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

BYTE gbyHKHeader8[40] = {
	0x34, 0x48, 0x4b, 0x48, 0xfe, 0xb3, 0xd0, 0xd6,
	0x08, 0x03, 0x04, 0x20, 0x00, 0x00, 0x00, 0x00,
	0x03, 0x10, 0x02, 0x10, 0x01, 0x10, 0x10, 0x00,
	0x80, 0x3e, 0x00, 0x00, 0x60, 0x01, 0xF0, 0x00,
	0x11, 0x10, 0x00, 0x00, 0x04, 0x00, 0x00, 0x00
};

BYTE gbyTDHeader_H264[88] = {
    0x19, 0x00, 0xc0, 0x02, 0x40, 0x02, 0x15, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x10, 0x40, 0x1f, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

BYTE gbyTDHeader_H265[88] = {
    0x19, 0x00, 0xc0, 0x02, 0x40, 0x02, 0x17, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x10, 0x40, 0x1f, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

BYTE gbyTDHeader_mJpeg[88] = {
    0x19, 0x00, 0xc0, 0x02, 0x40, 0x02, 0x15, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x01, 0x10, 0x40, 0x1f, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

#ifdef _WIN32

#if 0
IMPLEMENT_FUNCTION(TC_CreateSystem);
IMPLEMENT_FUNCTION(TC_DeleteSystem);
IMPLEMENT_FUNCTION(TC_CreatePlayerFromStreamEx);
IMPLEMENT_FUNCTION(TC_CreatePlayerFromStream);
IMPLEMENT_FUNCTION(TC_CreatePlayerFromVoD);
IMPLEMENT_FUNCTION(TC_CreatePlayerFromFile);
IMPLEMENT_FUNCTION(TC_SetPlayRect);
IMPLEMENT_FUNCTION(TC_Play);
IMPLEMENT_FUNCTION(TC_PlayAudio);
IMPLEMENT_FUNCTION(TC_PutStreamToPlayer);
IMPLEMENT_FUNCTION(TC_FastForward);
IMPLEMENT_FUNCTION(TC_SlowForward);
IMPLEMENT_FUNCTION(TC_Pause);
IMPLEMENT_FUNCTION(TC_Stop);
IMPLEMENT_FUNCTION(TC_DeletePlayer);
IMPLEMENT_FUNCTION(TC_CleanStreamBuffer);
IMPLEMENT_FUNCTION(TC_GetFrameCount);
IMPLEMENT_FUNCTION(TC_GetPlayingFrameNum);
IMPLEMENT_FUNCTION(TC_SeekEx);
IMPLEMENT_FUNCTION(TC_GetStreamPlayBufferState);
IMPLEMENT_FUNCTION(TC_CaptureBmpPic);
#endif


static HINSTANCE g_hTiandyInst = NULL;

VOID LoadTiandyPlayDll()
{
// 	if (!g_hTiandyInst)
// 		g_hTiandyInst = LoadLibraryA("PlaySdkM4.dll");
	
// 	EXPORT_FUNCTION(g_hTiandyInst, TC_CreateSystem);
// 	EXPORT_FUNCTION(g_hTiandyInst, TC_DeleteSystem);
// 	EXPORT_FUNCTION(g_hTiandyInst, TC_CreatePlayerFromFile);
// 	EXPORT_FUNCTION(g_hTiandyInst, TC_CreatePlayerFromVoD);
// 	EXPORT_FUNCTION(g_hTiandyInst, TC_CreatePlayerFromStreamEx);
// 	EXPORT_FUNCTION(g_hTiandyInst, TC_SetPlayRect);
// 	EXPORT_FUNCTION(g_hTiandyInst, TC_Play);
// 	EXPORT_FUNCTION(g_hTiandyInst, TC_PlayAudio);
// 	EXPORT_FUNCTION(g_hTiandyInst, TC_PutStreamToPlayer);
// 	EXPORT_FUNCTION(g_hTiandyInst, TC_FastForward);
// 	EXPORT_FUNCTION(g_hTiandyInst, TC_SlowForward);
// 	EXPORT_FUNCTION(g_hTiandyInst, TC_Pause);
// 	EXPORT_FUNCTION(g_hTiandyInst, TC_Stop);
// 	EXPORT_FUNCTION(g_hTiandyInst, TC_DeletePlayer);
// 	EXPORT_FUNCTION(g_hTiandyInst, TC_CleanStreamBuffer);
// 	EXPORT_FUNCTION(g_hTiandyInst, TC_GetFrameCount);
// 	EXPORT_FUNCTION(g_hTiandyInst, TC_GetPlayingFrameNum);
// 	EXPORT_FUNCTION(g_hTiandyInst, TC_SeekEx);
// 	EXPORT_FUNCTION(g_hTiandyInst, TC_GetStreamPlayBufferState);
// 	EXPORT_FUNCTION(g_hTiandyInst, TC_CaptureBmpPic);
	
//    PLAYSDK_INTERFACE::TC_CreateSystem(NULL);


    LoadPLAYSDK();

	RSPlayInitPlayLib();
}

VOID FreeTiandyPlayDll()
{
//	TC_DeleteSystem();//Release 调用会报错

// 	if(g_hTiandyInst)
// 	{
// 		::FreeLibrary(g_hTiandyInst);
// 		g_hTiandyInst = NULL;
// 	}

	RSPlayDeinitPlayLib();
}

int __cdecl DrawFunCallbackTd(long _lHandle, PLAYHDC _hDc, void* _lUserData)
{
	Preview *pPreview = (Preview *)_lUserData;
	if (pPreview == NULL)
	{
		return 0;
	}

	/**< 注册回调绘图函数*/
    if (pPreview->m_stDrawParam.hDraw)
	{
		S32 s32Width = 0, s32Height = 0;
        PLAYSDK_INTERFACE::TC_GetVideoParam(pPreview->GetPlayerID(), &s32Width, &s32Height, NULL);

	   ((pfnDrawFun)(pPreview->m_stDrawParam.hDraw))(_hDc, s32Height, s32Width, pPreview->m_stDrawParam.hUser);
	 }

	/**< 内部预置位叠加*/
    pPreview->DrawPreset(_hDc);
	return 0;
}

VOID __stdcall DrawFunCallbackHk(long _lHandle,HDC _hDc,long _lUserData)
{
	Preview *pPreview = (Preview *)_lUserData;
	if (pPreview == NULL)
	{
		return;
	}

	/**< 注册回调绘图函数*/
	if (pPreview->m_stDrawParam.hDraw)
	{
	     ((pfnDrawFun)(pPreview->m_stDrawParam.hDraw))(_hDc, pPreview->m_stPicParam.lHeight, 
		                      pPreview->m_stPicParam.lWidth, pPreview->m_stDrawParam.hUser);
	}

	/**< 内部预置位叠加*/
	pPreview->DrawPreset(_hDc);
}

VOID __stdcall DrawFunCallbackDh(long _lHandle,HDC _hDc,VOID * _lUserData)
{
	Preview *pPreview = (Preview *)_lUserData;
	if (pPreview == NULL)
	{
		return;
	}

	/**< 注册回调绘图函数*/
	if (pPreview->m_stDrawParam.hDraw)
	{
	    long lWidth = 0, lHeight = 0;
		PLAY_GetPictureSize(pPreview->GetPort(), &lWidth, &lHeight);

	   ((pfnDrawFun)(pPreview->m_stDrawParam.hDraw))(_hDc, lHeight, lWidth, pPreview->m_stDrawParam.hUser);
	}

	/**< 内部预置位叠加*/
	pPreview->DrawPreset(_hDc);
}

VOID __stdcall DrawFunCallbackH264(HDC _hDc, DWORD width, DWORD height, void* pUser)
{
	Preview *pPreview = (Preview *)pUser;

	if (pPreview == NULL)
	{
		return;
	}

	/**< 注册回调绘图函数*/
	if (pPreview->m_stDrawParam.hDraw)
	  ((pfnDrawFun)(pPreview->m_stDrawParam.hDraw))(_hDc, height, width, pPreview->m_stDrawParam.hUser);

	/**< 内部预置位叠加*/
	pPreview->DrawPreset(_hDc);
}

/* 各大厂商解码后数据回调 */

VOID __cdecl DecDataCallbackTd(U32 _ulID, U8 *_cData, S32 _iLen, const FRAME_INFO_TD *_pFrameInfo, void* _iUser)
{
    Preview *pPreview = (Preview *)_iUser;
    if (pPreview == NULL)
    {
		return;
    }

	/* 仅接收视频解码数据 */
    if (_pFrameInfo->nType != T_YUV420_TD &&
        _pFrameInfo->nType != T_YUV422_TD)
    {
		return;
    }

	if (pPreview->m_stDecParam.hDecCb)
	{
		DEC_FRAME_INFO param = {0};
		param.ps8Data    = (S8*)_cData;
		param.u32Len     = _iLen;
		param.s32Height  = _pFrameInfo->nHeight;
		param.s32Width   = _pFrameInfo->nWidth;

        if (_pFrameInfo->nType == T_YUV420_TD)
		{
			param.u8Type = UT_I420;
		}
        else if (_pFrameInfo->nType == T_YUV422_TD)
		{
            param.u8Type = UT_YUV422;
		}

         ((pfnDecDataCallback)(pPreview->m_stDecParam.hDecCb))(&param, pPreview->m_stDecParam.hUser);
	}
}

VOID __stdcall DecDataCallbackHik(long nPort,S8 * _cData,long _lLen,FRAME_INFO * pFrameInfo, long nReserved1,long nReserved2)
{
    Preview *pPreview = (Preview *)nReserved1;
    if (pPreview == NULL)
    {
		return;
    }

	/* 仅接收视频解码数据 */
	if (pFrameInfo->nType != T_YV12 &&
		pFrameInfo->nType != T_UYVY &&
		pFrameInfo->nType != T_RGB32)
	{
		return;
	}

	if (pPreview->m_stDecParam.hDecCb)
	{
		DEC_FRAME_INFO param = {0};
		param.ps8Data    = _cData;
		param.u32Len     = _lLen;
		param.s32Height  = pFrameInfo->nHeight;
		param.s32Width   = pFrameInfo->nWidth;

		if (pFrameInfo->nType == T_YV12)
		{
			param.u8Type = UT_YV12;
		}
		else if (pFrameInfo->nType == T_UYVY)
		{
            param.u8Type = UT_UYVY;
		}
		else if (pFrameInfo->nType == T_RGB32)
		{
            param.u8Type = UT_RGB32;
		}

         ((pfnDecDataCallback)(pPreview->m_stDecParam.hDecCb))(&param, pPreview->m_stDecParam.hUser);
	}
}

VOID __stdcall DecDataCallbackDh(LONG nPort,S8 * _cData,LONG _lLen, FRAME_INFO_DH * pFrameInfo, void* pUserData, LONG nReserved2)
{
    Preview *pPreview = (Preview *)pUserData;
    if (pPreview == NULL)
    {
		return;
    }

	/* 仅接收视频解码数据 */
	if (pFrameInfo->nType != T_IYUV &&
		pFrameInfo->nType != T_RGB32)
	{
		return;
	}

	if (pPreview->m_stDecParam.hDecCb)
	{
		DEC_FRAME_INFO param = {0};
		param.ps8Data    = _cData;
		param.u32Len     = _lLen;
		param.s32Height  = pFrameInfo->nHeight;
		param.s32Width   = pFrameInfo->nWidth;

		if (pFrameInfo->nType == T_IYUV)
		{
            param.u8Type = UT_I420;
		}
		else if (pFrameInfo->nType == T_RGB32)
		{
            param.u8Type = UT_RGB32;
		}

         ((pfnDecDataCallback)(pPreview->m_stDecParam.hDecCb))(&param, pPreview->m_stDecParam.hUser);
	}
}

S32 __stdcall DecDataCallbackH264(long hHandle, char *_cData, long _lLen, RS_FRAME_INFO *pFrameInfo, DWORD dwUser)
{
    Preview *pPreview = (Preview *)dwUser;
    if (pPreview == NULL)
    {
		return -1;
    }

	if (pPreview->m_stDecParam.hDecCb)
	{
		DEC_FRAME_INFO param = {0};
		param.ps8Data    = _cData;
		param.u32Len     = _lLen;
		param.s32Height  = pFrameInfo->nHeight;
		param.s32Width   = pFrameInfo->nWidth;
		param.u8Type     = UT_YV12;

         ((pfnDecDataCallback)(pPreview->m_stDecParam.hDecCb))(&param, pPreview->m_stDecParam.hUser);
	}

	return 0;
}

VOID *HelmetPreviewFun(VOID *pArg)
{
    Preview *pPreview = (Preview *)pArg;
	S32  s32Ret = 0;
	
	while (!pPreview->m_bTrdPlayExit)
	{
        if (0 == pPreview->m_listHelmetData.size())
		{
			Msleep(1);
			continue;
		}
		
		{
			AutoLock lock(pPreview->m_mtxHelmet);
			
			while(pPreview->m_listHelmetData.size() != 0)
			{			
				list<void* >::iterator iter = pPreview->m_listHelmetData.begin();			
				
				stHelmetDataPView* pstHelmetData = (stHelmetDataPView*)(*iter);
				if (0 == pstHelmetData->u8Type)
				{
					if(NULL != pPreview->m_hPlayerHandle)
					{			
						while(1)
						{
							s32Ret = fnVARender_PumpVideoFrame(pPreview->m_hPlayerHandle, pstHelmetData->s8Data, pstHelmetData->u32Len);
							if (VARENDER_E_OK == s32Ret)
							{
								break;
							}
						}	
					}
				}
				else 
				{
					if(NULL != pPreview->m_hPlayerHandle && pPreview->m_bSoundState)
					{
						fnVARender_PumpAudioFrame(pPreview->m_hPlayerHandle, pstHelmetData->s8Data, pstHelmetData->u32Len);
					}					
				}
				
				delete [] pstHelmetData->s8Data;
				delete pstHelmetData;
				
				pPreview->m_listHelmetData.erase(iter);
			}

		}
	}
	
	return NULL;
}

VOID RealDataRenderCallback(HWND hWnd, VOID *pData, U32 u32Size, VOID *pUser)
{
	UT_FRAME_HEAD_S *pFrameHead = (UT_FRAME_HEAD_S *)pData;
	Preview *pPView = (Preview *)pUser;
	
	if (!pFrameHead || !pPView || (pFrameHead->FrameSize > 1024*1024))
	{
		return;
	}
	
	BYTE *pBuffer = (BYTE *)((S8 *)pData + sizeof(UT_FRAME_HEAD_S));
	
	if (FRAME_TAG_HIK   == pFrameHead->FrameTag || 
		FRAME_TAG_HIK_N == pFrameHead->FrameTag ||
		FRAME_TAG_HIK8  == pFrameHead->FrameTag	)
	{
		if (-1 == pPView->m_lPort)
		{	
			if (!PlayM4_GetPort(&pPView->m_lPort)) 
			{
				return;
			}

			/* 设置实时流播放模式 */
			if (!PlayM4_SetStreamOpenMode(pPView->m_lPort, STREAME_REALTIME)) 
			{
				return;
			}			

			if (!PlayM4_OpenStream(pPView->m_lPort, NULL, 0, 1024*1024)) {
				return;
			}

			if ( !PlayM4_Play(pPView->m_lPort, hWnd) ) {
				return;
			}

			pPView->m_hWnd = hWnd;
		}

		PlayM4_RenderPrivateData(pPView->m_lPort, PLAYM4_RENDER_TEM, TRUE);
		PlayM4_RenderPrivateDataEx(pPView->m_lPort, PLAYM4_RENDER_TEM, 
			              PLAYM4_TEM_REGION_BOX|PLAYM4_TEM_REGION_LINE|PLAYM4_TEM_REGION_POINT, TRUE);

        PlayM4_RegisterDrawFun(pPView->m_lPort, DrawFunCallbackHk, (long)pPView);

		PlayM4_GetPictureSize(pPView->m_lPort, &pPView->m_stPicParam.lWidth, &pPView->m_stPicParam.lHeight);

		PlayM4_SetDecCallBackExMend(pPView->m_lPort, DecDataCallbackHik, NULL, NULL, (long)pPView);

		if (pFrameHead->FrameSize > 0 && pPView->m_lPort != -1)
		{
			if (!PlayM4_InputData(pPView->m_lPort, pBuffer, pFrameHead->FrameSize)) {
				return;
			} 
		}
	}
	else if (FRAME_TAG_DAHUA == pFrameHead->FrameTag) 
	{ 
		pPView->m_u32FrameType = Preview::FRAME_TYPE_DAHUA;
		if (-1 == pPView->m_lPort)
		{
			if ( PLAY_GetFreePort(&pPView->m_lPort) == FALSE)
			{
				return;	
			}
			
 			if (PLAY_SetStreamOpenMode(pPView->m_lPort, STREAME_REALTIME) == FALSE)
 			{
 				return;
 			}
			
			if (PLAY_OpenStream(pPView->m_lPort, NULL, 0, 1024*900) == FALSE)
			{
				return;
			}			     

			PLAY_RigisterDrawFun(pPView->m_lPort, DrawFunCallbackDh, pPView);

			PLAY_SetDecCallBackEx(pPView->m_lPort, DecDataCallbackDh, pPView);

			if (PLAY_Play(pPView->m_lPort, hWnd) == FALSE)
			{
				return;
			}

			pPView->m_hWnd = hWnd;
		}

		PLAY_RigisterDrawFun(pPView->m_lPort, DrawFunCallbackDh, pPView);
		
		if (pFrameHead->FrameSize > 0 && pPView->m_lPort != -1)
		{
			if (!PLAY_InputData(pPView->m_lPort, pBuffer, pFrameHead->FrameSize))
			{
				return;
			} 
		}	
	}
	else if (FRAME_TAG_TIANDY == pFrameHead->FrameTag) 
	{ 
		pPView->m_u32FrameType = Preview::FRAME_TYPE_TIANDY;

		if (pPView->m_s32EncodeType != pFrameHead->EncodeType)
		{
			if (-1 != pPView->m_iPlayerID)
			{
                PLAYSDK_INTERFACE::TC_Stop(pPView->m_iPlayerID);
                PLAYSDK_INTERFACE::TC_DeletePlayerEx(pPView->m_iPlayerID, TRUE);
				pPView->m_iPlayerID = -1;
			}
		}

		pPView->m_s32EncodeType = pFrameHead->EncodeType;
		
		if (pPView->m_iPlayerID == -1)
		{
			BYTE byTdSysHeader[88] = {0};
			if (0x15 == pPView->m_s32EncodeType)
			{
                memcpy(byTdSysHeader, gbyTDHeader_H264, 88);
			}
			else if (0x17 == pPView->m_s32EncodeType)
			{
                memcpy(byTdSysHeader, gbyTDHeader_H265, 88);
			}
			else if (0x29 == pPView->m_s32EncodeType)
			{
                memcpy(byTdSysHeader, gbyTDHeader_mJpeg, 88);
			}
			else
			{
				return;
			}

			/* 天地播放库创建解码器需传入私有头*/
            pPView->m_iPlayerID = PLAYSDK_INTERFACE::TC_CreatePlayerFromStreamEx(hWnd, byTdSysHeader, 88, 0);
			
			if (pPView->m_iPlayerID >= 0)
			{		
                PLAYSDK_INTERFACE::TC_Play(pPView->m_iPlayerID);
			}
			else
			{			
				pPView->m_iPlayerID = -1;
				return;
			}

			pPView->m_hWnd = hWnd;
		}
		else
		{
			if (pFrameHead->FrameSize > 0)
			{
                PLAYSDK_INTERFACE::TC_PutStreamToPlayer(pPView->m_iPlayerID, pBuffer, pFrameHead->FrameSize);
			}
		}

        PLAYSDK_INTERFACE::TC_RegisterDrawFun(pPView->m_iPlayerID, DrawFunCallbackTd, (void*)pPView);

        PLAYSDK_INTERFACE::TC_SetDecCallBack(pPView->m_iPlayerID, DecDataCallbackTd, (void*)pPView);
	}
	else  if (FRAME_TAG_STD_H264 == pFrameHead->FrameTag)
	{
		pPView->m_u32FrameType = Preview::FRAME_TYPE_STD_H264;

		if (pPView->m_iPlayerID == -1)
		{
			pPView->m_iPlayerID = RSPlayCreatePlayInstance(PLAY_NETSTREAM_TYPE);
			RSPlaySetPlayWnd(pPView->m_iPlayerID, hWnd);
			RSPlayStartPlay(pPView->m_iPlayerID);

			pPView->m_hWnd = hWnd;
		}		

		if (-1 != pPView->m_iPlayerID)
		{
			RSPlayInputNetFrame(pPView->m_iPlayerID, pData, u32Size);

            RSPlayRegisterDrawFun(pPView->m_iPlayerID, DrawFunCallbackH264, pPView);
			RSPlayRegisterDecCallback(pPView->m_iPlayerID, DecDataCallbackH264, (unsigned long)pPView);
		}	
	}
	else if (FRAME_TAG_HELMET_GX == pFrameHead->FrameTag)
	{
		pPView->m_u32FrameType = Preview::FRAME_TYPE_HELMET_GX;
		S32 s32Ret = 0;		

		if (NULL == fnVARender_Open)
		{		
			return;
		}
	
		if (NULL == pPView->m_hPlayerHandle)
		{
			VARENDER_HANDLE * pHandle = (VARENDER_HANDLE *) &(pPView->m_hPlayerHandle);
			s32Ret = fnVARender_Open(pHandle);
			if (0 == s32Ret)
			{
				fnVARender_SetWindow(pPView->m_hPlayerHandle, hWnd);
				
				fnVARender_StartVideo(pPView->m_hPlayerHandle);
				
				fnVARender_StartAudio(pPView->m_hPlayerHandle);

				CreateJoinableThread(HelmetPreviewFun, pUser, &pPView->m_hHelmetPlayCtrl);
			}
		}
		
		if (NULL != pPView->m_hPlayerHandle)
		{		
			do 
			{
				stHelmetDataPView * pstHelmetData = new stHelmetDataPView;
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
				
				AutoLock lock(pPView->m_mtxHelmet);
				pPView->m_listHelmetData.push_back(pstHelmetData);
			} while (0);
		}
	}
}
#endif

Preview::Preview(Connection *pConn)
    :BasePView()
{
	m_pConn              = pConn;
	m_bIsGetUdpPViewKey  = FALSE;
	m_bExit              = FALSE;
	m_trdPView           = 0;
	m_lPort              = -1;
	m_iPlayerID          = -1;
	m_bRecord            = FALSE;
	m_pSaveFile          = NULL;
	m_pRecWriter         = NULL;
	m_bRecordSwitch      = FALSE;
	m_bWriteHikSysHeader = FALSE;
	m_bWriteTiandySysHeader = FALSE;
	m_bWriteDahuaSysHeader  = FALSE;
	m_u32FrameType        = 0;

	m_u8ReqType      = 0;
	m_u32ChnNO       = 0;
	m_u8Type         = 0;
	m_u8Content      = 0;
	m_u32Proto       = 0;
	m_bAutoReconn    = 0;
	m_u16DevicePort  = 0;
	m_u16SubServPort = 0;

	memset(m_u8DeviceIp, 0, MAX_IPV4_LEN);
	memset(m_u8SubServIp, 0, MAX_IPV4_LEN);
	memset(m_s8FrontAddrCode, 0, MAX_ADDR_LEN);

	memset(&m_s8HikSysHeader, 0, 40);
	memset(&m_PViewRsp, 0, sizeof(m_PViewRsp));

	memset(m_s8PresetName, 0, MAX_NAME_LEN);

	m_u16Px         = 0;
	m_u16Py         = 0;
	m_u8WndNum      = 0;
	m_s32EncodeType = 0x15;

	m_tmPrevTime     = 0;

#ifdef _WIN32
	 //加载预置位字体信息
	m_cfPreset.yHeight = 200;
	m_cfPreset.bCharSet = 134;
	m_cfPreset.bPitchAndFamily = 49;
	m_cfPreset.dwMask = CFM_ALL;
 	memcpy(&m_cfPreset.szFaceName, "宋体", strlen("宋体"));

	COLORREF color;
	color = RGB(255, 255, 255);
	
	m_cfPreset.yHeight = 760;
	m_cfPreset.crTextColor = color;
	m_cfPreset.dwEffects = CFE_BOLD;

	m_stDrawParam.hDraw = NULL;
	m_stDrawParam.hUser = NULL;

	m_stDecParam.hDecCb = NULL;
	m_stDecParam.hUser  = NULL;
#endif

	m_hPlayerHandle     = NULL;
	m_bSoundState       = FALSE;
	m_bTrdPlayExit      = FALSE;
    m_mixAudioVideo     = NULL;
}

BOOL Preview::IsSameStream(const NET_PVIEW_PARAM_S* pParam)
{
	if (pParam->ps8FrontAddrCode)
	{
		return (0 == memcmp(m_s8FrontAddrCode, pParam->ps8FrontAddrCode, MAX_ADDR_LEN));
	}

	U8 u8DeviceIp[MAX_IPV4_LEN] = {0};
	U8 u8SubServIp[MAX_IPV4_LEN]= {0};
	INET_PTON(pParam->ps8DeviceIp,  u8DeviceIp);
	INET_PTON(pParam->ps8SubServIp, u8SubServIp);

	return m_u32ChnNO       == pParam->u32ChnNO 
		&& m_u8Type         == pParam->u8Type 
		&& m_u8Content      == pParam->u8Content
		&& m_u32Proto       == pParam->u32Proto
		&& m_bAutoReconn    == pParam->bAutoReconn
		&& m_u16DevicePort  == pParam->u16DevicePort
		&& m_u16SubServPort == pParam->u16SubServPort
		&& 0 == memcmp(m_u8DeviceIp, u8DeviceIp, MAX_IPV4_LEN)
		&& 0 == memcmp(m_u8SubServIp, u8SubServIp, MAX_IPV4_LEN);
}

PreviewClient *Preview::Start(const NET_PVIEW_PARAM_S *pParam)
{
	ASSERT(pParam);
// 	ASSERT(pParam->ps8DeviceIp);
// 	ASSERT(pParam->ps8SubServIp);

	AutoLock lock(m_mtxPViewParams);

	PreviewClient PViewClient;
	PViewClient.stParam       = *pParam;
	PViewClient.pPView        = this;
	PViewClient.bNeedKeyFrame = TRUE;

	m_PViewClients.push_back(PViewClient);

	if (m_PViewClients.size() == 1)
	{
		m_bAutoReconn =  pParam->bAutoReconn;
		m_u32ChnNO    =  pParam->u32ChnNO;
		m_u8Type      =  pParam->u8Type;
		m_u8Content   =  pParam->u8Content;
		m_u32Proto    =  pParam->u32Proto;

		if (pParam->ps8FrontAddrCode)
		{
			m_u8ReqType = 1;
			strncpy(m_s8FrontAddrCode, pParam->ps8FrontAddrCode, MAX_ADDR_LEN);
		}
		else
		{
			m_u8ReqType   = 0;
			m_u16DevicePort  =  pParam->u16DevicePort;
			m_u16SubServPort =  pParam->u16SubServPort;
			INET_PTON(pParam->ps8DeviceIp,  m_u8DeviceIp);
			INET_PTON(pParam->ps8SubServIp, m_u8SubServIp);
		}

		CreateJoinableThread(PreviewFun, this, &m_trdPView);
	}

	return &m_PViewClients.back();
}

BOOL Preview::Stop(PreviewClient* pPViewClient)
{
	{
		AutoLock lock(m_mtxPViewParams);
		list<PreviewClient>::iterator i = m_PViewClients.begin();
		BOOL found = FALSE;
		while (i != m_PViewClients.end())
		{
			if (&(*i) == pPViewClient)
			{
				found = TRUE;
				m_PViewClients.erase(i);
				break;
			}
			++i;
		}

		ASSERT(found);

		if (!m_PViewClients.empty())
			return FALSE;
	}

	Stop();

	m_bExit       = TRUE;
	m_bAutoReconn = FALSE;
	ThreadJoin(m_trdPView);

	return TRUE;
}

VOID Preview::Stop()
{
#ifdef _WIN32
	if (-1 != m_lPort)
	{
		if (FRAME_TYPE_HIK == m_u32FrameType)
		{
			PlayM4_CloseStreamEx(m_lPort);
			PlayM4_Stop(m_lPort);
			PlayM4_FreePort(m_lPort);
		}
		else if (FRAME_TYPE_DAHUA == m_u32FrameType)
		{
			PLAY_CloseStream(m_lPort);
			PLAY_Stop(m_lPort);
		}
		m_lPort = -1;
	}

	if (-1 != m_iPlayerID)
	{
		if (FRAME_TYPE_TIANDY == m_u32FrameType)
		{
            PLAYSDK_INTERFACE::TC_Stop(m_iPlayerID);
            PLAYSDK_INTERFACE::TC_DeletePlayer(m_iPlayerID);
		}
		else if (FRAME_TYPE_STD_H264 == m_u32FrameType)
		{
			RSPlayStopPlay(m_iPlayerID);
			RSPlayDestroyPlayInstance(m_iPlayerID);
		}

		m_iPlayerID = -1;
	}

	if (NULL != m_hPlayerHandle)
	{
		 m_bTrdPlayExit  = TRUE;
		 ThreadJoin(m_hHelmetPlayCtrl);

         fnVARender_StopVideo(m_hPlayerHandle);
		 fnVARender_StopAudio(m_hPlayerHandle);
         fnVARender_Close(m_hPlayerHandle);
		 m_hPlayerHandle = NULL;
	}

#endif

	{
		AutoLock lock(m_mtxPViewSock);
		m_PViewSock.KillSocket();
	}

	StopSaveRealData();

#if 0 
	if (!IsTcp())
		mMsgTransactionEvent.Set();
#endif
}

BOOL Preview::StopAll()
{
	{
		AutoLock lock(m_mtxPViewParams);
		m_PViewClients.erase(m_PViewClients.begin(), m_PViewClients.end());
	}

	Stop();

	m_bExit       = TRUE;
	m_bAutoReconn = FALSE;
	ThreadJoin(m_trdPView);

	return TRUE;
}

VOID *Preview::PreviewFun(VOID *pArg)
{
	Preview* pPView = (Preview *)pArg;
	pPView->PreviewLoop();

	return 0;
}

BOOL Preview::IsTcp()
{
	return m_u32Proto != 1;
}

VOID Preview::PreviewLoop()
{
	do 
	{
		if (!m_pConn->IsLogined())
		{
			Msleep(100);
			continue;
		}

		time_t tmNow = time(NULL);
		if (tmNow - m_tmPrevTime >= 2)
		{
			//PreviewStatusFun(RSNetMsgPreviewOpenStream);
			BOOL streamOpened = OpenStream();
			if (streamOpened)
			{
				//PreviewStatusFun(RSNetMsgPreviewOpenStreamSuccess);
				//mTransDataRateStat.Clear();
				RecvStream();
			}
			else
			{
				m_tmPrevTime = tmNow;

// 				if (m_bAutoReconn)
// 				{
// 				//	Msleep(2000);
// 					Msleep(100);
// 				}
			}
		}
		else
		{
			Msleep(100);
		}
#if 0

		mTransDataRateStat.Clear();

		if (mGetUdpPreviewKey)
		{
			UdpStopPreviewReq stReq;
			stReq.UdpPreviewKey = mUdpPreviewKey;

			MsgHeadWrap msgHead(MsgStopUdpPreview, MSG_NO_SUBTYPE, sizeof(stReq));
			msgHead.Version = m_pConn->mMsgHeadVersion;
			MsgBuf buf;

			m_pConn->Transaction(msgHead, &stReq, buf, 0);

			mGetUdpPreviewKey = FALSE;
		}

		if (streamOpened)
			PreviewStatusFun(RSNetMsgPreviewStreamClosed);
#endif
	} while (m_bAutoReconn);

#if 0
	if (mGetUdpPreviewKey)
	{
		UdpStopPreviewReq stReq;
		stReq.UdpPreviewKey = mUdpPreviewKey;

		MsgHeadWrap msgHead(MsgStopUdpPreview, MSG_NO_SUBTYPE, sizeof(stReq));
		msgHead.Version = m_pConn->mMsgHeadVersion;
		MsgBuf buf;

		m_pConn->Transaction(msgHead, &stReq, buf, 0);

		mGetUdpPreviewKey = FALSE;
	}
#endif
}

VOID Preview::RecvStream()
{
	FrameBuf frameBuf;
	PackBuf packBuf;

	do 
	{
		if (!packBuf.RecvPack(m_PViewSock, IsTcp()))
		{
			break;
		}

		//mTransDataRateStat.TransData(packBuf.PackSize + sizeof(PackHead));

		if (frameBuf.FrameComplete(packBuf))
		{
			AutoLock lock(m_mtxPViewParams);

			list<PreviewClient>::iterator i = m_PViewClients.begin();
			for (; i!=m_PViewClients.end(); ++i)
			{
				PreviewClient& pvClient = *i;
				const NET_PVIEW_PARAM_S *param = &pvClient.stParam;

				UT_FRAME_HEAD_S *pHead = (UT_FRAME_HEAD_S*)frameBuf.GetFrame();

				if (pHead->FrameTag == FRAME_TAG_HIK && pHead->FrameType == FRAME_I )
				{
					/* 保存海康私有头 */
					memcpy(m_s8HikSysHeader, (S8 *)frameBuf.GetFrame() + 40, 40);
				}
				else if (pHead->FrameTag == FRAME_TAG_TIANDY /*&& pHead->FrameType == FRAME_I*/ )
				{
					/* 保存天地私有头 */
					if (pHead->EncodeType == 0x15)
					{
                        memcpy(m_s8TiandySysHeader, gbyTDHeader_H264, 88);
					}
					else if (pHead->EncodeType == 0x17)
					{
						memcpy(m_s8TiandySysHeader, gbyTDHeader_H265, 88);
					}
					else
					{
						break;
					}										
				}

				if (((UT_FRAME_HEAD_S*)frameBuf.GetFrame())->FrameTag == FRAME_TAG_HIK ||
						((UT_FRAME_HEAD_S*)frameBuf.GetFrame())->FrameTag == FRAME_TAG_HIK_N)
				{
					m_u32FrameType = FRAME_TYPE_HIK;
				}
				else if (((UT_FRAME_HEAD_S*)frameBuf.GetFrame())->FrameTag == FRAME_TAG_TIANDY)
				{
					m_u32FrameType = FRAME_TYPE_TIANDY;
				}
				else if (((UT_FRAME_HEAD_S*)frameBuf.GetFrame())->FrameTag == FRAME_TAG_DAHUA)
				{
					m_u32FrameType = FRAME_TYPE_DAHUA;
				}
				else if (((UT_FRAME_HEAD_S*)frameBuf.GetFrame())->FrameTag == FRAME_TAG_STD_H264)
				{
					m_u32FrameType = FRAME_TYPE_STD_H264;
				}

				if (param->fRealDataCallback)
				{
#if 0
					if (pvClient.bNeedKeyFrame)
					{
						if (((UT_FRAME_HEAD_S*)frameBuf.GetFrame())->FrameType != FRAME_I)
							continue;

						pvClient.bNeedKeyFrame = FALSE;
					}
#endif

					param->fRealDataCallback(param->hWnd, frameBuf.GetFrame(), 
							frameBuf.GetFrameTotalSize(), param->pUser);
				}

#ifdef _WIN32
				if (param->hWnd || m_stDecParam.hDecCb)
				{
					RealDataRenderCallback(param->hWnd, frameBuf.GetFrame(), 
							frameBuf.GetFrameTotalSize(), this);
				}
#endif
				if (m_bRecord)
				{	
					if ( FRAME_TAG_HIK == m_u32FrameType && !m_bWriteHikSysHeader)
					{
						if (m_pSaveFile)
							fwrite(gbyHKHeaderO, 40, 1, m_pSaveFile);
						m_bWriteHikSysHeader = TRUE;
					}
					else if (((UT_FRAME_HEAD_S*)frameBuf.GetFrame())->FrameTag == FRAME_TAG_HIK_N
						&& !m_bWriteHikSysHeader)
					{
						if (m_pSaveFile)
							fwrite(gbyHKHeaderN, 40, 1, m_pSaveFile);
						m_bWriteHikSysHeader = TRUE;
					}
					else if (FRAME_TYPE_TIANDY == m_u32FrameType && !m_bWriteTiandySysHeader)
					{
						if (m_pSaveFile)
							fwrite(m_s8TiandySysHeader, 88, 1, m_pSaveFile);
						m_bWriteTiandySysHeader = TRUE;
					}
					else if (FRAME_TYPE_DAHUA == m_u32FrameType && !m_bWriteDahuaSysHeader)
					{
						if (m_pSaveFile)
							fwrite("DHAV", 4, 1, m_pSaveFile);
						m_bWriteDahuaSysHeader = TRUE;
					}

					if (FRAME_TYPE_STD_H264 == m_u32FrameType)
					{
						Record(frameBuf.GetFrame());
					}
					else
					{
						if (m_pSaveFile)
						{
							fwrite((S8 *)frameBuf.GetFrame() + 40, frameBuf.GetFrameTotalSize() - 40, 1, m_pSaveFile);
						}
					}
				}
                
                if (param->u32ChnNO == m_pConn->GetTalkChannel())
                {                    
				    if (((UT_FRAME_HEAD_S*)frameBuf.GetFrame())->FrameTag == FRAME_TAG_HIK ||
						    ((UT_FRAME_HEAD_S*)frameBuf.GetFrame())->FrameTag == FRAME_TAG_HIK_N)
                    {
                        m_pConn->WritePDualTalkEx((S8 *)frameBuf.GetFrame() + 40,frameBuf.GetFrameTotalSize() - 40);
                    }
                }
			}	
		}
	}while(!m_bExit);

	AutoLock lock(m_mtxPViewSock);
	m_PViewSock.Close();

	if (m_pSaveFile)
	{
		fclose(m_pSaveFile);
		m_pSaveFile = NULL;
	}
}

BOOL Preview::OpenStream()
{
	{
		AutoLock lock(m_mtxPViewSock);
		if (!m_PViewSock.Socket(IsTcp()))
		{
			return FALSE;
		}
	}

	if (IsTcp())
	{
		TCP_PVIEW_REQ_S stReq;
		memset(&stReq, 0, sizeof(stReq));

		stReq.u32UserKey = m_pConn->GetUserKey();
		stReq.stParam.u8Type    = m_u8Type;    
		stReq.stParam.u8Content = m_u8Content;
		if (1 == m_u8ReqType)
		{
			stReq.stParam.u8ReqType = 1;
			strncpy(stReq.stParam.s8FrontAddrCode, m_s8FrontAddrCode, MAX_ADDR_LEN);
		}
		else
		{
			stReq.stParam.u8ReqType      = 0;
			stReq.stParam.u32ChnNO       = m_u32ChnNO;
			stReq.stParam.u16DevicePort  = m_u16DevicePort;
			stReq.stParam.u16SubServPort = m_u16SubServPort; 
		
			memcpy(stReq.stParam.u8DeviceIp,  m_u8DeviceIp,  MAX_IPV4_LEN);
			memcpy(stReq.stParam.u8SubServIp, m_u8SubServIp, MAX_IPV4_LEN);
		}

		MsgHeadWrap msgHead(MSG_TCP_PREVIEW, MSG_NO_SUBTYPE, sizeof(stReq));
		msgHead.u2Version = m_pConn->m_u32MsgVersion;
		MsgBuf buf;

		if (m_PViewSock.SetSendTimeOut(SEND_TIMEOUT) \
				&& m_PViewSock.SetRecvTimeOut(RECV_TIMEOUT) \
				&& m_PViewSock.SetRecvBuf(256*1024)  \
				&& m_PViewSock.Connect(m_pConn->GetIP(), m_pConn->GetPort(), CONN_TIMEOUT, TRUE) \
				&& SendMsg(m_PViewSock, msgHead, &stReq) \
				&& RecvMsg(m_PViewSock, buf) \
				&& buf.ValidRsp(msgHead) \
				&& buf.IsSuccess())
		{
			memcpy(&m_PViewRsp, buf.GetData(), buf.GetDataLen());

			return TRUE;
		}
	}

	else
	{
		MsgHeadWrap msgHead(MSG_GET_UDPADDR);
		msgHead.u2Version = m_pConn->m_u32MsgVersion;
		MsgBuf buf;

		if (m_PViewSock.SetSendTimeOut(SEND_TIMEOUT) 
				&& m_PViewSock.SetRecvTimeOut(RECV_TIMEOUT) 
				&& m_PViewSock.SetRecvBuf(256*1024) 
				&& m_PViewSock.Connect(m_pConn->GetIP(), m_pConn->GetPort(), CONN_TIMEOUT, FALSE) 
				&& SendMsg(m_PViewSock, msgHead) 
				/* && RecvMsgFrom(m_PViewSock, buf) */
				&& buf.ValidRsp(msgHead))
		{
			UDP_START_PVIEW_REQ_S stReq;
			memset(&stReq, 0, sizeof(stReq));
			stReq.stUdpRsp = *(UDPADDR_RSP_S *)buf.GetData();
			stReq.stParam.u32ChnNO  = m_u32ChnNO;
			stReq.stParam.u8Type    = m_u8Type;
			stReq.stParam.u8Content = m_u8Content;
			stReq.stParam.u16DevicePort   = m_u16DevicePort;
			stReq.stParam.u16SubServPort  = m_u16SubServPort;
			memcpy(stReq.stParam.u8DeviceIp,  m_u8DeviceIp,  MAX_IPV4_LEN);
			memcpy(stReq.stParam.u8SubServIp, m_u8SubServIp, MAX_IPV4_LEN);
#if 0	
			mReqHead.SetMsgHead(MsgStartUdpPreview, MSG_NO_SUBTYPE, sizeof(stReq));
			mReqHead.Version = m_pConn->mMsgHeadVersion;
			mMsgTransaction.Init(mReqHead, buf, mMsgTransactionEvent);

			if (!mStopPreviewEvent.Wait(0) && m_pConn->Transaction(mMsgTransaction, &stReq) && buf.IsSuccess())
			{
				UdpStartPreviewRsp* rsp = (UdpStartPreviewRsp*)buf.GetData();
				mUdpPreviewKey = rsp->UdpPreviewKey;
				mGetUdpPreviewKey = TRUE;
				return TRUE;
			}

			if (mAutoReconnect)
				mStopPreviewEvent.Wait(2000);
#endif
		}
	}

	AutoLock lock(m_mtxPViewSock);
	m_PViewSock.Close();

	return FALSE;
}

BOOL Preview::GetResponse(PVIEW_RSP_S *pRsp)
{
	if (!pRsp)
	{
		return FALSE;
	}

	memcpy(pRsp, &m_PViewRsp, sizeof(PVIEW_RSP_S));

	return TRUE;
}

BOOL Preview::SaveRealData(S8 *ps8FileName)
{
	ASSERT(ps8FileName);

#ifdef _WIN32
	if (-1 != m_iPlayerID)
	{
		if (FRAME_TYPE_STD_H264 == m_u32FrameType)
		{
			RSPlayStartlocalrecord(m_iPlayerID, ps8FileName, 1);
			return TRUE;
		}
	}

	if (m_pSaveFile)
	{
		fclose(m_pSaveFile);
		m_pSaveFile = NULL;
	}

	if (ps8FileName)
	{
		m_pSaveFile = fopen(ps8FileName, "wb");
		if (!m_pSaveFile)
		{
			return  FALSE;
		}
		m_bRecord = TRUE;

		return  TRUE;
	}
#else
	if (FRAME_TYPE_STD_H264 == m_u32FrameType)
	{
		AutoLock lock(m_mtxRecord);
		if (m_pRecWriter)
		{
			delete m_pRecWriter;
			m_pRecWriter = NULL;
		}
		
		m_pRecWriter = new RecordWriter();
		if (!m_pRecWriter)
		{
			return FALSE;
		}
		
		if (!m_pRecWriter->open(ps8FileName))
		{
			delete m_pRecWriter;
			m_pRecWriter = NULL;
		}
		m_bRecord = TRUE;
		return  TRUE;
	}
#endif

	return FALSE;
}

BOOL Preview::StopSaveRealData()
{
#ifdef _WIN32
	if (-1 != m_iPlayerID)
	{
		if (FRAME_TYPE_STD_H264 == m_u32FrameType)
		{
			RSPlayStoplocalrecord(m_iPlayerID);
			return TRUE;
		}
	}

	m_bRecord            = FALSE;
	m_bWriteHikSysHeader = FALSE;
	m_bWriteTiandySysHeader = FALSE;
	m_bWriteDahuaSysHeader  = FALSE;
	
	if (m_pSaveFile)
	{
		fclose(m_pSaveFile);
		m_pSaveFile = NULL;
	}
#else
	if (FRAME_TYPE_STD_H264 == m_u32FrameType)
	{
		AutoLock lock(m_mtxRecord);
		if (!m_bRecord)
		{
			return FALSE;
		}
		
		if (m_pRecWriter)
		{
			delete m_pRecWriter;
			m_pRecWriter = NULL;
		}
		m_bRecord = FALSE;
	}
#endif

	return TRUE;
}

VOID Preview::Record(VOID *pFrame)
{
#ifdef _WIN32
#else
	AutoLock lock(m_mtxRecord);
	if (!m_bRecord)
	{
		return;
	}
	
	if (!m_pRecWriter)
	{
		return;
	}
	
	UT_FRAME_HEAD_S *pFrameHead = (UT_FRAME_HEAD_S *)pFrame;
	
	if (m_bRecordSwitch && 
		FRAME_I == pFrameHead->FrameType)
	{
		IWriter *pRecWriter = new RecordWriter;
		if (pRecWriter)
		{
			if (!pRecWriter->open(m_strFilePathSwitch.c_str()))
			{
				
				delete pRecWriter;
				pRecWriter = NULL;
			}
			else
			{
				delete m_pRecWriter;
				m_pRecWriter = pRecWriter;
			}
			
			m_bRecordSwitch = FALSE;
		}
	}
	
	m_pRecWriter->write(pFrame);
#endif
}

VOID Preview::SaveSwitch(S8 *ps8FilePath, U32 u32FileType)
{
#ifdef _WIN32
#else
	AutoLock lock(m_mtxRecord);
	m_strFilePathSwitch = ps8FilePath;
	m_bRecordSwitch     = TRUE;
#endif
}

BOOL Preview::CapturePicture(S8 *ps8PicFileName, U32 u32CaptueMode)
{
	ASSERT(ps8PicFileName);

	BOOL bRet = TRUE;

#ifdef _WIN32
	if (FRAME_TYPE_HIK == m_u32FrameType)
	{
		FILE *pPicFile = NULL;
		if (ps8PicFileName)
		{
			pPicFile = fopen(ps8PicFileName, "wb");
			if (!pPicFile)
			{
				return  FALSE;
			}
		}
		
		DWORD dwBuffSize = sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + 1280*720*4;
		DWORD dwPicSize   = 0;
		
		BYTE *pPicBuff = new BYTE[dwBuffSize];
		if (!pPicBuff)
		{
			fclose(pPicFile);
			return FALSE;
		}

		if (-1 != m_lPort)
		{
			DWORD dwPicSize  = 0;
			if (CAP_BMP_MODE == u32CaptueMode)
			{
				if (PlayM4_GetBMP(m_lPort, pPicBuff, dwBuffSize, &dwPicSize))
				{
					fwrite(pPicBuff, dwPicSize, 1, pPicFile);
				}
			}
			else if (CAP_JPEG_MODE == u32CaptueMode)
			{
				if (PlayM4_GetJPEG(m_lPort, pPicBuff, dwBuffSize, &dwPicSize))
				{
					fwrite(pPicBuff, dwPicSize, 1, pPicFile);
				}			
			}
		}

		fclose(pPicFile);
		delete []pPicBuff;
	}
	else if (FRAME_TYPE_DAHUA == m_u32FrameType)	
	{		
		if (-1 != m_lPort)
		{
			if (CAP_BMP_MODE == u32CaptueMode)
			{
				bRet = PLAY_CatchPicEx(m_lPort, ps8PicFileName, PicFormat_BMP);
			}
			else if (CAP_JPEG_MODE == u32CaptueMode)
			{	
				bRet = PLAY_CatchPicEx(m_lPort, ps8PicFileName, PicFormat_JPEG);
			}
		}
	}
	else if (FRAME_TYPE_TIANDY == m_u32FrameType)
	{
		if (-1 != m_iPlayerID)
		{
			/* 为了适应客户端，这里不分类型 */
			//if (CAP_BMP_MODE == u32CaptueMode)
			//{
            if (PLAYSDK_INTERFACE::TC_CaptureBmpPic(m_iPlayerID, ps8PicFileName) > 0)
				{
					//fwrite(pPicBuff, dwPicSize, 1, pPicFile);
				}
			//}
			//else if (CAP_JPEG_MODE == u32CaptueMode)
			//{		
			//}
		}
	}

	else if (FRAME_TYPE_STD_H264 == m_u32FrameType)
	{
		if (-1 != m_iPlayerID)
		{
			if (CAP_BMP_MODE == u32CaptueMode)
			{
				if (RSPlayCapturePicture(m_iPlayerID, ps8PicFileName) > 0)
				{
				}
			}
			else if (CAP_JPEG_MODE == u32CaptueMode)
			{		
			}
		}
	}
#endif
	return bRet;
}

BOOL Preview::OpenSound()
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
	else if (FRAME_TYPE_HELMET_GX == m_u32FrameType)
	{
		if (NULL != m_hPlayerHandle && !m_bSoundState)
		{
			m_bSoundState = TRUE;
		}
	}
#endif
	return FALSE;
}

BOOL Preview::CloseSound()
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
	else if (FRAME_TYPE_HELMET_GX == m_u32FrameType)
	{
		if (NULL != m_hPlayerHandle && m_bSoundState)
		{
            m_bSoundState = FALSE;
		}
	}
#endif
	return FALSE;
}

#ifdef _WIN32
VOID Preview::DrawPreset(S8 *ps8PresetName, U8 u8WndNum, U16 u16Left, U16 u16Top)
{
	memset(m_s8PresetName, 0, MAX_NAME_LEN);

	if (strlen(ps8PresetName) > MAX_NAME_LEN-1)
	{
		S32 s32Split = split_name(ps8PresetName, MAX_NAME_LEN-1);
    	strncpy(m_s8PresetName, ps8PresetName, s32Split);
	}
	else
	{
        strncpy(m_s8PresetName, ps8PresetName, MAX_NAME_LEN);
	}
	
	m_u16Px    = u16Left;
	m_u16Py    = u16Top;
	m_u8WndNum = u8WndNum;

}

VOID CharformatToLogfont(CHARFORMAT & cf, LOGFONT & lf, COLORREF & cr)
{
	lf.lfCharSet = cf.bCharSet;
	lf.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lf.lfEscapement = 0;
	lf.lfOrientation = 0;
	lf.lfOutPrecision = OUT_DEFAULT_PRECIS;
	lf.lfQuality = DEFAULT_QUALITY;
	lf.lfPitchAndFamily = DEFAULT_PITCH;
	
	if ( (cf.dwEffects & CFE_BOLD) == CFE_BOLD)
	{
		lf.lfWeight = FW_BOLD;
	}

	HDC hdc = CreateDC(("display"), NULL, NULL, NULL);
	S32 s32Px = GetDeviceCaps(hdc, LOGPIXELSY);
	lf.lfHeight = -MulDiv(cf.yHeight/20, s32Px, 72);
	DeleteDC(hdc);
	
	lf.lfUnderline = ( (cf.dwEffects & CFE_UNDERLINE) == CFE_UNDERLINE);
	lf.lfStrikeOut = ( (cf.dwEffects & CFE_STRIKEOUT) == CFE_STRIKEOUT);
	lf.lfItalic = ( (cf.dwEffects & CFE_ITALIC) == CFE_ITALIC);
	
	lf.lfWidth = 0;
	memcpy(&lf.lfFaceName, &cf.szFaceName, LF_FACESIZE);
	
	//save color separately because LOGFONT does not accept color
	cr = cf.crTextColor;	
}

VOID Preview::DrawPreset(HDC &_hDc)
{
	if (strncmp(m_s8PresetName, "", MAX_NAME_LEN) == 0)
    {
		return;
    }
	
    COLORREF color;
	color = RGB(255, 255, 255);
	SetBkMode(_hDc, TRANSPARENT);
	SetTextColor(_hDc,color);

	std::string m_strNowPreset = m_s8PresetName; 
	std::string strFirstRow  = m_s8PresetName; 
	std::string strSecondRow = "";

	if (strlen(m_s8PresetName) > 32)
	{
		S32 s32Split = split_name(m_s8PresetName, 32);
		strFirstRow = m_strNowPreset.substr(0, s32Split);
		strSecondRow = m_strNowPreset.substr(s32Split, strlen(m_s8PresetName));
	}

	RECT rcWnd;
	::GetClientRect(m_hWnd, &rcWnd);

	S32 s32Count = m_u8WndNum;
//	m_cfPreset.yHeight = 760 / (sqrt(s32Count));
	m_cfPreset.yHeight = rcWnd.bottom-rcWnd.top;

	LOGFONT lf;
	CharformatToLogfont(m_cfPreset, lf, color);//获取字体设置参数

	HFONT hf = CreateFontIndirect(&lf); //创建字体 
    HGDIOBJ hOldFont = SelectObject(_hDc, hf);

	S32 s32TextX = 10;
    if (m_u16Px != 0)
    {
        s32TextX = m_u16Px;
    }

	S32 s32TextY = rcWnd.bottom-rcWnd.top+2*lf.lfHeight;
	if (m_u16Py != 0)
	{
        s32TextY = m_u16Py;
	}
		
	//输出第一行
	TextOut(_hDc, s32TextX, s32TextY, strFirstRow.c_str(), strFirstRow.length());
	//输出第二行
	if (strSecondRow != "")
	{
		TextOut(_hDc, s32TextX, s32TextY+lf.lfHeight, strSecondRow.c_str(), strSecondRow.length());
	}
	
	DeleteObject(hf);
	SelectObject(_hDc, hOldFont);
}

VOID Preview::SetRegDraw(HANDLE hDraw, HANDLE hUser)
{
	m_stDrawParam.hDraw = hDraw;
	m_stDrawParam.hUser = hUser;
}

VOID Preview::SetDecDataCallBack(HANDLE hDecCb, HANDLE hUser)
{
	m_stDecParam.hDecCb = hDecCb;
	m_stDecParam.hUser  = hUser;
}
#endif
