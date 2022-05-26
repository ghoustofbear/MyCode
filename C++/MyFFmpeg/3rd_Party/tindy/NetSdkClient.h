#ifndef _INTERFACEH_
#define _INTERFACEH_

#include "NetClientTypes.h"

#ifdef WIN32
#define WIN32_LEAN_AND_MEAN 
#include "windows.h"
#else
#define __stdcall 
#endif

#ifdef __cplusplus
extern "C"{
#endif

	int __stdcall NetClient_Startup_V4(int _iServerPort, int _iClientPort, int _iWnd);

	int __stdcall NetClient_SetNotifyFunction_V4(MAIN_NOTIFY_V4        _MainNotify, 
		ALARM_NOTIFY_V4       _AlarmNotify,
		PARACHANGE_NOTIFY_V4  _ParaNotify,
		COMRECV_NOTIFY_V4     _ComNotify,
		PROXY_NOTIFY       _ProxyNotify);
	
	int __stdcall NetClient_StartRecv_V4(unsigned int* _uiRecvID, CLIENTINFO* _cltInfo, NVSDATA_NOTIFY _cbkDataArrive,void* _iUserData);

	int __stdcall NetClient_StartRecv_V5(unsigned int* _puiRecvID, NetClientPara* _ptPara, int _iParaSize);

	int __stdcall NetClient_SetNotifyUserData_V4(int _iLogonID,void* _iUserData);

	int __stdcall NetClient_SetComRecvNotify_V4(COMRECV_NOTIFY_V4 _comNotify);

	int __stdcall NetClient_GetHTTPPort_V4(int _iLogonID, int *_iPort); 

	int __stdcall NetClient_SetHTTPPort_V4(int _iLogonID, int _iPort); 

	int __stdcall NetClient_SetDomainParsePara_V4(int _iLogonID, int _iInterval, char* _cUserID, char* _cPassword, char* _cHost, char* _cDomain1, char* _cDomain2,int _iPort1, int _iPort2); 

	int __stdcall NetClient_GetDomainParsePara_V4(int _iLogonID, int *_iInterval, char* _cUserID, char* _cPassword, char* _cHost, char* _cDomain1, char* _cDomain2,int* _iPort1, int* _iPort2);

	int __stdcall NetClient_GetBitrateOnVideo_V4(unsigned int _ulConID, int* _piX, int* _piY, int* _piEnabled, char *_pcInfo);

	int __stdcall NetClient_SetDecCallBack_V4(unsigned int _ulConID, DECYUV_NOTIFY_V4 _cbkDecYUV, void* _iUserData);

	int __stdcall NetClient_RegisterDrawFun(unsigned int _ulConID, CBK_DRAW_FUNC _pfDrawFun, long _lUserData, void *_pCmd, int _iCmdLen);

	int __stdcall NetClient_SetPort( int _iServerPort, int _iClientPort );

#ifdef WIN32

	int __stdcall NetClient_Startup();

	int __stdcall NetClient_SetNotifyFunction( LOGON_NOTIFY _LogonNotify,ALARM_NOTIFY _AlarmNotify, PARACHANGE_NOTIFY _ParaNotify);

	int __stdcall NetClient_SetNotifyFunctionEx( MAIN_NOTIFY _cbkMainNotify, ALARM_NOTIFY_EX _cbkAlarmNotify, PARACHANGE_NOTIFY_EX _cbkParaChangeNotify, void* _pNotifyUserData );
	
	int __stdcall NetClient_SetMSGHandle( unsigned int _uiMessage, HWND _hWnd, unsigned int _uiParaMsg, unsigned int _uiAlarmMsg );

	int __stdcall NetClient_SetMSGHandleEx( unsigned int _uiMessage, HWND _hWnd, unsigned int _uiParaMsg, unsigned int _uiAlarmMsg );

	int __stdcall NetClient_StartRecv(unsigned int	*_ulConID, CLIENTINFO*	_cltInfo, RECVDATA_NOTIFY	_cbkDataArrive);

	int __stdcall NetClient_StartRecvEx(unsigned int *_ulConID,CLIENTINFO* _cltInfo,RECVDATA_NOTIFY_EX _cbkDataNotifyEx, void* _lpUserData);

	int __stdcall NetClient_SetComRecvNotify(COMRECV_NOTIFY _comNotify); 

	int __stdcall NetClient_GetHTTPPort(int _iLogonID, unsigned short *_iPort); 

	int __stdcall NetClient_SetHTTPPort(int _iLogonID, unsigned short _iPort); 

	int __stdcall NetClient_SetDomainParsePara(int _iLogonID, int _iInterval, char* _cUserID, char* _cPassword, char* _cHost, char* _cDomain1, char* _cDomain2,unsigned short _iPort1, unsigned short _iPort2); 

	int __stdcall NetClient_GetDomainParsePara(int _iLogonID, int *_iInterval, char* _cUserID, char* _cPassword, char* _cHost, char* _cDomain1, char* _cDomain2,unsigned short* _iPort1, unsigned short* _iPort2);

	int __stdcall NetClient_GetBitrateOnVideo(unsigned int	_ulConID, int*	_iX, int* _iY, int* _iEnabled);

	int __stdcall NetClient_SetDecCallBack(unsigned int	_ulConID, DECYUV_NOTIFY	_cbkGetYUV, void* _pContext);

	int __stdcall NetClient_InterTalkStartEx(unsigned int * _uiConnID, int _iLogonID, RECVDATA_NOTIFY_EX _cbkDataArrive, void* _iUserData);
#else
int __stdcall NetClient_Startup(int _iServerPort/*=3000*/, int _iClientPort/*=6000*/, int _iWnd/*=0*/);

int __stdcall NetClient_SetNotifyFunction(MAIN_NOTIFY_V4        _MainNotify, 
										  ALARM_NOTIFY_V4       _AlarmNotify,
										  PARACHANGE_NOTIFY_V4  _ParaNotify,
										  COMRECV_NOTIFY_V4     _ComNotify,
										  PROXY_NOTIFY       _ProxyNotify);

int __stdcall NetClient_StartRecv(unsigned int* _uiRecvID, CLIENTINFO* _cltInfo, NVSDATA_NOTIFY _cbkDataArrive,void* _iUserData);

int __stdcall NetClient_SetNotifyUserData(int _iLogonID,void* _iUserData);

int __stdcall NetClient_SetComRecvNotify(COMRECV_NOTIFY_V4 _comNotify);

int __stdcall NetClient_GetHTTPPort(int _iLogonID, int *_iPort); 

int __stdcall NetClient_SetHTTPPort(int _iLogonID, int _iPort); 

int __stdcall NetClient_SetDomainParsePara(int _iLogonID, int _iInterval, char* _cUserID, char* _cPassword, char* _cHost, char* _cDomain1, char* _cDomain2,int _iPort1, int _iPort2); 

int __stdcall NetClient_GetDomainParsePara(int _iLogonID, int *_iInterval, char* _cUserID, char* _cPassword, char* _cHost, char* _cDomain1, char* _cDomain2,int* _iPort1, int* _iPort2);

int __stdcall NetClient_GetBitrateOnVideo(unsigned int _ulConID, int* _piX, int* _piY, int* _piEnabled, char *_pcInfo);

int __stdcall NetClient_SetDecCallBack(unsigned int _ulID, pfCBGetDecAV _cbkGetYUV, void* _iUserData);

int __stdcall NetClient_SetDecCallBackEx(unsigned int _ulID, DECYUV_NOTIFY _cbkDecYUV, void* _iUserData);

int __stdcall NetClient_InterTalkStartEx(unsigned int * _uiConnID, int _iLogonID, NVSDATA_NOTIFY _cbkDataArrive, void* _iUserData);
#endif

int __stdcall NetClient_Cleanup();

int __stdcall NetClient_GetVersion(SDK_VERSION *_ver);

int __stdcall NetClient_Logon(char* _cProxy,char* _cIP,char* _cUserName,
					char* _cPwd,char* _cProductID,int _iPort);

int __stdcall NetClient_LogonEx(char* _cProxy,char* _cIP,char* _cUserName,
							  char* _cPwd,char* _cProductID,int _iPort,const char* _pcCharSet);

int __stdcall NetClient_Logoff(int _iLogonID);

int __stdcall NetClient_GetLogonStatus(int _iLogonID);

int __stdcall NetClient_ProxyGetDevInfo(int _iLogonID, int _iCmd, void* _pvOutBuf, int _iBufLen);

int __stdcall NetClient_StopRecv(unsigned int _uiRecvID);

unsigned int __stdcall NetClient_GetRecvID(int _iLogonID, int _iChannel, int _iStreamNO);

int __stdcall NetClient_GetInfoByConnectID(unsigned int _iConnectID,st_ConnectInfo* _stConnectInfo);  

int __stdcall NetClient_SetFullStreamNotify (unsigned int _uiRecvID, FULLFRAME_NOTIFY _cbkFullStream); // Set raw frame data callback

int __stdcall NetClient_SetFullStreamNotify_V4 (unsigned int _uiRecvID, FULLFRAME_NOTIFY_V4 _cbkFullStream, void * _iUserData); // Set the raw frame data callback

int __stdcall NetClient_GetCmdString(int _iLogonID,int _iType,int _iPara,CMDSTRING_NOTIFY _cbkCmdString,void* _pUserData);

int __stdcall NetClient_GetDevInfo(int _iLogonID,ENCODERINFO *_pEncoderInfo);

int __stdcall NetClient_SendDataToServer(int _iLogonID,char *_cData,int _iLen);

int __stdcall NetClient_IsValidUser(int _iLogonID,char *_cUserName,char *_cPwd);

int __stdcall NetClient_SetInnerDataNotify(unsigned int _uiRecvID,INNER_DATA_NOTIFY _cbkNotify,void* _iUserData);

int __stdcall NetClient_SetWorkMode(int _iWorkMode);

#ifdef  __WIN__
int __stdcall NetClient_AddActiveServer(char *_cDevIP,char *_cFactoryID,void* _iSocket, void *_pUserData);
int __stdcall NetClient_BindSocket(int _iLogonID,int _iChan,void* _iSocket, void *_pUserData);
#else
int __stdcall NetClient_AddActiveServer(char *_cDevIP,char *_cFactoryID,int _iSocket, void *_pUserData);
int __stdcall NetClient_BindSocket(int _iLogonID,int _iChan,int _iSocket, void *_pUserData);
#endif

int __stdcall NetClient_PushData(int _iLogonID,int _iChan,char *_cData,int _iLen);

int __stdcall NetClient_DelActiveServer(int _iLogonID);

int __stdcall NetClient_StartCaptureData(unsigned long _ulID);

int __stdcall NetClient_StopCaptureData(unsigned long _ulID);

int __stdcall NetClient_GetVideoHeader(unsigned long _ulID,unsigned char *_ucHeader);

int __stdcall NetClient_SetRawFrameCallBack(unsigned int _ulConID, RAWFRAME_NOTIFY _cbkGetFrame, void* _pContext);
int __stdcall NetClient_SetRawFrameCallBackEx(unsigned int _ulConID, RAWFRAME_NOTIFY_EX _cbkGetFrame, void* _pContext);

int __stdcall NetClient_StartCaptureFile(unsigned int _uiRecvID, char* _pszFileName, int _iRecordFileType);

int __stdcall NetClient_StopCaptureFile(unsigned int _uiRecvID);

int __stdcall NetClient_GetCaptureStatus(unsigned int	_ulConID);

int __stdcall NetClient_SetCaptureFileSize(unsigned int _uiRecvID, int _iFileSize);

int __stdcall NetClient_StartPlay(unsigned int _ulID, int _hWnd, RECT _rcShow, unsigned int _uiDecflag);

int __stdcall NetClient_StartPlayEx(unsigned int _ulID, void* _pvBuff, int iBuffSize);

int __stdcall NetClient_StartPlayEs(unsigned int _ulID, int _hWnd);

int __stdcall NetClient_StopPlay(unsigned int _ulID);

int __stdcall NetClient_StopPlayEx(unsigned int _ulID, unsigned int _iParam);

int __stdcall NetClient_SetPlayRectEx(unsigned int _ulID, RECT* _pDrawRect, int _dwMask);

int __stdcall NetClient_SetSrcRect(unsigned int _ulID, void* _pSrcRect);

int __stdcall NetClient_ResetPlayerWnd(unsigned int _ulID, int _hwnd);

int __stdcall NetClient_GetPlayingStatus(unsigned int _ulID);

int __stdcall NetClient_AdjustVideo(unsigned int _ulID, RECT _rctShow);

int __stdcall NetClient_AudioStart(unsigned int _ulID);

int __stdcall NetClient_AudioStop(unsigned int _ulID);

int __stdcall NetClient_SetLocalAudioVolume(int _iVolume);

int __stdcall NetClient_SetBufferNum(unsigned int _ulID, int _iNum);

int __stdcall NetClient_SetPlayDelay(unsigned long _ulID, int _iNum);

int __stdcall NetClient_GetChannelNum(int _iLogonID, int* _piChanNum);

int __stdcall NetClient_GetAlarmPortNum(int _iLogonID, int* _iAlarmInPortNum, int* _iAlarmOutPortNum);

int __stdcall NetClient_GetLocalAlarmNum(int _iLogonID, int* _iLocalAlarmInNum, int* _iLocalAlarmOutNum);

int __stdcall NetClient_SetVideoPara(int _iLogonID, int _iChannelNum, STR_VideoParam* _strVideoParam);

int __stdcall NetClient_GetVideoPara(int _iLogonID, int _iChannelNum, STR_VideoParam* _strVideoParam);

int __stdcall NetClient_SetVideoparaSchedule(int _iLogonID, int _iChannelNum,STR_VideoParam* _strVideoParam[MAX_TIMESEGMENT]);

int __stdcall NetClient_GetVideoparaSchedule(int _iLogonID, int _iChannelNum,STR_VideoParam* _strVideoParam[MAX_TIMESEGMENT]);

int __stdcall NetClient_SetVideoQuality(int _iLogonID, int _iChannelNum, int _iVideoQuality, int _iStreamNO);

int __stdcall NetClient_GetVideoQuality(int _iLogonID,int _iChannelNum,int* _iVideoQuality, int _iStreamNO); 

int __stdcall NetClient_SetFrameRate(int _iLogonID, int _iChannelNum, int _iFrameRate, int _iStreamNO);

int __stdcall NetClient_GetFrameRate(int _iLogonID, int _iChannelNum, int* _iFrameRate, int _iStreamNO); 

int __stdcall NetClient_GetDecordFrameNum (unsigned int _ulConID); //stop the server layer, not implemented

int __stdcall NetClient_SetStreamType(int _iLogonID, int _iChannelNum, int _iStreamType, int _iStreamNO); 

int __stdcall NetClient_GetStreamType(int _iLogonID, int _iChannelNum, int* _iStreamType, int _iStreamNO);

int __stdcall NetClient_SetMotionAreaEnable (int _iLogonID,int _iChannelNum);

int __stdcall NetClient_SetMotionDetetionArea(int _iLogonID, int _iChannelNum, int _ix, int _iy,int _iEnabled);

int __stdcall NetClient_GetMotionDetetionArea(int _iLogonID, int _iChannelNum, int _ix, int _iy,int* _iEnabled);

int __stdcall NetClient_SetThreshold(int _iLogonID, int _iChannelNum,int _iThreshold);

int __stdcall NetClient_GetThreshold(int _iLogonID, int _iChannelNum,int* _iThreshold);

int __stdcall NetClient_SetMotionDetection(int _iLogonID, int _iChannelNum,int _iEnabled); 

int __stdcall NetClient_GetMotionDetection(int _iLogonID, int _iChannelNum,int* _iEnabled); 

int __stdcall NetClient_SetMotionDecLinkRec(int _iLogonID,int _iChannelNum,int _iEnable); 

int __stdcall NetClient_GetMotionDecLinkRec(int _iLogonID,int _iChannelNum,int* _iEnable);

int __stdcall NetClient_SetMotionDecLinkSnap(int _iLogonID,int _iChannelNum,int _iEnable);

int __stdcall NetClient_GetMotionDecLinkSnap(int _iLogonID,int _iChannelNum,int* _iEnable);

int __stdcall NetClient_SetMotionDecLinkSoundDisplay(int _iLogonID,int _iChannelNum,int _iSoundEnable,int _iDisplayEnable);

int __stdcall NetClient_GetMotionDecLinkSoundDisplay(int _iLogonID,int _iChannelNum,int *_iSoundEnable,int *_iDisplayEnable);

int __stdcall NetClient_SetMotionDecLinkOutport(int _iLogonID,int _iChannelNum,int _iOutportArray); 

int __stdcall NetClient_GetMotionDecLinkOutport(int _iLogonID,int _iChannelNum,int *_iOutportArray); 

int __stdcall NetClient_SetMotionDetectSchedule(int _iLogonID,int _iChannelNum,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]); 

int __stdcall NetClient_GetMotionDetectSchedule(int _iLogonID,int _iChannelNum,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]); 

int __stdcall NetClient_SetOsdDiaphaneity(int _iLogonID,int _iChannelNum,int _iDiaphaneity); 

int __stdcall NetClient_GetOsdDiaphaneity(int _iLogonID,int _iChannelNum,int *_iDiaphaneity); 

int __stdcall NetClient_SetOsdText(int _iLogonID, int _iChannelNum, unsigned char * _pcTxtPtr, unsigned long _ulTextColor); 

int __stdcall NetClient_GetOsdText(int _iLogonID, int _iChannelNum, unsigned char* _pcOSDText, unsigned long* _pulTextColor);

int __stdcall NetClient_SetOsdType(int _iLogonID, int _iChannelNum,int _iPositionX,int _iPositionY,int _iOSDType,int _iEnabled); 

int __stdcall NetClient_GetOsdType(int _iLogonID, int _iChannelNum, int _iOSDType, int* _iPositionX ,int* _iPositionY , int *_iEnabled); 

int __stdcall NetClient_SetDateFormat(int _iLogonID, int _iFormat, char _cSeparate);

int __stdcall NetClient_GetDateFormat(int _iLogonID, int *_iFormat, char *_cSeparate);

int __stdcall NetClient_SetOsdLOGO (int _iLogonID, int _iChannelNum, unsigned char * _cLogoFile, unsigned int _ulbkColor); // Unimplemented

int __stdcall NetClient_SetAudioChannel(int _iLogonID, int _iChannelNum, int _iAudioChannel);

int __stdcall NetClient_GetAudioChannel(int _iLogonID, int _iChannelNum, int* _iAudioChannel);

int __stdcall NetClient_SetIpFilter(int _iLogonID, char* _cFilterIP,char* _cFilterMask);

int __stdcall NetClient_GetIpFilter(int _iLogonID, char* _cFilterIP,char* _cFilterMask);

int __stdcall NetClient_SetAlarmOutput(int _iLogonID, int _iAlarmInput, unsigned long _ulAlarmOutput);  

int __stdcall NetClient_GetAlarmOutput(int _iLogonID, int _iAlarmInput, unsigned long* _ulAlarmOutput);

int __stdcall NetClient_GetAlarmIPortState(int _iLogonID, int _iInPort,int *_iState);

int __stdcall NetClient_SetAlarmPortEnable(int _iLogonID,int _iInPort,int _iEnabled);

int __stdcall NetClient_GetAlarmPortEnable(int _iLogonID,int _iInPort,int *_iEnabled);

int __stdcall NetClient_SetInportAlarmLinkRec(int _iLogonID,int _iPortNo,int _iEnable);

int __stdcall NetClient_GetInportAlarmLinkRec(int _iLogonID,int _iPortNo,int* _iEnable); 

int __stdcall NetClient_SetInportAlarmLinkSnap(int _iLogonID,int _iPortNo,int _iEnable);

int __stdcall NetClient_GetInportAlarmLinkSnap(int _iLogonID,int _iPortNo,int* _iEnable);

int __stdcall NetClient_SetInportAlarmLinkPTZ(int _iLogonID,int _iPortNo,int _iLinkChannelNum,int _iLinkType,int _iActNum); 

int __stdcall NetClient_GetInportAlarmLinkPTZ(int _iLogonID,int _iPortNo,int _iLinkChannelNum,int *_iLinkType,int *_iActNum) ;

int __stdcall NetClient_SetInportAlarmLinkSoundDisplay(int _iLogonID,int _iPortNo,int _iSoundEnable,int _iDisplayEnable);

int __stdcall NetClient_GetInportAlarmLinkSoundDisplay(int _iLogonID,int _iPortNo,int *_iSoundEnable,int *_iDisplayEnable);

int __stdcall NetClient_SetInportAlmLinkOutport(int _iLogonID,int _iPortNo,int _iOutportArray);

int __stdcall NetClient_GetInportAlmLinkOutport(int _iLogonID,int _iPortNo,int *_iOutportArray);

int __stdcall NetClient_SetAlarmInMode(int _iLogonID, int _iPortNum, int _iLowOrHigh);

int __stdcall NetClient_GetAlarmInMode(int _iLogonID, int _iPortNum, int* _iLowOrHigh);

int __stdcall NetClient_SetAlarmOutMode(int _iLogonID, int _iPortNum, int _iLowOrHigh, int _iPulseWidth);

int __stdcall NetClient_GetAlarmOutMode(int _iLogonID, int _iPortNum, int* _iLowOrHigh, int* _iPulseWidth);

int __stdcall NetClient_SetInportAlarmSchedule(int _iLogonID,int _iPortNo,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]);

int __stdcall NetClient_GetInportAlarmSchedule(int _iLogonID,int _iPortNo,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]);

int __stdcall NetClient_SetOutportAlarmSchedule(int _iLogonID,int _iPortNo,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]);

int __stdcall NetClient_GetOutportAlarmSchedule(int _iLogonID,int _iPortNo,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]); 

int __stdcall NetClient_SetOutportAlarmDelay(int _iLogonID,int _iPortNo,int _iClearType,int _iDelayTime);

int __stdcall NetClient_GetOutportAlarmDelay(int _iLogonID,int _iPortNo,int *_iClearType,int *_iDelayTime);

int __stdcall NetClient_SetInportEnable(int _iLogonID, int _iPortNo,int _iEnable);

int __stdcall NetClient_GetInportEnable(int _iLogonID, int _iPortNo,int *_iEnable); 

int __stdcall NetClient_SetOutportEnable(int _iLogonID, int _iPortNo,int _iEnable);

int __stdcall NetClient_GetOutportEnable(int _iLogonID, int _iPortNo,int* _iEnable);

int __stdcall NetClient_SetOutportState(int _iLogonID, int _iPort, int _iState);

int __stdcall NetClient_GetOutportState(int _iLogonID, int _iPort, int *_iState);

int __stdcall NetClient_SetAlmVdoCovThreshold(int _iLogonID, int _iChannelNum, int _iThreshold); 

int __stdcall NetClient_GetAlmVdoCovThreshold(int _iLogonID, int _iChannelNum, int* _iThreshold);

int __stdcall NetClient_SetAlmVideoCov(int _iLogonID, int _iChannelNum, int _iEnabled);

int __stdcall NetClient_GetAlmVideoCov(int _iLogonID, int _iChannelNum, int* _iEnabled); 

int __stdcall NetClient_SetDeviceType(int _iLogonID,int _iChannelNum,int _iComNo,int _iDevAddress,const char* _pcProtocol);

int __stdcall NetClient_GetDeviceType(int _iLogonID,int _iChannelNum,int* _iComPort,int* _iDevAddress ,char* _cDeviceType);

int __stdcall NetClient_SetComFormat(int _iLogonID,int _iComPort,char* _cDeviceType,char* _cComFormat,int _iWorkMode); 

int __stdcall NetClient_GetComFormat(int _iLogonID, int _iCom, char* _cComFormat,int* _iWorkMode);

int __stdcall NetClient_GetAllSupportDeviceType(int _iLogonID, int* _iSumDeviceType, char* _cDeviceType);

int __stdcall NetClient_DeviceCtrl(int _iLogonID, int _iChannelNum,int _iActionType, int _iParam1,int _iParam2);


/***************************************************************
Function			：		NetClient_DeviceCtrlEx
Description			:		ptz or curise control
_iLogonID (IN)		:		logon id
_iChannelNum	(in):		channel no.
_iActionType(in)	:		control type
_iParam1(in)		:		control param1
_iParam2(in)		:		control param2
_iControlType(in)	:		ptz type 0:normal 1:electron ptz
Return				:		0: correct other: wrong
***************************************************************/
int __stdcall NetClient_DeviceCtrlEx(int _iLogonID,int _iChannelNum,int _iActionType,int _iParam1,int _iParam2,int _iControlType);

