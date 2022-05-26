#ifndef NVSSDK_INTERFACE_H
#define NVSSDK_INTERFACE_H

#include <windows.h>
#include "NetClientTypes.h"
#include <stdio.h>
#include "../Common/CommonFun.h"
#include "GlobalTypes.h"
#include "AVPlaySdkTypes.h"


namespace NVSSDK_INTERFACE
{
	int 	LoadNVSSDK(const char* _pszPath = "nvssdk.dll");
	bool    ReleaseNVSSDK();

	TYPEDEF_FUNCTION(NetClient_SetPort)(int _iServerPort,int _iClientPort);
	DECLARE_FUNCTION(NetClient_SetPort);
	TYPEDEF_FUNCTION(NetClient_SetMSGHandle)(UINT _uiMessage, HWND _hWnd, UINT _uiParaMsg, UINT _uiAlarmMsg);
	DECLARE_FUNCTION(NetClient_SetMSGHandle);
	TYPEDEF_FUNCTION(NetClient_Startup)();
	DECLARE_FUNCTION(NetClient_Startup);
	TYPEDEF_FUNCTION(NetClient_Cleanup)();
	DECLARE_FUNCTION(NetClient_Cleanup);
	TYPEDEF_FUNCTION(NetClient_SetNotifyFunction)(LOGON_NOTIFY _LogonNotify,ALARM_NOTIFY _AlarmNotify,PARACHANGE_NOTIFY _ParaNotify);
	DECLARE_FUNCTION(NetClient_SetNotifyFunction);
	TYPEDEF_FUNCTION(NetClient_Logon)(char* _cProxy,char* _cIP,char* _cUserName,char* _cPassword,char* _pcProID,int _iPort);
	DECLARE_FUNCTION(NetClient_Logon);
	TYPEDEF_FUNCTION(NetClient_GetLogonStatus)( int _iLogonID);
	DECLARE_FUNCTION(NetClient_GetLogonStatus);
	TYPEDEF_FUNCTION(NetClient_StartRecv)(unsigned int *_ulConID, CLIENTINFO* _cltInfo, RECVDATA_NOTIFY _cbkDataArrive);
	DECLARE_FUNCTION(NetClient_StartRecv);
	TYPEDEF_FUNCTION(NetClient_StartCaptureData)(unsigned long _ulConID);
	DECLARE_FUNCTION(NetClient_StartCaptureData);
	TYPEDEF_FUNCTION(NetClient_StopCaptureData)(unsigned long _ulConID);
	DECLARE_FUNCTION(NetClient_StopCaptureData);
	TYPEDEF_FUNCTION(NetClient_StartRecvNetPicStream)(int _iLogonID, NetPicPara* _ptPara, int _iBufLen, unsigned int* _puiRecvID);
	DECLARE_FUNCTION(NetClient_StartRecvNetPicStream);
	TYPEDEF_FUNCTION(NetClient_StopRecvNetPicStream)(unsigned int _uiRecvID);
	DECLARE_FUNCTION(NetClient_StopRecvNetPicStream);	
	TYPEDEF_FUNCTION(NetClient_GetVideoHeader)(unsigned long _ulConID,unsigned char *ucHeader);
	DECLARE_FUNCTION(NetClient_GetVideoHeader);
	TYPEDEF_FUNCTION(NetClient_StartCaptureFile)(unsigned int _uiRecvID, char* _pszFilePath, int _iRecordFileType);
	DECLARE_FUNCTION(NetClient_StartCaptureFile);
	TYPEDEF_FUNCTION(NetClient_StopCaptureFile)(unsigned long _ulConID);
	DECLARE_FUNCTION(NetClient_StopCaptureFile);
	TYPEDEF_FUNCTION(NetClient_GetCaptureStatus)(unsigned long _ulConID);
	DECLARE_FUNCTION(NetClient_GetCaptureStatus);
	TYPEDEF_FUNCTION(NetClient_StopPlay)(unsigned long _ulConID);
	DECLARE_FUNCTION(NetClient_StopPlay);
	TYPEDEF_FUNCTION(NetClient_StopPlayEx)(unsigned long _ulConID, unsigned long _ulParam);
	DECLARE_FUNCTION(NetClient_StopPlayEx);
	TYPEDEF_FUNCTION(NetClient_StartPlay)(unsigned long _ulConID,HWND _hWnd, RECT _rcShow, unsigned int _iDecflag /*= H264DEC_DECTWO*/);
	DECLARE_FUNCTION(NetClient_StartPlay);
	TYPEDEF_FUNCTION(NetClient_ResetPlayerWnd)(unsigned long _ulConID,HWND _hwnd);
	DECLARE_FUNCTION(NetClient_ResetPlayerWnd);
	TYPEDEF_FUNCTION(NetClient_GetPlayingStatus)(unsigned long _ulConID);
	DECLARE_FUNCTION(NetClient_GetPlayingStatus);
	TYPEDEF_FUNCTION(NetClient_SetDecCallBack)(unsigned long _ulConID, DECYUV_NOTIFY _cbkGetYUV, void* _pContext /*= NULL*/);
	DECLARE_FUNCTION(NetClient_SetDecCallBack);
	TYPEDEF_FUNCTION(NetClient_SetDecCallBack_V4)(unsigned int _ulConID, DECYUV_NOTIFY_V4 _cbkGetYUV, int _iUserData);
	DECLARE_FUNCTION(NetClient_SetDecCallBack_V4);
	TYPEDEF_FUNCTION(NetClient_SendStringToServer)(int _iLogonID,char* _cMsg,int _iLen);
	DECLARE_FUNCTION(NetClient_SendStringToServer);
	TYPEDEF_FUNCTION(NetClient_ReceiveString)(char* _cIpAddress,int* _iType,char* _cMsg,int* _iLen);
	DECLARE_FUNCTION(NetClient_ReceiveString);

