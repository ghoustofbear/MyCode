#ifndef __CAPTURE_H__
#define __CAPTURE_H__

#include <windows.h>

#ifdef CAPTURE_EXPORTS
#define CAP_API __declspec(dllexport)
#else
#define CAP_API __declspec(dllimport)
#endif

typedef struct  
{
	int   iId;
	char  szName[36];
}CAP_CAMERA_S;

#if __cplusplus
extern "C" {
#endif
	
/** 
 * @brief 事件订阅
 *
 * @param [out] lDataType   1-RGB24数据 2-YUV420数据 3-H.264数据
 * @param [out] pData       数据缓冲区
 * @param [out] lDataSize   数据长度
 * @param [out] pUser       用户数据
 *
 * @return 成功返回0, 失败-1.   
 */
typedef VOID (*pfnCapDataCallback)(long lDataType, BYTE *pData, long lDataSize, VOID *pUser);

CAP_API long CAP_Init();
CAP_API void CAP_UnInit();
CAP_API long CAP_EnumDevices(int *iCameraNum, CAP_CAMERA_S stCamera[]);
CAP_API long CAP_StartCapture(int iDeviceId, HWND hWndShow, pfnCapDataCallback fnCapDataCallback,UINT uCBMode, VOID *pUser);
CAP_API void CAP_StopCapture(int iDeviceId);
CAP_API void CAP_SetCapture(bool bCap);

#ifdef __cplusplus
}
#endif

#endif