int __stdcall NetClient_ComSend(int _iLogonID, unsigned char* _ucBuf, int _iLength, int _iComNo);

int __stdcall NetClient_DevicePTZCtrl(int _iLogonID, int _iProtocolType, int _iActionType, int _iComNo, int _iAddress, int _iSpeed, int _iPresetNO);

int __stdcall NetClient_GetComPortCounts(int _iLogonID, int *_piComPortCounts, int *_piComPortEnabledStatus);

int __stdcall NetClient_SetAlarmVideoLost(int _iLogonID, int _iChannelNum,int _iEnabled);

int __stdcall NetClient_GetAlarmVideoLost(int _iLogonID, int _iChannelNum, int* _iEnabled); 

int __stdcall NetClient_SetVideoLostLinkPTZ(int _iLogonID,int _iChannelNum,int _iLinkChannelNum,int _iLinkType,int _iActNum);

int __stdcall NetClient_GetVideoLostLinkPTZ(int _iLogonID,int _iChannelNum,int _iLinkChannelNum,int *_iLinkType,int *_iActNum);

int __stdcall NetClient_SetVideoLostLinkSoundDisplay(int _iLogonID,int _iChannelNum,int _iSoundEnable,int _iDisplayEnable );

int __stdcall NetClient_GetVideoLostLinkSoundDisplay(int _iLogonID,int _iChannelNum,int *_iSoundEnable,int *_iDisplayEnable);

int __stdcall NetClient_GetAlarmVLostState(int _iLogonID, int _iChannel,int *_iState); 

int __stdcall NetClient_SetVideoLostLinkOutport(int _iLogonID,int _iChannelNum,int _iOutportArray);

int __stdcall NetClient_GetVideoLostLinkOutport(int _iLogonID,int _iChannelNum,int *_iOutportArray);

int __stdcall NetClient_SetVideoLostSchedule(int _iLogonID,int _iChannelNum,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]);

int __stdcall NetClient_GetVideoLostSchedule(int _iLogonID,int _iChannelNum,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]); 

int __stdcall NetClient_SetVideoLostLinkRec(int _iLogonID,int _iChannelNum,int _iEnable);

int __stdcall NetClient_GetVideoLostLinkRec(int _iLogonID,int _iChannelNum,int* _iEnable); 

int __stdcall NetClient_SetVideoLostLinkSnap(int _iLogonID,int _iChannelNum,int _iEnable);

int __stdcall NetClient_GetVideoLostLinkSnap(int _iLogonID,int _iChannelNum,int* _iEnable);

int __stdcall NetClient_SetIFrameRate(int _iLogonID, int _iChannelNum, int _iFrameRate, int _iStreamNO);

int __stdcall NetClient_GetIFrameRate(int _iLogonID, int _iChannelNum, int* _iFrameRate, int _iStreamNO);

int __stdcall NetClient_ForceIFrame(int _iLogonID, int _iChannelNum, int _iStreamNO);

int __stdcall NetClient_SetTime(int _iLogonID, int _iyy, int _imo, int _idd, int _ihh, int _imi, int _iss);

int __stdcall NetClient_Reboot(int _iLogonID);

int __stdcall NetClient_RebootEx(int _iLogonID, int _iChannelNo);

int __stdcall NetClient_DefaultPara(int _iLogonID);

int __stdcall NetClient_DefaultParaEx(int _iLogonID, int _iType);

int __stdcall NetClient_GetServerVersion(int _iLogonID, char *_cVer);

int __stdcall NetClient_SetNVS(int _iLogonID, int _iVideo, int _iCheck);

int __stdcall NetClient_UpgradeProgram(int _iLogonID, char* _cFileName, PROUPGRADE_NOTIFY _UpgradeNotify); 

int __stdcall NetClient_UpgradeWebPage(int _iLogonID, char* _cFileName, WEBUPGRADE_NOTIFY _UpgradeWebNotify);

int __stdcall NetClient_GetUpgradePercent(int _iLogonID);

int __stdcall NetClient_GetUserNum(int _iLogonID,int* _iUserNum);

int __stdcall NetClient_GetUserInfo(int _iLogonID,int _iUserSerial,char *_cUserName,char* _cPassword,int* _iAuthority);

int __stdcall NetClient_GetCurUserInfo(int _iLogonID, char _cUserName[16], char _cPassword[16], int* _iAuthority);

int __stdcall NetClient_AddUser(int _iLogonID, char* _cUserName, char* _cPassword, int _iAuthority);

int __stdcall NetClient_DelUser(int _iLogonID, char* _cUserName);

int __stdcall NetClient_ModifyPwd(int _iLogonID, char* _cUserName, char* _cNewPwd);

int __stdcall NetClient_SetMaxConUser(int _iLogonID, int _iMaxConUser);

int __stdcall NetClient_GetMaxGetUser(int _iLogonID,int* _iMaxConUser);

int __stdcall NetClient_TalkStart(int _iLogonID, int _iUser);

int __stdcall NetClient_TalkEnd(int _iLogonID);

int __stdcall NetClient_InputTalkingdata(unsigned char* _ucData, unsigned int	_iLen);

int __stdcall NetClient_GetTalkingState(int _iLogonID,int* _iTalkState);

int __stdcall NetClient_CapturePic(unsigned int _ulConID,unsigned char** _pucData); 

int __stdcall NetClient_CaptureBmpPic(unsigned int _ulConID, char* _pcFileName);

int __stdcall NetClient_ChangeSvrIP(int _iLogonID, char* _cNewSvrIP, char* _cMask, char* _cGateway, char* _cDNS); 

int __stdcall NetClient_GetIpProperty(int _iLogonID, char* _cMAC, char* _cMask, char* _cGateway, char* _cDNS);

int __stdcall NetClient_SetDHCPParam(int _iLogonID,int _iDHCP); 

int __stdcall NetClient_GetDHCPParam(int _iLogonID,int* _iDHCP);

int __stdcall NetClient_SetWifiDHCPParam(int _iLogonID,int _iDHCP); 

int __stdcall NetClient_GetWifiDHCPParam(int _iLogonID,int* _iDHCP);

int __stdcall NetClient_GetVideoCovArea(int _iLogonID, int _iChannelNum, RECT* _rect, int _iStreamNO);

int __stdcall NetClient_SetVideoCovArea(int _iLogonID, int _iChannelNum, RECT* _rect, int _iStreamNO);

int __stdcall NetClient_GetVideoSize(int _iLogonID, int _iChannelNum, int* _width, int * _height, int _iStreamNO);
 
int __stdcall NetClient_SetVideoSize(int _iLogonID, int _iChannelNum, int _iVideoSize, int _iStreamNO);
 
int __stdcall NetClient_GetVideoSizeEx(int _iLogonID, int _iChannelNum, int* _ivideoSize, int _iStreamNO);
 
int __stdcall NetClient_GetMaxMinorVideoSize(int _iLogonID, int* _iMinorVideoSize);

int __stdcall NetClient_BindInterface(int	_interface);

char* __stdcall NetClient_GetNetInterface(int	_interface);

int __stdcall NetClient_SetMaxKByteRate(int _iLogonID, int _iChannelNum, int _ibitRate, int _iStreamNO);

int __stdcall NetClient_GetMaxKByteRate(int _iLogonID, int _iChannelNum, int* _ibitRate, int _iStreamNO); 

int __stdcall NetClient_WriteUserData(int	_iLogonID,int _iOffset,unsigned char* _u8Buf,int _iLength);

int __stdcall NetClient_ReadUserData(int _iLogonID,int _iOffset,unsigned char* _u8Buf,int	_iLength);

int __stdcall NetClient_SetReducenoiseState(int _iLogonID, int _iChannelNum,int _iState);

int __stdcall NetClient_GetReducenoiseState(int _iLogonID,int _iChannelNum,int *_iState);

int __stdcall NetClient_DrawRectOnLocalVideo(unsigned int	_ulConID, RECT*	_rcRect, int _iCount);

int __stdcall NetClient_DrawTextOnVideo(int _iLogonID, int _iChannelNum, int _iX, int	_iY, char* _cText, int _iStore, int	_iStreamNO);

int __stdcall NetClient_GetTextOnVideo(int _iLogonID, int	_iChannelNum, int* _iX, int* _iY, char* _cText,int _iStreamNO);

int __stdcall NetClient_SetBothStreamSame(int _iLogonID, int _iChannelNum, int _iState);

int __stdcall NetClient_GetBothStreamSame(int _iLogonID, int _iChannelNum, int* _iState);

int __stdcall NetClient_ShowBitrateOnVideo(unsigned int _ulConID, int _iX, int _iY, int _iEnabled);

int __stdcall NetClient_SetPPPoEInfo(int _iLogonID,char* _cAccount, char* _cPassword, int	_iEnabled);

int __stdcall NetClient_GetPPPoEInfo(int _iLogonID,char* _cAccount, char* _cPassword, int *_iEnabled);

int __stdcall NetClient_CPUCheckEnabled(int _iEnabled, int _interval);

int __stdcall NetClient_SetEncodeMode(int _iLogonID, int _iChannelNum, int _iStreamNO, int _iMode);

int __stdcall NetClient_GetEncodeMode(int _iLogonID, int _iChannelNum, int _iStreamNO, int* _iMode);

int __stdcall NetClient_SetPreferMode(int _iLogonID, int _iChannelNum, int _iStreamNO, int _iMode);

int __stdcall NetClient_GetPreferMode(int _iLogonID, int _iChannelNum, int _iStreamNO, int* _iMode);

int __stdcall NetClient_LogFileSetProperty(int _iLogonID, int _iLevel, int _iSize);

int __stdcall NetClient_LogFileGetProperty(int _iLogonID, int* _iLevel, int* _iSize);

int __stdcall NetClient_LogFileDownload(int _iLogonID); 

int __stdcall NetClient_LogFileClear(int _iLogonID); 

int __stdcall NetClient_LogFileGetDetails(int _iLogonID, char* _cBuf, int* _iLen); 

int __stdcall NetClient_GetVideoNPMode(int _iLogonID, VIDEO_NORM *_vMode);

int __stdcall NetClient_SetVideoNPMode(int _iLogonID, VIDEO_NORM _vMode);

int __stdcall NetClient_SetAudioEncoder(int _iLogonID, int _iChannel, AUDIO_ENCODER _aCoder);

int __stdcall NetClient_GetAudioEncoder(int _iLogonID, int _iChannel, AUDIO_ENCODER *_aCoder);

int __stdcall NetClient_NetFileQuery(int _iLogonID, PNVS_FILE_QUERY _ptFileQuery);

int __stdcall NetClient_NetFileSetRecordRule(int _iLogonID, int _iRule, int _iTimelen, int _iFreedisk ,int _iFileSize); 

int __stdcall NetClient_NetFileGetRecordRule(int _iLogonID, int* _iRule, int *_iTimelen, int *_iFreedisk,int *_iFileSize);

int __stdcall NetClient_NetFileSetAlarmRule(int _iLogonID, int _iPreRecordEnable, int _iPreRecordTime, int _iDelayTime ,int _iDelayEnable ,int _iChannelNum);

int __stdcall NetClient_NetFileGetAlarmRule(int _iLogonID, int *_iPreEnable, int *_iPreTime, int *_iDelayTime ,int *_iDelayEnable,int _iChannelNum);

int __stdcall NetClient_NetFileSetAlarmState(int _iLogonID, int _iChannel, int _iState);

int __stdcall NetClient_NetFileGetAlarmState(int _iLogonID, int _iChannel, int *_iState);

int __stdcall NetClient_NetFileSetTaskState(int _iLogonID, int _iChannel, int _iState);

int __stdcall NetClient_NetFileGetTaskState(int _iLogonID, int _iChannel, int *_iState); 

int __stdcall NetClient_NetFileSetTaskSchedule(int _iLogonID, int _iChannel, int _iWeekday, NVS_SCHEDTIME *_Schedule[MAX_TIMESEGMENT]);

int __stdcall NetClient_NetFileGetTaskSchedule(int _iLogonID, int _iChannel, int _iWeekday, NVS_SCHEDTIME *_Schedule[MAX_TIMESEGMENT]); 

int __stdcall NetClient_NetFileSetTaskScheduleEx(int _iLogonID, int _iChannel, int _iWeekday, NVS_SCHEDTIME_Ex *_Schedule[MAX_TIMESEGMENT]);

int __stdcall NetClient_NetFileGetTaskScheduleEx(int _iLogonID, int _iChannel, int _iWeekday, NVS_SCHEDTIME_Ex *_Schedule[MAX_TIMESEGMENT]); 

int __stdcall NetClient_NetFileGetFileCount(int _iLogonID, int *_piTotalCount, int *_piCurrentCount); 

int __stdcall NetClient_NetFileRebuildIndexFile(int _iLogonID, int _iState);

int __stdcall NetClient_NetFileGetDiskInfo(int _iLogonID, PNVS_STORAGEDEV _storage);

int __stdcall NetClient_NetFileGetDiskInfoEx(int _iLogonID, PNVS_STORAGEDEV _storage, int _iSize);


int __stdcall NetClient_NetFileIsSupportStorage(int _iLogonID);

int __stdcall NetClient_NetFileDownloadFile(unsigned int *_ulID,
								  int _iLogonID, 
								  char* _cRemoteFilename, 
								  char* _cLocalFilename,
								  int	_iFlag,
								  int	_iPosition,
								  int	_Speed); 

int __stdcall NetClient_NetFileDownloadFileEx(unsigned int* _ulConID,
									int _iLogonID, 
									char* _cRemoteFilename, 
									char* _cLocalFilename,
									int	_iFlag,
									int	_iPosition,
									int	_Speed,
									NVSDATA_NOTIFY _cbkDataArrive,
									void* _iUserData);

int __stdcall NetClient_NetFileStopDownloadFile(unsigned int _ulID);

/***************************************************************
Function			：		NetClient_NetFileDownload
Description			:		get already download file size and download pos
_ulID (IN)			:		The download ID
_iPos	(OUT)		:		download pos
_iDLSize(OUT)		:		download file size UNITS（Bytes）
Return				:		0: correct other: wrong
***************************************************************/
int __stdcall NetClient_NetFileGetDownloadPos(unsigned int _ulID, int *_iPos, int *_iDLSize);

int __stdcall NetClient_NetFileMountUSB(int _iLogonID, int _iState);

int __stdcall NetClient_NetFileGetRecordState(int _iLogonID, int _iChannel, RECORD_STATE *_piState);

int __stdcall NetClient_NetFileDelFile(int _iLogonID,const char *_pcFileName);

int __stdcall NetClient_DiskSetUsage(int _iLogonID,int _iDiskNo,int _iUsage);

int __stdcall NetClient_NetFileGetQueryfile(int _iLogonID, int _iFileIndex, PNVS_FILE_DATA _fileInfo); 

int __stdcall NetClient_DiskFormat(int _iLogonID,int _iDiskNo,int _iFsType); 

int __stdcall NetClient_DiskPart(int _iLogonID,int _iDiskNo,int _iPartNum,int _iFormatNow);

int __stdcall NetClient_NetFileManualRecord(int _iLogonID, int _iChannel, int _iState);

int __stdcall NetClient_NetFileMapStoreDevice(int _iLogonID,PNVS_NFS_DEV _storeDev);

int __stdcall NetClient_NetFileGetMapStoreDevice(int _iLogonID,PNVS_NFS_DEV _storeDev);

int __stdcall NetClient_NetFileGetUSBstate(int _iLogonID, int *_iState);

int __stdcall NetClient_NetFileSetExtendname(int _iLogonID, char *_cExtend);

int __stdcall NetClient_NetFileGetExtendname(int _iLogonID, char *_cExtend);

int __stdcall NetClient_ClearDisk(int	_iLogonID,int _iDiskNo);

int __stdcall NetClient_GetDownloadFailedFileName(int _iLogonID, int _iFileID, char *_pcFileName, int _iFileNameBufSize);

//3.51

//3.52
int __stdcall NetClient_SetMediaStreamClient(int _iLogonID, int _iChannel, char* _cClientIP, unsigned short _iClientPort, int _iStreamType);

int __stdcall NetClient_GetMediaStreamClient(int _iLogonID, int _iChannel, char* _cClientIP, unsigned short* _iClientPort, int* _iStreamType);

//3.53

int __stdcall NetClient_SetEmailAlarm(int _iLogonID, PSMTP_INFO _pSmtp); 

int __stdcall NetClient_GetEmailAlarm(int _iLogonID, PSMTP_INFO _pSmtp);

int __stdcall NetClient_SetEmailAlarmEnable(int _iLogonID, int _iChannel, int _iEnable); 

int __stdcall NetClient_GetEmailAlarmEnable(int _iLogonID, int _iChannel, int *_iEnable); 

//3.54
int __stdcall NetClient_SetScene(int _iLogonID, int _iChannel, int _iScene);

int __stdcall NetClient_GetScene(int _iLogonID, int _iChannel, int *_iScene);

int __stdcall NetClient_SetSensorFlip(int	_iLogonID, int _iChannel, int _iFlip);

int __stdcall NetClient_GetSensorFlip(int _iLogonID, int _iChannel, int *_iFlip);

int __stdcall NetClient_SetSensorMirror(int _iLogonID, int _iChannel, int _iMirror);

int __stdcall NetClient_GetSensorMirror(int _iLogonID, int _iChannel, int *_iMirror);

//3.55
int __stdcall NetClient_Snapshot(int _iLogonID, int _iChannel, int _iQvalue);

//3.56
int __stdcall NetClient_GetFactoryID(int _iLogonID, char* _cFactoryID);

//3.57
int __stdcall NetClient_SetWifiParam(int _iLogonID, NVS_WIFI_PARAM* _pWifiParam);

int __stdcall NetClient_GetWifiParam(int _iLogonID, NVS_WIFI_PARAM *_pWifiParam);

int __stdcall NetClient_WifiSearch(int _iLogonID);

int __stdcall NetClient_GetWifiSearchResult(int _iLogonID, WIFI_INFO** _pWifiInfo, int* _iWifiCount);

//3.58
int __stdcall NetClient_SetPrivacyProtect(int _iLogonID,int _iChannelNum,int _iEnabled);

int __stdcall NetClient_GetPrivacyProtect(int _iLogonID,int _iChannelNum,int* _iEnabled);

//3.59
int __stdcall NetClient_IYUVtoYV12(int _iWidth, int _iHeight, unsigned char* _YUV420);

//3.60
int __stdcall NetClient_GetDevType(int _iLogonID,int* _iDevType);

/***************************************************************
Function    : NetClient_GetProductType
Description	: Get Product Modle (PM value)
Prameter:    [IN]_iLogonID: Logon handle 
			 [OUT]_iType: output product model, detailed information  by ProductModel.h
others       :	xxxx.xx.xx
***************************************************************/
int __stdcall NetClient_GetProductType(int _iLogonID,int* _iType);

/***************************************************************
Function    : NetClient_GetProductTypeEx
Description	: Get Product Type: 0--reserverd, 1--ipc, 2--nvr
Prameter:    [IN]_iLogonID: Logon handle 
			 [OUT]_piProductMode: output product model, detailed information  by ProductModel.h
			 [OUT]_piProductType: output product type: 0--reserverd, 1--ipc, 2--nvr
return       :	== 0：Success; other:Failed, Return Value Description by RetValue.h.
others       :	2019.11.22
***************************************************************/
int __stdcall NetClient_GetProductTypeEx(int _iLogonID, int* _piProductMode, int* _piProductType);

//3.61
int __stdcall NetClient_BackupKernel(int _iLogonID);

//3.62
int __stdcall NetClient_SetUPNPEnable(int	_iLogonID,int _iEnable);

int __stdcall NetClient_GetUPNPEnable(int _iLogonID,int *_iEnable);

//3.63
int __stdcall NetClient_GetSysInfo(int _iLogonID);

//3.64
int __stdcall NetClient_SetDDNSPara(int _iLogonID,char* _cDUserName,char* _cDPassword,char* _cDNvsName, char* _cDomain,int _iPort,int _iDEnable);

int __stdcall NetClient_GetDDNSPara(int _iLogonID,char* _cDUserName,char* _cDPassword,char* _cDNvsName, char* _cDomain,int* _iPort,int* _iDEnable);

//3.65
int __stdcall NetClient_SetFuncListArray(int _iLogonID,int _iEnableArray);

int __stdcall NetClient_GetFuncListArray(int _iLogonID,int *_iEnableArray);

//3.66
int __stdcall NetClient_SendStringToServer(int _iLogonID, char* _cMsg, int _iLen);

int __stdcall NetClient_ReceiveString(char* _cIpAddress,int* _iType,char* _cMsg,int* _iLen);

int __stdcall NetClient_SendStringToCenter(char* _cIpAddress, int _iServerPort,char* _cMsg, int _iLen);

//3.67
int __stdcall NetClient_SetVencType(int _iLogonID,int _iChannelNum,int _iType);

int __stdcall NetClient_GetVencType(int _iLogonID,int _iChannelNum,int *_iType);

//3.68
int __stdcall NetClient_SetComServer(int _iLogonID,char* _cComServer,int _iComServerPort);

int __stdcall NetClient_GetComServer(int _iLogonID,char* _cComServer,int* _iComServerPort);

//3.69
int __stdcall NetClient_Get3GDeviceStatus(int _iLogonID, int* _i3GDeviceType,int* _iStatus,int* _iIntensity,char* _pcIP,char* _pcStarttime);

int __stdcall NetClient_Set3GDialog(int _iLogonID, int _iStartType,int _iStopType,int _iDuration);

int __stdcall NetClient_Get3GDialog(int _iLogonID, int* _iStartType,int* _iStopType,int* _iDuration);

int __stdcall NetClient_Set3GMessage(int _iLogonID, char* _cNotify,char* _cPhoneNum1,char* _cPhoneNum2,char* _cPhoneNum3,char* _cPhoneNum4,char* _cPhoneNum5);

int __stdcall NetClient_Get3GMessage(int _iLogonID, char* _cNotify,char* _cPhoneNum1,char* _cPhoneNum2,char* _cPhoneNum3,char* _cPhoneNum4,char* _cPhoneNum5);

int __stdcall NetClient_Set3GTaskSchedule(int _iLogonID, int _iEnable, PNVS_SCHEDTIME _strScheduleParam);

int __stdcall NetClient_Get3GTaskSchedule(int _iLogonID, int* _iEnable, PNVS_SCHEDTIME _strScheduleParam);

int __stdcall NetClient_Set3GNotify(int _iLogonID, int _iType,char* _cMessage);

int __stdcall NetClient_Get3GNotify(int _iLogonID, int* _iType,char* _cMessage);

//3.70
int __stdcall NetClient_SetHDCamer(int _iLogonID, int _iChannel,int _iFuncNum,int _iValue);

int __stdcall NetClient_GetHDCamer(int _iLogonID, int _iChannel,int _iFuncNum,int* _iValue);

//3.71	
int __stdcall NetClient_SetAlarmServer(int _iLogonID,const char* _pcAlarmServer,int _iServerPort);

int __stdcall NetClient_GetAlarmServer(int _iLogonID,char* _pcAlarmServer,int* _iServerPort);


//==============================================================================================

int __stdcall NetClient_InterTalkStart(unsigned int * _uiConnID, int _iLogonID, int _iUserData );

int __stdcall NetClient_InterTalkEnd(unsigned int _uiConnID, bool _blStopTalk);

//==============================================================================================

int __stdcall NetClient_NetFileQueryEx(int _iLogonID, PNVS_FILE_QUERY _pfileQuery, int _iSizeOfQuery);

int __stdcall NetClient_ControlDeviceRecord(int _iLogonID, int _iChannelID, int _iRecordType, int _iAction);

//==============================================================================================

int __stdcall NetClient_NetFileDownloadByTimeSpan(unsigned int*	_ulConID, int _iLogonID, char* _pcLocalFile, 
					int _iChannelNO, unsigned int _uiFromSecond, unsigned int _uiToSecond, 
                                                  int _iFlag, int _iPosition,int _iSpeed);

int __stdcall NetClient_NetFileDownloadByTimeSpanEx( unsigned int* _ulConID, int _iLogonID, char* _pcLocalFile, 
					int _iChannelNO, NVS_FILE_TIME *_pTimeBegin, NVS_FILE_TIME *_pTimeEnd, 
                                                     int _iFlag, int _iPosition,int _iSpeed);

