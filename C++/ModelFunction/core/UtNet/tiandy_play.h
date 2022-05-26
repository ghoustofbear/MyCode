#ifndef PLAYSDKM4_INTERFACE_H
#define PLAYSDKM4_INTERFACE_H

#include "PublicDef.h"

//#define  CALLING_CONVENTION		__cdecl
//#define  CALLING_CONVENTION		__stdcall

//////////////////////////////////////////////////////////////////////////////////////
//////////////////////////////////////////////////////////////////////////////////////

#define PLAYSDKM4_EXPORTS

#ifdef PLAYSDKM4_EXPORTS
#define TD_PLAYSDK_API  extern "C"__declspec(dllexport) 
#else
#define TD_PLAYSDK_API __declspec(dllimport)
#endif

/***********  ϵͳ�������ͷ�  ********************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_CreateSystem(HWND _hWnd);
TD_PLAYSDK_API int CALLING_CONVENTION TC_DeleteSystem();

/***********   �����͹رղ���ʵ��  **************************************/
TD_PLAYSDK_API int CALLING_CONVENTION  TC_CreatePlayerFromFile(
				HWND  _hWnd, 
				char* _pcFileName,
				int   _iDownloadBufSz = 0,    /*���ػ�������С*/
				int   _iFileTrueSz    = 0,    /*�ļ���С*/
				int*  _piNowSz        = NULL, /*��ǰ�ļ���С*/
				int   _iLastFrmNo     = 0,    /*���һ֡֡���*/
				int*  _piCompleteFlag = NULL  /*�Ƿ��������*/);
TD_PLAYSDK_API int CALLING_CONVENTION TC_DeletePlayer(int _iID);
TD_PLAYSDK_API int CALLING_CONVENTION TC_DeletePlayerEx(int _iID, int _iParam);		//20110705-->hyq  ֹͣ������Ƶʱ��������ѡ���:�Ƿ������һ֡ͼ��

