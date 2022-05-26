/**
 * (C) Copyright 2013, xxx Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Feb 26 16:45:16 PST 2013
 *
 * Network sdk functions define.
 */
#ifndef __UT_NET_H__
#define __UT_NET_H__

#ifndef SDK_EXPORT
#include "proto_def.h"
#endif

#ifdef _WIN32
#ifdef   NET_EXPORTS
#define  NET_API __declspec(dllexport)
#else
#define  NET_API __declspec(dllimport)
#endif
#define  WM_VIDEO_CAPTURE     WM_USER + 200
#define  MSG_CAP_START        1
#define  MSG_CAP_STOP         2
#else
#define  NET_API
#endif

#if defined(SDK_EXPORT)
typedef unsigned char           U8;
typedef unsigned char           UCHAR;
typedef unsigned short          U16;
typedef unsigned int            U32;
typedef unsigned long           ULONG;

#ifdef _WIN32
#include <windows.h>
typedef ULONGLONG               U64;
typedef LONGLONG                S64;
#else
typedef unsigned long long      U64;
typedef long long               S64;
typedef void                    VOID;
typedef int                     BOOL;
typedef long                    LONG;
typedef unsigned int            DWORD;
typedef void* 			        HANDLE;
typedef unsigned int            HWND;
typedef const char*             PCSTR;
typedef char*                   PCHAR;
typedef char                    CHAR;
#endif

typedef char                    S8;
typedef short                   S16;
typedef int                     S32;

typedef float                   FLOAT;
typedef double                  DOUBLE;

//typedef unsigned long           SIZE_T;

typedef unsigned char           BYTE;
typedef unsigned short          WORD;
typedef unsigned int            UINT;

typedef LONG *                  PLONG;
typedef PLONG                   LPLONG;

typedef U8                      ULONG8;
typedef U32                     ULONG32;
typedef S32                     INT32;

#define MAX_NAME_LEN	       (36)
#define MAX_ADDR_LEN           (24)
#define MAX_IP_STR_LEN         (16)
#define MAX_PORT_STR_LEN       (6)
#define MAX_MAC_LEN            (6)
#define MAX_IPV4_LEN           (4)
#define MAX_IPV6_LEN           (16)
#define MAX_FILENAME_LEN       (128)
#define MAX_OBJECTNAME_LEN     (64)
#define MAX_AIOBJ_NUM          (6)
#define MAX_OBJ_PRESSPLATE     (144)

#define UT_I420			       (1)
#define UT_YV12                (2)
#define UT_YUV422			   (3)
#define UT_UYVY                (4)
#define UT_RGB32               (5)
#define MAX_CHANNEL_NUM        (64)

typedef struct
{
    U32                      u32MngRightBit;                  /**< 管理权限位, 0-基本设置 1-站点管理 2-设备管理 3-权限组管理
		                     								                  4-用户管理 5-设备状态 6-用户状态 7-系统状态
		                     								  		      8-系统日志 9-报警日志 10-设备日志 11-用户日志 */
    U32                      u32OptRightBit;                  /**< 操作权限位, 0-预览 1-回放 2-下载 3-云台 4-报警 */
    U32                      u32PVChannel;                    /**< 预览通道 */
    U32                      u32PBChannel;                    /**< 回放通道 */
    U32                      u32PtzChannel;                   /**< 云台控制通道 */
    U32                      u32AlarmChannel;                 /**< 报警上报通道 */
    U8                       u8Res1[8];
    U8                       u8MngLevel;                      /**< 控制权限等级 */
    U8                       u8OptLevel;                      /**< 控制权限等级 */
    U8                       u8Res2[6];
} USER_RIGHT_S;

typedef struct _LOGIN_RSP_S_
{
    S8                       s8HWVerison[MAX_NAME_LEN];       /**< 硬件版本 */
    S8                       s8IEVerison[MAX_NAME_LEN];       /**< IE版本 */
    S8                       s8SWVersion[MAX_NAME_LEN];       /**< 软件版本 */
    S8                       s8SystemName[MAX_NAME_LEN];      /**< 系统名称, 可以修改 */
    S8                       s8AddrCode[MAX_ADDR_LEN];        /**< 地址编码 */
    S8                       s8MacAddr[MAX_NAME_LEN];         /**< 设备MAC地址 */

    U32                      u32UserKey;                      /**< 连接分配的用户密钥 */
    USER_RIGHT_S             stUserRight;
    U32                      u32DeviceNum;                    /**< 通道数 */
    U32                      u32ChnInfoSize;
    U8                       u8Res[20];
} LOGIN_RSP_S;

typedef struct
{
    S8                       s8DevName[MAX_NAME_LEN];         /**< 设备名称 */
    S8                       s8AddrCode[MAX_ADDR_LEN];        /**< 设备地址编码 */
    U8                       u8IPAddr[4];

    U16                      u16Port;
    U16                      u16DevType;                      /**< 设备类型 */
    U8                       u8ChannelNum;                    /**< 通道数 */

    U8                       u8ManuType;                      /**< 设备厂商类型 */
    U8                       u8AlarmInNum;                    /**< 报警输入数 */
    U8                       u8AlarmOutNum;                   /**< 报警输出数(继电器个数) */
    U8                       u8IsOnline;                      /**< 设备是否在线 */
    U8                       u8Reserved[3];
} CHN_INFO_S;

/* 用户列表项信息 */
typedef struct
{
    U32                      u32StSize;                       /**< sizeof(USERINFO_RSP_S) */
    S8                       s8Username[MAX_NAME_LEN];        /**< 用户名 */
    S8                       s8Password[MAX_NAME_LEN];        /**< 密码 */
    USER_RIGHT_S             stUserRight;                     /**< 用户权限 */
} USERINFO_RSP_S;

/* 设备列表项信息 */
typedef struct
{
    U32                     u32StSize;                        /**< sizeof(DEVINFO_RSP_S) */

    S8                      s8AddrCode[MAX_ADDR_LEN];         /**< 设备名称 */
    S8                      s8DevName[MAX_NAME_LEN];          /**< 设备地址编码 */
    S8                      s8PareAddrCode[MAX_ADDR_LEN];     /**< 父设备地址编码 */
    S8                      s8DevDesc[100];                   /**< 设备描叙 */

    S32                     s32ChnNum;                        /**< 设备通道数 */
    S32                     s32ChnNo;                         /**< 设备通道号 */
    U8                      u8IPAddr[MAX_IPV4_LEN];           /**< 设备IP地址 */

    U16                     u16Port;                          /**< 设备端口号 */
    U16                     u16DevType;                       /**< 设备类型 */

    S8                      s8ManuType;                       /**< 设备厂商类型 */
    S8                      s8ProtoType;                      /**< 协议类型 */
    S8                      s8StreamType;                     /**< 码流类型 */
} DEVINFO_RSP_S;

typedef struct
{
    CHN_INFO_S              stChnInfo;
} PVIEW_RSP_S;

typedef struct
{
    U32                     u32Flag;                          /**< 0 - 云台控制开始，1 - 云台控制停止 */
    U32                     u32Speed;                         /**< 云台控制速度 */
} PTZ_CTRL_NORMAL_S;

typedef struct
{
    U32                     u32Flag;                          /**< 0 - 云台控制开始，1 - 云台控制停止 */
    U32                     u32No;
} PTZ_CTRL_CRUISE_S;

typedef struct
{
    U32                     u32Index;                         /**< 云台设置或调用预置点号, 从0开始 */
} PTZ_CTRL_PRESET_S;

typedef struct
{
    U32                     u32StSize;
    U32                     u32PresetNo;                      /**< 预置点号 */
    S8                      s8PresetName[MAX_NAME_LEN];       /**< 预置点名称 */
} GET_PRESET_RSP_S;

typedef struct
{
    U32                     u32StSize;
    U32                     u32PresetNo;                      /**< 预置点号 */
    S8                      s8PresetName[MAX_NAME_LEN];       /**< 预置点名称 */
    U8                      u32PresetType;                    /**< 预置位类型 */
} GET_PRESET_RSP_S_EX; /* 新规约 */

#ifndef _DATETIME_
#define _DATETIME_
typedef struct
{
    U32                     u32Year;                          /**< 年, 2013 */
    U32                     u32Month;                         /**< 月, 1~12 */
    U32                     u32Day;                           /**< 日, 1~31 */
    U32                     u32Hour;                          /**< 时, 0~23 */
    U32                     u32Minute;                        /**< 分, 0~59 */
    U32                     u32Second;                        /**< 秒, 0~59 */
} DATETIME_S;
#endif

/* 录像类型 */
#define RECORD_NORMAL	    0x1                               /**< 常规录像 */
#define RECORD_ALARM	    0x2                               /**< 报警录像 */
#define RECORD_TIME         0x4                               /**< 定时录像 */
#define RECORD_ALL		    0xFFFFFFFF                        /**< Only for req, equal to RECORD_NORMAL|RECORD_ALARM... */

typedef struct
{
    U32                     u32StSize;                        /**< sizeof(REC_DETAIL_RSP_S) */
    U32                     u32ChnNO;                         /**< 通道号 */
    DATETIME_S              stBegin;                          /**< 录像开始时间 */
    DATETIME_S              stEnd;                            /**< 录像结束时间 */
    U32                     u32Type;                          /**< 录像类型 */
    U32                     u32Size;                          /**< 录像文件总长度 */
    S8                      s8FileName[MAX_FILENAME_LEN];     /**< 录像文件名 */
} REC_DETAIL_RSP_S;

typedef struct
{
    U32                     u32StSize;                        /**< REC_CALENDAR_RSP_S */
    DATETIME_S              stDay;
} REC_CALENDAR_RSP_S;

enum
{
    REC_PLAY_IP_TIME = 0,                                     /**<  0-IP+PORT+时间段回放 */
    REC_PLAY_IP_NAME,                                         /**<  1-IP_PORT+文件回放 */
    REC_PLAY_ADDR_URL,                                        /**<  2-地址码+RTSP URL回放 */
    REC_PLAY_ADDR_TIME,                                       /**<  3-地址码+时间回放 */
	REC_PLAY_HIKPLATFORM,                                      /**< 海康平台回放 */
};

/* 报警器结构 */
typedef struct
{
    S8                      s8DeviceName[MAX_NAME_LEN];       /**< 报警设备名 */
    U8                      u8DeviceIp[MAX_IPV4_LEN];         /**< 报警设备IP */

    U16                     u16DevicePort;                    /**< 报警设备端口 */
    U8                      u8Res[2];
} ALARMER_S;

typedef struct
{
    DATETIME_S              stTime;                           /**< 报警产生时间 */
    U8                      u8ChnNo;                          /**< 报警通道号 */
    U8                      u8Status;                         /**< 报警状态，0-开始 1-结束 */
} ALARM_IO_RPT_S;

typedef struct
{
    DATETIME_S              stTime;                           /**< 报警产生时间 */
    U8                      u8ChnNo;                          /**< 报警通道号 */
    U8                      u8Status;                         /**< 报警状态，0-开始 1-结束 */
} ALARM_MD_RPT_S;

typedef struct
{
    DATETIME_S              stTime;                           /**< 报警产生时间 */
    U8                      u8ChnNo;                          /**< 报警通道号 */
    U8                      u8Status;                         /**< 报警状态，0-开始 1-结束 */
} ALARM_VLOSS_RPT_S;

typedef struct
{
    DATETIME_S              stTime;                           /**< 报警产生时间 */
    U8                      u8HddNo;                          /**< 报警硬盘号 */
    U8                      u8Status;                         /**< 报警状态，0-开始 1-结束 */
} ALARM_HDD_RPT_S;

/* 设备状态上传结构 */
typedef struct
{
    DATETIME_S              stTime;                            /**< 报警产生时间 */
    U8                      u8DeviceIp[MAX_IPV4_LEN];          /**< 报警设备IP */

    U16                     u16DevicePort;                     /**< 报警设备*/
    U8                      u8Status;                          /**< 报警状态，0-离线 1-在线 */
} ALARM_DEVSTATE_RPT_S;

typedef struct
{
    S8                      s8TaskName[MAX_FILENAME_LEN];      /**< 联动任务名 */

    U8                      u8DeviceIp[MAX_IPV4_LEN];          /**< 联动设备IP */

    U8                      u8ChnNO;                           /**< 联动设备通道号 */
    U8                      u8PresetNo;                        /**< 联动设备预置点号 */
    U16                     u16TaskId;                         /**< 联动任务ID */
} LINKAGE_RPT_S;

/* 客户端联动转发推送信息 */
typedef struct
{
    S8                      s8SrcName[MAX_NAME_LEN];           /**< 联动转发源(客户端)名称 */
    U8                      u8SrcIp[MAX_IPV4_LEN];             /**< 联动转发源(客户端)IP */
    S8                      s8Desc[MAX_FILENAME_LEN];          /**< 联动描述 */
    U32                     u32DeviceNum;                      /**< 联动设备个数, 1-16 */
    U32                     u32CameraId[16];                   /**< 联动设备通道ID号 */
    U8                      u8PresetNo[16];                    /**< 联动设备预置点号 */
} LINKAGE_TRANS_RPT_S;

enum
{
    AI_TYPE_TRIPWIRE  = 4,                                    /**< 拌线 */
    AI_TYPE_PERIMETER,                                        /**< 周界 */
    AI_TYPE_ITEMLEFT,                                         /**< 物品遗留丢失 */
    AI_TYPE_PLATE,                                            /**< 车牌检测 */
    AI_TYPE_HOVER,                                            /**< 徘徊检测  */
    AI_TYPE_ANTITHEFT,                                        /**< 防盗检测   */
    AI_TYPE_OFFJOB,                                           /**< 脱岗检测   */
    AI_TYPE_MAINTENAREA,                                      /**< 检修区域检测 */
    AI_TYPE_PRESSPLATE,                                       /**< 压板投退状态检测 */
    AI_TYPE_INDICATOR,                                        /**< 分合指示牌检测 */
    AI_TYPE_DISCONNECTOR,                                     /**< 隔离开关(触头)状态检测 */
    AI_TYPE_LIGHT,                                            /**< 指示灯状态检测  */
    AI_TYPE_HELMET,                                           /**< 安全帽检测 */
    AI_TYPE_OCRTEXT,                                          /**< 文本识别 */
	AI_TYPE_GNDWIRE,                                          /**< 接地线识别 */
	AI_TYPE_SAFETYRULE,                                       /**< 安规事件 */  
    AI_TYPE_TOWERLEAN,                                        /**< 构架倾斜识别 */
	AI_TYPE_SNAPMATCH,                                        /**< 人脸比对识别 */
	AI_TYPE_ELECTRICFENCE,                                    /** < 电子围栏 */
	AI_TYPE_GATE,                                             /**< 道闸状态 */
	AI_TYPE_JUEYUAN,                                          /**< 外观检测-绝缘子 */
	AI_TYPE_GANTA,											  /**< 外观检测-杆塔 */
	AI_TYPE_THERMOMETRYALARM,                                 /**< 红外温度告警 */
	AI_TYPE_PTZ_ALARM,                                        /**< 云台控制告警 */
	AI_YX_COUNT,
    AI_TYPE_LEDDIGIT  = 30,                                   /**< 表计读数检测(七段数码管) */
    AI_TYPE_METER,                                            /**< 表计读数检测(刻度盘) */
    AI_TYPE_NUMPEOPLE,                                        /**< 人数统计 */
    AI_TYPE_THERMOMETRY,                                      /**< 测温 */
	AI_TYPE_AREAPEOPLE,                                       /**< 区域人数统计 */	
	AI_TYPE_PASSFLOW,                                         /**< 客流量人数统计 */
};