int __stdcall NetClient_NetFileDownloadByTimeSpanCallBack( unsigned int* _ulConID, 
                                                 int _iLogonID, 
												 char* _pcLocalFile,
												 int _iChannelNO,
                                                 NVS_FILE_TIME *_pTimeBegin, 
                                                 NVS_FILE_TIME *_pTimeEnd, 
												 NVSDATA_NOTIFY _cbkDataArrive,
                                                 int _iFlag, 
                                                 int _iPosition,
                                                 int _iSpeed,
                                                 void* _iUserData);                     // In the time period to download the file, the data flow back out. add by lyh 2010.9.1

int __stdcall NetClient_NetLogQuery(int _iLogonID, PNVS_LOG_QUERY _logQuery);

int __stdcall NetClient_NetLogGetLogfile(int _iLogonID, int _iLogIndex, PNVS_LOG_DATA _pLogInfo);

int __stdcall NetClient_NetLogGetLogCount(int _iLogonID, int *_iTotalCount, int *_iCurrentCount);

int __stdcall NetClient_GetProtocolList(int _iLogonID, st_NVSProtocol* _pstNVSProtocol);

//===============================================================================================

/***************************************************************
Function			：		NetClient_SetCHNPTZCRUISE
Description			:		set channel ptz curise info
_iLogonID (IN)		:		The longon ID
_iChannelNum(IN)	:		channel no.
_iCruiseNo(IN)		:		curise no.
_iEnable(IN)		:		enable  default 1 able
_iCruiseNum(IN)		:		curise preset num
_stPTZCruise(IN)	:		curise info in detail
Return				:		0: correct other: wrong
***************************************************************/
int __stdcall NetClient_SetCHNPTZCRUISE(int _iLogonID,int _iChannelNum,int _iCruiseNo,int _iEnable,int _iCruiseNum,st_PTZCruise* _stPTZCruise);

/***************************************************************
Function			：		NetClient_GetCHNPTZCRUISE
Description			:		set channel ptz curise info
_iLogonID (IN)		:		The longon ID
_iChannelNum(IN)	:		channel no.
_iCruiseNo(IN)		:		curise no.
_iEnable(OUT)		:		enable  default 1 able
_iCruiseNum(OUT)	:		curise preset num
_stPTZCruise(OUT)	:		curise info in detail
Return				:		0: correct other: wrong
***************************************************************/
int __stdcall NetClient_GetCHNPTZCRUISE(int _iLogonID,int _iChannelNum,int _iCruiseNo,int* _iEnable,int* _iCruiseNum,st_PTZCruise* _stPTZCruise);

int __stdcall NetClient_SetVIDEOCOVER_LINKRECORD(int _iLogonID,int _iChannelNum,int _iEnableByBits);

int __stdcall NetClient_GetVIDEOCOVER_LINKRECORD(int _iLogonID,int _iChannelNum,int* _iEnableByBits);

int __stdcall NetClient_SetVIDEOCOVER_LINKPTZ(int _iLogonID,int _iChannelNum,int _iLinkChannel,int _iLinkType,int _iNo);

int __stdcall NetClient_GetVIDEOCOVER_LINKPTZ(int _iLogonID,int _iChannelNum,int _iLinkChannel,int* _iLinkType,int* _iNo);

int __stdcall NetClient_GetAlarmVCoverState(int _iLogonID, int _iChannel,int *_iState);

int __stdcall NetClient_StopCaptureDate(unsigned long _ulID);

/*
 * ------------------------------------------------------------------------------------------------------------------ 
 * add by lyh, for 3520 
 */



int __stdcall NetClient_SetColorToGray(int _iLogonID,int _iChannelNum, int _iEnable);

int __stdcall NetClient_GetColorToGray(int _iLogonID,int _iChannelNum, int* _iEnable);

int __stdcall NetClient_SetCustomChannelName(int _iLogonID,int _iChannelNum,int _iChannelType, char* _cChannelName);

int __stdcall NetClient_GetCustomChannelName(int _iLogonID,int _iChannelNum,int _iChannelType, char* _cChannelName);

int __stdcall NetClient_SetCustomRecType(int _iLogonID,int _iRecType, char* _cRecTypeName);

int __stdcall NetClient_GetCustomRecType(int _iLogonID,int _iRecType, char* _cRecTypeName);

int __stdcall NetClient_ChangeSvrIPEx(int _iLogonID, char* _cNewSvrIP, char* _cMask, char* _cGateway, char* _cDNS, char* _cBackDNS);

int __stdcall NetClient_GetIpPropertyEx(int _iLogonID,char* _cMAC, char* _cMask, char* _cGateway, char* _cDNS, char* _cBackDNS);

int __stdcall NetClient_SetFTPUpdate(int _iLogonID,char* _cFtpAddr,char* _cFilePath,char* _cFtpUserName, char* _cFtpPassword);

int __stdcall NetClient_GetFTPUpdate(int _iLogonID,char* _cFtpAddr,char* _cFilePath,char* _cFtpUserName, char* _cFtpPassword);

int __stdcall NetClient_SetCHNPTZFormat(int _iLogonID,int _iChannelNum,char* _cComFormat);

int __stdcall NetClient_GetCHNPTZFormat(int _iLogonID,int _iChannelNum,char* _cComFormat);

int __stdcall NetClient_GetServerVersionEx(int _iLogonID, char *_cVer,char *_cUIVersion,char* _cSlaveVersion);

int __stdcall NetClient_GetOSDTypeColor(int _iLogonID, int _iChannelNum, int _iOSDType, int* _iColor);

int __stdcall NetClient_SetOSDTypeColor(int _iLogonID, int _iChannelNum, int _iOSDType, int _iColor);

int __stdcall NetClient_GetExceptionMsg(int _iLogonID, int _iExceptionTyep, int* _iState);

int __stdcall NetClient_SetNTPInfo(int _iLogonID, char* _NTPServer, unsigned short _iPort, int _iInterval);

int __stdcall NetClient_GetNTPInfo(int _iLogonID, char* _NTPServer, unsigned short *_iPort, int *_iInterval);

//20101202  -->hyq
//-----------------------------------------
/************************************************************************
*	Function:	Control CD-Burn (start or stop)
*	Parameters:	
		_iLogonID				LogonID (from __stdcall NetClient_Logon)
		_iChannel				Channel ID, could be set -1 if it will not be used any way
		_iCmd					CD-Burn command
		_lpCmdBuf				buffer to control
		_iBufSize				buffer size
************************************************************************/
int __stdcall NetClient_SetCDBurnConfig(int _iLogonID, int _iChannel, int _iCmd, void* _lpCmdBuf, int _iBufSize);
int __stdcall NetClient_GetCDBurnConfig(int _iLogonID, int _iChannel, int _iCmd, void* _lpCmdBuf, int _iBufSize);

/************************************************************************
*	Function:	Video Encrypt
*	Parameters:	
		_iLogonID				LogonID (from __stdcall NetClient_Logon)
		_iChannel				Channel ID, could be set -1 if it will not be used any way
		_lpCmdBuf				buffer of video encrypt
		_iBufSize				buffer size                                                                      
************************************************************************/
int __stdcall NetClient_SetVideoEncrypt(int _iLogonID, int _iChannel, void* _lpCmdBuf, int _iBufSize);
int __stdcall NetClient_GetVideoEncrypt(int _iLogonID, int _iChannel, void* _lpCmdBuf, int _iBufSize);
int __stdcall NetClient_SetVideoDecrypt(int _iLogonID, int _iChannel, void* _lpCmdBuf, int _iBufSize);
int __stdcall NetClient_GetVideoDecrypt(int _iLogonID, int _iChannel, void* _lpCmdBuf, int _iBufSize);

/************************************************************************
* Funtion: Set multi-screen synthesis enable
************************************************************************/
int __stdcall NetClient_SetPreRecEnable(int _iLogonID, int _iChannel, int _iEnable);
int __stdcall NetClient_GetPreRecEnable(int _iLogonID, int _iChannel, int *_piEnable);
//To achieve multi-screen synthesis
int __stdcall NetClient_SetVideoCombine(int _iLogonID, void* _lpBuf, int _iBufSize);
int __stdcall NetClient_GetVideoCombine(int _iLogonID, void* _lpBuf, int _iBufSize);

//VCA  20110111-->hyq
int __stdcall NetClient_VCASetConfig(int _iLogonID, int _iVCACmdID, int _iChannel, void* _lpCmdBuf, int _iCmdBufLen);

int __stdcall NetClient_VCAGetConfig(int _iLogonID, int _iVCACmdID, int _iChannel, void* _lpCmdBuf, int _iCmdBufLen);

int __stdcall NetClient_VCARestart(int _iLogonID);

int __stdcall NetClient_VCARestartEx(int _iLogonID, int _iChannelNO);

int __stdcall NetClient_VCAGetAlarmInfo(int _iLogonID, int _iAlarmIndex, void* _lpBuf, int _iBufSize);

// Email Alarm extension protocol
int __stdcall NetClient_SetEmailAlarmEx(int _iLogonID, PSMTP_INFO _pSmtp, int _iSize);
int __stdcall NetClient_GetEmailAlarmEx(int	_iLogonID, PSMTP_INFO _pSmtp, int _iSize);
// FTP upload automatically
int __stdcall NetClient_SetFTPUploadConfig(int _iLogonID, int _iCmd, void* _lpCmdBuf, int _iBufSize);
int __stdcall NetClient_GetFTPUploadConfig(int _iLogonID, int _iCmd, void* _lpCmdBuf, int _iBufSize);

//Car 3G DVR
/************************************************************************
*	Function:	Set 3G DVR configuration, such as POWERDELAY, SIMNUM, GPSINFO, GPSFILTER
*	Parameters:
		_iLogonID				LogonID (from __stdcall NetClient_Logon)
		_iCmd					3G CmdID
		_iChannel				Channel ID, could be set -1 if it will not be used any way
		_lpCmdBuf				buffer to get actual setting, should be &T3GDVRInfo
		_iBufSize				buffer size, should be sizeof(T3GDVRInfo)
************************************************************************/
int __stdcall NetClient_Set3GConfig(int _iLogonID, int _iCmd, int _iChannel, void* _lpValueBuf, int _iBufSize);
int __stdcall NetClient_Get3GConfig(int _iLogonID, int _iCmd, int _iChannel, void* _lpValueBuf, int _iBufSize);

/************************************************************************
* Digital channel parameter setting and reading
************************************************************************/
int __stdcall NetClient_SetDigitalChannelConfig(int _iLogonID, int _iChannel, int _iCmd, void* _lpCmdBuf, int _iBufSize);
int __stdcall NetClient_GetDigitalChannelConfig(int _iLogonID, int _iChannel, int _iCmd, void* _lpCmdBuf, int _iBufSize);

/************************************************************************
* Control the NVR front-end transparent channel, similar to NetClient_ComSend
************************************************************************/
int __stdcall NetClient_DigitalChannelSend(int _iLogonID, int _iChannel, unsigned char* _ucBuf, int _iLength);

/************************************************************************
* Control the serial data, NetClient_ComSend interface for the second package
************************************************************************/
int __stdcall NetClient_SendComData(int _iLogonID, int _iCommand, void *_lpInBuffer, int _iInBufferSize);

/************************************************************************
* VideoMode (N / P) according to channel setting
************************************************************************/
int __stdcall NetClient_SetVideoNPModeEx(int _iLogonID, int _iChannel, VIDEO_NORM _iNPMode);
int __stdcall NetClient_GetVideoNPModeEx(int _iLogonID, int _iChannel, VIDEO_NORM *_piNPMode);

/************************************************************************
* Get the number of digital channels, and get the properties of each channel (digital channel or local channel)
************************************************************************/
int __stdcall NetClient_GetDigitalChannelNum(int _iLogonID, int *_piDigitChannelNum);
/***************************************************************
Function: NetClient_SetChannelProperty
Description: Sets the channel attributes
Prameter (in): _iLogonID:
                    The login ID
_iChannel:
Number of Channels, Range: [0, Number of Channels -1], Number of Channels Returned by NetClient_GetChannelNum Parameters
_iCmd:
 Command ID
GENERAL_CMD_CHANNEL_TYPE Get the type of a channel for this device
_lpBuf:
The buffer pointer
                        Corresponds to _iCmd: GENERAL_CMD_CHANNEL_TYPE to int:
Channel type: 1, the local analog channel; 2, the digital channel; 3, composite channel
                    _iBufSize:
Buffer size, depending on _iCmd
Return: 0: correct
                    -1: Failed
                    Other: Error
Others: 2013-04-10
***************************************************************/
int __stdcall  NetClient_SetChannelProperty( int _iLogonID, int _iChannel, int _iCmd, void* _lpBuf, int _iBufSize );
/***************************************************************
Function     ：	NetClient_GetChannelProperty
Description: Gets the channel attribute
Prameter (in): _iLogonID:
                    The login ID
_iChannel:
Number of Channels, Range: [0, Number of Channels -1], Number of Channels Returned by NetClient_GetChannelNum Parameters
_iCmd:
 Command ID
GENERAL_CMD_GET_CHANNEL_TYPE Get the type of a channel for this device (change to the previous macro)
GENERAL_CMD_CHANNEL_TYPE Get the type of a channel of this device (changed after the macro)
_iBufSize:
Buffer size, depending on _iCmd
Prameter (out): _lpBuf:
                        The buffer pointer
                        Corresponds to _iCmd: GENERAL_CMD_GET_CHANNEL_TYPE to int *:
Corresponds to _iCmd: GENERAL_CMD_CHANNEL_TYPE to int *:
Channel type: 1, the local analog channel; 2, the digital channel; 3, composite channel
Return: 0: correct
                    -1: Failed
                    Other: Error
others         ：	2013-04-10 
***************************************************************/
int __stdcall NetClient_GetChannelProperty(int _iLogonID, int _iChannel, int _iCmd, void* _lpBuf, int _iBufSize);

int	__stdcall NetClient_SetDeviceTimerReboot(int _iLogonID, int _iEnable, int _iInterval, int _iHour);
int	__stdcall NetClient_GetDeviceTimerReboot(int _iLogonID, int *_iEnable, int *_iInterval, int *_iHour);
int __stdcall NetClient_SetVideoCoverSchedule(int _iLogonID, int _iChannel, int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]);
int __stdcall NetClient_GetVideoCoverSchedule(int _iLogonID, int _iChannel, int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]);
int	__stdcall NetClient_SetCPUMEMAlarmThreshold(int _iLogonID, int _iCPUThreshold, int _iMEMThreshold);
int	__stdcall NetClient_GetCPUMEMAlarmThreshold(int _iLogonID, int *_iCPUThreshold, int *_iMEMThreshold);
int	__stdcall NetClient_SetDZInfo(int _iLogonID, DZ_INFO_PARAM *_pDZ_INFO);
int	__stdcall NetClient_GetDZInfo(int _iLogonID, DZ_INFO_PARAM *_pDZ_INFO);
int	__stdcall NetClient_SetPTZAutoBack(int _iLogonID, int _iChannel,int _iEnable,int _iPresetIndex,int _iIdleTime);
int	__stdcall NetClient_GetPTZAutoBack(int _iLogonID, int _iChannel,int *_iEnable,int *_iPresetIndex,int *_iIdleTime);
int	__stdcall NetClient_Set3GVPND(int _iLogonID, char* _cDialNumber,char* _cAccount,char* _cPassword);
int __stdcall NetClient_Get3GVPND(int _iLogonID, char* _cDialNumber,char* _cAccount,char* _cPassword);
//	HDCamera extend interface
int	__stdcall NetClient_SetHDCamerEx(int _iLogonID, int _iChannel, int _iFuncID, void* _lpBuf, int _iSize);
int	__stdcall NetClient_GetHDCamerEx(int _iLogonID, int _iChannel, int _iFuncID, void* _lpBuf, int _iSize);

//add 20110310-->hyq
int __stdcall NetClient_SetFTPUsage(int _iLogonID, char* _cFtpAddr, int _iPort, char* _cFilePath,char* _cFtpUserName, char* _cFtpPassword,int _iUsage);
int __stdcall NetClient_GetFTPUsage(int _iLogonID, char* _cFtpAddr, int* _piPort, char* _cFilePath,char* _cFtpUserName, char* _cFtpPassword,int* _piUsage);

//sip add by cmz 20110303
int __stdcall NetClient_SetChannelSipConfig(int _iLogonID, int _iChannel, int _iCmd, void* _lpBuf, int _iBufSize);
int __stdcall NetClient_GetChannelSipConfig(int _iLogonID, int _iChannel, int _iCmd, void* _lpBuf, int _iBufSize);

//MaxVideoSize add by cmz 20110305
int __stdcall NetClient_GetMaxVideoSize(int _iLogonID, int* _iVideoSize); 

//BitRatePercent add by cmz 20110308
int __stdcall NetClient_SetBitRatePercent(int _iLogonID, int _iChannelNO, int _iPercent);
int __stdcall NetClient_GetBitRatePercent(int _iLogonID, int _iChannelNO, int* _piPercent);

//GetVideoParam add by cmz 20110308
int __stdcall NetClient_GetVideoParam(unsigned int _uiConnID, int *_piWidth, int *_piHeight, int *_piFrameRate);
//OSDAlpha
int __stdcall NetClient_SetOSDAlpha(int _iLogonID, int _iChannel,int _iAlpha);
int __stdcall NetClient_GetOSDAlpha(int _iLogonID, int _iChannel,int *_iAlpha);
//DeviceSetup add by cmz 20110308
int __stdcall NetClient_DeviceSetup(int _iLogonID, int _iFlag,const char *_pcSection,const char* _pcKeyword,const char* _pcValue);

int __stdcall NetClient_SetPlayerShowFrameMode(int _iLogonID,int _iChannelNum, unsigned int _uiShowFrameMode, int _iStreamNO);

int __stdcall NetClient_GetPlayerShowFrameMode(int _iLogonID,int _iChannelNum,int _iStreamNO);

int __stdcall NetClient_DrawRectOnLocalVideo(unsigned int _uiConID, RECT* _rcRect, int _iCount);

int __stdcall NetClient_DrawPolyOnLocalVideo(unsigned int _uiConnID, POINT* _pPointArray, int _iPointCount, int _iFlag);

int __stdcall NetClient_SendStringToServerEx(int _iLogonID,char* _cMsg,int _iLen, int _iFlag);

int __stdcall NetClient_SetNetFileDownloadFileCallBack(unsigned int _ulConID, RECV_DOWNLOADDATA_NOTIFY _cbkDataNotify, void* _lpUserData);

int __stdcall NetClient_SetDataPackCallBack(unsigned int _ulConID, int _iCBType, void* _pvCallBack, void* _pvUserData);

int __stdcall NetClient_AddConnectionToNetWork(int _iSocket, void *_Connection, void *_NotifyFun);
void* __stdcall NetClient_MallocConnection();
int __stdcall NetClient_FreeConnection(void *_pConnect);

/***************************************************************
Function     ： NetClient_NetFileSetChannelParam
Description: Sets the channel-specific storage policy
Prameter (in): _iLogonID: login ID
_iChannelNo: Channel number
_iCmd: Channel-dependent storage policy
_lpBuf: Stores the corresponding policy structure
return       ：	0:	success
<0:	failed
others       ：	2012-06-27 16:20:00
***************************************************************/
int __stdcall NetClient_NetFileSetChannelParam(int _iLogonID,int _iChannelNo,int _iCmd, void* _lpBuf);

/***************************************************************
Function     ： NetClient_NetFileGetChannelParam
Description: Gets the channel-specific storage policy
Prameter (in): _iLogonID: login ID
_iChannelNo: Channel number
_iCmd: Channel-dependent storage policy
Prameter (in out): _lpBuf: Stores the policy counterpart structure
return       ：	0:	success
<0:	failed
others       ：	2012-06-27 16:20:00
***************************************************************/
int __stdcall NetClient_NetFileGetChannelParam(int _iLogonID,int _iChannelNo,int _iCmd, void* _lpBuf);

/***************************************************************
Function     ：NetClient_ShutDownDev
Description: Turns off
Prameter (in): _iLogonID login ID
               _iFlag Shutdown flag
Prameter(out)：NULL
return       ：0：    success
               other：fail
others       ：2012-02-24 11:28:20
***************************************************************/
int __stdcall NetClient_ShutDownDev(int _iLogonID, int _iFlag);

/*
Function: Backup the current application and resource file to backup the firmware program of the device
*/
int __stdcall NetClient_BackupImage(int _iLogonID , int _iType);


/***************************************************************
Function     ：NetClient_SetLanParam
Description: Set the network card parameters
Prameter (in): _iLogonID login ID
Prameter (in): _ iCmd Sets the suboption
Prameter (in): _ lpData Data pointer
return       ：0： success
other：fail
others       ：2012-02-08 
***************************************************************/
int __stdcall NetClient_SetLanParam(int _iLogonID, int _iCmd, void* _lpData);

/***************************************************************
Function     ：NetClient_GetLanParam
Description: Gets the network card parameters
Prameter (in): _iLogonID login ID
Prameter (in): _ iCmd Get the suboption
Prameter (in out): _ lpData Data pointer
return       ：0： success
other：fail
others       ：2012-02-08 
***************************************************************/
int __stdcall NetClient_GetLanParam(int _iLogonID, int _iCmd, void* _lpData);

/************************************************************************/
// Send a list of supported video sizes for each channel (one for each channel)
//************************************************************************/
/***************************************************************
Function     ：		NetClient_GetVideoSzList
Description: Send the list of supported video sizes for a channel
Prameter (in): _iLogonID: The login ID
_iChannelNo: Channel number
_iStreamNo: Stream number
Prameter (out): _piList: Corresponding resolution list
Prameter (in, out): _piLstCount: list the number of elements ------------- both make parameters, but also do into the parameters, into the reference that the buffer size
Return: 0: success
                    -1: Insufficient buffer
                    other：fail
others       ：2012-02-01 
***************************************************************/
int __stdcall NetClient_GetVideoSzList( int _iLogonID, int _iChannel, int _iStreamNo, int* _piList, int*  _piLstCount );

//add
/***************************************************************
Function     ：	NetClient_SetAlarmConfig
Description: Sets the general alarm interface
Prameter (in): _iLogonID: login ID
_iAlarmType: Alarm type
_iCmdLen: The parameter type
Prameter (in, out): _pvParam: Buffer pointer for alarm template parameters, refer to NetClientTypes.h
return       ：	0： success
				<0:	failed
others       ：2012-2-24 9:48:39
***************************************************************/
int  __stdcall NetClient_SetAlarmConfig( int _iLogonID, int _iChannel, int _iAlarmType, int _iCmd, void* _pvCmdBuf);  //cb@120415

/***************************************************************
Function     ： NetClient_GetAlarmConfig
Description: Generic alarm acquisition interface
Prameter (in): _iLogonID: login ID
_iAlarmType: Alarm type 0
_iCmdLen: The parameter type
Prameter (in, out): _pvParam: Alarm template parameters, refer to NetClientTypes.h
return       ：	0:	success
				<0:	failed
others       ：	2012-2-24 9:51:53
***************************************************************/
int  __stdcall NetClient_GetAlarmConfig(int _iLogonID, int _iChannel, int _iAlarmType, int _iCmd, void* _pvCmdBuf);


//CAUTION：ITS
/************************************************************************
* Camera metering area coordinates
************************************************************************/
/***************************************************************
Function     ：	NetClient_SetITSBlock
Description: Camera metering area coordinate setting interface
Prameter (in): _iLogonID: login ID
_iBlockID: Area number
Prameter (in, out): _iX: X coordinate
_iY: Y coordinate
return       ：	0： success
<0:	failed
others       ：2012-10-18Supplementary explanation
***************************************************************/
int __stdcall NetClient_SetITSBlock(int _iLogonID, int _iBlockID, int _iX, int _iY);

/***************************************************************
Function     ：	NetClient_GetITSBlock
Description: Camera metering area coordinate acquisition interface
Prameter (in): _iLogonID: login ID
_iBlockID: Area number
Prameter (in, out): _iX: X coordinate
_iY: Y coordinate
return       ：	0： success
<0:	failed
others       ：2012-10-18 Supplementary explanation
***************************************************************/
int __stdcall NetClient_GetITSBlock(int _iLogonID, int _iBlockID, int* _iX, int* _iY);

/************************************************************************
* General settings for camera time zone
************************************************************************/
/***************************************************************
Function     ：	NetClient_SetHDTimeRangeParam
Description: Sets the interface for the camera time zone
Prameter (in): _iLogonID: login ID
_iChannel: Channel number
_iIndex: Time range 0-7
                _iBufSize: The size of the structure
Prameter (in, out): _lpBuf: Stores the parameter structure pointer
return       ：	0： success
<0:	failed
others       ：2012-10-18 Supplementary explanation
***************************************************************/
int __stdcall NetClient_SetHDTimeRangeParam(int _iLogonID, int _iChannel, int _iIndex, void* _lpBuf, int _iBufSize);

