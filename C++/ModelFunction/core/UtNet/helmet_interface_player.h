#ifndef __HELMET_INTF_PLAYER_H__
#define __HELMET_INTF_PLAYER_H__

#include "3rd/helmet/VARecorder.h"
#include "3rd/helmet/VARender.h"
#include "3rd/helmet/AudioDecPlay.h"
#include "3rd/helmet/AudioCapEnc.h"

typedef int (__stdcall *pfnVARender_Startup)(const char *cpszDLLPath);

typedef void (__stdcall *pfnVARender_Cleanup)();

typedef int  (__stdcall *pfnVARender_Open)(VARENDER_HANDLE *phVARender);

typedef void (__stdcall *pfnVARender_Close)(VARENDER_HANDLE hVARender);

typedef int (__stdcall *pfnVARender_EnableHA)(VARENDER_HANDLE hVARender, int bEnable);

typedef void (__stdcall *pfnVARender_EnableRefresh)(VARENDER_HANDLE hVARender, int bEnable);

typedef void (__stdcall *pfnVARender_SetDisplayCallBack)(VARENDER_HANDLE hVARender, VARENDER_DISPLAY_CALLBACK pCallBack, VARENDERUSER_HANDLE hUser);

typedef void (__stdcall *pfnVARender_SetVideoCallBack)(VARENDER_HANDLE hVARender, VARENDER_VIDEO_CALLBACK pCallBack, VARENDERUSER_HANDLE hUser, char cFormat);

typedef void (__stdcall *pfnVARender_SetAudioCallBack)(VARENDER_HANDLE hVARender, VARENDER_AUDIO_CALLBACK pCallBack, VARENDERUSER_HANDLE hUser);

typedef void (__stdcall *pfnVARender_SetAudioLRChannel)(VARENDER_HANDLE hVARender, int iLRChannel);

typedef int (__stdcall *pfnVARender_SetMonitor)(VARENDER_HANDLE hVARender, int iNo, int iX, int iY);

typedef int (__stdcall *pfnVARender_SetWindow)(VARENDER_HANDLE hVARender, VARENDER_HWND hWnd);

typedef void (__stdcall *pfnVARender_SetFixAspectRatio)(VARENDER_HANDLE hVARender, int bFixRatio);

typedef void (__stdcall *pfnVARender_SetDestRect)(VARENDER_HANDLE hVARender, int XDest, int YDest, int nDestWidth, int nDestHeight);

typedef void (__stdcall *pfnVARender_SetSrcRect)(VARENDER_HANDLE hVARender, int XSrc, int YSrc, int nSrcWidth, int nSrcHeight);

typedef int (__stdcall *pfnVARender_SetDDraw)(VARENDER_HANDLE hVARender, int bDDraw);

typedef int (__stdcall *pfnVARender_IsDDraw)(VARENDER_HANDLE hVARender);

typedef void (__stdcall *pfnVARender_StartVideo)(VARENDER_HANDLE hVARender);

typedef void (__stdcall *pfnVARender_StopVideo)(VARENDER_HANDLE hVARender);

typedef void (__stdcall *pfnVARender_StartAudio)(VARENDER_HANDLE hVARender);

typedef void (__stdcall *pfnVARender_StopAudio)(VARENDER_HANDLE hVARender);

typedef int (__stdcall *pfnVARender_PumpVideoFrame)(VARENDER_HANDLE hVARender, char *pFrame, int iLen);

typedef int (__stdcall *pfnVARender_PumpAudioFrame)(VARENDER_HANDLE hVARender, char *pFrame, int iLen);

typedef void (__stdcall *pfnVARender_PumpCallAudioFrame)(VARENDER_HANDLE hVARender, char *pFrame, int iLen);

typedef int (__stdcall *pfnVARender_Snapshot)(VARENDER_HANDLE hVARender, const char *pszFileName);

typedef void (__stdcall *pfnVARender_Refresh)(VARENDER_HANDLE hVARender);