enum
{
    PLATE_TIANDY_STANDARD = 1,                                /**< 天地车牌识别机(标准) */
    PLATE_DAHUA_STARNDARD,                                    /**< 大华车牌识别机(标准) */
    PLATE_HIK_STARNDARD,                                      /**< 海康车牌识别机(标准) */
    PLATE_HIK_NORMAL,                                         /**< 海康车牌识别机(普通) */
};

typedef struct
{
    S8                      s8Plate[12];                      /**< 车牌号 */
    U8                      u8PlateType;                      /**< 车牌类型 */
    U8                      u8PlateColor;                     /**< 车牌颜色 */
    U8                      u8CarColor;                       /**< 车辆颜色 */
    U8                      u8DevType;                        /**< 设备类型 如:PLATE_TIANDY_STANDARD*/
    U16                     u16Millsecond;                    /**< 抓拍毫秒 */
    U8                      u8CarType;                        /**< 车辆类型 1:小汽车 2:卡车 3:面包车 0:其它*/
    U8                      u8Direct;                         /**< 进出类型 1:进 2:出 0:未知*/
    U32                     u32PicNo;                         /**< 天地伟业ftp抓拍图片名称序号 */
} AI_PLATE_S;

/* 智能分析点坐标 */
typedef struct
{
    U16                     u16Px;
    U16                     u16Py;
} AI_POINT_S;

/* 智能分析矩形报警 */
typedef struct
{
    AI_POINT_S              stFrom;                           /**< 矩形左上点 */
    AI_POINT_S              stTo;                             /**< 矩形右下点 */
    U8                      u8Enable;                         /**< 是否有效 */
    U8                      u8Alarm;                          /**< 是否报警 */
    U8                      u8Reserved[2];
} AI_RECTANGLE_S;

/* 智能分析报警颜色(车牌识别) */
enum AI_COLOR_S
{
    AI_COLOR_BLACK = 0,                                       /**< 黑色 */
    AI_COLOR_GRAY,                                            /**< 灰色 */
    AI_COLOR_WHITE,                                           /**< 白色 */
    AI_COLOR_RED,                                             /**< 红色 */
    AI_COLOR_YELLOW,                                          /**< 黄色 */
    AI_COLOR_GREEN,                                           /**< 绿色 */
    AI_COLOR_BLUE,                                            /**< 蓝色 */
    AI_COLOR_PURPLE,                                          /**< 紫色 */
    AI_COLOR_DARKGRAY,                                        /**< 深灰*/
    AI_COLOR_BKAIR,                                           /**< 民航黑色(海康) */
    AI_COLOR_SILVER,                                          /**< 银色 */
    AI_COLOR_DARKBLUE,                                        /**< 深蓝 */
    AI_COLOR_BROWN,                                           /**< 棕色 */
    AI_COLOR_PINK,                                            /**< 粉色 */
    AI_COLOR_CYAN,                                            /**< 青色 */
    AI_COLOR_ORANGE_RED,                                      /**< 橙红 */
    AI_COLOR_ORGANE,                                          /**< 橙色 */
    AI_COLOR_LIGHT_BLUE,                                      /**< 淡蓝 */
    AI_COLOR_LIGHT_YELLOW,                                    /**< 淡黄 */
    AI_COLOR_LIGHT_CYAN,                                      /**< 淡青 */
    AI_COLOR_LIGHT_GREEN,                                     /**< 淡绿 */
    AI_COLOR_LIGHT_PURPLE,                                    /**< 淡紫 */
    AI_COLOR_YELLOW_GREEN,                                    /**< 黄绿 */
    AI_COLOR_GOLDEN,                                          /**< 金色 */
    AI_COLOR_BLUE_GRAY,                                       /**< 蓝灰 */
    AI_COLOR_GREEN_GRAY,                                      /**< 绿灰 */
    AI_COLOR_CREAM,                                           /**< 米色 */
    AI_COLOR_CYAN_GREEN,                                      /**< 青绿 */
    AI_COLOR_DARK_RED,                                        /**< 深红 */
    AI_COLOR_SKY_BLUE,                                        /**< 天蓝 */
    AI_COLOR_SILVER_GRAY,                                     /**< 银灰 */
    AI_COLOR_PURPLE_RED,                                      /**< 紫红 */
    AI_COLOR_BROWN_RED,                                       /**< 棕红 */
    AI_COLOR_BROWN_YELLOW,                                    /**< 棕黄 */
    AI_COLOR_FLESH,                                           /**< 肉色 */
    AI_COLOR_UNKNOWN = 100,
};

/* 拌线坐标定义 */
typedef struct
{
    AI_POINT_S              stFrom;                           /**< 起始点   */
    AI_POINT_S              stTo;                             /**< 终点     */

    U8                      u8Direct ;                        /**< 是否双向 */
    U8                      u8Enable ;                        /**< 是否有效 */
    U8                      u8Alarm;                          /**< 是否报警 */
    U8                      u8Reserved[1];
} AI_TRIPWIRE_S;


typedef struct
{
    union
    {
        struct
        {
            AI_POINT_S      stFrom;                           /**< 矩形左上点 */
            AI_POINT_S      stTo;                             /**< 矩形右下点 */
        } stAiRect;

        AI_POINT_S          stPts[10];                        /**< 多边形点序列(最多10个点) */
    };

    U8                      u8Type;                           /**< 区域类型 1-矩形、2-多边形*/
    U8                      u8Enable;                         /**< 是否有效 */
    U8                      u8Alarm;                          /**< 是否报警 */
    U8                      u8Reserved[1];
} AI_PERIMETER_S;

typedef struct
{
    union
    {
        struct
        {
            AI_POINT_S      stFrom;                           /**< 矩形左上点 */
            AI_POINT_S      stTo;                             /**< 矩形右下点 */
        } stAiRect;

        AI_POINT_S          stPts[10];                        /**< 多边形点序列(最多10个点) */
    };

    U8                      u8Type;                           /**< 区域类型 1-矩形、2-多边形*/
    U8                      u8Enable;                         /**< 是否有效 */
    U8                      u8Alarm;                          /**< 是否报警 */
    U8                      u8Reserved[1];
} AI_ITEMLEFT_S;

typedef struct
{
    union
    {
        struct
        {
            AI_POINT_S      stFrom;                           /**< 矩形左上点 */
            AI_POINT_S      stTo;                             /**< 矩形右下点 */
        } stAiRect;

        AI_POINT_S          stPts[10];                        /**< 多边形点序列(最多10个点) */
    };

    U8                      u8Type;                           /**< 区域类型 1-矩形、2-多边形*/
    U8                      u8Enable;                         /**< 是否有效 */
    U8                      u8Alarm;                          /**< 是否报警 */
    U8                      u8Reserved[1];
} AI_HOVER_S;

typedef struct
{
    union
    {
        struct
        {
            AI_POINT_S      stFrom;                           /**< 矩形左上点 */
            AI_POINT_S      stTo;                             /**< 矩形右下点 */
        } stAiRect;

        AI_POINT_S          stPts[10];                        /**< 多边形点序列(最多10个点) */
    };

    U8                      u8Type;                           /**< 区域类型 1-矩形、2-多边形*/
    U8                      u8Enable;                         /**< 是否有效 */
    U8                      u8Alarm;                          /**< 是否报警 */
    U8                      u8Reserved[1];
} AI_ANTITHEFT_S;

typedef struct
{
    union
    {
        struct
        {
            AI_POINT_S      stFrom;                           /**< 矩形左上点 */
            AI_POINT_S      stTo;                             /**< 矩形右下点 */
        } stAiRect;

        AI_POINT_S          stPts[10];                        /**< 多边形点序列(最多10个点) */
    };

    U8                      u8Type;                           /**< 区域类型 1-矩形、2-多边形*/
    U8                      u8Enable;                         /**< 是否有效 */
    U8                      u8Alarm;                          /**< 是否报警 */
    U8                      u8Reserved[1];
} AI_OFFJOB_S;

typedef struct
{
    union
    {
        struct
        {
            AI_POINT_S      stFrom;                           /**< 矩形左上点 */
            AI_POINT_S      stTo;                             /**< 矩形右下点 */
        } stAiRect;

        AI_POINT_S          stPts[10];                        /**< 多边形点序列(最多10个点) */
    };

    U8                      u8Type;                           /**< 区域类型 1-矩形、2-多边形*/
    U8                      u8Enable;                         /**< 是否有效 */
    U8                      u8Alarm;                          /**< 是否报警 */
    U8                      u8Reserved[1];
} AI_MAINTENAREA_S;

typedef struct
{
    AI_POINT_S              stFrom;                           /**< 矩形左上点 */
    AI_POINT_S              stTo;                             /**< 矩形右下点 */
    AI_COLOR_S              enAiColor;                        /**< 颜色 */
    U8                      u8OnOff;                          /**< 亮灭或开关状态 */
    U8                      u8Enable;                         /**< 是否有效 */
    U8                      u8Reserved[1];
} AI_LIGHT_S;

typedef struct
{
    AI_POINT_S              stPts[3];                         /**< 中点，始末点*/
    U8                      u8DialType;                       /**< 刻度盘类型  */
    U8                      u8Enable;                         /**< 是否有效 */
    U8                      u8Reserved[2];

    FLOAT                   fDialValue;                       /**< 刻度盘当前值 */
} AI_METER_S;

typedef struct
{
    AI_POINT_S              stFrom;                           /**< 起始点   */
    AI_POINT_S              stTo;                             /**< 终点     */
    U8                      u8Enable;                         /**< 是否有效 */
    U8                      u8Alarm;                          /**< 是否报警 */
    U8                      u8Reserved[2];
} AI_INDICATOR_S;

typedef struct
{
    AI_POINT_S              stFrom;                           /**< 起始点   */
    AI_POINT_S              stTo;                             /**< 终点     */

    U8                      u8Enable;                         /**< 是否有效 */
    U8                      u8Alarm;                          /**< 是否报警 */
    FLOAT                   fValue;                           /**< 表记数值 */
    U8                      u8DecNum;                         /**< 小数位数 */
    U8                      u8Reserved[3];
} AI_LEDDIGIT_S;

typedef struct
{
    U8						s8IsChange[MAX_OBJ_PRESSPLATE];/** < 判断是否改变,1改变，0不变*/
    S8						s8CurState[MAX_OBJ_PRESSPLATE];   /**< 当前状态 0(压板退出状态),1(压板投入状态) */
    U8						u8Enable[MAX_OBJ_PRESSPLATE];     /**< 当前状态是否有效*/
    U8						u8PressPlateNum;             /**< 压板数目 */
    U8						u8Reserved[3];
} AI_PRESSPLATE_S;

typedef struct
{
    AI_POINT_S              stFrom;                           /**< 起始点   */
    AI_POINT_S              stTo;                             /**< 终点     */

    U8                      u8Alarm;                          /**< 报警标记 0:分 1:合 */
    U8                      u8Enable;
    U8                      u8Reserved[2];                    /**< 预留 */
} AI_DISCONNECTOR_S;

typedef struct
{
    AI_RECTANGLE_S          stMovPosition;                    /**< 移动轮廓矩形框 */
    U8                      u8Alarm;                          /**< 报警标记 0:未报警 1:报警 */
    U8                      u8Enable;
    U8                      u8Reserved[2];                    /**< 预留 */
} AI_HELMET_S;

typedef struct
{
    AI_POINT_S              stFrom;                          /**< 起始点   */
    AI_POINT_S              stTo;                            /**< 终点     */

    U32                     u32NumPeople;
    U8                      u8Enable;
    U8                      u8Direct;
    U8                      u8Reserved[2];
} AI_NUMPEOPLE_S;

typedef struct
{
    FLOAT            fValue;
    U8               u8Enable;
    U8               u8Reserved[3];
} AI_THERMOMETRY_S;

typedef struct
{
	AI_POINT_S        stFrom;          /**< 起始点   */
    AI_POINT_S        stTo;            /**< 终点     */
    S8                szText[256];
	U8                u8Enable;
}AI_OCRTEXT_S;

typedef struct 
{
    union
    {
        struct
        {
            AI_POINT_S      stFrom;                           /**< 矩形左上点 */
            AI_POINT_S      stTo;                             /**< 矩形右下点 */
        } stAiRect;
		
        AI_POINT_S          stPts[10];                        /**< 多边形点序列(最多10个点) */
    };
	
    U8                      u8Type;                           /**< 区域类型 1-矩形、2-多边形*/
    U8                      u8Enable;                         /**< 是否有效 　*/
	U8                      u8Res[2]; 
}AI_GNDWIRE_S;

typedef struct 
{
	AI_POINT_S        stFrom;          /**< 起始点   */
    AI_POINT_S        stTo;            /**< 终点     */
	
    U8                u8Alarm;         /**< 报警标记*/
    U8                u8Enable;
    U8                u8Reserved[2];   /**< 预留 */
}AI_SAFETYRULE_S;

typedef struct
{
	union
	{
		struct
		{
			AI_POINT_S      stFrom;                           /**< 矩形左上点 */
			AI_POINT_S      stTo;                             /**< 矩形右下点 */
		} stAiRect;
		
		AI_POINT_S          stPts[10];                        /**< 多边形点序列(最多10个点) */
	};
	
	U16                     u16PeopelNum;                     /**< 人数统计　*/
	U8                      u8Enable;                         /**< 是否有效 　*/
	U8                      u8Res[1];
}AI_AREAPEOPLE_S;

typedef struct  
{
	AI_POINT_S        stFrom;          /**< 起始点   */
    AI_POINT_S        stTo;            /**< 终点     */
	
    U8                u8Alarm;         /**< 报警标记*/
    U8                u8Enable;
    U8                u8Reserved[2];   /**< 预留 */
}AI_TOWERLEAN_S;

typedef struct  
{
	float             fSimilarity;
	S8                s8PersionID[256];
	S8                s8RealSnapPic[MAX_PATH];
	S8                s8BackInputPic[MAX_PATH];
}AI_SNAPMATCH_S;

typedef struct  
{
	DWORD             dwInPerson;
	DWORD             dwOutPerson;
}AI_PASSFLOW_S;

typedef struct
{
    AI_POINT_S              stFrom;                           /**< 起始点   */
    AI_POINT_S              stTo;                             /**< 终点     */
    U8                      u8Enable;                         /**< 是否有效 */
    U8                      u8Alarm;                          /**< 是否报警 */
    U8                      u8Reserved[2];
	
}AI_GATE_S;
typedef struct
{
    AI_POINT_S              stFrom;                           /**< 起始点   */
    AI_POINT_S              stTo;                             /**< 终点     */
    U8                      u8Enable;                         /**< 是否有效 */
    U8                      u8Alarm;                          /**< 是否报警 */
    U8                      u8Reserved[2];
	
}AI_THERMOMETRY_ALARM_S;

typedef struct
{
    AI_POINT_S              stFrom;                           /**< 起始点   */
    AI_POINT_S              stTo;                             /**< 终点     */
    U8                      u8Enable;                         /**< 是否有效 */
    U8                      u8Alarm;                          /**< 是否报警 */
    U8                      u8Reserved[2];
	
}AI_PTZ_ALARM_S;