/***************************************************************
Function     ：	NetClient_GetHDTimeRangeParam
Description: This parameter is used to obtain the interface
Prameter (in): _iLogonID: login ID
_iChannel: Channel number
_iIndex: Time range 0-7
_iBufSize: The size of the structure
Prameter (in, out): _lpBuf: Stores the parameter structure pointer
return       ：	0： success
				<0:	failed
others       ：2012-10-18 Supplementary explanation
***************************************************************/
int __stdcall NetClient_GetHDTimeRangeParam(int _iLogonID, int _iChannel, int _iIndex, void* _lpBuf, int _iBufSize);

/************************************************************************
* HD Camera Set the template name
************************************************************************/
/***************************************************************
Function     ：	NetClient_SetHDTemplateName
Description: Sets the HD camera template name
Prameter (in): _iLogonID: login ID
_iTemplateID: Template number 0-7, supports up to 8 templates.
Prameter (in, out): _cTemplateName: Template name, up to 32 characters
return       ：	0： success
<0:	failed
others       ：2012-10-20 Supplementary explanation
***************************************************************/
int __stdcall NetClient_SetHDTemplateName(int _iLogonID, int _iTemplateID, char* _cTemplateName);

/***************************************************************
Function     ：	NetClient_GetHDTemplateName
Description: HD camera template name acquisition interface
Prameter (in): _iLogonID: login ID
_iTemplateID: Template number 0-7, supports up to 8 templates.
Prameter (in, out): _cTemplateName: Template name, up to 32 characters
return       ：	0： success
<0:	failed
others       ：2012-10-20 Supplementary explanation
***************************************************************/
int __stdcall NetClient_GetHDTemplateName(int _iLogonID, int _iTemplateID,char* _cTemplateName);


/************************************************************************
* HD camera sets the time period template mapping
************************************************************************/
/***************************************************************
Function     ：NetClient_GetHDTemplateMap
Description: This interface is used to capture the mapping relationship between the time zone and the HD camera template
Prameter (in): _iLogonID: login id
_iChannel: Channel number
_iBufSize: The size of the structure
Prameter (out): _ lpBuf: Time range and template mapping
return       ：0：success
               other:failed
others       ：2012-10-21Add@cb
***************************************************************/
int __stdcall NetClient_SetHDTemplateMap(int _iLogonID, int _iChannel, void* _lpBuf, int _iBufSize);

/***************************************************************
Function     ：NetClient_GetHDTemplateMap
Description: This interface is used to capture the mapping relationship between the time range and the HD camera template
Prameter (in): _iLogonID: login id
_iChannel: Channel number
_iBufSize: The size of the structure
Prameter (out): _ lpBuf: Time range and template mapping
return       ：0：success
               other:failed
others       ：2012-10-21Add@cb
***************************************************************/
int __stdcall NetClient_GetHDTemplateMap(int _iLogonID, int _iChannel, void* _lpBuf, int _iBufSize);

/************************************************************************
* The camera time period setting is enabled
************************************************************************/
/***************************************************************
Function     ：NetClient_SetITSTimeRange
Description: Sets the time zone for the camera
Prameter (in): _iLogonID: login id
_iChannel: Channel number
_iIndex: Number of the time range
_iBufSize: The size of the structure
Prameter (out):
_lpBuf: camera time period structure
return       ：0：success
               other:failed
others       ：2012-10-24Add@cb
***************************************************************/
int __stdcall NetClient_SetITSTimeRangeEnable(int _iLogonID, int _iChannel, int _iIndex, void* _lpBuf, int _iBufSize);

/***************************************************************
Function     ：NetClient_SetITSTimeRange
Description: The camera time segment acquisition interface
Prameter (in): _iLogonID: login id
_iChannel: Channel number
_iIndex: Number of the time range
_iBufSize: The size of the structure
Prameter (out):
_lpBuf: camera time period structure
return       ：0：success
               other:failed
others       ：2012-10-24Add@cb
***************************************************************/
int __stdcall NetClient_GetITSTimeRangeEnable(int _iLogonID, int _iChannel, int _iIndex, void* _lpBuf, int _iBufSize);

/************************************************************************
* Camera time setting
************************************************************************/
/***************************************************************
Function     ：NetClient_SetITSTimeRange
Description: Time Stamp Metering Area Enable and Time Stamp The flash enable acquisition interface sets the interface
Prameter (in): _iLogonID: login id
_iIndex: Number of the time range
_iCmd: command number, the internal distinction to achieve
_iBufSize: The size of the structure
Prameter (out):
_lpBuf:
For the time zone metering area enabled:
Bit0: Use metering area 1
Bit1: Use metering area 2
Bit2: Use metering area 3
Bit3: Use metering area 4
Bit4: Use metering area 5
When the flash is enabled for the time zone acquisition interface:
                Whether to enable or not;
return       ：0：success
               other:failed
others       ：2012-10-24Add@cb
***************************************************************/
int __stdcall NetClient_SetITSTimeRange(int _iLogonID, int _iIndex, int _iCmd, void* _lpBuf, int _iBufSize);

/***************************************************************
Function     ：NetClient_GetITSTimeRange
Description: Time Stamp Metering Area Enable and Time Stamp The flash enable acquisition interface
Prameter (in): _iLogonID: login id
_iIndex: Number of the time range
_iCmd: command number, the internal distinction to achieve
_iBufSize: The size of the structure
Prameter (out):
_lpBuf:
For the time zone metering area enabled:
Bit0: Use metering area 1
Bit1: Use metering area 2
Bit2: Use metering area 3
Bit3: Use metering area 4
Bit4: Use metering area 5
When the flash is enabled for the time zone acquisition interface:
Whether to enable or not;
return       ：0：success
               other:failed
others       ：2012-10-24Add@cb
***************************************************************/
int __stdcall NetClient_GetITSTimeRange(int _iLogonID, int _iIndex, int _iCmd, void* _lpBuf, int _iBufSize);

/************************************************************************
* Camera detection mode
************************************************************************/
/***************************************************************
Function     ：NetClient_SetITSTimeRange
Description: Sets the camera detection mode setting interface
Prameter (in): _iLogonID: login id
Prameter (out):
_iMode: Sets the camera detection mode
return       ：0：success
               other:failed
others       ：2012-10-25Add@cb
***************************************************************/
int __stdcall NetClient_SetITSDetectMode(int _iLogonID, int _iMode);

/***************************************************************
Function     ：NetClient_GetITSDetectMode
Description: Sets the camera detection mode setting interface
Prameter (in): _iLogonID: login id
Prameter (out):
_iMode: Sets the camera detection mode
return       ：0：success
               other:failed
others       ：2012-10-25Add@cb
***************************************************************/
int __stdcall NetClient_GetITSDetectMode(int _iLogonID, int* _iMode);

/************************************************************************
* Camera coil operating mode
************************************************************************/
/***************************************************************
Function     ：NetClient_SetITSLoopMode
Description: Coil operating mode setting interface
Prameter (in): _iLogonID: landing id
Prameter (out):
_iLoopMode: Camera coil operating mode
return       ：0：success
               other:failed
others       ：2012-10-26Add@cb
***************************************************************/
int __stdcall NetClient_SetITSLoopMode(int _iLogonID, int _iLoopMode);

/***************************************************************
Function     ：NetClient_GetITSLoopMode
Description: Coil operating mode acquisition interface
Prameter (in): _iLogonID: login id
Prameter (out):
_iLoopMode: Camera coil operating mode
return       ：0：success
               other:failed
others       ：2012-10-26Add@cb
***************************************************************/
int __stdcall NetClient_GetITSLoopMode(int _iLogonID, int* _iLoopMode);

/************************************************************************
* Camera peripheral type
************************************************************************/
/***************************************************************
Function     ：NetClient_SetITSDeviceType
Description: Sets the interface for the camera peripheral type
Prameter (in): _iLogonID: login id
Prameter (out):
_iDeviceType: Device type
return       ：0：success
               other:failed
others       ：2012-10-27Add@cb
***************************************************************/
int __stdcall NetClient_SetITSDeviceType(int _iLogonID, int _iDeviceType);

/***************************************************************
Function     ：NetClient_SetITSDeviceType
Description: Camera peripheral type acquisition interface
Prameter (in): _iLogonID: login id
Prameter (out):
_iDeviceType: Device type
return       ：0：success
               other:failed
others       ：2012-10-27Add@cb
***************************************************************/
int __stdcall NetClient_GetITSDeviceType(int _iLogonID, int* _iDeviceType);

/************************************************************************/
// * intelligent traffic one machine project lane parameter
/************************************************************************/
/***************************************************************
Function     ：NetClient_SetITSRoadwayParam
Description: intelligent Traffic All-in-One Project lane parameter setting interface
Prameter (in): _iLogonID: login id
                _iCmd: protocol to resolve
_iBufSize: The size of the structure
Prameter (out):
_lpBuf: structure
return       ：0：success
               other:failed
others       ：2012-11-01Add@cb
***************************************************************/
int __stdcall NetClient_SetITSRoadwayParam(int _iLogonID, int _iCmd, void * _lpBuf, int _iBufSize);

/***************************************************************
Function     ：NetClient_GetITSRoadwayParam
Description: intelligent Traffic All-in-One Project lane parameter setting interface
Prameter (in): _iLogonID: login id
                _iCmd: protocol to resolve
_iBufSize: The size of the structure
Prameter (out):
_lpBuf: structure
return       ：0：success
               other:failed
others       ：2012-11-01Add@cb
***************************************************************/
int __stdcall NetClient_GetITSRoadwayParam(int _iLogonID, int _iCmd, void * _lpBuf, int _iBufSize);

/************************************************************************/
/* intelligent Vehicle All-in-One Project License Plate Optimization */
/************************************************************************/
/***************************************************************
Function     ：NetClient_SetITSLicensePlateOptimize
Description: intelligent Vehicle All-in-One Project License Plate Optimization interface
Prameter (in): _iLogonID: login id
                _iCmd: protocol to resolve
_iBufSize: The size of the structure
Prameter (out):
_lpBuf: Structure
return       ：0：success
               other:failed
others       ：2012-11-06Add@cb
***************************************************************/
int __stdcall NetClient_SetITSLicensePlateOptimize(int _iLogonID, int _iCmd, void * _lpBuf, int _iBufSize);
/***************************************************************
Function     ：NetClient_GetITSLicensePlateOptimize
Description: intelligent Vehicle All-in-One Project License Plate Optimization Acquisition interface
Prameter (in): _iLogonID: login id
                _iCmd: protocol to resolve
_iBufSize: structure size
Prameter (out):
_lpBuf: structure
return       ：0：success
               other:failed
others       ：2012-11-06Add@cb
***************************************************************/
int __stdcall NetClient_GetITSLicensePlateOptimize(int _iLogonID, int _iCmd, void * _lpBuf, int _iBufSize);

/***************************************************************
Function     ：NetClient_SetITSExtraInfo
Description: Sets the traffic ball interface
Prameter (in): _iLogonID: Login ID

_iCmd input
ITS_ILLEGALPARK_CMD_SET //set illegal parking parameters
ITS_ILLEGALPARKPARM_CMD_SET //set illegal parking calibration area parameters
ITS_LICENSEOPTIMIZEOTHER_CMD_SET // Set the license plate to be retrieved and the untrusted license plate
ITS_OPTIM_CMD_SET //set the first Chinese characters optimized license plate
ITS_RECOPARAM_CMD_SET //set the identification algorithm parameters
ITS_ROADWAYENABLE_CMD_SET // Set the lane
ITS_CAMLOCATION_CMD_SET // Set the device information
ITS_EXACTCONFIRM_CMD_SET // Precise positioning commands
ITS_JPEGSIZEINFO_CMD_SET //Set the camera capture resolution
			
			  _iChannel temporarily unused

_pvCmdBuf The input data
According to different orders corresponding to different structures
ITS_ILLEGALPARK_CMD_SET Use ITS_IllegalPark structure //set illegal parking parameters
ITS_ILLEGALPARKPARM_CMD_SET Use ITS_IllegalParkParm structure //set illegal parking calibration area parameters
ITS_LICENSEOPTIMIZEOTHER_CMD_SET Use ITS_LicenseOptimizeOther //set the plate to be retrieved and not credible license plate
ITS_OPTIM_CMD_SET use SetLicensePlateOptimizeFirstcharacter //set license plate optimization first Chinese characters
ITS_RECOPARAM_CMD_SET Use ITS_RecoParamEx // to set the recognition algorithm parameters
ITS_ROADWAYENABLE_CMD_SET Use TITSRoadwayInfo // to set the lane
ITS_CAMLOCATION_CMD_SET Use ITS_CamLocation // to set the device information
ITS_EXACTCONFIRM_CMD_SET Use ITS_ExactConfirm // to locate the structure precisely
ITS_JPEGSIZEINFO_CMD_SET Use ITS_JPEGSizeInfo // to set the camera capture resolution

_iCmdLen sizeof (structure corresponding to the command) length
return       ：0：success
***************************************************************/
int __stdcall NetClient_SetITSExtraInfo(int _iLogonID, int _iCmd, int _iChannel, void* _pvCmdBuf, int _iCmdLen);

/***************************************************************
Function     ：NetClient_GetITSExtraInfo
Description  ：交通球获取参数接口
Prameter(in): _iLogonID:登录ID

_iCmd input
ITS_ILLEGALPARK_CMD_GET //set illegal parking parameters
ITS_ILLEGALPARKPARM_CMD_GET //set illegal parking area parameters
ITS_LICENSEOPTIMIZEOTHER_CMD_GET // Set the license plate to be retrieved and the untrusted license plate
ITS_OPTIM_CMD_GET //set license plate optimization first Chinese characters
ITS_RECOPARAM_CMD_GET //set the identification algorithm parameters
ITS_ROADWAYENABLE_CMD_GET // Set the lane
ITS_CAMLOCATION_CMD_GET //set the device information

_iChannel is temporarily unused

_pvCmdBuf The input data
According to different orders corresponding to different structures
ITS_ILLEGALPARK_CMD_GET Use ITS_IllegalPark structure //set illegal parking parameters
ITS_ILLEGALPARKPARM_CMD_GET Use ITS_IllegalParkParm structure //set illegal parking calibration area parameters
ITS_LICENSEOPTIMIZEOTHER_CMD_GET Use ITS_LicenseOptimizeOther //set the plate to be retrieved and untrusted license plate
ITS_OPTIM_CMD_GET use SetLicensePlateOptimizeFirstcharacter //set license plate optimization first Chinese characters
ITS_RECOPARAM_CMD_GET Use ITS_RecoParamEx // to set the recognition algorithm parameters
ITS_ROADWAYENABLE_CMD_GET Use TITSRoadwayInfo // to set the lane
ITS_CAMLOCATION_CMD_GET Use ITS_CamLocation // to set the device information
ITS_EXACTCONFIRM_CMD_GET Use ITS_ExactConfirm // to locate the structure precisely

_iCmdLen
Sizeof (the length of the structure corresponding to the command)
return       ：0：success
other:faile
***************************************************************/
int __stdcall NetClient_GetITSExtraInfo(int _iLogonID, int _iCmd, int _iChannel, void* _pvCmdBuf, int _iCmdLen);

// IPC 3MP referance
/***************************************************************
Function     ：NetClient_CheckDeviceState
Description: Check the real-time status of the device
Prameter (in): _iLogonID: logon id
               _iChannelNo: Channel number
_iType: Query item type 0: HD camera real-time brightness value
Prameter(out)：NULL
return       ：0: success
               other:fail
others       ：2011-11-28 16:14:09
***************************************************************/
int __stdcall NetClient_CheckDeviceState(int _iLogonID, int _iChannelNo, int _iType);

/***************************************************************
Function     ：NetClient_GetDeviceState
Description: Call NetClient_CheckDeviceState, after receiving the corresponding message according to the value of the corresponding call to return
Prameter (in): _iLogonID: logon id
_iChannelNo: Channel number
_iType: Query item type 0: HD camera real-time brightness value
Prameter (out): _ iValue: Real time value
return       ：0: success
               other:fail
others       ：2011-11-29 11:21:37
***************************************************************/
int __stdcall NetClient_GetDeviceState(int _iLogonID, int _iChannelNo, int _iType, int *_iValue);

/***************************************************************
Function     ：NetClient_GetCameraCheckInfo
Description: Obtain the dead pixel detection status of the corresponding channel, and call it after receiving the dead point detection message
Prameter (in): _iLogonID: logon id
_iChannelNo: The channel number
_iType: 0: Bad point detection 1: Aperture detection
Prameter (out): _ iValue: Status value
return       ：0： success
               other：fail
others       ：2011-11-29 14:02:47
***************************************************************/
int __stdcall NetClient_GetCameraCheckInfo(int _iLogonID, int _iChannelNo, int _iType, int *_iValue);

/***************************************************************
Function     ：NetClient_CheckCamera
Description  ：check some information of the camera
Prameter (in): _iLogonID: landing id
			   _iChannelNo:channel no
			   _iType:     check type. 0:bad pixel  1:Iris check 
			   _iEnable：  start?（Reserverd）
Prameter(out)：NULL
return       ：0： success
			   other：fail
others       ：2011-11-13 14:27:47
***************************************************************/
int __stdcall NetClient_CheckCamera(int _iLogonID, int _iChannelNo, int _iType, int _iEnable);

/***************************************************************
Function     ：NetClient_GetCharSet
Description: Gets the character set that the current device is using
Prameter (in): _iLogonID: login id
Prameter (out): _ pccharSet: character set
return       ：0：success
				other:failed
others       ：2012-10-17 13:45:04
***************************************************************/
int __stdcall NetClient_GetCharSet(int _iLogonID, char* _pcCharSet);

/***************************************************************
Function     ：NetClient_SetTimeZone
Description: Sets the time zone used by the device
Prameter (in): _iLogonID: login ID
_iTimeZone: Time zone, range [-12,13]
Prameter(out)：NULL
return       ：0:success 
				other:failed
others       ：2012-10-17 13:45:04
***************************************************************/
int __stdcall NetClient_SetTimeZone(int _iLogonID, int _iTimeZone);

/***************************************************************
Function     ：NetClient_GetTimeZone
Description: Gets the time zone used by the device
Prameter (in): _iLogonID: login ID
Prameter (out): _ iTimeZone: Time zone, range [-12,13]
return       ：0：success
				other:failed
others       ：2012-10-17 13:45:04
***************************************************************/
int __stdcall NetClient_GetTimeZone(int _iLogonID, int* _iTimeZone);

/***************************************************************
Function     ：NetClient_SetCurLanguage
Description: Sets the language that the device is currently using
Prameter (in): _iLogonID: login ID
_pcLanguage: Language name
Prameter(out)：NULL
return       ：0：success
				other:failed
others       ：2012-10-17 13:45:04
***************************************************************/
int __stdcall NetClient_SetCurLanguage(int _iLogonID, char* _pcLanguage);

/***************************************************************
Function     ：NetClient_GetCurLanguage
Description: Gets the language the device is currently using
Prameter (in): _iLogonID: login ID
Prameter (out): _ pcLanguage: Language name
return       ：0：success 
				other:failed
others       ：2012-10-17 13:45:04
***************************************************************/
int __stdcall NetClient_GetCurLanguage(int _iLogonID, char* _pcLanguage);

/***************************************************************
Function     ：NetClient_GetLanguageList
Description: Gets the list of languages ??supported by the device
Prameter (in): _iLogonID: login ID
Prameter (out): _ pStrctLanguageList: Language list structure
return       ：0：success
				other:failed
others       ：2012-10-17 13:45:04
***************************************************************/
int __stdcall NetClient_GetLanguageList(int _iLogonID, st_NVSLanguageList* _pStrctLanguageList);

/***************************************************************
Function     ：NetClient_SetChannelEncodeProfile
Description: Sets the channel encoding profile
Prameter (in): _iLogonID: login ID
               _iChannelNum: Channel number
_iStreamNo: Primary stream or secondary stream
			   _iLevel：Profile level （0-baseline  1-main  2-high）
Prameter(out)：NULL
return       ：0：success
               other：fail
others       ：2011-8-17 14:39:09
***************************************************************/
int __stdcall NetClient_SetChannelEncodeProfile(int _iLogonID, int _iChannelNum, int _iStreamNo, int _iLevel);

/***************************************************************
Function     ：NetClient_GetChannelEncodeProfile
Description: Gets the channel encoding profile
Prameter (in): _iLogonID: login ID
               _iChannelNum: Channel number
_iStreamNo: Primary stream or secondary stream
Prameter (out): _ piLevel: Profile level (0-baseline 1-main 2-high)
return       ：0：success
               other:faile
others       ：2011-8-18 13:55:35
***************************************************************/
int __stdcall NetClient_GetChannelEncodeProfile(int _iLogonID, int _iChannelNum, int _iStreamNo, int* _piLevel);

/************************************************************************/
/* Set the alarm       
_iClearType:	ALARM_TYPE_VIDEO_LOST		
				ALARM_TYPE_PORT_ALARM      	
				ALARM_TYPE_MOTION_DETECTION	
				ALARM_TYPE_VIDEO_COVER			
				ALARM_TYPE_EXCPETION			
				ALARM_TYPE_ALL	
				(See NetClientType.h)
return：		0：success
				other:failed

*/
/************************************************************************/
int __stdcall NetClient_SetAlarmClear(int _iLogonID, int _iChannelNo, int _iClearType);

/***************************************************************
Function     ：NetClient_SetExceptionHandleParam
Description: Sets the exception handling parameters
Prameter (in): _iLogonID: login id
               _iExceptionType: Exception type (0, disk full; 1, no disk; 2, disk read and write errors; 3, illegal access; 4, IP conflict;
_iFlag: Processing method (bitwise operation: bit0, enable, if 0, then no treatment will take effect;
Bit1, whether the pop-up prompt dialog box;
Bit2, whether the linkage buzzer;
Bit3, whether to notify the network client)
Prameter(out): NULL
return       ：0:success
               other:fail
others       ：2012/10/22 11:46:34
***************************************************************/
int __stdcall NetClient_SetExceptionHandleParam( int _iLogonID, int _iExceptionType, int _iFlag );

/***************************************************************
Function     ：NetClient_GetExceptionHandleParam
Description: Gets the exception handling parameters
Prameter (in): _iLogonID: login id
                _iExceptionType: Exception type (0, disk full; 1, no disk; 2, disk read and write errors; 3, illegal access; 4, IP conflict;

Prameter (out): _iFlag: Processing mode (bitwise operation: bit0, enable, if 0, then no processing will take effect;
Bit1, whether the pop-up prompt dialog box;
Bit2, whether the linkage buzzer;
Bit3, whether to notify the network client)
return       ：0:success
               other:fail
others       ：2012/10/22 11:46:34
***************************************************************/
int __stdcall NetClient_GetExceptionHandleParam( int _iLogonID, int _iExceptionType, int *_iFlag);

/************************************************************************/
/* Set / get alarm linkage parameters
_iAlarmKLinkType:	ALARMLINKTYPE_VIDEOLOST_LINKRECORD    
					ALARMLINKTYPE_VIDEOLOST_LINKSNAP   	  
					ALARMLINKTYPE_VIDEOLOST_LINKOUTPORT   
					ALARMLINKTYPE_MOTIONDETECT_LINKRECORD 
					ALARMLINKTYPE_MOTIONDETECT_LINKSNAP   
					ALARMLINKTYPE_MOTIONDETECT_LINKOUTPORT
					ALARMLINKTYPE_PORTALARM_LINKRECORD    
					ALARMLINKTYPE_PORTALARM_LINKSNAP      
					ALARMLINKTYPE_PORTALARM_LINKOUTPORT   
					(See NetClientType.h)
_ChannelNo: _iAlarmKLinkType is ALARMLINKTYPE_PORTALARM_LINKRECORD,
										ALARMLINKTYPE_PORTALARM_LINKSNAP、
ALARMLINKTYPE_PORTALARM_LINKOUTPORT This parameter is the input port number
_iAlarmKLinkType This parameter is the channel number for other types

_pParam: Incoming parameters, currently int * _pParam [0] for 1-32 channel enable list, _pParam [1] for 33-64 channel enable list

_iParamSize: _pParam Size

return：		0：success
				other:failed

*/
/************************************************************************/
int __stdcall NetClient_SetAlarmLink_V1(int _iLogonID, int _iChannelNo, int _iAlarmLinkType, void * _pParam, int _iParamSize );
int __stdcall NetClient_GetAlarmLink_V1(int _iLogonID, int _iChannelNo, int _iAlarmLinkType, void * _pParam, int _iParamSize );

