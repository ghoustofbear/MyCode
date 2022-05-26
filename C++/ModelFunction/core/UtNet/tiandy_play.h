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

/***********  系统创建与释放  ********************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_CreateSystem(HWND _hWnd);
TD_PLAYSDK_API int CALLING_CONVENTION TC_DeleteSystem();

/***********   创建和关闭播放实例  **************************************/
TD_PLAYSDK_API int CALLING_CONVENTION  TC_CreatePlayerFromFile(
				HWND  _hWnd, 
				char* _pcFileName,
				int   _iDownloadBufSz = 0,    /*下载缓冲区大小*/
				int   _iFileTrueSz    = 0,    /*文件大小*/
				int*  _piNowSz        = NULL, /*当前文件大小*/
				int   _iLastFrmNo     = 0,    /*最后一帧帧序号*/
				int*  _piCompleteFlag = NULL  /*是否结束接收*/);
TD_PLAYSDK_API int CALLING_CONVENTION TC_DeletePlayer(int _iID);
TD_PLAYSDK_API int CALLING_CONVENTION TC_DeletePlayerEx(int _iID, int _iParam);		//20110705-->hyq  停止播放视频时可以设置选项，如:是否保留最后一帧图像

/**********  注册消息和回调  ******************************************/
TD_PLAYSDK_API void CALLING_CONVENTION TC_RegisterEventMsg(HWND _hEventWnd, UINT _uiEventMsg = MSG_PLAYSDKM4);
TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterNotifyPlayToEnd(void* _PlayEndFun);
TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterNotifyGetDecAV(int _iID, void* _GetDecAVCbk, bool _blDisplay);
//TD_PLAYSDK_API int __cdecl TC_RegisterDrawFun(int _iID, int (__cdecl*)(int, HDC, long), long);
TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterDrawFun(int _iID, pfCBDrawFun _pfDrawFun, long _lUserData);
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetDecCallBack(int _iID, DECYUV_NOTIFY_V4 _cbkGetYUV, int _iUserData);
/************************************************************************/
/* 设置画图回调
_lUserData ： 用户数据
_lpCmd     ： 目前传 int 指针,用于标识回调的HDC是否按窗口大小缩放 0：不缩放 1：缩放  默认为1
_iLen      ： _lpCmd 大小
/************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterDrawFunEx(int _iID, pfCBDrawFun _pfDrawFun, long _lUserData, void *_pCmd, int _iCmdLen);


/**********  播放控制    *********************************************/
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

