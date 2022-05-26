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

/* ��������sip���� */
GWSDK_API long  CreateLocalSipServer(const char *addrcode, const char *host, int port);

/* ���ٱ���sip���� */
GWSDK_API void  DestroyLocalSipServer(long lServer);

/* ����Զ��A�·��� */
GWSDK_API long  CreateLocalAServer(const char *addrcode, const char *rhhost, int rhport, const char *lshost, int lsport);

/* �豸��Դ�ص� */
typedef void (*pfnQueryResourceCallback)(char *data, int size, void *pUser);

/* ��ȡ�豸��Դ */
GWSDK_API bool  QueryResource(long lServer, const char *toaddroce, const char *tohost, int toport, const char *code, pfnQueryResourceCallback fnCb, void *pUser);

/* ��̨���� */
GWSDK_API bool  ControlCamera(long lServer, const char *toaddrcode, const char *tohost, int toport, const char *cameraaddr, int cmd, int param1, int param2, int param3);

/* ��ʷ�澯���ݻص� */
typedef void(*pfnAlarmCallback)(char *data, int size, void *pUser);

/* ��ѯ��ʷ�澯 */
GWSDK_API bool  QueryAlarm(long lServer, const char *toaddrcode, const char *tohost, int toport, const char *useraddrcode, char *begin, char *end, int type,
	                              pfnAlarmCallback fnCb, void *pUser);

/* ��ʷ¼�����ݻص� */
typedef void(*pfnRecordCallback)(char *data, int size, void *pUser);

/* ��ѯ��ʷ¼�� */
GWSDK_API bool  QueryRecord(long lServer, const char *cameraaddr, const char *tohost, int toport, const char *usercode, char *begin, char *end, int type,
	     pfnRecordCallback fnCb, void *pUser);

/* ���ĸ澯���¼� expires˵����0:ȡ������ ����>=3600 ������ */
GWSDK_API bool  Subscribe(long lServer, const char *toaddrcode, const char *tohost, int toport, int mode, int alarmtype, int expires = 3600);

/* ���ö��ĸ澯��״̬�¼��ص� */
GWSDK_API void  SetSubsribeEventCallback(long lServer, void(*pfn)(const char *data, int size, void *puser), void *puser);

/* ��ʼԤ�� */
GWSDK_API long  StartPreview(long lServer, const char *cameraaddr, const char *tohost, int toport, HWND hWnd);

/* ֹͣԤ�� */
GWSDK_API void  StopPreview(long lPreview);

/* ��ʼ�ط� */
GWSDK_API long  StartPlayback(long lServer, const char *url, HWND hWnd);

/* ֹͣ�ط� */
GWSDK_API void  StopPlayback(long lPlayback);

/* ������ǰ���豸�����Խ� */
GWSDK_API long  StartTalk(long lServer, long lPreview, const char *tohost, int toport);

/* ֹͣ��ǰ���豸�����Խ� */
GWSDK_API void  StopTalk(long lTalk);

/* �������̨ǰ���豸�����㲥 */
GWSDK_API long  StartSoundBroadcast(long lServer, const char *json, int size);

/* ֹͣ�����㲥 */
GWSDK_API void  StopSoundBroadcast(long lBroadcst);

typedef void(*pfnDevAbilityCallback)(char *data, int size, void *pUser);
/* ��ȡ�豸������ */
GWSDK_API bool  QueryDevAbility(long lServer, const char *cameraaddr, const char *tohost, int toport, pfnDevAbilityCallback fnDevAbiltyCb, void *pUser);

/* ����|ֹͣ���ܷ��� */
GWSDK_API bool  DoAlgorithmTask(long lServer, const char *cameraaddr, const char *tohost, int toport, const char *json, int size);

/* ����ͼƬץ�� */
GWSDK_API bool  DoPictureSnap(long lServer, const char *cameraaddr, const char *tohost, int toport, const char *json, int size);

typedef void(*pfnDevRuleCallback)(char *data, int size, void *pUser);
/* ��ȡ�豸���� */
GWSDK_API bool  DoQueryDevRule(long lServer, const char *cameraaddr, const char *tohost, int toport, pfnDevRuleCallback fnDevRuleCb, void *pUser);

typedef void(*pfnAllTaskCallback)(char *data, int size, void *pUser);
/* ��ȡ�������� */
GWSDK_API bool  DoQueryAllTask(long lServer, const char *toaaddrcode, const char *tohost, int toport, pfnAllTaskCallback fnAllTaskCb, void *pUser);

enum 
{
	PLAYBACK_PAUSE,
	PLAYBACK_CONTINUE,
	PLAYBACK_FAST,
	PLAYBACK_SLOW
};
/* �طſ��� */
GWSDK_API void  PlayCtrl(long lPlayback, int cmd);


/* ��ѯ���� */
GWSDK_API int   QueryFlow(long lServer, const char *toaddr, const char *useraddr, const char *toshost, int sport, int &bandWidth, int &streamNum);

#ifdef __cplusplus
};
#endif

#endif