/**********  ע����Ϣ�ͻص�  ******************************************/
TD_PLAYSDK_API void CALLING_CONVENTION TC_RegisterEventMsg(HWND _hEventWnd, UINT _uiEventMsg = MSG_PLAYSDKM4);
TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterNotifyPlayToEnd(void* _PlayEndFun);
TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterNotifyGetDecAV(int _iID, void* _GetDecAVCbk, bool _blDisplay);
//TD_PLAYSDK_API int __cdecl TC_RegisterDrawFun(int _iID, int (__cdecl*)(int, HDC, long), long);
TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterDrawFun(int _iID, pfCBDrawFun _pfDrawFun, long _lUserData);
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetDecCallBack(int _iID, DECYUV_NOTIFY_V4 _cbkGetYUV, int _iUserData);
/************************************************************************/
/* ���û�ͼ�ص�
_lUserData �� �û�����
_lpCmd     �� Ŀǰ�� int ָ��,���ڱ�ʶ�ص���HDC�Ƿ񰴴��ڴ�С���� 0�������� 1������  Ĭ��Ϊ1
_iLen      �� _lpCmd ��С
/************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterDrawFunEx(int _iID, pfCBDrawFun _pfDrawFun, long _lUserData, void *_pCmd, int _iCmdLen);


/**********  ���ſ���    *********************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetPlayRect(int _iID, RECT* _pDrawRect);
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetPlayRectEx(int _iID, RECT* _pDrawRect, DWORD _dwMask);

TD_PLAYSDK_API int CALLING_CONVENTION TC_SetSrcRect(int _iID, RECTEX* _pSrc);
TD_PLAYSDK_API int CALLING_CONVENTION TC_Play(int _iID);
TD_PLAYSDK_API int CALLING_CONVENTION TC_Pause(int _iID);
TD_PLAYSDK_API int CALLING_CONVENTION TC_Stop(int _iID);
TD_PLAYSDK_API int CALLING_CONVENTION TC_GoBegin(int _iID);
TD_PLAYSDK_API int CALLING_CONVENTION TC_GoEnd(int _iID);
TD_PLAYSDK_API int CALLING_CONVENTION TC_StepForward(int _iID);
TD_PLAYSDK_API int CALLING_CONVENTION TC_StepBackward(int _iID);
TD_PLAYSDK_API int CALLING_CONVENTION TC_FastForward(int _iID, int _iSpeed);
TD_PLAYSDK_API int CALLING_CONVENTION TC_SlowForward(int _iID, int _iSpeed);
TD_PLAYSDK_API int CALLING_CONVENTION TC_FastBackward(int _iID);
TD_PLAYSDK_API int CALLING_CONVENTION TC_PlayAudio(int _iID);
TD_PLAYSDK_API int CALLING_CONVENTION TC_StopAudio(int _iID);
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetAudioVolumn(unsigned short _ustVolume);
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetAudioVolume(unsigned short _ustVolume);
TD_PLAYSDK_API int CALLING_CONVENTION TC_Seek(int _iID, int _iFrameNo);
TD_PLAYSDK_API int CALLING_CONVENTION TC_SeekEx(int _iID, int _iFrameNo);

/*********  ��ȡ������Ϣ   **********************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetFrameRate(int _iID);						//	�õ���ǰ��֡��
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetPlayingFrameNum(int _iID);				//	�õ���ǰ���ŵ���֡��
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetFrameCount(int _iID);						//	�õ�֡���ܸ���
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetBeginEnd(int _iID, int* _piBegin, int* _piEnd);			
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetFileName(int _iID, char* _pcFileName);
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetVideoParam(int _iID, OUT int* _pWidth, OUT int *_pHeight, OUT int *_pFrameRate);
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetPlayTime(int _iID);

/********   ץ��    ************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_CapturePic(int _iID, unsigned char** _ppucYUV);
TD_PLAYSDK_API int CALLING_CONVENTION TC_CaptureBmpPic(int _iID, char* _pcSaveFile);
 
/********   ������  ***************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_CreatePlayerFromStream(HWND _hWnd, unsigned char* _pucVideoHeadBuf, int _iHeadSize);
TD_PLAYSDK_API int CALLING_CONVENTION TC_CreatePlayerFromStreamEx(HWND _hWnd, unsigned char* _pucVideoHeadBuf, int _iHeadSize, int _iStreamBufferSize);
TD_PLAYSDK_API int CALLING_CONVENTION TC_PutStreamToPlayer(int _iID, unsigned char* _pucStreamData, int _iSize);
TD_PLAYSDK_API int CALLING_CONVENTION TC_CleanStreamBuffer(int _iID);

TD_PLAYSDK_API int CALLING_CONVENTION TC_CreatePlayerFromVoD(HWND _hWnd, unsigned char* _pucVideoHeadBuf, int _iHeadSize);
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetFrameListBufferSize(int _iID, int _iBufSize, int _iMaxFrameCount = -1);
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetStreamBufferSize(int _iID, int _iBufSize);
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetVoDPlayerOver(int _iID);
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetCleanVoDBuffer(int _iID);

TD_PLAYSDK_API int CALLING_CONVENTION TC_EnablePlayStreamCtrl(int _iID, bool _blCtrl);			//  2010-9-20-10:59 @yys@���Ӷ��ư�SDK����ֲ����

/*********   CPU���   *******************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_StartMonitorCPU();
TD_PLAYSDK_API int CALLING_CONVENTION TC_StopMonitorCPU();

/*********  ��ȡ�汾��Ϣ   *********************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetVersion(SDK_VERSION* _ver);

/*********  ��Ƶ�ļ��༭  ***************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_Locate(int _iID, int* _piFrameNo);
TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterVECallBack(void* _pVECallBack);
TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterVECallBackEx(pfCBVideoEdit _pVECallBack);		//	void *pf(int)
TD_PLAYSDK_API int CALLING_CONVENTION TC_SegmentToFile(char* _pcSaveFile, char* _pcSourceFile, int _iBeginFrmNo, int _iEndFrmNo);
TD_PLAYSDK_API int CALLING_CONVENTION TC_Combine(char* _pcSaveFile, char* _pcSource1, int _iBeginFrmNo1, int _iEndFrmNo1, char* _pcSource2, int _iBeginFrmNo2, int _iEndFrmNo2);
TD_PLAYSDK_API int CALLING_CONVENTION TC_RemoveAudio(char* _pcSaveFile, char* _pcSourceFile, int _iBeginFrmNo, int _iEndFrmNo);
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetVEState(int* _piState, int* _piProgress);
TD_PLAYSDK_API int CALLING_CONVENTION TC_VEAbort();
TD_PLAYSDK_API int CALLING_CONVENTION TC_VECleanup();

// ���ù���ר�����������������������ͷ����ȷ������£����������_iModeRule,�����ݽ�������������Զ�������ͼʵ��
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetModeRule(int _iID, int _iModeRule);
// �л���ʾ��Ļ
TD_PLAYSDK_API int CALLING_CONVENTION TC_ResetPlayWnd(int _iID, HWND _hWnd);

/********   ���ӷŴ�  ****************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_DrawRect(int _iID, RECT *_rcDraw, int _iCount);
TD_PLAYSDK_API int CALLING_CONVENTION TC_AddEZoom(int _iID, HWND _hWnd, RECT* _rcZoom, int _iCount);
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetEZoom(int _iID, int _iZoomID, RECT _pRectInVideo);
TD_PLAYSDK_API int CALLING_CONVENTION TC_RemoveEZoom(int _iID, int _iZoomID);
TD_PLAYSDK_API int CALLING_CONVENTION TC_ResetEZoom(int _iID, int _iZoomID);

/************************************************************************
* ������ʾ����ؽӿ�                                                                     
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_ChangeMonitor(int _iID, int _iMonitorIndex);
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetMonitorCount();
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetMonitorInfo(int _iMonitorIndex, MONITOR_INFO* _pMonitorInfo);
TD_PLAYSDK_API int CALLING_CONVENTION TC_UpdateMonitor();

/************************************************************************
* �õ�¼���е�VCA��Ϣ                                                                     
************************************************************************/

