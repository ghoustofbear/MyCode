#include <stdafx.h>
#include "NVSSDK_INTERFACE.h"
using namespace NVSSDK_INTERFACE;

namespace NVSSDK_INTERFACE
{
	static HINSTANCE  hNVSSDK = NULL;
	pfNetClient_SetPort	NetClient_SetPort	=NULL;
	pfNetClient_SetMSGHandle	NetClient_SetMSGHandle =NULL;
	pfNetClient_Startup	NetClient_Startup	=NULL;
	pfNetClient_Cleanup	NetClient_Cleanup	=NULL;
	pfNetClient_SetNotifyFunction	NetClient_SetNotifyFunction =NULL;
	pfNetClient_Logon		NetClient_Logon		=NULL;
	pfNetClient_GetLogonStatus NetClient_GetLogonStatus =NULL;
	pfNetClient_StartRecv	NetClient_StartRecv =NULL;
	pfNetClient_StartCaptureData	NetClient_StartCaptureData =NULL;
	pfNetClient_StopCaptureData	NetClient_StopCaptureData =NULL;
	pfNetClient_StartRecvNetPicStream NetClient_StartRecvNetPicStream = NULL;
	pfNetClient_StopRecvNetPicStream NetClient_StopRecvNetPicStream  = NULL;
	pfNetClient_GetVideoHeader	NetClient_GetVideoHeader =NULL;
	pfNetClient_StartCaptureFile	NetClient_StartCaptureFile	=NULL;
	pfNetClient_StopCaptureFile	NetClient_StopCaptureFile =NULL;
	pfNetClient_GetCaptureStatus	NetClient_GetCaptureStatus =NULL;
	pfNetClient_StopPlay		NetClient_StopPlay	=NULL;
	pfNetClient_StopPlayEx		NetClient_StopPlayEx = NULL;
	pfNetClient_StartPlay		NetClient_StartPlay	=NULL;
	pfNetClient_ResetPlayerWnd	NetClient_ResetPlayerWnd =NULL;
	pfNetClient_GetPlayingStatus	NetClient_GetPlayingStatus =NULL;
	pfNetClient_SetDecCallBack	NetClient_SetDecCallBack =NULL;
	pfNetClient_SetDecCallBack_V4	NetClient_SetDecCallBack_V4 =NULL;
	pfNetClient_SendStringToServer	NetClient_SendStringToServer =NULL;
	pfNetClient_ReceiveString		NetClient_ReceiveString =NULL;
	pfNetClient_SetLocalAudioVolume		NetClient_SetLocalAudioVolume =NULL;
	pfNetClient_AudioStart		NetClient_AudioStart =NULL;
	pfNetClient_AudioStop			NetClient_AudioStop	=NULL;
	pfNetClient_SetBufferNum		NetClient_SetBufferNum =NULL;
	pfNetClient_SetPlayDelay		NetClient_SetPlayDelay	=NULL;
	pfNetClient_GetDecordFrameNum	NetClient_GetDecordFrameNum =NULL;
	pfNetClient_GetAlarmPortNum	NetClient_GetAlarmPortNum =NULL;
	pfNetClient_SetVideoPara	NetClient_SetVideoPara =NULL;
	pfNetClient_GetVideoPara	NetClient_GetVideoPara =NULL;
	pfNetClient_SetVideoQuality	NetClient_SetVideoQuality =NULL;
	pfNetClient_GetVideoQuality	NetClient_GetVideoQuality =NULL;
	pfNetClient_SetFrameRate	NetClient_SetFrameRate =NULL;
	pfNetClient_GetFrameRate	NetClient_GetFrameRate =NULL;
	pfNetClient_SetStreamType	NetClient_SetStreamType =NULL;
	pfNetClient_GetStreamType	NetClient_GetStreamType =NULL;
	pfNetClient_SetMotionAreaEnable	NetClient_SetSetMotionAreaEnable =NULL;
	pfNetClient_SetMotionDetetionArea	NetClient_SetMotionDetetionArea =NULL;
	pfNetClient_GetMotionDetetionArea	NetClient_GetMotionDetetionArea =NULL;
	pfNetClient_SetThreshold	NetClient_SetThreshold =NULL;
	pfNetClient_GetThreshold	NetClient_GetThreshold =NULL;
	pfNetClient_SetMotionDetection	NetClient_SetMotionDetection =NULL;
	pfNetClient_GetMotionDetection	NetClient_GetMotionDetection =NULL;
	pfNetClient_SetOsdText	NetClient_SetOsdText =NULL;
    pfNetClient_GetOsdText	NetClient_GetOsdText =NULL;
	pfNetClient_SetOsdType	NetClient_SetOsdType =NULL;
	pfNetClient_GetOsdType	NetClient_GetOsdType =NULL;
	pfNetClient_SetOsdLOGO	NetClient_SetOsdLOGO =NULL;
	pfNetClient_SetAudioChannel	NetClient_SetAudioChannel =NULL;
	pfNetClient_GetAudioChannel	NetClient_GetAudioChannel =NULL;
	pfNetClient_SetIpFilter	NetClient_SetIpFilter =NULL;
	pfNetClient_GetIpFilter	NetClient_GetIpFilter =NULL;
	pfNetClient_SetAlarmOutput	NetClient_SetAlarmOutput =NULL;
	pfNetClient_GetAlarmOutput	NetClient_GetAlarmOutput =NULL;
	pfNetClient_SetAlarmInMode	NetClient_SetAlarmInMode =NULL;
	pfNetClient_GetAlarmInMode	NetClient_GetAlarmInMode =NULL;
	pfNetClient_SetAlarmOutMode	NetClient_SetAlarmOutMode =NULL;
	pfNetClient_GetAlarmOutMode	NetClient_GetAlarmOutMode =NULL;
	pfNetClient_SetAlmVdoCovThreshold	NetClient_SetAlmVdoCovThreshold =NULL;
	pfNetClient_GetAlmVdoCovThreshold	NetClient_GetAlmVdoCovThreshold =NULL;
	pfNetClient_SetAlmVideoCov	NetClient_SetAlmVideoCov =NULL;
	pfNetClient_GetAlmVideoCov	NetClient_GetAlmVideoCov =NULL;
	pfNetClient_SetDeviceType	NetClient_SetDeviceType =NULL;
	pfNetClient_GetDeviceType	NetClient_GetDeviceType =NULL;
	pfNetClient_GetAllSupportDeviceType	NetClient_GetAllSupportDeviceType =NULL;
	pfNetClient_SetComFormat	NetClient_SetComFormat =NULL;
	pfNetClient_GetComFormat	NetClient_GetComFormat =NULL;
	pfNetClient_DeviceCtrl	NetClient_DeviceCtrl =NULL;
 	pfNetClient_DeviceCtrlEx	NetClient_DeviceCtrlEx =NULL;
	pfNetClient_ComSend	NetClient_ComSend =NULL;
	pfNetClient_SetComRecvNotify	NetClient_SetComRecvNotify =NULL;
	pfNetClient_SetAlarmVideoLost	NetClient_SetAlarmVideoLost =NULL;
	pfNetClient_GetAlarmVideoLost	NetClient_GetAlarmVideoLost =NULL;
	pfNetClient_SetIFrameRate	NetClient_SetIFrameRate =NULL;
	pfNetClient_GetIFrameRate	NetClient_GetIFrameRate =NULL;
	pfNetClient_ForceIFrame	NetClient_ForceIFrame =NULL;
	pfNetClient_SetTime	NetClient_SetTime =NULL;
	pfNetClient_Reboot	NetClient_Reboot =NULL;
	pfNetClient_DefaultPara	NetClient_DefaultPara =NULL;
	pfNetClient_GetServerVersion	NetClient_GetServerVersion =NULL;
	pfNetClient_UpgradeProgram	NetClient_UpgradeProgram =NULL;
	pfNetClient_UpgradeWebPage	NetClient_UpgradeWebPage =NULL;
	pfNetClient_GetUpgradePercent	NetClient_GetUpgradePercent =NULL;
	pfNetClient_GetUserNum	NetClient_GetUserNum =NULL;
	pfNetClient_GetUserInfo	NetClient_GetUserInfo =NULL;
	pfNetClient_GetCurUserInfo	NetClient_GetCurUserInfo =NULL;
	pfNetClient_AddUser	NetClient_AddUser =NULL;
	pfNetClient_DelUser	NetClient_DelUser =NULL;
	pfNetClient_ModifyPwd	NetClient_ModifyPwd =NULL;
	pfNetClient_ChangeSvrIP	NetClient_ChangeSvrIP =NULL;
	pfNetClient_SetMaxConUser	NetClient_SetMaxConUser =NULL;
	pfNetClient_GetMaxGetUser	NetClient_GetMaxGetUser =NULL;
	pfNetClient_TalkStart	NetClient_TalkStart =NULL;
	pfNetClient_TalkEnd	NetClient_TalkEnd =NULL;
	pfNetClient_CapturePicture NetClient_CapturePicture = NULL;
	pfNetClient_CapturePic	NetClient_CapturePic =NULL;
	pfNetClient_CaptureBmpPic	NetClient_CaptureBmpPic =NULL;
	pfNetClient_SetOutPortState	NetClient_SetOutPortState =NULL;
	pfNetClient_GetIpProperty	NetClient_GetIpProperty =NULL;
	pfNetClient_GetVideoCovArea	NetClient_GetVideoCovArea =NULL;
	pfNetClient_SetVideoCovArea	NetClient_SetVideoCovArea =NULL;
	pfNetClient_GetVideoSizeEx	NetClient_GetVideoSizeEx =NULL;
	pfNetClient_SetVideoSize	NetClient_SetVideoSize =NULL;
	pfNetClient_GetVideoSize	NetClient_GetVideoSize =NULL;
	pfNetClient_SetMaxKByteRate	NetClient_SetMaxKByteRate =NULL;
	pfNetClient_GetMaxKByteRate	NetClient_GetMaxKByteRate =NULL;
	pfNetClient_SetAlarmServer NetClient_SetAlarmServer =NULL;
	pfNetClient_GetAlarmServer NetClient_GetAlarmServer =NULL;
	pfNetClient_SetDHCPParam NetClient_SetDHCPParam =NULL;
	pfNetClient_GetDHCPParam NetClient_GetDHCPParam =NULL;
	pfNetClient_DeviceSetup NetClient_DeviceSetup =NULL;
	pfNetClient_WriteUserData	NetClient_WriteUserData =NULL;
	pfNetClient_ReadUserData	NetClient_ReadUserData =NULL;
	pfNetClient_SetDomainParsePara	NetClient_SetDomainParsePara =NULL;
	pfNetClient_GetDomainParsePara	NetClient_GetDomainParsePara =NULL;
	pfNetClient_SendStringToCenter	NetClient_SendStringToCenter =NULL;
	pfNetClient_DrawRectOnLocalVideo	NetClient_DrawRectOnLocalVideo =NULL;
	pfNetClient_GetBothStreamSame	NetClient_GetBothStreamSame =NULL;
	pfNetClient_DrawTextOnVideo	NetClient_DrawTextOnVideo =NULL;
	pfNetClient_GetTextOnVideo	NetClient_GetTextOnVideo	=	NULL;
	pfNetClient_SetAlarmPortEnable	NetClient_SetAlarmPortEnable =NULL;
	pfNetClient_GetAlarmPortEnable	NetClient_GetAlarmPortEnable =NULL;
	pfNetClient_ShowBitrateOnVideo NetClient_ShowBitrateOnVideo =NULL;
	pfNetClient_SetPPPoEInfo NetClient_SetPPPoEInfo =NULL;
	pfNetClient_GetPPPoEInfo NetClient_GetPPPoEInfo =NULL;
	pfNetClient_CPUCheckEnabled NetClient_CPUCheckEnabled =NULL;
	pfNetClient_GetBitrateOnVideo NetClient_GetBitrateOnVideo =NULL;
	pfNetClient_GetMaxVideoSize NetClient_GetMaxVideoSize =NULL;
	pfNetClient_SetEncodeMode NetClient_SetEncodeMode =NULL;
	pfNetClient_GetEncodeMode NetClient_GetEncodeMode =NULL;
	pfNetClient_SetPreferMode NetClient_SetPreferMode =NULL;
	pfNetClient_GetPreferMode NetClient_GetPreferMode =NULL;
	pfNetClient_NetFileQuery NetClient_NetFileQuery =NULL;
	pfNetClient_NetFileGetFileCount NetClient_NetFileGetFileCount =NULL;
	pfNetClient_NetFileGetQueryfile NetClient_NetFileGetQueryfile =NULL;
	pfNetClient_NetFileManualRecord NetClient_NetFileManualRecord =NULL;
	pfNetClient_NetFileMapStoreDevice NetClient_NetFileMapStoreDevice;
	pfNetClient_NetFileGetMapStoreDevice NetClient_NetFileGetMapStoreDevice =NULL;
	pfNetClient_NetFileMountUSB NetClient_NetFileMountUSB =NULL;
	pfNetClient_NetFileGetUSBstate NetClient_NetFileGetUSBstate =NULL;
	pfNetClient_NetFileSetExtendname NetClient_NetFileSetExtendname =NULL;
	pfNetClient_NetFileGetExtendname NetClient_NetFileGetExtendname =NULL;
	pfNetClient_NetFileSetRecordRule NetClient_NetFileSetRecordRule =NULL;
	pfNetClient_NetFileGetRecordRule NetClient_NetFileGetRecordRule =NULL;
	pfNetClient_NetFileSetAlarmRule NetClient_NetFileSetAlarmRule =NULL;
	pfNetClient_NetFileGetAlarmRule NetClient_NetFileGetAlarmRule =NULL;
	pfNetClient_NetFileSetAlarmState NetClient_NetFileSetAlarmState =NULL;
	pfNetClient_NetFileGetAlarmState NetClient_NetFileGetAlarmState =NULL;
	pfNetClient_NetFileSetTaskState NetClient_NetFileSetTaskState =NULL;
	pfNetClient_NetFileGetTaskState NetClient_NetFileGetTaskState =NULL;
	pfNetClient_NetFileSetTaskSchedule NetClient_NetFileSetTaskSchedule =NULL;
	pfNetClient_NetFileGetTaskSchedule NetClient_NetFileGetTaskSchedule =NULL;
	pfNetClient_NetFileRebuildIndexFile NetClient_NetFileRebuildIndexFile =NULL;
	pfNetClient_NetFileGetDiskInfo NetClient_NetFileGetDiskInfo =NULL;
	pfNetClient_NetFileIsSupportStorage NetClient_NetFileIsSupportStorage =NULL;
	pfNetClient_NetFileDownloadFile NetClient_NetFileDownloadFile =NULL;
	pfNetClient_NetFileGetRecordState NetClient_NetFileGetRecordState =NULL;
	pfNetClient_SetAudioEncoder NetClient_SetAudioEncoder =NULL;
	pfNetClient_GetAudioEncoder NetClient_GetAudioEncoder =NULL;
	pfNetClient_SetVideoNPMode NetClient_SetVideoNPMode=0;
	pfNetClient_GetVideoNPMode NetClient_GetVideoNPMode=0;
	pfNetClient_SetDeviceTimerReboot NetClient_SetDeviceTimerReboot=0;
	pfNetClient_GetDeviceTimerReboot NetClient_GetDeviceTimerReboot=0;
	pfNetClient_SetManagerServersInfo NetClient_SetManagerServersInfo=0;
	pfNetClient_GetManagerServersInfo NetClient_GetManagerServersInfo =NULL;
	pfNetClient_SetDeviceID NetClient_SetDeviceID =NULL;
	pfNetClient_GetDeviceID NetClient_GetDeviceID =NULL;
	pfNetClient_SetMediaStreamClient NetClient_SetMediaStreamClient =NULL;
	pfNetClient_GetMediaStreamClient NetClient_GetMediaStreamClient =NULL;
	pfNetClient_GetAlarmIPortState NetClient_GetAlarmIPortState =NULL;
	pfNetClient_GetAlarmVLostState NetClient_GetAlarmVLostState =NULL;
	pfNetClient_GetAlarmVCoverState NetClient_GetAlarmVCoverState =NULL;
	pfNetClient_SetDateFormat NetClient_SetDateFormat =NULL;
	pfNetClient_GetDateFormat NetClient_GetDateFormat =NULL;
	pfNetClient_SetRawFrameCallBack NetClient_SetRawFrameCallBack =NULL;
	pfNetClient_SetRawFrameCallBackEx NetClient_SetRawFrameCallBackEx =NULL;
	pfNetClient_SetHTTPPort NetClient_SetHTTPPort =NULL;
	pfNetClient_GetHTTPPort NetClient_GetHTTPPort =NULL;
	pfNetClient_IYUVtoYV12 NetClient_IYUVtoYV12 =NULL;
	pfNetClient_SetEmailAlarm NetClient_SetEmailAlarm =NULL;
	pfNetClient_GetEmailAlarm NetClient_GetEmailAlarm =NULL;
	pfNetClient_SetEmailAlarmEnable NetClient_SetEmailAlarmEnable =NULL;
	pfNetClient_GetEmailAlarmEnable NetClient_GetEmailAlarmEnable =NULL;
	pfNetClient_SetScene NetClient_SetScene =NULL;
	pfNetClient_GetScene NetClient_GetScene =NULL;
	pfNetClient_Snapshot NetClient_Snapshot =NULL;
	pfNetClient_GetFactoryID NetClient_GetFactoryID =NULL;
	pfNetClient_SetSensorFlip NetClient_SetSensorFlip =NULL;
	pfNetClient_GetSensorFlip NetClient_GetSensorFlip =NULL;
	pfNetClient_SetSensorMirror NetClient_SetSensorMirror =NULL;
	pfNetClient_GetSensorMirror NetClient_GetSensorMirror =NULL;
	pfNetClient_SetWifiParam NetClient_SetWifiParam =NULL;
	pfNetClient_GetWifiParam NetClient_GetWifiParam =NULL;
	pfNetClient_WifiSearch NetClient_WifiSearch =NULL;
	pfNetClient_GetWifiSearchResult NetClient_GetWifiSearchResult =NULL;
	pfNetClient_SetPrivacyProtect NetClient_SetPrivacyProtect =NULL;
	pfNetClient_GetPrivacyProtect NetClient_GetPrivacyProtect =NULL;
	pfNetClient_InputTalkingdata NetClient_InputTalkingdata =NULL;
	pfNetClient_SetNTPInfo NetClient_SetNTPInfo =NULL;
	pfNetClient_GetNTPInfo NetClient_GetNTPInfo =NULL;
	pfNetClient_SetCPUMEMAlarmThreshold NetClient_SetCPUMEMAlarmThreshold =NULL;
	pfNetClient_GetCPUMEMAlarmThreshold NetClient_GetCPUMEMAlarmThreshol =NULL;
	pfNetClient_SetOSDAlpha NetClient_SetOSDAlpha =NULL;
	pfNetClient_GetOSDAlpha NetClient_GetOSDAlpha =NULL;
	pfNetClient_GetDevType NetClient_GetDevType =NULL;