/*********  获取播放信息   **********************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetFrameRate(int _iID);						//	得到当前的帧率
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetPlayingFrameNum(int _iID);				//	得到当前播放的总帧数
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetFrameCount(int _iID);						//	得到帧的总个数
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetBeginEnd(int _iID, int* _piBegin, int* _piEnd);			
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetFileName(int _iID, char* _pcFileName);
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetVideoParam(int _iID, OUT int* _pWidth, OUT int *_pHeight, OUT int *_pFrameRate);
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetPlayTime(int _iID);

/********   抓拍    ************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_CapturePic(int _iID, unsigned char** _ppucYUV);
TD_PLAYSDK_API int CALLING_CONVENTION TC_CaptureBmpPic(int _iID, char* _pcSaveFile);
 
/********   流播放  ***************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_CreatePlayerFromStream(HWND _hWnd, unsigned char* _pucVideoHeadBuf, int _iHeadSize);
TD_PLAYSDK_API int CALLING_CONVENTION TC_CreatePlayerFromStreamEx(HWND _hWnd, unsigned char* _pucVideoHeadBuf, int _iHeadSize, int _iStreamBufferSize);
TD_PLAYSDK_API int CALLING_CONVENTION TC_PutStreamToPlayer(int _iID, unsigned char* _pucStreamData, int _iSize);
TD_PLAYSDK_API int CALLING_CONVENTION TC_CleanStreamBuffer(int _iID);

TD_PLAYSDK_API int CALLING_CONVENTION TC_CreatePlayerFromVoD(HWND _hWnd, unsigned char* _pucVideoHeadBuf, int _iHeadSize);
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetFrameListBufferSize(int _iID, int _iBufSize, int _iMaxFrameCount = -1);
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetStreamBufferSize(int _iID, int _iBufSize);
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetVoDPlayerOver(int _iID);
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetCleanVoDBuffer(int _iID);

TD_PLAYSDK_API int CALLING_CONVENTION TC_EnablePlayStreamCtrl(int _iID, bool _blCtrl);			//  2010-9-20-10:59 @yys@，从定制版SDK中移植过来

/*********   CPU监控   *******************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_StartMonitorCPU();
TD_PLAYSDK_API int CALLING_CONVENTION TC_StopMonitorCPU();

/*********  获取版本信息   *********************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetVersion(SDK_VERSION* _ver);

/*********  视频文件编辑  ***************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_Locate(int _iID, int* _piFrameNo);
TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterVECallBack(void* _pVECallBack);
TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterVECallBackEx(pfCBVideoEdit _pVECallBack);		//	void *pf(int)
TD_PLAYSDK_API int CALLING_CONVENTION TC_SegmentToFile(char* _pcSaveFile, char* _pcSourceFile, int _iBeginFrmNo, int _iEndFrmNo);
TD_PLAYSDK_API int CALLING_CONVENTION TC_Combine(char* _pcSaveFile, char* _pcSource1, int _iBeginFrmNo1, int _iEndFrmNo1, char* _pcSource2, int _iBeginFrmNo2, int _iEndFrmNo2);
TD_PLAYSDK_API int CALLING_CONVENTION TC_RemoveAudio(char* _pcSaveFile, char* _pcSourceFile, int _iBeginFrmNo, int _iEndFrmNo);
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetVEState(int* _piState, int* _piProgress);
TD_PLAYSDK_API int CALLING_CONVENTION TC_VEAbort();
TD_PLAYSDK_API int CALLING_CONVENTION TC_VECleanup();

// 设置规则，专用于特殊情况，比如流数据头不正确的情况下，如果设置了_iModeRule,则会根据解码出来的数据自动调整绘图实例
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetModeRule(int _iID, int _iModeRule);
// 切换显示屏幕
TD_PLAYSDK_API int CALLING_CONVENTION TC_ResetPlayWnd(int _iID, HWND _hWnd);

/********   电子放大  ****************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_DrawRect(int _iID, RECT *_rcDraw, int _iCount);
TD_PLAYSDK_API int CALLING_CONVENTION TC_AddEZoom(int _iID, HWND _hWnd, RECT* _rcZoom, int _iCount);
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetEZoom(int _iID, int _iZoomID, RECT _pRectInVideo);
TD_PLAYSDK_API int CALLING_CONVENTION TC_RemoveEZoom(int _iID, int _iZoomID);
TD_PLAYSDK_API int CALLING_CONVENTION TC_ResetEZoom(int _iID, int _iZoomID);

/************************************************************************
* 多屏显示的相关接口                                                                     
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_ChangeMonitor(int _iID, int _iMonitorIndex);
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetMonitorCount();
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetMonitorInfo(int _iMonitorIndex, MONITOR_INFO* _pMonitorInfo);
TD_PLAYSDK_API int CALLING_CONVENTION TC_UpdateMonitor();

/************************************************************************
* 得到录像中的VCA信息                                                                     
************************************************************************/