/***************************************************************
Function     ：NetClient_SetCameraParam
Description: Sets the general protocol for camera parameters
Prameter (in): _iLogonID: Login ID
_lpBuf: Pointer to the structure TCameraParam
_iBufSize: Size of the structure
return       ：0：success
other:faile
***************************************************************/
int __stdcall NetClient_SetCameraParam(int _iLogonID, void * _lpBuf, int _iBufSize);

/***************************************************************
Function     ：NetClient_GetCameraParam
Description: Gets the general protocol for camera parameters
Prameter (in): _iLogonID: Login ID
_iBufSize: Size of the structure
Prameter (in out): _lpBuf: pointer to the structure TCameraParam
return       ：0：success
other:faile
***************************************************************/
int __stdcall NetClient_GetCameraParam(int _iLogonID, void * _lpBuf, int _iBufSize);

/************************************************************************/
// Set the color parameter COLORPARA
/************************************************************************/
int __stdcall NetClient_SetColorParam(int _iLogonID,int _iChannelNum, int _iColorToGradEnable, int _iDayRange, int _iNightRange);
int __stdcall NetClient_GetColorParam(int _iLogonID,int _iChannelNum, int *_iColorToGradEnable,  int *_iDayRange, int *_iNightRange);

void* __stdcall NetClient_InnerMallocBlock(int _iNeedSize);
int __stdcall NetClient_InnerFreeBlock(void * _pBlock);
int __stdcall NetClient_InnerReferBlock(void * _pBlock);
int __stdcall NetClient_InnerReleaseBlock(void *_pBlock);
int __stdcall NetClient_SetJPEGQuality(int _iLogonID, int _iJpegQuality);
int __stdcall NetClient_GetJPEGQuality(int _iLogonID, int* _iJpegQuality);

/************************************************************************
* Be connected to the device side
************************************************************************/
int __stdcall NetClient_GetConnectInfo(int _iLogonID, void* _lpBuf, int _iSize);

/************************************************************************
* Platform server list
************************************************************************/
int __stdcall NetClient_SetPlatformApp(int _iLogonID, int _iCmd, void* _lpBuf, int _iBufSize);
int __stdcall NetClient_GetPlatformApp(int _iLogonID, int _iCmd, void* _lpBuf, int _iBufSize);


/*
Function: Set manager servers infomation
Parameters: _iLogonID	    logonID
            _cRegSvrIP 	    Register Server IP
            _wdRegPort      Register Server listen port
            _cHeartSvrIP    Heartbeat server IP
            _wdHeartPort    Heartbeat server listen port
            _cAlarmSvrIP    Alarm server IP
            _wdAlarmPort    Alarm server listen port
Return value:  0             Success
          -1             Illegal IP address or not logon server
          -2             Use has no authority
          -3             Illegal sent parameter
*/
int __stdcall NetClient_SetManagerServersInfo(int _iLogonID, char* _cRegSvrIP, unsigned short _wdRegPort,
            char* _cHeartSvrIP, unsigned short _wdHeartPort, char* _cAlarmSvrIP, unsigned short _wdAlarmPort);

/*
Function: Get manager servers infomation
Parameters: _iLogonID	    logonID
            _cRegSvrIP 	    [OUT]Register Server IP
            _wdRegPort      [OUT]Register Server listen port
            _cHeartSvrIP    [OUT]Heartbeat server IP
            _wdHeartPort    [OUT]Heartbeat server listen port
            _cAlarmSvrIP    [OUT]Alarm server IP
            _wdAlarmPort    [OUT]Alarm server listen port
Return value:  0             Success
          -1             Illegal IP address or not logon server
          -2             Use has no authority
          -3             Illegal sent parameter
*/
int __stdcall NetClient_GetManagerServersInfo(int _iLogonID, char* _cRegSvrIP, unsigned short *_wdRegPort,
            char* _cHeartSvrIP, unsigned short *_wdHeartPort, char* _cAlarmSvrIP, unsigned short *_wdAlarmPort);

/*
Function: Set device's ID and name
Parameters: _iLogonID	    logonID
            _cDeviceID 	    NVS device ID, max length 32Bytes
            _cDeviceName    NVS device name, max length 32Bytes
Return value:  0            Success
          -1             Illegal IP address or not logon server
          -2             Use has no authority
          -3             Illegal sent parameter
*/
int __stdcall NetClient_SetDeviceID(int _iLogonID, char* _cDeviceID, char* _cDeviceName, unsigned short _iVspPort, char* _cAccessPass, unsigned short _iVapPort);

/*
Function: Get device's ID and name
Parameters: _iLogonID	    logonID
            _cDeviceID 	    [OUT]NVS device ID
            _cDeviceName    [OUT]NVS device name
Return value:  0            Success
          -1             Illegal IP address or not logon server
          -2             Use has no authority
          -3             Illegal sent parameter
*/
int __stdcall NetClient_GetDeviceID(int _iLogonID, char* _cDeviceID, char* _cDeviceName, unsigned short *_iVspPort, char* _cAccessPass, unsigned short *_iVapPort);

/***************************************************************
Function     ： NetClient_SetATMConfig
Description: Sets the ATM related parameters
Prameter (in): _iLogonID: login ID
_iCmd: Opcode
_lpBuf: the structure of the corresponding opcode pointer
return       ：	0:	success
<0:	failed
others       ：	2012-09-12 10:23:00
***************************************************************/
int __stdcall NetClient_SetATMConfig(int _iLogonID,int _iCmd, void* _lpBuf);

/***************************************************************
Function     ： NetClient_GetATMConfig
Description: Get the ATM related parameters
Prameter (in): _iLogonID: login ID
_iCmd: Opcode
Prameter (in out): _lpBuf: the structure of the corresponding opcode pointer
return       ：	0:	success
<0:	failed
others       ：	2012-09-12 10:23:00
***************************************************************/
int __stdcall NetClient_GetATMConfig(int _iLogonID,int _iCmd, void* _lpBuf);

/***************************************************************
Function     ： NetClient_ATMQueryFile
Description: Query the recording file
Prameter (in): _iLogonID: login ID
_iCmd: Opcode (ATM_QUERY_CMD_FIELD, ATM_QUERY_CMD_CARD)
_lpBuf: Pointer to the structure code for the opcode (ATM_FIELD_QUERY, ATM_CARD_QUERY)
return       ：	0:	success
<0:	failed
others       ：	2012-09-12 10:23:00
***************************************************************/
int __stdcall NetClient_ATMQueryFile(int _iLogonID, int _iCmd, void* _lpBuf);

/***************************************************************
Function     ： NetClient_ATMGetQueryFile
Description: Get the video file information
Prameter (in): _iLogonID: login ID
_iFileIndex: File index
Prameter (out): _pFileData: Record file information
return       ：	0:	success
<0:	failed
others       ：	2012-09-12 10:23:00
***************************************************************/
int __stdcall NetClient_ATMGetQueryFile(int	_iLogonID, int	_iFileIndex, ATM_FILE_DATA* _pFileData);

/***************************************************************
Function     ： NetClient_SetAudioSample
Description: Sets the audio sampling rate related parameters
Prameter (in): _iLogonID: login ID
_iChannel: Channel number
_iCmd: Opcode
_lpBuf: the structure of the corresponding opcode pointer
return       ：	0:	success
<0:	failed
others       ：	2012-09-19 17:00:00
***************************************************************/
int __stdcall NetClient_SetAudioSample(int _iLogonID, int _iChannel,int _iCmd, void* _lpBuf);

/***************************************************************
Function     ： NetClient_GetAudioSample
Description: Get the audio sample rate related parameters
Prameter (in): _iLogonID: login ID
_iChannel: Channel number
_iCmd: Opcode
Prameter (in out): _lpBuf: Pointer to the opcode for the opcode
return       ：	0:	success
<0:	failed
others       ：	2012-09-19 17:00:00
***************************************************************/
int __stdcall NetClient_GetAudioSample(int _iLogonID, int _iChannel,int _iCmd, void* _lpBuf);

/***************************************************************
Function     ：NetClient_SetSystemTypeEx
Description: Sets the system type
Prameter (in): _iLogonID: Login ID
_iType: system type, 0: debugging, 3: bayonet 4: electric police
return       ：0：success
other:faile
***************************************************************/
int __stdcall NetClient_SetSystemTypeEx( int _iLogonID, int _iCmd, void* _pvCmdBuf);

/***************************************************************
Function     ：NetClient_GetSystemTypeEx
Description: Gets the system type
Prameter (in): _iLogonID: Login ID
Prameter (out): _piType: system type, 0: debugging, 3: bayonet 4: electric police
return       ：0：success
other:faile
***************************************************************/
int __stdcall NetClient_GetSystemTypeEx( int _iLogonID, int _iCmd, void* _pvCmdBuf);

/***************************************************************
Function     ：NetClient_SetHXListenPortInfo
Description: Sets the listening port
Prameter (in): _iLogonID: Login ID
_iVideoPort: The video listening port
_iTalkPort: Talkback listening port
_iMsgPort: The command listen port of the device
Prameter (in out): _lpBuf: pointer to the structure TCameraParam
return       ：0：success
other:faile
others       ：	2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_SetHXListenPortInfo(int _iLogonID,int _iVideoPort,int _iTalkPort,int _iMsgPort);

/***************************************************************
Function     ：NetClient_GetHXListenPortInfo
Description: Gets the listening port
Prameter (in): _iLogonID: Login ID
Prameter (out): _ iVideoPort: Video listening port
_iTalkPort: Talkback listening port
_iMsgPort: The command listen port of the device
Prameter (in out): _lpBuf: pointer to the structure TCameraParam
return       ：0：success
other:faile
others       ：	2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_GetHXListenPortInfo(int _iLogonID,int* _iVideoPort,int* _iTalkPort,int* _iMsgPort);

/***************************************************************
Function     ：NetClient_SetVideoModeMethod
Description: Sets the source mode of the video source
Prameter (in): _iLogonID: Login ID
               _iChannel: The channel number is temporarily processed by 0XFFFF, which means it takes effect on the entire device. Others indicate the channel number
               _iMethod: Get method 0, set manually; 1, adaptive;
Prameter(out)：NULL
return       ：0：   success
               other：fail
others       ：	2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_SetVideoModeMethod(int _iLogonID, int _iChannel, VIDEO_METHOD  _iMethod);

/***************************************************************
Function     ：NetClient_GetVideoModeMethod
Description: Sets the source mode of the video source
Prameter (in): _iLogonID: Login ID
               _iChannel: The channel number is temporarily processed by 0XFFFF, which means it takes effect on the entire device. Others indicate the channel number
Prameter (out): * _ piMethod: Get method 0, set manually; 1, adaptive;
return       ：0：   success
               other：fail
others       ：	2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_GetVideoModeMethod(int _iLogonID, int _iChannel, VIDEO_METHOD  *_piMethod);

/***************************************************************
Function     ：NetClient_GetMonitorNum
Description: Obtains the number of monitors
Return:> = 0: The number of monitors
               other：fail
others       ：	2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_GetMonitorNum();

/***************************************************************
Function     ：NetClient_GetMonitorInfo
Description: Gets the monitor information
Prameter (in): _uiIndex: Monitor serial number
Prameter (out): _MonitorInfo: Monitor information
return       ：0：success
               other：fail
others       ：	2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_GetMonitorInfo(unsigned int _uiIndex,MONITOR_INFO* _MonitorInfo);

/***************************************************************
Function     ：NetClient_ChangeMonitor
Description: Modify the monitor information
Prameter (in): _iLogonID: Login ID
_iChannelNum: Channel number
_uiIndex: Monitor serial number
_iStreamNO: The stream type
return       ：0：success
               other：fail
others       ：	2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_ChangeMonitor(int _iLogonID,int _iChannelNum,unsigned int _uiIndex, int _iStreamNO);

/***************************************************************
Function     ：NetClient_EZoomAdd
Description: Adds an electronic zoom
Prameter (in): _uiConnID: Connection ID
_hWnd: Displays the handle
_rctDisplay: Display area
_uiMonitorIndex: Monitor serial number
return       ：>=0：EZoomID
               other：fail
others       ：2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_EZoomAdd(unsigned int _uiConnID, int _hWnd, RECT _rctDisplay, unsigned int _uiMonitorIndex);

/***************************************************************
Function     ：NetClient_EZoomSet
Description: Sets the electronic magnification
Prameter (in): _uiConnID: Connection ID
_iEZoomID: The electronic zoom ID
_rctVideo: Display area
return       ：0：sucess
               other：fail
others       ：2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_EZoomSet(unsigned int _uiConnID, int _iEZoomID, RECT _rctVideo);

/***************************************************************
Function     ：NetClient_EZoomReset
Description: Reinitializes the electronic zoom
Prameter (in): _uiConnID: The connection ID
_iEZoomID: The electronic zoom ID
return       ：0：sucess
               other：fail
others       ：2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_EZoomReset(unsigned int _uiConnID, int _iEZoomID);

/***************************************************************
Function     ：NetClient_EZoomRemove
Description: Deletes the electronic zoom
Prameter (in): _uiConnID: Connection ID
_iEZoomID: The electronic zoom ID
return       ：0：sucess
               other：fail
others       ：2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_EZoomRemove(unsigned int _uiConnID, int _iEZoomID);

/***************************************************************
Function     ：NetClient_SetPsPackCallBack
Description: Sets the callback function for PS stream encapsulation
Prameter (in): _uiConnID: Connection ID
_cbkPsPackNotify: PS stream callback function
_pContext: User data
return       ：0：sucess
               other：fail
others       ：2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_SetPsPackCallBack(unsigned int _ulConID, PSPACK_NOTIFY _cbkPsPackNotify, void*	_pContext);

/***************************************************************
Function     ：NetClient_DCardInit
Description: Initializes the decode card
return       ：>=0：Success, return hardware decode channel number in system, that is, the max video number supported by the system
			   -1：Failure
			   -10：Core program Decoder.out does not exist or illegal format
               other：fail
others       ：2012-11-12 11:23:00
Notes: The interface and NetClient_DCardRelease (Release system recode card)must be used
together; functions related to hardware decode must be used between them, every hardware
channel can decode 1 channel D1, or1 channel Half-D1, or 4 channels Cif.
***************************************************************/
int __stdcall   NetClient_DCardInit();

/***************************************************************
Function     ：NetClient_DCardSetShowMode
Description  ：Set show mode of decode channel
Prameter(in) ：_iCardChannel：Decode card channel number, from 0 to -1
			  _iShowMode：Show mode of the channel
return       ：=0：Success, return file size
				<0 ：Failure
				-1 ：Whether channel number is legal
				-2 ：Whether shoe mode is legal
				-3 ：Showing decode, mode can not be set
				-5 ：Decode card has not been initialized
others       ：2012-11-12 11:23:00
Notes:  The channel can only decode 1 channle video if set 1 channel show, up to 4 channels CIF
can be decoded if set 4 channels show
***************************************************************/
int __stdcall   NetClient_DCardSetShowMode(int	_iCardChannel, int	_iShowMode);

/***************************************************************
Function     ：NetClient_DCardGetShowMode
Description  ：Get show mode of decode channel
Prameter(in) ：_iCardChannel：Decode card channel number, from 0 to -1
Prameter(out)：_iShowMode：Show mode of the channel
return       ：=0：Success, return file size
				<0 ：Failure
				-1 ：Illegal channel
				-5 ：Decode card has not been initialized
others       ：2012-11-12 11:23:00
Notes:  The channel can only decode 1 channle video if set 1 channel show, up to 4 channels CIF
can be decoded if set 4 channels show
***************************************************************/
int __stdcall	NetClient_DCardGetShowMode(int _iCardChannel, int *_iShowMode);

/***************************************************************
Function     ：NetClient_DCardStartPlay
Description  ：Start decord output
Prameter(in) ：_uiConnID：Connection mark, returned parameter when connect video
			   _iCardChannel：Decode channel number, form 0 to -1
			   _iPos：Display position of the channel video, it wil be 0 if show
					   0: Up-left
					   1: Up-right
					   2: Down-left
					   3: Down-right
return       ：0：Success, return file size
				<0 Failure
				-1 Invalid connection mark _ulConID
				-3 Illagal channel number
				-4 Show mode has not been set
				-5 Decode card has not been initialized
others       ：2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_DCardStartPlay(unsigned int	_ulConID, int	_iCardChannel, int	_iPos);

/***************************************************************
Function     ：NetClient_DCardStopPlay
Description  ：Stop decode output
Prameter(in) ：_uiConnID：Connection mark, returned parameter when connect video
			   _iFlag：Stop param
return       ：0：Success, return file size
				<0 Failure
				-1 Invalid connection mark _ulConID
				-5 Decode card has not been initialized
others       ：2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_DCardStopPlay(unsigned int	_ulConID, int	_iFlag);

/***************************************************************
Function     ：NetClient_DCardRelease
Description  ：Destroy system decode card
return       ：>=0：Success, return hardware decode channel number, that is the max video channel number supported by the system
				<0 Failure
				-1 Invalid connection mark _ulConID
				-5 Decode card has not been initialized
others       ：2012-11-12 11:23:00
Notes: The interface and NetClient_DCardRelease (Release system recode card)must be used together;
functions related to hardware decode must be used between them, every hardware channel can decode
1 channel D1, or1 channel Half-D1, or 4 channels Cif.
The three interfaces below are showed decoeded data to the users（NVS stream data used for the type show）
***************************************************************/
int __stdcall NetClient_DCardRelease();

/***************************************************************
Function     ：NetClient_DCardReInit
Description  ：Reinited one decode card
Prameter(in) ：_iCardChannel：Decode channel number, form 0 to -1
return       ：0：Success
				-1：Failure
				-3：Illegal parameters
				-10：Decode card need file:Decoder.out is illegal.
others       ：2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_DCardReInit(int	_iCardChannel);

/***************************************************************
Function     ：NetClient_DCardGetState
Description  ：Get the Decode card working state
Prameter(in) ：_iCardChannel：Decode channel number, form 0 to -1
Prameter(out) ：_iState：working state
return       ：0：Success
				-1：Failure
				-3：Illegal parameters
				-4：the decode card don't support this API
others       ：2012-11-12 11:23:00
Note：  Every time get the _iState will be diffirent.If get the same value in series 2 time, that mean the
card work unormal. can reinited card use API NetClient_DCardReInit
***************************************************************/
int __stdcall NetClient_DCardGetState(int	_iCardChannel, int*	_iState);

/***************************************************************
Function     ：NetClient_DCardStartPlayEx
Description  ：Start decord output
Prameter(in) ：_iId：Connection mark, returned parameter when connect video
			   _dParam：Decode param
return       ：0：Success, return file size
				<0 Failure
				-1 Invalid connection mark _ulConID
				-2 Illegal video frame
				-3 Illagal channel number
				-4 Show mode has not been set
				-5 Decode card has not been initialized
others       ：2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_DCardStartPlayEx(int*	_iId, DecoderParam*	_dParam);

/***************************************************************
Function     ：NetClient_DCardPutDataEx
Description  ：Send external data to decode output
Prameter(in) ：_iId：Connection mark, returned parameter when connect video
			   _cBuf：Index for data buffer
			   _iLen：Data length
return       ：0：Success, return file size
				<0：Failure
				-1：Invalid connection mark _ulConID
				-5：Decode card has not been initialized
others       ：2012-11-12 11:23:00
Notes: External data will pass the interface after decode, send data to decoder output
***************************************************************/
int __stdcall NetClient_DCardPutDataEx(int	_iId, char*	_cBuf, int	_iLen);

/***************************************************************
Function     ：NetClient_DCardStopPlayEx
Description  ：Stop external data decode output
Prameter(in) ：_iId：Connection mark, returned parameter when connect video
			   _iFlag：stop param
return       ：0：Success, return file size
				<0：Failure
				-1：Invalid connection mark _ulConID
				-5：Decode card has not been initialized
others       ：2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_DCardStopPlayEx(int	_iId, int	_iFlag);

/***************************************************************
Function     ：NetClient_DCardStartPlayAudio
Description  ：Start decode card audio output
Prameter(in) ：_uiConnID：Connection mark, returned parameters when connect video
return       ：0：sucess
			  -1：Invalid connection mark _ulConID
			  -5：Decode card has not been initialized
               other：fail
others       ：2012-11-12 11:23:00
***************************************************************/
int __stdcall NetClient_DCardStartPlayAudio(unsigned int _ulConID);

/***************************************************************
Function     ：NetClient_DCardStopPlayAudio
Description  ：Stop decode card audio output
Prameter(in) ：_uiConnID：Connection mark, returned parameters when connect video
return       ：0：sucess
			  -1：Invalid connection mark _ulConID
			  -5：Decode card has not been initialized
               other：fail
others       ：2012-11-12 11:23:00
***************************************************************/
int __stdcall	NetClient_DCardStopPlayAudio(unsigned int _ulConID);

/***************************************************************
Function     ：NetClient_SetEncryptSN
Description: Sets the encrypted serial number
Prameter (in): _iLogonID: Login ID
_iType: encryption type, 0: recognition algorithm encryption type, 1: program encryption type
_cSN: Encrypted sequence number
return       ：0：success
other:faile
***************************************************************/
int __stdcall NetClient_SetEncryptSN( int _iLogonID,int _iType,char* _cSN);

/***************************************************************
Function     ：NetClient_GetSNReg
Description: Get the registration result
Prameter (in): _iLogonID: Login ID
Prameter (out): _piLockRet: Register the result, "bit0: Identify the dog, bit1: intelligent traffic dongle;
return       ：0：success
other:faile
***************************************************************/
int __stdcall NetClient_GetSNReg( int _iLogonID,int* _piLockRet);

/***************************************************************
Function     ：NetClient_GetComFormat_V1
Description: This command is used to get the extended interface of the serial port attribute and increase the query of the protocol name
Prameter (in): _iLogonID: Login ID
_iComPort: serial number
Prameter (out): _cDeviceType: Protocol name
_cCommFormat: Serial port attributes
_iWorkMode: serial port working mode 1: control protocol, 2: transparent channel, 3: industry reservation
return       ：0：success
other:faile
***************************************************************/
int __stdcall NetClient_GetComFormat_V1(int _iLogonID, int _iComPort,char* _cDeviceType,char* _cCommFormat,int* _iWorkMode);

/***************************************************************
Function     ： NetClient_SetComFormat_V2
Description: Sets the serial port format and extends iComType
Prameter (in): _iLogonID: login ID
Prameter (in): _pComFormat: serial format data
return       ：	0:	success
<0:	failed
others       ：	2012-03-06 10:00:00
***************************************************************/
int __stdcall NetClient_SetComFormat_V2(int _iLogonID, COMFORMAT* _pComFormat);

/***************************************************************
Function     ： NetClient_GetComFormat_V2
Description: Obtains the serial port format and extends iComType
Prameter (in): _iLogonID: login ID
Prameter (in out): _pComFormat: serial format data
return       ：	0:	success
<0:	failed
others       ：	2012-03-06 10:00:00
***************************************************************/
int __stdcall NetClient_GetComFormat_V2(int _iLogonID, COMFORMAT* _pComFormat);

/***************************************************************
Function     ：NetClient_GetServerVersion_V1
Description  ：Get the version of plugin
Prameter(in) ：_iLogonID:       logon id       
Prameter(out)：_pstrctServerVer:the struct point of the server version 
return       ：0:success
               other:fail
others       ：2011-10-19 11:27:35
***************************************************************/
int __stdcall NetClient_GetServerVersion_V1(int _iLogonID, SERVER_VERSION *_pstrctServerVer);

/***************************************************************
Function     ：NetClient_InputTalkingdataEx
Description: Send intercom data as much as possible to send 640 bytes of data each time
Prameter (in): _iLogonID: Login ID
_ucData: intercom data
_iLen: intercom data length
return       ：0：success
other:faile
***************************************************************/
int __stdcall NetClient_InputTalkingdataEx(int _iLogonID, unsigned char* _ucData, unsigned int _iLen);

/***************************************************************
Function     ：TC_SetVerticalSync
Description: Sets the vertical sync parameter. Turn on vertical sync to prevent split screen, but will consume a lot of CPU
Prameter (in): _ulConID: Connection mark
_iFlag: Synchronization flag 0: Off 1: On
return       ：0： success
other：fail
others       ：2011-12-07 13:20:00
***************************************************************/
int __stdcall NetClient_SetVerticalSync(unsigned int _ulConID,int _iFlag);