	pfNetClient_SetOsdDiaphaneity NetClient_SetOsdDiaphaneity =NULL;
	pfNetClient_GetOsdDiaphaneity NetClient_GetOsdDiaphaneity =NULL;
	pfNetClient_Set3DEnable NetClient_Set3DEnable =NULL;
	pfNetClient_Get3DEnable NetClient_Get3DEnable =NULL;
	pfNetClient_NetFileDelFile NetClient_NetFileDelFile =NULL;
	pfNetClient_DiskSetUsage NetClient_DiskSetUsage =NULL;
	pfNetClient_DiskFormat NetClient_DiskFormat =NULL;
	pfNetClient_DiskPart NetClient_DiskPart =NULL;
	pfNetClient_BackupKernel NetClient_BackupKernel =NULL;

	pfNetClient_ProxyAddProxy	NetClient_ProxyAddProxy	=	NULL;
	pfNetClient_ProxyStartup	NetClient_ProxyStartup	=	NULL;
	pfNetClient_ProxyHasConnected	NetClient_ProxyHasConnected	=	NULL;
	pfNetClient_ProxyGetAllLinks	NetClient_ProxyGetAllLinks	=	NULL;
	pfNetClient_ProxyGetLinkInfo	NetClient_ProxyGetLinkInfo	=	NULL;
	pfNetClient_ProxyDeleteLink	NetClient_ProxyDeleteLink	=	NULL;
	 pfNetClient_ProxyDeleteProxy NetClient_ProxyDeleteProxy	=	NULL;
	pfNetClient_ProxySendStringToClient	NetClient_ProxySendStringToClient	=	NULL;
	 pfNetClient_ProxySendStringToClientByID NetClient_ProxySendStringToClientByID	=	NULL;
	 pfNetClient_ProxySetMaxInOutVideo NetClient_ProxySetMaxInOutVideo	=	NULL;
	 pfNetClient_ProxyCloseAllLinks NetClient_ProxyCloseAllLinks	=	NULL;

	pfNetClient_GetChannelNum	NetClient_GetChannelNum	=	NULL;
	pfNetClient_StopRecv	NetClient_StopRecv	=	NULL;
	pfNetClient_GetRecvID	NetClient_GetRecvID	=	NULL;
	pfNetClient_Logoff	NetClient_Logoff	=	NULL;
	pfNetClient_DCardInit NetClient_DCardInit	=	NULL;
	pfNetClient_DCardSetShowMode NetClient_DCardSetShowMode	=	NULL;
	 pfNetClient_DCardGetShowMode NetClient_DCardGetShowMode	=	NULL;
	 pfNetClient_DCardStartPlay  NetClient_DCardStartPlay	=	NULL;
	 pfNetClient_DCardStopPlay NetClient_DCardStopPlay	=	NULL;
	pfNetClient_DCardRelease NetClient_DCardRelease	=	NULL;
	pfNetClient_DCardReInit NetClient_DCardReInit	=	NULL;
	pfNetClient_DCardGetState NetClient_DCardGetState	=	NULL;
	 pfNetClient_DCardStartPlayEx NetClient_pfNetClient_DCardStartPlayEx	=	NULL;
	 pfNetClient_DCardPutDataEx NetClient_pfNetClient_DCardPutDataEx	=	NULL;
	 pfNetClient_DCardStopPlayEx NetClient_pfNetClient_DCardStopPlayEx	=	NULL;
	 pfNetClient_DCardStartPlayAudio NetClient_DCardStartPlayAudio	=	NULL;
	 pfNetClient_DCardStopPlayAudio NetClient_DCardStopPlayAudio	=	NULL;
	//-------------------------------------------------------------------------

	 pfNetClient_LogFileSetProperty NetClient_LogFileSetProperty	=	NULL;
	 pfNetClient_LogFileGetProperty NetClient_LogFileGetProperty	=	NULL;
	 pfNetClient_LogFileDownload NetClient_LogFileDownload	=	NULL;
	 pfNetClient_LogFileClear NetClient_LogFileClear	=	NULL;
	 pfNetClient_LogFileGetDetails NetClient_LogFileGetDetails	=	NULL;
	pfNetClient_NetFileStopDownloadFile NetClient_NetFileStopDownloadFile	=	NULL;
	pfNetClient_NetFileGetDownloadPos NetClient_NetFileGetDownloadPos	=	NULL;
	pfNetClient_SetReducenoiseState NetClient_SetReducenoiseState	=	NULL;
	pfNetClient_GetReducenoiseState NetClient_GetReducenoiseState	=	NULL;
	pfNetClient_GetVersion NetClient_GetVersion	=	NULL;
	pfNetClient_Bindinterface	NetClient_Bindinterface	=	NULL;
	 pfNetClient_GetNetinterface NetClient_GetNetinterface	=	NULL;
	 //pfNetClient_DSMSetDirectoryServer NetClient_DSMSetDirectoryServer	=	NULL;
	 pfNetClient_ProxyGetEncoderList NetClient_ProxyGetEncoderList	=	NULL;
	 pfNetClient_ProxyGetEncoderInfo NetClient_ProxyGetEncoderInfo	=	NULL;
	 pfNetClient_SetNVS NetClient_SetNVS	=	NULL;
	pfNetClient_SetMotionAreaEnable	NetClient_SetMotionAreaEnable	=	NULL;
	pfNetClient_BindInterface	NetClient_BindInterface	=	NULL;
	 pfNetClient_GetNetInterface NetClient_GetNetInterface	=	NULL;
    pfNetClient_NetFileQueryEx NetClient_NetFileQueryEx = NULL;
    pfNetClient_ControlDeviceRecord NetClient_ControlDeviceRecord = NULL;

	pfNetClient_SetUPNPEnable			NetClient_SetUPNPEnable		=	NULL;
	pfNetClient_GetUPNPEnable			NetClient_GetUPNPEnable		=	NULL;
	pfNetClient_GetSysInfo			NetClient_GetSysInfo		=	NULL;
	pfNetClient_SetWifiDHCPParam		NetClient_SetWifiDHCPParam	=	NULL;
	pfNetClient_GetWifiDHCPParam		NetClient_GetWifiDHCPParam	=	NULL;
	pfNetClient_SetDDNSPara			NetClient_SetDDNSPara		=	NULL;
	pfNetClient_GetDDNSPara			NetClient_GetDDNSPara		=	NULL;
	pfNetClient_GetProductType		NetClient_GetProductType	=	NULL;
	pfNetClient_GetProductTypeEx		NetClient_GetProductTypeEx	=	NULL;
	pfNetClient_SetVideoparaSchedule	NetClient_SetVideoparaSchedule	=	NULL;
	pfNetClient_GetVideoparaSchedule	NetClient_GetVideoparaSchedule	=	NULL;
	pfNetClient_SetMotionDetectSchedule	NetClient_SetMotionDetectSchedule	=	NULL;
	pfNetClient_GetMotionDetectSchedule	NetClient_GetMotionDetectSchedule	=	NULL;
	pfNetClient_SetOutportAlarmDelay	NetClient_SetOutportAlarmDelay	=	NULL;
	pfNetClient_GetOutportAlarmDelay	NetClient_GetOutportAlarmDelay	=	NULL;
	pfNetClient_SetInportAlarmSchedule	NetClient_SetInportAlarmSchedule	=	NULL;
	pfNetClient_GetInportAlarmSchedule	NetClient_GetInportAlarmSchedule	=	NULL;
	pfNetClient_SetVideoLostSchedule	NetClient_SetVideoLostSchedule	=	NULL;
	pfNetClient_GetVideoLostSchedule	NetClient_GetVideoLostSchedule	=	NULL;
	pfNetClient_SetMotionDecLinkRec	NetClient_SetMotionDecLinkRec	=	NULL;
	pfNetClient_GetMotionDecLinkRec	NetClient_GetMotionDecLinkRec	=	NULL;
	pfNetClient_SetVideoLostLinkRec	NetClient_SetVideoLostLinkRec	=	NULL;
	pfNetClient_GetVideoLostLinkRec	NetClient_GetVideoLostLinkRec	=	NULL;
	pfNetClient_SetInportAlarmLinkRec	NetClient_SetInportAlarmLinkRec	=	NULL;
	pfNetClient_GetInportAlarmLinkRec	NetClient_GetInportAlarmLinkRec	=	NULL;
	pfNetClient_SetOutportAlarmSchedule	NetClient_SetOutportAlarmSchedule	=	NULL;
	pfNetClient_GetOutportAlarmSchedule	NetClient_GetOutportAlarmSchedule	=	NULL;
	pfNetClient_SetMotionDecLinkOutport	NetClient_SetMotionDecLinkOutport	=	NULL;
	pfNetClient_GetMotionDecLinkOutport	NetClient_GetMotionDecLinkOutport	=	NULL;
	pfNetClient_SetInportAlmLinkOutport	NetClient_SetInportAlmLinkOutport	=	NULL;
	pfNetClient_GetInportAlmLinkOutport	NetClient_GetInportAlmLinkOutport	=	NULL;
	pfNetClient_SetVideoLostLinkOutport	NetClient_SetVideoLostLinkOutport	=	NULL;
	pfNetClient_GetVideoLostLinkOutport	NetClient_GetVideoLostLinkOutport	=	NULL;
	pfNetClient_SetMotionDecLinkSnap	NetClient_SetMotionDecLinkSnap	=	NULL;
	pfNetClient_GetMotionDecLinkSnap	NetClient_GetMotionDecLinkSnap	=	NULL;
	pfNetClient_SetInportAlarmLinkSnap	NetClient_SetInportAlarmLinkSnap	=	NULL;
	pfNetClient_GetInportAlarmLinkSnap	NetClient_GetInportAlarmLinkSnap	=	NULL;
	pfNetClient_SetVideoLostLinkSnap	NetClient_SetVideoLostLinkSnap	=	NULL;
	pfNetClient_GetVideoLostLinkSnap	NetClient_GetVideoLostLinkSnap	=	NULL;
	pfNetClient_SetInportAlarmLinkPTZ	NetClient_SetInportAlarmLinkPTZ	=	NULL;
	pfNetClient_GetInportAlarmLinkPTZ	NetClient_GetInportAlarmLinkPTZ	=	NULL;
	pfNetClient_SetVideoLostLinkPTZ	NetClient_SetVideoLostLinkPTZ	=	NULL;
	pfNetClient_GetVideoLostLinkPTZ	NetClient_GetVideoLostLinkPTZ	=	NULL;
	pfNetClient_SetInportEnable	NetClient_SetInportEnable			=	NULL;
	pfNetClient_GetInportEnable	NetClient_GetInportEnable			=	NULL;
	pfNetClient_SetOutportEnable	NetClient_SetOutportEnable			=	NULL;


	pfNetClient_GetOutportEnable	NetClient_GetOutportEnable			=	NULL;

	pfNetClient_SetZTEInfo		NetClient_SetZTEInfo				=	NULL;
	pfNetClient_GetZTEInfo		NetClient_GetZTEInfo				=	NULL;
	pfNetClient_SetOnlineState	NetClient_SetOnlineState			=	NULL;
	pfNetClient_GetOnlineState	NetClient_GetOnlineState			=	NULL;

	pfNetClient_SetDZInfo			NetClient_SetDZInfo					=	NULL;
	pfNetClient_GetDZInfo			NetClient_GetDZInfo					=	NULL;

	pfNetClient_ClearDisk						NetClient_ClearDisk					=	NULL;
	pfNetClient_GetMaxMinorVideoSize			NetClient_GetMaxMinorVideoSize		=	NULL;

	pfNetClient_SetComServer		NetClient_SetComServer=	NULL;
	pfNetClient_GetComServer		NetClient_GetComServer=	NULL;

	pfNetClient_SetPTZAutoBack		NetClient_SetPTZAutoBack=	NULL;
	pfNetClient_GetPTZAutoBack		NetClient_GetPTZAutoBack=	NULL;


    pfNetClient_SetFuncListArray NetClient_SetFuncListArray = NULL;
    pfNetClient_GetFuncListArray NetClient_GetFuncListArray = NULL;
    pfNetClient_GetOutportState NetClient_GetOutportState  = NULL;
    pfNetClient_GetCPUMEMAlarmThreshold NetClient_GetCPUMEMAlarmThreshold = NULL;
    pfNetClient_SetBothStreamSame NetClient_SetBothStreamSame = NULL;
    pfNetClient_GetInportAlarmLinkSoundDisplay NetClient_GetInportAlarmLinkSoundDisplay = NULL;
    pfNetClient_GetVideoLostLinkSoundDisplay NetClient_GetVideoLostLinkSoundDisplay = NULL;
    pfNetClient_GetMotionDecLinkSoundDisplay NetClient_GetMotionDecLinkSoundDisplay = NULL;
    pfNetClient_SetInportAlarmLinkSoundDisplay NetClient_SetInportAlarmLinkSoundDisplay = NULL;
    pfNetClient_SetMotionDecLinkSoundDisplay NetClient_SetMotionDecLinkSoundDisplay = NULL;
    pfNetClient_SetVideoLostLinkSoundDisplay  NetClient_SetVideoLostLinkSoundDisplay = NULL;

    pfNetClient_GetVencType     NetClient_GetVencType = NULL;
    pfNetClient_SetVencType     NetClient_SetVencType = NULL;

    pfNetClient_Get3GDeviceStatus    NetClient_Get3GDeviceStatus = NULL;
    pfNetClient_Set3GDialog          NetClient_Set3GDialog       = NULL;
    pfNetClient_Get3GDialog          NetClient_Get3GDialog       = NULL;
    pfNetClient_Set3GMessage         NetClient_Set3GMessage      = NULL;
    pfNetClient_Get3GMessage         NetClient_Get3GMessage      = NULL;
    pfNetClient_Set3GTaskSchedule    NetClient_Set3GTaskSchedule = NULL;
    pfNetClient_Get3GTaskSchedule    NetClient_Get3GTaskSchedule = NULL;
    pfNetClient_Set3GNotify          NetClient_Set3GNotify       = NULL;
    pfNetClient_Get3GNotify          NetClient_Get3GNotify       = NULL;
    pfNetClient_Set3GVPND            NetClient_Set3GVPND       = NULL;
    pfNetClient_Get3GVPND            NetClient_Get3GVPND       = NULL;

    pfNetClient_SetHDCamer           NetClient_SetHDCamer      = NULL;
    pfNetClient_GetHDCamer           NetClient_GetHDCamer      = NULL;

    pfNetClient_GetComPortCounts     NetClient_GetComPortCounts = NULL; 
    pfNetClient_GetDownloadFailedFileName  NetClient_GetDownloadFailedFileName = NULL;

    pfNetClient_SetHXListenPortInfo           NetClient_SetHXListenPortInfo      = NULL;
    pfNetClient_GetHXListenPortInfo           NetClient_GetHXListenPortInfo      = NULL;

    pfNetClient_AdjustVideo     NetClient_AdjustVideo = NULL;

    pfNetClient_ProxyAddProxyEx NetClient_ProxyAddProxyEx = NULL;

    pfNetClient_SetPlayerShowFrameMode NetClient_SetPlayerShowFrameMode = NULL;
    pfNetClient_GetPlayerShowFrameMode NetClient_GetPlayerShowFrameMode = NULL;


    pfNetClient_SetColorToGray NetClient_SetColorToGray = NULL;
    pfNetClient_GetColorToGray NetClient_GetColorToGray = NULL;
    pfNetClient_SetColorParam NetClient_SetColorParam = NULL;
    pfNetClient_GetColorParam NetClient_GetColorParam = NULL;

    pfNetClient_SetCustomChannelName NetClient_SetCustomChannelName = NULL;
    pfNetClient_GetCustomChannelName NetClient_GetCustomChannelName = NULL;
    pfNetClient_SetCustomRecType NetClient_SetCustomRecType = NULL;
    pfNetClient_GetCustomRecType NetClient_GetCustomRecType = NULL;
    pfNetClient_ChangeSvrIPEx NetClient_ChangeSvrIPEx = NULL;
    pfNetClient_GetIpPropertyEx NetClient_GetIpPropertyEx = NULL;
    pfNetClient_SetFTPUpdate NetClient_SetFTPUpdate = NULL;
    pfNetClient_GetFTPUpdate NetClient_GetFTPUpdate = NULL;
    pfNetClient_SetCHNPTZFormat NetClient_SetCHNPTZFormat = NULL;
    pfNetClient_GetCHNPTZFormat NetClient_GetCHNPTZFormat = NULL;
    pfNetClient_SetOSDTypeColor NetClient_SetOSDTypeColor = NULL;
    pfNetClient_GetOSDTypeColor NetClient_GetOSDTypeColor = NULL;
    pfNetClient_GetServerVersionEx NetClient_GetServerVersionEx = NULL;
    pfNetClient_GetExceptionMsg NetClient_GetExceptionMsg = NULL;

    pfNetClient_GetTalkingState NetClient_GetTalkingState               = NULL;
    //about multi monitor
    pfNetClient_GetMonitorNum   NetClient_GetMonitorNum               = NULL;
    //pfNetClient_GetMonitorInfo  NetClient_GetMonitorInfo              = NULL;//4.0todo
    pfNetClient_ChangeMonitor   NetClient_ChangeMonitor               = NULL;
    //pfNetClient_UpdateMonitor   NetClient_UpdateMonitor               = NULL;
    pfNetClient_GetInfoByConnectID  NetClient_GetInfoByConnectID      = NULL;
    pfNetClient_InterTalkStart      NetClient_InterTalkStart        = NULL;
    pfNetClient_InterTalkEnd    NetClient_InterTalkEnd = NULL;