typedef struct
{
    AI_POINT_S              stFrom;                           /**< 起始点   */
    AI_POINT_S              stTo;                             /**< 终点     */
    U8                      u8Enable;                         /**< 是否有效 */
    U8                      u8Alarm;                          /**< 是否报警 */
    U8                      u8Reserved[2];
	
}AI_JUEYUAN_S;

typedef struct
{
    AI_POINT_S              stFrom;                           /**< 起始点   */
    AI_POINT_S              stTo;                             /**< 终点     */
    U8                      u8Enable;                         /**< 是否有效 */
    U8                      u8Alarm;                          /**< 是否报警 */
    U8                      u8Reserved[2];
	
}AI_GANTA_S;

typedef struct  
{
    AI_POINT_S        stPts[10];       /**< 多边形点序列(最多10个点) */
	U8                u8Alarm;         /**< 报警标记*/
    U8                u8Enable;
    U8                u8Reserved[2];   /**< 预留 */
}AI_ELECTRICFENCE_S;

typedef struct
{
    DATETIME_S              stTime;                           /**< 报警产生时间 */

    U8                      u8AiType;                         /**< 智能分析类型, 1-拌线 2-周界 */
    U8                      u8ChnNo;                          /**< 智能分析通道号 */
    U8                      u8Preset;                         /**< 智能分析通道预置位 */
    U8                      u8AreaType;                       /**< 智能分析目标区域类型 LINE:0, RECT:1, POLY:2 */

    U16                     u16PicHeight;	                  /**< 智能分析图像分辨率高度 */
    U16                     u16PicWidth;                      /**< 智能分析图像分辨率宽度 */
    U32                     s32Status;                        /**< 0:告警结束, 1:告警开始 */
//   U8                      u8AlarmType;                     /**< 报警类型 1:报警 0:轮廓 */

    union
    {

        AI_TRIPWIRE_S       AiTripwite[MAX_AIOBJ_NUM];        /**< 拌线报警 */
        AI_PERIMETER_S      AiPerimeter[MAX_AIOBJ_NUM];       /**< 周界报警 */
        AI_ITEMLEFT_S       AiItemleft[MAX_AIOBJ_NUM];        /**< 物品遗留 */
        AI_PLATE_S          AiPlate;                          /**< 车牌信息 */
        AI_HOVER_S          AiHover[MAX_AIOBJ_NUM];           /**< 徘徊检测  */
        AI_ANTITHEFT_S      AiAntitheft[MAX_AIOBJ_NUM];       /**< 防盗检测   */
        AI_OFFJOB_S         AiOffjob[MAX_AIOBJ_NUM];          /**< 脱岗检测   */
        AI_MAINTENAREA_S    AiMaintenarea[MAX_AIOBJ_NUM];     /**< 检修区域检测 */
        AI_PRESSPLATE_S     AiPressplate;					  /**< 压板投退状态检测 */
        AI_INDICATOR_S      AiIndicator[MAX_AIOBJ_NUM];       /**< 分合指示牌检测 */
        AI_DISCONNECTOR_S   AiDisconnector[MAX_AIOBJ_NUM];    /**< 隔离开关(触头)状态检测 */
        AI_LIGHT_S          AiLight[MAX_AIOBJ_NUM];           /**< 指示灯状态检测  */
        AI_LEDDIGIT_S       AiLeddigit[MAX_AIOBJ_NUM];        /**< 表计读数检测(七段数码管) */
        AI_METER_S          AiMeter[MAX_AIOBJ_NUM];           /**< 表计读数检测(刻度盘) */
        AI_HELMET_S         AiHelmet[MAX_AIOBJ_NUM][20];      /**< 安全帽检测 */
        AI_NUMPEOPLE_S      AiNumPeople[MAX_AIOBJ_NUM];       /**< 人数统计 */
        AI_THERMOMETRY_S    AiThermometry[MAX_AIOBJ_NUM];     /**< 测温 */
		AI_OCRTEXT_S        AiOcrText[MAX_AIOBJ_NUM];         /**< 文本识别 */
        AI_GNDWIRE_S        AiGndWire[MAX_AIOBJ_NUM];         /**< 接地线识别 */
		AI_SAFETYRULE_S     AiSafetyRule[MAX_AIOBJ_NUM];      /**< 安规事件 */
        AI_AREAPEOPLE_S     AiAreaPeople[MAX_AIOBJ_NUM];      /**< 区域人数统计 */
		AI_TOWERLEAN_S      AiTowerLean[MAX_AIOBJ_NUM];       /**< 架构倾斜识别 */
		AI_SNAPMATCH_S      AiSnapMatch;                      /**< 人脸比对识别 */
		AI_PASSFLOW_S       AiPassFlow;                       /**< 客流量人数统计 */
		AI_ELECTRICFENCE_S  AiEleCtriCfence[MAX_AIOBJ_NUM];   /**< 电子围栏 */
		AI_GATE_S			AiGate[MAX_AIOBJ_NUM];               /**< 道闸状态 */	
		AI_THERMOMETRY_ALARM_S AiThermometryAlarm[MAX_AIOBJ_NUM];/**<红外温度告警*/	
		AI_PTZ_ALARM_S      AiPTZControlAlarm[MAX_AIOBJ_NUM];    /**<云台控制告警*/	
		AI_JUEYUAN_S		AiJueYuan[MAX_AIOBJ_NUM];		     /**< 外观检测-绝缘子 */
		AI_GANTA_S			AiGanTa[MAX_AIOBJ_NUM];		         /**< 外观检测-杆塔 */
    };

	S8                      s8PicName[MAX_PATH];                 /**< 智能分析抓拍图片文件名称 */
    
} ALARM_AI_RPT_S;

/**< 系统运行参数 */
typedef struct
{
    time_t                  stStartTime;                       /**< 系统开启时间 */
    time_t                  stRunTime;                         /**< 系统运行时间 */
    U32                     u32ClientNo;                       /**< 客户端链接数 */
    U32                     u32TranTaskNo;                     /**< 转发任务数 */
    U32                     u32CpuUsed;                        /**< 系统使用cpu率 */
    U32                     u32TranCpuUsed;                    /**< 转发任务使用cpu率 */
    U32                     u32PhMemTotal;                     /**< 物理内存总量 */
    U32                     u32PhMemRemain;                    /**< 物理内存可用量 */
    U32                     u32VirMemTotal;                    /**< 虚拟内存总量 */
    U32                     u32VirMemRemain;                   /**< 虚拟内存可用 */
    FLOAT                   fSendNetworkTotal;                 /**< 网络发送总量 */
    FLOAT                   fSendNetworkRemain;                /**< 网络可用量 */
    FLOAT                   fRecvNetworkTotal;                 /**< 网络接收总量 */
    FLOAT                   fRecvNetworkRemain;                /**< 网络接收可用量 */
} ALARM_SYS_RPT_S;

typedef struct
{
    DATETIME_S              stTime;                            /**< 报警产生时间 */
    U8                      u8ChnNo;                           /**< 报警通道号 */
    U8                      u8Status;                          /**< 报警状态，0-开始 1-结束 */
} ALARM_MANUAL_RPT_S;

typedef struct
{
    DATETIME_S              stTime;                            /**< 报警产生时间 */
    S8	                    s8Division[2];                     /* division[0]:'E'or'W'(东经/西经), division[1]:'N'or'S'(北纬/南纬) */
    U8                      u8Satelites;					   /* 卫星数量 */
    U8	                    u8Precision;					   /* 精度因子，原始值*100 */
    U32                     u32Longitude;                      /* 经度，取值范围为（0～180*3600*100），转换公式为：longitude= 实际度*3600*100+实际分*60*100+实际秒*100 */
    U32                     u32Latitude;                       /* 纬度，取值范围为（0～90*3600*100），转换公式为：latitude = 实际度*3600*100+实际分*60*100+实际秒*100 */
    U32	                    u32Direction;                      /* 方向，取值范围为（0～359.9*100），正北方向为0，转换公式为：direction= 实际方向*100 */
    U32	                    u32Speed;                          /* 速度，取值范围为（0～999.9*100000），转换公式为：speed =实际速度*100000，相当于cm/h */
    U32	                    u32Height;						   /* 高度，单位：cm */
} ALARM_GPS_RPT_S;

typedef struct
{
    S8                      s8RecordGuid[MAX_ADDR_LEN];        /**< 登记编码 */
    S8                      s8CarNumber[MAX_ADDR_LEN];         /**< 车牌号 */
    S8                      s8PersonCharge[MAX_ADDR_LEN];      /**< 工作负责人 */
    S8                      s8PersonBooker[MAX_ADDR_LEN];      /**< 工作登记人 */
    S8                      s8IDCardNumber[MAX_ADDR_LEN];      /**< 工作卡号 */
    S8                      s8Unit[MAX_ADDR_LEN];              /**< 单位 */
    S8                      s8Department[MAX_ADDR_LEN];        /**< 部门 */
    S8                      s8Content[MAX_ADDR_LEN];           /**< 工作内容 */
    S8                      s8WorkNumb[MAX_NAME_LEN];          /**< 工作票号 */
    U16                     u16PersonCount;                    /**< 随行人数*/
    S8                      s8CardImgUrl[MAX_NAME_LEN * 2];    /**< 身份图像url */
    S8                      s8CarImgUrl[MAX_NAME_LEN * 2];     /**< 车辆图像url */
    S8                      s8Time[MAX_NAME_LEN * 2];          /**< 时间 */
} ALARM_ACCESS_RPT_S;

typedef struct 
{
    U8                      u8DevIp[MAX_IPV4_LEN];
	U16                     u16DevPort;
	U16                     u16Chn;
	U8                      u8Cmd;
	U8                      u8Res[3];
}ALARM_CALL_RPT_S;

typedef struct  
{
	U8                      u8DevIp[MAX_IPV4_LEN];
	U16                     u16DevPort;
	U16                     u16Chn;
	U16                     u16PtzCmd;
	U8                      u8Preset;
	U8                      u8Res[3];		
}ALARM_PTZ_RPT_S;

typedef struct  
{
	U8                      u8DevIp[MAX_IPV4_LEN];
	U16                     u16DevPort;
	U16                     u16Chn;
	U8                      u8Preset;
	U8                      u8IvaType;
	U8                      u8State;
	U8                      u8Res;
}IVA_STATE_S;

typedef struct
{
	U16                     u16Mode;           // 0:同步所有 1:同步单个变化
	U8                      u8IvaIp[MAX_IPV4_LEN];
	U16                     u16IvaPort;
	U16                     u16Num;
	IVA_STATE_S             ivaList[100];
}ALARM_IVASTATE_RPT_S;

typedef struct
{
	U16  u16Chn;
	U8   u8Status;
	U8   u8Res;
}ALARM_CAMERA_STATUS;

typedef struct
{
	U8   u8DeviceIp[MAX_IPV4_LEN];         //摄像机所在NVR的ip
	U16  u16DevPort;
	ALARM_CAMERA_STATUS camStatus[MAX_CHANNEL_NUM];
}ALARM_CAMERA_RPT_S;


typedef struct
{
    U32                     u32StSize;                         /**< 结构体长度 */
    S8                      s8Name[MAX_NAME_LEN];              /**< 远程客户名称 */
    S32                     s32LocalUserId;                    /**< 远程登录用户ID */
    S8                      s8IP[MAX_IP_STR_LEN];              /**< 远程客户的IP地址 */
    U16                     u16Port;                           /**< 远程客户的端口号 */
    U32                     u32ClientKey;                      /**< 分配客户关键码 */
    U32                     u32DTalkState;                     /**< 音视频对讲状态 */
} CLIENT_INFO_RSP_S;

typedef struct
{
    U32                     u32StSize;                         /**< 结构体长度 */
    S8                      s8AddrCode[MAX_ADDR_LEN];          /**< 设备地址编码 */
    S8                      s8DevName[MAX_NAME_LEN];           /**< 设备名称  */
    S32                     s32DecoderTag;                     /**< 解码插件标签 */
    S32                     s32Longitude;                      /**< 经度 */
    S32                     s32Latitude;                       /**< 纬度 */
} RESOURCE_RSP_S;

typedef struct
{
    U8                      u8ReqType;                         /**< 请求方式，0-IP+名称 1-地址码方式 */
    U8                      u8TalkType;                        /**< 对讲方式，0-音频对讲 1-视频对讲 2-音视频对讲 3-音频喊话 4-视频喊话 5-音视频喊话 */
    S8                      s8LocalName[MAX_NAME_LEN];         /**< 本地名称 */
    U8                      u8LocalIp[MAX_IPV4_LEN];           /**< 本地IP */
    U32                     u32UserSize;                       /**< 用户数据长度 */
} DTALK_MSG_S;

typedef struct
{
    U32                     u32StSize;                         /**< 结构体长度 */
    U32                     u32Id;	                 	       /**< 对象ID号 */
    U32                     u32Type;			               /**< 对象类型，参照UTSYSOBJ_OBJTYPE */
    S8                      s8Name[MAX_OBJECTNAME_LEN];	       /**< 对象名称 */
    U32                     u32ParentId;		               /**< 对象父ID号，如果为-1，表示无父ID */
    U32                     u32ParentType;                     /**< 对象类型，参照UTSYSOBJ_OBJTYPE */
    U8	                    u8DvrIP[MAX_IPV4_LEN];	           /**< 所属dvr设备的ip地址，对象类型为预置位时有用 */
    U32                     u32ChnNo;		                   /**< 预置位对应的DVR设备通道号, 从1开始 */
    U16                     u16Port;		                   /**< 所属dvr设备的侦听端口, 对象类型为预置位时有用*/
    U16	                    u16Preset;		                   /**< 预置点序号 0表示无预置点 序号从1开始 (备注:预置位下的智能分析点共用该字段,序号从1开始)*/
    U8                      u8IsIVS;                           /**< 是否为智能分析 */
    S8                      s8IVSType;                         /**< 智能分析结果类型 0:YX:遥信类型 1:YC：遥测类型 2:YK遥控类型 其它:-1*/
    U8                      u8Reseverd[2];
} DV_PARAMSET_S;

typedef struct
{
    U32                     u32StSize;                         /**< 结构体长度 */
    U32                     u32Id;	                 	       /**< 对象ID号 */
    U32                     u32Type;			               /**< 对象类型，参照UTSYSOBJ_OBJTYPE */
    S8                      s8Name[MAX_OBJECTNAME_LEN];	       /**< 对象名称 */
    U32                     u32ParentId;		               /**< 对象父ID号，如果为-1，表示无父ID */
    U32                     u32ParentType;                     /**< 对象类型，参照UTSYSOBJ_OBJTYPE */
    U8	                    u8DvrIP[MAX_IPV4_LEN];	           /**< 所属dvr设备的ip地址，对象类型为预置位时有用 */
    U32                     u32ChnNo;		                   /**< 预置位对应的DVR设备通道号, 从1开始 */
    U16                     u16Port;		                   /**< 所属dvr设备的侦听端口, 对象类型为预置位时有用*/
    U16	                    u16Preset;		                   /**< 预置点序号 0表示无预置点 序号从1开始 (备注:预置位下的智能分析点共用该字段,序号从1开始)*/
    U8                      u8IsIVS;                           /**< 是否为智能分析 */
    S8                      s8IVSType;                         /**< 智能分析结果类型 0:YX:遥信类型 1:YC：遥测类型 2:YK遥控类型 其它:-1*/
    U8                      u8PresetType;                      /**< 预置位类型 */
    U8                      u8AreaType;                        /**< 区域类型 */
    S8                      s8Desc[MAX_OBJECTNAME_LEN];        /**< 间隔描述 */
} DV_PARAMSET_S_EX;