/***************************************************************
Function     ：TC_GetVerticalSync
Description: Gets the vertical synchronization parameter
Prameter (in): _ulConID: Connection mark
Prameter (out): _piFlag: Synchronization flag 0: Off 1: On
return       ：0： success
other：fail
others       ：2011-12-07 13:20:00
***************************************************************/
int __stdcall NetClient_GetVerticalSync(unsigned int _ulConID,int* _piFlag);


/***************************************************************
Function     ：NetClient_SetLocalAudioVolumeEx
Description: Sets the extended interface for the volume
Prameter (in): _uiConID The ConnectID corresponding to this channel video
Prameter (out): _ iVolume: The volume level
return       ：0： success
other：fail
others       ：2011-12-08 19:32:20
***************************************************************/
int __stdcall NetClient_SetLocalAudioVolumeEx(unsigned int _ulConID, int _iVolume);

/***************************************************************
Function     ：NetClient_GetLocalAudioVolumeEx
Description: An extended interface to get the volume
Prameter (in): _uiConID The ConnectID corresponding to this channel video
Prameter (out): _ iVolume: The volume value
return       ：0： success
other：fail
others       ：2011-12-14 
***************************************************************/
int __stdcall NetClient_GetLocalAudioVolumeEx(unsigned int _ulConID, int *_iVolume);

/************************************************************************
* Function: Clears a locally drawn polygon
*	Parameters:
		_uiConnID		ConnectID (from NetClient_StartRecv)
		_iPolygonIndex  Polygon Index(from NetClient_DrawPolyOnLocalVideo)
*	Return:
		>=0		Success
		<0		Error
************************************************************************/
int __stdcall NetClient_ClearPolyLocalVideo(unsigned int _uiConnID, int _iPolygonIndex);

/*
Function: Set video show mode
Parameters: 
_iLogonID       logonID
_iChannelNum The channel number
IOsdType character overlay type, 1: channel name (text) 2: date and time 3: logo color 4: additional characters 5: traffic-specific
ISize: Overlay font size, 1-5 options
Return value:  
				0     Success
<0              Failed
*/
int __stdcall NetClient_SetOSDTypeFontSize(int _iLogonID,int _iChannelNum,int _iOSDType, int _iSize);

/*
Function: Get video show mode .
Parameters: 
_iLogonID       logonID
_iChannelNum The channel number
IOsdType Character Overlay Type, 1: Channel Name 2: Time Date 3: logo Color 4: Additional Characters 5: Traffic Specific
ISize superimposes the font size, 1-5 options
Return value:
				0      Success
<0              Failed
*/
int __stdcall NetClient_GetOSDTypeFontSize(int _iLogonID,int _iChannelNum,int _iOSDType, int* _iSize);

/***************************************************************
Function     ：NetClient_SetImgDisposal
Description: Sets the picture effect processing function
Prameter (in): _iLogonID: Login ID
_lpBuf: TImgDisposal structure pointer
_iBufSize: The size of the structure
return       ：0：success
other:faile
***************************************************************/	
int __stdcall NetClient_SetImgDisposal( int _iLogonID,void * _lpBuf, int _iBufSize);

/***************************************************************
Function     ：NetClient_GetImgDisposal
Description: Gets the picture effect processing function
Prameter (in): _iLogonID: Login ID
_iBufSize: The size of the structure
Prameter (in out): _lpBuf: TImgDisposal Pointer to the structure
return       ：0：success
other:faile
***************************************************************/	
int __stdcall NetClient_GetImgDisposal( int _iLogonID,void * _lpBuf, int _iBufSize);

/************************************************************************/
/* Set mute
_uiConID The return value of NetClient_StartRecv

_iMuted MUTED-Mute NOT_MUTED-Not mute
*/
/************************************************************************/
int __stdcall NetClient_SetMuted(unsigned int _uiConID, int _iMuted);

/***************************************************************
Function     ：NetClient_SetPWMValue
Description: Sets the IR lamp PWM control value
Prameter (in): _iLogonID login ID
               _iChannelNo The channel number
_iValue PWM value
Prameter(out)：NULL
return       ：0：    success
			   other：fail
others       ：2012-02-10 11:28:20
***************************************************************/
int __stdcall NetClient_SetPWMValue(int _iLogonID, int _iChannelNo, int _iValue);   //0210

/***************************************************************
Function     ：NetClient_SetPWMValue
Description: Gets the PWM control value of the infrared lamp
Prameter (in): _iLogonID login ID
_iChannelNo The channel number
_iValue PWM value
Prameter(out)：NULL
return       ：0：    success
               other：fail
others       ：2012-02-10 11:28:20
***************************************************************/
int __stdcall NetClient_GetPWMValue(int _iLogonID, int _iChannelNo, int* _piValue); //0210

/***************************************************************
Function     ：NetClient_SetSystemType
Description: Sets the system type
Prameter (in): _iLogonID: Login ID
_iType: system type, 0: debugging, 3: bayonet 4: electric police
return       ：0：success
other:faile
***************************************************************/
int __stdcall NetClient_SetSystemType( int _iLogonID,int _iType);

/***************************************************************
Function     ：NetClient_GetSystemType
Description: Gets the system type
Prameter (in): _iLogonID: Login ID
Prameter (out): _piType: system type, 0: debugging, 3: bayonet 4: electric police
return       ：0：success
other:faile
***************************************************************/
int __stdcall NetClient_GetSystemType( int _iLogonID,int* _piType);

/************************************************************************
* Set the video and external trigger mode switching time and set the peripheral delay capture time (mainly for radar)
************************************************************************/
int __stdcall NetClient_SetITSSwitchTime(int _iLogonID, int _iSwitchTime, int _iDelayTime);
int __stdcall NetClient_GetITSSwitchTime(int _iLogonID, int* _iSwitchTime, int* _iDelayTime);

/************************************************************************
* Traffic recognition algorithm parameters
************************************************************************/
int __stdcall NetClient_SetITSRecoParam(int _iLogonID, void* _lpBuf, int _iBufSize);
int __stdcall NetClient_GetITSRecoParam(int _iLogonID, void* _lpBuf, int _iBufSize);

/************************************************************************
* Traffic Daytime Night time zone
************************************************************************/
int __stdcall NetClient_SetITSDayNight(int _iLogonID, int _iTimeSegment);
int __stdcall NetClient_GetITSDayNight(int _iLogonID, int* _iTimeSegment);

/************************************************************************
* Traffic camera location
************************************************************************/
int __stdcall NetClient_SetITSCamLocation(int _iLogonID, void* _lpBuf, int _iBufSize);
int __stdcall NetClient_GetITSCamLocation(int _iLogonID, void* _lpBuf, int _iBufSize);

/************************************************************************
* Traffic camera operating mode
************************************************************************/
int __stdcall NetClient_SetITSWorkMode(int _iLogonID, void* _lpBuf, int _iBufSize);
int __stdcall NetClient_GetITSWorkMode(int _iLogonID, void* _lpBuf, int _iBufSize);

/***************************************************************
Function     ：NetClient_SetWaterMarkEnable
Description: Sets the watermark enable
Prameter (in): _iLogonID: Login ID
_iChannelID: lane number, maximum support 4 lanes: 0-3
_iEnable: Enable, 0: Disable; 1: Enable
return       ：0：success
other:faile
***************************************************************/
int __stdcall NetClient_SetWaterMarkEnable( int _iLogonID,int _iChannelID,int _iEnable);

/***************************************************************
Function     ：NetClient_GetWaterMarkEnable
Description: Get the watermark enable
Prameter (in): _iLogonID: Login ID
_iChannelID: lane number, maximum support 4 lanes: 0-3
Prameter (out): _piEnable: Enable, 0: Disable, 1: Enable
return       ：0：success
other:faile
***************************************************************/
int __stdcall NetClient_GetWaterMarkEnable( int _iLogonID,int _iChannelID,int* _piEnable);

/***************************************************************
Function     ：NetClient_SetITSLightInfo
Description: Set the parameters corresponding to the semaphores
Prameter (in): _iLogonID: Login ID
_lpBuf: Pointer to the structure TITSLightInfo
_iBufSize: Size of the structure
return       ：0：success
other:faile
***************************************************************/
int __stdcall NetClient_SetITSLightInfo(int _iLogonID, void * _lpBuf, int _iBufSize);

/***************************************************************
Function     ：NetClient_GetITSLightInfo
Description: Gets the parameter corresponding to the semaphore
Prameter (in): _iLogonID: Login ID
_iBufSize: Size of the structure
Prameter (in out): _lpBuf: Pointer to the structure TITSLightInfo
return       ：0：success
other:faile
***************************************************************/
int __stdcall NetClient_GetITSLightInfo(int _iLogonID, void * _lpBuf, int _iBufSize);

//cb@121205for8127
/***************************************************************
Function     ：NetClient_SetHardWare
Description: Sets the sensor voltage
Prameter (in): _iLogonID: Login ID
               _iCmd: Device configuration command
_lpBuf: Data cache pointer
_iBufSize: The data cache size

For _lpBuf: sensor voltage:
               _iType: device type, 0: reserved; 1: sensor; 2: xxx
               _iMold: Device parameter type, 0: reserved; 1: Voltage; 2: Current
_iValue: Parameter value: different according to different types of values. Voltage unit: V, customer direct transmission, SDK V * 100 to send or receive, the device divided by 100, the float value to set the voltage;
return       ：0：success
               other:faile
***************************************************************/
int __stdcall NetClient_SetHardWareParam( int _iLogonID, int _iCmd, void* _lpBuf, int _iBufSize);

/***************************************************************
Function     ：NetClient_GetHardWare
Description: Gets the sensor voltage
Prameter (in): _iLogonID: Login ID
_iCmd: Device configuration command
_lpBuf: Data cache pointer
_iBufSize: The data cache size

                For _lpBuf: sensor voltage:
_iType: device type, 0: reserved; 1: sensor; 2: xxx
_iMold: Device parameter type, 0: reserved; 1: Voltage; 2: Current
_iValue: Parameter value: different according to different types of values. Voltage unit: V, customer direct transmission, SDK by V * 100 receive, SDK side divided by 100, take the float value to the upper;
return       ：0：success
               other:faile
***************************************************************/
int __stdcall NetClient_GetHardWareParam( int _iLogonID, int _iCmd, void* _lpBuf, int _iBufSize);

/***************************************************************
Function     ：NetClient_SetDomeAdvParam
Description: Sets the advanced parameters of the dome
Prameter (in): _iLogonID: Login ID
_iChannelNO: The channel number
_iCmd: The type of operation
_pvCmdBuf: Pointer to the operation type related parameter
			   _iCmdBufLen：
return       ：0：success
other		 :failed
***************************************************************/
int __stdcall NetClient_SetDomeAdvParam( int _iLogonID, int _iChannelNO, int _iCmd, void* _pvCmdBuf, int _iCmdBufLen);

/***************************************************************
Function     ：NetClient_SetDomeAdvParam
Description: Gets the advanced parameters of the dome
Prameter (in): _iLogonID: Login ID
_iChannelNO: Channel number
_iCmd: The type of operation
_pvCmdBuf: Pointer to the operation type related parameter
			   _iCmdBufLen：
return       ：0：success
other		 :failed
***************************************************************/
int __stdcall NetClient_GetDomeAdvParam( int _iLogonID, int _iChannelNO, int _iCmd, void* _pvCmdBuf, int _iCmdBufLen);

/***************************************************************
Function     ：	NetClient_SetDiskGroup
Description: Sets the disk group information
Prameter (in): _iLogonID: Login ID
_lpBuf: The structure (DISK_GROUP) pointer
_iDiskGroupNum: Number of disk groups, up to 8
Return: 0: correct
Other: Error
others       ：	2013-03-26 
***************************************************************/
int __stdcall NetClient_SetDiskGroup( int _iLogonID,void* _lpBuf, int _iDiskGroupNum);

/***************************************************************
Function     ：	NetClient_GetDiskGroup
Description: Gets the disk group information
Prameter (in): _iLogonID: Login ID
_iDiskGroupNum: Maximum number of disk groups supported by the current device
Prameter (out): _lpBuf: structure (DISK_GROUP) pointer
Return: 0: correct
-1: Insufficient buffer
Other: Error
others       ：	2013-03-26 
***************************************************************/
int __stdcall NetClient_GetDiskGroup( int _iLogonID,void* _lpBuf, int _iDiskGroupNum);

/***************************************************************
Function     ：	NetClient_SetDiskQuota
Description: Sets the disk quota
Prameter (in): _iLogonID: Login ID
_iChannelNo: Channel number
_lpBuf: Structure (DISK_QUOTA) pointer
_iBufSize: The size of the structure
Return: 0: correct
Other: Error
others       ：	2013-03-26 
***************************************************************/
int __stdcall NetClient_SetDiskQuota( int _iLogonID, int _iChannel, void* _lpBuf, int _iBufSize);

/***************************************************************
Function     ：	NetClient_GetQuotaState
Description: Gets the disk quota status for a channel
Prameter (in): _iLogonID: Login ID
_iChannelNo: Channel number (depending on _iCmd)
_iCmd: The command number
CMD_DISK_QUOTA_MAX_USED The maximum quota size for the disk
CMD_DISK_QUOTA_TOTAL The size of the total disk
CMD_DISK_QUOTA_CURRENT_USED The quota size currently used by the disk
_iBufSize: The size of the structure
Prameter (out): _lpBuf: Dependent on _iCmd, see below
Return: 0: correct
-1: Insufficient buffer
Other: Error
others       ：	2013-04-01  
***************************************************************/
int __stdcall NetClient_GetDiskQuotaState ( int _iLogonID, int _iChannelNo, int _iCmd, void* _lpBuf, int _iBufSize );

/***************************************************************
Function     ：		NetClient_ModifyUserAuthority
Description: Modify user rights
Prameter (in): _iLogonID: Login ID
_pcUserName: Non-administrator user name (up to 32 characters)
_lpBuf: Pointer to the structure (USER_AUTHORITY)
_iBufSize: The size of the structure
Return: 0: success
-1: Insufficient buffer
other：fail
others       ：		2013-03-18 
***************************************************************/
int __stdcall NetClient_ModifyUserAuthority( int _iLogonID, char* _pcUserName, void* _lpBuf, int _iBufSize );

/***************************************************************
Function     ：		NetClient_GetUserAuthority
Description: Gets the modified user right
Prameter (in): _iLogonID: Login ID
_pcUserName: Non-administrator user name (up to 32 characters)
_iBufSize: The size of the structure
Prameter (out): _lpBuf: The structure (USER_AUTHORITY) pointer
Return: 0: success
-1: Insufficient buffer
other：fail
others       ：		2013-03-18 
***************************************************************/
int __stdcall NetClient_GetUserAuthority( int _iLogonID, char* _pcUserName, void* _lpBuf, int _iBufSize );

/***************************************************************
Function     ：	NetClient_GetGroupAuthority
Description: Gets the corresponding permission based on the device privilege group
Prameter (in): _iLogonID: Login ID
_iBufSize: The size of the structure
Prameter (in, out): _lpBuf: structure (GROUP_AUTHORITY) pointer, (in) permission group number (out) corresponding authority
Return: 0: correct
-1: Insufficient buffer
Other: Error
others       ：	2013-03-26 
***************************************************************/
int __stdcall NetClient_GetGroupAuthority( int _iLogonID, void* _lpBuf, int _iBufSize);

/***************************************************************
Function     ： NetClient_NetFileGetQueryfileEx
Description: Get the video file information
Prameter (in): _iLogonID: login ID
_iFileIndex: File index
Prameter (in out): _pFileInfo: Record file information structure pointer
return       ：	0:	success
<0:	failed
others       ：	2013-03-27 10:20:00
***************************************************************/
int __stdcall NetClient_NetFileGetQueryfileEx(int _iLogonID,int _iFileIndex, PNVS_FILE_DATA_EX _pFileInfo);

/***************************************************************
Function     ：NetClient_NetFileLockFile
Description: Unlocks the file
Prameter (in): _iLogonID: Login ID
_cFileName: The file name
_iLock: Add unlock state
return       ：	0:	success
<0:	failed
others       ：	2013-03-27 10:20:00
***************************************************************/
int __stdcall NetClient_NetFileLockFile( int _iLogonID, char* _cFileName, int _iLock);

/***************************************************************
Function     ：NetClient_GetOsdTextEx
Description: Get the channel name and color
Prameter (in out): _iLogonID: Login ID
_iChannel: Channel number
_lpBuf: Pointer to the structure
_iBufSize: The size of the structure
return       ：	0:	success
<0:	failed
others       ：	2013-03-14 14:20:00
***************************************************************/
int __stdcall NetClient_GetOsdTextEx(int _iLogonID,int _iChannel,void* _lpBuf, int _iBufSize);

/***************************************************************
Function     ：NetClient_SetHolidayPlan
Description: Set the holiday schedule related parameters
Prameter (in): _iLogonID: Login ID
_iCmd: Opcode (HOLIDAY_PLAN_CMD_SCHEDULE)
_lpBuf: Pointer to the opcode for the structure (HOLIDAYPLAN_SCHEDULE)
_iBufSize: The size of the structure
return       ：	0:	success
<0:	failed
others       ：	2013-03-15 14:20:00
***************************************************************/
int __stdcall NetClient_SetHolidayPlan(int _iLogonID,int _iCmd,void* _lpBuf, int _iBufSize);

/***************************************************************
Function     ：NetClient_GetHolidayPlan
Description: Get the holiday plan related parameters
Prameter (in): _iLogonID: Login ID
_iCmd: Opcode (HOLIDAY_PLAN_CMD_SCHEDULE)
_iBufSize: The size of the structure
Prameter (in out): _lpBuf: Pointer to the opcode corresponding to the structure (HOLIDAYPLAN_SCHEDULE)
return       ：	0:	success
<0:	failed
others       ：	2013-03-15 14:20:00
***************************************************************/
int __stdcall NetClient_GetHolidayPlan(int _iLogonID,int _iCmd,void* _lpBuf, int _iBufSize);

/***************************************************************
Function     ：	NetClient_SetCommonEnable
Description: Enable or disable the general service function
Prameter (in):
_iLogonID: The login ID
_iEnableID: Function ID (refer to the specific value EnableID corresponding table)
_iChannelNo: channel number (according to the actual situation, if the message is not for the channel, then use 0x7FFFFFFF, on behalf of invalid parameters)
_iEnableValue: Enable value (1: enable, 0: disable)
Return: 0: correct
Other: Error
others       ：	2013-10-25 
***************************************************************/
int __stdcall NetClient_SetCommonEnable( int _iLogonID, int _iEnableID, int _iChannel, int _iEnableValue );

/***************************************************************
Function     ：	NetClient_GetCommonEnable
Description: Enables or disables the Universal Service feature
Prameter (in):
_iLogonID: The login ID
_iEnableID: Function ID (refer to the specific value EnableID corresponding table)
_iChannelNo: channel number (according to the actual situation, if the message is not for the channel, then use 0x7FFFFFFF, on behalf of invalid parameters)
Prameter (out): _iEnableValue: enable value (1: enable, 0: disable)
Return: 0: correct
Other: Error
others       ：	2013-10-25 
***************************************************************/
int __stdcall NetClient_GetCommonEnable( int _iLogonID, int _iEnableID, int _iChannel, int* _iEnableValue );

/***************************************************************
Function			：		NetClient_NetFileDownload
Description			:		Download the video file
_ulConID (OUT)		:		The download ID
_iLogonID (IN)		:		Login ID
_iCmd	(IN)		:		The command number
_lpBuf	(IN OUT)	:		the structure of the corresponding command pointer
_iBufSize（IN）		:		The size of the structure
Return				:		0: correct other: wrong
others				：		2013-04-17 
***************************************************************/
int __stdcall NetClient_NetFileDownload(unsigned int* _ulConID, int _iLogonID, int _iCmd, void* _lpBuf, int _iBufSize);


int __stdcall NetClient_Upgrade_V4(int _iLogonID, char* _cFileName, int _iUpgradeType, UPGRADE_NOTIFY_V4 _UpgradeNotify); 

/***************************************************************
Function     ：		NetClient_GetAudioCoderList
Description: Gets the list of audio encodings supported by a channel
Prameter (in): _iLogonID: Login ID
_iChannelNo: Channel number
_iStreamNo: Stream number
Prameter (out): _piList: Corresponding resolution list
Prameter (in, out): _piLstCount: the number of elements list, both make parameters, but also into the Senate, into the reference that the buffer size
Return: 0: success
                    -1: Insufficient buffer
                    other：fail
others       ：		2013-02-18 
***************************************************************/
int __stdcall NetClient_GetAudioCoderList( int _iLogonID, int _iChannel, int _iStreamNo, int* _piList, int* _piLstCount );

// Automatically debug generic protocol (internal) interface specification
/***************************************************************
Function ：	NetClient_InnerAutoTest
Description: General automation settings interface
Prameter (in): _iLogonID: login ID, returned by NetClient_Logon
_iCmd: Debugging entries
_pvTestBuf: debug parameter buffer pointer
_iBufSize: Debug parameter buffer size
return ：	   0：success
			   <0:	failed
others ：      13-05-29 11:48:39
***************************************************************/
int __stdcall NetClient_InnerAutoTest(int _iLogonID, int _iCmd, void* _pvTestBuf, int _iBufSize);

//add by wd @20130621 
/***************************************************************
Function     ：	  NetClient_SetJEPGSize
Description: Sets the resolution of the camera capture picture
Prameter (in): _iLogonID: Login ID
                  _iChannelNo: Channel number 0 ~ n-1
                  _iWidth: Capture picture width - width, pixel value
                  _iHeight: Snap the picture high - high, pixel value
Return: 0: correct
                  Other: Error
***************************************************************/
int __stdcall NetClient_SetJEPGSize(int _iLogonID, int _iChannelNo, int _iWidth, int _iHeight);

/***************************************************************
Function     ：	NetClient_GetJEPGSize
Description: Gets the resolution of the camera capture picture
Prameter (in): _iLogonID: Login ID
                _iChannelNo: Channel number 0 ~ n-1

Prameter (out): _iWidth: Capture picture width - width, pixel value
                _iHeight: Snap the picture high - high, pixel value
Return: 0: correct
                Other: Error
***************************************************************/
int __stdcall NetClient_GetJEPGSize(int _iLogonID, int _iChannelNo, int* _iWidth, int * _iHeight);

/***************************************************************
Function     ：NetClient_QueryDevStatus
Description: Query the device status (CMD * DEVSTATUS * iType for protocol)
Prameter (in): _iLogonID login ID
               _iType Device status 0-red
Prameter (out): NULL
Return: 0 is correct
                -1 failed
***************************************************************/
int __stdcall NetClient_QueryDevStatus(int _iLogonID, int _iType);

/***************************************************************
Function          ： NetClient_GetDevStatus
Description: Get the device status
Prameter (in): _iLogonID login ID
       _iBufSize The size of the structure
Prameter (in / out): _ lpBuf Pointer to the structure STR_DevStatus
Return: 0 is correct
                  -1 failed
***************************************************************/
int __stdcall NetClient_GetDevStatus(int _iLogonID, void* _lpBuf, int _iBufSize);

//add end
/***************************************************************
Function          ： NetClient_GetHDTemplateIndex
Description: Gets the HD template number
Prameter (in): _iLogonID login ID
_iChannelNo Channel number, reserved, defaults to 0
Prameter (in / out): _iTemplateIndex The HD template number
Return: 0 is correct
                    -1 failed
***************************************************************/
int __stdcall NetClient_GetHDTemplateIndex(int _iLogonID, int _iChannelNo /*= 0*/, int* _iTemplateIndex);

/***************************************************************
Function          ： NetClient_SetStreamInsertData
Description: Insert user-defined stream data
Prameter (in): _iLogonID login ID
                     _iType 0 Reserved, starting at 1 for different applications. 1: Synchronous serial number of the binocular camera frame
_pDataBuf User data store
_iDataLen Data length, less than 64 bytes
Return: 0 is correct
                    -1 failed
***************************************************************/
int __stdcall NetClient_SetStreamInsertData(int _iLogonID, int _iChannelNo, int _iFlag, int _iType, void* _pDataBuf, int _iDataLen);

/***************************************************************
Function          ： NetClient_GetStreamInsertData
Description: Get the user-defined bit stream to insert data
Prameter (in): _iLogonID login ID
                     _iType 0 Reserved, starting at 1 for different applications. 1: Synchronous serial number of the binocular camera frame
_pDataBuf User data store
_iDataLen Data length, less than 64 bytes
Return: 0 is correct
                    -1 failed
***************************************************************/
int __stdcall NetClient_GetStreamInsertData(int _iLogonID, int _iChannelNo, int _iFlag, int _iType, void* _pDataBuf, int _iDataLen);