    pfNetClient_NetFileDownloadByTimeSpan NetClient_NetFileDownloadByTimeSpan = NULL;
    pfNetClient_NetFileDownloadByTimeSpanEx  NetClient_NetFileDownloadByTimeSpanEx = NULL;
    pfNetClient_SetNetFileDownloadFileCallBack NetClient_SetNetFileDownloadFileCallBack = NULL;

    pfNetClient_NetLogQuery NetClient_NetLogQuery = NULL;
    pfNetClient_NetLogGetLogfile NetClient_NetLogGetLogfile = NULL;
    pfNetClient_NetLogGetLogCount NetClient_NetLogGetLogCount = NULL;

    pfNetClient_GetProtocolList NetClient_GetProtocolList = NULL;

    pfNetClient_SetCHNPTZCRUISE NetClient_SetCHNPTZCRUISE = NULL;
    pfNetClient_GetCHNPTZCRUISE NetClient_GetCHNPTZCRUISE = NULL;
    pfNetClient_SetVIDEOCOVER_LINKRECORD NetClient_SetVIDEOCOVER_LINKRECORD = NULL;
    pfNetClient_GetVIDEOCOVER_LINKRECORD NetClient_GetVIDEOCOVER_LINKRECORD = NULL;
    pfNetClient_SetVIDEOCOVER_LINKPTZ NetClient_SetVIDEOCOVER_LINKPTZ = NULL;
    pfNetClient_GetVIDEOCOVER_LINKPTZ NetClient_GetVIDEOCOVER_LINKPTZ = NULL;

    pfNetClient_SetVideoCoverSchedule NetClient_SetVideoCoverSchedule = NULL;
    pfNetClient_GetVideoCoverSchedule NetClient_GetVideoCoverSchedule = NULL;

    pfNetClient_EZoomAdd NetClient_EZoomAdd = NULL;
    pfNetClient_EZoomSet NetClient_EZoomSet = NULL;
    pfNetClient_EZoomRemove NetClient_EZoomRemove = NULL;

    pNetClient_SetFTPUsage NetClient_SetFTPUsage = NULL;
    pNetClient_GetFTPUsage NetClient_GetFTPUsage = NULL;

    pfNetClient_DrawPolyOnLocalVideo NetClient_DrawPolyOnLocalVideo = NULL;
    pfNetClient_GetVideoParam NetClient_GetVideoParam = NULL;

    pfNetClient_VCASetConfig    NetClient_VCASetConfig = NULL;
    pfNetClient_VCAGetConfig    NetClient_VCAGetConfig = NULL;
    pfNetClient_VCARestart  NetClient_VCARestart = NULL;
    pfNetClient_VCAGetAlarmInfo NetClient_VCAGetAlarmInfo = NULL;

    pfNetClient_SetEmailAlarmEx NetClient_SetEmailAlarmEx = NULL;
    pfNetClient_GetEmailAlarmEx NetClient_GetEmailAlarmEx = NULL;

    pfNetClient_SetFTPUploadConfig NetClient_SetFTPUploadConfig = NULL;
    pfNetClient_GetFTPUploadConfig NetClient_GetFTPUploadConfig = NULL;

    pfNetClient_GetHDCamerEx NetClient_GetHDCamerEx = NULL;
    pfNetClient_SetHDCamerEx NetClient_SetHDCamerEx = NULL;

    pfNetClient_Get3GConfig NetClient_Get3GConfig = NULL;
    pfNetClient_Set3GConfig NetClient_Set3GConfig = NULL;

    pfNetClient_SetCDBurnConfig NetClient_SetCDBurnConfig = NULL;
    pfNetClient_GetCDBurnConfig NetClient_GetCDBurnConfig = NULL;
    pfNetClient_SetVideoEncrypt NetClient_SetVideoEncrypt = NULL;
    pfNetClient_GetVideoEncrypt NetClient_GetVideoEncrypt = NULL;

    pfNetClient_SetVideoDecrypt NetClient_SetVideoDecrypt = NULL;
    pfNetClient_GetVideoDecrypt NetClient_GetVideoDecrypt = NULL;

    pfNetClient_SetPreRecEnable NetClient_SetPreRecEnable = NULL;
    pfNetClient_GetPreRecEnable NetClient_GetPreRecEnable = NULL;

    pfNetClient_SetITSExtraInfo NetClient_SetITSExtraInfo = NULL;
    pfNetClient_GetITSExtraInfo NetClient_GetITSExtraInfo = NULL;

	pfNetClient_SetATMConfig NetClient_SetATMConfig = NULL;//ATM
	pfNetClient_GetATMConfig NetClient_GetATMConfig = NULL;//ATM

	//add by zhy 2013.04.06&2013.04.08&2013.04.11
	pfNetClient_SetDiskGroup NetClient_SetDiskGroup = NULL;
	pfNetClient_GetDiskGroup NetClient_GetDiskGroup = NULL;
	pfNetClient_SetDiskQuota NetClient_SetDiskQuota = NULL;
	pfNetClient_GetDiskQuotaState NetClient_GetDiskQuotaState = NULL;
	pfNetClient_ModifyUserAuthority NetClient_ModifyUserAuthority = NULL;
	pfNetClient_GetUserAuthority NetClient_GetUserAuthority = NULL;
	pfNetClient_GetGroupAuthority NetClient_GetGroupAuthority = NULL;
	pfNetClient_SetHolidayPlan NetClient_SetHolidayPlan = NULL;
	pfNetClient_GetHolidayPlan NetClient_GetHolidayPlan = NULL;
	pfNetClient_SetCommonEnable NetClient_SetCommonEnable = NULL;
	pfNetClient_GetCommonEnable NetClient_GetCommonEnable = NULL;
	//add by zhy end

    IMPLEMENT_FUNCTION(NetClient_SetDigitalChannelConfig);
    IMPLEMENT_FUNCTION(NetClient_GetDigitalChannelConfig);
    IMPLEMENT_FUNCTION(NetClient_DigitalChannelSend);
    IMPLEMENT_FUNCTION(NetClient_SetVideoNPModeEx);
    IMPLEMENT_FUNCTION(NetClient_GetVideoNPModeEx);
    IMPLEMENT_FUNCTION(NetClient_SetVideoModeMethod);  //cb@120517 //4.0todo
    IMPLEMENT_FUNCTION(NetClient_GetVideoModeMethod);
    IMPLEMENT_FUNCTION(NetClient_GetChannelSipConfig);
    IMPLEMENT_FUNCTION(NetClient_SetChannelSipConfig);
    //IMPLEMENT_FUNCTION(NetClient_SetPsPackCallBack);//4.0todo
    IMPLEMENT_FUNCTION(NetClient_StartRecvEx);
    IMPLEMENT_FUNCTION(NetClient_SetVideoCombine);
    IMPLEMENT_FUNCTION(NetClient_GetVideoCombine);
    IMPLEMENT_FUNCTION(NetClient_GetConnectInfo);
    IMPLEMENT_FUNCTION(NetClient_SetPlatformApp);
    IMPLEMENT_FUNCTION(NetClient_GetPlatformApp);
    IMPLEMENT_FUNCTION(NetClient_GetDigitalChannelNum);
    IMPLEMENT_FUNCTION(NetClient_SetITSTimeRange);
    IMPLEMENT_FUNCTION(NetClient_GetITSTimeRange);
    /*IMPLEMENT_FUNCTION(NetClient_SetITSTimeRangeParam);
    IMPLEMENT_FUNCTION(NetClient_GetITSTimeRangeParam);
    IMPLEMENT_FUNCTION(NetClient_SetITSAgcBlockEnable);
    IMPLEMENT_FUNCTION(NetClient_GetITSAgcBlockEnable);
    IMPLEMENT_FUNCTION(NetClient_SetITSFlashLampEnable);
    IMPLEMENT_FUNCTION(NetClient_GetITSFlashLampEnable);  */
    IMPLEMENT_FUNCTION(NetClient_SetITSDetectMode);
    IMPLEMENT_FUNCTION(NetClient_GetITSDetectMode);
    IMPLEMENT_FUNCTION(NetClient_SetITSBlock);
    IMPLEMENT_FUNCTION(NetClient_GetITSBlock);
    IMPLEMENT_FUNCTION(NetClient_SetITSLoopMode);
    IMPLEMENT_FUNCTION(NetClient_GetITSLoopMode);
    IMPLEMENT_FUNCTION(NetClient_SetITSDeviceType);
    IMPLEMENT_FUNCTION(NetClient_GetITSDeviceType);
    IMPLEMENT_FUNCTION(NetClient_SetITSSwitchTime);
    IMPLEMENT_FUNCTION(NetClient_GetITSSwitchTime);
    IMPLEMENT_FUNCTION(NetClient_SetJPEGQuality);
    IMPLEMENT_FUNCTION(NetClient_GetJPEGQuality);
    IMPLEMENT_FUNCTION(NetClient_SetOSDTypeFontSize);
    IMPLEMENT_FUNCTION(NetClient_GetOSDTypeFontSize);
    IMPLEMENT_FUNCTION(NetClient_VCARestartEx);
	IMPLEMENT_FUNCTION(NetClient_SetChannelProperty);
    IMPLEMENT_FUNCTION(NetClient_GetChannelProperty);
    IMPLEMENT_FUNCTION(NetClient_SetITSRoadwayParam);
    IMPLEMENT_FUNCTION(NetClient_GetITSRoadwayParam);
    IMPLEMENT_FUNCTION(NetClient_SetITSLicensePlateOptimize);
    IMPLEMENT_FUNCTION(NetClient_GetITSLicensePlateOptimize);
    IMPLEMENT_FUNCTION(NetClient_SetHDTimeRangeParam);
    IMPLEMENT_FUNCTION(NetClient_GetHDTimeRangeParam);
    IMPLEMENT_FUNCTION(NetClient_SetITSTimeRangeEnable);
    IMPLEMENT_FUNCTION(NetClient_GetITSTimeRangeEnable);
    IMPLEMENT_FUNCTION(NetClient_SetHDTemplateName);
    IMPLEMENT_FUNCTION(NetClient_GetHDTemplateName);
    IMPLEMENT_FUNCTION(NetClient_SetHDTemplateMap);
    IMPLEMENT_FUNCTION(NetClient_GetHDTemplateMap);
    IMPLEMENT_FUNCTION(NetClient_SetITSRecoParam);
    IMPLEMENT_FUNCTION(NetClient_GetITSRecoParam);
    IMPLEMENT_FUNCTION(NetClient_SetITSDayNight);
    IMPLEMENT_FUNCTION(NetClient_GetITSDayNight);
    IMPLEMENT_FUNCTION(NetClient_SetITSCamLocation);
    IMPLEMENT_FUNCTION(NetClient_GetITSCamLocation);
    IMPLEMENT_FUNCTION(NetClient_SetITSWorkMode);
    IMPLEMENT_FUNCTION(NetClient_GetITSWorkMode);
    IMPLEMENT_FUNCTION(NetClient_LogonEx);
    IMPLEMENT_FUNCTION(NetClient_SetTimeZone);
    IMPLEMENT_FUNCTION(NetClient_GetTimeZone);
    IMPLEMENT_FUNCTION(NetClient_SetCurLanguage);
    IMPLEMENT_FUNCTION(NetClient_GetCurLanguage);
    IMPLEMENT_FUNCTION(NetClient_GetLanguageList);//4.0todo
    IMPLEMENT_FUNCTION(NetClient_SetChannelEncodeProfile);
    IMPLEMENT_FUNCTION(NetClient_GetChannelEncodeProfile);
    IMPLEMENT_FUNCTION(NetClient_SetExceptionHandleParam);
    IMPLEMENT_FUNCTION(NetClient_GetExceptionHandleParam);
    IMPLEMENT_FUNCTION(NetClient_SetAlarmClear);
    IMPLEMENT_FUNCTION(NetClient_GetCharSet);
    IMPLEMENT_FUNCTION(NetClient_SetImgDisposal);
    IMPLEMENT_FUNCTION(NetClient_GetImgDisposal);
    IMPLEMENT_FUNCTION(NetClient_SetWaterMarkEnable);
    IMPLEMENT_FUNCTION(NetClient_GetWaterMarkEnable);
    IMPLEMENT_FUNCTION(NetClient_SetSystemType);
    IMPLEMENT_FUNCTION(NetClient_GetSystemType);
    IMPLEMENT_FUNCTION(NetClient_SetEncryptSN);
    IMPLEMENT_FUNCTION(NetClient_GetSNReg);
    IMPLEMENT_FUNCTION(NetClient_SetAlarmLink_V1);
    IMPLEMENT_FUNCTION(NetClient_GetAlarmLink_V1);
    IMPLEMENT_FUNCTION(NetClient_SetCameraParam);
    IMPLEMENT_FUNCTION(NetClient_GetCameraParam);
    IMPLEMENT_FUNCTION(NetClient_SetITSLightInfo);
    IMPLEMENT_FUNCTION(NetClient_GetITSLightInfo);
    //IMPLEMENT_FUNCTION(NetClient_GetServerVersion_V1);//4.0todo
    IMPLEMENT_FUNCTION(NetClient_CheckDeviceState);
    IMPLEMENT_FUNCTION(NetClient_GetDeviceState);
	IMPLEMENT_FUNCTION(NetClient_SetVerticalSync);
	IMPLEMENT_FUNCTION(NetClient_GetVerticalSync);
    IMPLEMENT_FUNCTION(NetClient_SetLocalAudioVolumeEx);
    IMPLEMENT_FUNCTION(NetClient_GetLocalAudioVolumeEx);
    IMPLEMENT_FUNCTION(NetClient_SetMuted);
    IMPLEMENT_FUNCTION(NetClient_SetLanParam);
    IMPLEMENT_FUNCTION(NetClient_GetLanParam);
    IMPLEMENT_FUNCTION(NetClient_SetPWMValue);      //chenbin@120213
    IMPLEMENT_FUNCTION(NetClient_GetPWMValue);
    IMPLEMENT_FUNCTION(NetClient_ShutDownDev);      //chenbin@120224
    IMPLEMENT_FUNCTION(NetClient_GetVideoSzList);
    IMPLEMENT_FUNCTION(NetClient_ClearPolyLocalVideo);
    IMPLEMENT_FUNCTION(NetClient_SetAlarmConfig);
    IMPLEMENT_FUNCTION(NetClient_GetAlarmConfig);
    IMPLEMENT_FUNCTION(NetClient_SetITSSwitchInterval);
    IMPLEMENT_FUNCTION(NetClient_GetITSSwitchInterval);
    IMPLEMENT_FUNCTION(NetClient_NetFileSetChannelParam);
    IMPLEMENT_FUNCTION(NetClient_NetFileGetChannelParam);
    IMPLEMENT_FUNCTION(NetClient_NetFileGetDiskInfoEx);
    IMPLEMENT_FUNCTION(NetClient_InterTalkStartEx);
    IMPLEMENT_FUNCTION(NetClient_SetAudioSample);
    IMPLEMENT_FUNCTION(NetClient_GetAudioSample);
    IMPLEMENT_FUNCTION(NetClient_Startup_V4);
	IMPLEMENT_FUNCTION(NetClient_SetNotifyFunction_V4);
	IMPLEMENT_FUNCTION(NetClient_SetAlarmNotify_V5);
    IMPLEMENT_FUNCTION(NetClient_StartRecv_V4);
	IMPLEMENT_FUNCTION(NetClient_StartRecv_V5);
    IMPLEMENT_FUNCTION(NetClient_SetNotifyUserData_V4);
    IMPLEMENT_FUNCTION(NetClient_SetComRecvNotify_V4);
    IMPLEMENT_FUNCTION(NetClient_GetHTTPPort_V4);
    IMPLEMENT_FUNCTION(NetClient_SetHTTPPort_V4);
    IMPLEMENT_FUNCTION(NetClient_SetDomainParsePara_V4);
    IMPLEMENT_FUNCTION(NetClient_GetDomainParsePara_V4);
    IMPLEMENT_FUNCTION(NetClient_GetBitrateOnVideo_V4);
    IMPLEMENT_FUNCTION(NetClient_GetDevInfo);
    IMPLEMENT_FUNCTION(NetClient_SetComFormat_V2);
    IMPLEMENT_FUNCTION(NetClient_GetComFormat_V2);
    IMPLEMENT_FUNCTION(NetClient_ATMQueryFile);
    IMPLEMENT_FUNCTION(NetClient_ATMGetQueryFile);
    IMPLEMENT_FUNCTION(NetClient_GetServerVersion_V1);
	IMPLEMENT_FUNCTION(NetClient_SetFullStreamNotify_V4);
	IMPLEMENT_FUNCTION(NetClient_NetFileLockFile);//文件加锁
	IMPLEMENT_FUNCTION(NetClient_NetFileGetQueryfileEx);
	IMPLEMENT_FUNCTION(NetClient_GetOsdTextEx);
	IMPLEMENT_FUNCTION(NetClient_NetFileDownload);
	IMPLEMENT_FUNCTION(NetClient_Upgrade_V4);//修改文件升级不兼容问题
	IMPLEMENT_FUNCTION(NetClient_SetStreamInsertData);
	IMPLEMENT_FUNCTION(NetClient_GetStreamInsertData);
	IMPLEMENT_FUNCTION(NetClient_GetOtherID);
	IMPLEMENT_FUNCTION(NetClient_SetDomePTZ);
	IMPLEMENT_FUNCTION(NetClient_GetDomePTZ);
	IMPLEMENT_FUNCTION(NetClient_GetUserDataInfo);
	IMPLEMENT_FUNCTION(NetClient_GetBroadcastMessage);
	IMPLEMENT_FUNCTION(NetClient_SetDevConfig);
	IMPLEMENT_FUNCTION(NetClient_GetDevConfig);
	IMPLEMENT_FUNCTION(NetClient_SetJEPGSize);
	IMPLEMENT_FUNCTION(NetClient_GetJEPGSize);
	IMPLEMENT_FUNCTION(NetClient_QueryDevStatus);
	IMPLEMENT_FUNCTION(NetClient_GetDevStatus);
	IMPLEMENT_FUNCTION(NetClient_SendCommand);
	IMPLEMENT_FUNCTION(NetClient_RecvCommand);
	IMPLEMENT_FUNCTION(NetClient_SetDsmConfig);
	IMPLEMENT_FUNCTION(NetClient_GetDsmRegstierInfo);
	IMPLEMENT_FUNCTION(NetClient_Logon_V4);
	IMPLEMENT_FUNCTION(NetClient_SetDevDiskConfig);
	IMPLEMENT_FUNCTION(NetClient_GetDevDiskConfig);
	IMPLEMENT_FUNCTION(NetClient_GetPseChInfo);
	IMPLEMENT_FUNCTION(NetClient_SetPseChProperty);
	IMPLEMENT_FUNCTION(NetClient_GetPseChProperty);
	IMPLEMENT_FUNCTION(NetClient_ChannelTalkStart);
	IMPLEMENT_FUNCTION(NetClient_ChannelTalkEnd);
	IMPLEMENT_FUNCTION(NetClient_InputChannelTalkingdata);
	IMPLEMENT_FUNCTION(NetClient_GetChannelTalkingState);
	IMPLEMENT_FUNCTION(NetClient_RebootDeviceByType);
	IMPLEMENT_FUNCTION(NetClient_SetSDKWorkMode);
	IMPLEMENT_FUNCTION(NetClient_StartDownload);
	IMPLEMENT_FUNCTION(NetClient_StopDownload);
	IMPLEMENT_FUNCTION(NetClient_GetDownloadPos);
	IMPLEMENT_FUNCTION(NetClient_PlayBack);
	IMPLEMENT_FUNCTION(NetClient_StopPlayBack);
	IMPLEMENT_FUNCTION(NetClient_PlayBackControl);
	IMPLEMENT_FUNCTION(NetClient_SetDomeAdvParam);
	IMPLEMENT_FUNCTION(NetClient_GetDomeAdvParam);
	IMPLEMENT_FUNCTION(NetClient_GetHDTemplateIndex);
	IMPLEMENT_FUNCTION(NetClient_InnerAutoTest);
	IMPLEMENT_FUNCTION(NetClient_SetDevUserDataNotify);
	IMPLEMENT_FUNCTION(NetClient_Query_V4);
	IMPLEMENT_FUNCTION(NetClient_GetQueryResult_V4);
	IMPLEMENT_FUNCTION(NetClient_CheckCamera);
	IMPLEMENT_FUNCTION(NetClient_RegisterDrawFun);
	IMPLEMENT_FUNCTION(NetClient_GetRecvInfoById);
	IMPLEMENT_FUNCTION(NetClient_SetDataPackCallBack);
	IMPLEMENT_FUNCTION(NetClient_Query_V5);
	IMPLEMENT_FUNCTION(NetClient_FaceConfig);
	IMPLEMENT_FUNCTION(NetClient_SetUnipueAlertConfig);
	IMPLEMENT_FUNCTION(NetClient_GetUnipueAlertConfig);
	IMPLEMENT_FUNCTION(NetClient_CmdConfig);
	IMPLEMENT_FUNCTION(NetClient_GetLastError);
	IMPLEMENT_FUNCTION(NetClient_SyncLogon);
	IMPLEMENT_FUNCTION(NetClient_SyncRealPlay);
	IMPLEMENT_FUNCTION(NetClient_StopRealPlay);
	IMPLEMENT_FUNCTION(NetClient_SyncQuery);
	IMPLEMENT_FUNCTION(NetClient_SyncSetDevCfg);
	IMPLEMENT_FUNCTION(NetClient_GetZFConfig);
	IMPLEMENT_FUNCTION(NetClient_SetZFConfig);
    IMPLEMENT_FUNCTION(NetClient_SetExternDevLogonInfo);
	IMPLEMENT_FUNCTION(NetClient_SetAVMode);
	IMPLEMENT_FUNCTION(NetClient_CapturePicByDevice);
	IMPLEMENT_FUNCTION(NetClient_GetDevConfig_V5);
    IMPLEMENT_FUNCTION(NetClient_ZFConfig);
	IMPLEMENT_FUNCTION(NetClient_HttpXmlConfig);
    
