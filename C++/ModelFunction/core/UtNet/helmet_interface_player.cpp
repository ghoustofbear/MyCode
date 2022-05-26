#include "helmet_interface_player.h"

pfnVARender_Startup fnVARender_Startup       = NULL; 
pfnVARender_Cleanup fnVARender_Cleanup       = NULL;
pfnVARender_Open    fnVARender_Open          = NULL;
pfnVARender_Close   fnVARender_Close         = NULL;
pfnVARender_SetWindow fnVARender_SetWindow   = NULL;
pfnVARender_StartVideo fnVARender_StartVideo = NULL;
pfnVARender_StopVideo  fnVARender_StopVideo  = NULL;
pfnVARender_StartAudio fnVARender_StartAudio = NULL;
pfnVARender_StopAudio  fnVARender_StopAudio  = NULL;
pfnVARender_PumpVideoFrame fnVARender_PumpVideoFrame = NULL;
pfnVARender_PumpAudioFrame fnVARender_PumpAudioFrame = NULL;
pfnVARender_SetPlayMode fnVARender_SetPlayMode = NULL;
pfnVARender_SetSpeed    fnVARender_SetSpeed    = NULL;
pfnVARender_GetLatestPlayVideoTimeStamp fnVARender_GetLatestPlayVideoTimeStamp = NULL;

pfnAudioCapEnc_Startup  fnAudioCapEnc_Startup    = NULL;
pfnAudioCapEnc_Cleanup  fnAudioCapEnc_Cleanup    = NULL;
pfnAudioCapEnc_Open     fnAudioCapEnc_Open       = NULL;
pfnAudioCapEnc_Close    fnAudioCapEnc_Close      = NULL;
pfnAudioCapEnc_ReadFrame fnAudioCapEnc_ReadFrame = NULL;
pfnAudioCapEnc_GetFrameVolume fnAudioCapEnc_GetFrameVolume  = NULL;

#define SUPPORT_JOYOJ_DIR 0

void loadhelmetgxlibrary()
{
	if (NULL == hDllModule)
	{
        hDllModule = LoadLibrary("VARndr.dll");
	}							

	if (NULL != hDllModule)
	{ 
		fnVARender_Startup    = (pfnVARender_Startup)GetProcAddress(hDllModule, "VARender_Startup");
		fnVARender_Cleanup    = (pfnVARender_Cleanup)GetProcAddress(hDllModule, "VARender_Cleanup");
		fnVARender_Open       = (pfnVARender_Open)GetProcAddress(hDllModule, "VARender_Open");	

		fnVARender_Close      = (pfnVARender_Close)GetProcAddress(hDllModule, "VARender_Close");
		fnVARender_SetWindow  = (pfnVARender_SetWindow)GetProcAddress(hDllModule, "VARender_SetWindow");
		fnVARender_StartVideo = (pfnVARender_StartVideo)GetProcAddress(hDllModule, "VARender_StartVideo");
		fnVARender_StopVideo  = (pfnVARender_StopVideo)GetProcAddress(hDllModule, "VARender_StopVideo");
		fnVARender_StartAudio = (pfnVARender_StartAudio)GetProcAddress(hDllModule, "VARender_StartAudio");
		fnVARender_StopAudio  = (pfnVARender_StopAudio)GetProcAddress(hDllModule, "VARender_StopAudio");
		fnVARender_PumpVideoFrame  = (pfnVARender_PumpVideoFrame)GetProcAddress(hDllModule, "VARender_PumpVideoFrame");
		fnVARender_PumpAudioFrame  = (pfnVARender_PumpAudioFrame)GetProcAddress(hDllModule, "VARender_PumpAudioFrame");
		fnVARender_SetPlayMode     = (pfnVARender_SetPlayMode)GetProcAddress(hDllModule, "VARender_SetPlayMode");
		fnVARender_SetSpeed        = (pfnVARender_SetSpeed)GetProcAddress(hDllModule, "VARender_SetSpeed");
		fnVARender_GetLatestPlayVideoTimeStamp = (pfnVARender_GetLatestPlayVideoTimeStamp)GetProcAddress(hDllModule, "VARender_GetLatestPlayVideoTimeStamp");
	
#if SUPPORT_JOYOJ_DIR
		fnVARender_Startup(".\\VideoRunCode\\iCVS2\\OEMCodec\\");	
#else
        fnVARender_Startup(".\\iCVS2\\OEMCodec\\");
#endif
	}
	
	if (NULL == hAudioDllModule)
	{
        hAudioDllModule = LoadLibrary("AudioCapEnc.dll");
	}
	
	if (NULL != hAudioDllModule)
	{
        fnAudioCapEnc_Startup = (pfnAudioCapEnc_Startup)GetProcAddress(hAudioDllModule, "AudioCapEnc_Startup");
        fnAudioCapEnc_Cleanup = (pfnAudioCapEnc_Cleanup)GetProcAddress(hAudioDllModule, "AudioCapEnc_Cleanup");
		fnAudioCapEnc_Open = (pfnAudioCapEnc_Open)GetProcAddress(hAudioDllModule, "AudioCapEnc_Open");
		fnAudioCapEnc_Close = (pfnAudioCapEnc_Close)GetProcAddress(hAudioDllModule, "AudioCapEnc_Close");
		fnAudioCapEnc_ReadFrame = (pfnAudioCapEnc_ReadFrame)GetProcAddress(hAudioDllModule, "AudioCapEnc_ReadFrame");
		fnAudioCapEnc_GetFrameVolume = (pfnAudioCapEnc_GetFrameVolume)GetProcAddress(hAudioDllModule, "AudioCapEnc_GetFrameVolume");

#if SUPPORT_JOYOJ_DIR
		fnAudioCapEnc_Startup(".\\VideoRunCode\\iCVS2\\OEMCodec\\");
#else
        fnAudioCapEnc_Startup(".\\iCVS2\\OEMCodec\\");
#endif
	}
}

void freehelmetgxlibrary()
{	
    if (NULL != hDllModule)
	{
		FreeLibrary(hDllModule);
		hDllModule = NULL;
	}
}