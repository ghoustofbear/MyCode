#ifndef __HELMET_INTERFACE_H__
#define __HELMET_INTERFACE_H__

#include "3rd/helmet/iCVSC_base.h"
#include "3rd/helmet/iCVSC.h"
//
//#if defined _WIN32
//#define NULL 0
//#define ssssstdcall __stdcall
//#else
//#define ssssstdcall
//#endif 
//
//typedef int (ssssstdcall *pfnIC_Initialize)();
//
//typedef int (ssssstdcall *pfnIC_Terminate)(void);
//
//typedef int (ssssstdcall *pfnIC_EnableConvertCharacter)(int enableFlag);
//
//typedef int (ssssstdcall *pfnIC_Open)(const char *pszAddress, const char *pszUsername, const char *pszPassword, const char *pszEpid, IC_HANDLE *hSession);
//
//typedef int (ssssstdcall *pfnIC_Close)(IC_HANDLE hSession);
//
//typedef int (ssssstdcall *pfnIC_ForkPUList)(IC_HANDLE hSession, int nOffset, IC_HANDLE *hPUArray, int *nCount);
//
//typedef int (ssssstdcall *pfnIC_GetResourceName)(IC_HANDLE hResource, IC_STR szName);
//
//typedef int (ssssstdcall *pfnIC_GetResourcePUID)(IC_HANDLE hResource, IC_STR szPUID);
//
//typedef int (ssssstdcall *pfnIC_GetResourceType)(IC_HANDLE hResource, IC_STR szType);
//
//typedef int (ssssstdcall *pfnIC_GetPUOnlineFlag)(IC_HANDLE hResource, int *nOnlineFlag);
//
//typedef int (ssssstdcall *pfnIC_ForkPUResource)(IC_HANDLE hPU, IC_HANDLE *hResourcesArray, int *nArrayCount);
//
//typedef int (ssssstdcall *pfnIC_StartStream)(IC_HANDLE hResource, int nStreamID, IC_HANDLE *hStream);
//
//typedef int (ssssstdcall *pfnIC_StopStream)(IC_HANDLE hStream);
//
//typedef int (ssssstdcall *pfnIC_ReceiveNotify)(IC_HANDLE hSession, IC_STR szID, IC_STR szSrcID, int *nIndex, int *nUTC);
//
//typedef int (ssssstdcall *pfnIC_ReceiveFrame)(IC_HANDLE hStream, char *pFrame, int *nLen, int *nFrameType, int *nKeyFrameFlag);
//
//typedef int (ssssstdcall *pfnIC_SendFrame)(IC_HANDLE hStream, char *pFrame, int nLen, int nFrameType);
//
//typedef int (ssssstdcall *pfnIC_SendEndPacket)(IC_HANDLE hStream);
//
//typedef int (ssssstdcall *pfnIC_StartTalk)(IC_HANDLE hResource, IC_HANDLE *hStream);
//
//typedef int (ssssstdcall *pfnIC_GetResourceHandle)(IC_HANDLE hPU, const char *pszType, int nIndex, IC_HANDLE *hResource);
//
//typedef int (ssssstdcall *pfnIC_CSSQueryFile)(IC_HANDLE hSession, int nBeginTime, int nEndTime, int nType, const IC_STR *pReason, int nReasonCnt,
//						   const IC_HANDLE *pRes, int nResCnt, int nOffset, IC_FILE_INFO *rfiaFileInfos, int *nCount);
//
//typedef int (ssssstdcall *pfnIC_CSSVODFile)(IC_HANDLE hSession, const char *pszID, const char *pszFile, int nDirection,
//						 int nStartTime, int nDuration, int nRate, IC_STR szVodID, IC_HANDLE *hStream);
//
//typedef int (ssssstdcall *pfnIC_CSSVODTime)(IC_HANDLE hSession, IC_HANDLE hRes, int nBeginTime, int nEndTime, int nRate, IC_STR szVodID, IC_HANDLE *hStream);
//
//typedef int (ssssstdcall *pfnIC_CSSVODPause)(IC_HANDLE hSession, const char *pszVodID);
//
//typedef int (ssssstdcall *pfnIC_CSSVODResume)(IC_HANDLE hSession, const char *pszVodID);
//
//typedef int (ssssstdcall *pfnIC_CSSVODChangePosition)(IC_HANDLE hSession, const char *pszVodID, int nPosition);
//
//typedef int (ssssstdcall *pfnIC_CSSVODChangeRate)(IC_HANDLE hSession, const char *pszVodID, int nRate);
//
//typedef int (ssssstdcall *pfnIC_CSSDownloadFile)(IC_HANDLE hSession, const char *pszID, const char *pszFile, int nOffset, int nLength, IC_HANDLE *hStream);
//
//typedef int (ssssstdcall *pfnIC_SendFrame)(IC_HANDLE hStream, char *pFrame, int nLen, int nFrameType);
//
//typedef int (ssssstdcall *pfnIC_RecvEventMessage)(IC_HANDLE hSession, char **szEventMessage);
//
//typedef int (ssssstdcall *pfnIC_ReleaseMessage)(IC_HANDLE hSession, char *szMessage);
//
//typedef int (ssssstdcall *pfnIC_QueryFile)(IC_HANDLE hResource, int nIndex, int nBegin, int nEnd, int nType, int nOffset, IC_FILE_INFO *rfiaFileInfos, int *nCount);
//
//typedef int (ssssstdcall *pfnIC_GetPULongitude)(IC_HANDLE hResource, double *dbLongitude);
//
//typedef int (ssssstdcall *pfnIC_GetPULatitude)(IC_HANDLE hResource, double *dbLatitude);
//
//#ifdef _WIN32
//
//#include <windows.h>
//static HMODULE  hDllModule = NULL;
//
//#elif defined __linux__
//
//#include <dlfcn.h>
//static void*    hDllModule = NULL;
//
//#endif
//
//extern pfnIC_Initialize             fnIC_Initialize;
//extern pfnIC_Terminate              fnIC_Terminate;
//extern pfnIC_EnableConvertCharacter fnIC_EnableConvertCharacter;
//extern pfnIC_Open                   fnIC_Open;
//extern pfnIC_Close                  fnIC_Close;
//extern pfnIC_ForkPUList             fnIC_ForkPUList;
//extern pfnIC_GetResourceName        fnIC_GetResourceName;
//extern pfnIC_GetResourcePUID        fnIC_GetResourcePUID;
//extern pfnIC_GetResourceType        fnIC_GetResourceType;
//extern pfnIC_GetPUOnlineFlag        fnIC_GetPUOnlineFlag;
//extern pfnIC_ForkPUResource         fnIC_ForkPUResource;
//extern pfnIC_StartStream            fnIC_StartStream;
//extern pfnIC_StopStream             fnIC_StopStream;
//extern pfnIC_ReceiveNotify          fnIC_ReceiveNotify;
//extern pfnIC_ReceiveFrame           fnIC_ReceiveFrame;
//extern pfnIC_SendFrame              fnIC_SendFrame;
//extern pfnIC_SendEndPacket          fnIC_SendEndPacket;
//extern pfnIC_StartTalk              fnIC_StartTalk;
//extern pfnIC_GetResourceHandle      fnIC_GetResourceHandle;
//extern pfnIC_CSSQueryFile           fnIC_CSSQueryFile;
//extern pfnIC_CSSVODFile             fnIC_CSSVODFile;
//extern pfnIC_CSSVODTime             fnIC_CSSVODTime;
//extern pfnIC_CSSVODPause            fnIC_CSSVODPause;
//extern pfnIC_CSSVODResume           fnIC_CSSVODResume;
//extern pfnIC_CSSVODChangePosition   fnIC_CSSVODChangePosition;
//extern pfnIC_CSSVODChangeRate       fnIC_CSSVODChangeRate;
//extern pfnIC_CSSDownloadFile        fnIC_CSSDownloadFile;
//extern pfnIC_RecvEventMessage       fnIC_RecvEventMessage;
//extern pfnIC_ReleaseMessage         fnIC_ReleaseMessage;
//extern pfnIC_QueryFile              fnIC_QueryFile;
//extern pfnIC_GetPULongitude         fnIC_GetPULongitude;
//extern pfnIC_GetPULatitude          fnIC_GetPULatitude;
//
//
//void loadlibrary();
//
//void freelibrary();
//


#endif