    int LoadNVSSDK(const char* _pszPath)
	{
		if (hNVSSDK)
			return	DLL_RELOAD;
		else
		{
			hNVSSDK=::LoadLibrary(_pszPath);
			if (!hNVSSDK)
				return	DLL_NOT_FOUND;
		}
		NetClient_SetPort                =    (pfNetClient_SetPort)GetProcAddress(hNVSSDK, "NetClient_SetPort");
		NetClient_SetMSGHandle           =    (pfNetClient_SetMSGHandle)GetProcAddress(hNVSSDK, "NetClient_SetMSGHandle");
		NetClient_Startup                =    (pfNetClient_Startup)GetProcAddress(hNVSSDK, "NetClient_Startup");
		NetClient_Cleanup                =    (pfNetClient_Cleanup)GetProcAddress(hNVSSDK, "NetClient_Cleanup");
		NetClient_SetNotifyFunction      =    (pfNetClient_SetNotifyFunction)GetProcAddress(hNVSSDK, "NetClient_SetNotifyFunction");
		NetClient_Logon                  =    (pfNetClient_Logon)GetProcAddress(hNVSSDK, "NetClient_Logon");
		NetClient_Logoff                 =    (pfNetClient_Logoff)GetProcAddress(hNVSSDK, "NetClient_Logoff");
		NetClient_GetLogonStatus         =    (pfNetClient_GetLogonStatus)GetProcAddress(hNVSSDK, "NetClient_GetLogonStatus");
		NetClient_StartRecv              =    (pfNetClient_StartRecv)GetProcAddress(hNVSSDK, "NetClient_StartRecv");
		NetClient_StopRecv               =    (pfNetClient_StopRecv)GetProcAddress(hNVSSDK, "NetClient_StopRecv");
		NetClient_GetRecvID              =    (pfNetClient_GetRecvID)GetProcAddress(hNVSSDK, "NetClient_GetRecvID");
		NetClient_StartCaptureData       =    (pfNetClient_StartCaptureData)GetProcAddress(hNVSSDK, "NetClient_StartCaptureData");
		NetClient_StopCaptureData        =    (pfNetClient_StopCaptureData)GetProcAddress(hNVSSDK, "NetClient_StopCaptureData");
		NetClient_StartRecvNetPicStream  =	  (pfNetClient_StartRecvNetPicStream)GetProcAddress(hNVSSDK, "NetClient_StartRecvNetPicStream");
		NetClient_StopRecvNetPicStream   =    (pfNetClient_StopRecvNetPicStream)GetProcAddress(hNVSSDK, "NetClient_StopRecvNetPicStream");
		NetClient_GetVideoHeader         =    (pfNetClient_GetVideoHeader)GetProcAddress(hNVSSDK, "NetClient_GetVideoHeader");
		NetClient_StartCaptureFile       =    (pfNetClient_StartCaptureFile)GetProcAddress(hNVSSDK, "NetClient_StartCaptureFile");
		NetClient_StopCaptureFile        =    (pfNetClient_StopCaptureFile)GetProcAddress(hNVSSDK, "NetClient_StopCaptureFile");
		NetClient_GetCaptureStatus       =    (pfNetClient_GetCaptureStatus)GetProcAddress(hNVSSDK, "NetClient_GetCaptureStatus");
		NetClient_StopPlay               =    (pfNetClient_StopPlay)GetProcAddress(hNVSSDK, "NetClient_StopPlay");
		NetClient_StopPlayEx			 =	  (pfNetClient_StopPlayEx)GetProcAddress(hNVSSDK, "NetClient_StopPlayEx");
		NetClient_StartPlay              =    (pfNetClient_StartPlay)GetProcAddress(hNVSSDK, "NetClient_StartPlay");
		NetClient_ResetPlayerWnd         =    (pfNetClient_ResetPlayerWnd)GetProcAddress(hNVSSDK, "NetClient_ResetPlayerWnd");
		NetClient_GetPlayingStatus       =    (pfNetClient_GetPlayingStatus)GetProcAddress(hNVSSDK, "NetClient_GetPlayingStatus");
		NetClient_SetDecCallBack         =    (pfNetClient_SetDecCallBack)GetProcAddress(hNVSSDK, "NetClient_SetDecCallBack");
		NetClient_SetDecCallBack_V4       =    (pfNetClient_SetDecCallBack_V4)GetProcAddress(hNVSSDK, "NetClient_SetDecCallBack_V4");
		NetClient_SendStringToServer     =    (pfNetClient_SendStringToServer)GetProcAddress(hNVSSDK, "NetClient_SendStringToServer");
		NetClient_ReceiveString          =    (pfNetClient_ReceiveString)GetProcAddress(hNVSSDK, "NetClient_ReceiveString");
		NetClient_SetLocalAudioVolume    =    (pfNetClient_SetLocalAudioVolume)GetProcAddress(hNVSSDK, "NetClient_SetLocalAudioVolume");
		NetClient_AudioStart             =    (pfNetClient_AudioStart)GetProcAddress(hNVSSDK, "NetClient_AudioStart");
		NetClient_AudioStop              =    (pfNetClient_AudioStop)GetProcAddress(hNVSSDK, "NetClient_AudioStop");
		NetClient_SetBufferNum           =    (pfNetClient_SetBufferNum)GetProcAddress(hNVSSDK, "NetClient_SetBufferNum");
		NetClient_SetPlayDelay           =    (pfNetClient_SetPlayDelay)GetProcAddress(hNVSSDK, "NetClient_SetPlayDelay");
		NetClient_GetDecordFrameNum      =    (pfNetClient_GetDecordFrameNum)GetProcAddress(hNVSSDK, "NetClient_GetDecordFrameNum");
		NetClient_GetChannelNum          =    (pfNetClient_GetChannelNum)GetProcAddress(hNVSSDK, "NetClient_GetChannelNum");
		NetClient_GetAlarmPortNum        =    (pfNetClient_GetAlarmPortNum)GetProcAddress(hNVSSDK, "NetClient_GetAlarmPortNum");
		NetClient_SetVideoPara           =    (pfNetClient_SetVideoPara)GetProcAddress(hNVSSDK, "NetClient_SetVideoPara");
		NetClient_GetVideoPara           =    (pfNetClient_GetVideoPara)GetProcAddress(hNVSSDK, "NetClient_GetVideoPara");
		NetClient_SetVideoQuality        =    (pfNetClient_SetVideoQuality)GetProcAddress(hNVSSDK, "NetClient_SetVideoQuality");
		NetClient_GetVideoQuality        =    (pfNetClient_GetVideoQuality)GetProcAddress(hNVSSDK, "NetClient_GetVideoQuality");
		NetClient_SetFrameRate           =    (pfNetClient_SetFrameRate)GetProcAddress(hNVSSDK, "NetClient_SetFrameRate");
		NetClient_GetFrameRate           =    (pfNetClient_GetFrameRate)GetProcAddress(hNVSSDK, "NetClient_GetFrameRate");
		NetClient_SetStreamType          =    (pfNetClient_SetStreamType)GetProcAddress(hNVSSDK, "NetClient_SetStreamType");
		NetClient_GetStreamType          =    (pfNetClient_GetStreamType)GetProcAddress(hNVSSDK, "NetClient_GetStreamType");
		NetClient_SetSetMotionAreaEnable =    (pfNetClient_SetMotionAreaEnable)GetProcAddress(hNVSSDK, "NetClient_SetMotionAreaEnable");
		NetClient_SetMotionDetetionArea  =    (pfNetClient_SetMotionDetetionArea)GetProcAddress(hNVSSDK, "NetClient_SetMotionDetetionArea");
		NetClient_GetMotionDetetionArea  =    (pfNetClient_GetMotionDetetionArea)GetProcAddress(hNVSSDK, "NetClient_GetMotionDetetionArea");
		NetClient_SetThreshold           =    (pfNetClient_SetThreshold)GetProcAddress(hNVSSDK, "NetClient_SetThreshold");
		NetClient_GetThreshold           =    (pfNetClient_GetThreshold)GetProcAddress(hNVSSDK, "NetClient_GetThreshold");
		NetClient_SetMotionDetection     =    (pfNetClient_SetMotionDetection)GetProcAddress(hNVSSDK, "NetClient_SetMotionDetection");
		NetClient_GetMotionDetection     =    (pfNetClient_GetMotionDetection)GetProcAddress(hNVSSDK, "NetClient_GetMotionDetection");
		NetClient_SetOsdText             =    (pfNetClient_SetOsdText)GetProcAddress(hNVSSDK, "NetClient_SetOsdText");
		NetClient_GetOsdText             =    (pfNetClient_GetOsdText)GetProcAddress(hNVSSDK, "NetClient_GetOsdText");
		NetClient_SetOsdType             =    (pfNetClient_SetOsdType)GetProcAddress(hNVSSDK, "NetClient_SetOsdType");
		NetClient_GetOsdType             =    (pfNetClient_GetOsdType)GetProcAddress(hNVSSDK, "NetClient_GetOsdType");
		NetClient_SetOsdLOGO             =    (pfNetClient_SetOsdLOGO)GetProcAddress(hNVSSDK, "NetClient_SetOsdLOGO");
		NetClient_SetAudioChannel        =    (pfNetClient_SetAudioChannel)GetProcAddress(hNVSSDK, "NetClient_SetAudioChannel");
		NetClient_GetAudioChannel        =    (pfNetClient_GetAudioChannel)GetProcAddress(hNVSSDK, "NetClient_GetAudioChannel");
		NetClient_SetIpFilter            =    (pfNetClient_SetIpFilter)GetProcAddress(hNVSSDK, "NetClient_SetIpFilter");
		NetClient_GetIpFilter            =    (pfNetClient_GetIpFilter)GetProcAddress(hNVSSDK, "NetClient_GetIpFilter");
		NetClient_SetAlarmOutput         =    (pfNetClient_SetAlarmOutput)GetProcAddress(hNVSSDK, "NetClient_SetAlarmOutput");
		NetClient_GetAlarmOutput         =    (pfNetClient_GetAlarmOutput)GetProcAddress(hNVSSDK, "NetClient_GetAlarmOutput");
		NetClient_SetAlarmInMode         =    (pfNetClient_SetAlarmInMode)GetProcAddress(hNVSSDK, "NetClient_SetAlarmInMode");
		NetClient_GetAlarmInMode         =    (pfNetClient_GetAlarmInMode)GetProcAddress(hNVSSDK, "NetClient_GetAlarmInMode");
		NetClient_SetAlarmOutMode        =    (pfNetClient_SetAlarmOutMode)GetProcAddress(hNVSSDK, "NetClient_SetAlarmOutMode");
		NetClient_GetAlarmOutMode        =    (pfNetClient_GetAlarmOutMode)GetProcAddress(hNVSSDK, "NetClient_GetAlarmOutMode");
		NetClient_SetAlmVdoCovThreshold  =    (pfNetClient_SetAlmVdoCovThreshold)GetProcAddress(hNVSSDK, "NetClient_SetAlmVdoCovThreshold");
		NetClient_GetAlmVdoCovThreshold  =    (pfNetClient_GetAlmVdoCovThreshold)GetProcAddress(hNVSSDK, "NetClient_GetAlmVdoCovThreshold");
		NetClient_SetAlmVideoCov         =    (pfNetClient_SetAlmVideoCov)GetProcAddress(hNVSSDK, "NetClient_SetAlmVideoCov");
		NetClient_GetAlmVideoCov         =    (pfNetClient_GetAlmVideoCov)GetProcAddress(hNVSSDK, "NetClient_GetAlmVideoCov");
		NetClient_SetDeviceType          =    (pfNetClient_SetDeviceType)GetProcAddress(hNVSSDK, "NetClient_SetDeviceType");
		NetClient_GetDeviceType          =    (pfNetClient_GetDeviceType)GetProcAddress(hNVSSDK, "NetClient_GetDeviceType");
		NetClient_GetAllSupportDeviceType=    (pfNetClient_GetAllSupportDeviceType)GetProcAddress(hNVSSDK, "NetClient_GetAllSupportDeviceType");
		NetClient_SetComFormat           =    (pfNetClient_SetComFormat)GetProcAddress(hNVSSDK, "NetClient_SetComFormat");
		NetClient_GetComFormat           =    (pfNetClient_GetComFormat)GetProcAddress(hNVSSDK, "NetClient_GetComFormat");
		NetClient_DeviceCtrl             =    (pfNetClient_DeviceCtrl)GetProcAddress(hNVSSDK, "NetClient_DeviceCtrl");
		NetClient_DeviceCtrlEx           =    (pfNetClient_DeviceCtrlEx)GetProcAddress(hNVSSDK, "NetClient_DeviceCtrlEx");
		NetClient_ComSend                =    (pfNetClient_ComSend)GetProcAddress(hNVSSDK, "NetClient_ComSend");
		NetClient_SetComRecvNotify       =    (pfNetClient_SetComRecvNotify)GetProcAddress(hNVSSDK, "NetClient_SetComRecvNotify");
		NetClient_SetAlarmVideoLost      =    (pfNetClient_SetAlarmVideoLost)GetProcAddress(hNVSSDK, "NetClient_SetAlarmVideoLost");
		NetClient_GetAlarmVideoLost      =    (pfNetClient_GetAlarmVideoLost)GetProcAddress(hNVSSDK, "NetClient_GetAlarmVideoLost");
		NetClient_SetIFrameRate          =    (pfNetClient_SetIFrameRate)GetProcAddress(hNVSSDK, "NetClient_SetIFrameRate");
		NetClient_GetIFrameRate          =    (pfNetClient_GetIFrameRate)GetProcAddress(hNVSSDK, "NetClient_GetIFrameRate");
		NetClient_ForceIFrame            =    (pfNetClient_ForceIFrame)GetProcAddress(hNVSSDK, "NetClient_ForceIFrame");
		NetClient_SetTime                =    (pfNetClient_SetTime)GetProcAddress(hNVSSDK, "NetClient_SetTime");
		NetClient_Reboot                 =    (pfNetClient_Reboot)GetProcAddress(hNVSSDK, "NetClient_Reboot");
		NetClient_DefaultPara            =    (pfNetClient_DefaultPara)GetProcAddress(hNVSSDK, "NetClient_DefaultPara");
		NetClient_GetServerVersion       =    (pfNetClient_GetServerVersion)GetProcAddress(hNVSSDK, "NetClient_GetServerVersion");
		NetClient_UpgradeProgram         =    (pfNetClient_UpgradeProgram)GetProcAddress(hNVSSDK, "NetClient_UpgradeProgram");
		NetClient_UpgradeWebPage         =    (pfNetClient_UpgradeWebPage)GetProcAddress(hNVSSDK, "NetClient_UpgradeWebPage");
		NetClient_GetUpgradePercent      =    (pfNetClient_GetUpgradePercent)GetProcAddress(hNVSSDK, "NetClient_GetUpgradePercent");
		NetClient_GetUserNum             =    (pfNetClient_GetUserNum)GetProcAddress(hNVSSDK, "NetClient_GetUserNum");
		NetClient_GetUserInfo            =    (pfNetClient_GetUserInfo)GetProcAddress(hNVSSDK, "NetClient_GetUserInfo");
		NetClient_GetCurUserInfo         =    (pfNetClient_GetCurUserInfo)GetProcAddress(hNVSSDK, "NetClient_GetCurUserInfo");
		NetClient_AddUser                =    (pfNetClient_AddUser)GetProcAddress(hNVSSDK, "NetClient_AddUser");
		NetClient_DelUser                =    (pfNetClient_DelUser)GetProcAddress(hNVSSDK, "NetClient_DelUser");
		NetClient_ModifyPwd              =    (pfNetClient_ModifyPwd)GetProcAddress(hNVSSDK, "NetClient_ModifyPwd");
		NetClient_ChangeSvrIP            =    (pfNetClient_ChangeSvrIP)GetProcAddress(hNVSSDK, "NetClient_ChangeSvrIP");
		NetClient_SetMaxConUser          =    (pfNetClient_SetMaxConUser)GetProcAddress(hNVSSDK, "NetClient_SetMaxConUser");
		NetClient_GetMaxGetUser          =    (pfNetClient_GetMaxGetUser)GetProcAddress(hNVSSDK, "NetClient_GetMaxGetUser");
		NetClient_TalkStart              =    (pfNetClient_TalkStart)GetProcAddress(hNVSSDK, "NetClient_TalkStart");
		NetClient_TalkEnd                =    (pfNetClient_TalkEnd)GetProcAddress(hNVSSDK, "NetClient_TalkEnd");
		NetClient_CapturePicture		 =	  (pfNetClient_CapturePicture)GetProcAddress(hNVSSDK, "NetClient_CapturePicture");
		NetClient_CapturePic             =    (pfNetClient_CapturePic)GetProcAddress(hNVSSDK, "NetClient_CapturePic");
		NetClient_CaptureBmpPic          =    (pfNetClient_CaptureBmpPic)GetProcAddress(hNVSSDK, "NetClient_CaptureBmpPic");
		NetClient_SetOutPortState        =    (pfNetClient_SetOutPortState)GetProcAddress(hNVSSDK, "NetClient_SetOutPortState");
		NetClient_GetIpProperty          =    (pfNetClient_GetIpProperty)GetProcAddress(hNVSSDK, "NetClient_GetIpProperty");
		NetClient_GetVideoCovArea        =    (pfNetClient_GetVideoCovArea)GetProcAddress(hNVSSDK, "NetClient_GetVideoCovArea");
		NetClient_SetVideoCovArea        =    (pfNetClient_SetVideoCovArea)GetProcAddress(hNVSSDK, "NetClient_SetVideoCovArea");
		NetClient_GetVideoSizeEx         =    (pfNetClient_GetVideoSizeEx)GetProcAddress(hNVSSDK, "NetClient_GetVideoSizeEx");
		NetClient_SetVideoSize           =    (pfNetClient_SetVideoSize)GetProcAddress(hNVSSDK, "NetClient_SetVideoSize");
		NetClient_GetVideoSize           =    (pfNetClient_GetVideoSize)GetProcAddress(hNVSSDK, "NetClient_GetVideoSize");
		NetClient_SetMaxKByteRate        =    (pfNetClient_SetMaxKByteRate)GetProcAddress(hNVSSDK, "NetClient_SetMaxKByteRate");
		NetClient_GetMaxKByteRate        =    (pfNetClient_GetMaxKByteRate)GetProcAddress(hNVSSDK, "NetClient_GetMaxKByteRate");
		NetClient_SetAlarmServer         =    (pfNetClient_SetAlarmServer)GetProcAddress(hNVSSDK, "NetClient_SetAlarmServer");
		NetClient_GetAlarmServer         =    (pfNetClient_GetAlarmServer)GetProcAddress(hNVSSDK, "NetClient_GetAlarmServer");
		NetClient_SetDHCPParam           =    (pfNetClient_SetDHCPParam)GetProcAddress(hNVSSDK, "NetClient_SetDHCPParam");
		NetClient_GetDHCPParam           =    (pfNetClient_GetDHCPParam)GetProcAddress(hNVSSDK, "NetClient_GetDHCPParam");
		NetClient_DeviceSetup            =    (pfNetClient_DeviceSetup)GetProcAddress(hNVSSDK, "NetClient_DeviceSetup");


		NetClient_WriteUserData          =    (pfNetClient_WriteUserData)GetProcAddress(hNVSSDK, "NetClient_WriteUserData");
		NetClient_ReadUserData           =    (pfNetClient_ReadUserData)GetProcAddress(hNVSSDK, "NetClient_ReadUserData");
		NetClient_SetDomainParsePara     =    (pfNetClient_SetDomainParsePara)GetProcAddress(hNVSSDK, "NetClient_SetDomainParsePara");
		NetClient_GetDomainParsePara     =    (pfNetClient_GetDomainParsePara)GetProcAddress(hNVSSDK, "NetClient_GetDomainParsePara");
		NetClient_SendStringToCenter     =    (pfNetClient_SendStringToCenter)GetProcAddress(hNVSSDK, "NetClient_SendStringToCenter");
		NetClient_DrawRectOnLocalVideo   =    (pfNetClient_DrawRectOnLocalVideo)GetProcAddress(hNVSSDK, "NetClient_DrawRectOnLocalVideo");
		NetClient_GetBothStreamSame      =    (pfNetClient_GetBothStreamSame)GetProcAddress(hNVSSDK, "NetClient_GetBothStreamSame");
		NetClient_DrawTextOnVideo        =    (pfNetClient_DrawTextOnVideo)GetProcAddress(hNVSSDK, "NetClient_DrawTextOnVideo");
		NetClient_GetTextOnVideo		 =    (pfNetClient_GetTextOnVideo)GetProcAddress(hNVSSDK, "NetClient_GetTextOnVideo");
		NetClient_SetAlarmPortEnable     =    (pfNetClient_SetAlarmPortEnable)GetProcAddress(hNVSSDK, "NetClient_SetAlarmPortEnable");
		NetClient_GetAlarmPortEnable     =    (pfNetClient_GetAlarmPortEnable)GetProcAddress(hNVSSDK, "NetClient_GetAlarmPortEnable");
		NetClient_ShowBitrateOnVideo     =    (pfNetClient_ShowBitrateOnVideo)GetProcAddress(hNVSSDK, "NetClient_ShowBitrateOnVideo");
		NetClient_SetPPPoEInfo           =    (pfNetClient_SetPPPoEInfo)GetProcAddress(hNVSSDK, "NetClient_SetPPPoEInfo");
		NetClient_GetPPPoEInfo           =    (pfNetClient_GetPPPoEInfo)GetProcAddress(hNVSSDK, "NetClient_GetPPPoEInfo");
		NetClient_CPUCheckEnabled        =    (pfNetClient_CPUCheckEnabled)GetProcAddress(hNVSSDK, "NetClient_CPUCheckEnabled");
		NetClient_GetBitrateOnVideo      =    (pfNetClient_GetBitrateOnVideo)GetProcAddress(hNVSSDK, "NetClient_GetBitrateOnVideo");
		NetClient_GetMaxVideoSize        =    (pfNetClient_GetMaxVideoSize)GetProcAddress(hNVSSDK, "NetClient_GetMaxVideoSize");
		NetClient_SetEncodeMode          =    (pfNetClient_SetEncodeMode)GetProcAddress(hNVSSDK, "NetClient_SetEncodeMode");
		NetClient_GetEncodeMode          =    (pfNetClient_GetEncodeMode)GetProcAddress(hNVSSDK, "NetClient_GetEncodeMode");
		NetClient_SetPreferMode          =    (pfNetClient_SetPreferMode)GetProcAddress(hNVSSDK, "NetClient_SetPreferMode");
		NetClient_GetPreferMode          =    (pfNetClient_GetPreferMode)GetProcAddress(hNVSSDK, "NetClient_GetPreferMode");
		NetClient_NetFileQuery           =    (pfNetClient_NetFileQuery)GetProcAddress(hNVSSDK, "NetClient_NetFileQuery");
		NetClient_NetFileGetFileCount    =    (pfNetClient_NetFileGetFileCount)GetProcAddress(hNVSSDK, "NetClient_NetFileGetFileCount");
		NetClient_NetFileGetQueryfile    =    (pfNetClient_NetFileGetQueryfile)GetProcAddress(hNVSSDK, "NetClient_NetFileGetQueryfile");
		NetClient_NetFileManualRecord    =    (pfNetClient_NetFileManualRecord)GetProcAddress(hNVSSDK, "NetClient_NetFileManualRecord");
		NetClient_NetFileMapStoreDevice  =    (pfNetClient_NetFileMapStoreDevice)GetProcAddress(hNVSSDK, "NetClient_NetFileMapStoreDevice");
		NetClient_NetFileGetMapStoreDevice=    (pfNetClient_NetFileGetMapStoreDevice)GetProcAddress(hNVSSDK, "NetClient_NetFileGetMapStoreDevice");
		NetClient_NetFileMountUSB        =    (pfNetClient_NetFileMountUSB)GetProcAddress(hNVSSDK, "NetClient_NetFileMountUSB");
		NetClient_NetFileGetUSBstate     =    (pfNetClient_NetFileGetUSBstate)GetProcAddress(hNVSSDK, "NetClient_NetFileGetUSBstate");
		NetClient_NetFileSetExtendname   =    (pfNetClient_NetFileSetExtendname)GetProcAddress(hNVSSDK, "NetClient_NetFileSetExtendname");
		NetClient_NetFileGetExtendname   =    (pfNetClient_NetFileGetExtendname)GetProcAddress(hNVSSDK, "NetClient_NetFileGetExtendname");
		NetClient_NetFileSetRecordRule   =    (pfNetClient_NetFileSetRecordRule)GetProcAddress(hNVSSDK, "NetClient_NetFileSetRecordRule");
		NetClient_NetFileGetRecordRule   =    (pfNetClient_NetFileGetRecordRule)GetProcAddress(hNVSSDK, "NetClient_NetFileGetRecordRule");
		NetClient_NetFileSetAlarmRule    =    (pfNetClient_NetFileSetAlarmRule)GetProcAddress(hNVSSDK, "NetClient_NetFileSetAlarmRule");
		NetClient_NetFileGetAlarmRule    =    (pfNetClient_NetFileGetAlarmRule)GetProcAddress(hNVSSDK, "NetClient_NetFileGetAlarmRule");
		NetClient_NetFileSetAlarmState   =    (pfNetClient_NetFileSetAlarmState)GetProcAddress(hNVSSDK, "NetClient_NetFileSetAlarmState");
		NetClient_NetFileGetAlarmState   =    (pfNetClient_NetFileGetAlarmState)GetProcAddress(hNVSSDK, "NetClient_NetFileGetAlarmState");
		NetClient_NetFileSetTaskState    =    (pfNetClient_NetFileSetTaskState)GetProcAddress(hNVSSDK, "NetClient_NetFileSetTaskState");
		NetClient_NetFileGetTaskState    =    (pfNetClient_NetFileGetTaskState)GetProcAddress(hNVSSDK, "NetClient_NetFileGetTaskState");
		NetClient_NetFileSetTaskSchedule =    (pfNetClient_NetFileSetTaskSchedule)GetProcAddress(hNVSSDK, "NetClient_NetFileSetTaskSchedule");
		NetClient_NetFileGetTaskSchedule =    (pfNetClient_NetFileGetTaskSchedule)GetProcAddress(hNVSSDK, "NetClient_NetFileGetTaskSchedule");
		NetClient_NetFileRebuildIndexFile=    (pfNetClient_NetFileRebuildIndexFile)GetProcAddress(hNVSSDK, "NetClient_NetFileRebuildIndexFile");
		NetClient_NetFileGetDiskInfo     =    (pfNetClient_NetFileGetDiskInfo)GetProcAddress(hNVSSDK, "NetClient_NetFileGetDiskInfo");
		NetClient_NetFileIsSupportStorage=    (pfNetClient_NetFileIsSupportStorage)GetProcAddress(hNVSSDK, "NetClient_NetFileIsSupportStorage");
		NetClient_NetFileDownloadFile    =    (pfNetClient_NetFileDownloadFile)GetProcAddress(hNVSSDK, "NetClient_NetFileDownloadFile");
		NetClient_NetFileStopDownloadFile=    (pfNetClient_NetFileStopDownloadFile)GetProcAddress(hNVSSDK, "NetClient_NetFileStopDownloadFile");
		NetClient_NetFileGetDownloadPos  =    (pfNetClient_NetFileGetDownloadPos)GetProcAddress(hNVSSDK, "NetClient_NetFileGetDownloadPos");
		NetClient_NetFileGetRecordState  =    (pfNetClient_NetFileGetRecordState)GetProcAddress(hNVSSDK, "NetClient_NetFileGetRecordState");
		NetClient_SetAudioEncoder        =    (pfNetClient_SetAudioEncoder)GetProcAddress(hNVSSDK, "NetClient_SetAudioEncoder");
		NetClient_GetAudioEncoder        =    (pfNetClient_GetAudioEncoder)GetProcAddress(hNVSSDK, "NetClient_GetAudioEncoder");
		NetClient_SetVideoNPMode         =    (pfNetClient_SetVideoNPMode)GetProcAddress(hNVSSDK, "NetClient_SetVideoNPMode");
		NetClient_GetVideoNPMode         =    (pfNetClient_GetVideoNPMode)GetProcAddress(hNVSSDK, "NetClient_GetVideoNPMode");
		NetClient_SetDeviceTimerReboot   =    (pfNetClient_SetDeviceTimerReboot)GetProcAddress(hNVSSDK, "NetClient_SetDeviceTimerReboot");
		NetClient_GetDeviceTimerReboot   =    (pfNetClient_GetDeviceTimerReboot)GetProcAddress(hNVSSDK, "NetClient_GetDeviceTimerReboot");
		NetClient_SetManagerServersInfo  =    (pfNetClient_SetManagerServersInfo)GetProcAddress(hNVSSDK, "NetClient_SetManagerServersInfo");
		NetClient_GetManagerServersInfo  =    (pfNetClient_GetManagerServersInfo)GetProcAddress(hNVSSDK, "NetClient_GetManagerServersInfo");
		NetClient_SetDeviceID            =    (pfNetClient_SetDeviceID)GetProcAddress(hNVSSDK, "NetClient_SetDeviceID");
		NetClient_GetDeviceID            =    (pfNetClient_GetDeviceID)GetProcAddress(hNVSSDK, "NetClient_GetDeviceID");
		NetClient_SetMediaStreamClient   =    (pfNetClient_SetMediaStreamClient)GetProcAddress(hNVSSDK, "NetClient_SetMediaStreamClient");
		NetClient_GetMediaStreamClient   =    (pfNetClient_GetMediaStreamClient)GetProcAddress(hNVSSDK, "NetClient_GetMediaStreamClient");
		NetClient_GetAlarmIPortState     =    (pfNetClient_GetAlarmIPortState)GetProcAddress(hNVSSDK, "NetClient_GetAlarmIPortState");
		NetClient_GetAlarmVLostState     =    (pfNetClient_GetAlarmVLostState)GetProcAddress(hNVSSDK, "NetClient_GetAlarmVLostState");
        NetClient_GetAlarmVCoverState    =    (pfNetClient_GetAlarmVCoverState)GetProcAddress(hNVSSDK, "NetClient_GetAlarmVCoverState");
		NetClient_SetDateFormat          =    (pfNetClient_SetDateFormat)GetProcAddress(hNVSSDK, "NetClient_SetDateFormat");
		NetClient_GetDateFormat          =    (pfNetClient_GetDateFormat)GetProcAddress(hNVSSDK, "NetClient_GetDateFormat");
		NetClient_SetRawFrameCallBack    =    (pfNetClient_SetRawFrameCallBack)GetProcAddress(hNVSSDK, "NetClient_SetRawFrameCallBack");
		NetClient_SetRawFrameCallBackEx  =    (pfNetClient_SetRawFrameCallBackEx)GetProcAddress(hNVSSDK, "NetClient_SetRawFrameCallBackEx");
		NetClient_SetHTTPPort            =    (pfNetClient_SetHTTPPort)GetProcAddress(hNVSSDK, "NetClient_SetHTTPPort");
		NetClient_GetHTTPPort            =    (pfNetClient_GetHTTPPort)GetProcAddress(hNVSSDK, "NetClient_GetHTTPPort");
		NetClient_IYUVtoYV12             =    (pfNetClient_IYUVtoYV12)GetProcAddress(hNVSSDK, "NetClient_IYUVtoYV12");
		NetClient_SetEmailAlarm          =    (pfNetClient_SetEmailAlarm)GetProcAddress(hNVSSDK, "NetClient_SetEmailAlarm");
		NetClient_GetEmailAlarm          =    (pfNetClient_GetEmailAlarm)GetProcAddress(hNVSSDK, "NetClient_GetEmailAlarm");
		NetClient_SetEmailAlarmEnable    =    (pfNetClient_SetEmailAlarmEnable)GetProcAddress(hNVSSDK, "NetClient_SetEmailAlarmEnable");
		NetClient_GetEmailAlarmEnable    =    (pfNetClient_GetEmailAlarmEnable)GetProcAddress(hNVSSDK, "NetClient_GetEmailAlarmEnable");
		NetClient_SetScene               =    (pfNetClient_SetScene)GetProcAddress(hNVSSDK, "NetClient_SetScene");
		NetClient_GetScene               =    (pfNetClient_GetScene)GetProcAddress(hNVSSDK, "NetClient_GetScene");
		NetClient_Snapshot               =    (pfNetClient_Snapshot)GetProcAddress(hNVSSDK, "NetClient_Snapshot");
		NetClient_GetFactoryID           =    (pfNetClient_GetFactoryID)GetProcAddress(hNVSSDK, "NetClient_GetFactoryID");
		NetClient_SetSensorFlip          =    (pfNetClient_SetSensorFlip)GetProcAddress(hNVSSDK, "NetClient_SetSensorFlip");
		NetClient_GetSensorFlip          =    (pfNetClient_GetSensorFlip)GetProcAddress(hNVSSDK, "NetClient_GetSensorFlip");
		NetClient_SetSensorMirror        =    (pfNetClient_SetSensorMirror)GetProcAddress(hNVSSDK, "NetClient_SetSensorMirror");
		NetClient_GetSensorMirror        =    (pfNetClient_GetSensorMirror)GetProcAddress(hNVSSDK, "NetClient_GetSensorMirror");
		NetClient_SetWifiParam           =    (pfNetClient_SetWifiParam)GetProcAddress(hNVSSDK, "NetClient_SetWifiParam");
		NetClient_GetWifiParam           =    (pfNetClient_GetWifiParam)GetProcAddress(hNVSSDK, "NetClient_GetWifiParam");
		NetClient_WifiSearch             =    (pfNetClient_WifiSearch)GetProcAddress(hNVSSDK, "NetClient_WifiSearch");
		NetClient_GetWifiSearchResult    =    (pfNetClient_GetWifiSearchResult)GetProcAddress(hNVSSDK, "NetClient_GetWifiSearchResult");
		NetClient_SetPrivacyProtect      =    (pfNetClient_SetPrivacyProtect)GetProcAddress(hNVSSDK, "NetClient_SetPrivacyProtect");
		NetClient_GetPrivacyProtect      =    (pfNetClient_GetPrivacyProtect)GetProcAddress(hNVSSDK, "NetClient_GetPrivacyProtect");
		NetClient_InputTalkingdata       =    (pfNetClient_InputTalkingdata)GetProcAddress(hNVSSDK, "NetClient_InputTalkingdata");
		NetClient_SetNTPInfo             =    (pfNetClient_SetNTPInfo)GetProcAddress(hNVSSDK, "NetClient_SetNTPInfo");
		NetClient_GetNTPInfo             =    (pfNetClient_GetNTPInfo)GetProcAddress(hNVSSDK, "NetClient_GetNTPInfo");
		NetClient_SetCPUMEMAlarmThreshold=    (pfNetClient_SetCPUMEMAlarmThreshold)GetProcAddress(hNVSSDK, "NetClient_SetCPUMEMAlarmThreshold");
		NetClient_GetCPUMEMAlarmThreshol =    (pfNetClient_GetCPUMEMAlarmThreshold)GetProcAddress(hNVSSDK, "NetClient_GetCPUMEMAlarmThreshold");
		NetClient_SetOSDAlpha            =    (pfNetClient_SetOSDAlpha)GetProcAddress(hNVSSDK, "NetClient_SetOSDAlpha");
		NetClient_GetOSDAlpha            =    (pfNetClient_GetOSDAlpha)GetProcAddress(hNVSSDK, "NetClient_GetOSDAlpha");
		NetClient_GetDevType             =    (pfNetClient_GetDevType)GetProcAddress(hNVSSDK, "NetClient_GetDevType");

		NetClient_GetVersion             =    (pfNetClient_GetVersion)GetProcAddress(hNVSSDK, "NetClient_GetVersion");
		NetClient_BindInterface          =    (pfNetClient_BindInterface)GetProcAddress(hNVSSDK, "NetClient_BindInterface");
		NetClient_GetNetInterface        =    (pfNetClient_GetNetInterface)GetProcAddress(hNVSSDK, "NetClient_GetNetInterface");
		NetClient_ProxyAddProxy          =    (pfNetClient_ProxyAddProxy)GetProcAddress(hNVSSDK, "NetClient_ProxyAddProxy");
		NetClient_ProxyStartup           =    (pfNetClient_ProxyStartup)GetProcAddress(hNVSSDK, "NetClient_ProxyStartup");
		NetClient_ProxyHasConnected      =    (pfNetClient_ProxyHasConnected)GetProcAddress(hNVSSDK, "NetClient_ProxyHasConnected");
		NetClient_ProxyGetAllLinks       =    (pfNetClient_ProxyGetAllLinks)GetProcAddress(hNVSSDK, "NetClient_ProxyGetAllLinks");
		NetClient_ProxyGetLinkInfo       =    (pfNetClient_ProxyGetLinkInfo)GetProcAddress(hNVSSDK, "NetClient_ProxyGetLinkInfo");
		NetClient_ProxyDeleteLink        =    (pfNetClient_ProxyDeleteLink)GetProcAddress(hNVSSDK, "NetClient_ProxyDeleteLink");
		NetClient_ProxyDeleteProxy       =    (pfNetClient_ProxyDeleteProxy)GetProcAddress(hNVSSDK, "NetClient_ProxyDeleteProxy");
		NetClient_ProxySendStringToClient=    (pfNetClient_ProxySendStringToClient)GetProcAddress(hNVSSDK, "NetClient_ProxySendStringToClient");
		NetClient_ProxySendStringToClientByID=    (pfNetClient_ProxySendStringToClientByID)GetProcAddress(hNVSSDK, "NetClient_ProxySendStringToClientByID");
		NetClient_SetReducenoiseState    =    (pfNetClient_SetReducenoiseState)GetProcAddress(hNVSSDK, "NetClient_SetReducenoiseState");
		NetClient_GetReducenoiseState    =    (pfNetClient_GetReducenoiseState)GetProcAddress(hNVSSDK, "NetClient_GetReducenoiseState");
		NetClient_ProxySetMaxInOutVideo  =    (pfNetClient_ProxySetMaxInOutVideo)GetProcAddress(hNVSSDK, "NetClient_ProxySetMaxInOutVideo");
		NetClient_ProxyCloseAllLinks     =    (pfNetClient_ProxyCloseAllLinks)GetProcAddress(hNVSSDK, "NetClient_ProxyCloseAllLinks");
		NetClient_DCardInit              =    (pfNetClient_DCardInit)GetProcAddress(hNVSSDK, "NetClient_DCardInit");
		NetClient_DCardSetShowMode       =    (pfNetClient_DCardSetShowMode)GetProcAddress(hNVSSDK, "NetClient_DCardSetShowMode");
		NetClient_DCardGetShowMode       =    (pfNetClient_DCardGetShowMode)GetProcAddress(hNVSSDK, "NetClient_DCardGetShowMode");
		NetClient_DCardStartPlay         =    (pfNetClient_DCardStartPlay)GetProcAddress(hNVSSDK, "NetClient_DCardStartPlay");
		NetClient_DCardStopPlay          =    (pfNetClient_DCardStopPlay)GetProcAddress(hNVSSDK, "NetClient_DCardStopPlay");
		NetClient_DCardRelease           =    (pfNetClient_DCardRelease)GetProcAddress(hNVSSDK, "NetClient_DCardRelease");
		NetClient_DCardReInit            =    (pfNetClient_DCardReInit)GetProcAddress(hNVSSDK, "NetClient_DCardReInit");
		NetClient_DCardGetState          =    (pfNetClient_DCardGetState)GetProcAddress(hNVSSDK, "NetClient_DCardGetState");
		NetClient_pfNetClient_DCardStartPlayEx=    (pfNetClient_DCardStartPlayEx)GetProcAddress(hNVSSDK, "NetClient_DCardStartPlayEx");
		NetClient_pfNetClient_DCardPutDataEx=    (pfNetClient_DCardPutDataEx)GetProcAddress(hNVSSDK, "NetClient_DCardPutDataEx");
		NetClient_pfNetClient_DCardStopPlayEx=    (pfNetClient_DCardStopPlayEx)GetProcAddress(hNVSSDK, "NetClient_DCardStopPlayEx");
		//NetClient_DSMSetDirectoryServer  =    (pfNetClient_DSMSetDirectoryServer)GetProcAddress(hNVSSDK, "NetClient_DSMSetDirectoryServer");
		NetClient_ProxyGetEncoderList    =    (pfNetClient_ProxyGetEncoderList)GetProcAddress(hNVSSDK, "NetClient_ProxyGetEncoderList");
		NetClient_ProxyGetEncoderInfo    =    (pfNetClient_ProxyGetEncoderInfo)GetProcAddress(hNVSSDK, "NetClient_ProxyGetEncoderInfo");
		NetClient_SetNVS                 =    (pfNetClient_SetNVS)GetProcAddress(hNVSSDK, "NetClient_SetNVS");
		NetClient_DCardStartPlayAudio    =    (pfNetClient_DCardStartPlayAudio)GetProcAddress(hNVSSDK, "NetClient_DCardStartPlayAudio");
		NetClient_DCardStopPlayAudio     =    (pfNetClient_DCardStopPlayAudio)GetProcAddress(hNVSSDK, "NetClient_DCardStopPlayAudio");
		NetClient_LogFileSetProperty     =    (pfNetClient_LogFileSetProperty)GetProcAddress(hNVSSDK, "NetClient_LogFileSetProperty");
		NetClient_LogFileGetProperty     =    (pfNetClient_LogFileGetProperty)GetProcAddress(hNVSSDK, "NetClient_LogFileGetProperty");
		NetClient_LogFileDownload        =    (pfNetClient_LogFileDownload)GetProcAddress(hNVSSDK, "NetClient_LogFileDownload");
		NetClient_LogFileClear           =    (pfNetClient_LogFileClear)GetProcAddress(hNVSSDK, "NetClient_LogFileClear");
		NetClient_LogFileGetDetails      =    (pfNetClient_LogFileGetDetails)GetProcAddress(hNVSSDK, "NetClient_LogFileGetDetails");



		NetClient_SetOsdDiaphaneity      =    (pfNetClient_SetOsdDiaphaneity)GetProcAddress(hNVSSDK, "NetClient_SetOsdDiaphaneity");
		NetClient_GetOsdDiaphaneity      =    (pfNetClient_GetOsdDiaphaneity)GetProcAddress(hNVSSDK, "NetClient_GetOsdDiaphaneity");
		NetClient_Set3DEnable            =    (pfNetClient_Set3DEnable)GetProcAddress(hNVSSDK, "NetClient_Set3DEnable");
		NetClient_Get3DEnable            =    (pfNetClient_Get3DEnable)GetProcAddress(hNVSSDK, "NetClient_Get3DEnable");
		NetClient_NetFileDelFile         =    (pfNetClient_NetFileDelFile)GetProcAddress(hNVSSDK, "NetClient_NetFileDelFile");
		NetClient_DiskSetUsage           =    (pfNetClient_DiskSetUsage)GetProcAddress(hNVSSDK, "NetClient_DiskSetUsage");
		NetClient_DiskFormat             =    (pfNetClient_DiskFormat)GetProcAddress(hNVSSDK, "NetClient_DiskFormat");
		NetClient_DiskPart               =    (pfNetClient_DiskPart)GetProcAddress(hNVSSDK, "NetClient_DiskPart");
		NetClient_BackupKernel           =    (pfNetClient_BackupKernel)GetProcAddress(hNVSSDK, "NetClient_BackupKernel");
		NetClient_SetMotionAreaEnable	=	(pfNetClient_SetMotionAreaEnable)GetProcAddress(hNVSSDK,	 "NetClient_SetMotionAreaEnable");
		NetClient_SetUPNPEnable			=	(pfNetClient_SetUPNPEnable)GetProcAddress(hNVSSDK, "NetClient_SetUPNPEnable");
		NetClient_GetUPNPEnable			=	(pfNetClient_GetUPNPEnable)GetProcAddress(hNVSSDK, "NetClient_GetUPNPEnable");
		NetClient_GetSysInfo			=	(pfNetClient_GetSysInfo)GetProcAddress(hNVSSDK, "NetClient_GetSysInfo");
		NetClient_SetWifiDHCPParam		=	(pfNetClient_SetWifiDHCPParam)GetProcAddress(hNVSSDK, "NetClient_SetWifiDHCPParam");
		NetClient_GetWifiDHCPParam		=	(pfNetClient_GetWifiDHCPParam)GetProcAddress(hNVSSDK, "NetClient_GetWifiDHCPParam");
		NetClient_SetDDNSPara			=	(pfNetClient_SetDDNSPara)GetProcAddress(hNVSSDK, "NetClient_SetDDNSPara");
		NetClient_GetDDNSPara			=	(pfNetClient_GetDDNSPara)GetProcAddress(hNVSSDK, "NetClient_GetDDNSPara");
		NetClient_GetProductType		=	(pfNetClient_GetProductType)GetProcAddress(hNVSSDK, "NetClient_GetProductType");
		NetClient_GetProductTypeEx		=	(pfNetClient_GetProductTypeEx)GetProcAddress(hNVSSDK, "NetClient_GetProductTypeEx");
		NetClient_SetVideoparaSchedule	=	(pfNetClient_SetVideoparaSchedule)GetProcAddress(hNVSSDK, "NetClient_SetVideoparaSchedule");
		NetClient_GetVideoparaSchedule	=	(pfNetClient_GetVideoparaSchedule)GetProcAddress(hNVSSDK, "NetClient_GetVideoparaSchedule");
		NetClient_SetMotionDetectSchedule	=	(pfNetClient_SetMotionDetectSchedule)GetProcAddress(hNVSSDK, "NetClient_SetMotionDetectSchedule");
		NetClient_GetMotionDetectSchedule	=	(pfNetClient_GetMotionDetectSchedule)GetProcAddress(hNVSSDK, "NetClient_GetMotionDetectSchedule");
		NetClient_SetOutportAlarmDelay	=	(pfNetClient_SetOutportAlarmDelay)GetProcAddress(hNVSSDK, "NetClient_SetOutportAlarmDelay");
		NetClient_GetOutportAlarmDelay	=	(pfNetClient_GetOutportAlarmDelay)GetProcAddress(hNVSSDK, "NetClient_GetOutportAlarmDelay");
		NetClient_SetInportAlarmSchedule=	(pfNetClient_SetInportAlarmSchedule)GetProcAddress(hNVSSDK, "NetClient_SetInportAlarmSchedule");
		NetClient_GetInportAlarmSchedule=	(pfNetClient_GetInportAlarmSchedule)GetProcAddress(hNVSSDK, "NetClient_GetInportAlarmSchedule");
		NetClient_SetVideoLostSchedule	=	(pfNetClient_SetVideoLostSchedule)GetProcAddress(hNVSSDK, "NetClient_SetVideoLostSchedule");
		NetClient_GetVideoLostSchedule	=	(pfNetClient_GetVideoLostSchedule)GetProcAddress(hNVSSDK, "NetClient_GetVideoLostSchedule");
		NetClient_SetMotionDecLinkRec	=	(pfNetClient_SetMotionDecLinkRec)GetProcAddress(hNVSSDK, "NetClient_SetMotionDecLinkRec");
		NetClient_GetMotionDecLinkRec	=	(pfNetClient_GetMotionDecLinkRec)GetProcAddress(hNVSSDK, "NetClient_GetMotionDecLinkRec");
		NetClient_SetVideoLostLinkRec	=	(pfNetClient_SetVideoLostLinkRec)GetProcAddress(hNVSSDK, "NetClient_SetVideoLostLinkRec");
		NetClient_GetVideoLostLinkRec	=	(pfNetClient_GetVideoLostLinkRec)GetProcAddress(hNVSSDK, "NetClient_GetVideoLostLinkRec");
		NetClient_SetInportAlarmLinkRec	=	(pfNetClient_SetInportAlarmLinkRec)GetProcAddress(hNVSSDK, "NetClient_SetInportAlarmLinkRec");
		NetClient_GetInportAlarmLinkRec	=	(pfNetClient_GetInportAlarmLinkRec)GetProcAddress(hNVSSDK, "NetClient_GetInportAlarmLinkRec");
		NetClient_SetOutportAlarmSchedule	=	(pfNetClient_SetOutportAlarmSchedule)GetProcAddress(hNVSSDK, "NetClient_SetOutportAlarmSchedule");
		NetClient_GetOutportAlarmSchedule	=	(pfNetClient_GetOutportAlarmSchedule)GetProcAddress(hNVSSDK, "NetClient_GetOutportAlarmSchedule");
		
		NetClient_SetMotionDecLinkOutport	=	(pfNetClient_SetMotionDecLinkOutport)GetProcAddress(hNVSSDK, "NetClient_SetMotionDecLinkOutport");
		NetClient_GetMotionDecLinkOutport	=	(pfNetClient_GetMotionDecLinkOutport)GetProcAddress(hNVSSDK, "NetClient_GetMotionDecLinkOutport");
		NetClient_SetVideoLostLinkOutport	=	(pfNetClient_SetVideoLostLinkOutport)GetProcAddress(hNVSSDK, "NetClient_SetVideoLostLinkOutport");
		NetClient_GetVideoLostLinkOutport	=	(pfNetClient_GetVideoLostLinkOutport)GetProcAddress(hNVSSDK, "NetClient_GetVideoLostLinkOutport");
		NetClient_SetInportAlmLinkOutport	=	(pfNetClient_SetInportAlmLinkOutport)GetProcAddress(hNVSSDK, "NetClient_SetInportAlmLinkOutport");
		NetClient_GetInportAlmLinkOutport	=	(pfNetClient_GetInportAlmLinkOutport)GetProcAddress(hNVSSDK, "NetClient_GetInportAlmLinkOutport");

		NetClient_SetMotionDecLinkSnap	=	(pfNetClient_SetMotionDecLinkSnap)GetProcAddress(hNVSSDK, "NetClient_SetMotionDecLinkSnap");
		NetClient_GetMotionDecLinkSnap	=	(pfNetClient_GetMotionDecLinkSnap)GetProcAddress(hNVSSDK, "NetClient_GetMotionDecLinkSnap");
		NetClient_SetInportAlarmLinkSnap=	(pfNetClient_SetInportAlarmLinkSnap)GetProcAddress(hNVSSDK, "NetClient_SetInportAlarmLinkSnap");
		NetClient_GetInportAlarmLinkSnap=	(pfNetClient_GetInportAlarmLinkSnap)GetProcAddress(hNVSSDK, "NetClient_GetInportAlarmLinkSnap");
		NetClient_SetVideoLostLinkSnap	=	(pfNetClient_SetVideoLostLinkSnap)GetProcAddress(hNVSSDK, "NetClient_SetVideoLostLinkSnap");
		NetClient_GetVideoLostLinkSnap	=	(pfNetClient_GetVideoLostLinkSnap)GetProcAddress(hNVSSDK, "NetClient_GetVideoLostLinkSnap");
		NetClient_SetInportAlarmLinkPTZ	=	(pfNetClient_SetInportAlarmLinkPTZ)GetProcAddress(hNVSSDK, "NetClient_SetInportAlarmLinkPTZ");
		NetClient_GetInportAlarmLinkPTZ	=	(pfNetClient_GetInportAlarmLinkPTZ)GetProcAddress(hNVSSDK, "NetClient_GetInportAlarmLinkPTZ");
	                                 
		
		NetClient_SetVideoLostLinkPTZ	=	(pfNetClient_SetVideoLostLinkPTZ)GetProcAddress(hNVSSDK, "NetClient_SetVideoLostLinkPTZ");
		NetClient_GetVideoLostLinkPTZ	=	(pfNetClient_GetVideoLostLinkPTZ)GetProcAddress(hNVSSDK, "NetClient_GetVideoLostLinkPTZ");

		NetClient_SetInportEnable		=	(pfNetClient_SetInportEnable)GetProcAddress(hNVSSDK, "NetClient_SetInportEnable");
		NetClient_GetInportEnable		=	(pfNetClient_GetInportEnable)GetProcAddress(hNVSSDK, "NetClient_GetInportEnable");
		NetClient_SetOutportEnable		=	(pfNetClient_SetOutportEnable)GetProcAddress(hNVSSDK, "NetClient_SetOutportEnable");
		NetClient_GetOutportEnable		=	(pfNetClient_GetOutportEnable)GetProcAddress(hNVSSDK, "NetClient_GetOutportEnable");

		NetClient_SetZTEInfo			=	(pfNetClient_SetZTEInfo)GetProcAddress(hNVSSDK, "NetClient_SetZTEInfo");
		NetClient_GetZTEInfo			=	(pfNetClient_GetZTEInfo)GetProcAddress(hNVSSDK, "NetClient_GetZTEInfo");
		NetClient_SetOnlineState		=	(pfNetClient_SetOnlineState)GetProcAddress(hNVSSDK, "NetClient_SetOnlineState");
		NetClient_GetOnlineState		=	(pfNetClient_GetOnlineState)GetProcAddress(hNVSSDK, "NetClient_GetOnlineState");

		NetClient_SetDZInfo				=	(pfNetClient_SetDZInfo)GetProcAddress(hNVSSDK, "NetClient_SetDZInfo");
		NetClient_GetDZInfo				=	(pfNetClient_GetDZInfo)GetProcAddress(hNVSSDK, "NetClient_GetDZInfo");

		NetClient_ClearDisk				=	(pfNetClient_ClearDisk)GetProcAddress(hNVSSDK, "NetClient_ClearDisk");
		NetClient_GetMaxMinorVideoSize	=	(pfNetClient_GetMaxMinorVideoSize)GetProcAddress(hNVSSDK, "NetClient_GetMaxMinorVideoSize");

		NetClient_SetComServer			=	(pfNetClient_SetComServer)GetProcAddress(hNVSSDK, "NetClient_SetComServer");
		NetClient_GetComServer			=	(pfNetClient_GetComServer)GetProcAddress(hNVSSDK, "NetClient_GetComServer");

        NetClient_SetVencType			=	(pfNetClient_SetVencType)GetProcAddress(hNVSSDK, "NetClient_SetVencType");
		NetClient_GetVencType			=	(pfNetClient_GetVencType)GetProcAddress(hNVSSDK, "NetClient_GetVencType");

		NetClient_SetPTZAutoBack		=	(pfNetClient_SetPTZAutoBack)GetProcAddress(hNVSSDK, "NetClient_SetPTZAutoBack");
		NetClient_GetPTZAutoBack		=	(pfNetClient_GetPTZAutoBack)GetProcAddress(hNVSSDK, "NetClient_GetPTZAutoBack");

        NetClient_SetFuncListArray      =   (pfNetClient_SetFuncListArray)GetProcAddress(hNVSSDK,"NetClient_SetFuncListArray");
        NetClient_GetFuncListArray      =   (pfNetClient_GetFuncListArray)GetProcAddress(hNVSSDK,"NetClient_GetFuncListArray");

        NetClient_GetOutportState       =   (pfNetClient_GetOutportState)GetProcAddress(hNVSSDK, "NetClient_GetOutportState");

        NetClient_SetCPUMEMAlarmThreshold = (pfNetClient_SetCPUMEMAlarmThreshold)GetProcAddress(hNVSSDK, "NetClient_SetCPUMEMAlarmThreshold");
        NetClient_GetCPUMEMAlarmThreshold = (pfNetClient_GetCPUMEMAlarmThreshold)GetProcAddress(hNVSSDK, "NetClient_GetCPUMEMAlarmThreshold");

        NetClient_SetBothStreamSame     =   (pfNetClient_SetBothStreamSame)GetProcAddress(hNVSSDK, "NetClient_SetBothStreamSame");

        NetClient_GetInportAlarmLinkSoundDisplay = (pfNetClient_GetInportAlarmLinkSoundDisplay)GetProcAddress(hNVSSDK, "NetClient_GetInportAlarmLinkSoundDisplay");

        NetClient_GetVideoLostLinkSoundDisplay = (pfNetClient_GetVideoLostLinkSoundDisplay)GetProcAddress(hNVSSDK, "NetClient_GetVideoLostLinkSoundDisplay");

        NetClient_GetMotionDecLinkSoundDisplay = (pfNetClient_GetMotionDecLinkSoundDisplay)GetProcAddress(hNVSSDK, "NetClient_GetMotionDecLinkSoundDisplay");

        NetClient_SetInportAlarmLinkSoundDisplay = (pfNetClient_SetInportAlarmLinkSoundDisplay)GetProcAddress(hNVSSDK, "NetClient_SetInportAlarmLinkSoundDisplay");

        NetClient_SetMotionDecLinkSoundDisplay  = (pfNetClient_SetMotionDecLinkSoundDisplay)GetProcAddress(hNVSSDK, "NetClient_SetMotionDecLinkSoundDisplay");

        NetClient_SetVideoLostLinkSoundDisplay  = (pfNetClient_SetVideoLostLinkSoundDisplay)GetProcAddress(hNVSSDK, "NetClient_SetVideoLostLinkSoundDisplay");

        NetClient_SetInportAlarmLinkSoundDisplay = (pfNetClient_SetInportAlarmLinkSoundDisplay)GetProcAddress(hNVSSDK, "NetClient_SetInportAlarmLinkSoundDisplay");

        NetClient_SetMotionDecLinkSoundDisplay = (pfNetClient_SetMotionDecLinkSoundDisplay)GetProcAddress(hNVSSDK, "NetClient_SetMotionDecLinkSoundDisplay");

        NetClient_SetVideoLostLinkSoundDisplay  = (pfNetClient_SetVideoLostLinkSoundDisplay)GetProcAddress(hNVSSDK, "NetClient_SetVideoLostLinkSoundDisplay");

        NetClient_Get3GDeviceStatus  = (pfNetClient_Get3GDeviceStatus)GetProcAddress(hNVSSDK, "NetClient_Get3GDeviceStatus");
        NetClient_Set3GDialog  = (pfNetClient_Set3GDialog)GetProcAddress(hNVSSDK, "NetClient_Set3GDialog");
        NetClient_Get3GDialog  = (pfNetClient_Get3GDialog)GetProcAddress(hNVSSDK, "NetClient_Get3GDialog");
        NetClient_Set3GMessage  = (pfNetClient_Set3GMessage)GetProcAddress(hNVSSDK, "NetClient_Set3GMessage");
        NetClient_Get3GMessage  = (pfNetClient_Get3GMessage)GetProcAddress(hNVSSDK, "NetClient_Get3GMessage");
        NetClient_Set3GTaskSchedule  = (pfNetClient_Set3GTaskSchedule)GetProcAddress(hNVSSDK, "NetClient_Set3GTaskSchedule");
        NetClient_Get3GTaskSchedule  = (pfNetClient_Get3GTaskSchedule)GetProcAddress(hNVSSDK, "NetClient_Get3GTaskSchedule");
        NetClient_Set3GNotify  = (pfNetClient_Set3GNotify)GetProcAddress(hNVSSDK, "NetClient_Set3GNotify");
        NetClient_Get3GNotify  = (pfNetClient_Get3GNotify)GetProcAddress(hNVSSDK, "NetClient_Get3GNotify");
        NetClient_Set3GVPND  = (pfNetClient_Set3GVPND)GetProcAddress(hNVSSDK, "NetClient_Set3GVPND");
        NetClient_Get3GVPND  = (pfNetClient_Get3GVPND)GetProcAddress(hNVSSDK, "NetClient_Get3GVPND");

        NetClient_SetHDCamer  = (pfNetClient_SetHDCamer)GetProcAddress(hNVSSDK, "NetClient_SetHDCamer");
        NetClient_GetHDCamer  = (pfNetClient_GetHDCamer)GetProcAddress(hNVSSDK, "NetClient_GetHDCamer");

        NetClient_GetComPortCounts = (pfNetClient_GetComPortCounts)GetProcAddress(hNVSSDK, "NetClient_GetComPortCounts");
        NetClient_GetDownloadFailedFileName = (pfNetClient_GetDownloadFailedFileName)GetProcAddress(hNVSSDK, "NetClient_GetDownloadFailedFileName");

        NetClient_SetHXListenPortInfo  = (pfNetClient_SetHXListenPortInfo)GetProcAddress(hNVSSDK, "NetClient_SetHXListenPortInfo");
        NetClient_GetHXListenPortInfo  = (pfNetClient_GetHXListenPortInfo)GetProcAddress(hNVSSDK, "NetClient_GetHXListenPortInfo");

        NetClient_AdjustVideo = (pfNetClient_AdjustVideo)GetProcAddress(hNVSSDK, "NetClient_AdjustVideo");

        NetClient_ProxyAddProxyEx = (pfNetClient_ProxyAddProxyEx)GetProcAddress(hNVSSDK, "NetClient_ProxyAddProxyEx");

        NetClient_SetPlayerShowFrameMode = (pfNetClient_SetPlayerShowFrameMode)GetProcAddress(hNVSSDK, "NetClient_SetPlayerShowFrameMode");
        NetClient_GetPlayerShowFrameMode = (pfNetClient_GetPlayerShowFrameMode)GetProcAddress(hNVSSDK, "NetClient_GetPlayerShowFrameMode");

        NetClient_SetColorToGray = (pfNetClient_SetColorToGray)GetProcAddress(hNVSSDK, "NetClient_SetColorToGray");
        NetClient_GetColorToGray = (pfNetClient_GetColorToGray)GetProcAddress(hNVSSDK, "NetClient_GetColorToGray");
        NetClient_SetColorParam = (pfNetClient_SetColorParam)GetProcAddress(hNVSSDK, "NetClient_SetColorParam");
        NetClient_GetColorParam = (pfNetClient_GetColorParam)GetProcAddress(hNVSSDK, "NetClient_GetColorParam");
        NetClient_SetCustomChannelName = (pfNetClient_SetCustomChannelName)GetProcAddress(hNVSSDK, "NetClient_SetCustomChannelName");
        NetClient_GetCustomChannelName = (pfNetClient_GetCustomChannelName)GetProcAddress(hNVSSDK, "NetClient_GetCustomChannelName");
        NetClient_SetCustomRecType = (pfNetClient_SetCustomRecType)GetProcAddress(hNVSSDK, "NetClient_SetCustomRecType");
        NetClient_GetCustomRecType = (pfNetClient_GetCustomRecType)GetProcAddress(hNVSSDK, "NetClient_GetCustomRecType");
        NetClient_ChangeSvrIPEx = (pfNetClient_ChangeSvrIPEx)GetProcAddress(hNVSSDK, "NetClient_ChangeSvrIPEx");
        NetClient_GetIpPropertyEx = (pfNetClient_GetIpPropertyEx)GetProcAddress(hNVSSDK, "NetClient_GetIpPropertyEx");
        NetClient_SetFTPUpdate = (pfNetClient_SetFTPUpdate)GetProcAddress(hNVSSDK, "NetClient_SetFTPUpdate");
        NetClient_GetFTPUpdate = (pfNetClient_GetFTPUpdate)GetProcAddress(hNVSSDK, "NetClient_GetFTPUpdate");
        NetClient_SetCHNPTZFormat = (pfNetClient_SetCHNPTZFormat)GetProcAddress(hNVSSDK, "NetClient_SetCHNPTZFormat");
        NetClient_GetCHNPTZFormat = (pfNetClient_GetCHNPTZFormat)GetProcAddress(hNVSSDK, "NetClient_GetCHNPTZFormat");
        NetClient_SetOSDTypeColor = (pfNetClient_SetOSDTypeColor)GetProcAddress(hNVSSDK, "NetClient_SetOSDTypeColor");
        NetClient_GetOSDTypeColor = (pfNetClient_GetOSDTypeColor)GetProcAddress(hNVSSDK, "NetClient_GetOSDTypeColor");
        NetClient_GetServerVersionEx = (pfNetClient_GetServerVersionEx)GetProcAddress(hNVSSDK, "NetClient_GetServerVersionEx");
        NetClient_GetExceptionMsg = (pfNetClient_GetExceptionMsg)GetProcAddress(hNVSSDK, "NetClient_GetExceptionMsg");

        NetClient_GetTalkingState        = (pfNetClient_GetTalkingState)GetProcAddress(hNVSSDK,"NetClient_GetTalkingState");
        //------about multi monitor
        NetClient_GetMonitorNum        = (pfNetClient_GetMonitorNum)GetProcAddress(hNVSSDK,"NetClient_GetMonitorNum");
        //NetClient_GetMonitorInfo        = (pfNetClient_GetMonitorInfo)GetProcAddress(hNVSSDK,"NetClient_GetMonitorInfo");//4.0todo
        NetClient_ChangeMonitor        = (pfNetClient_ChangeMonitor)GetProcAddress(hNVSSDK,"NetClient_ChangeMonitor");
        //NetClient_UpdateMonitor        = (pfNetClient_UpdateMonitor)GetProcAddress(hNVSSDK,"NetClient_UpdateMonitor");
        NetClient_GetInfoByConnectID   = (pfNetClient_GetInfoByConnectID)GetProcAddress(hNVSSDK,"NetClient_GetInfoByConnectID");

        NetClient_InterTalkStart = (pfNetClient_InterTalkStart)GetProcAddress(hNVSSDK, "NetClient_InterTalkStart");
        NetClient_InterTalkEnd = (pfNetClient_InterTalkEnd)GetProcAddress(hNVSSDK, "NetClient_InterTalkEnd");

        NetClient_NetFileDownloadByTimeSpan = (pfNetClient_NetFileDownloadByTimeSpan)GetProcAddress(hNVSSDK, "NetClient_NetFileDownloadByTimeSpan");
        NetClient_NetFileDownloadByTimeSpanEx = (pfNetClient_NetFileDownloadByTimeSpanEx)GetProcAddress(hNVSSDK, "NetClient_NetFileDownloadByTimeSpanEx");

        NetClient_SetNetFileDownloadFileCallBack = (pfNetClient_SetNetFileDownloadFileCallBack)GetProcAddress(hNVSSDK, "NetClient_SetNetFileDownloadFileCallBack");

        NetClient_NetLogQuery = (pfNetClient_NetLogQuery)GetProcAddress(hNVSSDK, "NetClient_NetLogQuery");
        NetClient_NetLogGetLogfile = (pfNetClient_NetLogGetLogfile)GetProcAddress(hNVSSDK, "NetClient_NetLogGetLogfile");
        NetClient_NetLogGetLogCount = (pfNetClient_NetLogGetLogCount)GetProcAddress(hNVSSDK, "NetClient_NetLogGetLogCount");

        NetClient_GetProtocolList = (pfNetClient_GetProtocolList)GetProcAddress(hNVSSDK, "NetClient_GetProtocolList");
        NetClient_NetFileQueryEx = (pfNetClient_NetFileQueryEx)GetProcAddress(hNVSSDK, "NetClient_NetFileQueryEx");
        NetClient_ControlDeviceRecord = (pfNetClient_ControlDeviceRecord)GetProcAddress(hNVSSDK, "NetClient_ControlDeviceRecord");

        NetClient_SetCHNPTZCRUISE = (pfNetClient_SetCHNPTZCRUISE)GetProcAddress(hNVSSDK, "NetClient_SetCHNPTZCRUISE");
        NetClient_GetCHNPTZCRUISE = (pfNetClient_GetCHNPTZCRUISE)GetProcAddress(hNVSSDK, "NetClient_GetCHNPTZCRUISE");
        NetClient_SetVIDEOCOVER_LINKRECORD = (pfNetClient_SetVIDEOCOVER_LINKRECORD)GetProcAddress(hNVSSDK, "NetClient_SetVIDEOCOVER_LINKRECORD");
        NetClient_GetVIDEOCOVER_LINKRECORD = (pfNetClient_GetVIDEOCOVER_LINKRECORD)GetProcAddress(hNVSSDK, "NetClient_GetVIDEOCOVER_LINKRECORD");
        NetClient_SetVIDEOCOVER_LINKPTZ = (pfNetClient_SetVIDEOCOVER_LINKPTZ)GetProcAddress(hNVSSDK, "NetClient_SetVIDEOCOVER_LINKPTZ");
        NetClient_GetVIDEOCOVER_LINKPTZ = (pfNetClient_GetVIDEOCOVER_LINKPTZ)GetProcAddress(hNVSSDK, "NetClient_GetVIDEOCOVER_LINKPTZ");

        NetClient_SetVideoCoverSchedule = (pfNetClient_SetVideoCoverSchedule)GetProcAddress(hNVSSDK, "NetClient_SetVideoCoverSchedule");
        NetClient_GetVideoCoverSchedule = (pfNetClient_GetVideoCoverSchedule)GetProcAddress(hNVSSDK, "NetClient_GetVideoCoverSchedule");

        NetClient_EZoomAdd = (pfNetClient_EZoomAdd)GetProcAddress(hNVSSDK, "NetClient_EZoomAdd");
        NetClient_EZoomSet = (pfNetClient_EZoomSet)GetProcAddress(hNVSSDK, "NetClient_EZoomSet");
//        NetClient_EZoomReset = (pfNetClient_EZoomReset)GetProcAddress(hNVSSDK, "NetClient_EZoomReset");
        NetClient_EZoomRemove = (pfNetClient_EZoomRemove)GetProcAddress(hNVSSDK, "NetClient_EZoomRemove");

        NetClient_SetFTPUsage = (pNetClient_SetFTPUsage)GetProcAddress(hNVSSDK, "NetClient_SetFTPUsage");
        NetClient_GetFTPUsage = (pNetClient_GetFTPUsage)GetProcAddress(hNVSSDK, "NetClient_GetFTPUsage");

        NetClient_DrawPolyOnLocalVideo = (pfNetClient_DrawPolyOnLocalVideo)GetProcAddress(hNVSSDK, "NetClient_DrawPolyOnLocalVideo");
        NetClient_GetVideoParam = (pfNetClient_GetVideoParam)GetProcAddress(hNVSSDK, "NetClient_GetVideoParam");

        NetClient_VCASetConfig = (pfNetClient_VCASetConfig)GetProcAddress(hNVSSDK, "NetClient_VCASetConfig");
        NetClient_VCAGetConfig = (pfNetClient_VCAGetConfig)GetProcAddress(hNVSSDK, "NetClient_VCAGetConfig");
        NetClient_VCARestart = (pfNetClient_VCARestart)GetProcAddress(hNVSSDK, "NetClient_VCARestart");
        NetClient_VCAGetAlarmInfo = (pfNetClient_VCAGetAlarmInfo)GetProcAddress(hNVSSDK, "NetClient_VCAGetAlarmInfo");

        NetClient_SetEmailAlarmEx = (pfNetClient_SetEmailAlarmEx)GetProcAddress(hNVSSDK, "NetClient_SetEmailAlarmEx");
        NetClient_GetEmailAlarmEx = (pfNetClient_GetEmailAlarmEx)GetProcAddress(hNVSSDK, "NetClient_GetEmailAlarmEx");

        NetClient_GetFTPUploadConfig = (pfNetClient_GetFTPUploadConfig)GetProcAddress(hNVSSDK, "NetClient_GetFTPUploadConfig");
        NetClient_SetFTPUploadConfig = (pfNetClient_SetFTPUploadConfig)GetProcAddress(hNVSSDK, "NetClient_SetFTPUploadConfig");

        NetClient_GetHDCamerEx = (pfNetClient_GetHDCamerEx)GetProcAddress(hNVSSDK, "NetClient_GetHDCamerEx");
        NetClient_SetHDCamerEx = (pfNetClient_SetHDCamerEx)GetProcAddress(hNVSSDK, "NetClient_SetHDCamerEx");

        NetClient_Set3GConfig = (pfNetClient_Set3GConfig)GetProcAddress(hNVSSDK, "NetClient_Set3GConfig");
        NetClient_Get3GConfig = (pfNetClient_Get3GConfig)GetProcAddress(hNVSSDK, "NetClient_Get3GConfig");

        NetClient_SetCDBurnConfig = (pfNetClient_SetCDBurnConfig)GetProcAddress(hNVSSDK, "NetClient_SetCDBurnConfig");
        NetClient_GetCDBurnConfig = (pfNetClient_GetCDBurnConfig)GetProcAddress(hNVSSDK, "NetClient_GetCDBurnConfig");
        NetClient_SetVideoEncrypt = (pfNetClient_SetVideoEncrypt)GetProcAddress(hNVSSDK, "NetClient_SetVideoEncrypt");
        NetClient_GetVideoEncrypt = (pfNetClient_GetVideoEncrypt)GetProcAddress(hNVSSDK, "NetClient_GetVideoEncrypt");

        NetClient_SetVideoDecrypt = (pfNetClient_SetVideoDecrypt)GetProcAddress(hNVSSDK, "NetClient_SetVideoDecrypt");
        NetClient_GetVideoDecrypt = (pfNetClient_GetVideoDecrypt)GetProcAddress(hNVSSDK, "NetClient_GetVideoDecrypt");

        NetClient_SetPreRecEnable = (pfNetClient_SetPreRecEnable)GetProcAddress(hNVSSDK, "NetClient_SetPreRecEnable");
        NetClient_GetPreRecEnable = (pfNetClient_GetPreRecEnable)GetProcAddress(hNVSSDK, "NetClient_GetPreRecEnable");

        NetClient_SetITSExtraInfo = (pfNetClient_SetITSExtraInfo)GetProcAddress(hNVSSDK, "NetClient_SetITSExtraInfo");
        NetClient_GetITSExtraInfo = (pfNetClient_GetITSExtraInfo)GetProcAddress(hNVSSDK, "NetClient_GetITSExtraInfo");

		NetClient_SetATMConfig = (pfNetClient_SetATMConfig)GetProcAddress(hNVSSDK, "NetClient_SetATMConfig");//ATM
		NetClient_GetATMConfig = (pfNetClient_GetATMConfig)GetProcAddress(hNVSSDK, "NetClient_GetATMConfig");//ATM

		//add by zhy 2013.04.06&2013.04.08
		NetClient_SetDiskGroup = (pfNetClient_SetDiskGroup)GetProcAddress(hNVSSDK, "NetClient_SetDiskGroup");
		NetClient_GetDiskGroup = (pfNetClient_GetDiskGroup)GetProcAddress(hNVSSDK, "NetClient_GetDiskGroup");
		NetClient_SetDiskQuota = (pfNetClient_SetDiskQuota)GetProcAddress(hNVSSDK, "NetClient_SetDiskQuota");
		NetClient_GetDiskQuotaState = (pfNetClient_GetDiskQuotaState)GetProcAddress(hNVSSDK, "NetClient_GetDiskQuotaState");
		NetClient_ModifyUserAuthority = (pfNetClient_ModifyUserAuthority)GetProcAddress(hNVSSDK, "NetClient_ModifyUserAuthority");
		NetClient_GetUserAuthority = (pfNetClient_GetUserAuthority)GetProcAddress(hNVSSDK, "NetClient_GetUserAuthority");
		NetClient_GetGroupAuthority = (pfNetClient_GetGroupAuthority)GetProcAddress(hNVSSDK, "NetClient_GetGroupAuthority");
		NetClient_SetHolidayPlan = (pfNetClient_SetHolidayPlan)GetProcAddress(hNVSSDK, "NetClient_SetHolidayPlan");
		NetClient_GetHolidayPlan = (pfNetClient_GetHolidayPlan)GetProcAddress(hNVSSDK, "NetClient_GetHolidayPlan");
		NetClient_SetCommonEnable = (pfNetClient_SetCommonEnable)GetProcAddress(hNVSSDK, "NetClient_SetCommonEnable");
		NetClient_GetCommonEnable = (pfNetClient_GetCommonEnable)GetProcAddress(hNVSSDK, "NetClient_GetCommonEnable");
		//add by zhy end

        EXPORT_FUNCTION(hNVSSDK, NetClient_SetDigitalChannelConfig);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetDigitalChannelConfig);
        EXPORT_FUNCTION(hNVSSDK, NetClient_DigitalChannelSend);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetVideoNPModeEx);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetVideoNPModeEx);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetVideoModeMethod);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetVideoModeMethod);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetChannelSipConfig);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetChannelSipConfig);
        //EXPORT_FUNCTION(hNVSSDK, NetClient_SetPsPackCallBack);//4.0todo
        EXPORT_FUNCTION(hNVSSDK, NetClient_StartRecvEx);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetVideoCombine);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetVideoCombine);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetConnectInfo);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetPlatformApp);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetPlatformApp);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetDigitalChannelNum);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetITSTimeRange);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetITSTimeRange);
      /*  EXPORT_FUNCTION(hNVSSDK, NetClient_SetITSTimeRangeParam);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetITSTimeRangeParam);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetITSAgcBlockEnable);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetITSAgcBlockEnable);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetITSFlashLampEnable);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetITSFlashLampEnable);     */
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetITSDetectMode);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetITSDetectMode);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetITSBlock);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetITSBlock);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetITSLoopMode);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetITSLoopMode);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetITSDeviceType);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetITSDeviceType);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetITSSwitchTime);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetITSSwitchTime);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetJPEGQuality);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetJPEGQuality);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetOSDTypeFontSize);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetOSDTypeFontSize);
        EXPORT_FUNCTION(hNVSSDK, NetClient_VCARestartEx);
		EXPORT_FUNCTION(hNVSSDK, NetClient_SetChannelProperty);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetChannelProperty);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetITSRoadwayParam);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetITSRoadwayParam);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetITSLicensePlateOptimize);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetITSLicensePlateOptimize);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetHDTimeRangeParam);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetHDTimeRangeParam);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetITSTimeRangeEnable);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetITSTimeRangeEnable);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetHDTemplateName);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetHDTemplateName);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetHDTemplateMap);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetHDTemplateMap);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetITSRecoParam);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetITSRecoParam);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetITSDayNight);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetITSDayNight);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetITSCamLocation);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetITSCamLocation);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetITSWorkMode);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetITSWorkMode);
        EXPORT_FUNCTION(hNVSSDK, NetClient_LogonEx);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetTimeZone);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetTimeZone);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetCurLanguage);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetCurLanguage);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetLanguageList);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetChannelEncodeProfile);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetChannelEncodeProfile);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetExceptionHandleParam);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetExceptionHandleParam);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetAlarmClear);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetCharSet);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetImgDisposal);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetImgDisposal);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetWaterMarkEnable);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetWaterMarkEnable);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetSystemType);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetSystemType);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetEncryptSN);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetSNReg);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetAlarmLink_V1);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetAlarmLink_V1);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetCameraParam);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetCameraParam);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetITSLightInfo);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetITSLightInfo);
        //EXPORT_FUNCTION(hNVSSDK, NetClient_GetServerVersion_V1);//4.0todo
        EXPORT_FUNCTION(hNVSSDK, NetClient_CheckDeviceState);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetDeviceState);
		EXPORT_FUNCTION(hNVSSDK, NetClient_SetVerticalSync);
		EXPORT_FUNCTION(hNVSSDK, NetClient_GetVerticalSync);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetLocalAudioVolumeEx);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetLocalAudioVolumeEx);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetMuted);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetLanParam);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetLanParam);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetPWMValue);   //120213
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetPWMValue);
        EXPORT_FUNCTION(hNVSSDK, NetClient_ShutDownDev);   //120224
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetVideoSzList);
        EXPORT_FUNCTION(hNVSSDK, NetClient_ClearPolyLocalVideo);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetAlarmConfig);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetAlarmConfig);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetITSSwitchInterval);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetITSSwitchInterval);
        EXPORT_FUNCTION(hNVSSDK, NetClient_NetFileSetChannelParam);
        EXPORT_FUNCTION(hNVSSDK, NetClient_NetFileGetChannelParam);
        EXPORT_FUNCTION(hNVSSDK, NetClient_NetFileGetDiskInfoEx);
        EXPORT_FUNCTION(hNVSSDK, NetClient_InterTalkStartEx);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetAudioSample);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetAudioSample);
        EXPORT_FUNCTION(hNVSSDK, NetClient_Startup_V4);
		EXPORT_FUNCTION(hNVSSDK, NetClient_SetNotifyFunction_V4);
		EXPORT_FUNCTION(hNVSSDK, NetClient_SetAlarmNotify_V5);
        EXPORT_FUNCTION(hNVSSDK, NetClient_StartRecv_V4);
		EXPORT_FUNCTION(hNVSSDK, NetClient_StartRecv_V5);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetNotifyUserData_V4);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetComRecvNotify_V4);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetHTTPPort_V4);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetHTTPPort_V4);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetDomainParsePara_V4);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetDomainParsePara_V4);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetBitrateOnVideo_V4);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetDevInfo);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetComFormat_V2);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetComFormat_V2);
		EXPORT_FUNCTION(hNVSSDK, NetClient_ATMQueryFile);
		EXPORT_FUNCTION(hNVSSDK, NetClient_ATMGetQueryFile);
		EXPORT_FUNCTION(hNVSSDK, NetClient_GetServerVersion_V1);
		EXPORT_FUNCTION(hNVSSDK, NetClient_SetFullStreamNotify_V4);
		EXPORT_FUNCTION(hNVSSDK, NetClient_NetFileLockFile);//文件加锁
		EXPORT_FUNCTION(hNVSSDK, NetClient_NetFileGetQueryfileEx);
		EXPORT_FUNCTION(hNVSSDK, NetClient_GetOsdTextEx);
		EXPORT_FUNCTION(hNVSSDK, NetClient_NetFileDownload);
		EXPORT_FUNCTION(hNVSSDK, NetClient_Upgrade_V4);//文件升级统一为一个接口，解决不兼容问题
		EXPORT_FUNCTION(hNVSSDK, NetClient_SetStreamInsertData);
		EXPORT_FUNCTION(hNVSSDK, NetClient_GetStreamInsertData);
		EXPORT_FUNCTION(hNVSSDK, NetClient_GetOtherID);
		EXPORT_FUNCTION(hNVSSDK, NetClient_SetDomePTZ);
		EXPORT_FUNCTION(hNVSSDK, NetClient_GetDomePTZ);
		EXPORT_FUNCTION(hNVSSDK, NetClient_GetUserDataInfo);
        EXPORT_FUNCTION(hNVSSDK, NetClient_GetBroadcastMessage);
		EXPORT_FUNCTION(hNVSSDK, NetClient_SetDevConfig);
		EXPORT_FUNCTION(hNVSSDK, NetClient_GetDevConfig);
		EXPORT_FUNCTION(hNVSSDK, NetClient_SetJEPGSize);
		EXPORT_FUNCTION(hNVSSDK, NetClient_GetJEPGSize);
		EXPORT_FUNCTION(hNVSSDK, NetClient_QueryDevStatus);
		EXPORT_FUNCTION(hNVSSDK, NetClient_GetDevStatus);
		EXPORT_FUNCTION(hNVSSDK, NetClient_SendCommand);
		EXPORT_FUNCTION(hNVSSDK, NetClient_RecvCommand);
		EXPORT_FUNCTION(hNVSSDK, NetClient_SetDsmConfig);
		EXPORT_FUNCTION(hNVSSDK, NetClient_GetDsmRegstierInfo);
		EXPORT_FUNCTION(hNVSSDK, NetClient_Logon_V4);
		EXPORT_FUNCTION(hNVSSDK, NetClient_SetDevDiskConfig);
		EXPORT_FUNCTION(hNVSSDK, NetClient_GetDevDiskConfig);
		EXPORT_FUNCTION(hNVSSDK, NetClient_GetPseChInfo);
		EXPORT_FUNCTION(hNVSSDK, NetClient_SetPseChProperty);
		EXPORT_FUNCTION(hNVSSDK, NetClient_GetPseChProperty);
		EXPORT_FUNCTION(hNVSSDK, NetClient_ChannelTalkStart);
		EXPORT_FUNCTION(hNVSSDK, NetClient_ChannelTalkEnd);
		EXPORT_FUNCTION(hNVSSDK, NetClient_InputChannelTalkingdata);
		EXPORT_FUNCTION(hNVSSDK, NetClient_GetChannelTalkingState);
		EXPORT_FUNCTION(hNVSSDK, NetClient_RebootDeviceByType);
		EXPORT_FUNCTION(hNVSSDK, NetClient_SetSDKWorkMode);
		EXPORT_FUNCTION(hNVSSDK, NetClient_StartDownload);	
		EXPORT_FUNCTION(hNVSSDK, NetClient_StopDownload);	
		EXPORT_FUNCTION(hNVSSDK, NetClient_GetDownloadPos);	
		EXPORT_FUNCTION(hNVSSDK, NetClient_PlayBack);
		EXPORT_FUNCTION(hNVSSDK, NetClient_StopPlayBack);
		EXPORT_FUNCTION(hNVSSDK, NetClient_PlayBackControl);
		EXPORT_FUNCTION(hNVSSDK, NetClient_SetDomeAdvParam);
		EXPORT_FUNCTION(hNVSSDK, NetClient_GetDomeAdvParam);
		EXPORT_FUNCTION(hNVSSDK, NetClient_GetHDTemplateIndex);
		EXPORT_FUNCTION(hNVSSDK, NetClient_InnerAutoTest);
		EXPORT_FUNCTION(hNVSSDK, NetClient_SetDevUserDataNotify);
		EXPORT_FUNCTION(hNVSSDK, NetClient_Query_V4);
		EXPORT_FUNCTION(hNVSSDK, NetClient_GetQueryResult_V4);
		EXPORT_FUNCTION(hNVSSDK, NetClient_CheckCamera);
		EXPORT_FUNCTION(hNVSSDK, NetClient_RegisterDrawFun);
		EXPORT_FUNCTION(hNVSSDK, NetClient_GetRecvInfoById);
		EXPORT_FUNCTION(hNVSSDK, NetClient_SetDataPackCallBack);
		EXPORT_FUNCTION(hNVSSDK, NetClient_Query_V5);
		EXPORT_FUNCTION(hNVSSDK, NetClient_FaceConfig);
		EXPORT_FUNCTION(hNVSSDK, NetClient_SetUnipueAlertConfig);
		EXPORT_FUNCTION(hNVSSDK, NetClient_GetUnipueAlertConfig);
		EXPORT_FUNCTION(hNVSSDK, NetClient_CmdConfig);
		EXPORT_FUNCTION(hNVSSDK, NetClient_GetLastError);
		EXPORT_FUNCTION(hNVSSDK, NetClient_SyncLogon);
		EXPORT_FUNCTION(hNVSSDK, NetClient_SyncRealPlay);
		EXPORT_FUNCTION(hNVSSDK, NetClient_StopRealPlay);
		EXPORT_FUNCTION(hNVSSDK, NetClient_SyncQuery);
		EXPORT_FUNCTION(hNVSSDK, NetClient_SyncSetDevCfg);
		EXPORT_FUNCTION(hNVSSDK, NetClient_SetZFConfig);
		EXPORT_FUNCTION(hNVSSDK, NetClient_GetZFConfig);
        EXPORT_FUNCTION(hNVSSDK, NetClient_SetExternDevLogonInfo);
		EXPORT_FUNCTION(hNVSSDK, NetClient_SetAVMode);
		EXPORT_FUNCTION(hNVSSDK, NetClient_CapturePicByDevice);
		EXPORT_FUNCTION(hNVSSDK, NetClient_GetDevConfig_V5);
        EXPORT_FUNCTION(hNVSSDK, NetClient_ZFConfig);
		EXPORT_FUNCTION(hNVSSDK, NetClient_HttpXmlConfig);
        return DLL_LOAD_SUCCESS;
	}

    bool ReleaseNVSSDK()
    {
        if (hNVSSDK)
        {
            ::FreeLibrary(hNVSSDK);
            hNVSSDK = NULL;
        }

        return true;
    }
}