/************************************************************************
*   按照一定规则搜索                                                                   
*	_iID:					Player ID
	_iSearchFlag			搜索规则,  1：周界 2：绊线
	_piFrameStart			开始帧号
	_pParamBuf				输入参数缓冲区，1：周界信息结构体， 2：绊线信息结构体
	_iBufSize				缓冲区大小，    1：周界结构体大小， 2：绊线结构体大小
	_iContinuePlay          成功定位后是否继续播放 
	                        0：停止       1： 继续   
	_puiFrameResult         成功定位的帧号
*	返回值：
					>=0		成功
					<0		失败
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_SearchVCAInfo(int _iID, int _iSearchRule, int _iFrameStart, void* _pParamBuf, int _iBufSize, int _iContinuePlay, unsigned int* _puiFrameResult);

TD_PLAYSDK_API int CALLING_CONVENTION TC_GetBeginEndTimeStamp(int _iID, unsigned int * _puiBeginTimeStamp, unsigned int * _puiEndTimeStamp);

/************************************************************************
* 设置和得到解密密码
* 参数说明：
		_iID				PlayerID
		_lpBuf				Decrypt Key Buffer
		_iBufSize			Buffer size (<=16)
*	返回值：
		>=0		成功
		<0		失败
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetVideoDecryptKey(int _iID, void* _lpBuf, int _iBufSize);
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetVideoDecryptKey(int _iID, void* _lpBuf, int _iBufSize);

/************************************************************************
*	通用的回调函数，目前用于需要解密或解密密码错误时，对上层应用程序进行通知                                                                     
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterCommonEventCallBack(pfCBCommonEventNotifyEx _pf);
TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterCommonEventCallBackEx(pfCBCommonEventNotifyEx _pf, void* _lpUserData);
TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterCommonEventCallBack_V4(int _iID, pfCBCommonEventNotifyEx _pf, void* _lpUserData);

/************************************************************************
* 根据播放ID设置和得到相对应音频的音量
* 参数说明：
		_iID				PlayerID
		_usVolume           要设置的音量值
		_pusVolume          获得音量值的存储地址
*	返回值：
		>=0		成功
		<0		失败
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetAudioVolumeEx(int _iID, unsigned short _usVolume);
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetAudioVolumeEx(int _iID, unsigned short * _pusVolume);

/************************************************************************
* 在本地视频上画多边形
* 参数说明：
		_iID				PlayerID
		_pPointArray        多边形顶点数组的指针
		_iPointCount        多边形顶点的个数
		_iFlag				byte0:是否允许画箭头 byte1:是否增加多边形  byte2:只修改上一次画的多边形
*	返回值：
		>=0		成功
		<0		失败
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_DrawPolygonOnLocalVideo(int _iID, POINT* _pPointArray, int _iPointCount, int _iFlag);

TD_PLAYSDK_API int CALLING_CONVENTION TC_GetStreamPlayBufferState(int _iID, int * _piStreamBufferState);

/************************************************************************/
/* 获得帧用户数据包括GET_USERDATA_INFO_TIME、GET_USERDATA_INFO_OSD、GET_USERDATA_INFO_GPS、GET_USERDATA_INFO_VCA 
	_iFrameNO： -2 第一个I帧时间绝对时间 ,-1 当前播放帧绝对时间
*/
/************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetUserDataInfo(int _iID, int _iFrameNO, int _iFlag, void*  _Buffer, int _iSize);

/************************************************************************
* Ps流封装回调函数设置                                                                     
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterPsPackCallBack( PSPACK_NOTIFY _cbkPsPackNotify, void* _pContext /*= NULL*/ );

/************************************************************************
* 
功  能：				   得到虚拟帧信息  
参  数：
_iID					   播放器ID
_pData		               打点帧
_iSize					   打点帧长度 sizeof
返回值：打点帧个数		   如果_puiData == NULL 或者 _iSize < 4 查询打点帧个数
>=0						   操作成功
-1						   操作失败

如果_puiData不能容纳所有打点帧，就只返回前_iSize/sizeof(int)个                                                                   
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetMarkInfo(int _iID,PVOID _pData = NULL,int _iSize = 0);

/************************************************************************
* 
功  能：添加或删除一个打点信息
参  数：
_iID					   播放器ID
_iFrameNO				   打点帧帧号
返回值：新添打点I帧帧号		   
>=0						   操作成功
-1						   操作失败  

如果_iFrameNO <= 0添加离当前帧最近的I帧
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetMarkInfo(int _iID,int _iFrameNO,int _iType);

/************************************************************************
* 
功  能：定位到一个标记
参  数：
_iID					   播放器ID
_iFrameNO				   打点帧帧号
返回值：		   
>=0						   操作成功
-1						   操作失败  

************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_SeekMark(int _iID,int _iFrameNO);

TD_PLAYSDK_API int CALLING_CONVENTION TC_SetMarkInfo(int _iID,int _iFrameNO,int _iType);

/************************************************************************
* 
功  能：设置音频播放参数
参  数：
_iID					   播放器ID
_iChannel                  通道1为单声道，2为立体声  
_iSamplePerSec;            采样频率8000 	   
_iBitsPerSample;           采样数据位16>=0	

返回值：		   
>=0						   操作成功
-1						   操作失败  


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
功  能：				   注册原始码流的回调函数  
参  数：
_iID					   播放器ID
_pFun		               回调函数
_pvParam				   用户数据
返回值：				   是否注册成功
>=0						   操作成功
-1						   操作失败                                                                
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterRawFrameCallback(int _iID, void* _pFun, void* _pvParam);

/************************************************************************
* 
功  能：				   设置垂直同步参数。开启垂直同步可以防止裂屏，但是会消耗大量的Cpu
参  数：
_iID					   播放器ID
_iFlag		               同步标示：0关闭 1开启
返回值：				   
>=0						   操作成功
-1						   操作失败                                                                
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_SetVerticalSync(int _iID,int _iFlag);

/************************************************************************
* 
功  能：					按照cmd中不同的需求创建播放器
参  数：
	_iCmd					命令码
	_pvCmdBuf				命令参数buf
	_iBufLen				buf大小
	_pvUserData				用户数据
返回值：				   
>=0							操作成功
-1							操作失败                                                                
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_CreatePlayerForSpecialNeeds(int _iCmd, void* _pvCmdBuf, int _iBufLen, void* _pvUserData);

/************************************************************************/
/*  
*/
/************************************************************************/