/***************************************************************
Function          ：  NetClient_GetOtherID
Description: Gets the NVS OtherID. (Current ITS camera gets CPUID through this ID).
Prameter (in): _iLogonID login ID
                     _iBufLen Buffer length
Prameter (out) _pcBuff Buffer that stores OtherID.
Return: 0 is correct
                    Other: Error
***************************************************************/
int __stdcall NetClient_GetOtherID(int _iLogonID, void* _pcBuff, int _iBufLen);

/***************************************************************
Function		：	NetClient_SetDomePTZ
Description: Sets the Ptz parameter
Prameter (in): _iLogonID: Login ID
_iChannelNo: Channel number
_lpBuf: structure (DOMEPTZ) pointer
 _iBufSize: The size of the structure
Return: 0: correct
                    -1: Error
***************************************************************/
int __stdcall NetClient_SetDomePTZ(int _iLogonID, int _iChannelNo, void *_lpBuf, int _iBufSize);

/***************************************************************
Function		：	NetClient_GetDomePTZ
Description: Gets the modified user right
Prameter (in): _iLogonID: Login ID
_iChannelNo: Channel number
_lpBuf: structure (DOMEPTZ) pointer
 _iBufSize: The size of the structure
Return: 0: correct
                    -1: Error
***************************************************************/
int __stdcall NetClient_GetDomePTZ(int _iLogonID, int _iChannelNo, void *_lpBuf, int _iBufSize);

/***************************************************************
Function		：	NetClient_GetUserDataInfo
Description: Gets the user data for the live stream being played
Prameter (in): _uiRecvID: The ID returned by NetClient_Startrecv
_iFlag: GET_USERDATA_INFO_MIN ~ GET_USERDATA_INFO_MAX
_pBuffer: Returns the result buffer
 _iSize: Buffer size
Return: 0: correct
                    -1: Error
***************************************************************/
int __stdcall NetClient_GetUserDataInfo(unsigned int _ulConID, int _iFlag, void*  _pBuffer, int _iSize);

/***************************************************************
Function     ：	NetClient_GetBroadcastMessage
Description: Gets a generic broadcast of various messages that are called after receiving the WCM_BROADCAST_MSG message
Prameter (in): _iLogonID: Login ID
_iBufSize: The size of the structure (BROADCAST_MSG)
Prameter (in out): _lpBuf: Pointer to the structure
Return: 0: correct
Other: Error
others       ：	2013-08-20 
***************************************************************/
int __stdcall NetClient_GetBroadcastMessage( int _iLogonID,void *_lpBuf, int _iBufSize);

/***************************************************************
Function     ：	NetClient_GetModuleCapability
Description: Gets the functionality supported by the module
Prameter (in): _iLogonID: Login ID
_iModule: The module number
Prameter (out): _uCaps: module capability, stored in bits
Return: 0: correct
Other: Error
others       ：	2013-08-20 
***************************************************************/
int __stdcall NetClient_GetModuleCapability( int _iLogonID,int _iModule,unsigned int* _uCaps);

/***************************************************************
// The standard version does not support this interface yet
Function     ：	NetClient_KeyboardCtrl
Description: Sends the keypad control protocol
Prameter (in): _cIP: Device IP
_iAction: action code
_iValue: action parameters
Return: 0: correct
Other: Error
***************************************************************/
int __stdcall NetClient_KeyboardCtrl(char* _pcIP, int _iAction,int _iValue);

/***************************************************************
Function     ：	NetClient_NetFileSetSchedule
Description: Set the intelligent recording template and the intelligent recording parameter template
Prameter (in): _iLogonID: Login ID
_iChannel: Channel number
_iCmd: Opcode (TTEMPLATE_CMD_SMAR, TTEMPLATE_CMD_VIDEO)
_lpBuf: Pointer to the structure (SMAR_TTEMPLATE, VIDEO_TEMPLATE)
_iBufSize: The size of the structure
Return: 0: correct
Other: Error
others       ：	2013-09-06
***************************************************************/
int __stdcall NetClient_NetFileSetSchedule(int _iLogonID, int _iChannel,int _iCmd,void *_lpBuf, int _iBufSize);

/***************************************************************
Function     ：	NetClient_NetFileGetSchedule
Description: Obtain the intelligent recording template and the intelligent recording parameter template
Prameter (in): _iLogonID: Login ID
_iChannel: Channel number
_iCmd: Opcode (TTEMPLATE_CMD_SMAR, TTEMPLATE_CMD_VIDEO)
_iBufSize: The size of the structure
Prameter (in out): _lpBuf: Structure (SMAR_TTEMPLATE, VIDEO_TEMPLATE) Pointer
Return: 0: correct
Other: Error
others       ：	2013-09-06
***************************************************************/
int __stdcall NetClient_NetFileGetSchedule(int _iLogonID, int _iChannel, int _iCmd, void *_lpBuf, int _iBufSize);

/***************************************************************
Function	:	NetClient_SetDevConfig
Description:	Set the configuration information of the device
Prameter	:	[IN] _iLogonID: Login ID
				[IN] _iCommand: Device Configuration Commands, see Configuration Commands
				[IN] _iChannel: channel number, if the command does not require the channel number, change the parameter is invalid, set to XFFFFFFFF can
				[IN] _lpInBuffer: Input data buffer pointer
				[IN] _iInBufferSize: The length of the input data buffer, in bytes
Parameter details:	when _iCommand == 100(NET_CLIENT_NTP_INFO): set NTP 
						 _lpInBuffer: input parameter , the variable of struct NTPInfo
						 _iInBufferSize: the size of struct, sizeof(NTPInfo)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.	
					when _iCommand == 232(NET_CLIENT_SHDB_RUNSTATE): set the operation status of Huasan Shanghai landmark 
						 _lpInBuffer: input parameter , the variable of struct ShdbRunState
						 _iInBufferSize: the size of struct, sizeof(ShdbRunState)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
					when _iCommand == 233(NET_CLIENT_SHDB_ALARMPIC): set up Huasan Shanghai landmark alarm linkage  
						 _lpInBuffer: input parameter , the variable of struct ShdbAlarmPic
						 _iInBufferSize: the size of struct, sizeof(ShdbAlarmPic)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
					when _iCommand == 234(NET_CLIENT_SHDB_TIMESNAP): set the daily picture of Huasan Shanghai landmark  
						 _lpInBuffer: input parameter , the variable of struct ShdbTimeSnap
						 _iInBufferSize: the size of struct, sizeof(ShdbTimeSnap)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
					when _iCommand == 235(NET_CLIENT_ALM_LOOP_DETEC):Set whether alarm input loop detection of XNVR is on, inlcude nvr
					     _lpOutBuffer: intput alarm input loop detection enable, the variable of struct AlmLoopDetec
						 _iOutBufferSize: the size of struct, sizeof(AlmLoopDetec)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
					when _iCommand == 236(NET_CLIENT_IDENTIFICATION_TYPE):Set protocol authentication type of XNVR, inlcude nvr
						 _lpOutBuffer: intput protocol and config of Identication type, the first address of struct array IdentificationType 
						 _iOutBufferSize: the size of struct, sizeof(IdentificationType)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
					when _iCommand == 237(NET_CLIENT_DEV_AUTOTIMING): Set the automatic timing time of the device  
						 _lpInBuffer: input parameter , the variable of struct DevAutoTimingParam
						 _iInBufferSize: the size of struct, sizeof(DevAutoTimingParam)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
					when _iCommand == 238(NET_CLIENT_DEVICENAME):Set device name of XNVR, inlcude nvr
						 _lpOutBuffer: input device name, the variable of struct DeviceName
						 _iOutBufferSize: the size of struct, sizeof(DeviceName)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
					when _iCommand == 241(NET_CLIENT_PARSEPARA_IPV6): Set up directory server  
						 _lpInBuffer: input parameter , the variable of struct DomainParsePara
						 _iInBufferSize: the size of struct, sizeof(DomainParsePara)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
					when _iCommand == 242(NET_CLIENT_LOCALSTORE_MAPDEVICE_IPV6): Set mapping network disk   
						 _lpInBuffer: input parameter , the variable of struct NVS_NFS_DEVEx
						 _iInBufferSize: the size of struct, sizeof(NVS_NFS_DEVEx)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
others		:	2019.11.26
					when _iCommand == 245(NET_CLIENT_WATERLEVEL_SOURCE): Set the type of equipment water level data source (integrated water conservancy machine)    
					_lpInBuffer: input parameter , the variable of struct WaterLevelSource
					_iInBufferSize: the size of struct, sizeof(WaterLevelSource)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
					when _iCommand == 246(NET_CLIENT_WATERLEVEL_SOURCE_ID): Set the equipment acceptance water level data source ID (integrated water conservancy machine)     
					_lpInBuffer: input parameter , the variable of struct WaterLevelSourceID
					_iInBufferSize: the size of struct, sizeof(WaterLevelSourceID)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
others		:	2019.12.13
					when _iCommand == 248(NET_CLIENT_OSD_CLARITY): Set the Character overlay background transparency   
					_lpInBuffer: input parameter, the variable of struct OsdClarity
					_iInBufferSize: the size of struct, sizeof(OsdClarity)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
others		:	2020.02.20
***************************************************************/
int __stdcall NetClient_SetDevConfig(int _iLogonID, int _iCommand, int _iChannel, void *_lpInBuffer, int _iInBufferSize);

/***************************************************************
Function	:	NetClient_GetDevConfig
Description:	Gets the configuration information of the device
Prameter	:	[IN] _iLogonID: Login ID
				[IN] _iCommand: Device Configuration Commands, see Configuration Commands
				[IN] _iChannel: channel number, if the command does not require the channel number, change the parameter is invalid, set to XFFFFFFFF can
				[OUT] _lpOutBuffer: Input data buffer pointer
				[IN] _iOutBufferSize: The length of the input data buffer, in bytes
				[OUT] _lpBytesReturned: actually received data length pointer, can not be NULL
Parameter details: when _iCommand == 100(NET_CLIENT_NTP_INFO): get the NTP 
						_lpOutBuffer: output parameter , the variable of struct NTPInfo
						_iInBufferSize: the size of struct, sizeof(NTPInfo)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
				   when _iCommand == 232(NET_CLIENT_SHDB_RUNSTATE): get the operation status of Huasan Shanghai landmark 
						_lpOutBuffer: output parameter , the variable of struct ShdbRunState
						_iInBufferSize: the size of struct, sizeof(ShdbRunState)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
				   when _iCommand == 233(NET_CLIENT_SHDB_ALARMPIC): get the Huasan Shanghai landmark alarm linkage
						_lpOutBuffer: output parameter , the variable of struct ShdbAlarmPic
						_iInBufferSize: the size of struct, sizeof(ShdbAlarmPic)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
				   when _iCommand == 234(NET_CLIENT_SHDB_TIMESNAP): get the daily picture of Huasan Shanghai landmark 
						_lpOutBuffer: output parameter , the variable of struct ShdbTimeSnap
						_iInBufferSize: the size of struct, sizeof(ShdbTimeSnap)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
				   when _iCommand == 235(NET_CLIENT_ALM_LOOP_DETEC):get whether alarm input loop detection of XNVR is on, inlcude nvr
						_lpOutBuffer: output alarm input loop detection enable, the variable of struct AlmLoopDetec
						_iOutBufferSize: the size of struct, sizeof(AlmLoopDetec)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
				   when _iCommand == 236(NET_CLIENT_IDENTIFICATION_TYPE):get Set protocol authentication type of XNVR, inlcude nvr
						_lpOutBuffer: output protocol and config of Identication type, the first address of struct array IdentificationType 
						_iOutBufferSize: the size of struct, sizeof(IdentificationType)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
				   when _iCommand == 237(NET_CLIENT_DEV_AUTOTIMING): get the automatic timing time of the device  
						_lpOutBuffer: output parameter , the variable of struct DevAutoTimingParam
						_iInBufferSize: the size of struct, sizeof(DevAutoTimingParam)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
				   when _iCommand == 238(NET_CLIENT_DEVICENAME):get device name of XNVR, inlcude nvr
						_lpOutBuffer: output device name, the variable of struct DeviceName
						_iOutBufferSize: the size of struct, sizeof(DeviceName)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
				   when _iCommand == 240(NET_CLIENT_CHNDEVINFO): get the device information of the channel 
						_lpOutBuffer: output parameter , the variable of struct ChnDevInfo
						_iInBufferSize: the size of struct, sizeof(ChnDevInfo)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
				   when _iCommand == 241(NET_CLIENT_PARSEPARA_IPV6): get the Registry information  
						_lpOutBuffer: output parameter , the variable of struct DomainParsePara
						_iInBufferSize: the size of struct, sizeof(DomainParsePara)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
				   when _iCommand == 242(NET_CLIENT_LOCALSTORE_MAPDEVICE_IPV6): get the NFS status  
						_lpOutBuffer: output parameter , the variable of struct NVS_NFS_DEVEx
						_iInBufferSize: the size of struct, sizeof(NVS_NFS_DEVEx)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
				   when _iCommand == 243(NET_CLIENT_GET_ENCODERINFO): get encoder device information, inlcude ipc and nvr
						_lpOutBuffer: output encoder device info, the variable of struct EncoderInfoEx
						_iOutBufferSize: the size of struct, sizeof(EncoderInfoEx)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
others		:	2019.11.19
				   when _iCommand == 245(NET_CLIENT_WATERLEVEL_SOURCE): Type of equipment water level data source (integrated water conservancy machine) 
				   _lpOutBuffer: output parameter, the variable of struct WaterLevelSource
                   _iOutBufferSize: the size of struct, sizeof(WaterLevelSource)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
				   when _iCommand == 246(NET_CLIENT_WATERLEVEL_SOURCE_ID): Obtain the ID of the water level data source received by the equipment (integrated water conservancy machine)  
				  _lpOutBuffer: output parameter, the variable of struct WaterLevelSourceID
				  _iOutBufferSize: the size of struct, sizeof(WaterLevelSourceID)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
others		:	2019.12.13
				  when _iCommand == 247(NET_CLIENT_IPV6_ADDRLIST): Obtain the IPv6 address list of network card    
				  _lpOutBuffer: output parameter, the variable of struct IPV6AddrList
				  _iOutBufferSize: the size of struct, sizeof(IPV6AddrList)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
				   when _iCommand == 248(NET_CLIENT_OSD_CLARITY): Obtain the Character overlay background transparency   
				   _lpOutBuffer: output parameter, the variable of struct OsdClarity
				   _iOutBufferSize: the size of struct, sizeof(OsdClarity)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
				   when _iCommand == 249(NET_CLIENT_LEDPOWER_LIMIT): Obtain the Obtain the power consumption limit parameters of the fill lamp    
				   _lpOutBuffer: output parameter, the variable of struct LedPowerLimit
				   _iOutBufferSize: the size of struct, sizeof(LedPowerLimit)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
others		:	2020.02.20
***************************************************************/
int __stdcall NetClient_GetDevConfig(int _iLogonID, int _iCommand, int _iChannel, void *_lpOutBuffer, int _iOutBufferSize, int *_lpBytesReturned);

/***************************************************************
Function	:	NetClient_SendCommand
Description:	send the command information  of the device
Prameter	:	[IN] _iLogonID: Login ID
				[IN] _iCommand: Device Configuration Commands, see Configuration Commands
				[IN] _iChannel: channel number, if the command does not require the channel number, change the parameter is invalid, set to XFFFFFFFF can
				[IN] _pBuffer: Input data buffer pointer
				[IN] _iBufferSize: The length of the input data buffer, in bytes 
Parameter details:	when _iCommand == 85(COMMAND_ID_SHDB_APPREPAIRSYS): Send Huasan Shanghai landmark user repair system request   
						 _lpInBuffer: input parameter , the variable of struct QueryShdbApprepairSys
						 _iInBufferSize: the size of struct, sizeof(QueryShdbApprepairSys)
				return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
					when _iCommand == 86(COMMAND_ID_SHDB_SERVICETYPE): Send Huasan Shanghai landmark maintenance parameters  
						 _lpInBuffer: input parameter , the variable of struct QueryShdbServiceType
						 _iInBufferSize: the size of struct, sizeof(QueryShdbServiceType)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
					when _iCommand == 87(COMMAND_ID_SHDB_TESTMAINTAIN): Send Huasan Shanghai landmark test and maintenance parameters   
						 _lpInBuffer: input parameter , the variable of struct QueryShdbTestMainTain
						 _iInBufferSize: the size of struct, sizeof(QueryShdbTestMainTain)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
					when _iCommand == 88(COMMAND_ID_SHDB_CHECKMANAGE): Send Huasan Shanghai landmark acceptance management parameters   
						 _lpInBuffer: input parameter , the variable of struct QueryShdbCheckManage
						 _iInBufferSize: the size of struct, sizeof(QueryShdbCheckManage)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
others		:	2019.11.26
***************************************************************/
int __stdcall NetClient_SendCommand(int _iLogonID,  int _iCommand,  int _iChannel,  void* _pBuffer,  int _iBufferSize);
int __stdcall NetClient_RecvCommand(int _iLogonID,  int _iCommand,  int _iChannel,  void* _pBuffer,  int _iBufferSize);


/***************************************************************
Function     ：	NetClient_SetDevDiskConfig
Description: Sets the disk configuration information of the device
Prameter (in): [IN] _iLogonID: Login ID
[IN] _iCommand: Device Configuration Commands, see Configuration Commands
                [IN] _iChannel: channel number, if the command does not require the channel number, change the parameter is invalid, set to XFFFFFFFF
[IN] _lpInBuffer: Input data buffer pointer
[IN] _iInBufferSize: The length of the input data buffer, in bytes
Return: 0: correct
Other: Error
others       ：
***************************************************************/
int __stdcall NetClient_SetDevDiskConfig (int _iLogonID, int _iCommand, int _iChannel, void *_lpInBuffer, int _iInBufferSize);

/***************************************************************
Function     ：	NetClient_GetDevDiskConfig
Description: Gets the disk configuration information of the device
Prameter (in): [IN] _iLogonID: Login ID
[IN] _iCommand: Configuration commands, see Configuration Commands
                   [IN] _iChannel: channel number, if the command does not require the channel number, change the parameter is invalid, set to XFFFFFFFF
[OUT] _lpInBuffer: Input data buffer pointer
[IN] _iInBufferSize: The length of the input data buffer, in bytes
[OUT] _lpBytesReturned: actual received data length pointer, can not be NULL
Return: 0: correct
Other: Error
others       ：
***************************************************************/
int __stdcall NetClient_GetDevDiskConfig (int _iLogonID, int _iCommand, int _iChannel, void *_lpOutBuffer, int _iOutBufferSize, int *_lpBytesReturned);


// Active mode optimization new interface
/***************************************************************
Function     ：	NetClient_Logon_V4
Description: Log in to the specified network video server or directory server
Prameter (in): [IN] _iLogonType: connection type, 0-normal mode, 1-active mode, 2-domain name resolution
[IN] _pvPara: Login parameter structure
                   [IN] _iInBufferSize: The length of the input data buffer, in bytes
Return: >=0: correct
Other: Error
others       ：2014-08-01
***************************************************************/
int __stdcall NetClient_Logon_V4(int _iLogonType, void* _pvPara, int _iInBufferSize);

/***************************************************************
Function     ：	NetClient_PlayBackControl
Description  ：	PlayBackControl
Prameter(in) ：		[IN]	_ulConID:Return value NetlClient_PlayBackByName or NetClient_PlayBackByFlag
				    [IN]	_iControlCode： 
                   	[IN]	_pcInBuffer:
					[IN]	_iInLen:
					[IN]    _pcOutBuffer
					[IN]	_iOutLen
return       ：		connect iD
others       ：2015-09-15
***************************************************************/
int __stdcall NetClient_PlayBackControl(unsigned long _ulConID, int _iControlCode, void* _pcInBuffer, int _iInLen, void* _pcOutBuffer, int *_iOutLen);
/***************************************************************
Function     ：	NetClient_StopPlayBack
Description  ：	StopPlayBack
Prameter(in) ：		[IN]	_ulConID:Return value NetlClient_PlayBackByName or NetClient_PlayBackByFlag
return       ：		connect iD
others       ：2015-09-15
***************************************************************/
int __stdcall NetClient_StopPlayBack(unsigned long _ulConID);

/***************************************************************
Function     ：	NetClient_PlayBack
Description  ：	PlayBackByFlag
Prameter(out):      [OUT]   _ulConID return value
Prameter(in) ：		[IN]	_iCmd:Play Back Flag 
				    [IN]	_PlayerParam：_PlayerParam
                   	[IN]	_hWnd:the handle of the play window
return       ：		connect iD
others       ：2015-09-14
***************************************************************/
int __stdcall NetClient_PlayBack(unsigned int* _ulConID, int _iCmd, PlayerParam* _PlayerParam, void* _hWnd);

int __stdcall NetClient_GetPseChInfo(int _iLogonID, void *_lpBuf, int _iBufSize);
int __stdcall NetClient_SetPseChProperty(int _iLogonID, int _iPseCh, void *_lpBuf, int _iBufSize);
int __stdcall NetClient_GetPseChProperty(int _iLogonID, int _iPseCh, void *_lpBuf, int _iBufSize);

//NVR channel talk zyb add 20141219
/***************************************************************
Function     ：	NetClient_ChannelTalkStart
Description: Enables the intercom of the front panel nvr channel
Prameter (in): [IN] _iLogonID: Login ID
[IN] _iChannel: Channel number, starting from 0
                [IN] _iUser: Value 0 or 1,
                0: sdk to collect data, do not need to call NetClient_InputChannelTalkingdata
                1: The upper layer needs to call NetClient_InputChannelTalkingdata to transfer pcm data
Return: 0: correct
Other: Error
Others: none
***************************************************************/
int __stdcall NetClient_ChannelTalkStart(int _iLogonID, int _iChannel, int _iUser);


/***************************************************************
Function     ：	NetClient_ChannelTalkEnd
Description: Ends the intercom with the front-end nvr channel
Prameter (in): [IN] _iLogonID: Login ID
[IN] _iChannel: Channel number, starting from 0
Return: 0: correct.
Other: Error
Others: none
***************************************************************/
int __stdcall NetClient_ChannelTalkEnd(int _iLogonID, int _iChannel);

/***************************************************************
Function     ：	NetClient_InputChannelTalkingdata
Description: Sends audio data
Prameter (in): [IN] _iLogonID: Login ID
[IN] _iChannel: channel number [-1, number of channels -1]
                -1: All front-end nvr talkback channels use the same data (currently only -1).
                [IN] _ucData: Talkback data
                [IN] _iLen: intercom data length
Return: 0: Correct
Other: Error
Others: none
***************************************************************/
int __stdcall NetClient_InputChannelTalkingdata(int _iLogonID, int _iChannel, unsigned char* _ucData, unsigned int _iLen); 

/***************************************************************
Function     ：	NetClient_ChannelTalkEnd
Description: Gets the current intercom status of the NVR channel corresponding to the front end device
Prameter (in): [IN] _iLogonID: Login ID
[IN] _iChannel: Channel number, starting from 0
[OUT] _iTalkState: Talkback status 0: No intercom 1: Requested to start Talkback 2: intercom 3: Has requested to stop Talkback 4: Auto resuming Talkback
Return: 0: correct.
Other: Error
Others: none
***************************************************************/
int __stdcall NetClient_GetChannelTalkingState(int _iLogonID, int _iChannel, int* _iTalkState);
//add end

/***************************************************************
Function     ：	NetClient_CapturePicture
Description: Capture a picture
Prameter (in): [IN] _ulConID: Connection ID
[IN] _iPicType: Image Type 1: BMP 2: JPG
[IN] _cFileName: Image save path name
Return: 0: correct
Other: Error
others       ：2015-03-05
***************************************************************/
int __stdcall NetClient_CapturePicture(unsigned int _ulConID, int _iPicType, char* _pcFileName);

/***************************************************************
Function     ：	NetClient_CapturePicData
Description: Get snapshot image data
Prameter (in): [IN] _ulConID: Connection ID
[IN] _iPicType: Image Type 1: BMP 2: JPG
[OUT] _pPicBuf: Image data
[OUT] _iPicSize: Image data in and out
Return: 0: correct
Other: Error
others       ：2015-09-21
***************************************************************/
int __stdcall NetClient_CapturePicData(unsigned int _ulConID, int _iPicType, char* _pcPicBuf, int* _piPicSize);

int __stdcall NetClient_SetSDKWorkMode(int _iWorkMode);