/************************************************************************
*   ����һ����������                                                                   
*	_iID:					Player ID
	_iSearchFlag			��������,  1���ܽ� 2������
	_piFrameStart			��ʼ֡��
	_pParamBuf				���������������1���ܽ���Ϣ�ṹ�壬 2��������Ϣ�ṹ��
	_iBufSize				��������С��    1���ܽ�ṹ���С�� 2�����߽ṹ���С
	_iContinuePlay          �ɹ���λ���Ƿ�������� 
	                        0��ֹͣ       1�� ����   
	_puiFrameResult         �ɹ���λ��֡��
*	����ֵ��
					>=0		�ɹ�
					<0		ʧ��
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_SearchVCAInfo(int _iID, int _iSearchRule, int _iFrameStart, void* _pParamBuf, int _iBufSize, int _iContinuePlay, unsigned int* _puiFrameResult);

TD_PLAYSDK_API int CALLING_CONVENTION TC_GetBeginEndTimeStamp(int _iID, unsigned int * _puiBeginTimeStamp, unsigned int * _puiEndTimeStamp);

/************************************************************************
* ���ú͵õ���������
* ����˵����
		_iID				PlayerID
		_lpBuf				Decrypt Key Buffer
		_iBufSize			Buffer size (<=16)
*	����ֵ��
		>=0		�ɹ�
		<0		ʧ��
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetVideoDecryptKey(int _iID, void* _lpBuf, int _iBufSize);
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetVideoDecryptKey(int _iID, void* _lpBuf, int _iBufSize);

/************************************************************************
*	ͨ�õĻص�������Ŀǰ������Ҫ���ܻ�����������ʱ�����ϲ�Ӧ�ó������֪ͨ                                                                     
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterCommonEventCallBack(pfCBCommonEventNotifyEx _pf);
TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterCommonEventCallBackEx(pfCBCommonEventNotifyEx _pf, void* _lpUserData);
TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterCommonEventCallBack_V4(int _iID, pfCBCommonEventNotifyEx _pf, void* _lpUserData);

/************************************************************************
* ���ݲ���ID���ú͵õ����Ӧ��Ƶ������
* ����˵����
		_iID				PlayerID
		_usVolume           Ҫ���õ�����ֵ
		_pusVolume          �������ֵ�Ĵ洢��ַ
*	����ֵ��
		>=0		�ɹ�
		<0		ʧ��
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetAudioVolumeEx(int _iID, unsigned short _usVolume);
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetAudioVolumeEx(int _iID, unsigned short * _pusVolume);

/************************************************************************
* �ڱ�����Ƶ�ϻ������
* ����˵����
		_iID				PlayerID
		_pPointArray        ����ζ��������ָ��
		_iPointCount        ����ζ���ĸ���
		_iFlag				byte0:�Ƿ�������ͷ byte1:�Ƿ����Ӷ����  byte2:ֻ�޸���һ�λ��Ķ����
*	����ֵ��
		>=0		�ɹ�
		<0		ʧ��
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_DrawPolygonOnLocalVideo(int _iID, POINT* _pPointArray, int _iPointCount, int _iFlag);

TD_PLAYSDK_API int CALLING_CONVENTION TC_GetStreamPlayBufferState(int _iID, int * _piStreamBufferState);

/************************************************************************/
/* ���֡�û����ݰ���GET_USERDATA_INFO_TIME��GET_USERDATA_INFO_OSD��GET_USERDATA_INFO_GPS��GET_USERDATA_INFO_VCA 
	_iFrameNO�� -2 ��һ��I֡ʱ�����ʱ�� ,-1 ��ǰ����֡����ʱ��
*/
/************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetUserDataInfo(int _iID, int _iFrameNO, int _iFlag, void*  _Buffer, int _iSize);

/************************************************************************
* Ps����װ�ص���������                                                                     
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterPsPackCallBack( PSPACK_NOTIFY _cbkPsPackNotify, void* _pContext /*= NULL*/ );