typedef struct
{
    U32                     u32StSize;
    U32                     u32Id;	                 		   /**< 巡视路线ID或巡视步骤ID*/
    U32                     u32Type;			               /**< 巡视对象类型，参照UT_TOUR_TYPE*/
    S8                      s8Name[MAX_NAME_LEN];		       /**< 巡视路线名称或*巡视设备名称 */
    U32                     u32No;	                 		   /**< 巡视步骤序号 */
    U32                     u32DevId;	                 	   /**< 巡视设备ID */
    U32                     u32ParentId;                       /**< 无父节点则为0*/
    U8	                    u8Hour;                            /**< 巡视时间*/
    U8	                    u8Minute;
    U8	                    u8Second;
    U8                      u8Reseverd[1];
} TL_PARAMSET_S;

#endif

/**
 * 设备连接
 */

/** 登录时消息回调码定义 */
enum
{
    MSG_CONN_START		    = 0,
    MSG_CONN_SUCCESS	    = 1,
    MSG_CONN_FAIL		    = 2,
    MSG_CONN_CLOSED		    = 3,
    MSG_LOGIN_REQUEST	    = 4,
    MSG_LOGIN_SUCCESS	    = 5,
    MSG_LOGIN_USERLOGINED   = 6,
    MSG_LOGIN_NOUSERNAME    = 7,
    MSG_LOGIN_PASSWORDERROR = 8,
    MSG_LOGIN_FAIL		    = 9,
    MSG_LOGIN_NORIGHT       = 10,
    MSG_LOGIN_AAA_AUTH_FAIL = 11,
};

/**
 * @brief  登录状态信息回调函数定义
 *
 * @param [out] u32Code 消息回调码
 * @param [out] pUser   用户数据
 */
typedef VOID (*pfnConnCallback)(U32 u32Code, VOID *pUser);


/**
 * @brief  报警输出数据回调函数定义
 *
 * @param [out] u16Type 报警类型
 * @param [out] pData   报警数据的缓冲区指针
 * @param [out] u32Size 缓冲区大小
 * @param [out] pUser   用户数据
 */
enum
{
    ALARM_IO  = 1,                                               /**< I/O探头告警开始 */
    ALARM_MD,                                                    /**< 移动侦测告警开始 */
    ALARM_VLOSS, 	                                             /**< 视频丢失告警开始 */
    ALARM_VCOVER,                                                /**< 视频遮挡告警开始 */
    ALARM_HDD,                                                   /**< 硬盘告警开始开始 */
    ALARM_AI,  		                                             /**< 智能分析告警开始 */
    REPORT_DEV_STAT,                                             /**< 设备状态上告 */
    LINKAGE_PUSH,                                                /**< 五防联动上传 */
    LINKAGE_TRANSMIT,                                            /**< 客户端联动转发 */
    ALARM_VI,                                                    /**< 智能分析巡警结果告警开始 */
    DEVICE_STATUS_UPLOAD,                                        /**< 设备状态上告 */
    SYSTEM_INFO_UPLOAD,                                          /**< 系统运行参数 */
    ALARM_MANUAL,                                                /**< 外部移动手持设备手动告警 */
    ALARM_GPS,                                                   /**< 外部移动手持设备GPS告警 */
    ALARM_ACCESS,                                                /**< 平板出入登记 */
	ALARM_CALL_STATUS,                                           /**< 门卫机呼叫状态 */
	ALARM_PTZ_CALL,                                              /**< 云台调用告警事件 */
	ALARM_IVA_STATE,                                             /**< 智能分析状态 */
	ALARM_CAMERA_STATE,                                          /**< NVR下摄像机状态告警 */
};

enum
{
    WFLINK_TASK_START  = 1,                                      /* 5防操作票任务开始 */
    WFLINK_TASK_OPERING,                                         /* 5防操作票任务操作 */
    WFLINK_TASK_END, 	                                         /* 5防操作票任务结束 */
    WFLINK_SIMU_OPERING,                                         /* 5防模拟操作 */
    WFLINK_SIMU_SCOUT,  		                                 /* 5防模拟屏巡视 */
};

typedef VOID (*pfnAlarmCallback)(ALARMER_S *pAlarmer, U16 u16Type, VOID *pData, U32 u32Size, VOID *pUser);

typedef struct
{
    S8    * s8Ip;                                                /**< 连接服务器IP */
    U16     u16Port;                                             /**< 连接服务器端口 */
    S8    * ps8Username;                                         /**< 登录用户名 */
    S8    * ps8Password;                                         /**< 登录密码 */
    S8    * ps8Name;                                             /**< 客户端名称 */
    S8    * ps8ClientIp;                                         /**< 客户端IP */
    U16     u16ClientTalkPort;                                   /**< 客户端开放的对讲端口*/
    HWND    hWnd;                                                /**< 登录过程消息递送窗口, 仅windows平台 */
    U32     u32Msg;                                              /**< 登录过程消息类型, 仅windows平台 */
    VOID  * pUser;                                               /**< 用户数据 */
    BOOL    bAutoReconn;                                         /**< 是否重连 */
    BOOL    bAlarmDesired;                                       /**< 是否开启报警订阅 */
    pfnAlarmCallback fAlarmCallback;                             /**< 报警数据回调 */
    pfnConnCallback  fConnCallback;                              /**< 登录消息回调 */
} NET_CONN_PARAM_S;

/**
 * @brief  报警输出数据回调函数定义
 *
 * @param [out] u16Type 联动类型
 * @param [out] pData   联动数据的缓冲区指针
 * @param [out] u32Size 缓冲区大小
 * @param [out] pUser   用户数据
 */
typedef VOID (*pfnWFLinkageCallback)(ALARMER_S *pAlarmer, U16 u16Type, VOID *pData, U32 u32Size, VOID *pUser);

/**
 * @brief  用户列表信息回调函数定义
 *
 * @param [out] pUserInfo 用户信息
 * @param [out] pUser     用户数据
 */
typedef VOID (*pfnUserListCallback)(const USERINFO_RSP_S *pUserRsp, VOID *pUser);
typedef struct
{
    VOID  * pUser;                                               /**< 用户数据 */
    pfnUserListCallback fUserListCallback;                       /**< 用户信息数据回调 */
} NET_USER_PARAM_S;

/**
 * @brief  设备列表信息回调函数定义
 *
 * @param [out] pDevInfo  设备信息
 * @param [out] pUser     用户数据
 */
typedef VOID (*pfnDeviceListCallback)(const DEVINFO_RSP_S *pDevInfo, VOID *pUser);
typedef struct
{
    S8    * ps8AddrCode;                                         /**< 设备地址码 */
    S8    * ps8DeviceIp;                                         /**< 设备IP */
    S8    * ps8DeviceName;                                       /**< 设备名 */
    U16     u16DevicePort;                                       /**< 设备端口 */

    VOID  * pUser;                                               /**< 用户数据 */
    pfnDeviceListCallback fDeviceListCallback;                   /**< 设备列表数据回调 */
} NET_DEVICE_PARAM_S;
/**
 * 预览
 */

/**
 * @brief  码流数据回调函数定义
 *
 * @param [out] hWnd    当前的预览句柄
 * @param [out] pData   存放数据的缓冲区指针,UT_FRAME_HEAD_S+音视频数据
 * @param [out] u32Size 缓冲区大小
 * @param [out] pUser   用户数据
 */
typedef VOID (*pfnRealDataCallback)(HWND hWnd, VOID *pData, U32 u32Size, VOID *pUser);

typedef struct
{
    U32     u32ChnNO;                                            /**< 通道号 0,1,2,3... */
    U8      u8Type;                                              /**< 0-主码流, 1-从码流 */
    U8      u8Content;                                           /**< 0-复合流, 1-视频流, 2-音频流 */

    S8    * ps8FrontAddrCode;                                    /**< 前端设备地址编码, 非空则采用地址码方式查找前端设备，后面配置的IP和端口不起作用 */
    S8    * ps8DeviceIp;                                         /**< 设备IP */
    U16     u16DevicePort;                                       /**< 设备端口 */
    S8    * ps8SubServIp;                                        /**< 二级流媒体服务器IP */
    U16     u16SubServPort;                                      /**< 二级流媒体服务器端口 */

    U32     u32Proto;                                            /**< 0-TCP,1-UDP */
    BOOL    bAutoReconn;                                         /**< 是否重连 */
    VOID  * pUser;                                               /**< 用户数据 */
    HWND    hWnd;                                                /**< 播放窗口的句柄，为NULL表示不显示图像 */
    U32     u32Msg;
    pfnRealDataCallback fRealDataCallback;                       /**< 码流数据回调函数 */
} NET_PVIEW_PARAM_S;

typedef struct
{
    U32     u32ChnNO;                                            /**< 通道号 0,1,2,3... */
    U8      u8Type;                                              /**< 0-主码流, 1-从码流 */
    U8      u8Reserved[3];

    S8    * ps8FrontAddrCode;                                    /**< 前端设备地址编码 */
    S8    * ps8DeviceIp;                                         /**< 设备IP地址 */
    U16     u16DevicePort;                                       /**< 设备端口号 */
    S8    * ps8SubServIp;                                        /**< 二级流媒体服务器地址 */
    U16     u16SubServPort;                                      /**< 二级流媒体端口号 */
} NET_REQI_PARAM_S;

/**
 * 预览画面切换
 */
typedef enum
{
    SHOW_MODE_1     = 1,                                         /**< 单画面 */
    SHOW_MODE_4     = 4,                                         /**< 4画面 */
    SHOW_MODE_9     = 9,                                         /**< 9画面 */
    SHOW_MODE_16    = 16,                                        /**< 16画面 */
} NET_SHOW_MODE_E;

typedef struct
{
    U32     u32Mode;                                             /**< NET_SHOW_MODE_E值 */
    U32     u32Channel;                                          /**< 目标切换通道值, 一位对应一个通道 */

    S8    * ps8DeviceIp;                                         /**< 设备IP地址 */
    U16     u16DevicePort;                                       /**< 设备端口号 */
    S8    * ps8SubServIp;                                        /**< 二级流媒体服务器地址 */
    U16     u16SubServPort;                                      /**< 二级流媒体端口号 */
} NET_SWITCH_PARAM_S;


/**
 * 矩阵切换
 */
typedef struct
{
    U32     u32InPort;                                           /**< 矩阵输入端口号 */
    U32     u32OutPort;                                          /**< 矩阵输出端口号 */

    S8    * ps8DeviceIp;                                         /**< 设备IP地址 */
    U16     u16DevicePort;                                       /**< 设备端口号 */
    S8    * ps8SubServIp;                                        /**< 二级流媒体服务器地址 */
    U16     u16SubServPort;                                      /**< 二级流媒体端口号 */
} NET_MATRIX_PARAM_S;

/**
 * 解码器画面切换
 */
typedef struct
{
    U32     u32Mode;                                              /**< 显示模式，NET_SHOW_MODE_E*/
    U32     u32Channel;                                           /**< 需要解码显示的通道，一位对应一个通道 */
    U32     u32OutPort;                                           /**< 显示输出端口, 从1开始*/

    S8    * ps8DeviceIp;                                          /**< 设备IP地址 */
    U16     u16DevicePort;                                        /**< 设备端口号 */
    S8    * ps8SubServIp;                                         /**< 二级流媒体服务器地址 */
    U16     u16SubServPort;                                       /**< 二级流媒体端口号 */
} NET_DECODER_PARAM_S;

typedef struct
{
    U32     u32Mode;                                              /**< 显示模式，NET_SHOW_MODE_E*/
    U32     u32OutPort;                                           /**< 显示输出端口 */

    S8    * ps8DeviceIp;                                          /**< 设备IP地址 */
    U16     u16DevicePort;                                        /**< 设备端口号 */
    S8    * ps8SubServIp;                                         /**< 二级流媒体服务器地址 */
    U16     u16SubServPort;                                       /**< 二级流媒体端口号 */
} NET_DEC_DIS_PARAM_S;

/* 流媒体服务器基本配置 */
typedef struct
{
    S8    * ps8Url;                                               /**< 流媒体服务器IP */

    U16     u16Port;	                                          /**< 流媒体服务器端口 */

    S8    * ps8Username;                                          /**< 登录用户名 */
    S8    * ps8Password;                                          /**< 登录用密码 */

    U32     u32TransProtocol;	                                  /**< 传输协议类型 0-TCP，1-UDP*/
} NET_DEC_STREAM_CFG_S;

/* DVR设备通道信息 */
typedef struct
{
    S8    * ps8Ip;                                                /**< DVR IP地址 */
    U16     u16Port;                                              /**< DVR 端口号 */

    S8    * ps8Username;                                          /**< DVR 登录用户名 */
    S8    * ps8Password;                                          /**< DVR 登录用密码 */

    U32     u32ChnNo;                                             /**< DVR 通道号, 从0开始 */
    U32     u32TransProtocol;		                              /**< 传输协议类型0-TCP，1-UDP */
    U32     u32TransMode;			                              /**< 传输码流模式 0－主码流 1－子码流 */
    U32     u32ProtoType;			                              /**< 前端设备协议类型 */
} NET_DEC_DVR_CFG_S;

typedef struct
{
    BOOL    bValidStream;                                         /**< 流媒体服务器是否可用 */
    union
    {
        NET_DEC_DVR_CFG_S    stDvrCfg;
        NET_DEC_STREAM_CFG_S stStreamCfg;
    };
} NET_DEC_CHN_PARAM_S;

enum
{
    CAP_BMP_MODE    = 0,                                          /**< BMP格式 */
    CAP_JPEG_MODE   = 1                                           /**< jpeg格式 */
};

typedef struct
{
    U8      u8PortType;                                           /**< 端口类型。1-server，2-client */
    S8 *    ps8LocalIp;                                           /**< 对讲本地IP地址 */
    U16     u16LocalPort;                                         /**< 对讲本地端口号 */
    S8 *    ps8RemoteIp;                                          /**< 对讲对方IP地址 */
    U16     u16RemotePort;                                        /**< 对讲对方端口号 */
} PC_TALK_PARAM_S;
/**
 * 云台控制
 */
enum
{
    PTZ_CMD_UNKNOW = 0,
    PTZ_CMD_UP = 1,
    PTZ_CMD_DOWN,
    PTZ_CMD_LEFT,
    PTZ_CMD_RIGHT,
    PTZ_CMD_UPLEFT = 5,
    PTZ_CMD_UPRIGHT,
    PTZ_CMD_DOWNLEFT,
    PTZ_CMD_DOWNRIGHT,
    PTZ_CMD_ZOOMIN,
    PTZ_CMD_ZOOMOUT = 10,
    PTZ_CMD_FOCUSNEAR,
    PTZ_CMD_FOCUSFAR,
    PTZ_CMD_IRISOPEN,
    PTZ_CMD_IRISCLOSE,
    PTZ_CMD_AUTOSCAN = 15,
    PTZ_CMD_CRUISE,
    PTZ_CMD_SETPRESET,
    PTZ_CMD_CLEARPRESET,
    PTZ_CMD_CALLPRESET,
    PTZ_CMD_RAIN_ON,
    PTZ_CMD_RAIN_OFF,
    PTZ_CMD_3D_POSITION,
};