typedef void (__stdcall *pfnVARender_SetSync)(VARENDER_HANDLE hVARender, int bSync);

typedef int (__stdcall *pfnVARender_IsSync)(VARENDER_HANDLE hVARender);

typedef void (__stdcall *pfnVARender_SetPlayMode)(VARENDER_HANDLE hVARender, int iMode);

typedef int (__stdcall *pfnVARender_GetPlayMode)(VARENDER_HANDLE hVARender);

typedef void (__stdcall *pfnVARender_SetSpeed)(VARENDER_HANDLE hVARender, int iSpeed);

typedef int (__stdcall *pfnVARender_GetSpeed)(VARENDER_HANDLE hVARender);

typedef int (__stdcall *pfnVARender_GetVolume)(VARENDER_HANDLE hVARender);

typedef void (__stdcall *pfnVARender_SetVolume)(VARENDER_HANDLE hVARender, int iVolume);

typedef void (__stdcall *pfnVARender_SetBufferTime)(VARENDER_HANDLE hVARender, int iBufferTime);

typedef void (__stdcall *pfnVARender_GetLatestPlayVideoTimeStamp)(VARENDER_HANDLE hVARender, unsigned int *puiTimeStamp);

//ÒôÆµ½Ó¿Ú
typedef int (__stdcall *pfnAudioCapEnc_Startup)(const char *cpszDLLPath);

typedef void (__stdcall *pfnAudioCapEnc_Cleanup)();

typedef int (__stdcall *pfnAudioCapEnc_Open)(int iProducerID, const char *cpszAlgName, unsigned int uiBaseTimeStamp, AUDIOCAPENC_HANDLE *phAudioCapEnc);

typedef void (__stdcall *pfnAudioCapEnc_Close)(AUDIOCAPENC_HANDLE hAudioCapEnc);

typedef int (__stdcall *pfnAudioCapEnc_ReadFrame)(AUDIOCAPENC_HANDLE hAudioCapEnc, char *pFrame, int iLen);

typedef int (__stdcall *pfnAudioCapEnc_GetFrameVolume)(AUDIOCAPENC_HANDLE hAudioCapEnc);

#ifdef _WIN32
#include <windows.h>

static HMODULE hDllModule = NULL;
static HMODULE hAudioDllModule = NULL;

extern pfnVARender_Startup fnVARender_Startup;
extern pfnVARender_Cleanup fnVARender_Cleanup;
extern pfnVARender_Open    fnVARender_Open;
extern pfnVARender_Close   fnVARender_Close;
extern pfnVARender_SetWindow fnVARender_SetWindow;
extern pfnVARender_StartVideo fnVARender_StartVideo;
extern pfnVARender_StopVideo  fnVARender_StopVideo;
extern pfnVARender_StartAudio fnVARender_StartAudio;
extern pfnVARender_StopAudio  fnVARender_StopAudio;
extern pfnVARender_PumpVideoFrame fnVARender_PumpVideoFrame;
extern pfnVARender_PumpAudioFrame fnVARender_PumpAudioFrame;
extern pfnVARender_SetPlayMode fnVARender_SetPlayMode;
extern pfnVARender_SetSpeed    fnVARender_SetSpeed;
extern pfnVARender_GetLatestPlayVideoTimeStamp fnVARender_GetLatestPlayVideoTimeStamp;

extern pfnAudioCapEnc_Startup  fnAudioCapEnc_Startup;
extern pfnAudioCapEnc_Cleanup  fnAudioCapEnc_Cleanup;
extern pfnAudioCapEnc_Open     fnAudioCapEnc_Open;
extern pfnAudioCapEnc_Close    fnAudioCapEnc_Close;
extern pfnAudioCapEnc_ReadFrame fnAudioCapEnc_ReadFrame;
extern pfnAudioCapEnc_GetFrameVolume fnAudioCapEnc_GetFrameVolume;

#endif

void loadhelmetgxlibrary();

void freehelmetgxlibrary();

#endif
