#ifndef  _GB28181_SDK_H_
#define  _GB28181_SDK_H_

#ifdef GB28181_STATICLIB
#  define GB28181_API
#elif defined(WIN32) || defined(_WIN32)
#  if defined(GB28181_EXPORTS)
#    define GB28181_API  __declspec(dllexport)
#  else
#    define GB28181_API  __declspec(dllimport)
#  endif
#else
#  define   GB28181_API
#endif


#ifdef _WIN32
#include <WinSock2.h>
#include <windows.h>
#else
typedef void*                HANDLE;
typedef unsigned long        HWND;
typedef void*                HDC;
#endif

#if defined(DLL_EXPORT)
#include <stdint.h>

#endif

#ifdef __cplusplus
extern "C" {
#endif


#ifndef DATETIME_S
#define DATETIME_S
    typedef struct
    {
        int            year;
        int            month;
        int            day;
        int            hour;
        int            minute;
        int            second;
    } datetime_s;
#endif

/**
* �ƾ�����
*/
enum {
    GB28181SDK_PTZ_CMD_IRISCLOSE    = 0x48,   /**< ��Ȧ��  */
    GB28181SDK_PTZ_CMD_IRISOPEN     = 0x44,   /**< ��Ȧ��  */

    GB28181SDK_PTZ_CMD_FOCUSNEAR    = 0x42,   /**< ���۽�  */
    GB28181SDK_PTZ_CMD_FOCUSFAR     = 0x41,   /**< Զ�۽�  */

    GB28181SDK_PTZ_CMD_ALLSTOP_LENS = 0x40,   /**< ֹͣ��ͷ���ж���  */

    GB28181SDK_PTZ_CMD_ZOOMOUT      = 0x20,   /**< ��С  */
    GB28181SDK_PTZ_CMD_ZOOMIN       = 0x10,   /**< �Ŵ�  */

    GB28181SDK_PTZ_CMD_UP           = 0x08,   /**< ����  */
    GB28181SDK_PTZ_CMD_DOWN         = 0x04,   /**< ����  */

    GB28181SDK_PTZ_CMD_RIGHT        = 0x01,   /**< ��ת  */
    GB28181SDK_PTZ_CMD_LEFT         = 0x02,   /**< ��ת  */

    GB28181SDK_PTZ_CMD_UPLEFT       = 0x0A,   /**< ���Ϸ����˶�  */
    GB28181SDK_PTZ_CMD_DOWNLEFT     = 0x06,   /**< ���·����˶�  */

    GB28181SDK_PTZ_CMD_UPRIGHT      = 0x09,   /**< ���Ϸ����˶�  */
    GB28181SDK_PTZ_CMD_DOWNRIGHT    = 0x05,   /**< ���·����˶�  */

    GB28181SDK_PTZ_CMD_ALLSTOP_PTZ  = 0x00,   /**< ֹͣ������̨����  */

    GB28181SDK_PTZ_CMD_SETPRESET    = 0x81,   /**< Ԥ��λ����  */
    GB28181SDK_PTZ_CMD_CALLPRESET   = 0x82,   /**< Ԥ��λ����  */
    GB28181SDK_PTZ_CMD_CLEARPRESET  = 0x83,   /**< Ԥ��λɾ��  */


    GB28181SDK_PTZ_CMD_RAIN_ON      = 0x8C,   /**< �������ض���:��  */
    GB28181SDK_PTZ_CMD_RAIN_OFF     = 0x8D,   /**< �������ض�������  */
};

typedef void(*gb28181_draw_cb)(long nPort, HDC hDc, long nUser);

    /**
    * @brief ��ʼ��gb28181sdk
    */
    GB28181_API void GB28181_Init();

    /**
    * @brief ȥ��ʼ��gb28181sdk
    */
    GB28181_API void GB28181_UnInit();

    /**
    * @brief ��������sip����
    * @param [in] addrcode          ������ַ��
    * @param [in] host              ����ip
    * @param [in] port              ����sip�˿�
    * @return �ɹ�-����sip��������ʧ��-NULL
    */
    GB28181_API HANDLE  GB28181_CreateLocalSipServer(const char *addrcode, const char *host, int port);

    /**
    * @brief  ���ٱ���sip����
    * @param [in] lServer           ����sip��������GB28181_CreateLocalSipServer����ֵ
    */
    GB28181_API void  GB28181_DestroyLocalSipServer(HANDLE lServer);

    /* ����Զ��A�·��� */
    GB28181_API HANDLE  GB28181_CreateLocalAServer(const char *addrcode, const char *rhhost, int rhport, const char *lshost, int lsport);

    /* �豸��Դ�ص� */
    typedef void (*pfnQueryResourceCallback)(char *data, int size, void *pUser);

    /* ��ȡ�豸��Դ */
    GB28181_API bool  GB28181_QueryResource(HANDLE lServer, const char *toaddroce, const char *tohost, int toport, const char *code, pfnQueryResourceCallback fnCb, void *pUser);

    /* ��̨���� */
    GB28181_API bool  GB28181_ControlCamera(HANDLE lServer, const char *toaddrcode, const char *tohost, int toport, const char *cameraaddr, int cmd, int param1, int param2, int param3);

    /* ��ʷ�澯���ݻص� */
    typedef void(*pfnAlarmCallback)(char *data, int size, void *pUser);

    /* ��ѯ��ʷ�澯 */
    GB28181_API bool  GB28181_QueryAlarm(HANDLE lServer, const char *toaddrcode, const char *tohost, int toport, const char *useraddrcode, uint64_t begin, uint64_t end, int type,
	                                  pfnAlarmCallback fnCb, void *pUser);

    /* ��ʷ¼�����ݻص� */
    typedef void(*pfnRecordCallback)(char *data, int size, void *pUser);

    /* ��ѯ��ʷ¼�� */
    GB28181_API bool  GB28181_QueryRecord(HANDLE lServer, const char *cameraaddr, const char *tohost, int toport, const char *usercode, int begin, int end, int type,
	         pfnRecordCallback fnCb, void *pUser);

    /**
    * @brief �����¼���Ŀ¼
    * @param [in] lServer           ����sip��������GB28181_CreateLocalSipServer����ֵ
    * @param [in] toaddrcode        �������ַ��
    * @param [in] tohost            �¼�ip
    * @param [in] toport            �¼�sip�˿�
    * @param [in] subType           �������ͣ�0-�¼���1-Ŀ¼
    * @param [in] u64Begin          ��ʼʱ��
    * @param [in] u64End            ����ʱ��
    * @return �ɹ�-true��ʧ��-false
    */
    GB28181_API bool GB28181_Subscribe(HANDLE lServer, const char *toaddrcode, const char *tohost, int toport, int subType, uint64_t u64Begin, uint64_t u64End, int expire);

    /* ���ö��ĸ澯��״̬�¼��ص� */
    GB28181_API void  GB28181_SetSubsribeEventCallback(HANDLE lServer, void(*pfn)(const char *data, int size, void *puser), void *puser);


    /* ����ý��״̬�¼��ص� */
    GB28181_API void  GB28181_SetMediaStatusCallback(HANDLE lServer, void(*pfn)(const char *data, int size, void *puser), void *puser);


    /**
    * @brief ��ʼԤ��
    * @param [in] lServer           ����sip��������GB28181_CreateLocalSipServer����ֵ
    * @param [in] cameraaddr        �������ַ��
    * @param [in] tohost            �¼�ip
    * @param [in] toport            �¼�sip�˿�
    * @param [in] hWnd              ���Ŵ��ھ��
    * @return �ɹ�-Ԥ�������ʧ��-NULL
    */
    GB28181_API HANDLE  GB28181_StartPreview(HANDLE lServer, const char *cameraaddr, const char *tohost, int toport, HWND hWnd);

    /**
    * @brief ֹͣԤ��
    * @param [in] lPreview           Ԥ�������GB28181_StartPreview����ֵ
    */
    GB28181_API void  GB28181_StopPreview(HANDLE lPreview);

    /* ��ʼ�ط� */
    GB28181_API HANDLE  GB28181_StartPlayback(HANDLE lServer, const char *cameraaddr, const char *tohost, int toport, uint32_t start, uint32_t end, HWND hWnd);

    /* ֹͣ�ط� */
    GB28181_API void  GB28181_StopPlayback(HANDLE lPlayback);

    /**
    * @brief �ı�Ԥ��/�طŴ��ڴ�С
    * @param [in] lPre_Pb_view           Ԥ�������GB28181_StartPreview�򷵻�ֵ
    */
    GB28181_API void  GB28181_ChangeWinSize(HANDLE lPre_Pb_view);

    /**
    * @brief  ����Ŵ�/��С����С������startX��ȥendX��ֵ����2���Ŵ�������startXС��endX
    * @param [in] lPreview           Ԥ�������GB28181_StartPreview�򷵻�ֵ
    * @param [in] startX                ��ʼ���x����
    * @param [in] startY                ��ʼ���y����
    * @param [in] endX                ��ֹ���x����
    * @param [in] endY                ��ֹ���y����
    * @param [in] winW                ���ڿ�
    * @param [in] winH                ���ڸ�
    * @return �ɹ�-0��ʧ��:-1
    */
    GB28181_API int  GB28181_DragZoom(HANDLE lPreview, int startX, int startY, int endX, int endY,int winW,int winH);

    /**
    * @brief �����ο�
    * @param [in] lPreview                  Ԥ�������GB28181_StartPreview�򷵻�ֵ
    * @param [in] x1                        ���x
    * @param [in] y1                        ���y
    * @param [in] x2                        �յ�x
    * @param [in] y2                        �յ�y
    * @param [in] enable                    ��ʾ/����ʾ
    */
    GB28181_API void  GB28181_DrawRect(HANDLE lPreview, int x1, int y1, int x2, int y2, bool enable);
    enum
    {
        PLAYBACK_PAUSE,
        PLAYBACK_CONTINUE,
        PLAYBACK_FAST,
        PLAYBACK_SLOW,
        PLAYBACK_SETTIME
    };
    /* �طſ��� */
    GB28181_API void  GB28181_PlayCtrl(HANDLE lPlayback, int cmd, int param);

    /* ¼������ */
    GB28181_API HANDLE  GB28181_DownloadRecord(HANDLE lServer, const char *cameraaddr, const char *tohost, int toport, const char* savefile, const char *recordfile, uint32_t start, uint32_t end, uint64_t fileSize = 0);

    /* ��ȡ���ؽ��� */
    GB28181_API long  GB28181_Get_Downloadpos(HANDLE dl);


    /* ֹͣ���� */
    GB28181_API void  GB28181_DownloadStop(HANDLE lDownload);

    /* ������ǰ���豸�����Խ� */
    GB28181_API HANDLE  GB28181_StartTalk(HANDLE lServer, HANDLE lPreview, const char *tohost, int toport);

    /* ֹͣ��ǰ���豸�����Խ� */
    GB28181_API void  GB28181_StopTalk(HANDLE lTalk);

    /* �������̨ǰ���豸�����㲥 */
    GB28181_API HANDLE  GB28181_StartSoundBroadcast(HANDLE lServer, const char *json, int size);

    /* ֹͣ�����㲥 */
    GB28181_API void  GB28181_StopSoundBroadcast(HANDLE lBroadcst);

    typedef void(*pfnDevAbilityCallback)(char *data, int size, void *pUser);
    /* ��ȡ�豸������ */
    GB28181_API bool  GB28181_QueryDevAbility(HANDLE lServer, const char *cameraaddr, const char *tohost, int toport, pfnDevAbilityCallback fnDevAbiltyCb, void *pUser);

    /* ����|ֹͣ���ܷ��� */
    GB28181_API bool  GB28181_DoAlgorithmTask(HANDLE lServer, const char *cameraaddr, const char *tohost, int toport, const char *json, int size);

    /* ����ͼƬץ�� */
    GB28181_API bool  GB28181_DoPictureSnap(HANDLE lServer, const char *cameraaddr, const char *tohost, int toport, const char *json, int size);

    typedef void(*pfnDevRuleCallback)(char *data, int size, void *pUser);
    /* ��ȡ�豸���� */
    GB28181_API bool  GB28181_DoQueryDevRule(HANDLE lServer, const char *cameraaddr, const char *tohost, int toport, pfnDevRuleCallback fnDevRuleCb, void *pUser);

    typedef void(*pfnAllTaskCallback)(char *data, int size, void *pUser);
    /* ��ȡ�������� */
    GB28181_API bool  GB28181_DoQueryAllTask(HANDLE lServer, const char *toaaddrcode, const char *tohost, int toport, pfnAllTaskCallback fnAllTaskCb, void *pUser);




    /* ��ѯ���� */
    GB28181_API int   GB28181_QueryFlow(HANDLE lServer, const char *toaddr, const char *useraddr, const char *toshost, int sport, int &bandWidth, int &streamNum);

#ifdef __cplusplus
};
#endif

#endif