/************************************************************************
* 
��  �ܣ�				   �õ�����֡��Ϣ  
��  ����
_iID					   ������ID
_pData		               ���֡
_iSize					   ���֡���� sizeof
����ֵ�����֡����		   ���_puiData == NULL ���� _iSize < 4 ��ѯ���֡����
>=0						   �����ɹ�
-1						   ����ʧ��

���_puiData�����������д��֡����ֻ����ǰ_iSize/sizeof(int)��                                                                   
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetMarkInfo(int _iID,PVOID _pData = NULL,int _iSize = 0);

/************************************************************************
* 
��  �ܣ���ӻ�ɾ��һ�������Ϣ
��  ����
_iID					   ������ID
_iFrameNO				   ���֡֡��
����ֵ��������I֡֡��		   
>=0						   �����ɹ�
-1						   ����ʧ��  

���_iFrameNO <= 0����뵱ǰ֡�����I֡
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetMarkInfo(int _iID,int _iFrameNO,int _iType);

/************************************************************************
* 
��  �ܣ���λ��һ�����
��  ����
_iID					   ������ID
_iFrameNO				   ���֡֡��
����ֵ��		   
>=0						   �����ɹ�
-1						   ����ʧ��  

************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_SeekMark(int _iID,int _iFrameNO);

TD_PLAYSDK_API int CALLING_CONVENTION TC_SetMarkInfo(int _iID,int _iFrameNO,int _iType);

/************************************************************************
* 
��  �ܣ�������Ƶ���Ų���
��  ����
_iID					   ������ID
_iChannel                  ͨ��1Ϊ��������2Ϊ������  
_iSamplePerSec;            ����Ƶ��8000 	   
_iBitsPerSample;           ��������λ16>=0	

����ֵ��		   
>=0						   �����ɹ�
-1						   ����ʧ��  


************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetAudioPlayParam(int _iID,int _iChannel, int _iSamplePerSec, int _iBitsPerSample);


TD_PLAYSDK_API int CALLING_CONVENTION TC_StartCaptureFile(int _iID, char* _cFileName,int _iRecFileType);

TD_PLAYSDK_API int CALLING_CONVENTION TC_StopCaptureFile(int _iID);

//  [11/11/2011 hanyongqiang]

TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterNotifyAudioCapture(void* _pvAudioCaptureFun,void* _pvContext);

TD_PLAYSDK_API int CALLING_CONVENTION TC_StartAudioCapture(int _iID, int _iWavein,int _iWaveout);

TD_PLAYSDK_API int CALLING_CONVENTION TC_StopAudioCapture(int _iID);

//  [11/14/2011 hanyongqiang]
TD_PLAYSDK_API int CALLING_CONVENTION TC_PackStreamToRawStream(unsigned char* _pucSrc, int _iSrcLen, unsigned char* _pucDst, int _iDstLen, int _iType, void* _pvReserved);
TD_PLAYSDK_API int CALLING_CONVENTION TC_RawStreamToPackStream(unsigned char* _pucSrc, int _iSrcLen, unsigned char* _pucDst, int _iDstLen, int _iType, void* _pvReserved);

/************************************************************************
* 
��  �ܣ�				   ע��ԭʼ�����Ļص�����  
��  ����
_iID					   ������ID
_pFun		               �ص�����
_pvParam				   �û�����
����ֵ��				   �Ƿ�ע��ɹ�
>=0						   �����ɹ�
-1						   ����ʧ��                                                                
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterRawFrameCallback(int _iID, void* _pFun, void* _pvParam);

/************************************************************************
* 
��  �ܣ�				   ���ô�ֱͬ��������������ֱͬ�����Է�ֹ���������ǻ����Ĵ�����Cpu
��  ����
_iID					   ������ID
_iFlag		               ͬ����ʾ��0�ر� 1����
����ֵ��				   
>=0						   �����ɹ�
-1						   ����ʧ��                                                                
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetVerticalSync(int _iID,int _iFlag);

/************************************************************************
* 
��  �ܣ�					����cmd�в�ͬ�����󴴽�������
��  ����
	_iCmd					������
	_pvCmdBuf				�������buf
	_iBufLen				buf��С
	_pvUserData				�û�����
����ֵ��				   
>=0							�����ɹ�
-1							����ʧ��                                                                
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_CreatePlayerForSpecialNeeds(int _iCmd, void* _pvCmdBuf, int _iBufLen, void* _pvUserData);

/************************************************************************/
/*  
*/
/************************************************************************/