/***************************************************************
Function     ：	NetClient_Query_V4
Description: Query front-end device files
Prameter (in): [IN] _iLogonID: Login ID
[IN] _iCmd: Device configuration command
[IN] _iChannel: Channel number
[IN] _pvCmdBuf: Input data buffer pointer
[IN] _iBufLen: Input data buffer size
Return 0: correct
Other: Error
others       ：2015-03-05
***************************************************************/
int __stdcall NetClient_Query_V4(int _iLogonID, int _iCmd, int _iChannel, void* _pvCmdBuf, int _iBufLen);

/***************************************************************
Function     ：	NetClient_GetQueryResult_V4
Description: Get the query results
Prameter (in): [IN] _iLogonID: Login ID
[IN] _iCmd: Device configuration command
[IN] _iChannel: Channel number
[IN] _iIndex: query record index number, get the total number of query records and the current number of this parameter is useless
[OUT] _pvCmdBuf: Output data buffer pointer
[IN] _iBufLen: Output data buffer size
Return 0: correct
Other: Error
others       ：2016-05-28
***************************************************************/
int __stdcall NetClient_GetQueryResult_V4(int _iLogonID, int _iCmd, int _iChannel, int _iIndex, void* _pvCmdBuf, int _iBufLen);

/***************************************************************
Function     :	NetClient_RebootDeviceByType
Description  :	Mutil reboot
Prameter(in) :	[IN] _iLogonID: Logon ID
				[IN] _iType:0-device, 1-GUI,2-APP
				[IN] _pvCmdBuf: input space
				[IN] _iBufLen: input space size
return       :	0:ok
				other:error
others       :2015-04-27
***************************************************************/
int __stdcall NetClient_RebootDeviceByType(int _iLogonID, int _iType, void* _pvCmdBuf, int _iBufLen);

/***************************************************************
Function     :	NetClient_StartDownload
Description  :	Start Download
Prameter(in) :	[IN] _iLogonID: Logon ID
				[IN] _iChannel: Channel Number
				[IN] _iDownloadMode:0-file, 1-time span
				[IN] _lpInBuf: input space
				[IN] _iInBufLen: input space size
				[OUT] _pulDownloadFd: output download fd
return       :	>=0:ok
				other:error
others       :2015-06-04
***************************************************************/
int	__stdcall NetClient_StartDownload(int _iLogonID, int _iChannel, int _iDownloadMode, void *_lpInBuf, int _iInBufLen
									  , unsigned long* _pulDownloadFd);
int	__stdcall NetClient_StopDownload(unsigned long _ulDownloadFd);

/***************************************************************
Function     :	NetClient_GetDownloadPos
Description  :	Get Download Pos
Prameter(in) :	[IN] _ulDownloadFd: download fd
				[OUT] _puiDownloadPos: output download pos
return       :	>=0:ok
				other:error
others       :2015-06-04
***************************************************************/
int	__stdcall NetClient_GetDownloadPos(unsigned long _ulDownloadFd, int* _puiDownloadPos);

/***************************************************************
Function     :	NetClient_ProxySend
Description  :	Proxy Send Protocol 
Prameter 	 :	[IN] _iProtocolType
				[IN] _blSend
return       :	>=0:ok             
other:error
others       :	2015.10.13 
***************************************************************/
int	__stdcall NetClient_ProxySend (int _iProtocolType, bool _blSend);

/***************************************************************
Function     :	NetClient_SetUserDataNotify
Description  :	set user data callback function
Prameter(in) :	[IN] _uiRecvID: recv ID,the return value of NetClient_StartRecv interface
				[IN] _cbkUserData: User Data callback function
				[IN] _pvUdata: Udata
return       :	=0:ok
				other:error
others       :2015-12-15
***************************************************************/
int __stdcall NetClient_SetDevUserDataNotify(unsigned int _uiRecvID, DEVUSERDATA_NOTIFY _cbkDevUserData, void* _pvUdata);

/***************************************************************
Function     ：	NetClient_SetZFConfig
Description: Set the political and legal related configuration information
Prameter (in): [IN] _iLogonID: Login ID
[IN] _iCommand: Political configuration commands
                [IN] _iChannel: Channel number
[IN] _lpInBuf: Input Data Buffer Pointer
[IN] _iInBuffSize: buffer length of the input data (in bytes)
return       ：	0：Success , other：Failed
others       ：	2016.04.15
***************************************************************/
int __stdcall NetClient_SetZFConfig(int _iLogonID, int _iCommand, int _iChannel, void *_lpInBuf, int _iInBufSize);

/***************************************************************
Function     ：	NetClient_GetDevConfig
Description: Get configuration information related to politics and law
Prameter: [IN] _iLogonID: Login ID
[IN] _iCommand: Political configuration commands
                [IN] _iChannel: Channel number
[OUT] _lpOutBuf: Input data buffer pointer
[IN] _iOutBufSize: The length of the input data buffer, in bytes
[OUT] _lpRetSize: The actual received data length pointer, can not be NULL
return       ：	0：Success , other：Failed
others       ：	2016.04.15
***************************************************************/
int __stdcall NetClient_GetZFConfig(int _iLogonID, int _iCommand, int _iChannel, void *_lpOutBuf, int _iOutBufSize, int *_lpRetSize);

/***************************************************************
Function     ：	NetClient_SetDsmConfig
Description: set the configuration information about the active mode
Prameter:
[IN] _iCommand: The command code
[IN OUT] _pvBuf: Input Data Buffer Pointer
[IN] _iBufSize: The length of the input data buffer, in bytes
return       ：	0：Success , other：Failed
others       ：	2018.02.28
***************************************************************/
int __stdcall NetClient_SetDsmConfig(int _iCommand, void* _pvBuf, int _iBufSize);

/***************************************************************
Function     ：	NetClient_GetDsmRegstierInfo
Description: Gets the configuration information about the active mode
Prameter:
[IN] _iCommand: The command code
[IN OUT] _pvBuf: Input Data Buffer Pointer
[IN] _iBufSize: The length of the input data buffer, in bytes
return       ：	0：Success , other：Failed
others       ：	2016.06.28
***************************************************************/
int __stdcall NetClient_GetDsmRegstierInfo(int _iCommand, void* _pvBuf, int _iBufSize);

/***************************************************************
Function    ：NetClient_GetRecvInfoById
Description	: Get Recv info by recvid
Prameter:
[IN] _iCommand: The command code
[IN OUT] _pvBuf: Input Data Buffer Pointer
[IN] _iBufSize: The length of the input data buffer, in bytes
return       ：	0：Success , other：Failed
others       ：	2016.11.28
***************************************************************/
int __stdcall NetClient_GetRecvInfoById(int _iRecvId, void* _pvBuf, int _iBufSize);

/***************************************************************
Function    ：NetClient_GetParamFromDevice
Description	: sent light para type
Prameter:
return       ：	0：Success , other：Failed
others       ：	2017.03.22
***************************************************************/
int __stdcall NetClient_GetParamFromDevice(int _iLogonID, int _iChannel, void *_lpBuf, int _iBufSize);

/***************************************************************
Function    ：NetClient_GetRealPlayerIndex
Description	: Get the player handle,call PlaysdkM4 Interface with this handle
Prameter:    _ulConID return from NetClient_StartRecv/NetClient_StartRecvEx/NetClient_StartRecv_V4
return       ：	>=0：Success , other：Failed
others       ：	2017.5.5
***************************************************************/
int __stdcall NetClient_GetPlayerIndex(unsigned int _uiConID);
int __stdcall NetClient_GetRealPlayerIndex(unsigned int* _ulConID);

/***************************************************************
Function    : NetClient_StartRecvNetPicStream
Description	: Start receive net picture stream
Prameter:    [IN]_iLogonID: Login ID
			 [IN]_ptPara: input para
			 [IN]_iBufLen: The length of the input data buffer, in bytes
			 [IN]_fcbPicStreamCallBack: Net receive picture stream callback to user
			 [IN]_pvUser: user data 
			 [OUT]_uiRecvID: Output para ,output current picture stream Identify ID,
return       :	== 0：Success , other：Failed
others       :	2017.7.18
***************************************************************/
int __stdcall NetClient_StartRecvNetPicStream(int _iLogonID, NetPicPara* _ptPara, int _iBufLen, unsigned int* _puiRecvID);

/***************************************************************
Function    : NetClient_StopRecvNetPicStream
Description	: Stop receive net picture stream
Prameter:    [IN]_iPicStreamID: The output para by NetClient_StartRecvNetPicStream's _puiRecvID
return       :	== 0：Success , other：Failed
others       :	2017.7.18
***************************************************************/
int __stdcall NetClient_StopRecvNetPicStream(unsigned int _uiRecvID);


/************************************************************************/
/* Inner func for proxy                                                                     */
/************************************************************************/
int __stdcall NetClient_SetProxyNotifyFunction(MAIN_NOTIFY_V4     _ProxyMainNotify,
											 PROXY_NOTIFY       _ProxyNotify);

int __stdcall NetClient_SetExternDevLogonInfo(unsigned int _uiAllowDevType);

//unipue alert config
int __stdcall NetClient_SetUnipueAlertConfig(int _iLogonId, int _iCmdId, int _iChannel, void* _lpCmdBuf, int _iCmdBufLen);
int __stdcall NetClient_GetUnipueAlertConfig(int _iLogonId, int _iCmdId, int _iChannel, void* _lpCmdBuf, int _iCmdBufLen);

int __stdcall NetClient_FaceConfig(int _iLogonId, int _iCmdId, int _iChanNo, void* _lpIn, int _iInLen, void* _lpOut, int _iOutLen);
int __stdcall NetClient_ZFConfig(int _iLogonId, int _iCmdId, int _iChanNo, void* _lpIn, int _iInLen, void* _lpOut, int _iOutLen);

/***************************************************************
Function    : NetClient_Query_V5
Description	: This interface is no longer maintained. It is recommended to use NetClient_SyncQuery
Prameter:    [IN]_iLogonID: Logon handle 
			 [IN]_iChanNo: channel number
		 	 [IN]_iCmd: query conmand, according to the different functions of _iCmd 
			 when _iCmd == 0(CMD_NETFILE_QUERY_FILE):
				[IN]_pvInPara: input query criteria, struct NETFILE_QUERY_V5 variable
				[IN]_iInLen: the size of struct sizeof(NETFILE_QUERY_V5)
				[OUT]_pvOutPara: output query result, the array of NVS_FILE_DATA, the array size is query page size
				[IN]_iOutLen: sizeof(NVS_FILE_DATA)
			when _iCmd == 7(CMD_NETFILE_QUERY_VCA):
				[IN]_pvInPara: input query criteria, struct NetFileQueryVca variable
				[IN]_iInLen: the size of struct sizeof(NetFileQueryVca)
				[OUT]_pvOutPara: output query result, the array of NetFileQueryVcaResult, the array size is query page size
				[IN]_iOutLen: sizeof(NetFileQueryVcaResult)
			when _iCmd == 8(CMD_NETFILE_QUERY_LOG):
				[IN]_pvInPara: input query criteria, struct NVS_LOG_QUERY variable
				[IN]_iInLen: the size of struct sizeof(NVS_LOG_QUERY)
				[OUT]_pvOutPara: output query result, the array of NVS_LOG_DATA, the array size is query page size 
				[IN]_iOutLen: sizeof(NVS_LOG_DATA)
return       :	== 0：Success; other:Failed, Return Value Description by RetValue.h.
others       :	2019.10.29
***************************************************************/
int __stdcall NetClient_Query_V5(int _iLogonId, int _iCmdId, int _iChanNo, void* _lpIn, int _iInLen, void* _lpOut, int _iOutLen);

int __stdcall NetClient_SetAlarmNotify_V5(ALARM_NOTIFY_V5 _pAlarm);

int	__stdcall NetClient_Upgrade_V5(int _iLogonId, int _iType, void* _lpBuf, int _iBufSize);

int __stdcall NetClient_CmdConfig(int _iLogonId, int _iCmdId, int _iChanNo, void* _lpIn, int _iInLen, void* _lpOut, int _iOutLen);

unsigned long __stdcall NetClient_GetLastError();

//
int __stdcall NetClient_GetConncetInfo(unsigned int _ulConID, int _iCmdId, void* _pvCmdBuf, int _iCmdBufLen);

//Synchronized blocking interface
/***************************************************************
Function    : NetClient_SyncLogon
Description	: Synchronized Blocking Login Interface
Prameter:    [IN]_iLogonType:  Logon type, 0-normal mode, 1-active mode
			 [IN]_pvPara: input para, Logon parameter structure
			            when _iLogonType == SERVER_NORMAL, _pvPara-->LogonPara
						when _iLogonType == SERVER_ACTIVE or _iLogonType == SERVER_REG_ACTIVE, _pvPara-->LogonPara
			 [IN]_iParaSize: The length of the input data buffer, in bytes
return       :	>= 0：Success, return LogonID; other:Failed.
others       :	2019.6.18
***************************************************************/
int	__stdcall NetClient_SyncLogon(int _iLogonType, void* _pvPara, int _iParaSize);

/***************************************************************
Function    : NetClient_SyncRealPlay
Description	: Synchronized Blocking real play audio and video data Interface
Prameter:    [OUT]_puiRecvID: outpt para, output RecvID, one video corresponds to one RecvID 
			 [IN]_ptPara: the struct of NetClientPara, input necessary parameters 
			 [IN]_iParaSize: The length of the struct NetClientPara
return       :	== 0：Success; other:Failed, Return Value Description by RetValue.h.
others       :	2019.6.19
***************************************************************/
int	__stdcall NetClient_SyncRealPlay(unsigned int* _puiRecvID, NetClientPara* _ptPara, int _iParaSize);

/***************************************************************
Function    : NetClient_StopRealPlay
Description	: Synchronized Blocking stop real play audio and video data Interface
Prameter:    [IN]_uiRecvID: the RecvID, NetClient_StartRealPlaySync return value
			 [IN]_iParam: the param, 0--retain the last frame image，1:not retain the last frame image
return       :	== 0：Success; other:Failed, Return Value Description by RetValue.h.
others       :	2019.7.29
***************************************************************/
int	__stdcall NetClient_StopRealPlay(unsigned int _uiRecvID, int _iParam);

/***************************************************************
Function    : NetClient_SyncQuery
Description	: Synchronized Blocking query Interface, according to the different functions of _iCmd, this interface is limited to single-threaded calls 
Prameter:    [IN]_iLogonID: Logon handle 
			 [IN]_iChanNo: channel number
		 	 [IN]_iCmd: query conmand, according to the different functions of _iCmd 
			 when _iCmd == 0(CMD_NETFILE_QUERY_FILE):
				[IN]_pvInPara: input query criteria, struct NETFILE_QUERY_V5 variable
				[IN]_iInLen: the size of struct sizeof(NETFILE_QUERY_V5)
				[OUT]_pvOutPara: output query result, the array of NVS_FILE_DATA, the array size is query page size
				[IN]_iOutTotalLen: The total memory size of _pvOutPara, when query page size is 20, _iOutTotalLen = 20 * sizeof(NVS_FILE_DATA)
				[IN]_iSingleLen: sizeof(NVS_FILE_DATA)
			when _iCmd == 7(CMD_NETFILE_QUERY_VCA):
				[IN]_pvInPara: input query criteria, struct NetFileQueryVca variable
				[IN]_iInLen: the size of struct sizeof(NetFileQueryVca)
				[OUT]_pvOutPara: output query result, the array of NetFileQueryVcaResult, the array size is query page size
				[IN]_iOutTotalLen: The total memory size of _pvOutPara, when query page size is 20, _iOutTotalLen = 20 * sizeof(NetFileQueryVcaResult)
				[IN]_iSingleLen: sizeof(NetFileQueryVcaResult)
			when _iCmd == 8(CMD_NETFILE_QUERY_LOG):
				[IN]_pvInPara: input query criteria, struct NVS_LOG_QUERY variable
				[IN]_iInLen: the size of struct sizeof(NVS_LOG_QUERY)
				[OUT]_pvOutPara: output query result, the array of NVS_LOG_DATA, the array size is query page size 
				[IN]_iOutTotalLen: The total memory size of _pvOutPara, when query page size is 20, _iOutTotalLen = 20 * sizeof(NVS_LOG_DATA)
				[IN]_iSingleLen: sizeof(NVS_LOG_DATA)
return       :	== 0：Success; other:Failed, Return Value Description by RetValue.h.
others       :	2019.10.29
***************************************************************/
int	__stdcall NetClient_SyncQuery(int _iLogonID, int _iChanNo, int _iCmd, void* _pvInPara, int _iInLen, void* _pvOutPara, int _iOutTotalLen, int _iSingleLen);

/***************************************************************
Function    : NetClient_SyncSetDevCfg
Description	: Synchronized set device config, according to the different functions of _iCmd, this interface is limited to single-threaded calls
Prameter:    [IN]_iLogonID: Logon handle 
			 [IN]_iChanNo: channel number
		 	 [IN]_iCmd: query conmand, according to the different functions of _iCmd 
			 when _iCmd == 0(SYNC_NET_CLIENT_VCA_SUSPEND):
				[IN]_pvInPara: input para, struct VCASuspend
				[IN]_iInLen: the length of the _pvInPara, tsizeof(VCASuspend)
				[IN]_pvOutRet: output set result, the struct VCASuspendResult
				[IN]_iOutLen: the length of the _pvOutPara, sizeof(VCASuspendResult)
return       :	== 0：Success; other:Failed, Return Value Description by RetValue.h.
others       :	2019.7.17
***************************************************************/
int	__stdcall NetClient_SyncSetDevCfg(int _iLogonID, int _iChanNo, int _iCmd, void* _pvInPara, int _iInLen, void* _pvOutRet, int _iOutLen);

/***************************************************************
Function    : NetClient_CapturePicByDevice
Description	: Real-time return of device capture results.
Prameter:    [IN]_iLogonID: Logon handle 
			 [IN]_iChanNo: channel number
		 	 [IN]_iQvalue: Q-value 

			 [OUT]_pcPicFilePath: save file path
			 [OUT]_ptSnapPicData: picture struct use save picture data
			 when _ptSnapPicData==NULL:don't save picture data
			 when _ptSnapPicData!=NULL:Need to actively malloc/free picture data
				SnapPicData tSnapPicData = { 0 };
				tSnapPicData.ptPicData = (PicData*)malloc(sizeof(PicData)); 
				tSnapPicData.ptPicData->pcPicData = (char*)malloc(1024 * 800 * 2);	//800K*2 Maximum frame length
				tSnapPicData.ptPicData->iDataLen = 1024 * 800 * 2;
			 [IN]_iInSize: picture struct size
return       :	== 0：Success; other:Failed, Return Value Description by RetValue.h.
others       :	2019.9.20
***************************************************************/
int	__stdcall NetClient_CapturePicByDevice(int _iLogonID, int _iChanNo, int _iQvalue, char* _pcPicFilePath, SnapPicData* _ptSnapPicData, int _iInSize);

/***************************************************************
Function	:	NetClient_SetSDKInitConfig
Description:	Set SDK initialization parameters.Called before interface (NetClient_Startup).
Prameter	:	[IN] _iCmd: SDK initialization parameters Commands
				[IN] _lpInBuffer: Input data buffer pointer
				[IN] _iInBufferSize: The length of the input data buffer, in bytes
Parameter details: when _iCmd == (INIT_CONFIG_LOCAL_LIBRARY_PATH): Set the library load path
						_lpInBuffer: LocalSDKPath
						_iInBufferSize: the size of struct, sizeof(LocalSDKPath)
return		:	== 0：Success; other:Failed, Return Value Description by RetValue.h.
others		:	2019.11.21
***************************************************************/
int	__stdcall NetClient_SetSDKInitConfig(int _iCmd, void *_lpInBuffer, int _iInBufferSize);

/***************************************************************
Function    : NetClient_SetAVMode
Description	: Set Audio and Video mode parameters.
Prameter:    [IN]_iRecvID: Retain. value: -1
			 [IN]_iCmd: Audio and Video parameters Commands
			 [IN]_pvBuf: Input data buffer pointer
			 [IN]_iBufSize: the size of _pvBuf
Parameter details: when _iCmd == (CMD_AV_DEC_SHOW_MODE): Set decode mode and show mode
						_lpInBuffer: is AVDecodeMode
						_iInBufferSize: sizeof(AVDecodeMode)
Parameter details: when _iCmd == (CMD_AV_DEC_HWDECODE_PARAM): Set Hw parm
						_lpInBuffer: is HwDecodeParam
						_iInBufferSize: sizeof(HwDecodeParam)
Parameter details: other _iCmd == (CMD_VIDEO_SHOW_MAIN_MODE | CMD_VIDEO_SHOW_SUB_MODE | CMD_AVMODE_VIDEO_DECLIB | CMD_AVMODE_AUDIOCOMMON_DECLIB | CMD_AVMODE_AUDIOAAC_DECLIB | CMD_AVMODE_AUDIO_PLAY_MODE)
						_lpInBuffer: is int

return       :	== 0：Success; other:Failed, Return Value Description by RetValue.h.
others       :	2019.11.27
***************************************************************/
int __stdcall NetClient_SetAVMode(int _iRecvID, int _iCmd, void* _pvBuf, int _iBufSize);


/***************************************************************
Function	:	NetClient_CreateQtWidget
Description:	Create a QT widget.
Prameter	:	void
return		:	== other：Success; NULL:Failed.
others		:	2019.12.11
***************************************************************/
void* __stdcall NetClient_CreateQtWidget(void* _pvParaBuf, int _iBufSize);


/***************************************************************
Function	:	NetClient_ReleaseQtWidget
Description:	Release a QT widget.
Prameter	:	[IN] _pvQWidget: Release widget
return       :	== 0：Success; other:Failed, Return Value Description by RetValue.h.
others		:	2019.12.11
***************************************************************/
int __stdcall NetClient_ReleaseQtWidget(void* _pvQWidget);

/***************************************************************
Function	:	NetClient_GetDevConfig_V5
Description:	Gets the configuration information of the device
Prameter	:	[IN] _iLogonID: Login ID
				[IN] _iCmd: Device Configuration Commands, see Configuration Commands
				[IN] _pvInParaBuf: Input data buffer pointer
				[IN] _iInBufLen: The length of the input data buffer, in bytes
				[OUT] _pvOutParaBuf: output para data pointer, can not be NULL
				[IN] _iOutBufLen: The length of the output data buffer, in bytes
Parameter details: when _iCmd == 0(NETCLIENT_V5_GET_HARDDISKINFO): get the nvr hard disk information
						_pvInParaBuf: Useless parameter, set be NULL
						_iInBufLen: Useless parameter, set be 0
						_pvOutParaBuf: output parameter , the variable of struct HardDiskInfo
						_iOutBufLen: the size of struct, sizeof(HardDiskInfo).
others		:	2020.01.06
***************************************************************/
int __stdcall NetClient_GetDevConfig_V5(int _iLogonID, int _iCmd, void* _pvInParaBuf, int _iInBufLen, void* _pvOutParaBuf, int _iOutBufLen);

/***************************************************************
Function	:	NetClient_SycStartVoiceTalk
Description:	talk with device
Prameter	:	[OUT] _puiTalkID: talk id
				[IN] _ptPara: talk params
				[IN] _iSize: sizeof NetVoiceTalkPara
return		:	>=0 seccess, <0,failed,also see description by RetValue.h
others		:	2020.01.13
***************************************************************/

int __stdcall NetClient_SycVoiceTalkStart(unsigned int* _puiTalkID, NetVoiceTalkPara* _pvPara, int _iSize);

int __stdcall NetClient_SycVoiceTalkStop(unsigned int _uiTalkID, void* _pvPara, int _iSize);

int __stdcall NetClient_SycVoiceTalkInputData(NetVoiceTalkInput* _pvPara, int _iSize);

/************************************************************************************
Function	:	NetClient_HttpXmlConfig
Description	:	ISAPI/CGI protocol command transparent transmission 
Parameter	:	[IN]_iLogonID: Login handle 
				[IN] _ptXmlInPara: Input parameter structure 		 	  
				[IN]_iInSize: Input parameter structure size 
				[IN] _ptXmlOutPara: Output parameter structure 
		 		[IN] _iOutSize: Output parameter structure size
return		:	== 0 Success; other:Failed, Return Value Description by RetValue.h.
others      :	2020.05.09
************************************************************************************/
int __stdcall NetClient_HttpXmlConfig(int _iLogonID, XmlCfgInPara* _ptXmlInPara, int _iInSize, XmlCfgOutPara* _ptXmlOutPara, int _iOutSize);

#ifdef __cplusplus
}
#endif
#endif