	TYPEDEF_FUNCTION(NetClient_SetLocalAudioVolume)(int _iVolume);
	DECLARE_FUNCTION(NetClient_SetLocalAudioVolume);
	TYPEDEF_FUNCTION(NetClient_AudioStart)(unsigned long _ulConID);
	DECLARE_FUNCTION(NetClient_AudioStart);
	TYPEDEF_FUNCTION(NetClient_AudioStop)(unsigned long _ulConID);
	DECLARE_FUNCTION(NetClient_AudioStop);
	TYPEDEF_FUNCTION(NetClient_SetBufferNum)(unsigned long _ulConID,int _iNum);
	DECLARE_FUNCTION(NetClient_SetBufferNum);
	TYPEDEF_FUNCTION(NetClient_SetPlayDelay)(unsigned long _ulConID,int _iNum);
	DECLARE_FUNCTION(NetClient_SetPlayDelay);
	TYPEDEF_FUNCTION(NetClient_GetDecordFrameNum)(unsigned long _ulConID);
	DECLARE_FUNCTION(NetClient_GetDecordFrameNum);
	TYPEDEF_FUNCTION(NetClient_GetAlarmPortNum)(int _iLogonID, int* _iAlarmInPortNum,int* _iAlarmOutPortNum);
	DECLARE_FUNCTION(NetClient_GetAlarmPortNum);
	typedef int (__stdcall *pfNetClient_SetVideoPara)(int _iLogonID, int _iChannelNum,STR_VideoParam* _strVideoParam);
	extern	pfNetClient_SetVideoPara	NetClient_SetVideoPara /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetVideoPara)(int _iLogonID,int _iChannelNum,STR_VideoParam* _strVideoParam);
	extern	pfNetClient_GetVideoPara	NetClient_GetVideoPara /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetVideoQuality)(int _iLogonID,int _iChannelNum,int _iVideoQuality, int _iStreamNO/*=MAIN_STREAM*/);
	extern	pfNetClient_SetVideoQuality	NetClient_SetVideoQuality /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetVideoQuality)(int _iLogonID,int _iChannelNum,int* _iVideoQuality, int _iStreamNO/*=MAIN_STREAM*/);
	extern	pfNetClient_GetVideoQuality	NetClient_GetVideoQuality /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetFrameRate)(int _iLogonID,int _iChannelNum,int _iFrameRate, int _iStreamNO/*=MAIN_STREAM*/);
	extern	pfNetClient_SetFrameRate	NetClient_SetFrameRate /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetFrameRate)(int _iLogonID,int _iChannelNum,int* _iFrameRate, int _iStreamNO/*=MAIN_STREAM*/);
	extern	pfNetClient_GetFrameRate	NetClient_GetFrameRate /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetStreamType)(int _iLogonID,int _iChannelNum,int _iStreamType, int _iStreamNO/*=MAIN_STREAM*/);
	extern	pfNetClient_SetStreamType	NetClient_SetStreamType /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetStreamType)(int _iLogonID,int _iChannelNum,int* _iStreamType, int _iStreamNO/*=MAIN_STREAM*/);
	extern	pfNetClient_GetStreamType	NetClient_GetStreamType /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetMotionAreaEnable)(int _iLogonID,int _iChannelNum);
	extern	pfNetClient_SetMotionAreaEnable	NetClient_SetSetMotionAreaEnable /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetMotionDetetionArea)(int _iLogonID,int _iChannelNum,int _ix,int _iy,int _iEnabled);
	extern	pfNetClient_SetMotionDetetionArea	NetClient_SetMotionDetetionArea /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetMotionDetetionArea)(int _iLogonID,int _iChannelNum,int _ix,int _iy,int* _iEnabled);
	extern	pfNetClient_GetMotionDetetionArea	NetClient_GetMotionDetetionArea /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetThreshold)(int _iLogonID,int _iChannelNum,int _iThreshold);
	extern	pfNetClient_SetThreshold	NetClient_SetThreshold /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetThreshold)(int _iLogonID,int _iChannelNum,int* _iThreshold);
	extern	pfNetClient_GetThreshold	NetClient_GetThreshold /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetMotionDetection)(int _iLogonID,int _iChannelNum,int _iEnabled);
	extern	pfNetClient_SetMotionDetection	NetClient_SetMotionDetection /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetMotionDetection)(int _iLogonID,int _iChannelNum,int* _iEnabled);
	extern	pfNetClient_GetMotionDetection	NetClient_GetMotionDetection /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetOsdText)(int _iLogonID,int _iChannelNum,unsigned char * _cTxtPtr,unsigned long _ulTextColor);
	extern	pfNetClient_SetOsdText	NetClient_SetOsdText /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetOsdText)(int _iLogonID,int _iChannelNum,unsigned char* _cOSDText,unsigned long* _ulTextColor);
	extern	pfNetClient_GetOsdText	NetClient_GetOsdText /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetOsdType)(int _iLogonID, int _iChannelNum,int _iPositionX,int _iPositionY,int _iOSDType,int _iEnabled);
	extern	pfNetClient_SetOsdType	NetClient_SetOsdType /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetOsdType)(int _iLogonID,int _iChannelNum,int _iOSDType,int* _iPositionX,int* _iPositionY,int *_iEnabled);
	extern	pfNetClient_GetOsdType	NetClient_GetOsdType /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetOsdLOGO)(int _iLogonID,int _iChannelNum,unsigned char * _cLogoFile,unsigned long _ulbkColor);
	extern	pfNetClient_SetOsdLOGO	NetClient_SetOsdLOGO /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetAudioChannel)(int _iLogonID,int _iChannelNum,int _iAudioChannel);
	extern	pfNetClient_SetAudioChannel	NetClient_SetAudioChannel /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetAudioChannel)(int _iLogonID,int _iChannelNum,int* _iAudioChannel);
	extern	pfNetClient_GetAudioChannel	NetClient_GetAudioChannel /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetIpFilter)(int _iLogonID,char* _cFilterIP,char* _cFilterMask);
	extern	pfNetClient_SetIpFilter	NetClient_SetIpFilter /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetIpFilter)(int _iLogonID,char* _cFilterIP,char* _cFilterMask);
	extern	pfNetClient_GetIpFilter	NetClient_GetIpFilter /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetAlarmOutput)(int _iLogonID, int _iInputPort,unsigned long _ulAlarmOutput);
	extern	pfNetClient_SetAlarmOutput	NetClient_SetAlarmOutput /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetAlarmOutput)(int _iLogonID,int _iInputPort,unsigned long* _ulAlarmOutput);
	extern	pfNetClient_GetAlarmOutput	NetClient_GetAlarmOutput /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetAlarmInMode)(int _iLogonID,int _iPortNum,int _iOnOrOff);
	extern	pfNetClient_SetAlarmInMode	NetClient_SetAlarmInMode /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetAlarmInMode)(int _iLogonID,int _iPortNum,int* _iOnOrOff);
	extern	pfNetClient_GetAlarmInMode	NetClient_GetAlarmInMode /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetAlarmOutMode)(int _iLogonID,int _iPortNum,int _iOnOrOff,int _iPulseWidth);
	extern	pfNetClient_SetAlarmOutMode	NetClient_SetAlarmOutMode /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetAlarmOutMode)(int _iLogonID,int _iPortNum,int* _iOnOrOff,int* _iPulseWidth);
	extern	pfNetClient_GetAlarmOutMode	NetClient_GetAlarmOutMode /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetAlmVdoCovThreshold)(int _iLogonID,int _iChannelNum,int _iThreshold);
	extern	pfNetClient_SetAlmVdoCovThreshold	NetClient_SetAlmVdoCovThreshold /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetAlmVdoCovThreshold)(int _iLogonID,int _iChannelNum,int* _iThreshold);
	extern	pfNetClient_GetAlmVdoCovThreshold	NetClient_GetAlmVdoCovThreshold /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetAlmVideoCov)(int _iLogonID,int _iChannelNum,int _iEnabled);
	extern	pfNetClient_SetAlmVideoCov	NetClient_SetAlmVideoCov /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetAlmVideoCov)(int _iLogonID,int _iChannelNum,int* _iEnabled);
	extern	pfNetClient_GetAlmVideoCov	NetClient_GetAlmVideoCov /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetDeviceType)( int _iLogonID,int _iChannelNum,DWORD_PTR _iComNo,int _iDevAddress,const char* _pcProtocol );
	extern	pfNetClient_SetDeviceType	NetClient_SetDeviceType /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetDeviceType)(int _iLogonID,int _iChannelNum,int* _iComPort,int* _iDevAddress,char* _cDeviceType);
	extern	pfNetClient_GetDeviceType	NetClient_GetDeviceType /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetAllSupportDeviceType)(int _iLogonID,int* _iSumDeviceType,char* _cDeviceType);
	extern	pfNetClient_GetAllSupportDeviceType	NetClient_GetAllSupportDeviceType /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetComFormat)( int _iLogonID,int _iComPort,char* _cDeviceType,char* _cComFormat,int _iWorkMode /*= 2/*í??÷í¨μà*/ );
	extern	pfNetClient_SetComFormat	NetClient_SetComFormat /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetComFormat)(int _iLogonID, int _iCom, char* _cCommFormat,int* _iWorkMode);
	extern	pfNetClient_GetComFormat	NetClient_GetComFormat /*= 0*/;
	typedef int (__stdcall *pfNetClient_DeviceCtrl)(int _iLogonID, int _iChannelNum,int _iActionType,int _iParam1,int _iParam2);
	extern	pfNetClient_DeviceCtrl	NetClient_DeviceCtrl /*= 0*/;
	typedef int (__stdcall *pfNetClient_DeviceCtrlEx)(int _iLogonID, int _iChannelNum,int _iActionType,int _iParam1,int _iParam2,int _iControlType);
	extern	pfNetClient_DeviceCtrlEx        NetClient_DeviceCtrlEx /*= 0*/;
	typedef int (__stdcall *pfNetClient_ComSend)(int _iLogonID,unsigned char* _ucBuf,int _iLength,int _iComNo);
	extern	pfNetClient_ComSend	NetClient_ComSend /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetComRecvNotify)(COMRECV_NOTIFY _comNotify);
	extern	pfNetClient_SetComRecvNotify	NetClient_SetComRecvNotify /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetAlarmVideoLost)(int _iLogonID, int _iChannelNum,int _iEnabled);
	extern	pfNetClient_SetAlarmVideoLost	NetClient_SetAlarmVideoLost /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetAlarmVideoLost)(int _iLogonID,int _iChannelNum,int* _iEnabled);
	extern	pfNetClient_GetAlarmVideoLost	NetClient_GetAlarmVideoLost /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetIFrameRate)(int _iLogonID,int _iChannelNum,int _iFrameRate, int _iStreamNO/*=MAIN_STREAM*/);
	extern	pfNetClient_SetIFrameRate	NetClient_SetIFrameRate /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetIFrameRate)(int _iLogonID,int _iChannelNum,int* _iFrameRate, int _iStreamNO/*=MAIN_STREAM*/);
	extern	pfNetClient_GetIFrameRate	NetClient_GetIFrameRate /*= 0*/;
	typedef int (__stdcall *pfNetClient_ForceIFrame)(int _iLogonID,int _iChannelNum, int _iStreamNO/*=MAIN_STREAM*/);
	extern	pfNetClient_ForceIFrame	NetClient_ForceIFrame /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetTime)(int _iLogonID,int _iyy,int _imo,int _idd,int _ihh,int _imi,int _iss);
	extern	pfNetClient_SetTime	NetClient_SetTime /*= 0*/;
	typedef int (__stdcall *pfNetClient_Reboot)(int _iLogonID);
	extern	pfNetClient_Reboot	NetClient_Reboot /*= 0*/;
	typedef int (__stdcall *pfNetClient_DefaultPara)(int _iLogonID);
	extern	pfNetClient_DefaultPara	NetClient_DefaultPara /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetServerVersion)(int _iLogonID, char *_cVer);
	extern	pfNetClient_GetServerVersion	NetClient_GetServerVersion /*= 0*/;
	typedef int (__stdcall *pfNetClient_UpgradeProgram)(int _iLogonID,char* _cFileName,PROUPGRADE_NOTIFY _UpgradeNotify);
	extern	pfNetClient_UpgradeProgram	NetClient_UpgradeProgram /*= 0*/;
	typedef int (__stdcall *pfNetClient_UpgradeWebPage)(int _iLogonID,char* _cFileName,WEBUPGRADE_NOTIFY _UpgradeWebNotify);
	extern	pfNetClient_UpgradeWebPage	NetClient_UpgradeWebPage /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetUpgradePercent)(int _iLogonID);
	extern	pfNetClient_GetUpgradePercent	NetClient_GetUpgradePercent /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetUserNum)(int _iLogonID,int* _iUserNum);
	extern	pfNetClient_GetUserNum	NetClient_GetUserNum /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetUserInfo)(int _iLogonID,int _iUserSerial,char* _cUserName,char* _cPassword,int* _iAuthority);
	extern	pfNetClient_GetUserInfo	NetClient_GetUserInfo /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetCurUserInfo)(int _iLogonID,char* _cUserName,char* _cPassword,int* _iAuthority);
	extern	pfNetClient_GetCurUserInfo	NetClient_GetCurUserInfo /*= 0*/;
	typedef int (__stdcall *pfNetClient_AddUser)(int _iLogonID,char* _cUserName,char* _cPassword,int _iAuthority);
	extern	pfNetClient_AddUser	NetClient_AddUser /*= 0*/;
	typedef int (__stdcall *pfNetClient_DelUser)(int _iLogonID,char* _cUserName);
	extern	pfNetClient_DelUser	NetClient_DelUser /*= 0*/;
	typedef int (__stdcall *pfNetClient_ModifyPwd)(int _iLogonID,char* _cUserName,char* _cNewPwd);
	extern	pfNetClient_ModifyPwd	NetClient_ModifyPwd /*= 0*/;
	typedef int (__stdcall *pfNetClient_ChangeSvrIP)(int _iLogonID, char* _cNewSvrIP, char* _cMask, char* _cGateway, char* _cDNSS);
	extern	pfNetClient_ChangeSvrIP	NetClient_ChangeSvrIP /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetMaxConUser)(int _iLogonID,int _iMaxConUser);
	extern	pfNetClient_SetMaxConUser	NetClient_SetMaxConUser /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetMaxGetUser)(int _iLogonID,int* _iMaxConUser);
	extern	pfNetClient_GetMaxGetUser	NetClient_GetMaxGetUser /*= 0*/;
	typedef int (__stdcall *pfNetClient_TalkStart)(int _iLogonID, int _bUser /*= false*/);
	extern	pfNetClient_TalkStart	NetClient_TalkStart /*= 0*/;
	typedef int (__stdcall *pfNetClient_TalkEnd)(int _iLogonID);
	extern	pfNetClient_TalkEnd	NetClient_TalkEnd /*= 0*/;
	typedef int (__stdcall *pfNetClient_CapturePicture)(unsigned int _ulConID, int _iPicType, char* _pcFileName);
	extern pfNetClient_CapturePicture NetClient_CapturePicture;
	typedef int (__stdcall *pfNetClient_CapturePic)(unsigned long _ulConID,unsigned char **ucData);
	extern	pfNetClient_CapturePic	NetClient_CapturePic /*= 0*/;
	typedef int (__stdcall *pfNetClient_CaptureBmpPic)(unsigned long _ulConID, char* _cFileName);
	extern	pfNetClient_CaptureBmpPic	NetClient_CaptureBmpPic /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetOutPortState)(int _iLogonID, int _iPort, int _iState);
	extern	pfNetClient_SetOutPortState	NetClient_SetOutPortState /*= 0*/;


	typedef int (__stdcall *pfNetClient_GetIpProperty)(int _iLogonID,char* _cMAC, char* _cMask, char* _cGateway, char* _cDNS);
	extern	pfNetClient_GetIpProperty	NetClient_GetIpProperty /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetVideoCovArea)(int _iLogonID,int _iChannelNum,RECT* _rect, int _iStreamNO/*=MAIN_STREAM*/);
	extern	pfNetClient_GetVideoCovArea	NetClient_GetVideoCovArea /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetVideoCovArea)(int _iLogonID,int _iChannelNum,RECT* _rect, int _iStreamNO/*=MAIN_STREAM*/);
	extern	pfNetClient_SetVideoCovArea	NetClient_SetVideoCovArea /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetVideoSizeEx)(int _iLogonID,int _iChannelNum,int* _ivideoSize, int _iStreamNO/*=SUB_STREAM*/);
	extern	pfNetClient_GetVideoSizeEx	NetClient_GetVideoSizeEx /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetVideoSize)(int _iLogonID,int _iChannelNum,int _iVideoSize, int _iStreamNO/*=MAIN_STREAM*/);
	extern	pfNetClient_SetVideoSize	NetClient_SetVideoSize /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetVideoSize)(int _iLogonID,int _iChannelNum,int* _width, int * _height, int _iStreamNO/*=MAIN_STREAM*/);
	extern	pfNetClient_GetVideoSize	NetClient_GetVideoSize /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetMaxKByteRate)(int _iLogonID,int _iChannelNum,int _ibitRate, int _iStreamNO/*=MAIN_STREAM*/);
	extern	pfNetClient_SetMaxKByteRate	NetClient_SetMaxKByteRate /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetMaxKByteRate)(int _iLogonID,int _iChannelNum,int* _ibitRate, int _iStreamNO/*=MAIN_STREAM*/);
	extern	pfNetClient_GetMaxKByteRate	NetClient_GetMaxKByteRate /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetAlarmServer)(int _iLogonID,const char* _pcAlarmServer,int _iServerPort);
	extern pfNetClient_SetAlarmServer NetClient_SetAlarmServer /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetAlarmServer)(int _iLogonID,char* _pcAlarmServer,int* _iServerPort);
	extern pfNetClient_GetAlarmServer NetClient_GetAlarmServer /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetDHCPParam)(int _iLogonID,int _iDHCP);
	extern pfNetClient_SetDHCPParam NetClient_SetDHCPParam /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetDHCPParam)(int _iLogonID,int* _iDHCP);
	extern pfNetClient_GetDHCPParam NetClient_GetDHCPParam /*= 0*/;
	typedef int (__stdcall *pfNetClient_DeviceSetup)(int _iLogonID, int _iFlag,const char *_pcSection,const char* _pcKeyword,const char* _pcValue);
	extern pfNetClient_DeviceSetup NetClient_DeviceSetup /*= 0*/;

	typedef int (__stdcall *pfNetClient_WriteUserData)(int _iLogonID,int _iOffset,unsigned char* _u8Buf,int _iLength);
	extern	pfNetClient_WriteUserData	NetClient_WriteUserData /*= 0*/;
	typedef int (__stdcall *pfNetClient_ReadUserData)(int _iLogonID,int _iOffset,unsigned char* _u8Buf,int _iLength);
	extern	pfNetClient_ReadUserData	NetClient_ReadUserData /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetDomainParsePara)(int _iLogonID,int _iinterval,char* _cUserID,char* _cPassword,char* _cHost, char* _cDomain1, char* _cDomain2, WORD _iPort1, WORD _iPort2);
	extern	pfNetClient_SetDomainParsePara	NetClient_SetDomainParsePara /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetDomainParsePara)(int _iLogonID,int *_iinterval,char* _cUserID,char* _cPassword,char* _cHost, char* _cDomain1, char* _cDomain2, WORD* _iPort1, WORD* _iPort2);
	extern	pfNetClient_GetDomainParsePara	NetClient_GetDomainParsePara /*= 0*/;
	typedef int (__stdcall *pfNetClient_SendStringToCenter)(char* _cIpAddress,int _iServerPort, char* _cMsg, int _iLen);
	extern	pfNetClient_SendStringToCenter	NetClient_SendStringToCenter /*= 0*/;
	typedef int (__stdcall *pfNetClient_DrawRectOnLocalVideo)(unsigned long _ulConID, RECT* _rcRect, int _iCount);
	extern	pfNetClient_DrawRectOnLocalVideo	NetClient_DrawRectOnLocalVideo /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetBothStreamSame)(int _iLogonID, int _iChannelNum, int* _iState);
	extern	pfNetClient_GetBothStreamSame	NetClient_GetBothStreamSame /*= 0*/;
	typedef int (__stdcall *pfNetClient_DrawTextOnVideo)(int _iLogonID, int _iChannelNum, int _iX, int _iY, char* _cText, int _iStore, int _iStreamNO/*=MAIN_STREAM*/);
	extern	pfNetClient_DrawTextOnVideo	NetClient_DrawTextOnVideo /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetTextOnVideo)(int _iLogonID, int _iChannelNum, int* _iX, int* _iY, char* _cText,int _iStreamNO);
	extern	pfNetClient_GetTextOnVideo	NetClient_GetTextOnVideo /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetAlarmPortEnable)(int _iLogonID,int _iInPort,int _iEnabled);
	extern	pfNetClient_SetAlarmPortEnable	NetClient_SetAlarmPortEnable /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetAlarmPortEnable)(int _iLogonID,int _iInPort,int *_iEnabled);
	extern	pfNetClient_GetAlarmPortEnable	NetClient_GetAlarmPortEnable /*= 0*/;
	typedef int (__stdcall *pfNetClient_ShowBitrateOnVideo)(unsigned long _ulConID, int _iX, int _iY, int _iEnabled);
	extern pfNetClient_ShowBitrateOnVideo NetClient_ShowBitrateOnVideo /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetPPPoEInfo)(int _iLogonID,char* _cAccount, char* _cPassword, int _iEnabled);
	extern pfNetClient_SetPPPoEInfo NetClient_SetPPPoEInfo /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetPPPoEInfo)(int _iLogonID,char* _cAccount, char* _cPassword, int *_iEnabled);
	extern pfNetClient_GetPPPoEInfo NetClient_GetPPPoEInfo /*= 0*/;
	typedef int (__stdcall *pfNetClient_CPUCheckEnabled)(int _iEnabled, int _interval);
	extern pfNetClient_CPUCheckEnabled NetClient_CPUCheckEnabled /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetBitrateOnVideo)(unsigned long _ulConID, int *_iX, int *_iY, int *_iEnabled);
	extern pfNetClient_GetBitrateOnVideo NetClient_GetBitrateOnVideo /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetMaxVideoSize)(int _iLogonID, int* _iVideoSize);
	extern pfNetClient_GetMaxVideoSize NetClient_GetMaxVideoSize /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetEncodeMode)(int _iLogonID, int _iChannelNum, int _iStreamNO, int _iMode);
	extern pfNetClient_SetEncodeMode NetClient_SetEncodeMode /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetEncodeMode)(int _iLogonID, int _iChannelNum, int _iStreamNO, int* _iMode);
	extern pfNetClient_GetEncodeMode NetClient_GetEncodeMode /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetPreferMode)(int _iLogonID, int _iChannelNum, int _iStreamNO, int _iMode);
	extern pfNetClient_SetPreferMode NetClient_SetPreferMode /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetPreferMode)(int _iLogonID, int _iChannelNum, int _iStreamNO, int* _iMode);
	extern pfNetClient_GetPreferMode NetClient_GetPreferMode /*= 0*/;
	typedef int (__stdcall *pfNetClient_NetFileQuery)(int _iLogonID, PNVS_FILE_QUERY _fileQuery);
	extern pfNetClient_NetFileQuery NetClient_NetFileQuery /*= 0*/;
	typedef int (__stdcall *pfNetClient_NetFileGetFileCount)(int _iLogonID, int *_iTotalCount, int *_iCurrentCount);
	extern pfNetClient_NetFileGetFileCount NetClient_NetFileGetFileCount /*= 0*/;
	typedef int (__stdcall *pfNetClient_NetFileGetQueryfile)(int _iLogonID, int _iFileIndex, PNVS_FILE_DATA _fileInfo);
	extern pfNetClient_NetFileGetQueryfile NetClient_NetFileGetQueryfile /*= 0*/;
	typedef int (__stdcall *pfNetClient_NetFileManualRecord)(int _iLogonID, int _iChannel, int _iState);
	extern pfNetClient_NetFileManualRecord NetClient_NetFileManualRecord /*= 0*/;
	typedef int (__stdcall *pfNetClient_NetFileMapStoreDevice)(int _iLogonID,PNVS_NFS_DEV _storeDev);
	extern pfNetClient_NetFileMapStoreDevice NetClient_NetFileMapStoreDevice;
	typedef int (__stdcall *pfNetClient_NetFileGetMapStoreDevice)(int _iLogonID,PNVS_NFS_DEV _storeDev);
	extern pfNetClient_NetFileGetMapStoreDevice NetClient_NetFileGetMapStoreDevice /*= 0*/;
	typedef int (__stdcall *pfNetClient_NetFileMountUSB)(int _iLogonID,PNVS_NFS_DEV _storeDev);
	extern pfNetClient_NetFileMountUSB NetClient_NetFileMountUSB /*= 0*/;
	typedef int (__stdcall *pfNetClient_NetFileGetUSBstate)(int _iLogonID, int *_iState);
	extern pfNetClient_NetFileGetUSBstate NetClient_NetFileGetUSBstate /*= 0*/;
	typedef int (__stdcall *pfNetClient_NetFileSetExtendname)(int _iLogonID, char *_cExtend);
	extern pfNetClient_NetFileSetExtendname NetClient_NetFileSetExtendname /*= 0*/;
	typedef int (__stdcall *pfNetClient_NetFileGetExtendname)(int _iLogonID, char *_cExtend);
	extern pfNetClient_NetFileGetExtendname NetClient_NetFileGetExtendname /*= 0*/;
	typedef int (__stdcall *pfNetClient_NetFileSetRecordRule)(int _iLogonID, int _iRule, int _iTimelen, int _iFreedisk,int _iFileSize /*= 0*/);
	extern pfNetClient_NetFileSetRecordRule NetClient_NetFileSetRecordRule /*= 0*/;
	typedef int (__stdcall *pfNetClient_NetFileGetRecordRule)(int _iLogonID, int* _iRule, int *_iTimelen, int *_iFreedisk,int *_iFileSize);
	extern pfNetClient_NetFileGetRecordRule NetClient_NetFileGetRecordRule /*= 0*/;
	typedef int (__stdcall *pfNetClient_NetFileSetAlarmRule)(int _iLogonID, int _iPreRecordEnable, int _iPreRecordTime, int _iDelayTime,int _iDelayEnable,int _iChannelNum /*= 0*/);
	extern pfNetClient_NetFileSetAlarmRule NetClient_NetFileSetAlarmRule /*= 0*/;
	typedef int (__stdcall *pfNetClient_NetFileGetAlarmRule)(int _iLogonID, int *_iPreEnable, int *_iPreTime, int *_iDelayTime,int *_iDealyEnable,int iChannelNum /*= 0*/);
	extern pfNetClient_NetFileGetAlarmRule NetClient_NetFileGetAlarmRule /*= 0*/;
	typedef int (__stdcall *pfNetClient_NetFileSetAlarmState)(int _iLogonID, int _iChannel, int _iState);
	extern pfNetClient_NetFileSetAlarmState NetClient_NetFileSetAlarmState /*= 0*/;
	typedef int (__stdcall *pfNetClient_NetFileGetAlarmState)(int _iLogonID, int _iChannel, int *_iStat);
	extern pfNetClient_NetFileGetAlarmState NetClient_NetFileGetAlarmState /*= 0*/;
	typedef int (__stdcall *pfNetClient_NetFileSetTaskState)(int _iLogonID, int _iChannel, int _iState);
	extern pfNetClient_NetFileSetTaskState NetClient_NetFileSetTaskState /*= 0*/;
	typedef int (__stdcall *pfNetClient_NetFileGetTaskState)(int _iLogonID, int _iChannel, int *_iState);
	extern pfNetClient_NetFileGetTaskState NetClient_NetFileGetTaskState /*= 0*/;
	typedef int (__stdcall *pfNetClient_NetFileSetTaskSchedule)(int _iLogonID, int _iChannel, int _iWeekday, NVS_SCHEDTIME *_Schedule[MAX_TIMESEGMENT]);
	extern pfNetClient_NetFileSetTaskSchedule NetClient_NetFileSetTaskSchedule /*= 0*/;
	typedef int (__stdcall *pfNetClient_NetFileGetTaskSchedule)(int _iLogonID, int _iChannel, int _iWeekday, NVS_SCHEDTIME *_Schedule[MAX_TIMESEGMENT]);
	extern pfNetClient_NetFileGetTaskSchedule NetClient_NetFileGetTaskSchedule /*= 0*/;
	typedef int (__stdcall *pfNetClient_NetFileRebuildIndexFile)(int _iLogonID, int _iState);
	extern pfNetClient_NetFileRebuildIndexFile NetClient_NetFileRebuildIndexFile /*= 0*/;
	typedef int (__stdcall *pfNetClient_NetFileGetDiskInfo)(int _iLogonID, PNVS_STORAGEDEV _storag);
	extern pfNetClient_NetFileGetDiskInfo NetClient_NetFileGetDiskInfo /*= 0*/;
	typedef int (__stdcall *pfNetClient_NetFileIsSupportStorage)(int _iLogonID);
	extern pfNetClient_NetFileIsSupportStorage NetClient_NetFileIsSupportStorage /*= 0*/;
	typedef int (__stdcall *pfNetClient_NetFileDownloadFile)(unsigned long *_ulConID,int _iLogonID, char* _cRemoteFilename, char* _cLocalFilename,int _iFlag /*= 0*/,int _iPosition /*= -1*/,int _Speed /*= 1*/);
	extern pfNetClient_NetFileDownloadFile NetClient_NetFileDownloadFile /*= 0*/;

	typedef int (__stdcall *pfNetClient_NetFileGetRecordState)(int _iLogonID, int _iChannel, int *_iState);
	extern pfNetClient_NetFileGetRecordState NetClient_NetFileGetRecordState /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetAudioEncoder)(int _iLogonID, int _iChannel, AUDIO_ENCODER _aCoder);
	extern pfNetClient_SetAudioEncoder NetClient_SetAudioEncoder /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetAudioEncoder)(int _iLogonID, int _iChannel, AUDIO_ENCODER *_aCoder);
	extern pfNetClient_GetAudioEncoder NetClient_GetAudioEncoder /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetVideoNPMode)(int _iLogonID, VIDEO_NORM _vMode);
	extern pfNetClient_SetVideoNPMode NetClient_SetVideoNPMode;//=0;
	typedef int (__stdcall *pfNetClient_GetVideoNPMode)(int _iLogonID, VIDEO_NORM *_vMode);
	extern pfNetClient_GetVideoNPMode NetClient_GetVideoNPMode;//=0;
	typedef int (__stdcall *pfNetClient_SetDeviceTimerReboot)(int _iLogonID, int _iEnable, int _iinterval, int _iHour);
	extern pfNetClient_SetDeviceTimerReboot NetClient_SetDeviceTimerReboot;//=0;
	typedef int (__stdcall *pfNetClient_GetDeviceTimerReboot)(int _iLogonID, int *_iEnable, int *_iinterval, int *_iHour);
	extern pfNetClient_GetDeviceTimerReboot NetClient_GetDeviceTimerReboot;//=0;
	typedef int (__stdcall *pfNetClient_SetManagerServersInfo)(int _iLogonID, char* _cRegSvrIP, WORD _wdRegPort,char* _cHeartSvrIP, WORD _wdHeartPort, char* _cAlarmSvrIP, WORD _wdAlarmPort);
	extern pfNetClient_SetManagerServersInfo NetClient_SetManagerServersInfo;//=0;
	typedef int (__stdcall *pfNetClient_GetManagerServersInfo)(int _iLogonID, char* _cRegSvrIP, WORD *_wdRegPort,char* _cHeartSvrIP, WORD *_wdHeartPort, char* _cAlarmSvrIP, WORD *_wdAlarmPort);
	extern pfNetClient_GetManagerServersInfo NetClient_GetManagerServersInfo /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetDeviceID)(int _iLogonID, char* _cDeviceID, char* _cDeviceName, unsigned short _iVspPort, char* _cAccessPass, unsigned short _iVapPort);
	extern pfNetClient_SetDeviceID NetClient_SetDeviceID /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetDeviceID)(int _iLogonID, char* _cDeviceID, char* _cDeviceName, unsigned short *_iVspPort, char* _cAccessPass, unsigned short *_iVapPort);
	extern pfNetClient_GetDeviceID NetClient_GetDeviceID /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetMediaStreamClient)(int _iLogonID, int _iChannel, char* _cClientIP, unsigned short _iClientPort, int _iStreamType);
	extern pfNetClient_SetMediaStreamClient NetClient_SetMediaStreamClient /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetMediaStreamClient)(int _iLogonID, int _iChannel, char* _cClientIP, unsigned short *_iClientPort, int *_iStreamType);
	extern pfNetClient_GetMediaStreamClient NetClient_GetMediaStreamClient /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetAlarmIPortState)(int _iLogonID, int _iInPort,int *_iState);
	extern pfNetClient_GetAlarmIPortState NetClient_GetAlarmIPortState /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetAlarmVLostState)(int _iLogonID, int _iChannel,int *_iState);
	extern pfNetClient_GetAlarmVLostState NetClient_GetAlarmVLostState /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetAlarmVCoverState)(int _iLogonID, int _iChannel,int *_iState);
	extern pfNetClient_GetAlarmVCoverState NetClient_GetAlarmVCoverState /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetDateFormat)(int _iLogonID, int _iFormat, char _cSeparate);
	extern pfNetClient_SetDateFormat NetClient_SetDateFormat /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetDateFormat)(int _iLogonID, int *_iFormat, char *_cSeparate);
	extern pfNetClient_GetDateFormat NetClient_GetDateFormat /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetRawFrameCallBack)(unsigned long _ulConID, RAWFRAME_NOTIFY _cbkGetFrame, void* _pContext /*= NULL*/);
	extern pfNetClient_SetRawFrameCallBack NetClient_SetRawFrameCallBack /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetRawFrameCallBackEx)(unsigned long _ulConID, RAWFRAME_NOTIFY_EX _cbkGetFrame, void* _pContext /*= NULL*/);
	extern pfNetClient_SetRawFrameCallBackEx NetClient_SetRawFrameCallBackEx /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetHTTPPort)(int _iLogonID, WORD _wPort);
	extern pfNetClient_SetHTTPPort NetClient_SetHTTPPort /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetHTTPPort)(int _iLogonID, WORD *_wPort);
	extern pfNetClient_GetHTTPPort NetClient_GetHTTPPort /*= 0*/;
	typedef int (__stdcall *pfNetClient_IYUVtoYV12)(int _iWidth, int _iHeight, BYTE* _YUV420);
	extern pfNetClient_IYUVtoYV12 NetClient_IYUVtoYV12 /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetEmailAlarm)(int _iLogonID, PSMTP_INFO _pSmtp);
	extern pfNetClient_SetEmailAlarm NetClient_SetEmailAlarm /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetEmailAlarm)(int _iLogonID, PSMTP_INFO _pSmtp);
	extern pfNetClient_GetEmailAlarm NetClient_GetEmailAlarm /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetEmailAlarmEnable)(int _iLogonID, int _iChannel, int _iEnable);
	extern pfNetClient_SetEmailAlarmEnable NetClient_SetEmailAlarmEnable /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetEmailAlarmEnable)(int _iLogonID, int _iChannel, int *_iEnabl);
	extern pfNetClient_GetEmailAlarmEnable NetClient_GetEmailAlarmEnable /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetScene)(int _iLogonID, int _iChannel, int _iScene);
	extern pfNetClient_SetScene NetClient_SetScene /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetScene)(int _iLogonID, int _iChannel, int *_iScene);
	extern pfNetClient_GetScene NetClient_GetScene /*= 0*/;
	typedef int (__stdcall *pfNetClient_Snapshot)(int _iLogonID, int _iChannel, int _iQvalue);
	extern pfNetClient_Snapshot NetClient_Snapshot /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetFactoryID)(int _iLogonID, char* _cFactoryID);
	extern pfNetClient_GetFactoryID NetClient_GetFactoryID /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetSensorFlip)(int _iLogonID, int _iChannel, int _iFli);
	extern pfNetClient_SetSensorFlip NetClient_SetSensorFlip /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetSensorFlip)(int _iLogonID, int _iChannel, int *_iFlip);
	extern pfNetClient_GetSensorFlip NetClient_GetSensorFlip /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetSensorMirror)(int _iLogonID, int _iChannel, int _iMirror);
	extern pfNetClient_SetSensorMirror NetClient_SetSensorMirror /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetSensorMirror)(int _iLogonID, int _iChannel, int *_iMirror);
	extern pfNetClient_GetSensorMirror NetClient_GetSensorMirror /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetWifiParam)(int _iLogonID, NVS_WIFI_PARAM *_pWifiParam);
	extern pfNetClient_SetWifiParam NetClient_SetWifiParam /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetWifiParam)(int _iLogonID, NVS_WIFI_PARAM *_pWifiParam);
	extern pfNetClient_GetWifiParam NetClient_GetWifiParam /*= 0*/;
	typedef int (__stdcall *pfNetClient_WifiSearch)(int _iLogonID);
	extern pfNetClient_WifiSearch NetClient_WifiSearch /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetWifiSearchResult)(int _iLogonID, WIFI_INFO **_pWifiInfo, int *_iWifiCount);
	extern pfNetClient_GetWifiSearchResult NetClient_GetWifiSearchResult /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetPrivacyProtect)(int _iLogonID,int _iChannelNum,int _iEnabled);
	extern pfNetClient_SetPrivacyProtect NetClient_SetPrivacyProtect /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetPrivacyProtect)(int _iLogonID,int _iChannelNum,int* _iEnabled);
	extern pfNetClient_GetPrivacyProtect NetClient_GetPrivacyProtect /*= 0*/;
	typedef int (__stdcall *pfNetClient_InputTalkingdata)(unsigned char* _ucData, unsigned int _iLend);
	extern pfNetClient_InputTalkingdata NetClient_InputTalkingdata /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetNTPInfo)(int _iLogonID, char* _NTPServer, unsigned short _iPort,int _iinterval);
	extern pfNetClient_SetNTPInfo NetClient_SetNTPInfo /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetNTPInfo)(int _iLogonID, char* _NTPServer, unsigned short *_iPort,int *_iinterval);
	extern pfNetClient_GetNTPInfo NetClient_GetNTPInfo /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetCPUMEMAlarmThreshold)(int _iLogonID, int _iCPUThreshold, int _iMEMThreshold);
	extern pfNetClient_SetCPUMEMAlarmThreshold NetClient_SetCPUMEMAlarmThreshold /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetCPUMEMAlarmThreshold)(int _iLogonID, int *_iCPUThreshold, int *_iMEMThreshold);
	extern pfNetClient_GetCPUMEMAlarmThreshold NetClient_GetCPUMEMAlarmThreshol /*= 0*/;
	typedef int (__stdcall *pfNetClient_SetOSDAlpha)(int _iLogonID, int _iChannel,int _iAlpha);
	extern pfNetClient_SetOSDAlpha NetClient_SetOSDAlpha /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetOSDAlpha)(int _iLogonID, int _iChannel,int *_iAlpha);
	extern pfNetClient_GetOSDAlpha NetClient_GetOSDAlpha /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetDevType)(int _iLogonID,int* _iDevType);
	extern pfNetClient_GetDevType NetClient_GetDevType /*= 0*/;

	typedef int (__stdcall *pfNetClient_SetOsdDiaphaneity)(int _iLogonID,int _iChannelNum,int _iDiaphaneity);
	extern pfNetClient_SetOsdDiaphaneity NetClient_SetOsdDiaphaneity /*= 0*/;
	typedef int (__stdcall *pfNetClient_GetOsdDiaphaneity)(int _iLogonID,int _iChannelNum,int *_iDiaphaneity);
	extern pfNetClient_GetOsdDiaphaneity NetClient_GetOsdDiaphaneity /*= 0*/;
	typedef int (__stdcall *pfNetClient_Set3DEnable)(int _iLogonID,int _iChannelNum,int _iEnable,int _iParam);
	extern pfNetClient_Set3DEnable NetClient_Set3DEnable /*= 0*/;
	typedef int (__stdcall *pfNetClient_Get3DEnable)(int _iLogonID,int _iChannelNum,int* _iEnable,int* _iParam);
	extern pfNetClient_Get3DEnable NetClient_Get3DEnable /*= 0*/;
	typedef int (__stdcall *pfNetClient_NetFileDelFile)(int _iLogonID,const char *_pcFileName);
	extern pfNetClient_NetFileDelFile NetClient_NetFileDelFile /*= 0*/;
	typedef int (__stdcall *pfNetClient_DiskSetUsage)(int _iLogonID,int _iDiskNo,int _iUsage);
	extern pfNetClient_DiskSetUsage NetClient_DiskSetUsage /*= 0*/;
	typedef int (__stdcall *pfNetClient_DiskFormat)(int _iLogonID,int _iDiskNo,int _iFsType);
	extern pfNetClient_DiskFormat NetClient_DiskFormat /*= 0*/;
	typedef int (__stdcall *pfNetClient_DiskPart)(int _iLogonID,int _iDiskNo,int _iPartNum,int _iFormatNow);
	extern pfNetClient_DiskPart NetClient_DiskPart /*= 0*/;
	typedef int (__stdcall *pfNetClient_BackupKernel)(int _iLogonID);
	extern pfNetClient_BackupKernel NetClient_BackupKernel /*= 0*/;

	typedef int (__stdcall *pfNetClient_ProxyAddProxy)(char* _cIP, char* _proxy, int _iNetMode,char* _pcProID ,unsigned short _iPort /*= 0*/);
	extern	pfNetClient_ProxyAddProxy	NetClient_ProxyAddProxy;
	typedef int (__stdcall *pfNetClient_ProxyStartup)(IDENTIFY _identify, unsigned short _iProxyPort /*= 0*/);
	extern	pfNetClient_ProxyStartup	NetClient_ProxyStartup;
	typedef int (__stdcall *pfNetClient_ProxyHasConnected)(int _iProxyID,int _iChannel, int _iStreamNO/*=MAIN_STREAM*/);
	extern	pfNetClient_ProxyHasConnected	NetClient_ProxyHasConnected;
	typedef int (__stdcall *pfNetClient_ProxyGetAllLinks)(int _iProxyID, int _iChannel, PUIDLIST _pIDList, int _iStreamNO/*=MAIN_STREAM*/);
	extern	pfNetClient_ProxyGetAllLinks	NetClient_ProxyGetAllLinks;
	typedef int (__stdcall *pfNetClient_ProxyGetLinkInfo)(int _iConnectID, PLINKINFO _pLinkInfo);
	extern	pfNetClient_ProxyGetLinkInfo	NetClient_ProxyGetLinkInfo;
	typedef int (__stdcall *pfNetClient_ProxyDeleteLink)(int _iConnectID);
	extern	pfNetClient_ProxyDeleteLink	NetClient_ProxyDeleteLink;
	typedef int (__stdcall *pfNetClient_ProxyDeleteProxy)(int _iProxyID);
	extern  pfNetClient_ProxyDeleteProxy NetClient_ProxyDeleteProxy;
	typedef int (__stdcall *pfNetClient_ProxySendStringToClient)(char* _cIpAddress, char* _cMsg, int _iLen);
	extern	pfNetClient_ProxySendStringToClient	NetClient_ProxySendStringToClient;
	typedef int (__stdcall *pfNetClient_ProxySendStringToClientByID)(int _iConnectID, char* _cMsg, int _iLen);
	extern  pfNetClient_ProxySendStringToClientByID NetClient_ProxySendStringToClientByID;
	typedef int (__stdcall *pfNetClient_ProxySetMaxInOutVideo)(int _iMaxIn, int _iMaxOut);
	extern  pfNetClient_ProxySetMaxInOutVideo NetClient_ProxySetMaxInOutVideo;
	typedef int (__stdcall *pfNetClient_ProxyCloseAllLinks)(int _iProxyID);
	extern  pfNetClient_ProxyCloseAllLinks NetClient_ProxyCloseAllLinks;

	typedef int (__stdcall *pfNetClient_GetChannelNum)(int _iLogonID, int* _iChannelNum);
	extern	pfNetClient_GetChannelNum	NetClient_GetChannelNum;
	typedef int (__stdcall *pfNetClient_StopRecv)(unsigned long _ulConID);
	extern pfNetClient_StopRecv	NetClient_StopRecv;
	typedef int (__stdcall *pfNetClient_GetRecvID)(int _iLogonID, int _iChannel, int _iStreamNO/*=MAIN_STREAM*/);
	extern	pfNetClient_GetRecvID	NetClient_GetRecvID;
	typedef int (__stdcall *pfNetClient_Logoff)(int _iLogonID);
	extern	pfNetClient_Logoff	NetClient_Logoff;
	//-------------------------------------------------------------------------
	typedef int (__stdcall *pfNetClient_DCardInit)();
	extern pfNetClient_DCardInit NetClient_DCardInit;
	typedef  int (__stdcall *pfNetClient_DCardSetShowMode)(int _iCardChannel, int _iShowMode);
	extern pfNetClient_DCardSetShowMode NetClient_DCardSetShowMode;
	typedef int (__stdcall *pfNetClient_DCardGetShowMode)(int _iCardChannel, int *_iShowMode);
	extern  pfNetClient_DCardGetShowMode NetClient_DCardGetShowMode;
	typedef int (__stdcall *pfNetClient_DCardStartPlay)(unsigned long _ulConID, int _iCardChannel, int _iPos);
	extern  pfNetClient_DCardStartPlay  NetClient_DCardStartPlay;
	typedef int (__stdcall *pfNetClient_DCardStopPlay)(unsigned long _ulConID, int _iFlag /*= 1*/);
	extern  pfNetClient_DCardStopPlay NetClient_DCardStopPlay;
	typedef int (__stdcall *pfNetClient_DCardRelease)();
	extern pfNetClient_DCardRelease NetClient_DCardRelease;
	typedef int (__stdcall *pfNetClient_DCardReInit)(int _iCardChannel);
	extern pfNetClient_DCardReInit NetClient_DCardReInit;
	typedef int (__stdcall *pfNetClient_DCardGetState)(int _iCardChannel, int *_iState);
	extern pfNetClient_DCardGetState NetClient_DCardGetState;
	typedef int (__stdcall *pfNetClient_DCardStartPlayEx)(int* _iId, DecoderParam* _dParam);
	extern  pfNetClient_DCardStartPlayEx NetClient_pfNetClient_DCardStartPlayEx;
	typedef int (__stdcall *pfNetClient_DCardPutDataEx)(int _iId, char* _cBuf, int _iLen);
	extern  pfNetClient_DCardPutDataEx NetClient_pfNetClient_DCardPutDataEx;
	typedef int (__stdcall *pfNetClient_DCardStopPlayEx)(int _iId, int _iFlag /*= 1*/);
	extern  pfNetClient_DCardStopPlayEx NetClient_pfNetClient_DCardStopPlayEx;
	typedef int (__stdcall *pfNetClient_DCardStartPlayAudio)(unsigned long _ulConID);
	extern  pfNetClient_DCardStartPlayAudio NetClient_DCardStartPlayAudio;
	typedef int (__stdcall *pfNetClient_DCardStopPlayAudio)(unsigned long _ulConID);
	extern  pfNetClient_DCardStopPlayAudio NetClient_DCardStopPlayAudio;
	//-------------------------------------------------------------------------

	typedef int (__stdcall *pfNetClient_LogFileSetProperty)(int _iLogonID, int _iLevel, int _iSize);
	extern  pfNetClient_LogFileSetProperty NetClient_LogFileSetProperty;
	typedef int (__stdcall *pfNetClient_LogFileGetProperty)(int _iLogonID, int* _iLevel, int* _iSize);
	extern  pfNetClient_LogFileGetProperty NetClient_LogFileGetProperty;
	typedef int (__stdcall *pfNetClient_LogFileDownload)(int _iLogonID);
	extern  pfNetClient_LogFileDownload NetClient_LogFileDownload;
	typedef int (__stdcall *pfNetClient_LogFileClear)(int _iLogonID);
	extern  pfNetClient_LogFileClear NetClient_LogFileClear;
	typedef int (__stdcall *pfNetClient_LogFileGetDetails)(int _iLogonID, char* _cBuf, int* _iLen);
	extern  pfNetClient_LogFileGetDetails NetClient_LogFileGetDetails;
	//-------------------------------------------------------------------------
	typedef int (__stdcall *pfNetClient_NetFileStopDownloadFile)(unsigned long _ulConID);
	extern  pfNetClient_NetFileStopDownloadFile NetClient_NetFileStopDownloadFile;
	typedef int (__stdcall *pfNetClient_NetFileGetDownloadPos)(unsigned long _ulConID, int *_iPos, int *_iDLSize);
	extern  pfNetClient_NetFileGetDownloadPos NetClient_NetFileGetDownloadPos;
	//-------------------------------------------------------------------------
	typedef int (__stdcall *pfNetClient_SetReducenoiseState)(int _iLogonID , int _iChannelNum,int _iState);
	extern  pfNetClient_SetReducenoiseState NetClient_SetReducenoiseState;
	typedef int (__stdcall *pfNetClient_GetReducenoiseState)(int _iLogonID , int _iChannelNum,int *_iStat);
	extern  pfNetClient_GetReducenoiseState NetClient_GetReducenoiseState;
	typedef int (__stdcall *pfNetClient_GetVersion)(SDK_VERSION *_ver);
	extern  pfNetClient_GetVersion NetClient_GetVersion;
	typedef int (__stdcall *pfNetClient_Bindinterface)(int _interface);
	extern	pfNetClient_Bindinterface	NetClient_Bindinterface;
	typedef char* (__stdcall *pfNetClient_GetNetinterface)(int _interface);
	extern  pfNetClient_GetNetinterface NetClient_GetNetinterface;
	//typedef int (__stdcall *pfNetClient_DSMSetDirectoryServer)(char* _cDServerIP1, WORD _wPort1, char* _cDServerIP2, WORD _wPort2,char* _cAccount, char* _cPassword);
	//extern  pfNetClient_DSMSetDirectoryServer NetClient_DSMSetDirectoryServer;
	typedef int (__stdcall *pfNetClient_ProxyGetEncoderList)(PUIDLIST _pIDList);
	extern  pfNetClient_ProxyGetEncoderList NetClient_ProxyGetEncoderList;
	typedef int (__stdcall *pfNetClient_ProxyGetEncoderInfo)(unsigned int _encoderID, PENCODERINFO _pLinkInfo);
	extern  pfNetClient_ProxyGetEncoderInfo NetClient_ProxyGetEncoderInfo;
	typedef int (__stdcall *pfNetClient_SetNVS)(int _iLogonID, int _iVideo, int _iCheck);
	extern  pfNetClient_SetNVS NetClient_SetNVS;
	typedef int (__stdcall *pfNetClient_SetMotionAreaEnable)(int _iLogonID,int _iChannelNum);
	extern	pfNetClient_SetMotionAreaEnable	NetClient_SetMotionAreaEnable;
	typedef int (__stdcall *pfNetClient_BindInterface)(int _interface);
	extern	pfNetClient_BindInterface	NetClient_BindInterface;
	typedef char*(__stdcall *pfNetClient_GetNetInterface)(int _interface);
	extern  pfNetClient_GetNetInterface NetClient_GetNetInterface;

	typedef int (__stdcall *pfNetClient_SetUPNPEnable)(int _iLogonID,int _iEnable);
	extern	pfNetClient_SetUPNPEnable	NetClient_SetUPNPEnable;
	typedef int (__stdcall *pfNetClient_GetUPNPEnable)(int _iLogonID,int	*_iEnable);
	extern	pfNetClient_GetUPNPEnable	NetClient_GetUPNPEnable;

	typedef	int	(__stdcall	*pfNetClient_GetSysInfo)(int	_iLogonID);
	extern	pfNetClient_GetSysInfo	NetClient_GetSysInfo;

	typedef int (__stdcall	*pfNetClient_SetWifiDHCPParam)(int _iLogonID,int _iDHCP);
	extern	pfNetClient_SetWifiDHCPParam	NetClient_SetWifiDHCPParam;

	typedef int (__stdcall	*pfNetClient_GetWifiDHCPParam)(int _iLogonID,int *_iDHCP);
	extern	pfNetClient_GetWifiDHCPParam	NetClient_GetWifiDHCPParam;

	typedef	int	(__stdcall	*pfNetClient_SetDDNSPara)(int _iLogonID,char* _cDUserName,char* _cDPassword,char* _cDNvsName, char* _cDomain,int _iPort,int _iDEnable);
	extern	pfNetClient_SetDDNSPara	NetClient_SetDDNSPara;

	typedef	int	(__stdcall	*pfNetClient_GetDDNSPara)(int _iLogonID,char* _cDUserName,char* _cPassword,char* _cDNvsName, char* _cDomain,int* _iPort,int* _iDEnable);
	extern	pfNetClient_GetDDNSPara	NetClient_GetDDNSPara;

	typedef	int	(__stdcall	*pfNetClient_GetProductType)(int _iLogonID,int *_iType);
	extern	pfNetClient_GetProductType	NetClient_GetProductType;

	typedef	int	(__stdcall	*pfNetClient_GetProductTypeEx)(int _iLogonID,int *_iMode,int *_iType);
	extern	pfNetClient_GetProductTypeEx	NetClient_GetProductTypeEx;

	typedef	int	(__stdcall	*pfNetClient_SetVideoparaSchedule)(int _iLogonID, int _iChannelNum,STR_VideoParam* _strVideoParam[MAX_TIMESEGMENT]);
	extern	pfNetClient_SetVideoparaSchedule	NetClient_SetVideoparaSchedule;

	typedef	int	(__stdcall	*pfNetClient_GetVideoparaSchedule)(int _iLogonID, int _iChannelNum,STR_VideoParam* _strVideoParam[MAX_TIMESEGMENT]);
	extern	pfNetClient_GetVideoparaSchedule	NetClient_GetVideoparaSchedule;

	typedef	int	(__stdcall	*pfNetClient_SetMotionDetectSchedule)(int _iLogonID,int _iChannelNum,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]);
	extern	pfNetClient_SetMotionDetectSchedule	NetClient_SetMotionDetectSchedule;

	typedef	int	(__stdcall	*pfNetClient_GetMotionDetectSchedule)(int _iLogonID,int _iChannelNum,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]);
	extern	pfNetClient_GetMotionDetectSchedule	NetClient_GetMotionDetectSchedule;

	typedef	int	(__stdcall	*pfNetClient_SetOutportAlarmDelay)(int _iLogonID,int _iPortNo,int _iClearType,int _iDelayTime);
	extern	pfNetClient_SetOutportAlarmDelay	NetClient_SetOutportAlarmDelay;

	typedef	int	(__stdcall	*pfNetClient_GetOutportAlarmDelay)(int _iLogonID,int _iPortNo,int *_iClearType,int *_iDelayTime);
	extern	pfNetClient_GetOutportAlarmDelay	NetClient_GetOutportAlarmDelay;

	typedef	int (__stdcall	*pfNetClient_SetInportAlarmSchedule)(int _iLogonID,int _iPortNo,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]);
	extern	pfNetClient_SetInportAlarmSchedule	NetClient_SetInportAlarmSchedule;

	typedef	int (__stdcall	*pfNetClient_GetInportAlarmSchedule)(int _iLogonID,int _iPortNo,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]);
	extern	pfNetClient_GetInportAlarmSchedule	NetClient_GetInportAlarmSchedule;

	typedef	int (__stdcall	*pfNetClient_SetVideoLostSchedule)(int _iLogonID,int _iChannelNum,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]);
	extern	pfNetClient_SetVideoLostSchedule	NetClient_SetVideoLostSchedule;

	typedef	int (__stdcall	*pfNetClient_GetVideoLostSchedule)(int _iLogonID,int _iChannelNum,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]);
	extern	pfNetClient_GetVideoLostSchedule	NetClient_GetVideoLostSchedule;

	typedef	int (__stdcall	*pfNetClient_SetMotionDecLinkRec)(int _iLogonID,int _iChannelNum,int _iEnable);
	extern	pfNetClient_SetMotionDecLinkRec	NetClient_SetMotionDecLinkRec;

	typedef	int (__stdcall	*pfNetClient_GetMotionDecLinkRec)(int _iLogonID,int _iChannelNum,int* _iEnable);
	extern	pfNetClient_GetMotionDecLinkRec	NetClient_GetMotionDecLinkRec;

	typedef	int (__stdcall	*pfNetClient_SetVideoLostLinkRec)(int _iLogonID,int _iChannelNum,int _iEnable);
	extern	pfNetClient_SetVideoLostLinkRec	NetClient_SetVideoLostLinkRec;

	typedef	int (__stdcall	*pfNetClient_GetVideoLostLinkRec)(int _iLogonID,int _iChannelNum,int*	_iEnable);
	extern	pfNetClient_GetVideoLostLinkRec	NetClient_GetVideoLostLinkRec;

	typedef	int (__stdcall	*pfNetClient_SetInportAlarmLinkRec)(int _iLogonID,int _iPortNo,int _iEnable);
	extern	pfNetClient_SetInportAlarmLinkRec	NetClient_SetInportAlarmLinkRec;

	typedef	int (__stdcall	*pfNetClient_GetInportAlarmLinkRec)(int _iLogonID,int _iPortNo,int* _iEnable);
	extern	pfNetClient_GetInportAlarmLinkRec	NetClient_GetInportAlarmLinkRec;

	typedef	int	(__stdcall	*pfNetClient_SetOutportAlarmSchedule)(int _iLogonID,int _iPortNo,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]);
	extern	pfNetClient_SetOutportAlarmSchedule	NetClient_SetOutportAlarmSchedule;

	typedef	int	(__stdcall	*pfNetClient_GetOutportAlarmSchedule)(int _iLogonID,int _iPortNo,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT]);
	extern	pfNetClient_GetOutportAlarmSchedule	NetClient_GetOutportAlarmSchedule;

	typedef	int (__stdcall	*pfNetClient_SetMotionDecLinkSnap)(int _iLogonID,int _iChannelNum,int _iEnable);
	extern	pfNetClient_SetMotionDecLinkSnap	NetClient_SetMotionDecLinkSnap;

	typedef	int (__stdcall	*pfNetClient_GetMotionDecLinkSnap)(int _iLogonID,int _iChannelNum,int* _iEnable);
	extern	pfNetClient_GetMotionDecLinkSnap	NetClient_GetMotionDecLinkSnap;

	typedef	int (__stdcall	*pfNetClient_SetInportAlarmLinkSnap)(int _iLogonID,int _iPortNo,int _iEnable);
	extern	pfNetClient_SetInportAlarmLinkSnap	NetClient_SetInportAlarmLinkSnap;

	typedef	int (__stdcall	*pfNetClient_GetInportAlarmLinkSnap)(int _iLogonID,int _iPortNo,int* _iEnable);
	extern	pfNetClient_GetInportAlarmLinkSnap	NetClient_GetInportAlarmLinkSnap;

	typedef	int (__stdcall	*pfNetClient_SetVideoLostLinkSnap)(int _iLogonID,int _iChannelNum,int _iEnable);
	extern	pfNetClient_SetVideoLostLinkSnap	NetClient_SetVideoLostLinkSnap;

	typedef	int (__stdcall	*pfNetClient_GetVideoLostLinkSnap)(int _iLogonID,int _iChannelNum,int* _iEnable);
	extern	pfNetClient_GetVideoLostLinkSnap	NetClient_GetVideoLostLinkSnap;

	typedef	int (__stdcall	*pfNetClient_SetMotionDecLinkOutport)(int _iLogonID,int _iChannelNum,int _iOutportArray);
	extern	pfNetClient_SetMotionDecLinkOutport	NetClient_SetMotionDecLinkOutport;

	typedef	int (__stdcall	*pfNetClient_GetMotionDecLinkOutport)(int _iLogonID,int _iChannelNum,int*	_iOutportArray);
	extern	pfNetClient_GetMotionDecLinkOutport	NetClient_GetMotionDecLinkOutport;

	typedef	int (__stdcall	*pfNetClient_SetInportAlmLinkOutport)(int _iLogonID,int _iPortNo,int _iOutportArray);
	extern	pfNetClient_SetInportAlmLinkOutport	NetClient_SetInportAlmLinkOutport;

	typedef	int (__stdcall	*pfNetClient_GetInportAlmLinkOutport)(int _iLogonID,int _iPortNo,int* _iOutportArray);
	extern	pfNetClient_GetInportAlmLinkOutport	NetClient_GetInportAlmLinkOutport;

	typedef	int (__stdcall	*pfNetClient_SetVideoLostLinkOutport)(int _iLogonID,int _iChannelNum,int _iOutportArray);
	extern	pfNetClient_SetVideoLostLinkOutport	NetClient_SetVideoLostLinkOutport;

	typedef	int (__stdcall	*pfNetClient_GetVideoLostLinkOutport)(int _iLogonID,int _iChannelNum,int* _iOutportArray);
	extern	pfNetClient_GetVideoLostLinkOutport	NetClient_GetVideoLostLinkOutport;

	typedef	int (__stdcall	*pfNetClient_SetInportAlarmLinkPTZ)(int _iLogonID,int _iPortNo,int _iLinkChannelNum,int _iLinkType,int _iActNum);
	extern	pfNetClient_SetInportAlarmLinkPTZ	NetClient_SetInportAlarmLinkPTZ;

	typedef	int (__stdcall	*pfNetClient_GetInportAlarmLinkPTZ)(int _iLogonID,int _iPortNo,int _iLinkChannelNum,int* _iLinkType,int* _iActNum);
	extern	pfNetClient_GetInportAlarmLinkPTZ	NetClient_GetInportAlarmLinkPTZ;

	typedef	int (__stdcall	*pfNetClient_SetVideoLostLinkPTZ)(int _iLogonID,int _iChannelNum,int _iLinkChannelNum,int _iLinkType,int _iActNum);
	extern	pfNetClient_SetVideoLostLinkPTZ	NetClient_SetVideoLostLinkPTZ;

	typedef	int (__stdcall	*pfNetClient_GetVideoLostLinkPTZ)(int _iLogonID,int _iChannelNum,int _iLinkChannelNum,int* _iLinkType,int* _iActNum);
	extern	pfNetClient_GetVideoLostLinkPTZ	NetClient_GetVideoLostLinkPTZ;

	typedef	int (__stdcall	*pfNetClient_SetInportEnable)(int _iLogonID, int _iPortNo,int _iEnable);
	extern	pfNetClient_SetInportEnable	NetClient_SetInportEnable;

	typedef	int (__stdcall	*pfNetClient_GetInportEnable)(int _iLogonID, int _iPortNo,int* _iEnable);
	extern	pfNetClient_GetInportEnable	NetClient_GetInportEnable;

	typedef	int (__stdcall	*pfNetClient_SetOutportEnable)(int _iLogonID, int _iPortNo,int _iEnable);
	extern	pfNetClient_SetOutportEnable	NetClient_SetOutportEnable;


	typedef	int (__stdcall	*pfNetClient_GetOutportEnable)(int _iLogonID, int _iPortNo,int* _iEnable);
	extern	pfNetClient_GetOutportEnable	NetClient_GetOutportEnable;
	//2009-3-16 -中兴通讯 --start
	typedef	int (__stdcall	*pfNetClient_SetZTEInfo)(int _iLogonID,char* _cTransType,int _iRtpPort,int _iRtspPort,char* _cAutoCleanAlarm,char* _cCleanAlarmDelay);
	extern	pfNetClient_SetZTEInfo		NetClient_SetZTEInfo;

	typedef	int (__stdcall	*pfNetClient_GetZTEInfo)(int _iLogonID,char* _cTransType,int* _iRtpPort,int* _iRtspPort,char* _cAutoCleanAlarm,char* _cCleanAlarmDelay);
	extern	pfNetClient_GetZTEInfo		NetClient_GetZTEInfo;

	typedef	int (__stdcall	*pfNetClient_SetOnlineState)(int _iLogonID,char* _cOnlineState);
	extern	pfNetClient_SetOnlineState	NetClient_SetOnlineState;

	typedef	int (__stdcall	*pfNetClient_GetOnlineState)(int _iLogonID,char* _cOnlineState);
	extern	pfNetClient_GetOnlineState	NetClient_GetOnlineState;
	//2009-3-16 -中兴通讯 --end

	//2009-4-21 -铁路项目 --start
	typedef	int (__stdcall	*pfNetClient_SetDZInfo)(int _iLogonID,DZ_INFO_PARAM *_pDZ_INFO);
	extern	pfNetClient_SetDZInfo		NetClient_SetDZInfo;

	typedef	int (__stdcall	*pfNetClient_GetDZInfo)(int _iLogonID,DZ_INFO_PARAM *_pDZ_INFO);
	extern	pfNetClient_GetDZInfo		NetClient_GetDZInfo;

	//2009-4-21 -铁路项目 --end

	typedef	int (__stdcall	*pfNetClient_ClearDisk)(int _iLogonID,int _iDiskNo);
	extern	pfNetClient_ClearDisk		NetClient_ClearDisk;

	typedef	int (__stdcall	*pfNetClient_GetMaxMinorVideoSize)(int _iLogonID,int* _iMinorVideoSize);
	extern	pfNetClient_GetMaxMinorVideoSize		NetClient_GetMaxMinorVideoSize;

	typedef	int (__stdcall	*pfNetClient_SetComServer)(int _iLogonID,char* _cComServer,int _iComServerPort);
	extern	pfNetClient_SetComServer		NetClient_SetComServer;

	typedef	int (__stdcall	*pfNetClient_GetComServer)(int _iLogonID,char* _cComServer,int* _iComServerPort);
	extern	pfNetClient_GetComServer		NetClient_GetComServer;

	typedef int (__stdcall *pfNetClient_SetVencType)(int _iLogonID, int _iChannelNum, int _iType);
	extern pfNetClient_SetVencType          NetClient_SetVencType;

	typedef int	(__stdcall *pfNetClient_GetVencType)(int _iLogonID, int _iChannelNum, int *_iType);
	extern pfNetClient_GetVencType          NetClient_GetVencType;


	typedef	int (__stdcall	*pfNetClient_SetPTZAutoBack)(int _iLogonID, int _iChannel,int _iEnable,int _iPresetIndex,int _iIdleTime);
	extern	pfNetClient_SetPTZAutoBack		NetClient_SetPTZAutoBack;

	typedef	int (__stdcall	*pfNetClient_GetPTZAutoBack)(int _iLogonID, int _iChannel,int *_iEnable,int *_iPresetIndex,int *_iIdleTime);
	extern	pfNetClient_GetPTZAutoBack		NetClient_GetPTZAutoBack;

	typedef int (__stdcall *pfNetClient_SetFuncListArray)(int _iLogonID,int _iEnableArray);
	extern  pfNetClient_SetFuncListArray    NetClient_SetFuncListArray;

	typedef int (__stdcall *pfNetClient_GetFuncListArray)(int _iLogonID,int *_iEnableArray);
	extern  pfNetClient_GetFuncListArray    NetClient_GetFuncListArray;

	typedef int (__stdcall *pfNetClient_GetOutportState)(int _iLogonID,int _iPort,int *_iState);
	extern  pfNetClient_GetOutportState     NetClient_GetOutportState;

	typedef int (__stdcall *pfNetClient_SetCPUMEMAlarmThreshold)(int _iLogonID, int _iCPUThreshold, int _iMEMThreshold);
	extern  pfNetClient_SetCPUMEMAlarmThreshold     NetClient_SetCPUMEMAlarmThreshold;

	typedef int (__stdcall *pfNetClient_GetCPUMEMAlarmThreshold)(int _iLogonID, int *_iCPUThreshold, int *_iMEMThreshold);
	extern  pfNetClient_GetCPUMEMAlarmThreshold     NetClient_GetCPUMEMAlarmThreshold;

	typedef int (__stdcall *pfNetClient_SetBothStreamSame)(int _iLogonID , int _iChannelNum, int _iState);
	extern  pfNetClient_SetBothStreamSame           NetClient_SetBothStreamSame;

	typedef int (__stdcall *pfNetClient_GetInportAlarmLinkSoundDisplay)(int _iLogonID,int _iPortNo,int *_iSoundEnable,int *_iDisplayEnable);
	extern  pfNetClient_GetInportAlarmLinkSoundDisplay      NetClient_GetInportAlarmLinkSoundDisplay;

	typedef int (__stdcall *pfNetClient_GetVideoLostLinkSoundDisplay)(int _iLogonID,int _iChannelNum,int *_iSoundEnable,int *_iDisplayEnable);
	extern  pfNetClient_GetVideoLostLinkSoundDisplay        NetClient_GetVideoLostLinkSoundDisplay;

	typedef int (__stdcall *pfNetClient_GetMotionDecLinkSoundDisplay)(int _iLogonID,int _iChannelNum,int *_iSoundEnable,int *_iDisplayEnable);
	extern  pfNetClient_GetMotionDecLinkSoundDisplay        NetClient_GetMotionDecLinkSoundDisplay;

	typedef int (__stdcall *pfNetClient_SetInportAlarmLinkSoundDisplay)(int _iLogonID,int _iPortNo,int _iSoundEnable,int _iDisplayEnable);
	extern  pfNetClient_SetInportAlarmLinkSoundDisplay      NetClient_SetInportAlarmLinkSoundDisplay;

	typedef int (__stdcall *pfNetClient_SetMotionDecLinkSoundDisplay)(int _iLogonID,int _iChannelNum,int _iSoundEnable,int _iDisplayEnable);
	extern  pfNetClient_SetMotionDecLinkSoundDisplay        NetClient_SetMotionDecLinkSoundDisplay;

	typedef int (__stdcall *pfNetClient_SetVideoLostLinkSoundDisplay)(int _iLogonID,int _iChannelNum,int _iSoundEnable,int _iDisplayEnable);
	extern  pfNetClient_SetVideoLostLinkSoundDisplay        NetClient_SetVideoLostLinkSoundDisplay;

	typedef int (__stdcall *pfNetClient_SetInportAlarmLinkSoundDisplay)(int _iLogonID,int _iPortNo,int _iSoundEnable,int _iDisplayEnable);
	extern  pfNetClient_SetInportAlarmLinkSoundDisplay      NetClient_SetInportAlarmLinkSoundDisplay;

	typedef int (__stdcall *pfNetClient_SetMotionDecLinkSoundDisplay)(int _iLogonID,int _iChannelNum,int _iSoundEnable,int _iDisplayEnable);
	extern  pfNetClient_SetMotionDecLinkSoundDisplay        NetClient_SetMotionDecLinkSoundDisplay;

	typedef int (__stdcall *pfNetClient_SetVideoLostLinkSoundDisplay)(int _iLogonID,int _iChannelNum,int _iSoundEnable,int _iDisplayEnable);
	extern  pfNetClient_SetVideoLostLinkSoundDisplay        NetClient_SetVideoLostLinkSoundDisplay;


	typedef int (__stdcall *pfNetClient_Get3GDeviceStatus)(int _iLogonID, int* _i3GDeviceType,int* _iStatus,int* _iIntensity,char* _pcIP,char* _pcStarttime);
	extern  pfNetClient_Get3GDeviceStatus        NetClient_Get3GDeviceStatus;

	typedef int (__stdcall *pfNetClient_Set3GDialog)(int _iLogonID, int _iStartType,int _iStopType,int _iDuration);
	extern  pfNetClient_Set3GDialog        NetClient_Set3GDialog;

	typedef int (__stdcall *pfNetClient_Get3GDialog)(int _iLogonID, int* _iStartType,int* _iStopType,int* _iDuration);
	extern  pfNetClient_Get3GDialog        NetClient_Get3GDialog;

	typedef int (__stdcall *pfNetClient_Set3GMessage)(int _iLogonID, char* _cNotify,char* _cPhoneNum1,char* _cPhoneNum2,char* _cPhoneNum3,char* _cPhoneNum4,char* _cPhoneNum5);
	extern  pfNetClient_Set3GMessage        NetClient_Set3GMessage;

	typedef int (__stdcall *pfNetClient_Get3GMessage)(int _iLogonID, char* _cNotify,char* _cPhoneNum1,char* _cPhoneNum2,char* _cPhoneNum3,char* _cPhoneNum4,char* _cPhoneNum5);
	extern  pfNetClient_Get3GMessage        NetClient_Get3GMessage;

	typedef int (__stdcall *pfNetClient_Set3GTaskSchedule)(int _iLogonID, int _iEnable, PNVS_SCHEDTIME _strScheduleParam);
	extern  pfNetClient_Set3GTaskSchedule   NetClient_Set3GTaskSchedule;
	typedef int (__stdcall *pfNetClient_Get3GTaskSchedule)(int _iLogonID, int* _iEnable, PNVS_SCHEDTIME _strScheduleParam);
	extern  pfNetClient_Get3GTaskSchedule   NetClient_Get3GTaskSchedule;

	typedef int (__stdcall *pfNetClient_Set3GNotify)(int _iLogonID, int _iType,char* _cMessage);
	extern  pfNetClient_Set3GNotify        NetClient_Set3GNotify;
	typedef int (__stdcall *pfNetClient_Get3GNotify)(int _iLogonID, int* _iType,char* _cMessage);
	extern  pfNetClient_Get3GNotify        NetClient_Get3GNotify;

	typedef int (__stdcall *pfNetClient_Set3GVPND)(int _iLogonID, char* _cDialNumber,char* _cAccount,char* _cPassword);
	extern  pfNetClient_Set3GVPND        NetClient_Set3GVPND;
	typedef int (__stdcall *pfNetClient_Get3GVPND)(int _iLogonID, char* _cDialNumber,char* _cAccount,char* _cPassword);
	extern  pfNetClient_Get3GVPND        NetClient_Get3GVPND;

	typedef int (__stdcall *pfNetClient_SetHDCamer)(int _iLogonID, int _iChannel,int _iFuncNum,int _iValue);
	extern  pfNetClient_SetHDCamer        NetClient_SetHDCamer;

	typedef int (__stdcall *pfNetClient_GetHDCamer)(int _iLogonID, int _iChannel,int _iFuncNum,int* _iValue);
	extern  pfNetClient_GetHDCamer        NetClient_GetHDCamer;

	typedef int (__stdcall *pfNetClient_GetComPortCounts)(const int _iLogonID, OUT int * _piComPortCounts, OUT int * _piComPortEnabled);
	extern  pfNetClient_GetComPortCounts    NetClient_GetComPortCounts;

	typedef int ( __stdcall *pfNetClient_GetDownloadFailedFileName)(const int _iLogonID, const int _iFileID, OUT char * _pcFileName, const int _iFileNameBufSize);
	extern pfNetClient_GetDownloadFailedFileName   NetClient_GetDownloadFailedFileName;

	typedef int (__stdcall *pfNetClient_SetHXListenPortInfo)(int _iLogonID,int _iVideoPort,int _iTalkPort,int _iMsgPort);
	extern  pfNetClient_SetHXListenPortInfo        NetClient_SetHXListenPortInfo;

	typedef int (__stdcall *pfNetClient_GetHXListenPortInfo)(int _iLogonID,int* _iVideoPort,int* _iTalkPort,int* _iMsgPort);
	extern  pfNetClient_GetHXListenPortInfo        NetClient_GetHXListenPortInfo;

	typedef int (__stdcall* pfNetClient_AdjustVideo)(unsigned int _uiConID, RECT _pRecOffset);
	extern  pfNetClient_AdjustVideo NetClient_AdjustVideo;

	typedef int (__stdcall* pfNetClient_ProxyAddProxyEx)(char* _cIP, char* _proxy, int _iNetMode, char* _pcProID ,	unsigned short _iPort, char* _pcUserName, char* _pcPassword);
	extern pfNetClient_ProxyAddProxyEx  NetClient_ProxyAddProxyEx;

	typedef int (__stdcall* pfNetClient_SetPlayerShowFrameMode)(int _iLogonID,int _iChannelNum,
		unsigned int _uiShowFrameMode, int _iStreamNO);
	extern  pfNetClient_SetPlayerShowFrameMode NetClient_SetPlayerShowFrameMode;

	typedef int (__stdcall* pfNetClient_GetPlayerShowFrameMode)(int _iLogonID,int _iChannelNum,int _iStreamNO);
	extern  pfNetClient_GetPlayerShowFrameMode NetClient_GetPlayerShowFrameMode;

	/*3520*/
	typedef int (__stdcall *pfNetClient_SetColorToGray)(int _iLogonID,int _iChannelNum, int _iEnable);
	extern  pfNetClient_SetColorToGray        NetClient_SetColorToGray;

	typedef int (__stdcall *pfNetClient_GetColorToGray)(int _iLogonID,int _iChannelNum, int* _iEnable);
	extern  pfNetClient_GetColorToGray        NetClient_GetColorToGray;

	typedef int (__stdcall *pfNetClient_SetColorParam)(int _iLogonID,int _iChannelNum, int _iColorToGradEnable, int _iDayRange, int _iNightRange)    ;
	extern  pfNetClient_SetColorParam          NetClient_SetColorParam;  \

		typedef int (__stdcall *pfNetClient_GetColorParam)(int _iLogonID,int _iChannelNum, int *_iColorToGradEnable, int *_iDayRange, int *_iNightRange)    ;
	extern  pfNetClient_GetColorParam          NetClient_GetColorParam;

	typedef int (__stdcall *pfNetClient_SetCustomChannelName)(int _iLogonID,int _iChannelNum,int _iChannelType, char* _cChannelName);
	extern  pfNetClient_SetCustomChannelName        NetClient_SetCustomChannelName;

	typedef int (__stdcall *pfNetClient_GetCustomChannelName)(int _iLogonID,int _iChannelNum,int _iChannelType, char* _cChannelName);
	extern  pfNetClient_GetCustomChannelName        NetClient_GetCustomChannelName;

	typedef int (__stdcall *pfNetClient_SetCustomRecType)(int _iLogonID,int _iRecType, char* _cRecTypeName);
	extern  pfNetClient_SetCustomRecType        NetClient_SetCustomRecType;

	typedef int (__stdcall *pfNetClient_GetCustomRecType)(int _iLogonID,int _iRecType, char* _cRecTypeName);
	extern  pfNetClient_GetCustomRecType        NetClient_GetCustomRecType;

	typedef int (__stdcall *pfNetClient_ChangeSvrIPEx)(int _iLogonID, char* _cNewSvrIP, char* _cMask, char* _cGateway, char* _cDNS, char* _cBackDNS);
	extern  pfNetClient_ChangeSvrIPEx        NetClient_ChangeSvrIPEx;

	typedef int (__stdcall *pfNetClient_GetIpPropertyEx)(int _iLogonID,char* _cMAC, char* _cMask, char* _cGateway, char* _cDNS, char* _cBackDNS);
	extern  pfNetClient_GetIpPropertyEx        NetClient_GetIpPropertyEx;

	typedef int (__stdcall *pfNetClient_SetFTPUpdate)(int _iLogonID,char* _cFtpAddr,char* _cFilePath,char* _cFtpUserName, char* _cFtpPassword);
	extern  pfNetClient_SetFTPUpdate        NetClient_SetFTPUpdate;

	typedef int (__stdcall *pfNetClient_GetFTPUpdate)(int _iLogonID,char* _cFtpAddr,char* _cFilePath,char* _cFtpUserName, char* _cFtpPassword);
	extern  pfNetClient_GetFTPUpdate        NetClient_GetFTPUpdate;

	typedef int (__stdcall *pfNetClient_SetCHNPTZFormat)(int _iLogonID,int _iChannelNum,char* _cComFormat);
	extern  pfNetClient_SetCHNPTZFormat        NetClient_SetCHNPTZFormat;

	typedef int (__stdcall *pfNetClient_GetCHNPTZFormat)(int _iLogonID,int _iChannelNum,char* _cComFormat);
	extern  pfNetClient_GetCHNPTZFormat        NetClient_GetCHNPTZFormat;

	typedef int (__stdcall *pfNetClient_SetOSDTypeColor)(int _iLogonID,int _iChannelNum,int _iOSDType,int _iColor);
	extern  pfNetClient_SetOSDTypeColor        NetClient_SetOSDTypeColor;

	typedef int (__stdcall *pfNetClient_GetOSDTypeColor)(int _iLogonID,int _iChannelNum,int _iOSDType,int* _iColor);
	extern  pfNetClient_GetOSDTypeColor        NetClient_GetOSDTypeColor;

	typedef int (__stdcall *pfNetClient_GetServerVersionEx)(int _iLogonID, char *_cVer,char *_cUIVersion,char* _cSlaveVersion);
	extern  pfNetClient_GetServerVersionEx     NetClient_GetServerVersionEx;

	typedef int (__stdcall *pfNetClient_GetExceptionMsg)(int _iLogonID,int _iExceptionTyep,int* _iState);
	extern  pfNetClient_GetExceptionMsg         NetClient_GetExceptionMsg;

	typedef int (__stdcall* pfNetClient_GetTalkingState)(int _iLogonID,int* _iTalkState);
	extern  pfNetClient_GetTalkingState NetClient_GetTalkingState;
	//------about multi monitor
	typedef int (__stdcall* pfNetClient_GetMonitorNum)();
	extern  pfNetClient_GetMonitorNum NetClient_GetMonitorNum;

	//typedef int (__stdcall* pfNetClient_GetMonitorInfo)(unsigned int _uiIndex,MONITOR_INFO* _MonitorInfo);
	//extern  pfNetClient_GetMonitorInfo NetClient_GetMonitorInfo; //4.0todo

	typedef int (__stdcall* pfNetClient_ChangeMonitor)(int _iLogonID,int _iChannelNum,unsigned int _uiIndex, int _iStreamNO);
	extern  pfNetClient_ChangeMonitor NetClient_ChangeMonitor;

	//typedef int (__stdcall* pfNetClient_UpdateMonitor)();
	//extern  pfNetClient_UpdateMonitor NetClient_UpdateMonitor;

	typedef int (__stdcall* pfNetClient_GetInfoByConnectID)(unsigned int _iConnectID,st_ConnectInfo* _stConnectInfo);
	extern  pfNetClient_GetInfoByConnectID NetClient_GetInfoByConnectID;

	typedef int (__stdcall* pfNetClient_InterTalkStart)(unsigned int * _uiConnID, int _iLogonID, int _iUserData);
	extern  pfNetClient_InterTalkStart      NetClient_InterTalkStart;

	typedef int (__stdcall* pfNetClient_InterTalkEnd)(unsigned int _uiConnID, bool _blStopTalk);
	extern  pfNetClient_InterTalkEnd    NetClient_InterTalkEnd;

	typedef int (__stdcall* pfNetClient_NetFileDownloadByTimeSpan)(unsigned int*	_ulConID, int _iLogonID, char* _pcLocalFile,
		int _iChannelNO, unsigned int _uiFromSecond, unsigned int _uiToSencond, int _iFlag, int _iPosition,int _Speed);
	extern pfNetClient_NetFileDownloadByTimeSpan NetClient_NetFileDownloadByTimeSpan;

	typedef int (__stdcall* pfNetClient_NetFileDownloadByTimeSpanEx)( unsigned int* _ulConID, int _iLogonID, char* _pcLocalFile,
		int _iChannelNO, NVS_FILE_TIME *_pTimeBegin, NVS_FILE_TIME *_pTimeEnd, int _iFlag, int _iPosition,int _iSpeed);
	extern pfNetClient_NetFileDownloadByTimeSpanEx  NetClient_NetFileDownloadByTimeSpanEx;

	typedef int (__stdcall *pfNetClient_SetNetFileDownloadFileCallBack)(unsigned long _ulConID, RECVDATA_NOTIFY_EX _cbkDataNotify, void* _lpUserData );
	extern pfNetClient_SetNetFileDownloadFileCallBack NetClient_SetNetFileDownloadFileCallBack;

	typedef int (__stdcall* pfNetClient_NetLogQuery)(int _iLogonID, PNVS_LOG_QUERY _logQuery);
	extern pfNetClient_NetLogQuery NetClient_NetLogQuery;
	typedef int (__stdcall* pfNetClient_NetLogGetLogfile)(int _iLogonID, int _iLogIndex, PNVS_LOG_DATA _pLogInfo);
	extern pfNetClient_NetLogGetLogfile NetClient_NetLogGetLogfile;
	typedef int (__stdcall* pfNetClient_NetLogGetLogCount)(int _iLogonID, int *_iTotalCount, int *_iCurrentCount);
	extern pfNetClient_NetLogGetLogCount NetClient_NetLogGetLogCount;

	typedef int (__stdcall* pfNetClient_GetProtocolList)( int _iLogonID, st_NVSProtocol* _pstNVSProtocol );
	extern pfNetClient_GetProtocolList NetClient_GetProtocolList;

	typedef int (__stdcall* pfNetClient_NetFileQueryEx)(int _iLogonID, PNVS_FILE_QUERY _pFileQuery, int _iSize);
	extern pfNetClient_NetFileQueryEx NetClient_NetFileQueryEx;

	typedef int (__stdcall* pfNetClient_ControlDeviceRecord)( int _iLogonID, int _iChannelID, int _iRecordType, int _iAction );
	extern pfNetClient_ControlDeviceRecord NetClient_ControlDeviceRecord;

	typedef int (__stdcall* pfNetClient_SetCHNPTZCRUISE)( int _iLogonID,int _iChannelNum,int _iCruiseNo,int _iEnable,int _iCruiseNum,st_PTZCruise* _stPTZCruise );
	extern pfNetClient_SetCHNPTZCRUISE NetClient_SetCHNPTZCRUISE;
	typedef int (__stdcall* pfNetClient_GetCHNPTZCRUISE)( int _iLogonID,int _iChannelNum,int _iCruiseNo,int* _iEnable,int* _iCruiseNum,st_PTZCruise* _stPTZCruise );
	extern pfNetClient_GetCHNPTZCRUISE NetClient_GetCHNPTZCRUISE;

	typedef int (__stdcall* pfNetClient_SetVIDEOCOVER_LINKRECORD)( int _iLogonID,int _iChannelNum,int _iEnableByBits );
	extern pfNetClient_SetVIDEOCOVER_LINKRECORD NetClient_SetVIDEOCOVER_LINKRECORD;
	typedef int (__stdcall* pfNetClient_GetVIDEOCOVER_LINKRECORD)(int _iLogonID,int _iChannelNum,int* _iEnableByBits );
	extern pfNetClient_GetVIDEOCOVER_LINKRECORD NetClient_GetVIDEOCOVER_LINKRECORD;

	typedef int (__stdcall* pfNetClient_SetVIDEOCOVER_LINKPTZ)(int _iLogonID,int _iChannelNum,int _iLinkChannel,int _iLinkType,int _iNo );
	extern pfNetClient_SetVIDEOCOVER_LINKPTZ NetClient_SetVIDEOCOVER_LINKPTZ;
	typedef int (__stdcall* pfNetClient_GetVIDEOCOVER_LINKPTZ)( int _iLogonID,int _iChannelNum,int _iLinkChannel,int* _iLinkType,int* _iNo );
	extern pfNetClient_GetVIDEOCOVER_LINKPTZ NetClient_GetVIDEOCOVER_LINKPTZ;

	typedef int (__stdcall* pfNetClient_SetVideoCoverSchedule)(int _iLogonID,int _iChannelNum,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT] );
	extern pfNetClient_SetVideoCoverSchedule NetClient_SetVideoCoverSchedule;
	typedef int (__stdcall* pfNetClient_GetVideoCoverSchedule)( int _iLogonID,int _iChannelNum,int _iWeekday,PNVS_SCHEDTIME _strScheduleParam[MAX_TIMESEGMENT] );
	extern pfNetClient_GetVideoCoverSchedule NetClient_GetVideoCoverSchedule;

	typedef int (__stdcall* pfNetClient_EZoomAdd)(unsigned int _uiConnID, HWND _hWnd, RECT _rctDisplay, unsigned int _uiMonitorIndex);
	extern pfNetClient_EZoomAdd NetClient_EZoomAdd;

	typedef int (__stdcall* pfNetClient_EZoomSet)(unsigned int _uiConnID, int _iEZoomID, RECT _rctVideo);
	extern pfNetClient_EZoomSet NetClient_EZoomSet;

	//    typedef int (__stdcall* pfNetClient_EZoomReset)(unsigned int _uiConnID, int _iEZoomID);
	//    extern pfNetClient_EZoomReset NetClient_EZoomReset;

	typedef int (__stdcall* pfNetClient_EZoomRemove)(unsigned int _uiConnID, int _iEZoomID);
	extern pfNetClient_EZoomRemove NetClient_EZoomRemove;

	typedef int (__stdcall *pNetClient_SetFTPUsage)(int _iLogonID,char* _cFtpAddr,int _iPort,char* _cFilePath,char* _cFtpUserName, char* _cFtpPassword,int _iUsage);
	extern pNetClient_SetFTPUsage NetClient_SetFTPUsage;

	typedef int (__stdcall *pNetClient_GetFTPUsage)(int _iLogonID,char* _cFtpAddr,int* _iPort,char* _cFilePath,char* _cFtpUserName, char* _cFtpPassword,int* _iUsage);
	extern  pNetClient_GetFTPUsage NetClient_GetFTPUsage;

	typedef int (__stdcall *pfNetClient_DrawPolyOnLocalVideo)(unsigned int _uiConnID, POINT* _pPointArray, int _iPointCount, int _iEnableArrow);
	extern pfNetClient_DrawPolyOnLocalVideo NetClient_DrawPolyOnLocalVideo;

	typedef int (__stdcall *pfNetClient_GetVideoParam)(unsigned int _uiConnID, int *_piWidth, int *_piHeight, int *_piFrameRate);
	extern pfNetClient_GetVideoParam NetClient_GetVideoParam;

	typedef int (__stdcall *pfNetClient_VCASetConfig)(int _iLogonID, int _iVCACmdID, int _iChannel, void* _lpCmdBuf, int _iCmdBufLen);
	extern pfNetClient_VCASetConfig NetClient_VCASetConfig;

	typedef int (__stdcall *pfNetClient_VCAGetConfig)(int _iLogonID, int _iVCACmdID, int _iChannel, void* _lpCmdBuf, int _iCmdBufLen);
	extern pfNetClient_VCAGetConfig NetClient_VCAGetConfig;

	typedef int (__stdcall *pfNetClient_VCARestart)(int _iLogonID);
	extern pfNetClient_VCARestart NetClient_VCARestart;

	typedef int (__stdcall *pfNetClient_VCARestartEx)(int _iLogonID, int _iChannelNo);
	extern pfNetClient_VCARestartEx NetClient_VCARestartEx;

	typedef int (__stdcall *pfNetClient_VCAGetAlarmInfo)(int _iLogonID, int _iAlarmIndex, void* _lpBuf, int _iBufSize);
	extern pfNetClient_VCAGetAlarmInfo NetClient_VCAGetAlarmInfo;

	typedef int (__stdcall *pfNetClient_GetEmailAlarmEx)(int _iLogonID, PSMTP_INFO _pSmtp, int _iSize);
	extern pfNetClient_GetEmailAlarmEx NetClient_GetEmailAlarmEx;

	typedef int (__stdcall *pfNetClient_SetEmailAlarmEx)(int _iLogonID, PSMTP_INFO _pSmtp, int _iSize);
	extern pfNetClient_SetEmailAlarmEx NetClient_SetEmailAlarmEx;

	typedef int (__stdcall *pfNetClient_SetFTPUploadConfig)(int _iLogonID, int _iCmd, void* _lpCmdBuf, int _iBufSize);
	extern pfNetClient_SetFTPUploadConfig NetClient_SetFTPUploadConfig;

	typedef int (__stdcall *pfNetClient_GetFTPUploadConfig)(int _iLogonID, int _iCmd, void* _lpCmdBuf, int _iBufSize);
	extern pfNetClient_GetFTPUploadConfig NetClient_GetFTPUploadConfig;

	typedef int (__stdcall* pfNetClient_GetHDCamerEx)( int _iLogonID, int _iChannel,int _iFuncNum, void* _lpValueBuf, int _iSize );
	extern pfNetClient_GetHDCamerEx NetClient_GetHDCamerEx;

	typedef int (__stdcall* pfNetClient_SetHDCamerEx)( int _iLogonID, int _iChannel,int _iFuncNum, void* _lpValueBuf, int _iSize );
	extern pfNetClient_SetHDCamerEx NetClient_SetHDCamerEx;

	typedef int (__stdcall* pfNetClient_Set3GConfig)(int _iLogonID, int _iCmd, int _iChannel, void* _lpValueBuf, int _iBufSize);
	extern pfNetClient_Set3GConfig NetClient_Set3GConfig;

	typedef int (__stdcall* pfNetClient_Get3GConfig)(int _iLogonID, int _iCmd, int _iChannel, void* _lpValueBuf, int _iBufSize);
	extern pfNetClient_Get3GConfig NetClient_Get3GConfig;

	typedef int (__stdcall* pfNetClient_SetCDBurnConfig)(int _iLogonID, int _iChannel, int _iCmd, void* _lpCmdBuf, int _iBufSize);
	extern pfNetClient_SetCDBurnConfig NetClient_SetCDBurnConfig;
	typedef int (__stdcall* pfNetClient_GetCDBurnConfig)(int _iLogonID, int _iChannel, int _iCmd, void* _lpCmdBuf, int _iBufSize);
	extern pfNetClient_GetCDBurnConfig NetClient_GetCDBurnConfig;
	typedef int (__stdcall* pfNetClient_SetVideoEncrypt)(int _iLogonID, int _iChannel, void* _lpCmdBuf, int _iBufSize);
	extern pfNetClient_SetVideoEncrypt NetClient_SetVideoEncrypt;
	typedef int (__stdcall* pfNetClient_GetVideoEncrypt)(int _iLogonID, int _iChannel, void* _lpCmdBuf, int _iBufSize);
	extern pfNetClient_GetVideoEncrypt NetClient_GetVideoEncrypt;

	typedef int (__stdcall* pfNetClient_SetVideoDecrypt)(int _iLogonID, int _iChannel, void* _lpCmdBuf, int _iBufSize);
	extern pfNetClient_SetVideoDecrypt NetClient_SetVideoDecrypt;
	typedef int (__stdcall* pfNetClient_GetVideoDecrypt)(int _iLogonID, int _iChannel, void* _lpCmdBuf, int _iBufSize);
	extern pfNetClient_GetVideoDecrypt NetClient_GetVideoDecrypt;

	typedef int (__stdcall* pfNetClient_SetPreRecEnable)( int _iLogonID, int _iChannel, int _iEnable );
	extern pfNetClient_SetPreRecEnable NetClient_SetPreRecEnable;
	typedef int (__stdcall* pfNetClient_GetPreRecEnable)( int _iLogonID, int _iChannel, int* _piEnable );
	extern pfNetClient_GetPreRecEnable NetClient_GetPreRecEnable;

	//add by zhy 2013.04.06&2013.04.08&2013.04.11
	typedef int (__stdcall* pfNetClient_SetDiskGroup)( int _iLogonID,void* _lpBuf, int _iDiskGroupNum);
	extern pfNetClient_SetDiskGroup NetClient_SetDiskGroup;
	typedef int (__stdcall* pfNetClient_GetDiskGroup)( int _iLogonID,void* _lpBuf, int _iDiskGroupNum);
	extern pfNetClient_GetDiskGroup NetClient_GetDiskGroup;
	typedef int (__stdcall* pfNetClient_SetDiskQuota)( int _iLogonID, int _iChannel, void* _lpBuf, int _iBufSize);
	extern pfNetClient_SetDiskQuota NetClient_SetDiskQuota;
	typedef int (__stdcall* pfNetClient_GetDiskQuotaState)( int _iLogonID, int _iChannelNo, int _iCmd, void* _lpBuf, int _iBufSize );
	extern pfNetClient_GetDiskQuotaState NetClient_GetDiskQuotaState;
	typedef int (__stdcall* pfNetClient_ModifyUserAuthority)( int _iLogonID, char* _pcUserName, void* _lpBuf, int _iBufSize );
	extern pfNetClient_ModifyUserAuthority NetClient_ModifyUserAuthority;
	typedef int (__stdcall* pfNetClient_GetUserAuthority)( int _iLogonID, char* _pcUserName, void* _lpBuf, int _iBufSize );
	extern pfNetClient_GetUserAuthority NetClient_GetUserAuthority;
	typedef int (__stdcall* pfNetClient_GetGroupAuthority)( int _iLogonID, void* _lpBuf, int _iBufSize);
	extern pfNetClient_GetGroupAuthority NetClient_GetGroupAuthority;
	typedef int (__stdcall *pfNetClient_SetHolidayPlan)(int _iLogonID,int _iCmd,void* _lpBuf, int _iBufSize);
	extern pfNetClient_SetHolidayPlan NetClient_SetHolidayPlan;
	typedef int (__stdcall *pfNetClient_GetHolidayPlan)(int _iLogonID,int _iCmd,void* _lpBuf, int _iBufSize);
	extern pfNetClient_GetHolidayPlan NetClient_GetHolidayPlan;
	typedef int (__stdcall *pfNetClient_SetCommonEnable)( int _iLogonID, DWORD_PTR _iEnableID, int _iChannel, int _iEnableValue );
	extern pfNetClient_SetCommonEnable NetClient_SetCommonEnable;
	typedef int (__stdcall *pfNetClient_GetCommonEnable)( int _iLogonID, DWORD_PTR _iEnableID, int _iChannel, int* _iEnableValue );
	extern pfNetClient_GetCommonEnable NetClient_GetCommonEnable;

	//add by zhy end

	typedef int (__stdcall *pfNetClient_SetDigitalChannelConfig)(int _iLogonID, int _iChannel, int _iCmd, void* _lpCmdBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_SetDigitalChannelConfig);
	typedef int (__stdcall *pfNetClient_GetDigitalChannelConfig)(int _iLogonID, int _iChannel, int _iCmd, void* _lpCmdBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_GetDigitalChannelConfig);
	typedef int (__stdcall *pfNetClient_DigitalChannelSend)(int _iLogonID, int _iChannel, unsigned char* _ucBuf, int _iLength);
	DECLARE_FUNCTION(NetClient_DigitalChannelSend);
	typedef int (__stdcall *pfNetClient_SetVideoNPModeEx)(int _iLogonID, int _iChannel, VIDEO_NORM _iNPMode);
	DECLARE_FUNCTION(NetClient_SetVideoNPModeEx);
	typedef int (__stdcall *pfNetClient_GetVideoNPModeEx)(int _iLogonID, int _iChannel, VIDEO_NORM *_piNPMode);
	DECLARE_FUNCTION(NetClient_GetVideoNPModeEx);
	typedef int (__stdcall *pfNetClient_SetVideoModeMethod)(int _iLogonID, int _iChannel, VIDEO_METHOD  _iMethod);
	DECLARE_FUNCTION(NetClient_SetVideoModeMethod);
	typedef int (__stdcall *pfNetClient_GetVideoModeMethod)(int _iLogonID, int _iChannel, VIDEO_METHOD  *_piMethod);
	DECLARE_FUNCTION(NetClient_GetVideoModeMethod);

	typedef int (__stdcall *pfNetClient_GetChannelSipConfig)(int _iLogonID, int _iChannel, int _iCmd, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_GetChannelSipConfig);
	typedef int (__stdcall *pfNetClient_SetChannelSipConfig)(int _iLogonID, int _iChannel, int _iCmd, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_SetChannelSipConfig);

	//typedef int (__stdcall *pfNetClient_SetPsPackCallBack)(unsigned int, PSPACK_NOTIFY	_cbkPsPackNotify, void*	_pContext);
	//DECLARE_FUNCTION(NetClient_SetPsPackCallBack);//4.0todo

	typedef int (__stdcall *pfNetClient_StartRecvEx)(unsigned int *_ulConID,CLIENTINFO* _cltInfo,RECVDATA_NOTIFY_EX _cbkDataNotifyEx, void* _lpUserData);
	DECLARE_FUNCTION(NetClient_StartRecvEx);

	typedef int (__stdcall *pfNetClient_SetVideoCombine)(int _iLogonID, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_SetVideoCombine);
	typedef int (__stdcall *pfNetClient_GetVideoCombine)(int _iLogonID, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_GetVideoCombine);


	typedef int (__stdcall *pfNetClient_GetConnectInfo)(int _iLogonID, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_GetConnectInfo);

	typedef int (__stdcall *pfNetClient_SetPlatformApp)(int _iLogonID, int _iCmd, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_SetPlatformApp);

	typedef int (__stdcall *pfNetClient_GetPlatformApp)(int _iLogonID, int _iCmd, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_GetPlatformApp);

	typedef int (__stdcall *pfNetClient_GetDigitalChannelNum)(int _iLogonID, int* _piDigitalChannelNum);
	DECLARE_FUNCTION(NetClient_GetDigitalChannelNum);

	typedef int (__stdcall *pfNetClient_SetITSTimeRange)(int _iLogonID, int _iIndex, int _iCmd, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_SetITSTimeRange);

	typedef int (__stdcall *pfNetClient_GetITSTimeRange)(int _iLogonID, int _iIndex, int _iCmd, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_GetITSTimeRange);

	/* typedef int (__stdcall *pfNetClient_SetITSTimeRangeParam)(int _iLogonID, int _iIndex, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_SetITSTimeRangeParam);

	typedef int (__stdcall *pfNetClient_GetITSTimeRangeParam)(int _iLogonID, int _iIndex, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_GetITSTimeRangeParam);

	typedef int (__stdcall *pfNetClient_SetITSAgcBlockEnable)(int _iLogonID, int _iIndex, int _iAgcBlock);
	DECLARE_FUNCTION(NetClient_SetITSAgcBlockEnable);

	typedef int (__stdcall *pfNetClient_GetITSAgcBlockEnable)(int _iLogonID, int _iIndex, int* _iAgcBlock);
	DECLARE_FUNCTION(NetClient_GetITSAgcBlockEnable);

	typedef int (__stdcall *pfNetClient_SetITSFlashLampEnable)(int _iLogonID, int _iIndex, int _iEnable);
	DECLARE_FUNCTION(NetClient_SetITSFlashLampEnable);

	typedef int (__stdcall *pfNetClient_GetITSFlashLampEnable)(int _iLogonID, int _iIndex, int* _iEnable);
	DECLARE_FUNCTION(NetClient_GetITSFlashLampEnable);   */

	typedef int (__stdcall *pfNetClient_SetITSDetectMode)(int _iLogonID, int _iMode);
	DECLARE_FUNCTION(NetClient_SetITSDetectMode);

	typedef int (__stdcall *pfNetClient_GetITSDetectMode)(int _iLogonID, int* _iMode);
	DECLARE_FUNCTION(NetClient_GetITSDetectMode);

	typedef int (__stdcall *pfNetClient_SetITSBlock)(int _iLogonID, int _iBlockID, int _iX, int _iY);
	DECLARE_FUNCTION(NetClient_SetITSBlock);

	typedef int (__stdcall *pfNetClient_GetITSBlock)(int _iLogonID, int _iBlockID, int* _iX, int* _iY);
	DECLARE_FUNCTION(NetClient_GetITSBlock);

	typedef int (__stdcall *pfNetClient_SetITSLoopMode)(int _iLogonID, int _iMode);
	DECLARE_FUNCTION(NetClient_SetITSLoopMode);

	typedef int (__stdcall *pfNetClient_GetITSLoopMode)(int _iLogonID, int* _iMode);
	DECLARE_FUNCTION(NetClient_GetITSLoopMode);

	typedef int (__stdcall *pfNetClient_SetITSDeviceType)(int _iLogonID, int _iDeviceType);
	DECLARE_FUNCTION(NetClient_SetITSDeviceType);

	typedef int (__stdcall *pfNetClient_GetITSDeviceType)(int _iLogonID, int* _iDeviceType);
	DECLARE_FUNCTION(NetClient_GetITSDeviceType);

	typedef int (__stdcall *pfNetClient_SetITSSwitchTime)(int _iLogonID, int _iSwitchTime, int _iDelayTime);
	DECLARE_FUNCTION(NetClient_SetITSSwitchTime);

	typedef int (__stdcall *pfNetClient_GetITSSwitchTime)(int _iLogonID, int* _iSwitchTime, int* _iDelayTime);
	DECLARE_FUNCTION(NetClient_GetITSSwitchTime);

	typedef int (__stdcall *pfNetClient_SetJPEGQuality)(int _iLogonID, int _iJpegQuality);
	DECLARE_FUNCTION(NetClient_SetJPEGQuality);

	typedef int (__stdcall *pfNetClient_GetJPEGQuality)(int _iLogonID, int* _iJpegQuality);
	DECLARE_FUNCTION(NetClient_GetJPEGQuality);

	typedef int (__stdcall *pfNetClient_SetOSDTypeFontSize)(int _iLogonID, int _iChannelNum, int _iOsdType, int _iSize);
	DECLARE_FUNCTION(NetClient_SetOSDTypeFontSize);

	typedef int (__stdcall *pfNetClient_GetOSDTypeFontSize)(int _iLogonID, int _iChannelNum, int _iOsdType, int* _iSize);
	DECLARE_FUNCTION(NetClient_GetOSDTypeFontSize);

	typedef int (__stdcall *pfNetClient_SetChannelProperty)(int _iLogonID, int _iChannel, int _iCmd, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_SetChannelProperty);
	typedef int (__stdcall *pfNetClient_GetChannelProperty)(int _iLogonID, int _iChannel, int _iCmd, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_GetChannelProperty);

	typedef int (__stdcall *pfNetClient_SetITSRoadwayParam)(int _iLogonID, int _iCmd, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_SetITSRoadwayParam);

	typedef int (__stdcall *pfNetClient_GetITSRoadwayParam)(int _iLogonID, int _iCmd, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_GetITSRoadwayParam);

	typedef int (__stdcall *pfNetClient_SetITSLicensePlateOptimize)(int _iLogonID, int _iCmd, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_SetITSLicensePlateOptimize);

	typedef int (__stdcall *pfNetClient_GetITSLicensePlateOptimize)(int _iLogonID, int _iCmd, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_GetITSLicensePlateOptimize);

	typedef int (__stdcall *pfNetClient_SetHDTimeRangeParam)(int _iLogonID, int _iChannel, int _iIndex, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_SetHDTimeRangeParam);

	typedef int (__stdcall *pfNetClient_GetHDTimeRangeParam)(int _iLogonID, int _iChannel, int _iIndex, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_GetHDTimeRangeParam);

	typedef int (__stdcall *pfNetClient_SetITSTimeRangeEnable)(int _iLogonID, int _iChannel, int _iIndex, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_SetITSTimeRangeEnable);

	typedef int (__stdcall *pfNetClient_GetITSTimeRangeEnable)(int _iLogonID, int _iChannel, int _iIndex, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_GetITSTimeRangeEnable);

	typedef int (__stdcall *pfNetClient_SetHDTemplateName)(int _iLogonID, int _iTemplateID, char* _cTemplateName);
	DECLARE_FUNCTION(NetClient_SetHDTemplateName);

	typedef int (__stdcall *pfNetClient_GetHDTemplateName)(int _iLogonID, int _iTemplateID, char* _cTemplateName);
	DECLARE_FUNCTION(NetClient_GetHDTemplateName);

	typedef int (__stdcall *pfNetClient_SetHDTemplateMap)(int _iLogonID, int _iChannel, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_SetHDTemplateMap);

	typedef int (__stdcall *pfNetClient_GetHDTemplateMap)(int _iLogonID, int _iChannel, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_GetHDTemplateMap);

	typedef int (__stdcall *pfNetClient_SetITSRecoParam)(int _iLogonID, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_SetITSRecoParam);

	typedef int (__stdcall *pfNetClient_GetITSRecoParam)(int _iLogonID, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_GetITSRecoParam);

	typedef int (__stdcall *pfNetClient_SetITSDayNight)(int _iLogonID, int _iTimeSegment);
	DECLARE_FUNCTION(NetClient_SetITSDayNight);

	typedef int (__stdcall *pfNetClient_GetITSDayNight)(int _iLogonID, int* _iTimeSegment);
	DECLARE_FUNCTION(NetClient_GetITSDayNight);

	typedef int (__stdcall *pfNetClient_SetITSCamLocation)(int _iLogonID, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_SetITSCamLocation);

	typedef int (__stdcall *pfNetClient_GetITSCamLocation)(int _iLogonID, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_GetITSCamLocation);

	typedef int (__stdcall *pfNetClient_SetITSWorkMode)(int _iLogonID, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_SetITSWorkMode);

	typedef int (__stdcall *pfNetClient_GetITSWorkMode)(int _iLogonID, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_GetITSWorkMode);

	typedef int (__stdcall *pfNetClient_LogonEx)(char* _cProxy,char* _cIP,char* _cUserName,char* _cPassword,char* _pcProID,int _iPort /*= 0*/,char* _pcCharSet/* = "UTF-8")*/);
	extern pfNetClient_LogonEx		NetClient_LogonEx;

	typedef int (__stdcall *pfNetClient_SetTimeZone)(int _iLogonID, int _iTimeZone);
	extern pfNetClient_SetTimeZone  NetClient_SetTimeZone;

	typedef int (__stdcall *pfNetClient_GetTimeZone)(int _iLogonID, int* _piTimeZone);
	extern pfNetClient_GetTimeZone  NetClient_GetTimeZone;

	typedef int (__stdcall *pfNetClient_SetCurLanguage)(int _iLogonID, char* _pcLanguage);
	extern pfNetClient_SetCurLanguage NetClient_SetCurLanguage;

	typedef int (__stdcall *pfNetClient_GetCurLanguage)(int _iLogonID, char* _pcLanguage);
	extern pfNetClient_GetCurLanguage NetClient_GetCurLanguage;

	typedef int (__stdcall *pfNetClient_GetLanguageList)(int _iLogonID, st_NVSLanguageList* _pStrctLanguageList);
	extern pfNetClient_GetLanguageList NetClient_GetLanguageList;//4.0todo

	typedef int (__stdcall *pfNetClient_SetChannelEncodeProfile)(int _iLogonID, int _iChannelNum, int _iStreamNo, int _iLevel);
	extern pfNetClient_SetChannelEncodeProfile NetClient_SetChannelEncodeProfile;

	typedef int (__stdcall *pfNetClient_GetChannelEncodeProfile)(int _iLogonID, int _iChannelNum, int _iStreamNo, int* _piLevel);
	extern pfNetClient_GetChannelEncodeProfile NetClient_GetChannelEncodeProfile;


	typedef int (__stdcall *pfNetClient_SetExceptionHandleParam)(int _iLogonID, int _iExceptionType, int _iFlag);
	extern pfNetClient_SetExceptionHandleParam NetClient_SetExceptionHandleParam;

	typedef int (__stdcall *pfNetClient_GetExceptionHandleParam)(int _iLogonID, int _iExceptionType, int  *_iFlag);
	extern pfNetClient_GetExceptionHandleParam NetClient_GetExceptionHandleParam;

	typedef int (__stdcall *pfNetClient_SetAlarmClear)(int _iLogonID, int _iChannelNo, int _iClearType);
	extern pfNetClient_SetAlarmClear NetClient_SetAlarmClear;

	typedef int (__stdcall *pfNetClient_GetCharSet)(int _iLogonID, char* _pcCharSet);
	extern pfNetClient_GetCharSet NetClient_GetCharSet;

	typedef int (__stdcall *pfNetClient_SetImgDisposal)( int _iLogonID,void * _lpBuf, int _iBufSize);
	extern pfNetClient_SetImgDisposal NetClient_SetImgDisposal;

	typedef int (__stdcall *pfNetClient_GetImgDisposal)( int _iLogonID,void * _lpBuf, int _iBufSize);
	extern pfNetClient_GetImgDisposal NetClient_GetImgDisposal;

	typedef int (__stdcall *pfNetClient_SetWaterMarkEnable)( int _iLogonID,int _iChannelID,int _iEnable);
	extern pfNetClient_SetWaterMarkEnable NetClient_SetWaterMarkEnable;

	typedef int (__stdcall *pfNetClient_GetWaterMarkEnable)( int _iLogonID,int _iChannelID,int* _piEnable);
	extern pfNetClient_GetWaterMarkEnable NetClient_GetWaterMarkEnable;

	typedef int (__stdcall *pfNetClient_SetSystemType)( int _iLogonID,int _iType);
	extern pfNetClient_SetSystemType NetClient_SetSystemType;

	typedef int (__stdcall *pfNetClient_GetSystemType)( int _iLogonID,int* _piType);
	extern pfNetClient_GetSystemType NetClient_GetSystemType;

	typedef int (__stdcall *pfNetClient_SetEncryptSN)( int _iLogonID,int _iType,char* _cSN);
	extern pfNetClient_SetEncryptSN NetClient_SetEncryptSN;

	typedef int (__stdcall *pfNetClient_GetSNReg)( int _iLogonID,int* _piLockRet);
	extern pfNetClient_GetSNReg NetClient_GetSNReg;

	typedef int (__stdcall *pfNetClient_SetAlarmLink_V1)(int _iLogonID, int _iChannelNo, int _iAlarmKLinkType, void * _pParam, int _iParamSize );
	extern pfNetClient_SetAlarmLink_V1 NetClient_SetAlarmLink_V1;

	typedef int (__stdcall *pfNetClient_GetAlarmLink_V1)(int _iLogonID, int _iChannelNo, int _iAlarmKLinkType, void * _pParam, int _iParamSize );
	extern pfNetClient_GetAlarmLink_V1 NetClient_GetAlarmLink_V1;

	typedef int (__stdcall *pfNetClient_SetCameraParam)(int _iLogonID, void * _lpBuf, int _iBufSize);
	extern pfNetClient_SetCameraParam NetClient_SetCameraParam;

	typedef int (__stdcall *pfNetClient_GetCameraParam)(int _iLogonID, void * _lpBuf, int _iBufSize);
	extern pfNetClient_GetCameraParam NetClient_GetCameraParam;

	typedef int (__stdcall *pfNetClient_SetITSLightInfo)(int _iLogonID, void * _lpBuf, int _iBufSize);
	extern pfNetClient_SetITSLightInfo NetClient_SetITSLightInfo;

	typedef int (__stdcall *pfNetClient_GetITSLightInfo)(int _iLogonID, void * _lpBuf, int _iBufSize);
	extern pfNetClient_GetITSLightInfo NetClient_GetITSLightInfo;

	//TYPEDEF_FUNCTION(NetClient_GetServerVersion_V1)(int _iLogonID, SERVER_VERSION *_pstrctServerVer);
	//DECLARE_FUNCTION(NetClient_GetServerVersion_V1);//4.0todo

	TYPEDEF_FUNCTION(NetClient_CheckDeviceState)(int _iLogonID, int _iChannelNo, int _iType);
	DECLARE_FUNCTION(NetClient_CheckDeviceState);

	TYPEDEF_FUNCTION(NetClient_GetDeviceState)(int _iLogonID, int _iChannelNo, int _iType, int *_iValue);
	DECLARE_FUNCTION(NetClient_GetDeviceState);

	typedef int (__stdcall *pfNetClient_SetITSExtraInfo)(int _iLogonID, int _iCmd, int _iChannel, void* _pvCmdBuf, int _iCmdLen);
	extern pfNetClient_SetITSExtraInfo NetClient_SetITSExtraInfo;

	typedef int (__stdcall *pfNetClient_GetITSExtraInfo)(int _iLogonID, int _iCmd, int _iChannel, void* _pvCmdBuf, int _iCmdLen);
	extern pfNetClient_GetITSExtraInfo NetClient_GetITSExtraInfo;

	typedef int (__stdcall *pfNetClient_SetATMConfig)(int _iLogonID,int _iCmd, void* _lpBuf);
	extern pfNetClient_SetATMConfig NetClient_SetATMConfig;//ATM相关

	typedef int (__stdcall *pfNetClient_GetATMConfig)(int _iLogonID,int _iCmd, void* _lpBuf);
	extern pfNetClient_GetATMConfig NetClient_GetATMConfig;//ATM相关

	TYPEDEF_FUNCTION(NetClient_SetVerticalSync)(unsigned int _ulConID,int _iFlag);
	DECLARE_FUNCTION(NetClient_SetVerticalSync);

	TYPEDEF_FUNCTION(NetClient_GetVerticalSync)(unsigned int _ulConID,int *_iFlag);
	DECLARE_FUNCTION(NetClient_GetVerticalSync);

	TYPEDEF_FUNCTION(NetClient_SetLocalAudioVolumeEx)(unsigned int _ulConID, int _iVolume);
	DECLARE_FUNCTION(NetClient_SetLocalAudioVolumeEx);

	TYPEDEF_FUNCTION(NetClient_GetLocalAudioVolumeEx)(unsigned int _ulConID, int *_iVolume);
	DECLARE_FUNCTION(NetClient_GetLocalAudioVolumeEx);

	TYPEDEF_FUNCTION(NetClient_SetMuted)(unsigned int _uiConID, int _iMuted);
	DECLARE_FUNCTION(NetClient_SetMuted);

	TYPEDEF_FUNCTION(NetClient_SetLanParam)(int _iLogonID, int _iCmd, void* _lpData);
	DECLARE_FUNCTION(NetClient_SetLanParam);

	TYPEDEF_FUNCTION(NetClient_GetLanParam)(int _iLogonID, int _iCmd, void* _lpData);
	DECLARE_FUNCTION(NetClient_GetLanParam);

	TYPEDEF_FUNCTION(NetClient_GetVideoSzList)( int _iLogonID, int _iChannel, int _iStreamNo, int* _piList, int*  _piLstCount );   //chenbinzhushi
	DECLARE_FUNCTION(NetClient_GetVideoSzList);

	TYPEDEF_FUNCTION(NetClient_SetPWMValue)(int _iLogonID, int _iChannelNo, int _iValue);      //120213
	DECLARE_FUNCTION(NetClient_SetPWMValue);

	TYPEDEF_FUNCTION(NetClient_GetPWMValue)(int _iLogonID, int _iChannelNo, int* _piValue);
	DECLARE_FUNCTION(NetClient_GetPWMValue);

	TYPEDEF_FUNCTION(NetClient_ShutDownDev)(int _iLogonID,int _iFlag);      //120224
	DECLARE_FUNCTION(NetClient_ShutDownDev);

	TYPEDEF_FUNCTION(NetClient_ClearPolyLocalVideo)(unsigned int , int );
	DECLARE_FUNCTION(NetClient_ClearPolyLocalVideo);

	TYPEDEF_FUNCTION(NetClient_SetAlarmConfig)(int _iLogonID, int _iChannel, int _iAlarmType, int _iCmd, void* _pvCmdBuf);
	DECLARE_FUNCTION(NetClient_SetAlarmConfig);

	TYPEDEF_FUNCTION(NetClient_GetAlarmConfig)(int _iLogonID, int _iChannel, int _iAlarmType, int _iCmd, void* _pvCmdBuf);
	DECLARE_FUNCTION(NetClient_GetAlarmConfig);

	TYPEDEF_FUNCTION(NetClient_SetITSSwitchInterval)(int _iLogonID, void* _pBuf);
	DECLARE_FUNCTION(NetClient_SetITSSwitchInterval);

	TYPEDEF_FUNCTION(NetClient_GetITSSwitchInterval)(int _iLogonID, void* _pBuf);
	DECLARE_FUNCTION(NetClient_GetITSSwitchInterval);

	TYPEDEF_FUNCTION(NetClient_NetFileSetChannelParam)(int _iLogonID,int _iChannelNo,int _iCmd, void* _lpBuf);
	DECLARE_FUNCTION(NetClient_NetFileSetChannelParam);

	TYPEDEF_FUNCTION(NetClient_NetFileGetChannelParam)(int _iLogonID,int _iChannelNo,int _iCmd, void* _lpBuf);
	DECLARE_FUNCTION(NetClient_NetFileGetChannelParam);
	
	TYPEDEF_FUNCTION(NetClient_NetFileGetDiskInfoEx)(int _iLogonID, PNVS_STORAGEDEV	_storage, int _iSize);
	DECLARE_FUNCTION(NetClient_NetFileGetDiskInfoEx);

	TYPEDEF_FUNCTION(NetClient_InterTalkStartEx)(unsigned int * _uiConnID, int _iLogonID, NVSDATA_NOTIFY _cbkDataArrive, int _iUserData);
	DECLARE_FUNCTION(NetClient_InterTalkStartEx);

	TYPEDEF_FUNCTION(NetClient_SetAudioSample)(int _iLogonID, int _iChannel,int _iCmd, void* _lpBuf);
	DECLARE_FUNCTION(NetClient_SetAudioSample);

	TYPEDEF_FUNCTION(NetClient_GetAudioSample)(int _iLogonID, int _iChannel,int _iCmd, void* _lpBuf);
	DECLARE_FUNCTION(NetClient_GetAudioSample);

	TYPEDEF_FUNCTION(NetClient_Startup_V4)(int _iServerPort/*=3000*/, int _iClientPort/*=6000*/, int _iWnd/*=0*/);
	DECLARE_FUNCTION(NetClient_Startup_V4);

	TYPEDEF_FUNCTION(NetClient_SetNotifyFunction_V4)(MAIN_NOTIFY_V4 _MainNotify,ALARM_NOTIFY_V4 _cbkAlarmNotify,PARACHANGE_NOTIFY_V4 _cbkParamChangeNotify,COMRECV_NOTIFY_V4 _cbkComRecvNotify,PROXY_NOTIFY _cbkProxyNotify);
	DECLARE_FUNCTION(NetClient_SetNotifyFunction_V4);

	TYPEDEF_FUNCTION(NetClient_SetAlarmNotify_V5)(ALARM_NOTIFY_V5 _pAlarm);
	DECLARE_FUNCTION(NetClient_SetAlarmNotify_V5);

	TYPEDEF_FUNCTION(NetClient_StartRecv_V4)(unsigned int* _uiConnID, CLIENTINFO* _cltInfo, NVSDATA_NOTIFY _cbkDataArrive,int _iUserData);
	DECLARE_FUNCTION(NetClient_StartRecv_V4);

	TYPEDEF_FUNCTION(NetClient_StartRecv_V5)(unsigned int* _puiRecvID, NetClientPara* _ptPara, int _iParaSize);
	DECLARE_FUNCTION(NetClient_StartRecv_V5);

	TYPEDEF_FUNCTION(NetClient_SetNotifyUserData_V4)(int _iLogonID,int _iUserData);
	DECLARE_FUNCTION(NetClient_SetNotifyUserData_V4);

	TYPEDEF_FUNCTION(NetClient_SetComRecvNotify_V4)(COMRECV_NOTIFY_V4 _comNotify);
	DECLARE_FUNCTION(NetClient_SetComRecvNotify_V4);

	TYPEDEF_FUNCTION(NetClient_GetHTTPPort_V4)(int _iLogonID, int *_iPort); 
	DECLARE_FUNCTION(NetClient_GetHTTPPort_V4);

	TYPEDEF_FUNCTION(NetClient_SetHTTPPort_V4)(int _iLogonID, int _iPort); 
	DECLARE_FUNCTION(NetClient_SetHTTPPort_V4);

	TYPEDEF_FUNCTION(NetClient_SetDomainParsePara_V4)(int _iLogonID, int _iInterval, char* _cUserID, char* _cPassword, char* _cHost, char* _cDomain1, char* _cDomain2,int _iPort1, int _iPort2); 
	DECLARE_FUNCTION(NetClient_SetDomainParsePara_V4);

	TYPEDEF_FUNCTION(NetClient_GetDomainParsePara_V4)(int _iLogonID, int *_iInterval, char* _cUserID, char* _cPassword, char* _cHost, char* _cDomain1, char* _cDomain2,int* _iPort1, int* _iPort2);
	DECLARE_FUNCTION(NetClient_GetDomainParsePara_V4);

	TYPEDEF_FUNCTION(NetClient_GetBitrateOnVideo_V4)(unsigned int _ulConnID, int* _piX, int* _piY, int* _piEnabled, char *_pcInfo);
	DECLARE_FUNCTION(NetClient_GetBitrateOnVideo_V4);

	TYPEDEF_FUNCTION(NetClient_GetDevInfo)(int _iLogonID,ENCODERINFO *_pEncoderInfo);
	DECLARE_FUNCTION(NetClient_GetDevInfo);

	TYPEDEF_FUNCTION(NetClient_SetComFormat_V2)(int _iLogonID, COMFORMAT* _pComFormat);
	DECLARE_FUNCTION(NetClient_SetComFormat_V2);
	
	TYPEDEF_FUNCTION(NetClient_GetComFormat_V2)(int _iLogonID, COMFORMAT* _pComFormat);
	DECLARE_FUNCTION(NetClient_GetComFormat_V2);

	TYPEDEF_FUNCTION(NetClient_ATMQueryFile)( int _iLogonID, int _iCmd, void* _lpBuf );
	DECLARE_FUNCTION(NetClient_ATMQueryFile);

	TYPEDEF_FUNCTION(NetClient_ATMGetQueryFile)( int _iLogonID, int _iFileIndex, ATM_FILE_DATA* _pFileData );
	DECLARE_FUNCTION(NetClient_ATMGetQueryFile);

	TYPEDEF_FUNCTION(NetClient_GetServerVersion_V1)( int _iLogonID, SERVER_VERSION *_pstrctServerVer);
	DECLARE_FUNCTION(NetClient_GetServerVersion_V1);

	TYPEDEF_FUNCTION(NetClient_SetFullStreamNotify_V4)(unsigned int _uiRecvID,FULLFRAME_NOTIFY_V4 _cbkFullStream, int _iUserData);
	DECLARE_FUNCTION(NetClient_SetFullStreamNotify_V4);

	TYPEDEF_FUNCTION(NetClient_NetFileLockFile)( int _iLogonID, char* _cFileName, int _iLock);
	DECLARE_FUNCTION(NetClient_NetFileLockFile);//文件加锁

	TYPEDEF_FUNCTION(NetClient_NetFileGetQueryfileEx)(int _iLogonID,int _iFileIndex, PNVS_FILE_DATA_EX _pFileInfo);
	DECLARE_FUNCTION(NetClient_NetFileGetQueryfileEx);

	TYPEDEF_FUNCTION(NetClient_GetOsdTextEx)(int _iLogonID,int _iChannel,void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_GetOsdTextEx);

	TYPEDEF_FUNCTION(NetClient_NetFileDownload)(unsigned int *_ulConID,int _iLogonID,int _iCmd, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_NetFileDownload);

	TYPEDEF_FUNCTION(NetClient_Upgrade_V4)( int _iLogonID, char* _cFileName, int _iUpgradeType, UPGRADE_NOTIFY_V4 _UpgradeNotify );
	DECLARE_FUNCTION(NetClient_Upgrade_V4);

	TYPEDEF_FUNCTION(NetClient_SetStreamInsertData)(int _iLogonID, int _iChannelNo, int _iFlag, int _iType, void* _pDataBuf, int _iDataLen);
	DECLARE_FUNCTION(NetClient_SetStreamInsertData);

	TYPEDEF_FUNCTION(NetClient_GetStreamInsertData)(int _iLogonID, int _iChannelNo, int _iFlag, int _iType, void* _pDataBuf, int _iDataLen);
	DECLARE_FUNCTION(NetClient_GetStreamInsertData);

	TYPEDEF_FUNCTION(NetClient_GetOtherID)(int _iLogonID, void* _pcBuff, int _iBufLen);
	DECLARE_FUNCTION(NetClient_GetOtherID);

	TYPEDEF_FUNCTION(NetClient_SetDomePTZ)(int _iLogonID, int _iChannelNo, void *_lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_SetDomePTZ);

	TYPEDEF_FUNCTION(NetClient_GetDomePTZ)(int _iLogonID, int _iChannelNo, void *_lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_GetDomePTZ);

	TYPEDEF_FUNCTION(NetClient_GetUserDataInfo)(unsigned int _ulConID, int _iFlag, void*  _pBuffer, int _iSize);
	DECLARE_FUNCTION(NetClient_GetUserDataInfo);

	TYPEDEF_FUNCTION(NetClient_GetBroadcastMessage)( int _iLogonID,void *_lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_GetBroadcastMessage);

	TYPEDEF_FUNCTION(NetClient_SetDevConfig)(int _iLogonID, int _iCommand, int _iChannel, void *_lpInBuffer, int _iInBufferSize);
	DECLARE_FUNCTION(NetClient_SetDevConfig);

	TYPEDEF_FUNCTION(NetClient_GetDevConfig)(int _iLogonID, int _iCommand, int _iChannel, void *_lpOutBuffer, int _iOutBufferSize, int *_lpBytesReturned);
	DECLARE_FUNCTION(NetClient_GetDevConfig);
	TYPEDEF_FUNCTION(NetClient_SetJEPGSize)(int _iLogonID, int _iChannelNo, int _iWidth, int _iHeight);
	DECLARE_FUNCTION(NetClient_SetJEPGSize);
	TYPEDEF_FUNCTION(NetClient_GetJEPGSize)(int _iLogonID, int _iChannelNo, int* _iWidth, int * _iHeight);
	DECLARE_FUNCTION(NetClient_GetJEPGSize);
	TYPEDEF_FUNCTION(NetClient_QueryDevStatus)(int _iLogonID, int _iType);
	DECLARE_FUNCTION(NetClient_QueryDevStatus);
	TYPEDEF_FUNCTION(NetClient_GetDevStatus)(int _iLogonID, void* _lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_GetDevStatus);
	TYPEDEF_FUNCTION(NetClient_SendCommand)(int _iLogonID,  int _iCommand,  int _iChannel,  void* _pBuffer,  int _iBufferSize);
	DECLARE_FUNCTION(NetClient_SendCommand);
	TYPEDEF_FUNCTION(NetClient_RecvCommand)(int _iLogonID,  int _iCommand,  int _iChannel,  void* _pBuffer,  int _iBufferSize);
	DECLARE_FUNCTION(NetClient_RecvCommand);

	TYPEDEF_FUNCTION(NetClient_SetDsmConfig)(int _iCommand, void *_pvBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_SetDsmConfig);
	TYPEDEF_FUNCTION(NetClient_GetDsmRegstierInfo)(int _iCommand, void *_pvBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_GetDsmRegstierInfo);
	TYPEDEF_FUNCTION(NetClient_Logon_V4)(int _iLogonType, void *_pvPara, int _iInBufferSize);
	DECLARE_FUNCTION(NetClient_Logon_V4);

	TYPEDEF_FUNCTION(NetClient_SetDevDiskConfig)(int _iLogonID, int _iCommand, int _iChannel, void *_lpInBuffer, int _iInBufferSize);
	DECLARE_FUNCTION(NetClient_SetDevDiskConfig);

	TYPEDEF_FUNCTION(NetClient_GetDevDiskConfig)(int _iLogonID, int _iCommand, int _iChannel, void *_lpOutBuffer, int _iOutBufferSize, int *_lpBytesReturned);
	DECLARE_FUNCTION(NetClient_GetDevDiskConfig);

	TYPEDEF_FUNCTION(NetClient_GetPseChInfo)(int _iLogonID, void *_lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_GetPseChInfo);

	TYPEDEF_FUNCTION(NetClient_SetPseChProperty)(int _iLogonID, int _iPseCh, void *_lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_SetPseChProperty);

	TYPEDEF_FUNCTION(NetClient_GetPseChProperty)(int _iLogonID, int _iPseCh, void *_lpBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_GetPseChProperty);

	TYPEDEF_FUNCTION(NetClient_ChannelTalkStart)(int _iLogonID, int _iChannel, int _iUser);
	DECLARE_FUNCTION(NetClient_ChannelTalkStart);

	TYPEDEF_FUNCTION(NetClient_ChannelTalkEnd)(int _iLogonID, int _iChannel);
	DECLARE_FUNCTION(NetClient_ChannelTalkEnd);

	TYPEDEF_FUNCTION(NetClient_InputChannelTalkingdata)(int _iLogonID, int _iChannel, unsigned char* _ucData, unsigned int _iLen);
	DECLARE_FUNCTION(NetClient_InputChannelTalkingdata);

	TYPEDEF_FUNCTION(NetClient_GetChannelTalkingState)(int _iLogonID, int _iChannel, int* _iTalkState);
	DECLARE_FUNCTION(NetClient_GetChannelTalkingState);

	TYPEDEF_FUNCTION(NetClient_RebootDeviceByType)(int _iLogonID, int _iType, void* _pvCmdBuf, int _iBufLen);
	DECLARE_FUNCTION(NetClient_RebootDeviceByType);

	TYPEDEF_FUNCTION(NetClient_SetSDKWorkMode)(int _iWorkMode);
	DECLARE_FUNCTION(NetClient_SetSDKWorkMode);

	TYPEDEF_FUNCTION(NetClient_StartDownload)(int _iLogonID, int _iChannel, int _iDownloadMode, void *_lpInBuf, int _iInBufLen
		, unsigned long* _pulDownloadFd);
	DECLARE_FUNCTION(NetClient_StartDownload);

	TYPEDEF_FUNCTION(NetClient_StopDownload)(unsigned long _ulDownloadFd);
	DECLARE_FUNCTION(NetClient_StopDownload);

	TYPEDEF_FUNCTION(NetClient_GetDownloadPos)(unsigned long _ulDownloadFd, int* _puiDownloadPos);
	DECLARE_FUNCTION(NetClient_GetDownloadPos);

	TYPEDEF_FUNCTION(NetClient_PlayBack)(unsigned int* _ulConID, int _iCmd, PlayerParam* _PlayerParam, void* _hWnd);
	DECLARE_FUNCTION(NetClient_PlayBack);

	TYPEDEF_FUNCTION(NetClient_StopPlayBack)(unsigned long _ulConID);
	DECLARE_FUNCTION(NetClient_StopPlayBack);

	TYPEDEF_FUNCTION(NetClient_PlayBackControl)(unsigned long _ulConID, int _iControlCode, void* _pcInBuffer, int _iInLen, void* _pcOutBuffer, int *_iOutLen);
	DECLARE_FUNCTION(NetClient_PlayBackControl);

	TYPEDEF_FUNCTION(NetClient_SetDomeAdvParam)( int _iLogonID, int _iChannelNO, int _iCmd, void* _pvCmdBuf, int _iCmdBufLen);
	DECLARE_FUNCTION(NetClient_SetDomeAdvParam);

	TYPEDEF_FUNCTION(NetClient_GetDomeAdvParam)( int _iLogonID, int _iChannelNO, int _iCmd, void* _pvCmdBuf, int _iCmdBufLen);
	DECLARE_FUNCTION(NetClient_GetDomeAdvParam);

	TYPEDEF_FUNCTION(NetClient_GetHDTemplateIndex)(int _iLogonID, int _iChannelNo /*= 0*/, int* _iTemplateIndex);
	DECLARE_FUNCTION(NetClient_GetHDTemplateIndex);

	TYPEDEF_FUNCTION(NetClient_InnerAutoTest)(int _iLogonID, int _iCmd, void* _pvTestBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_InnerAutoTest);

	TYPEDEF_FUNCTION(NetClient_SetDevUserDataNotify)(unsigned int _uiRecvID, DEVUSERDATA_NOTIFY _cbkDevUserData, void* _pvUdata);
	DECLARE_FUNCTION(NetClient_SetDevUserDataNotify);

	TYPEDEF_FUNCTION(NetClient_Query_V4)(int _iLogonID, int _iCmd, int _iChannel, void* _pvCmdBuf, int _iBufLen);
	DECLARE_FUNCTION(NetClient_Query_V4);

	TYPEDEF_FUNCTION(NetClient_GetQueryResult_V4)(int _iLogonID, int _iCmd, int _iChannel, int _iIndex, void* _pvCmdBuf, int _iBufLen);
	DECLARE_FUNCTION(NetClient_GetQueryResult_V4);

	TYPEDEF_FUNCTION(NetClient_CheckCamera)(int _iLogonID, int _iChannelNo, int _iType, int _iEnable);
	DECLARE_FUNCTION(NetClient_CheckCamera);

	TYPEDEF_FUNCTION(NetClient_RegisterDrawFun)(unsigned int _ulConID, CBK_DRAW_FUNC _pfDrawFun, long _lUserData, void *_pCmd, int _iCmdLen);
	DECLARE_FUNCTION(NetClient_RegisterDrawFun);

	TYPEDEF_FUNCTION(NetClient_GetRecvInfoById)(int _iRecvId, void* _pvBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_GetRecvInfoById);

	TYPEDEF_FUNCTION(NetClient_SetDataPackCallBack)(unsigned int _ulConID, int _iCbkType, void* _pvCallBack, void* _pvUserData);
	DECLARE_FUNCTION(NetClient_SetDataPackCallBack);

	TYPEDEF_FUNCTION(NetClient_Query_V5)(int _iLogonId, int _iCmdId, int _iChanNo, void* _lpIn, int _iInLen, void* _lpOut, int _iOutLen);
	DECLARE_FUNCTION(NetClient_Query_V5);

	TYPEDEF_FUNCTION(NetClient_FaceConfig)(int _iLogonId, int _iCmdId, int _iChannelID, void* _lpIn, int _iInLen, void* _lpOut, int _iOutLen);
	DECLARE_FUNCTION(NetClient_FaceConfig);

	TYPEDEF_FUNCTION(NetClient_SetUnipueAlertConfig)(int _iLogonId, int _iCmdId, int _iChannel, void* _lpCmdBuf, int _iCmdBufLen);
	DECLARE_FUNCTION(NetClient_SetUnipueAlertConfig);

	TYPEDEF_FUNCTION(NetClient_GetUnipueAlertConfig)(int _iLogonId, int _iCmdId, int _iChannel, void* _lpCmdBuf, int _iCmdBufLen);
	DECLARE_FUNCTION(NetClient_GetUnipueAlertConfig);

	TYPEDEF_FUNCTION(NetClient_GetLastError)();
	DECLARE_FUNCTION(NetClient_GetLastError);

	TYPEDEF_FUNCTION(NetClient_SyncLogon)(int _iLogonType, void* _pvPara, int _iParaSize);
	DECLARE_FUNCTION(NetClient_SyncLogon);

	TYPEDEF_FUNCTION(NetClient_SyncRealPlay)(unsigned int* _puiRecvID, NetClientPara* _ptPara, int _iParaSize);
	DECLARE_FUNCTION(NetClient_SyncRealPlay);

	TYPEDEF_FUNCTION(NetClient_StopRealPlay)(unsigned int _uiRecvID, int _iParam);
	DECLARE_FUNCTION(NetClient_StopRealPlay);

	TYPEDEF_FUNCTION(NetClient_SyncQuery)(int _iLogonID, int _iChanNo, int _iCmd, void* _pvInPara, int _iInLen, void* _pvOutPara, int _iOutLen);
	DECLARE_FUNCTION(NetClient_SyncQuery);

	TYPEDEF_FUNCTION(NetClient_SyncSetDevCfg)(int _iLogonID, int _iChanNo, int _iCmd, void* _pvInPara, int _iInLen, void* _pvOutRet, int _iOutLen);
	DECLARE_FUNCTION(NetClient_SyncSetDevCfg);

	TYPEDEF_FUNCTION(NetClient_SetZFConfig)(int _iLogonID, int _iCommand, int _iChannel, void *_lpInBuf, int _iInBufSize);
	DECLARE_FUNCTION(NetClient_SetZFConfig);

	TYPEDEF_FUNCTION(NetClient_GetZFConfig)(int _iLogonID, int _iCommand, int _iChannel, void *_lpOutBuf, int _iOutBufSize, int *_lpRetSize);
	DECLARE_FUNCTION(NetClient_GetZFConfig);

	TYPEDEF_FUNCTION(NetClient_SetExternDevLogonInfo)(unsigned int _uiAllowDevType);
	DECLARE_FUNCTION(NetClient_SetExternDevLogonInfo);

	TYPEDEF_FUNCTION(NetClient_SetAVMode)(int _iRecvID, int _iCmd, void* _pvBuf, int _iBufSize);
	DECLARE_FUNCTION(NetClient_SetAVMode);

	TYPEDEF_FUNCTION(NetClient_CapturePicByDevice)(int _iLogonID, int _iChannel, int _iQvalue, char* _pcPicFilePath, SnapPicData* _pBuf,int _iInSize);
	DECLARE_FUNCTION(NetClient_CapturePicByDevice);

	TYPEDEF_FUNCTION(NetClient_CmdConfig)(int _iLogonId, int _iCmdId, int _iChanNo, void* _lpIn, int _iInLen, void* _lpOut, int _iOutLen);
    DECLARE_FUNCTION(NetClient_CmdConfig);
	
	TYPEDEF_FUNCTION(NetClient_GetDevConfig_V5)(int _iLogonID, int _iCmd, void* _pvInParaBuf, int _iInBufLen, void* _pvOutParaBuf, int _iOutBufLen);
	DECLARE_FUNCTION(NetClient_GetDevConfig_V5);

    TYPEDEF_FUNCTION(NetClient_ZFConfig)(int _iLogonId, int _iCmdId, int _iChanNo, void* _lpIn, int _iInLen, void* _lpOut, int _iOutLen);
    DECLARE_FUNCTION(NetClient_ZFConfig);

	TYPEDEF_FUNCTION(NetClient_HttpXmlConfig)(int _iLogonID, XmlCfgInPara* _ptXmlInPara, int _iInSize, XmlCfgOutPara* _ptXmlOutPara, int _iOutSize);
	DECLARE_FUNCTION(NetClient_HttpXmlConfig);
}
#endif

