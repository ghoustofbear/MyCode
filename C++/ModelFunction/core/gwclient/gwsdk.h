#ifndef  _GW_SDK_H_
#define  _GW_SDK_H_

#ifdef GWSDK_STATICLIB
#  define GWSDK_API
#elif defined(WIN32) || defined(_WIN32)
#  if defined(GWSDK_EXPORTS)
#    define GWSDK_API  __declspec(dllexport)
#  else
#    define GWSDK_API  __declspec(dllimport)
#  endif
#else
#  define   GWSDK_API
#endif

#if defined(DLL_EXPORT)
#include <stdint.h>

#ifdef _WIN32
#include <windows.h>
#else
typedef void*                HANDLE;
typedef unsigned long        HWND;
typedef void*                HDC;
#endif


#endif

#ifdef __cplusplus
extern "C" {
#endif

GWSDK_API void GWSDKInit();

/* 创建本地sip服务 */
GWSDK_API long  CreateLocalSipServer(const char *addrcode, const char *host, int port);

/* 销毁本地sip服务 */
GWSDK_API void  DestroyLocalSipServer(long lServer);

/* 配置远程A下服务 */
GWSDK_API long  CreateLocalAServer(const char *addrcode, const char *rhhost, int rhport, const char *lshost, int lsport);

/* 设备资源回调 */
typedef void (*pfnQueryResourceCallback)(char *data, int size, void *pUser);

/* 获取设备资源 */
GWSDK_API bool  QueryResource(long lServer, const char *toaddroce, const char *tohost, int toport, const char *code, pfnQueryResourceCallback fnCb, void *pUser);

/* 云台控制 */
GWSDK_API bool  ControlCamera(long lServer, const char *toaddrcode, const char *tohost, int toport, const char *cameraaddr, int cmd, int param1, int param2, int param3);

/* 历史告警数据回调 */
typedef void(*pfnAlarmCallback)(char *data, int size, void *pUser);

/* 查询历史告警 */
GWSDK_API bool  QueryAlarm(long lServer, const char *toaddrcode, const char *tohost, int toport, const char *useraddrcode, char *begin, char *end, int type,
	                              pfnAlarmCallback fnCb, void *pUser);

/* 历史录像数据回调 */
typedef void(*pfnRecordCallback)(char *data, int size, void *pUser);

/* 查询历史录像 */
GWSDK_API bool  QueryRecord(long lServer, const char *cameraaddr, const char *tohost, int toport, const char *usercode, char *begin, char *end, int type,
	     pfnRecordCallback fnCb, void *pUser);

/* 订阅告警和事件 expires说明：0:取消订阅 其它>=3600 发起订阅 */
GWSDK_API bool  Subscribe(long lServer, const char *toaddrcode, const char *tohost, int toport, int mode, int alarmtype, int expires = 3600);

/* 设置订阅告警和状态事件回调 */
GWSDK_API void  SetSubsribeEventCallback(long lServer, void(*pfn)(const char *data, int size, void *puser), void *puser);

/* 开始预览 */
GWSDK_API long  StartPreview(long lServer, const char *cameraaddr, const char *tohost, int toport, HWND hWnd);

/* 停止预览 */
GWSDK_API void  StopPreview(long lPreview);

/* 开始回放 */
GWSDK_API long  StartPlayback(long lServer, const char *url, HWND hWnd);

/* 停止回放 */
GWSDK_API void  StopPlayback(long lPlayback);

/* 启动与前端设备语音对讲 */
GWSDK_API long  StartTalk(long lServer, long lPreview, const char *tohost, int toport);

/* 停止与前端设备语音对讲 */
GWSDK_API void  StopTalk(long lTalk);

/* 启动与多台前端设备语音广播 */
GWSDK_API long  StartSoundBroadcast(long lServer, const char *json, int size);

/* 停止语音广播 */
GWSDK_API void  StopSoundBroadcast(long lBroadcst);

typedef void(*pfnDevAbilityCallback)(char *data, int size, void *pUser);
/* 获取设备能力集 */
GWSDK_API bool  QueryDevAbility(long lServer, const char *cameraaddr, const char *tohost, int toport, pfnDevAbilityCallback fnDevAbiltyCb, void *pUser);

/* 启动|停止智能分析 */
GWSDK_API bool  DoAlgorithmTask(long lServer, const char *cameraaddr, const char *tohost, int toport, const char *json, int size);

/* 请求图片抓拍 */
GWSDK_API bool  DoPictureSnap(long lServer, const char *cameraaddr, const char *tohost, int toport, const char *json, int size);

typedef void(*pfnDevRuleCallback)(char *data, int size, void *pUser);
/* 获取设备规则 */
GWSDK_API bool  DoQueryDevRule(long lServer, const char *cameraaddr, const char *tohost, int toport, pfnDevRuleCallback fnDevRuleCb, void *pUser);

typedef void(*pfnAllTaskCallback)(char *data, int size, void *pUser);
/* 获取区域任务 */
GWSDK_API bool  DoQueryAllTask(long lServer, const char *toaaddrcode, const char *tohost, int toport, pfnAllTaskCallback fnAllTaskCb, void *pUser);

enum 
{
	PLAYBACK_PAUSE,
	PLAYBACK_CONTINUE,
	PLAYBACK_FAST,
	PLAYBACK_SLOW
};
/* 回放控制 */
GWSDK_API void  PlayCtrl(long lPlayback, int cmd);


/* 查询流量 */
GWSDK_API int   QueryFlow(long lServer, const char *toaddr, const char *useraddr, const char *toshost, int sport, int &bandWidth, int &streamNum);

#ifdef __cplusplus
};
#endif

#endif
