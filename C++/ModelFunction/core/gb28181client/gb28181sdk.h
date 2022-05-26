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
* 云镜控制
*/
enum {
    GB28181SDK_PTZ_CMD_IRISCLOSE    = 0x48,   /**< 光圈关  */
    GB28181SDK_PTZ_CMD_IRISOPEN     = 0x44,   /**< 光圈开  */

    GB28181SDK_PTZ_CMD_FOCUSNEAR    = 0x42,   /**< 近聚焦  */
    GB28181SDK_PTZ_CMD_FOCUSFAR     = 0x41,   /**< 远聚焦  */

    GB28181SDK_PTZ_CMD_ALLSTOP_LENS = 0x40,   /**< 停止镜头所有动作  */

    GB28181SDK_PTZ_CMD_ZOOMOUT      = 0x20,   /**< 缩小  */
    GB28181SDK_PTZ_CMD_ZOOMIN       = 0x10,   /**< 放大  */

    GB28181SDK_PTZ_CMD_UP           = 0x08,   /**< 向上  */
    GB28181SDK_PTZ_CMD_DOWN         = 0x04,   /**< 向下  */

    GB28181SDK_PTZ_CMD_RIGHT        = 0x01,   /**< 右转  */
    GB28181SDK_PTZ_CMD_LEFT         = 0x02,   /**< 左转  */

    GB28181SDK_PTZ_CMD_UPLEFT       = 0x0A,   /**< 左上方向运动  */
    GB28181SDK_PTZ_CMD_DOWNLEFT     = 0x06,   /**< 左下方向运动  */

    GB28181SDK_PTZ_CMD_UPRIGHT      = 0x09,   /**< 右上方向运动  */
    GB28181SDK_PTZ_CMD_DOWNRIGHT    = 0x05,   /**< 右下方向运动  */

    GB28181SDK_PTZ_CMD_ALLSTOP_PTZ  = 0x00,   /**< 停止所有云台操作  */

    GB28181SDK_PTZ_CMD_SETPRESET    = 0x81,   /**< 预置位保存  */
    GB28181SDK_PTZ_CMD_CALLPRESET   = 0x82,   /**< 预置位调用  */
    GB28181SDK_PTZ_CMD_CLEARPRESET  = 0x83,   /**< 预置位删除  */


    GB28181SDK_PTZ_CMD_RAIN_ON      = 0x8C,   /**< 辅助开关动作:开  */
    GB28181SDK_PTZ_CMD_RAIN_OFF     = 0x8D,   /**< 辅助开关动作：关  */
};