TD_PLAYSDK_API int CALLING_CONVENTION TC_PutFrameToPlayer(int _iID, unsigned char* _pucStreamData, int _iSize);

/************************************************************************
* 
��  �ܣ�				   ��ô�ֱͬ������
��  ����
_iID					   ������ID
_piFlag		               ͬ����ʾ��0�ر� 1����
����ֵ��				   
>=0						   �����ɹ�
-1						   ����ʧ��                                                                
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetVerticalSync(int _iID,int* _piFlag);

/************************************************************************
* 
��  �ܣ�				   ��������ѻ������
��  ����
_iID					   ������ID
_iPolygonIndex		       Polygon Index
����ֵ��				   
>=0						   �����ɹ�
-1						   ����ʧ��                                                                
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_ClearPolyLocalVideo(int _iID, int _iPolygonIndex);

/************************************************************************
* 
��  �ܣ�				   �ɼ���Ƶ����
��  ����
_iEnable				   ����״̬ 0���ر� 1������
����ֵ��				   
>=0						   �����ɹ�
-1						   ����ʧ��                                                                
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_RecordAudio(int _iEnable);

/************************************************************************
* 
��  �ܣ�				   ��Ƶ�ɼ��ص�ע�ắ��
��  ����
_cbkGetVoice			   �ص�����
����ֵ��				   
>=0						   �����ɹ�
-1						   ����ʧ��                                                                
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterNotifyGetVoice(RECORD_VOICE_NOTIFY _cbkGetVoice);

/************************************************************************
* 
��  �ܣ�				   �ര�ڲ���
��  ����
_iID					   ������ID
_hWnd		               ���ھ��
����ֵ��				   
>=0						   �����ɹ�
-1						   ����ʧ��                                                                
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION  TC_CreateDuplicatePlayer(int _iID, HWND  _hWnd);

/************************************************************************
* 
��  �ܣ�				   �����ַ���
��  ����
_iID					   ������ID
_iX						   X����
_iY						   Y����
_cText					   Ҫ��ʾ���ַ���
_iColor					   �ַ�����ɫ
����ֵ��				   
>=0						   �����ɹ�
-1						   ����ʧ��                                                                
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION  TC_DrawText(int _iID,int _iX, int _iY, char* _cText, int _iColor);

// TD_PLAYSDK_API int CALLING_CONVENTION  TC_CreatePlayerMaxVideoSize(HWND  _hWnd, int _iPlayMode, void *_pParam, int _iParamSize);

/************************************************************************
* 
��  �ܣ�				   ����������
��  ����			
_iType					   ����������  PlayModeCmd
_pvCmdBuf				   �����������������
_iBufLen				   _pvCmdBuf��С
_pvUserData				   �û�����

����ֵ��������ID
>=0						   �ɹ�
<0						   ʧ��                                                                
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_CreatePlayer(int _iType, void* _pvCmdBuf, int _iBufLen, void* _pvUserData);


TD_PLAYSDK_API int CALLING_CONVENTION TC_VODPlayerControl(int _iID, int _iCmd, void* _pvCmdBuf, int _iBufLen, void* _pvUserData);

TD_PLAYSDK_API int CALLING_CONVENTION TC_RefreshPlay(int _iID, int _iCmd, void* _pvCmdBuf, int _iBufLen);

TD_PLAYSDK_API int CALLING_CONVENTION TC_SetFileHeader(int _iID, unsigned char *_pFileHeader, int _iHeaderLen);
#endif