TD_PLAYSDK_API int CALLING_CONVENTION TC_PutFrameToPlayer(int _iID, unsigned char* _pucStreamData, int _iSize);

/************************************************************************
* 
功  能：				   获得垂直同步参数
参  数：
_iID					   播放器ID
_piFlag		               同步标示：0关闭 1开启
返回值：				   
>=0						   操作成功
-1						   操作失败                                                                
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_GetVerticalSync(int _iID,int* _piFlag);

/************************************************************************
* 
功  能：				   清除本地已画多边形
参  数：
_iID					   播放器ID
_iPolygonIndex		       Polygon Index
返回值：				   
>=0						   操作成功
-1						   操作失败                                                                
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_ClearPolyLocalVideo(int _iID, int _iPolygonIndex);

/************************************************************************
* 
功  能：				   采集音频数据
参  数：
_iEnable				   开关状态 0：关闭 1：开启
返回值：				   
>=0						   操作成功
-1						   操作失败                                                                
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_RecordAudio(int _iEnable);

/************************************************************************
* 
功  能：				   音频采集回调注册函数
参  数：
_cbkGetVoice			   回调函数
返回值：				   
>=0						   操作成功
-1						   操作失败                                                                
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_RegisterNotifyGetVoice(RECORD_VOICE_NOTIFY _cbkGetVoice);

/************************************************************************
* 
功  能：				   多窗口播放
参  数：
_iID					   播放器ID
_hWnd		               窗口句柄
返回值：				   
>=0						   操作成功
-1						   操作失败                                                                
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION  TC_CreateDuplicatePlayer(int _iID, HWND  _hWnd);

/************************************************************************
* 
功  能：				   绘制字符串
参  数：
_iID					   播放器ID
_iX						   X坐标
_iY						   Y坐标
_cText					   要显示的字符串
_iColor					   字符串颜色
返回值：				   
>=0						   操作成功
-1						   操作失败                                                                
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION  TC_DrawText(int _iID,int _iX, int _iY, char* _cText, int _iColor);

// TD_PLAYSDK_API int CALLING_CONVENTION  TC_CreatePlayerMaxVideoSize(HWND  _hWnd, int _iPlayMode, void *_pParam, int _iParamSize);

/************************************************************************
* 
功  能：				   创建播放器
参  数：			
_iType					   播放器类型  PlayModeCmd
_pvCmdBuf				   创建播放器所需参数
_iBufLen				   _pvCmdBuf大小
_pvUserData				   用户数据

返回值：播放器ID
>=0						   成功
<0						   失败                                                                
************************************************************************/
TD_PLAYSDK_API int CALLING_CONVENTION TC_CreatePlayer(int _iType, void* _pvCmdBuf, int _iBufLen, void* _pvUserData);


TD_PLAYSDK_API int CALLING_CONVENTION TC_VODPlayerControl(int _iID, int _iCmd, void* _pvCmdBuf, int _iBufLen, void* _pvUserData);

TD_PLAYSDK_API int CALLING_CONVENTION TC_RefreshPlay(int _iID, int _iCmd, void* _pvCmdBuf, int _iBufLen);

TD_PLAYSDK_API int CALLING_CONVENTION TC_SetFileHeader(int _iID, unsigned char *_pFileHeader, int _iHeaderLen);
#endif