typedef struct
{
    U32                   u32ChnNO;                               /**< 设备通道号, 从0开始,轨道车代表地址码  */
    S8                  * ps8FrontAddrCode;                       /**< 前端设备地址编码 */

    S8                  * ps8DeviceIp;                            /**< 设备IP地址 */
    U16                   u16DevicePort;                          /**< 设备端口号 */
    S8                  * ps8SubServIp;                           /**< 二级流媒体服务器地址 */
    U16                   u16SubServPort;                         /**< 二级流媒体端口号 */
    union
    {
        PTZ_CTRL_NORMAL_S stNormal;                               /**< 通常控制参数 */
        PTZ_CTRL_CRUISE_S stCruise;                               /**< 巡航控制参数 */
        PTZ_CTRL_PRESET_S stPreset;                               /**< 预置点控制参数 */
    };
} NET_PTZ_PARAM_S;

typedef struct
{
    U32         u32ChnNO;                                         /**< 设备通道号, 从0开始 */
    S8        * ps8FrontAddrCode;                                 /**< 前端设备地址编码 */

    S8        * ps8DeviceIp;                                      /**< 设备IP地址 */
    U16         u16DevicePort;                                    /**< 设备端口号 */
    S8        * ps8SubServIp;                                     /**< 二级流媒体服务器地址 */
    U16         u16SubServPort;                                   /**< 二级流媒体端口号 */
} NET_CHANNEL_PARAM_S;

/**
 * 录像查询
 */
enum
{
    QRECORD_DETAIL = 1,                                           /**< 查找给定时间段录像列表 */
    QRECORD_CALENDAR,                                             /**< 查找某月哪些天有录像 */
    QRECORD_DAY_HALFHOUR,                                         /**< 查找某天哪半小时有录像 */
};

/**
 * @brief  录像文件列表回调函数定义
 *
 * @param [out] pRsp   录像文件详细信息
 * @param [out] pUser  用户数据
 *
 */
typedef VOID (*pfnQueryDetailCallback)(const REC_DETAIL_RSP_S   *pRsp, VOID *pUser);

/**
 * @brief  哪些天有录像回调函数定义
 *
 * @param [out] pRsp   录像文件详细信息
 * @param [out] pUser  用户数据
 *
 */
typedef VOID (*pfnQueryCalendarCallback)(const REC_CALENDAR_RSP_S *pRsp, VOID *pUser);
typedef struct
{
    U32         u32QType;                                         /**< QRECORD_DETAIL, QRECORD_CALENDAR */

    U32         u32ChnNO;                                         /**< 设备通道号, 从0开始  */
    U32         u32Type;                                          /**< RECORD_NORMAL, RECORD_MOTION等值 */
    DATETIME_S  stBegin;                                          /**< 搜索开始时间 */
    DATETIME_S  stEnd;                                            /**< 搜索结束时间 */

    S8        * ps8FrontAddrCode;                                 /**< 前端设备地址编码 */
    S8        * ps8DeviceIp;                                      /**< 设备IP地址 */
    U16         u16DevicePort;                                    /**< 设备端口号 */
    S8        * ps8SubServIp;                                     /**< 二级流媒体服务器地址 */
    U16         u16SubServPort;                                   /**< 二级流媒体端口号 */

    VOID      * pUser;                                            /**< 用户数据 */
    pfnQueryDetailCallback   fDetailCallback;                     /**< 录像列表回调 */
    pfnQueryCalendarCallback fCalendarCallback;                   /**< 哪些天有录像回调 */
} NET_QRECORD_PARAM_S;

typedef struct
{
    NET_QRECORD_PARAM_S * pRecordReq;
    U32                   u32TriggerType;                         /**< 3--端口报警；4--移动报警；5--视频丢失报警；0x7FFFFFFF--无效*/
    U32                   u32TriggerPort;                         /**< u32TriggerType=3时，改参数为端口号*/
} NET_QRECORD_PARAM_EX_S;


#define NET_PLAYSTART		1                                     /**< 开始播放 */
#define NET_PLAYSTOP		2                                     /**< 停止播放 */
#define NET_PLAYPAUSE		3                                     /**< 暂停播放 */
#define NET_PLAYRESTART		4                                     /**< 恢复播放 */
#define NET_PLAYFAST		5                                     /**< 快放 */
#define NET_PLAYSLOW		6                                     /**< 慢放 */
#define NET_PLAYNORMAL		7                                     /**< 正常速度 */
#define NET_PLAYSTARTAUDIO	9                                     /**< 打开声音 */
#define NET_PLAYSTOPAUDIO	10                                    /**< 关闭声音 */
#define NET_PLAYSETPOS		12                                    /**< 改变文件回放的进度 */
#define NET_PLAYSETTIME     13                                    /**< 按绝对时间定位 */
#define NET_PLAYSTEP        14                                    /**< 单帧播放 */
#define NET_STARTREPOS      15                                    /**< 开始拖动进度条，按下拖拽滑块时调用 */


/**
 * 录像回放
 */

/**
 * @brief  录像回放码流数据回调函数定义
 *
 * @param [out] hWnd    当前的回放句柄
 * @param [out] pData   存放数据的缓冲区指针
 * @param [out] u32Size 缓冲区大小
 * @param [out] pUser   用户数据
 *
 * @return false-告诉内部缓冲区已满，并请求数据
 */
typedef BOOL (*pfnRecordDataCallback)(HWND hWnd, VOID *pData, U32 u32Size, VOID *pUser);

typedef struct
{
    U8                          u8PlayMode;                       /**< 请求方式，参考REC_PLAY_IP_TIME ... */
    U8                          u8Reserved[3];

    union
    {
        struct
        {
            S8                * ps8FrontAddrCode;                 /**< 前端设备地址编码 */
            union
            {
                S8            * ps8RtspUrl;                       /**< rtsp url路径 */

                struct
                {
                    DATETIME_S  stBegin;                          /**< 回放开始时间 */
                    DATETIME_S  stEnd;                            /**< 回放结束时间 */
                };
            };
        } stAddrMode;

        struct
        {
            union
            {
                S8            * ps8FileName;                      /**< 录像文件名*/
                struct
                {
                    U32         u32ChnNO;                         /**< 设备通道号, 从0开始 */
                    U32         u32Type;                          /**< RECORD_NORMAL, RECORD_ALARM等值 */
                    DATETIME_S  stBegin;                          /**< 回放开始时间 */
                    DATETIME_S  stEnd;                            /**< 回放结束时间 */
                };
            };

            S8                * ps8DeviceIp;                      /**< 设备IP地址 */
            U16                 u16DevicePort;                    /**< 设备端口号 */
            S8                * ps8SubServIp;                     /**< 二级流媒体服务器地址 */
            U16                 u16SubServPort;                   /**< 二级流媒体端口号 */
        } stIpMode;

		struct
		{
            S8            * ps8PlayUrl;                     /**< 录像url*/
			DATETIME_S    stBegin;                          /**< 回放开始时间 */
            DATETIME_S    stEnd;                            /**< 回放结束时间 */
			S8            * ps8DeviceIp;                    /**< 设备IP地址 */
            U16           u16DevicePort;                    /**< 设备端口号 */
			U16           u16ChnNo;                         /**< 设备通道号, 从0开始 */
		}stHikMode;
    };

    VOID                      * pUser;                            /**< 用户数据 */
    HWND                        hWnd;                             /**< 播放窗口的句柄，为NULL表示不显示图像 */
    U32                         u32Msg;
    pfnRecordDataCallback       fRecordDataCallback;              /**< 回放数据回调函数 */
} NET_RECPLAY_PARAM_S;

/**
 * 录像下载
 */

/** 录像下载消息回调码定义 */
enum
{
    MSG_DOWNLOAD_CLOSED    = 1,                    /**< 所有下载结束 */
    MSG_DOWNLOAD_OK        = 2,                    /**< 下载完一个文件 */
    MSG_DOWNLOAD_NOFILE    = 3,                    /**< 一个文件不存在 */
    MSG_DOWNLOAD_FAIL      = 4,                    /**< 该文件下载失败 */
    MSG_DOWNLOAD_STOREFAIL = 5,                    /**< 该文件存储失败 */

    MSG_PLAYBACK_END       = 20,                   /**< 该文件回放结束 */
};

/**
 * @brief  录像下载码流数据回调函数定义
 *
 * @param [out] pData   存放数据的缓冲区指针
 * @param [out] u32Size 缓冲区大小
 * @param [out] pUser   用户数据
 */
typedef BOOL (*pfnRecordDownloadDataCallback)(VOID *pData, U32 u32Size, VOID *pUser);
/**
 * @brief  录像下载状态数据回调函数定义
 *
 * @param [out] u32Code 录像下载消息回调码
 * @param [out] pUser   用户数据
 */
typedef BOOL (*pfnRecordDownloadStatusCallback)(U32 u32Code, VOID *pUser);

/**
 * @brief  录像回放状态数据回调函数定义
 *
 * @param [out] u32Code 录像下载消息回调码
 * @param [out] pUser   用户数据
 */
typedef VOID (*pfnRecordPlaybackStatusCallback)(U32 u32Code, VOID *pUser);

typedef struct
{
    S8        * ps8FrontAddrCode;                  /**< 设备地址码 */
    S8        * ps8RecFileName;                    /**< 录像文件名 */
    S8        * ps8FileName;                       /**< 保存文件路径 */
    U32         u32FileSize;                       /**< 录像文件长度 */

    S8        * ps8DeviceIp;                       /**< 设备IP地址 */
    U16         u16DevicePort;                     /**< 设备端口号 */
    S8        * ps8SubServIp;                      /**< 二级流媒体服务器地址 */
    U16         u16SubServPort;                    /**< 二级流媒体端口号 */

    VOID      * pUser;                             /**< 用户数据 */
    pfnRecordDownloadDataCallback   fRecordDataCallback;/**< 下载录像数据回调 */
    pfnRecordDownloadStatusCallback fStatusCallback;    /**< 下载状态消息回调 */
} NET_RECDOWNLOAD_PARAM_S;


typedef struct
{
    S8        * ps8FrontAddrCode;                  /**< 设备地址码 */
    S8        * ps8FileName;                       /**< 保存文件路径 */
    U32         u32ChnNO;                          /**< 设备通道号, 从0开始 */

    DATETIME_S  stBegin;                           /**< 下载开始时间 */
    DATETIME_S  stEnd;                             /**< 下载结束时间 */

    S8        * ps8DeviceIp;                       /**< 设备IP地址 */
    U16         u16DevicePort;                     /**< 设备端口号 */
    S8        * ps8SubServIp;                      /**< 二级流媒体服务器地址 */
    U16         u16SubServPort;                    /**< 二级流媒体端口号 */

    VOID      * pUser;                             /**< 用户数据 */
    pfnRecordDownloadDataCallback   fRecordDataCallback;/**< 下载录像数据回调 */
    pfnRecordDownloadStatusCallback fStatusCallback;    /**< 下载状态消息回调 */

} NET_RECDOWNLOAD_BYTIME_PARAM_S;

typedef struct
{
    DATETIME_S  stTime;                            /**< 同步时间值 */
    U8          u8TimeZone;                        /**< 时区 */
    U8          u8IsSetLoaclTime;                  /**< 是否设置本地时间 */
    U8          u8Reserved[2];
} NET_SYNTIME_PARAM_S;


enum
{
    DTALK_CONN_OK,                      /**< 连接OK */
    DTALK_REQ,                          /**< 被动请求 */
    DTALK_ACK,                          /**< 主动请求确认回应 */
    DTALK_NAK,                          /**< 主动请求拒绝回应 */
    DTALK_STOP,                         /**< 被动停止 */
    DTALK_DATA,                         /**< 被动推送数据 */
    DTALK_TALKINKG,                     /**< 正在对讲 */
    DTALK_NO_ACK,                       /**< 对方没响应 */
};

/**
 * @brief  对讲消息、数据回调
 *
 * @param  [out] u32MsgType  消息类型 DTALK_CONN_OK、DTALK_REQ、DTALK_ACK
 * @param  [out] pFromMsg  请求方用户名或地址码 只有u32MsgType为DTALK_REQ、DTALK_STOP才有值，其他为空
 * @param  [out] pUser 用户数据
 *
 * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
 *
 * @see
 */
typedef S32(*pfnDTalkMsgCallback)(U32 u32MsgType, DTALK_MSG_S *pFromMsg, VOID *pUser);
typedef struct
{
    U32                 u32CollectFlag;            /**< 是否内部采集音视频, 内部采集标记 bit0-采集音频 bit1-采集视频 */
    HWND                hRemoteWnd;                /**< 对方视频显示窗口的句柄，为NULL表示不显示图像 */
    HWND                hLocalWnd;                 /**< 本地窗口显示句柄 */
    HWND                hMsgWnd;                   /**< 视频采集消息通知窗口, WM_VIDEO_CAPTURE消息 */
    pfnDTalkMsgCallback fDTalkMsgCallback;         /**< 对讲消息回调函数 */
    pfnRealDataCallback fDTalkDataCallback;        /**< 对讲音视频数据回调函数 */
    VOID              * pUser;                     /**< 用户数据 */
} NET_DTALK_PARAM_S;

typedef struct
{
    U32         u32MsgType;                        /**< 消息类型 1-请求对讲 2-停止对讲 */
    S8        * ps8LocalName;                      /**< 本地名称 */
    S8        * ps8LocalIp;                        /**< 本地IP */

    U8          u8TalkType;                        /**< 对讲方式，0-音频对讲 1-视频对讲 2-音视频对讲 3-音频喊话 4-视频喊话 5-音视频喊话 */
    U8          u8Reserved[1];
    U32         u32UserNum;                        /**< 对讲用户数 */
} NET_DTALK_MSG_S;

typedef struct
{
    S8        * ps8Name;                           /**< 对方IP */
    S8        * ps8Ip;                             /**< 对方IP */
    S8        * ps8UserAddrCode;                   /**< 对方用户地址编码 */
} NET_DTALK_USER_S;

typedef struct
{
    S8        * ps8Ip;                             /**< 设备ip */
    U16         u16Port;                           /**< 设备port */
    U8          u8ChnNo;                           /**< 通道号 */
} NET_DTALK_DEV_INFO;

//************************************************************************
//函数名称：C2DevTalkStatus_CB(int code, void* userdata)
//函数说明：开始对讲状态回调
//函数参数：
//    (in) code 状态码 0:对讲成功,1:连接服务器失败,2:设备占线
//    (in) userdata 用户数据
//返回值：void
//************************************************************************/
typedef void (*C2DevTalkStatus_CB)(int code, void* userdata);


typedef struct
{
    U32                 u32DevListNum;             /**< 对讲设备数 */
    NET_DTALK_DEV_INFO *pDevList;                  /**< 对讲设备列表 */
	C2DevTalkStatus_CB  devTalk_cb;                /**< 对讲状态回调函数 */
	void               *userdata;                  /**< 用户数据 */
} NET_DTALK_DEV_LIST;

/**
 * 参数设置数
 */