typedef void(*gb28181_draw_cb)(long nPort, HDC hDc, long nUser);

    /**
    * @brief 初始化gb28181sdk
    */
    GB28181_API void GB28181_Init();

    /**
    * @brief 去初始化gb28181sdk
    */
    GB28181_API void GB28181_UnInit();

    /**
    * @brief 创建本地sip服务
    * @param [in] addrcode          本级地址码
    * @param [in] host              本级ip
    * @param [in] port              本级sip端口
    * @return 成功-本地sip服务句柄，失败-NULL
    */
    GB28181_API HANDLE  GB28181_CreateLocalSipServer(const char *addrcode, const char *host, int port);

    /**
    * @brief  销毁本地sip服务
    * @param [in] lServer           本地sip服务句柄，GB28181_CreateLocalSipServer返回值
    */
    GB28181_API void  GB28181_DestroyLocalSipServer(HANDLE lServer);

    /* 配置远程A下服务 */
    GB28181_API HANDLE  GB28181_CreateLocalAServer(const char *addrcode, const char *rhhost, int rhport, const char *lshost, int lsport);

    /* 设备资源回调 */
    typedef void (*pfnQueryResourceCallback)(char *data, int size, void *pUser);

    /* 获取设备资源 */
    GB28181_API bool  GB28181_QueryResource(HANDLE lServer, const char *toaddroce, const char *tohost, int toport, const char *code, pfnQueryResourceCallback fnCb, void *pUser);

    /* 云台控制 */
    GB28181_API bool  GB28181_ControlCamera(HANDLE lServer, const char *toaddrcode, const char *tohost, int toport, const char *cameraaddr, int cmd, int param1, int param2, int param3);

    /* 历史告警数据回调 */
    typedef void(*pfnAlarmCallback)(char *data, int size, void *pUser);

    /* 查询历史告警 */
    GB28181_API bool  GB28181_QueryAlarm(HANDLE lServer, const char *toaddrcode, const char *tohost, int toport, const char *useraddrcode, uint64_t begin, uint64_t end, int type,
	                                  pfnAlarmCallback fnCb, void *pUser);

    /* 历史录像数据回调 */
    typedef void(*pfnRecordCallback)(char *data, int size, void *pUser);

    /* 查询历史录像 */
    GB28181_API bool  GB28181_QueryRecord(HANDLE lServer, const char *cameraaddr, const char *tohost, int toport, const char *usercode, int begin, int end, int type,
	         pfnRecordCallback fnCb, void *pUser);

    /**
    * @brief 订阅事件和目录
    * @param [in] lServer           本地sip服务句柄，GB28181_CreateLocalSipServer返回值
    * @param [in] toaddrcode        摄像机地址码
    * @param [in] tohost            下级ip
    * @param [in] toport            下级sip端口
    * @param [in] subType           订阅类型：0-事件，1-目录
    * @param [in] u64Begin          开始时间
    * @param [in] u64End            结束时间
    * @return 成功-true，失败-false
    */
    GB28181_API bool GB28181_Subscribe(HANDLE lServer, const char *toaddrcode, const char *tohost, int toport, int subType, uint64_t u64Begin, uint64_t u64End, int expire);

    /* 设置订阅告警和状态事件回调 */
    GB28181_API void  GB28181_SetSubsribeEventCallback(HANDLE lServer, void(*pfn)(const char *data, int size, void *puser), void *puser);


    /* 设置媒体状态事件回调 */
    GB28181_API void  GB28181_SetMediaStatusCallback(HANDLE lServer, void(*pfn)(const char *data, int size, void *puser), void *puser);


    /**
    * @brief 开始预览
    * @param [in] lServer           本地sip服务句柄，GB28181_CreateLocalSipServer返回值
    * @param [in] cameraaddr        摄像机地址码
    * @param [in] tohost            下级ip
    * @param [in] toport            下级sip端口
    * @param [in] hWnd              播放窗口句柄
    * @return 成功-预览句柄，失败-NULL
    */
    GB28181_API HANDLE  GB28181_StartPreview(HANDLE lServer, const char *cameraaddr, const char *tohost, int toport, HWND hWnd);

    /**
    * @brief 停止预览
    * @param [in] lPreview           预览句柄，GB28181_StartPreview返回值
    */
    GB28181_API void  GB28181_StopPreview(HANDLE lPreview);

    /* 开始回放 */
    GB28181_API HANDLE  GB28181_StartPlayback(HANDLE lServer, const char *cameraaddr, const char *tohost, int toport, uint32_t start, uint32_t end, HWND hWnd);

    /* 停止回放 */
    GB28181_API void  GB28181_StopPlayback(HANDLE lPlayback);

    /**
    * @brief 改变预览/回放窗口大小
    * @param [in] lPre_Pb_view           预览句柄，GB28181_StartPreview或返回值
    */
    GB28181_API void  GB28181_ChangeWinSize(HANDLE lPre_Pb_view);

    /**
    * @brief  拉框放大/缩小，缩小条件：startX减去endX的值大于2。放大条件：startX小于endX
    * @param [in] lPreview           预览句柄，GB28181_StartPreview或返回值
    * @param [in] startX                起始点的x坐标
    * @param [in] startY                起始点的y坐标
    * @param [in] endX                终止点的x坐标
    * @param [in] endY                终止点的y坐标
    * @param [in] winW                窗口宽
    * @param [in] winH                窗口高
    * @return 成功-0，失败:-1
    */
    GB28181_API int  GB28181_DragZoom(HANDLE lPreview, int startX, int startY, int endX, int endY,int winW,int winH);

    /**
    * @brief 画矩形框
    * @param [in] lPreview                  预览句柄，GB28181_StartPreview或返回值
    * @param [in] x1                        起点x
    * @param [in] y1                        起点y
    * @param [in] x2                        终点x
    * @param [in] y2                        终点y
    * @param [in] enable                    显示/不显示
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
    /* 回放控制 */
    GB28181_API void  GB28181_PlayCtrl(HANDLE lPlayback, int cmd, int param);

    /* 录像下载 */
    GB28181_API HANDLE  GB28181_DownloadRecord(HANDLE lServer, const char *cameraaddr, const char *tohost, int toport, const char* savefile, const char *recordfile, uint32_t start, uint32_t end, uint64_t fileSize = 0);

    /* 获取下载进度 */
    GB28181_API long  GB28181_Get_Downloadpos(HANDLE dl);


    /* 停止下载 */
    GB28181_API void  GB28181_DownloadStop(HANDLE lDownload);

    /* 启动与前端设备语音对讲 */
    GB28181_API HANDLE  GB28181_StartTalk(HANDLE lServer, HANDLE lPreview, const char *tohost, int toport);

    /* 停止与前端设备语音对讲 */
    GB28181_API void  GB28181_StopTalk(HANDLE lTalk);

    /* 启动与多台前端设备语音广播 */
    GB28181_API HANDLE  GB28181_StartSoundBroadcast(HANDLE lServer, const char *json, int size);

    /* 停止语音广播 */
    GB28181_API void  GB28181_StopSoundBroadcast(HANDLE lBroadcst);

    typedef void(*pfnDevAbilityCallback)(char *data, int size, void *pUser);
    /* 获取设备能力集 */
    GB28181_API bool  GB28181_QueryDevAbility(HANDLE lServer, const char *cameraaddr, const char *tohost, int toport, pfnDevAbilityCallback fnDevAbiltyCb, void *pUser);

    /* 启动|停止智能分析 */
    GB28181_API bool  GB28181_DoAlgorithmTask(HANDLE lServer, const char *cameraaddr, const char *tohost, int toport, const char *json, int size);

    /* 请求图片抓拍 */
    GB28181_API bool  GB28181_DoPictureSnap(HANDLE lServer, const char *cameraaddr, const char *tohost, int toport, const char *json, int size);

    typedef void(*pfnDevRuleCallback)(char *data, int size, void *pUser);
    /* 获取设备规则 */
    GB28181_API bool  GB28181_DoQueryDevRule(HANDLE lServer, const char *cameraaddr, const char *tohost, int toport, pfnDevRuleCallback fnDevRuleCb, void *pUser);

    typedef void(*pfnAllTaskCallback)(char *data, int size, void *pUser);
    /* 获取区域任务 */
    GB28181_API bool  GB28181_DoQueryAllTask(HANDLE lServer, const char *toaaddrcode, const char *tohost, int toport, pfnAllTaskCallback fnAllTaskCb, void *pUser);




    /* 查询流量 */
    GB28181_API int   GB28181_QueryFlow(HANDLE lServer, const char *toaddr, const char *useraddr, const char *toshost, int sport, int &bandWidth, int &streamNum);

#ifdef __cplusplus
};
#endif

#endif