enum
{
    PARAM_TYPE_DV = 1,
    PARAM_TYPE_CRUISE,
    PARAM_TYPE_DV_EX,  /* 新归约 */
    PARAM_TYPE_DV_EX0815, /* 新规约扩充预置位类型*/
};

typedef struct
{
    U32         u32Type;
    VOID      * pParamData;
    U32         u32ParamSize;
} NET_PARAMSET_PARAM_S;

#if __cplusplus
extern "C" {
#endif

    /** SDK初始化 */

    /**
     * @brief  初始化SDK，调用其他SDK函数的前提
     *
     * @return 成功返回TRUE, 否则FALSE, 获取错误码调用NET_GetLastError
     *
     * @see    NET_Release
     */
    NET_API BOOL NET_Init();


    /**
     * @brief  释放SDK资源，在程序结束之前调用。
     *
     * @return 成功返回TRUE, 否则FALSE, 获取错误码调用NET_GetLastError
     *
     * @see    NET_Init
     */
    NET_API VOID NET_Release();

    /**
     * @brief  返回最后操作的错误码。
     *
     * @return 返回值为错误码,详见表:
     *
     */
    NET_API UINT NET_GetLastError();

    /** 设备连接 */

    /**
     * @brief  用户登录
     * @param [in] pparam 连接参数配置相关信息，参考NET_CONN_PARAM_S
     * @param [in] u32Mode 连接同模式
     *                      0 - 优特私有模式，默认
     *                      1 - 海康平台
	 *                      2 - 跨平台流媒体
     * @return 成功返回句柄,标示一个已经连接的设备， 失败返回NULL，
     *
     * @note   注意要保存返回的句柄,接下来相关操作都要求传入该句柄，注意和
     *         NET_ConnStop配套使用
     *
     * @see    NET_ConnStop
     */
    NET_API HANDLE NET_ConnStart(const NET_CONN_PARAM_S *pParam, U32 u32Mode = 0);

    /**
     * @brief  用户退出登录
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     *
     * @see    NET_ConnStart
     */
    NET_API VOID NET_ConnStop(HANDLE *hConn);

    /**
     * @brief  添加备用流媒体服务器
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] ip 流媒体ip
	 * @param [in] port 流媒体port
	 *
     * @see    成功返回TRUE,失败返回FALSE
     */
	NET_API BOOL NET_AddStandBy(HANDLE hConn, const char *ip, int port);

    /**
     * @brief  删除备用流媒体服务器
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] ip 流媒体ip
	 * @param [in] port 流媒体port
	 *
     * @see    成功返回TRUE,失败返回FALSE
     */
	NET_API BOOL NET_DelStandBy(HANDLE hConn, const char *ip, int port);

    /**
     * @brief  获取登录返回的设备相关信息
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [out] pRsp  设备相关信息结构体，参考LOGIN_RSP_S
     *
     * @return 成功返回TRUE, 否则FALSE
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_GetLoginRsp(HANDLE hConn, LOGIN_RSP_S *pRsp);

    /**
     * @brief  获取用户列表信息
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] pParam 获取用户信息列表参数
     *
     * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_GetUserList(HANDLE hConn, const NET_USER_PARAM_S *pParam);

    /**
     * @brief  获取某个系统（设备）所包含设备列表信息
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] pParam 需要获取设备信息列表的系统（设备）的参数配置
     *
     * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_GetDeviceList(HANDLE hConn, const NET_DEVICE_PARAM_S *pParam);

    /** 预览 */

    /**
     * @brief  启动某台设备某个通道的视频预览
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] pparam请求预览相关参数配置，参考NET_PVIEW_PARAM_S
     *
     * @return 成功返回一个句柄，标示一个已打开的通道，失败返回NULL。
     *
     * @note   成功调用后必须调用NET_StopPreview
     *
     * @see    NET_StopPreview, NET_ConnStart
     */
    NET_API HANDLE NET_StartPreview(HANDLE hConn, const NET_PVIEW_PARAM_S *pParam);

    /**
     * @brief  停止某台设备某个通道的视频预览
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] hPView NET_StartPreview返回值
     *
     * @see    NET_StartPreview, NET_ConnStart
     */
    NET_API VOID NET_StopPreview(HANDLE  hConn, HANDLE *hPView);

    /**
     * @brief  请求实时流中马上产生一个I帧
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] pparam 请求关键帧参数配置，参考NET_REQI_PARAM_S
     *
     * @return 成功返回TRUE, 否则FALSE
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_RequestIFrame(HANDLE hConn, const NET_REQI_PARAM_S *pParam);

    /**
     * @brief  预览画面切换
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] pparam 请求画面切换相关参数配置，参考NET_SWITCH_PARAM_S
     *
     * @return 成功返回一个句柄，标示一个已打开的通道，失败返回NULL。
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_PreviewSwitch(HANDLE hConn, const NET_SWITCH_PARAM_S* pParam);

    /**
     * @brief  获取登录返回的设备相关信息
     *
     * @param [in] hPView NET_StartPreview返回值
     * @param [out] pRsp  获取的当前通道信息结构体，参考PVIEW_RSP_S
     *
     * @return 成功返回TRUE, 否则FALSE
     *
     * @see    NET_StartPreview
     */
    NET_API BOOL NET_GetPreviewRsp(HANDLE hPView, PVIEW_RSP_S *pRsp);

    /**
     * @brief  捕获数据并存放到指定的文件中。
     *
     * @param [in]  hPView      NET_StartPreview返回值
     * @param [out] ps8FileName 文件路径指针
     *                          流媒体：  后缀名为.dav
     *                          海康平台：后缀名为.mp4
     *
     * @return 成功返回TRUE, 否则FALSE, 获取错误码调用NET_GetLastError
     *
     * @note   成功调用后必须调用NET_NET_StopSaveRealData
     *
     * @see    NET_StartPreview, NET_StopSaveRealData
     */
    NET_API BOOL NET_SaveRealData(HANDLE hPView, S8 *ps8FileName);

    /**
     * @brief  停止数据捕获。
     *
     * @param [in]  hPView  NET_StartPreview返回值
     *
     * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     *
     * @see    NET_StartPreview
     */
    NET_API BOOL NET_StopSaveRealData(HANDLE hPView);

    /**
     * @brief  预览时，单帧数据捕获并保存成图片。
     *
     * @param [in] hPView         NET_StartPreview返回值
     * @param [in] ps8PicFileName 图片文件路径指针
     * @param [in] u32CaptueMode  抓图图片格式, 参与CAP_MODE_E
     *                             0 - CAP_BMP_MODE ,32位位图格式
     *                             1 - CAP_JPEG_MODE,jpeg图片格式
     *
     * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     *
     * @see    NET_StartPreview
     */
    NET_API BOOL NET_CapturePicture(HANDLE hPView, S8 *ps8PicFileName, U32 u32CaptueMode = CAP_BMP_MODE);

    /** 云台 */
    /**
     * @brief  云台控制操作
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] u32Cmd 云台控制命令,参考PTZ_CMD_E
     * @param [in] pparam云台控制相关参数配置，参考NET_PTZ_PARAM_S
     *
     * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_PTZControl(HANDLE hConn, U32 u32Cmd, const NET_PTZ_PARAM_S *pParam);

    /** 云台(未开启视频通道预览下)*/
    /**
     * @brief  云台控制操作
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] u32Cmd 云台控制命令,参考PTZ_CMD_E
     * @param [in] pparam云台控制相关参数配置，参考NET_PTZ_PARAM_S
     *
     * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_PTZControlEx(HANDLE hConn, U32 u32Cmd, const NET_PTZ_PARAM_S *pParam);

    /** 录像查询 */

    /**
     * @brief  录像文件查找
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] pparam录像查找相关参数配置，参考NET_QRECORD_PARAM_S
     *
     * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_QueryRecord(HANDLE hConn, const NET_QRECORD_PARAM_S *pParam);

    /**
     * @brief  录像文件查找(送检增加告警类型查找)
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] pparam录像查找相关参数配置，参考NET_QRECORD_PARAM_S
     *
     * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_QueryRecordEx(HANDLE hConn, const NET_QRECORD_PARAM_EX_S *pParam);

    /** 录像回放 */
    /**
     * @brief  开始录像回放
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] pparam录像回放相关参数配置，参考NET_QRECORD_PARAM_S
     *
     * @return 成功返回一个句柄,标示一下已经打开的回放通道， 否则返回NULL, 获取错误码调用NET_GetLastError
     *
     * @see    NET_ConnStart, NET_StopRecordPlay
     */
    NET_API HANDLE NET_StartRecordPlay(HANDLE hConn,  const NET_RECPLAY_PARAM_S *pParam);

    /**
     * @brief  停止录像回放
     *
     * @param [in] hPlayback 已经建立连接的回放句柄, NET_StartRecordPlay返回值
     *
     * @see    NET_StartRecordPlay
     */
    NET_API VOID NET_StopRecordPlay(HANDLE *hPlayback);

    /**
     * @brief  控制录像回放的状态。
     *
     * @param [in]     hPlayback 已经建立连接的句柄, NET_StartRecordPlay返回值
     * @param [in]     u32Code    控制录像回放状态命令
     * @param [in]     pInVal     指向输入参数的指针
     * @param [in]     u32InSize  输入参数的长度
     * @param [out]    pOutVal    指向输出参数的指针
     * @param [in,out] u32OutSize 输出参数的长度
     *
     * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     *
     * @see    NET_StartRecordPlay
     */
    NET_API BOOL NET_RecordPlayCtrl(HANDLE hPlayback, U32 u32Code, VOID *pInVal = NULL, U32 u32InSize = 0, VOID *pOutVal = NULL, U32 *u32OutSize = NULL);

    /** 录像下载 */

    /**
     * @brief  开始按文件名下载录像文件
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] pparam录像下载相关参数配置，参考NET_RECDOWNLOAD_PARAM_S
     *
     * @return 成功返回一个句柄,标示一下已经打开的下载通道， 否则返回NULL, 获取错误码调用NET_GetLastError
     *
     * @see    NET_ConnStart, NET_StopRecordDownload
     */
    NET_API HANDLE NET_StartRecordDownload(HANDLE hConn,  const NET_RECDOWNLOAD_PARAM_S *pParam);

    /**
     * @brief  停止录像下载
     *
     * @param [in] hDownload 已经建立连接的下载句柄, NET_StartRecordDownload返回值
     *
     * @see    NET_StartRecordDownload
     */
    NET_API VOID NET_StopRecordDownload(HANDLE *hDownload);

    /**
     * @brief  获取录像下载进度
     *
     * @param [in] hDownload 已经建立连接的下载句柄, NET_StartRecordDownload返回值
     *
     * @return -1表示失败；0～100表示下载的进度；100表示下载结束；正常范围0-100，如返回200表明出现网络异常。
     * 		   获取错误码调用NET_DVR_GetLastError
     *
     * @see    NET_StartRecordDownload
     */
    NET_API S32 NET_GetDownloadPos(HANDLE hDownload);


    /**
     * @brief  开始按时间段下载录像文件
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] pParam录像下载相关参数配置，参考NET_RECDOWNLOAD_BYTIME_PARAM_S
     *
     * @return 成功返回一个句柄,标示一下已经打开的下载通道， 否则返回NULL, 获取错误码调用NET_GetLastError
     *
     * @see    NET_ConnStart, NET_StopRecordDownload
     */
    NET_API HANDLE NET_StartRecordDownloadByTime(HANDLE hConn,  const NET_RECDOWNLOAD_BYTIME_PARAM_S *pParam);

    /**
     * @brief  停止按时间段录像下载
     *
     * @param [in] hDownload 已经建立连接的下载句柄, NET_StartRecordDownloadByTime返回值
     *
     * @see    NET_StartRecordDownloadByTime
     */
    NET_API VOID NET_StopRecordDownloadByTime(HANDLE *hDownload);

    /**
     * @brief  矩阵切换
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] pparam 请求矩阵切换相关参数配置，参考NET_MATRIX_PARAM_S
     *
     * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_MatrixSwitch(HANDLE hConn, const NET_MATRIX_PARAM_S * pParam);

    /**
     * @brief  解码器显示切换
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] pparam 请求矩阵切换相关参数配置，参考NET_DECODER_PARAM_S
     *
     * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_DecoderSwitch(HANDLE hConn, const NET_DECODER_PARAM_S * pParam);

    /**
     * @brief  获取解码器显示通道
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] pparam 请求矩阵切换相关参数配置，参考NET_DEC_DIS_PARAM_S
     *
     * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     *
     * @note   注意与NET_DecoderReleaseDisplay配套使用
     *
     * @see    NET_ConnStart, NET_DecoderReleaseDisplay
     */
    NET_API HANDLE NET_DecoderGetDisplay(HANDLE hConn, const NET_DEC_DIS_PARAM_S * pParam);

    /**
     * @brief  释放解码器显示通道配置
     *
     * @param [in] hDisplay 解码器显示通道句柄, NET_DecoderGetDisplay返回值
     *
     * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     *
     * @note   注意与NET_DecoderReleaseDisplay配套使用
     *
     * @see    NET_DecoderReleaseDisplay
     */
    NET_API VOID NET_DecoderReleaseDisplay(HANDLE *hDisplay);

    /**
     * @brief  解码器开始动态解码
     *
     * @param [in] hDisplay  显示通道句柄,NET_DecoderGetDisplay返回值
     * @param [in] pparam 请求矩阵切换相关参数配置，参考NET_DEC_CHN_PARAM_S
     * @param [in] s32DecChnNo 解码通道号, 从0开始
     * @param [in] s32DisWndNo 显示通道所在窗口号, 从0开始
     *
     * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_DecoderStartDynamic(HANDLE hDisplay, S32 s32DecChnNo, S32 s32DisWndNo, const NET_DEC_CHN_PARAM_S *pParam);

    /**
     * @brief  解码器释放某一显示通道占有的解码通道
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] pparam 请求解码相关参数配置，参考NET_DEC_CHN_PARAM_S
     * @param [in] pParam 显示通道相关参数
     *
     * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     *
     * @see    NET_DecoderStopDynamic
     */
    NET_API BOOL NET_DecoderStopDynamic(HANDLE hConn, const NET_DEC_DIS_PARAM_S * pParam);


    /**
     * @brief  设置五防联动回调
     *
     * @param [in] hDisplay 解码器显示通道句柄, NET_DecoderGetDisplay返回值
     *
     * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     *
     * @see    NET_DecoderReleaseDisplay
     */
    NET_API BOOL NET_SetWFLinkageCallback(HANDLE hConn, pfnWFLinkageCallback fWFLinkageCallback, const VOID *pUser);

    /**
     * @brief  在线客户列表回调函数定义
     *
     * @param [out] pRsp   在线客户详细信息
     * @param [out] pUser  用户数据
     *
     */
    typedef VOID (*pfnGetClientCallback)(const CLIENT_INFO_RSP_S *pRsp, VOID *pUser);
    /**
     * @brief  获取在线客户信息
     *
     * @param  [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     *
     * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     *
     * @see
     */
    NET_API BOOL NET_GetClientList(HANDLE hConn, pfnGetClientCallback fClientCallback, VOID *pUser);

    /**
     * @brief  PC对讲消息回调函数
     *
     * @param [out] pRsp   在线客户详细信息
     * @param [out] pUser  用户数据
     *
     */
    typedef VOID (*pfnPcTalkCallback)(U8 u8Type, U32 u32SockFd, VOID *pData, U32 u32Size, VOID *pUser);
    /*PC对讲参数机构*/
    typedef struct
    {
        S8    * s8LocalName;                           /**< 本地名称 */
        S8    * s8LocalIp;                             /**< 本地IP */
        U16     u16LocalPort;                          /**< 本地端口 */
        S8    * s8RemoteIp;                            /**< 对方IP */
        U16     u16RemotePort;                         /**< 对方端口 */
        VOID  * pUser;                                 /**< 用户数据 */
        pfnPcTalkCallback  fPcTalkCallback;            /**< PC对讲消息回调 */
    } NET_PCTALK_PARAM_S;
    /**
     * @brief  初始化PC对讲服务
     *
     * @param [in] pParam  对讲服务参数
     *
     * @return 成功返回连接句柄(指针), 否则返回NULL
     */
    NET_API HANDLE NET_InitPcTalkServer(NET_PCTALK_PARAM_S * pParam);
    /**
     * @brief  初始化PC对讲客户端
     *
     * @param [in] pParam  对讲服务参数
     *
     * @return 成功返回连接句柄(指针), 否则返回NULL
     */
    NET_API HANDLE NET_InitPcTalkClient(NET_PCTALK_PARAM_S * pParam);

    /**
     * @brief  释放对讲类对象
     *
     * @param [in] hPcTalk  之前初始化的对讲类对象
     *
     * @return void
     */
    NET_API VOID NET_ReleasePcTalk(HANDLE hPcTalk);

    /* 对讲角色类型定义 */
#define PCTALK_SERVER     1
#define PCTALK_CLIENT     2

    /* PC对讲消息类型定义 */
    enum
    {
        PCTALK_REQUEST = 1,         /**< PC对讲请求 */
        PCTALK_ACK,                 /**< 接收对讲请求 */
        PCTALK_NAK,                 /**< 拒绝对讲请求 */
        PCTALK_DATA,                /**< 音频数据 */
        PCTALK_STOP,                /**< 结束对讲 */
        PCTALK_OFFLINE              /**< 对讲断线 */
    };
    /* PC对讲应用类型定义 */
    enum
    {
        PCTALK_DIALOG = 1,       /**< 1对1对讲 */
        PCTALK_BROADCAST,      /**< 1对多喊话 */
    };
    /* PC对讲信息头 */
    typedef struct
    {
        U32     u32SockFd;                             /**< socket fd */
        U8      u8SubOrBackType;                       /**< 子类型或回复类型 */
        S8      s8LocalName[MAX_NAME_LEN];             /**< 本地名称 */
        U8      u8LocalIp[MAX_IPV4_LEN];               /**< 本地IP */
        U8      u8RemoteIp[MAX_IPV4_LEN];              /**< 对方IP */
        U32     u32DataLen;                            /**< 后面数据长度 */
        U32     u32Reserve1;                           /**< 保留字段 */
    } NET_PCTALK_MASHEAD;
    /**
     * @brief  发送对讲消息
     *
     * @param [in] hConn    已经建立的连接句柄，NET_InitPcTalkServer或NET_InitPcTalkClient的返回值
     * @param [in] u8Type	消息类型：1-申请对讲，2-ACK, 3-NAK, 4-音频数据，5-结束对讲
     * @param [in] pData	数据
     * @param [in] u32Size  数据长度
     *
     * @return VOID
     */
    NET_API VOID NET_SendPcTalkMsg(HANDLE hPcTalk, U8 u8Type, VOID *pData, U32 u32Size);

    /**
     * @brief  向下同步时间
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] pParam 同步时间参数，参考NET_SYNTIME_PARAM_S
     *
     * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_SynchrTime(HANDLE hConn, const NET_SYNTIME_PARAM_S *pParam);

    /**
     * @brief  查询时间
     *
     * @param [in] hConn      已经建立连接的句柄,NET_ConnStart返回值
     * @param [out] pDateTime 时间
     *
     * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_QueryTime(HANDLE hConn, DATETIME_S *pDateTime);

    /**
     * @brief  打开声音预览
     *
     * @param [in] hPView NET_StartPreview返回值
     *
     * @see    NET_StartPreview, NET_ConnStart
     */
    NET_API BOOL NET_OpenSound(HANDLE hPView);

    /**
     * @brief  关闭声音预览
     *
     * @param [in] hPView NET_StartPreview返回值
     *
     * @see    NET_StartPreview, NET_ConnStart
     */
    NET_API BOOL NET_CloseSound(HANDLE hPView);


    /**
     * @brief  获取设备在线状态
     *
     * @param [in] hConn     已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] u8Ip      设备IP地址
     * @param [in] u16Port   设备端口
     *
     * @see
     */
    NET_API BOOL NET_GetDeviceState(HANDLE hConn, U8 u8Ip[], U16 u16Port);

    /**
     * @brief  在线客户列表回调函数定义
     *
     * @param [out] pRsp   资源信息
     * @param [out] pUser  用户数据
     *
     */
    typedef VOID (*pfnResourceCallback)(const RESOURCE_RSP_S *pRsp, VOID *pUser);

    /**
     * @brief  资源获取
     *
     * @param  [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param  [in] pUser 用户数据
     *
     * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     *
     * @see
     */
    NET_API BOOL NET_QueryResource(HANDLE hConn, pfnResourceCallback fResourceCallback, VOID *pUser);


    /* 对讲、喊话、音视频会话 */
    /**
     * @brief 对讲初始化
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] pParam 相关参数配置，参考NET_DTALK_PARAM_S
     *
     * @return 成功返回一个句柄，标示一个已建立的对讲通道相关资源，失败返回NULL。
     *
     * @see    NET_UnInitDualtalk, NET_ConnStart
     */
    NET_API HANDLE NET_InitDualtalk(HANDLE hConn, const NET_DTALK_PARAM_S *pParam);

    /**
     * @brief  对讲反初始化。
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] hDTalk  NET_InitDualtalk返回值
     *
     * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     *
     * @see NET_InitDualtalk, NET_ConnStart
     */
    NET_API VOID NET_UnInitDualtalk(HANDLE hConn, HANDLE *hDTalk);

    /**
     * @brief  发送对讲消息
     *
     * @param [in] hDTalk   已经建立的连接句柄，NET_InitDualtalk的返回值
     * @param [in] pParam   对讲消息参数
     *
     * @return 成功返回TRUE, 否则返回FALSE
     */
    NET_API BOOL NET_SendDTalkMsg(HANDLE hDTalk, const NET_DTALK_MSG_S *pParam, const NET_DTALK_USER_S *pUserList);

    /**
     * @brief  发送对讲音视频数据
     *
     * @param [in] hDTalk   已经建立的连接句柄，NET_InitDualtalk的返回值
     * @param [in] pData    g711a数据，包含私有帧头
     * @param [in] u32Size  数据长度,320+40
     *
     * @return 成功返回TRUE, 否则返回FALSE
     */
    NET_API BOOL NET_SendDTalkData(HANDLE hDTalk, U8 *pu8Data, U32 u32Size);

    /**
     * @brief  内部会给hMsgWnd发送WM_VIDEO_CAPTURE消息，应用接收到该消息可以调用该接口启动视频采集
     *
     * @param [in] hDTalk   已经建立的连接句柄，NET_InitDualtalk的返回值
     *
     * @return 成功返回TRUE, 否则返回FALSE
     */
    NET_API BOOL NET_StartCapture(HANDLE hDTalk);

    /**
     * @brief  联动转发
     *
     * @param [in] hConn        已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] pLinkTrans   联动信息
     *
     * @see
     */
    NET_API BOOL NET_LinkTransmit(HANDLE hConn, LINKAGE_TRANS_RPT_S* pLinkTrans);

    /**
     * @brief  获取设备配置信息
     *
     * @param [in] pData        设备信息指针
     * @param [in] u32DevNum    设备个数
     *
     * @see
     */
    NET_API CHN_INFO_S* NET_GetDeviceData(HANDLE hConn, U32 *u32DevNum);

    /**
     * @brief  轨道车控制操作
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] u32Cmd 云台控制命令,参考PTZ_CMD_E
     * @param [in] pparam云台控制相关参数配置，参考NET_PTZ_PARAM_S
     *
     * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_RailCarControl(HANDLE hConn, U32 u32Cmd, const NET_PTZ_PARAM_S *pParam);

    /**
     * @brief 远程启动或停止DVR的某个通道的录像
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] pparam 摄像机通道信息，参考NET_CHANNEL_PARAM_S
     * @param [in] bStart 1-启动录像，0-停止录像
     *
     * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     *
     * @see    NET_ControlRecord
     */
    NET_API BOOL NET_ControlRecord(HANDLE hConn, const NET_CHANNEL_PARAM_S *pParam, BOOL bStart);

    /**
     * @brief 参数设置(二进制模式)
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] pParam 设置参数内容，参考NET_PARAMSET_PARAM_S
     *
     * @return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     *
     * @see    NET_ControlRecord
     */
    NET_API BOOL NET_SetParameter(HANDLE hConn, const NET_PARAMSET_PARAM_S *pParam);


    /**
     * @brief 参数设置(json模式)
     *
     * param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * param [in] pParam 设置参数内容
     *
     * return 成功返回TRUE, 否则返回FALSE, 获取错误码调用NET_GetLastError
     */

    NET_API BOOL NET_SetParameterEx(HANDLE hConn, const S8 * pParam);

    /* 音视频对讲状态 */
    enum
    {
        TALK_STATE_NOTHING = 0,   /**< 空闲 */
        TALK_STATE_WAIT,      /**< 发起或被申请中，等待接受或被接受 */
        TALK_STATE_DIALOG,    /**< 正在对讲中 */
        TALK_STATE_BROADCAST, /**< 正在喊话中 */
    };
    /**
     * @brief 设置客户的音视频对讲状态
     *
     * @param [in] hConn         已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] u32DTalkState 音视频对讲状态
     *
     * @return BOOL
     *
     * @see
     */
    NET_API BOOL NET_SetDTalkState(HANDLE hConn, U32 u32DTalkState);

    /**
     * @brief  预置位列表信息回调函数定义
     *
     * @param [out] u32PresetNo   预置位号
     * @param [out] ps8PresetName 预置位名称
     */
    typedef VOID (*pfnPresetListCallback)(U32 u32PresetNo, const S8 *ps8PresetName, VOID *pUser);
    typedef struct
    {
        S8                    * ps8FrontAddrCode;                    /**< 前端设备地址编码, 非空则采用地址码方式查找前端设备，后面配置的IP和端口不起作用 */
        S8                    * ps8DeviceIp;                         /**< 设备IP */
        U16                     u16DevicePort;                       /**< 设备端口 */
        S8                    * ps8SubServIp;                        /**< 二级流媒体服务器IP */
        U16                     u16SubServPort;                      /**< 二级流媒体服务器端口 */

        U32                     u32ChnNo;                            /**< 通道号 */

        VOID                  * pUser;                               /**< 用户数据 */
        pfnPresetListCallback fnPresetListCallback;                  /**< 设备列表数据回调 */
    } NET_GETPRESET_PARAM_S;

    /**
     * @brief  获取设备通道预置位信息
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] pParam 获取设备通道信息，参考NET_GETPRESET_PARAM_S
     *
     * @see
     */
    NET_API BOOL NET_GetChnPreset(HANDLE hConn, const NET_GETPRESET_PARAM_S *pParam);

    typedef struct
    {
        U32                     u32PresetNo;                         /**< 预置位号 */
        S8                    * ps8PresetName;                       /**< 预置名称 */
    } NET_PRESET_INFO_S;

    typedef struct
    {
        S8                    * ps8FrontAddrCode;                    /**< 前端设备地址编码, 非空则采用地址码方式查找前端设备，后面配置的IP和端口不起作用 */
        S8                    * ps8DeviceIp;                         /**< 设备IP */
        U16                     u16DevicePort;                       /**< 设备端口 */
        S8                    * ps8SubServIp;                        /**< 二级流媒体服务器IP */
        U16                     u16SubServPort;                      /**< 二级流媒体服务器端口 */

        U32                     u32ChnNo;                            /**< 通道号 */
        U32                     u32PresetNum;                        /**< 预置位个数 */
        NET_PRESET_INFO_S     * pstPresetList;                       /**< 预置位列表 */
    } NET_SETPRESET_PARAM_S;

    /**
     * @brief  设置设备通道预置位信息
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] pParam 设置设备通道信息，参考NET_GETPRESET_PARAM_S
     *
     * @see
     */
    NET_API BOOL NET_SetChnPreset(HANDLE hConn, const NET_SETPRESET_PARAM_S *pParam);

    /**
     * @brief  预置位列表信息回调函数定义(新规约)
     *
     * @param [out] u32PresetNo   预置位号
     * @param [out] ps8PresetName 预置位名称
     * @param [out] u32PresetType  预置位类型
     */
    typedef VOID (*pfnPresetListCallbackEx)(U32 u32PresetNo, const S8 *ps8PresetName, U32 u32PresetType, VOID *pUser);
    typedef struct
    {
        S8                      * ps8FrontAddrCode;                    /**< 前端设备地址编码, 非空则采用地址码方式查找前端设备，后面配置的IP和端口不起作用 */
        S8                      * ps8DeviceIp;                         /**< 设备IP */
        U16                       u16DevicePort;                       /**< 设备端口 */
        S8                      * ps8SubServIp;                        /**< 二级流媒体服务器IP */
        U16                       u16SubServPort;                      /**< 二级流媒体服务器端口 */

        U32                       u32ChnNo;                            /**< 通道号 */

        VOID                    * pUser;                               /**< 用户数据 */
        pfnPresetListCallbackEx  fnPresetListCallback;                  /**< 设备列表数据回调 */
    } NET_GETPRESET_PARAM_S_EX;

    /**
     * @brief  获取设备通道预置位信息(新规约)
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] pParam 获取设备通道信息，参考NET_GETPRESET_PARAM_S
     *
     * @see
     */
    NET_API BOOL NET_GetChnPresetEx(HANDLE hConn, const NET_GETPRESET_PARAM_S_EX *pParam);

    typedef struct
    {
        U32                     u32PresetNo;                         /**< 预置位号 */
        S8                    * ps8PresetName;                       /**< 预置名称 */
        U32                     u32PresetType;                       /**< 预置位类型 */
    } NET_PRESET_INFO_S_EX;

    typedef struct
    {
        S8                    * ps8FrontAddrCode;                    /**< 前端设备地址编码, 非空则采用地址码方式查找前端设备，后面配置的IP和端口不起作用 */
        S8                    * ps8DeviceIp;                         /**< 设备IP */
        U16                     u16DevicePort;                       /**< 设备端口 */
        S8                    * ps8SubServIp;                        /**< 二级流媒体服务器IP */
        U16                     u16SubServPort;                      /**< 二级流媒体服务器端口 */

        U32                     u32ChnNo;                            /**< 通道号 */
        U32                     u32PresetNum;                        /**< 预置位个数 */
        NET_PRESET_INFO_S_EX  * pstPresetList;                       /**< 预置位列表 */
    } NET_SETPRESET_PARAM_S_EX;

    /**
     * @brief  设置设备通道预置位信息(新规约)
     *
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] pParam 设置设备通道信息，参考NET_GETPRESET_PARAM_S
     *
     * @see
     */
    NET_API BOOL NET_SetChnPresetEx(HANDLE hConn, const NET_SETPRESET_PARAM_S_EX *pParam);

#ifdef _WIN32
    /**
     * @brief  叠加字符到视频画面上
     * @param [in] hConn  已经建立连接的句柄,NET_ConnStart返回值
     * @param [in] pParam  参考NET_PRESET_DRAW
     */

    typedef struct
    {
        HANDLE *hPView;         /* 播放句柄*/
        S8     *ps8PresetName;  /* 叠加预置位字符 */
        U8      u8WndNum;       /* 当前窗口数目 */
        U16     u16Top;         /* 字符显示位置top */
        U16     u16Left;        /* 字符显示位置left */
    } NET_PRESET_DRAW;

    NET_API VOID NET_DrawPreset(HANDLE hConn, const NET_PRESET_DRAW * pParam);

    /**
     * @brief 输出sdk日志
     * @brief dir 输出目录
     */

    NET_API BOOL NET_SetLogToFile();

    /**
     * @brief 注册绘图函数
     * param [in] hConn 已经建立连接的句柄，NET_ConnStart返回值
     * param [in] NET_REGDRAW_PARAM 注册绘图参数
     * return 返回值，成功TRUE，失败FALSE
     */

    typedef VOID (*pfnDrawFun)(HDC hDC, S32 s32Height, S32 s32Width, VOID * pUser);

    typedef struct
    {
        HANDLE *hPView;         /* 播放句柄 */
        pfnDrawFun pfnDraw;     /* 绘图函数 */
        HANDLE     hUser;       /* 用户数据 */
    } NET_REGDRAW_PARAM;

    NET_API BOOL NET_RegisterDraw(HANDLE hConn, const NET_REGDRAW_PARAM * pParam);

    /**
     * @brief 解码数据回调函数
     * @param [in] hConn 已经建立连接的句柄，NET_ConnStart返回值
     *
     */

    typedef struct
    {
        U8   u8Type;      /* 颜色类型 */
        S32  s32Width;
        S32  s32Height;
        S8   *ps8Data;
        U32   u32Len;
    } DEC_FRAME_INFO;

    typedef VOID (*pfnDecDataCallback)(const DEC_FRAME_INFO *pDecFrameInfo, VOID * pUser);

    typedef struct
    {
        HANDLE *hPView;              /* 播放句柄 */
        pfnDecDataCallback pfnDecCb; /* 解码数据回调 */
        HANDLE   hUser;              /* 用户数据 */
    } NET_DECFRAME_PARAM;

    NET_API BOOL NET_SetDecDataCallback(HANDLE hConn, const NET_DECFRAME_PARAM * pParam);
#endif

    /**
     * @brief 获取sdk版本信息
     * @param DWORD 返回值 ret<0 失败
     * @param SDK版本信息，2个高字节表示主版本，2个低字节表示次版本。如0x01000007：表示版本为1.0.0.7。
     */

    NET_API DWORD NET_GetSDKVersion();

    /**
     * @brief 获取录像回放已播放时间
     * @param DWORD 返回值 录像已回放时间
     * @return param hPlayback NET_StartRecordPlay的返回值
     */

    NET_API DWORD NET_GetPlayedTime(HANDLE hPlayback);

    /**
     * @brief 设置录像回放状态回调函数
     * @param BOOL 成功返回TRUE,失败返回FALSE
     * @param param fnRecordPbStatusCb 设置回调函数
     * @param pUser 用户数据
     */
    NET_API BOOL NET_SetRecordPlaybackStatusCallback(HANDLE hPlayback, pfnRecordPlaybackStatusCallback fnRecordPbStatusCb, VOID *pUser);


    /** < 智能分析 */
    typedef struct _NET_IVA_ANALYSE_PARAM_
    {
        S8   * ps8IvaIp;        /* 智能分析服务器ip */
        U16    u16IvaPort;      /* 智能分析服务器port */
        S8   * ps8DevIp;        /* 设备ip */
        U16    u16DevPort;      /* 设备port */
        U8     u8DevChn;        /* 设备通道*/
        U8     u8DevPreset;     /* 设备预置位*/
    } NET_IVA_ANALYSE_PARAM;

    /**
     * @brief 开启智能分析(测温时ps8IvaIp为"0.0.0.0")
     * @param hConn 已经建立连接的句柄,NET_ConnStart返回值
     * @param pParam 智能分析参数
     * @return 返回值，成功TRUE，失败FALSE
     */

    NET_API BOOL NET_StartIvaAnalyse(HANDLE hConn, const NET_IVA_ANALYSE_PARAM *pParam);

    /**
     * @brief 停止智能分析(测温时ps8IvaIp为"0.0.0.0")
     * @param hConn 已经建立连接的句柄,NET_ConnStart返回值
     * @param pParam 智能分析参数
     * @return 返回值，成功TRUE，失败FALSE
     */

    NET_API BOOL NET_StopIvaAnalyse(HANDLE hConn, const NET_IVA_ANALYSE_PARAM *pParam);

    #ifndef _MUX_CALL_TYPE_
    #define _MUX_CALL_TYPE_
    enum MUX_CALL_TYPE{
        MUX_CALL_START,     //开始录像
        MUX_CALL_END,       //结束录像
    };
    #endif //__MUX_AVFILE_H__

    /**
    * @brief 设备对讲，封装音视频文件回调
    * @param strFilename：文件名（带路径）
    * @param type：回调类型
    * @param pUser：用户数据
    */
    typedef void(*muxavfDevTalk_callback)(char *strFilename, MUX_CALL_TYPE type, void* pUser);

    /** < 设备对讲，封装音视频文件 */
    typedef struct 
    {
        S8   * strFilePath;          /* 文件路径 例："E:/"，trFilepath不得为NULL或长度超过1024，否则默认为“./” */
        U32    nSubpackageTime;      /* 分包时长：min，不得小于5min，否则按5min计算；分包后命名规则：“test-1.avi”、...“test-n.avi”，n表示除“test.avi”外的第n个分包 */
        VOID   * pUser;              /* 用户数据 */
        muxavfDevTalk_callback    callback;      /* 文件创建、关闭回调 */
    } _NET_DEV_MUXAVF_PARAM_;
    /**
     * @brief 初始化设备对讲
     *
     * @param hConn      已经建立连接的句柄,NET_ConnStart返回值
     * @param audioType  语音格式类型 0-pcm 1-g711A 2-g711U 3-hkg722
     *
     * @return HANDLE 返回值，成功设备对讲句柄，失败NULL
     */

    NET_API HANDLE NET_InitDevTalk(HANDLE hConn, int audioType = 2);

    /**
     * @brief 释放设备对讲
     * @param hDevTalk 设备对讲句柄指针
     */

    NET_API VOID NET_UninitDevTalk(HANDLE *hDevTalk);

    /**
     * @brief 开启设备对讲
     * @param hDevTalk 设备对讲句柄
     * @param pParam 设备参数列表
     * @return 返回值，成功TRUE，失败FALSE
     */

    NET_API BOOL NET_StartDevTalk(HANDLE hDevTalk, const NET_DTALK_DEV_LIST *pParam);

    /**
     * @brief 停止设备对讲
     * @param hDevTalk 设备对讲句柄
     * @param pParam 设备参数列表
     * @return 无
     */

    NET_API VOID NET_StopDevTalk(HANDLE hDevTalk, const NET_DTALK_DEV_LIST *pParam);


	/**
     * @brief 发送音频数据
     * @param hDevTalk 设备对讲句柄
     * @param pParam ps8Data
     * @return 无
     */
	NET_API VOID NET_SendDataToDevTalk(HANDLE *hDevTalk, U8 * ps8Data, U16 u16Len);
    
	/**
     * @brief 初始化设备对讲封装文件参数，并创建封装文件
     * @param hDevTalk 设备对讲句柄，NET_InitDevTalk返回句柄
     * @param pParam 封装文件参数
     * @return 成功：TRUE，失败：FALSE
     *
     * @author zhangshenyu
     * @date 2020415
     */
	NET_API BOOL NET_InitMuxAVFDevTalk(HANDLE hDevTalk, _NET_DEV_MUXAVF_PARAM_ *pParam);

    typedef struct
    {
        float fxRatio;
        float fyRatio;
    } NET_AI_THERM_POS;

    typedef struct
    {
        S8    * ps8DevIp;       /* 设备ip */
        U16     u16DevPort;     /* 设备port */
        U8      u8ChnNo;        /* 通道号 */
        U8      u8PresetNo;     /* 预置位号 */
        U8      u8Enable;       /* 是否启用*/
        S8    * ps8RuleName;    /* 规则名称 */
        U8      u8RuleId;       /* 规则id */
        float   fEmissivity;    /* 发射率 */
        float   fDistance;      /* 距离 */
        float   fReflexValue;   /* 反射温度*/
        NET_AI_THERM_POS stFrom;
        NET_AI_THERM_POS stTo;
    } NET_THERM_RULE_INFO;

    /* 红外预置位配置参数 */
    typedef struct
    {
        U32                   u32RuleListNum;
        NET_THERM_RULE_INFO * pThermRuleList;
    } NET_THERM_RULE_LIST;

    /**
     * @brief 配置红外测温预置位下规则参数
     * @param hConn 已经建立连接的句柄,NET_ConnStart返回值
     * @param pParam 规则参数列表
     * @return 返回值，成功TRUE，失败FALSE
     */

    NET_API BOOL NET_SetThermometryRuleCfg(HANDLE hConn, const NET_THERM_RULE_LIST* pParam);

    /* 红外显示模式 */
    enum ThermDispalyMode
    {
        Therm_Show_WhiteHot = 0,    /* 白热 */
        Therm_Show_RainBow  = 10,   /* 彩虹 */
        Therm_Show_IronRed  = 12,   /* 铁红 */
        Therm_Show_GreenHot = 20,   /* 绿热 */
    };

    /* 切换红外热诚像模式 */
    typedef struct
    {
        S8             * ps8DevIp;
        U16              u16DevPort;
        U8               u8ChnNo;
        ThermDispalyMode mode;
    } NET_TERM_SHOWPARAM_S;

    /**
     * @brief 配置红外测温下显示模式(铁红、彩虹等)
     * @param hConn 已经建立连接的句柄,NET_ConnStart返回值
     * @param showMode 显示模式
     * @return 返回值，成功TRUE，失败FALSE
     */

    NET_API BOOL NET_SetThermDisplayMode(HANDLE hConn, const NET_TERM_SHOWPARAM_S * pParam);

    /**
     * @brief 设置或删除流媒体下预置位信息
     * @param hConn 已经建立连接的句柄,NET_ConnStart返回值
     * @param ps8PresetInfo 设置预置位信息json格式
     * @param us32Size 发送字节大小
     * @return 返回值，成功TRUE，失败FALSE
     */
    NET_API BOOL NET_SendPresetInfo(HANDLE hConn, const S8 * ps8PresetInfo, U32 us32Size);

    /**
     * @brief 获取流媒体下预置位信息
     * @param hConn 已经建立连接的句柄,NET_ConnStart返回值
     * @param ps8PresetReq 请求信息(全部或者单个)
     * @param u32Size 请求信息字节数
     * @param ps8PresetInfo 返回预置位信息json格式
     * @return 返回值，成功TRUE，失败FALSE
     */
    NET_API BOOL NET_GetPresetInfo(HANDLE hConn, const S8 * ps8PresetReq, U32 u32Size, S8 ** ps8PresetInfo);

    /**
     * @brief 下发和盛资(AXF|EST|DV|LINE)
     * @param hConn 已经建立连接的句柄,NET_ConnStart返回值
     * @param ps8Resource 一匙通资源信息json格式
     * @return 返回值，成功TRUE，失败FALSE
     */
    NET_API BOOL NET_SendHSResource(HANDLE hConn, const S8 * ps8Resource, S32 s32Size);

    /**
     * @brief 下发命令到流媒体(json格式)
     * @param hConn 已经建立连接的句柄,NET_ConnStart返回值
     * @param ps8Info 数据内容
     * @param s32Size 数据大小
     * @return 返回值，成功TRUE，失败FALSE
     */
    NET_API BOOL NET_SendCommand(HANDLE hConn, const S8 * ps8Info, S32 s32Size);
	
	/**
	* @brief 获取智能分析总召告警
	* @param hConn 已经建立连接的句柄,NET_ConnStart返回值
	* @param ps8Req 请求
	* @@param u32Size 请求大小
	* @ps8AlarmInfo 返回值
	* @return 返回值，成功TRUE，失败FALSE
	*/
	NET_API BOOL NET_GetRemoteIvaAlarmData(HANDLE hConn, const S8 * ps8Req, U32 u32Size, S8 ** ps8AlarmInfo);
	
	
	/**
	* @brief 获取智能分析轮巡列表
	* @param hConn 已经建立连接的句柄,NET_ConnStart返回值
	* @param ps8Req 请求
	* @@param u32Size 请求大小
	* @@param ps8IvaCruiseList 返回值
	* @return 返回值，成功TRUE，失败FALSE
	*/
	NET_API BOOL NET_GetIvaCruiseList(HANDLE hConn, const S8 * ps8Req, U32 u32Size, S8 ** ps8IvaCruiseList);

	/**
	* @brief 控制智能分析轮巡启停
	* @param hConn 已经建立连接的句柄,NET_ConnStart返回值
	* @param ps8Req 请求
	* @u32Size 请求大小
	* @ps8AlarmInfo 返回值
	* @return 返回值，成功TRUE，失败FALSE
	*/
	NET_API BOOL NET_ControlIvaCruise(HANDLE hConn, const S8 * ps8Req, U32 u32Size);

	/**
	* @brief 查询智能分析分析列表
	* @param hConn 已经建立连接的句柄,NET_ConnStart返回值
	* @param ps8Req 请求
	* @u32Size 请求大小
	* ps8Result 返回值
	* @return 返回值，成功TRUE，失败FALSE
	*/
	NET_API BOOL NET_QueryIvaRunList(HANDLE hConn, const S8 * ps8Req, U32 u32Size, S8 ** ps8Result);


	/**
	* @brief 查询硬盘录像机通道的录像状态
	* @param hConn 已经建立连接的句柄,NET_ConnStart返回值
	* @param ps8Req 请求
	* @u32Size 请求大小
	* ps8ChanRecordState 返回值
	* @return 返回值，成功TRUE，失败FALSE
	*/
	NET_API BOOL NET_QueryChanRecordState(HANDLE hConn, const S8 * ps8Req, U32 u32Size, S8 ** ps8ChanRecordState);
	
	/**
	* @brief 发送命令并获取返回结果
	* @param hConn 已经建立连接的句柄,NET_ConnStart返回值
	* @param ps8Req 请求,eg:{"ChannelNo":33,"DevIp":"192.168.80.235","DevPort":8000,"Method":"GetChannelStatus","version":"v1.00"}
	* @u32Size 请求大小
	* ps8Rsp 接收缓存
	* u8RspSize 传入接收缓存大小，返回实际数据大小
	* @return 返回值，成功TRUE，失败FALSE
	*/
	NET_API BOOL NET_GetResponse(HANDLE hConn, const S8 * ps8Req, U32 u32Size, S8 *ps8Rsp,U32 *u8RspSize);

#ifdef __cplusplus
}
#endif
#endif
