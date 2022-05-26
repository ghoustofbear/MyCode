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
    U32                      u32MngRightBit;                  /**< ����Ȩ��λ, 0-�������� 1-վ����� 2-�豸���� 3-Ȩ�������
		                     								                  4-�û����� 5-�豸״̬ 6-�û�״̬ 7-ϵͳ״̬
		                     								  		      8-ϵͳ��־ 9-������־ 10-�豸��־ 11-�û���־ */
    U32                      u32OptRightBit;                  /**< ����Ȩ��λ, 0-Ԥ�� 1-�ط� 2-���� 3-��̨ 4-���� */
    U32                      u32PVChannel;                    /**< Ԥ��ͨ�� */
    U32                      u32PBChannel;                    /**< �ط�ͨ�� */
    U32                      u32PtzChannel;                   /**< ��̨����ͨ�� */
    U32                      u32AlarmChannel;                 /**< �����ϱ�ͨ�� */
    U8                       u8Res1[8];
    U8                       u8MngLevel;                      /**< ����Ȩ�޵ȼ� */
    U8                       u8OptLevel;                      /**< ����Ȩ�޵ȼ� */
    U8                       u8Res2[6];
} USER_RIGHT_S;

typedef struct _LOGIN_RSP_S_
{
    S8                       s8HWVerison[MAX_NAME_LEN];       /**< Ӳ���汾 */
    S8                       s8IEVerison[MAX_NAME_LEN];       /**< IE�汾 */
    S8                       s8SWVersion[MAX_NAME_LEN];       /**< ����汾 */
    S8                       s8SystemName[MAX_NAME_LEN];      /**< ϵͳ����, �����޸� */
    S8                       s8AddrCode[MAX_ADDR_LEN];        /**< ��ַ���� */
    S8                       s8MacAddr[MAX_NAME_LEN];         /**< �豸MAC��ַ */

    U32                      u32UserKey;                      /**< ���ӷ�����û���Կ */
    USER_RIGHT_S             stUserRight;
    U32                      u32DeviceNum;                    /**< ͨ���� */
    U32                      u32ChnInfoSize;
    U8                       u8Res[20];
} LOGIN_RSP_S;

typedef struct
{
    S8                       s8DevName[MAX_NAME_LEN];         /**< �豸���� */
    S8                       s8AddrCode[MAX_ADDR_LEN];        /**< �豸��ַ���� */
    U8                       u8IPAddr[4];

    U16                      u16Port;
    U16                      u16DevType;                      /**< �豸���� */
    U8                       u8ChannelNum;                    /**< ͨ���� */

    U8                       u8ManuType;                      /**< �豸�������� */
    U8                       u8AlarmInNum;                    /**< ���������� */
    U8                       u8AlarmOutNum;                   /**< ���������(�̵�������) */
    U8                       u8IsOnline;                      /**< �豸�Ƿ����� */
    U8                       u8Reserved[3];
} CHN_INFO_S;

/* �û��б�����Ϣ */
typedef struct
{
    U32                      u32StSize;                       /**< sizeof(USERINFO_RSP_S) */
    S8                       s8Username[MAX_NAME_LEN];        /**< �û��� */
    S8                       s8Password[MAX_NAME_LEN];        /**< ���� */
    USER_RIGHT_S             stUserRight;                     /**< �û�Ȩ�� */
} USERINFO_RSP_S;

/* �豸�б�����Ϣ */
typedef struct
{
    U32                     u32StSize;                        /**< sizeof(DEVINFO_RSP_S) */

    S8                      s8AddrCode[MAX_ADDR_LEN];         /**< �豸���� */
    S8                      s8DevName[MAX_NAME_LEN];          /**< �豸��ַ���� */
    S8                      s8PareAddrCode[MAX_ADDR_LEN];     /**< ���豸��ַ���� */
    S8                      s8DevDesc[100];                   /**< �豸���� */

    S32                     s32ChnNum;                        /**< �豸ͨ���� */
    S32                     s32ChnNo;                         /**< �豸ͨ���� */
    U8                      u8IPAddr[MAX_IPV4_LEN];           /**< �豸IP��ַ */

    U16                     u16Port;                          /**< �豸�˿ں� */
    U16                     u16DevType;                       /**< �豸���� */

    S8                      s8ManuType;                       /**< �豸�������� */
    S8                      s8ProtoType;                      /**< Э������ */
    S8                      s8StreamType;                     /**< �������� */
} DEVINFO_RSP_S;

typedef struct
{
    CHN_INFO_S              stChnInfo;
} PVIEW_RSP_S;

typedef struct
{
    U32                     u32Flag;                          /**< 0 - ��̨���ƿ�ʼ��1 - ��̨����ֹͣ */
    U32                     u32Speed;                         /**< ��̨�����ٶ� */
} PTZ_CTRL_NORMAL_S;

typedef struct
{
    U32                     u32Flag;                          /**< 0 - ��̨���ƿ�ʼ��1 - ��̨����ֹͣ */
    U32                     u32No;
} PTZ_CTRL_CRUISE_S;

typedef struct
{
    U32                     u32Index;                         /**< ��̨���û����Ԥ�õ��, ��0��ʼ */
} PTZ_CTRL_PRESET_S;

typedef struct
{
    U32                     u32StSize;
    U32                     u32PresetNo;                      /**< Ԥ�õ�� */
    S8                      s8PresetName[MAX_NAME_LEN];       /**< Ԥ�õ����� */
} GET_PRESET_RSP_S;

typedef struct
{
    U32                     u32StSize;
    U32                     u32PresetNo;                      /**< Ԥ�õ�� */
    S8                      s8PresetName[MAX_NAME_LEN];       /**< Ԥ�õ����� */
    U8                      u32PresetType;                    /**< Ԥ��λ���� */
} GET_PRESET_RSP_S_EX; /* �¹�Լ */

#ifndef _DATETIME_
#define _DATETIME_
typedef struct
{
    U32                     u32Year;                          /**< ��, 2013 */
    U32                     u32Month;                         /**< ��, 1~12 */
    U32                     u32Day;                           /**< ��, 1~31 */
    U32                     u32Hour;                          /**< ʱ, 0~23 */
    U32                     u32Minute;                        /**< ��, 0~59 */
    U32                     u32Second;                        /**< ��, 0~59 */
} DATETIME_S;
#endif

/* ¼������ */
#define RECORD_NORMAL	    0x1                               /**< ����¼�� */
#define RECORD_ALARM	    0x2                               /**< ����¼�� */
#define RECORD_TIME         0x4                               /**< ��ʱ¼�� */
#define RECORD_ALL		    0xFFFFFFFF                        /**< Only for req, equal to RECORD_NORMAL|RECORD_ALARM... */

typedef struct
{
    U32                     u32StSize;                        /**< sizeof(REC_DETAIL_RSP_S) */
    U32                     u32ChnNO;                         /**< ͨ���� */
    DATETIME_S              stBegin;                          /**< ¼��ʼʱ�� */
    DATETIME_S              stEnd;                            /**< ¼�����ʱ�� */
    U32                     u32Type;                          /**< ¼������ */
    U32                     u32Size;                          /**< ¼���ļ��ܳ��� */
    S8                      s8FileName[MAX_FILENAME_LEN];     /**< ¼���ļ��� */
} REC_DETAIL_RSP_S;

typedef struct
{
    U32                     u32StSize;                        /**< REC_CALENDAR_RSP_S */
    DATETIME_S              stDay;
} REC_CALENDAR_RSP_S;

enum
{
    REC_PLAY_IP_TIME = 0,                                     /**<  0-IP+PORT+ʱ��λط� */
    REC_PLAY_IP_NAME,                                         /**<  1-IP_PORT+�ļ��ط� */
    REC_PLAY_ADDR_URL,                                        /**<  2-��ַ��+RTSP URL�ط� */
    REC_PLAY_ADDR_TIME,                                       /**<  3-��ַ��+ʱ��ط� */
	REC_PLAY_HIKPLATFORM,                                      /**< ����ƽ̨�ط� */
};

/* �������ṹ */
typedef struct
{
    S8                      s8DeviceName[MAX_NAME_LEN];       /**< �����豸�� */
    U8                      u8DeviceIp[MAX_IPV4_LEN];         /**< �����豸IP */

    U16                     u16DevicePort;                    /**< �����豸�˿� */
    U8                      u8Res[2];
} ALARMER_S;

typedef struct
{
    DATETIME_S              stTime;                           /**< ��������ʱ�� */
    U8                      u8ChnNo;                          /**< ����ͨ���� */
    U8                      u8Status;                         /**< ����״̬��0-��ʼ 1-���� */
} ALARM_IO_RPT_S;

typedef struct
{
    DATETIME_S              stTime;                           /**< ��������ʱ�� */
    U8                      u8ChnNo;                          /**< ����ͨ���� */
    U8                      u8Status;                         /**< ����״̬��0-��ʼ 1-���� */
} ALARM_MD_RPT_S;

typedef struct
{
    DATETIME_S              stTime;                           /**< ��������ʱ�� */
    U8                      u8ChnNo;                          /**< ����ͨ���� */
    U8                      u8Status;                         /**< ����״̬��0-��ʼ 1-���� */
} ALARM_VLOSS_RPT_S;

typedef struct
{
    DATETIME_S              stTime;                           /**< ��������ʱ�� */
    U8                      u8HddNo;                          /**< ����Ӳ�̺� */
    U8                      u8Status;                         /**< ����״̬��0-��ʼ 1-���� */
} ALARM_HDD_RPT_S;

/* �豸״̬�ϴ��ṹ */
typedef struct
{
    DATETIME_S              stTime;                            /**< ��������ʱ�� */
    U8                      u8DeviceIp[MAX_IPV4_LEN];          /**< �����豸IP */

    U16                     u16DevicePort;                     /**< �����豸*/
    U8                      u8Status;                          /**< ����״̬��0-���� 1-���� */
} ALARM_DEVSTATE_RPT_S;

typedef struct
{
    S8                      s8TaskName[MAX_FILENAME_LEN];      /**< ���������� */

    U8                      u8DeviceIp[MAX_IPV4_LEN];          /**< �����豸IP */

    U8                      u8ChnNO;                           /**< �����豸ͨ���� */
    U8                      u8PresetNo;                        /**< �����豸Ԥ�õ�� */
    U16                     u16TaskId;                         /**< ��������ID */
} LINKAGE_RPT_S;

/* �ͻ�������ת��������Ϣ */
typedef struct
{
    S8                      s8SrcName[MAX_NAME_LEN];           /**< ����ת��Դ(�ͻ���)���� */
    U8                      u8SrcIp[MAX_IPV4_LEN];             /**< ����ת��Դ(�ͻ���)IP */
    S8                      s8Desc[MAX_FILENAME_LEN];          /**< �������� */
    U32                     u32DeviceNum;                      /**< �����豸����, 1-16 */
    U32                     u32CameraId[16];                   /**< �����豸ͨ��ID�� */
    U8                      u8PresetNo[16];                    /**< �����豸Ԥ�õ�� */
} LINKAGE_TRANS_RPT_S;

enum
{
    AI_TYPE_TRIPWIRE  = 4,                                    /**< ���� */
    AI_TYPE_PERIMETER,                                        /**< �ܽ� */
    AI_TYPE_ITEMLEFT,                                         /**< ��Ʒ������ʧ */
    AI_TYPE_PLATE,                                            /**< ���Ƽ�� */
    AI_TYPE_HOVER,                                            /**< �ǻ����  */
    AI_TYPE_ANTITHEFT,                                        /**< �������   */
    AI_TYPE_OFFJOB,                                           /**< �Ѹڼ��   */
    AI_TYPE_MAINTENAREA,                                      /**< ���������� */
    AI_TYPE_PRESSPLATE,                                       /**< ѹ��Ͷ��״̬��� */
    AI_TYPE_INDICATOR,                                        /**< �ֺ�ָʾ�Ƽ�� */
    AI_TYPE_DISCONNECTOR,                                     /**< ���뿪��(��ͷ)״̬��� */
    AI_TYPE_LIGHT,                                            /**< ָʾ��״̬���  */
    AI_TYPE_HELMET,                                           /**< ��ȫñ��� */
    AI_TYPE_OCRTEXT,                                          /**< �ı�ʶ�� */
	AI_TYPE_GNDWIRE,                                          /**< �ӵ���ʶ�� */
	AI_TYPE_SAFETYRULE,                                       /**< �����¼� */  
    AI_TYPE_TOWERLEAN,                                        /**< ������бʶ�� */
	AI_TYPE_SNAPMATCH,                                        /**< �����ȶ�ʶ�� */
	AI_TYPE_ELECTRICFENCE,                                    /** < ����Χ�� */
	AI_TYPE_GATE,                                             /**< ��բ״̬ */
	AI_TYPE_JUEYUAN,                                          /**< ��ۼ��-��Ե�� */
	AI_TYPE_GANTA,											  /**< ��ۼ��-���� */
	AI_TYPE_THERMOMETRYALARM,                                 /**< �����¶ȸ澯 */
	AI_TYPE_PTZ_ALARM,                                        /**< ��̨���Ƹ澯 */
	AI_YX_COUNT,
    AI_TYPE_LEDDIGIT  = 30,                                   /**< ��ƶ������(�߶������) */
    AI_TYPE_METER,                                            /**< ��ƶ������(�̶���) */
    AI_TYPE_NUMPEOPLE,                                        /**< ����ͳ�� */
    AI_TYPE_THERMOMETRY,                                      /**< ���� */
	AI_TYPE_AREAPEOPLE,                                       /**< ��������ͳ�� */	
	AI_TYPE_PASSFLOW,                                         /**< ����������ͳ�� */
};

enum
{
    PLATE_TIANDY_STANDARD = 1,                                /**< ��س���ʶ���(��׼) */
    PLATE_DAHUA_STARNDARD,                                    /**< �󻪳���ʶ���(��׼) */
    PLATE_HIK_STARNDARD,                                      /**< ��������ʶ���(��׼) */
    PLATE_HIK_NORMAL,                                         /**< ��������ʶ���(��ͨ) */
};

typedef struct
{
    S8                      s8Plate[12];                      /**< ���ƺ� */
    U8                      u8PlateType;                      /**< �������� */
    U8                      u8PlateColor;                     /**< ������ɫ */
    U8                      u8CarColor;                       /**< ������ɫ */
    U8                      u8DevType;                        /**< �豸���� ��:PLATE_TIANDY_STANDARD*/
    U16                     u16Millsecond;                    /**< ץ�ĺ��� */
    U8                      u8CarType;                        /**< �������� 1:С���� 2:���� 3:����� 0:����*/
    U8                      u8Direct;                         /**< �������� 1:�� 2:�� 0:δ֪*/
    U32                     u32PicNo;                         /**< ���ΰҵftpץ��ͼƬ������� */
} AI_PLATE_S;

/* ���ܷ��������� */
typedef struct
{
    U16                     u16Px;
    U16                     u16Py;
} AI_POINT_S;

/* ���ܷ������α��� */
typedef struct
{
    AI_POINT_S              stFrom;                           /**< �������ϵ� */
    AI_POINT_S              stTo;                             /**< �������µ� */
    U8                      u8Enable;                         /**< �Ƿ���Ч */
    U8                      u8Alarm;                          /**< �Ƿ񱨾� */
    U8                      u8Reserved[2];
} AI_RECTANGLE_S;

/* ���ܷ���������ɫ(����ʶ��) */
enum AI_COLOR_S
{
    AI_COLOR_BLACK = 0,                                       /**< ��ɫ */
    AI_COLOR_GRAY,                                            /**< ��ɫ */
    AI_COLOR_WHITE,                                           /**< ��ɫ */
    AI_COLOR_RED,                                             /**< ��ɫ */
    AI_COLOR_YELLOW,                                          /**< ��ɫ */
    AI_COLOR_GREEN,                                           /**< ��ɫ */
    AI_COLOR_BLUE,                                            /**< ��ɫ */
    AI_COLOR_PURPLE,                                          /**< ��ɫ */
    AI_COLOR_DARKGRAY,                                        /**< ���*/
    AI_COLOR_BKAIR,                                           /**< �񺽺�ɫ(����) */
    AI_COLOR_SILVER,                                          /**< ��ɫ */
    AI_COLOR_DARKBLUE,                                        /**< ���� */
    AI_COLOR_BROWN,                                           /**< ��ɫ */
    AI_COLOR_PINK,                                            /**< ��ɫ */
    AI_COLOR_CYAN,                                            /**< ��ɫ */
    AI_COLOR_ORANGE_RED,                                      /**< �Ⱥ� */
    AI_COLOR_ORGANE,                                          /**< ��ɫ */
    AI_COLOR_LIGHT_BLUE,                                      /**< ���� */
    AI_COLOR_LIGHT_YELLOW,                                    /**< ���� */
    AI_COLOR_LIGHT_CYAN,                                      /**< ���� */
    AI_COLOR_LIGHT_GREEN,                                     /**< ���� */
    AI_COLOR_LIGHT_PURPLE,                                    /**< ���� */
    AI_COLOR_YELLOW_GREEN,                                    /**< ���� */
    AI_COLOR_GOLDEN,                                          /**< ��ɫ */
    AI_COLOR_BLUE_GRAY,                                       /**< ���� */
    AI_COLOR_GREEN_GRAY,                                      /**< �̻� */
    AI_COLOR_CREAM,                                           /**< ��ɫ */
    AI_COLOR_CYAN_GREEN,                                      /**< ���� */
    AI_COLOR_DARK_RED,                                        /**< ��� */
    AI_COLOR_SKY_BLUE,                                        /**< ���� */
    AI_COLOR_SILVER_GRAY,                                     /**< ���� */
    AI_COLOR_PURPLE_RED,                                      /**< �Ϻ� */
    AI_COLOR_BROWN_RED,                                       /**< �غ� */
    AI_COLOR_BROWN_YELLOW,                                    /**< �ػ� */
    AI_COLOR_FLESH,                                           /**< ��ɫ */
    AI_COLOR_UNKNOWN = 100,
};

/* �������궨�� */
typedef struct
{
    AI_POINT_S              stFrom;                           /**< ��ʼ��   */
    AI_POINT_S              stTo;                             /**< �յ�     */

    U8                      u8Direct ;                        /**< �Ƿ�˫�� */
    U8                      u8Enable ;                        /**< �Ƿ���Ч */
    U8                      u8Alarm;                          /**< �Ƿ񱨾� */
    U8                      u8Reserved[1];
} AI_TRIPWIRE_S;


typedef struct
{
    union
    {
        struct
        {
            AI_POINT_S      stFrom;                           /**< �������ϵ� */
            AI_POINT_S      stTo;                             /**< �������µ� */
        } stAiRect;

        AI_POINT_S          stPts[10];                        /**< ����ε�����(���10����) */
    };

    U8                      u8Type;                           /**< �������� 1-���Ρ�2-�����*/
    U8                      u8Enable;                         /**< �Ƿ���Ч */
    U8                      u8Alarm;                          /**< �Ƿ񱨾� */
    U8                      u8Reserved[1];
} AI_PERIMETER_S;

typedef struct
{
    union
    {
        struct
        {
            AI_POINT_S      stFrom;                           /**< �������ϵ� */
            AI_POINT_S      stTo;                             /**< �������µ� */
        } stAiRect;

        AI_POINT_S          stPts[10];                        /**< ����ε�����(���10����) */
    };

    U8                      u8Type;                           /**< �������� 1-���Ρ�2-�����*/
    U8                      u8Enable;                         /**< �Ƿ���Ч */
    U8                      u8Alarm;                          /**< �Ƿ񱨾� */
    U8                      u8Reserved[1];
} AI_ITEMLEFT_S;

typedef struct
{
    union
    {
        struct
        {
            AI_POINT_S      stFrom;                           /**< �������ϵ� */
            AI_POINT_S      stTo;                             /**< �������µ� */
        } stAiRect;

        AI_POINT_S          stPts[10];                        /**< ����ε�����(���10����) */
    };

    U8                      u8Type;                           /**< �������� 1-���Ρ�2-�����*/
    U8                      u8Enable;                         /**< �Ƿ���Ч */
    U8                      u8Alarm;                          /**< �Ƿ񱨾� */
    U8                      u8Reserved[1];
} AI_HOVER_S;

typedef struct
{
    union
    {
        struct
        {
            AI_POINT_S      stFrom;                           /**< �������ϵ� */
            AI_POINT_S      stTo;                             /**< �������µ� */
        } stAiRect;

        AI_POINT_S          stPts[10];                        /**< ����ε�����(���10����) */
    };

    U8                      u8Type;                           /**< �������� 1-���Ρ�2-�����*/
    U8                      u8Enable;                         /**< �Ƿ���Ч */
    U8                      u8Alarm;                          /**< �Ƿ񱨾� */
    U8                      u8Reserved[1];
} AI_ANTITHEFT_S;

typedef struct
{
    union
    {
        struct
        {
            AI_POINT_S      stFrom;                           /**< �������ϵ� */
            AI_POINT_S      stTo;                             /**< �������µ� */
        } stAiRect;

        AI_POINT_S          stPts[10];                        /**< ����ε�����(���10����) */
    };

    U8                      u8Type;                           /**< �������� 1-���Ρ�2-�����*/
    U8                      u8Enable;                         /**< �Ƿ���Ч */
    U8                      u8Alarm;                          /**< �Ƿ񱨾� */
    U8                      u8Reserved[1];
} AI_OFFJOB_S;

typedef struct
{
    union
    {
        struct
        {
            AI_POINT_S      stFrom;                           /**< �������ϵ� */
            AI_POINT_S      stTo;                             /**< �������µ� */
        } stAiRect;

        AI_POINT_S          stPts[10];                        /**< ����ε�����(���10����) */
    };

    U8                      u8Type;                           /**< �������� 1-���Ρ�2-�����*/
    U8                      u8Enable;                         /**< �Ƿ���Ч */
    U8                      u8Alarm;                          /**< �Ƿ񱨾� */
    U8                      u8Reserved[1];
} AI_MAINTENAREA_S;

typedef struct
{
    AI_POINT_S              stFrom;                           /**< �������ϵ� */
    AI_POINT_S              stTo;                             /**< �������µ� */
    AI_COLOR_S              enAiColor;                        /**< ��ɫ */
    U8                      u8OnOff;                          /**< ����򿪹�״̬ */
    U8                      u8Enable;                         /**< �Ƿ���Ч */
    U8                      u8Reserved[1];
} AI_LIGHT_S;

typedef struct
{
    AI_POINT_S              stPts[3];                         /**< �е㣬ʼĩ��*/
    U8                      u8DialType;                       /**< �̶�������  */
    U8                      u8Enable;                         /**< �Ƿ���Ч */
    U8                      u8Reserved[2];

    FLOAT                   fDialValue;                       /**< �̶��̵�ǰֵ */
} AI_METER_S;

typedef struct
{
    AI_POINT_S              stFrom;                           /**< ��ʼ��   */
    AI_POINT_S              stTo;                             /**< �յ�     */
    U8                      u8Enable;                         /**< �Ƿ���Ч */
    U8                      u8Alarm;                          /**< �Ƿ񱨾� */
    U8                      u8Reserved[2];
} AI_INDICATOR_S;

typedef struct
{
    AI_POINT_S              stFrom;                           /**< ��ʼ��   */
    AI_POINT_S              stTo;                             /**< �յ�     */

    U8                      u8Enable;                         /**< �Ƿ���Ч */
    U8                      u8Alarm;                          /**< �Ƿ񱨾� */
    FLOAT                   fValue;                           /**< �����ֵ */
    U8                      u8DecNum;                         /**< С��λ�� */
    U8                      u8Reserved[3];
} AI_LEDDIGIT_S;

typedef struct
{
    U8						s8IsChange[MAX_OBJ_PRESSPLATE];/** < �ж��Ƿ�ı�,1�ı䣬0����*/
    S8						s8CurState[MAX_OBJ_PRESSPLATE];   /**< ��ǰ״̬ 0(ѹ���˳�״̬),1(ѹ��Ͷ��״̬) */
    U8						u8Enable[MAX_OBJ_PRESSPLATE];     /**< ��ǰ״̬�Ƿ���Ч*/
    U8						u8PressPlateNum;             /**< ѹ����Ŀ */
    U8						u8Reserved[3];
} AI_PRESSPLATE_S;

typedef struct
{
    AI_POINT_S              stFrom;                           /**< ��ʼ��   */
    AI_POINT_S              stTo;                             /**< �յ�     */

    U8                      u8Alarm;                          /**< ������� 0:�� 1:�� */
    U8                      u8Enable;
    U8                      u8Reserved[2];                    /**< Ԥ�� */
} AI_DISCONNECTOR_S;

typedef struct
{
    AI_RECTANGLE_S          stMovPosition;                    /**< �ƶ��������ο� */
    U8                      u8Alarm;                          /**< ������� 0:δ���� 1:���� */
    U8                      u8Enable;
    U8                      u8Reserved[2];                    /**< Ԥ�� */
} AI_HELMET_S;

typedef struct
{
    AI_POINT_S              stFrom;                          /**< ��ʼ��   */
    AI_POINT_S              stTo;                            /**< �յ�     */

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
	AI_POINT_S        stFrom;          /**< ��ʼ��   */
    AI_POINT_S        stTo;            /**< �յ�     */
    S8                szText[256];
	U8                u8Enable;
}AI_OCRTEXT_S;

typedef struct 
{
    union
    {
        struct
        {
            AI_POINT_S      stFrom;                           /**< �������ϵ� */
            AI_POINT_S      stTo;                             /**< �������µ� */
        } stAiRect;
		
        AI_POINT_S          stPts[10];                        /**< ����ε�����(���10����) */
    };
	
    U8                      u8Type;                           /**< �������� 1-���Ρ�2-�����*/
    U8                      u8Enable;                         /**< �Ƿ���Ч ��*/
	U8                      u8Res[2]; 
}AI_GNDWIRE_S;

typedef struct 
{
	AI_POINT_S        stFrom;          /**< ��ʼ��   */
    AI_POINT_S        stTo;            /**< �յ�     */
	
    U8                u8Alarm;         /**< �������*/
    U8                u8Enable;
    U8                u8Reserved[2];   /**< Ԥ�� */
}AI_SAFETYRULE_S;

typedef struct
{
	union
	{
		struct
		{
			AI_POINT_S      stFrom;                           /**< �������ϵ� */
			AI_POINT_S      stTo;                             /**< �������µ� */
		} stAiRect;
		
		AI_POINT_S          stPts[10];                        /**< ����ε�����(���10����) */
	};
	
	U16                     u16PeopelNum;                     /**< ����ͳ�ơ�*/
	U8                      u8Enable;                         /**< �Ƿ���Ч ��*/
	U8                      u8Res[1];
}AI_AREAPEOPLE_S;

typedef struct  
{
	AI_POINT_S        stFrom;          /**< ��ʼ��   */
    AI_POINT_S        stTo;            /**< �յ�     */
	
    U8                u8Alarm;         /**< �������*/
    U8                u8Enable;
    U8                u8Reserved[2];   /**< Ԥ�� */
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
    AI_POINT_S              stFrom;                           /**< ��ʼ��   */
    AI_POINT_S              stTo;                             /**< �յ�     */
    U8                      u8Enable;                         /**< �Ƿ���Ч */
    U8                      u8Alarm;                          /**< �Ƿ񱨾� */
    U8                      u8Reserved[2];
	
}AI_GATE_S;
typedef struct
{
    AI_POINT_S              stFrom;                           /**< ��ʼ��   */
    AI_POINT_S              stTo;                             /**< �յ�     */
    U8                      u8Enable;                         /**< �Ƿ���Ч */
    U8                      u8Alarm;                          /**< �Ƿ񱨾� */
    U8                      u8Reserved[2];
	
}AI_THERMOMETRY_ALARM_S;

typedef struct
{
    AI_POINT_S              stFrom;                           /**< ��ʼ��   */
    AI_POINT_S              stTo;                             /**< �յ�     */
    U8                      u8Enable;                         /**< �Ƿ���Ч */
    U8                      u8Alarm;                          /**< �Ƿ񱨾� */
    U8                      u8Reserved[2];
	
}AI_PTZ_ALARM_S;

typedef struct
{
    AI_POINT_S              stFrom;                           /**< ��ʼ��   */
    AI_POINT_S              stTo;                             /**< �յ�     */
    U8                      u8Enable;                         /**< �Ƿ���Ч */
    U8                      u8Alarm;                          /**< �Ƿ񱨾� */
    U8                      u8Reserved[2];
	
}AI_JUEYUAN_S;

typedef struct
{
    AI_POINT_S              stFrom;                           /**< ��ʼ��   */
    AI_POINT_S              stTo;                             /**< �յ�     */
    U8                      u8Enable;                         /**< �Ƿ���Ч */
    U8                      u8Alarm;                          /**< �Ƿ񱨾� */
    U8                      u8Reserved[2];
	
}AI_GANTA_S;

typedef struct  
{
    AI_POINT_S        stPts[10];       /**< ����ε�����(���10����) */
	U8                u8Alarm;         /**< �������*/
    U8                u8Enable;
    U8                u8Reserved[2];   /**< Ԥ�� */
}AI_ELECTRICFENCE_S;

typedef struct
{
    DATETIME_S              stTime;                           /**< ��������ʱ�� */

    U8                      u8AiType;                         /**< ���ܷ�������, 1-���� 2-�ܽ� */
    U8                      u8ChnNo;                          /**< ���ܷ���ͨ���� */
    U8                      u8Preset;                         /**< ���ܷ���ͨ��Ԥ��λ */
    U8                      u8AreaType;                       /**< ���ܷ���Ŀ���������� LINE:0, RECT:1, POLY:2 */

    U16                     u16PicHeight;	                  /**< ���ܷ���ͼ��ֱ��ʸ߶� */
    U16                     u16PicWidth;                      /**< ���ܷ���ͼ��ֱ��ʿ�� */
    U32                     s32Status;                        /**< 0:�澯����, 1:�澯��ʼ */
//   U8                      u8AlarmType;                     /**< �������� 1:���� 0:���� */

    union
    {

        AI_TRIPWIRE_S       AiTripwite[MAX_AIOBJ_NUM];        /**< ���߱��� */
        AI_PERIMETER_S      AiPerimeter[MAX_AIOBJ_NUM];       /**< �ܽ籨�� */
        AI_ITEMLEFT_S       AiItemleft[MAX_AIOBJ_NUM];        /**< ��Ʒ���� */
        AI_PLATE_S          AiPlate;                          /**< ������Ϣ */
        AI_HOVER_S          AiHover[MAX_AIOBJ_NUM];           /**< �ǻ����  */
        AI_ANTITHEFT_S      AiAntitheft[MAX_AIOBJ_NUM];       /**< �������   */
        AI_OFFJOB_S         AiOffjob[MAX_AIOBJ_NUM];          /**< �Ѹڼ��   */
        AI_MAINTENAREA_S    AiMaintenarea[MAX_AIOBJ_NUM];     /**< ���������� */
        AI_PRESSPLATE_S     AiPressplate;					  /**< ѹ��Ͷ��״̬��� */
        AI_INDICATOR_S      AiIndicator[MAX_AIOBJ_NUM];       /**< �ֺ�ָʾ�Ƽ�� */
        AI_DISCONNECTOR_S   AiDisconnector[MAX_AIOBJ_NUM];    /**< ���뿪��(��ͷ)״̬��� */
        AI_LIGHT_S          AiLight[MAX_AIOBJ_NUM];           /**< ָʾ��״̬���  */
        AI_LEDDIGIT_S       AiLeddigit[MAX_AIOBJ_NUM];        /**< ��ƶ������(�߶������) */
        AI_METER_S          AiMeter[MAX_AIOBJ_NUM];           /**< ��ƶ������(�̶���) */
        AI_HELMET_S         AiHelmet[MAX_AIOBJ_NUM][20];      /**< ��ȫñ��� */
        AI_NUMPEOPLE_S      AiNumPeople[MAX_AIOBJ_NUM];       /**< ����ͳ�� */
        AI_THERMOMETRY_S    AiThermometry[MAX_AIOBJ_NUM];     /**< ���� */
		AI_OCRTEXT_S        AiOcrText[MAX_AIOBJ_NUM];         /**< �ı�ʶ�� */
        AI_GNDWIRE_S        AiGndWire[MAX_AIOBJ_NUM];         /**< �ӵ���ʶ�� */
		AI_SAFETYRULE_S     AiSafetyRule[MAX_AIOBJ_NUM];      /**< �����¼� */
        AI_AREAPEOPLE_S     AiAreaPeople[MAX_AIOBJ_NUM];      /**< ��������ͳ�� */
		AI_TOWERLEAN_S      AiTowerLean[MAX_AIOBJ_NUM];       /**< �ܹ���бʶ�� */
		AI_SNAPMATCH_S      AiSnapMatch;                      /**< �����ȶ�ʶ�� */
		AI_PASSFLOW_S       AiPassFlow;                       /**< ����������ͳ�� */
		AI_ELECTRICFENCE_S  AiEleCtriCfence[MAX_AIOBJ_NUM];   /**< ����Χ�� */
		AI_GATE_S			AiGate[MAX_AIOBJ_NUM];               /**< ��բ״̬ */	
		AI_THERMOMETRY_ALARM_S AiThermometryAlarm[MAX_AIOBJ_NUM];/**<�����¶ȸ澯*/	
		AI_PTZ_ALARM_S      AiPTZControlAlarm[MAX_AIOBJ_NUM];    /**<��̨���Ƹ澯*/	
		AI_JUEYUAN_S		AiJueYuan[MAX_AIOBJ_NUM];		     /**< ��ۼ��-��Ե�� */
		AI_GANTA_S			AiGanTa[MAX_AIOBJ_NUM];		         /**< ��ۼ��-���� */
    };

	S8                      s8PicName[MAX_PATH];                 /**< ���ܷ���ץ��ͼƬ�ļ����� */
    
} ALARM_AI_RPT_S;

/**< ϵͳ���в��� */
typedef struct
{
    time_t                  stStartTime;                       /**< ϵͳ����ʱ�� */
    time_t                  stRunTime;                         /**< ϵͳ����ʱ�� */
    U32                     u32ClientNo;                       /**< �ͻ��������� */
    U32                     u32TranTaskNo;                     /**< ת�������� */
    U32                     u32CpuUsed;                        /**< ϵͳʹ��cpu�� */
    U32                     u32TranCpuUsed;                    /**< ת������ʹ��cpu�� */
    U32                     u32PhMemTotal;                     /**< �����ڴ����� */
    U32                     u32PhMemRemain;                    /**< �����ڴ������ */
    U32                     u32VirMemTotal;                    /**< �����ڴ����� */
    U32                     u32VirMemRemain;                   /**< �����ڴ���� */
    FLOAT                   fSendNetworkTotal;                 /**< ���緢������ */
    FLOAT                   fSendNetworkRemain;                /**< ��������� */
    FLOAT                   fRecvNetworkTotal;                 /**< ����������� */
    FLOAT                   fRecvNetworkRemain;                /**< ������տ����� */
} ALARM_SYS_RPT_S;

typedef struct
{
    DATETIME_S              stTime;                            /**< ��������ʱ�� */
    U8                      u8ChnNo;                           /**< ����ͨ���� */
    U8                      u8Status;                          /**< ����״̬��0-��ʼ 1-���� */
} ALARM_MANUAL_RPT_S;

typedef struct
{
    DATETIME_S              stTime;                            /**< ��������ʱ�� */
    S8	                    s8Division[2];                     /* division[0]:'E'or'W'(����/����), division[1]:'N'or'S'(��γ/��γ) */
    U8                      u8Satelites;					   /* �������� */
    U8	                    u8Precision;					   /* �������ӣ�ԭʼֵ*100 */
    U32                     u32Longitude;                      /* ���ȣ�ȡֵ��ΧΪ��0��180*3600*100����ת����ʽΪ��longitude= ʵ�ʶ�*3600*100+ʵ�ʷ�*60*100+ʵ����*100 */
    U32                     u32Latitude;                       /* γ�ȣ�ȡֵ��ΧΪ��0��90*3600*100����ת����ʽΪ��latitude = ʵ�ʶ�*3600*100+ʵ�ʷ�*60*100+ʵ����*100 */
    U32	                    u32Direction;                      /* ����ȡֵ��ΧΪ��0��359.9*100������������Ϊ0��ת����ʽΪ��direction= ʵ�ʷ���*100 */
    U32	                    u32Speed;                          /* �ٶȣ�ȡֵ��ΧΪ��0��999.9*100000����ת����ʽΪ��speed =ʵ���ٶ�*100000���൱��cm/h */
    U32	                    u32Height;						   /* �߶ȣ���λ��cm */
} ALARM_GPS_RPT_S;

typedef struct
{
    S8                      s8RecordGuid[MAX_ADDR_LEN];        /**< �ǼǱ��� */
    S8                      s8CarNumber[MAX_ADDR_LEN];         /**< ���ƺ� */
    S8                      s8PersonCharge[MAX_ADDR_LEN];      /**< ���������� */
    S8                      s8PersonBooker[MAX_ADDR_LEN];      /**< �����Ǽ��� */
    S8                      s8IDCardNumber[MAX_ADDR_LEN];      /**< �������� */
    S8                      s8Unit[MAX_ADDR_LEN];              /**< ��λ */
    S8                      s8Department[MAX_ADDR_LEN];        /**< ���� */
    S8                      s8Content[MAX_ADDR_LEN];           /**< �������� */
    S8                      s8WorkNumb[MAX_NAME_LEN];          /**< ����Ʊ�� */
    U16                     u16PersonCount;                    /**< ��������*/
    S8                      s8CardImgUrl[MAX_NAME_LEN * 2];    /**< ���ͼ��url */
    S8                      s8CarImgUrl[MAX_NAME_LEN * 2];     /**< ����ͼ��url */
    S8                      s8Time[MAX_NAME_LEN * 2];          /**< ʱ�� */
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
	U16                     u16Mode;           // 0:ͬ������ 1:ͬ�������仯
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
	U8   u8DeviceIp[MAX_IPV4_LEN];         //���������NVR��ip
	U16  u16DevPort;
	ALARM_CAMERA_STATUS camStatus[MAX_CHANNEL_NUM];
}ALARM_CAMERA_RPT_S;


typedef struct
{
    U32                     u32StSize;                         /**< �ṹ�峤�� */
    S8                      s8Name[MAX_NAME_LEN];              /**< Զ�̿ͻ����� */
    S32                     s32LocalUserId;                    /**< Զ�̵�¼�û�ID */
    S8                      s8IP[MAX_IP_STR_LEN];              /**< Զ�̿ͻ���IP��ַ */
    U16                     u16Port;                           /**< Զ�̿ͻ��Ķ˿ں� */
    U32                     u32ClientKey;                      /**< ����ͻ��ؼ��� */
    U32                     u32DTalkState;                     /**< ����Ƶ�Խ�״̬ */
} CLIENT_INFO_RSP_S;

typedef struct
{
    U32                     u32StSize;                         /**< �ṹ�峤�� */
    S8                      s8AddrCode[MAX_ADDR_LEN];          /**< �豸��ַ���� */
    S8                      s8DevName[MAX_NAME_LEN];           /**< �豸����  */
    S32                     s32DecoderTag;                     /**< ��������ǩ */
    S32                     s32Longitude;                      /**< ���� */
    S32                     s32Latitude;                       /**< γ�� */
} RESOURCE_RSP_S;

typedef struct
{
    U8                      u8ReqType;                         /**< ����ʽ��0-IP+���� 1-��ַ�뷽ʽ */
    U8                      u8TalkType;                        /**< �Խ���ʽ��0-��Ƶ�Խ� 1-��Ƶ�Խ� 2-����Ƶ�Խ� 3-��Ƶ���� 4-��Ƶ���� 5-����Ƶ���� */
    S8                      s8LocalName[MAX_NAME_LEN];         /**< �������� */
    U8                      u8LocalIp[MAX_IPV4_LEN];           /**< ����IP */
    U32                     u32UserSize;                       /**< �û����ݳ��� */
} DTALK_MSG_S;

typedef struct
{
    U32                     u32StSize;                         /**< �ṹ�峤�� */
    U32                     u32Id;	                 	       /**< ����ID�� */
    U32                     u32Type;			               /**< �������ͣ�����UTSYSOBJ_OBJTYPE */
    S8                      s8Name[MAX_OBJECTNAME_LEN];	       /**< �������� */
    U32                     u32ParentId;		               /**< ����ID�ţ����Ϊ-1����ʾ�޸�ID */
    U32                     u32ParentType;                     /**< �������ͣ�����UTSYSOBJ_OBJTYPE */
    U8	                    u8DvrIP[MAX_IPV4_LEN];	           /**< ����dvr�豸��ip��ַ����������ΪԤ��λʱ���� */
    U32                     u32ChnNo;		                   /**< Ԥ��λ��Ӧ��DVR�豸ͨ����, ��1��ʼ */
    U16                     u16Port;		                   /**< ����dvr�豸�������˿�, ��������ΪԤ��λʱ����*/
    U16	                    u16Preset;		                   /**< Ԥ�õ���� 0��ʾ��Ԥ�õ� ��Ŵ�1��ʼ (��ע:Ԥ��λ�µ����ܷ����㹲�ø��ֶ�,��Ŵ�1��ʼ)*/
    U8                      u8IsIVS;                           /**< �Ƿ�Ϊ���ܷ��� */
    S8                      s8IVSType;                         /**< ���ܷ���������� 0:YX:ң������ 1:YC��ң������ 2:YKң������ ����:-1*/
    U8                      u8Reseverd[2];
} DV_PARAMSET_S;

typedef struct
{
    U32                     u32StSize;                         /**< �ṹ�峤�� */
    U32                     u32Id;	                 	       /**< ����ID�� */
    U32                     u32Type;			               /**< �������ͣ�����UTSYSOBJ_OBJTYPE */
    S8                      s8Name[MAX_OBJECTNAME_LEN];	       /**< �������� */
    U32                     u32ParentId;		               /**< ����ID�ţ����Ϊ-1����ʾ�޸�ID */
    U32                     u32ParentType;                     /**< �������ͣ�����UTSYSOBJ_OBJTYPE */
    U8	                    u8DvrIP[MAX_IPV4_LEN];	           /**< ����dvr�豸��ip��ַ����������ΪԤ��λʱ���� */
    U32                     u32ChnNo;		                   /**< Ԥ��λ��Ӧ��DVR�豸ͨ����, ��1��ʼ */
    U16                     u16Port;		                   /**< ����dvr�豸�������˿�, ��������ΪԤ��λʱ����*/
    U16	                    u16Preset;		                   /**< Ԥ�õ���� 0��ʾ��Ԥ�õ� ��Ŵ�1��ʼ (��ע:Ԥ��λ�µ����ܷ����㹲�ø��ֶ�,��Ŵ�1��ʼ)*/
    U8                      u8IsIVS;                           /**< �Ƿ�Ϊ���ܷ��� */
    S8                      s8IVSType;                         /**< ���ܷ���������� 0:YX:ң������ 1:YC��ң������ 2:YKң������ ����:-1*/
    U8                      u8PresetType;                      /**< Ԥ��λ���� */
    U8                      u8AreaType;                        /**< �������� */
    S8                      s8Desc[MAX_OBJECTNAME_LEN];        /**< ������� */
} DV_PARAMSET_S_EX;

typedef struct
{
    U32                     u32StSize;
    U32                     u32Id;	                 		   /**< Ѳ��·��ID��Ѳ�Ӳ���ID*/
    U32                     u32Type;			               /**< Ѳ�Ӷ������ͣ�����UT_TOUR_TYPE*/
    S8                      s8Name[MAX_NAME_LEN];		       /**< Ѳ��·�����ƻ�*Ѳ���豸���� */
    U32                     u32No;	                 		   /**< Ѳ�Ӳ������ */
    U32                     u32DevId;	                 	   /**< Ѳ���豸ID */
    U32                     u32ParentId;                       /**< �޸��ڵ���Ϊ0*/
    U8	                    u8Hour;                            /**< Ѳ��ʱ��*/
    U8	                    u8Minute;
    U8	                    u8Second;
    U8                      u8Reseverd[1];
} TL_PARAMSET_S;

#endif

/**
 * �豸����
 */

/** ��¼ʱ��Ϣ�ص��붨�� */
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
 * @brief  ��¼״̬��Ϣ�ص���������
 *
 * @param [out] u32Code ��Ϣ�ص���
 * @param [out] pUser   �û�����
 */
typedef VOID (*pfnConnCallback)(U32 u32Code, VOID *pUser);


/**
 * @brief  ����������ݻص���������
 *
 * @param [out] u16Type ��������
 * @param [out] pData   �������ݵĻ�����ָ��
 * @param [out] u32Size ��������С
 * @param [out] pUser   �û�����
 */
enum
{
    ALARM_IO  = 1,                                               /**< I/O̽ͷ�澯��ʼ */
    ALARM_MD,                                                    /**< �ƶ����澯��ʼ */
    ALARM_VLOSS, 	                                             /**< ��Ƶ��ʧ�澯��ʼ */
    ALARM_VCOVER,                                                /**< ��Ƶ�ڵ��澯��ʼ */
    ALARM_HDD,                                                   /**< Ӳ�̸澯��ʼ��ʼ */
    ALARM_AI,  		                                             /**< ���ܷ����澯��ʼ */
    REPORT_DEV_STAT,                                             /**< �豸״̬�ϸ� */
    LINKAGE_PUSH,                                                /**< ��������ϴ� */
    LINKAGE_TRANSMIT,                                            /**< �ͻ�������ת�� */
    ALARM_VI,                                                    /**< ���ܷ���Ѳ������澯��ʼ */
    DEVICE_STATUS_UPLOAD,                                        /**< �豸״̬�ϸ� */
    SYSTEM_INFO_UPLOAD,                                          /**< ϵͳ���в��� */
    ALARM_MANUAL,                                                /**< �ⲿ�ƶ��ֳ��豸�ֶ��澯 */
    ALARM_GPS,                                                   /**< �ⲿ�ƶ��ֳ��豸GPS�澯 */
    ALARM_ACCESS,                                                /**< ƽ�����Ǽ� */
	ALARM_CALL_STATUS,                                           /**< ����������״̬ */
	ALARM_PTZ_CALL,                                              /**< ��̨���ø澯�¼� */
	ALARM_IVA_STATE,                                             /**< ���ܷ���״̬ */
	ALARM_CAMERA_STATE,                                          /**< NVR�������״̬�澯 */
};

enum
{
    WFLINK_TASK_START  = 1,                                      /* 5������Ʊ����ʼ */
    WFLINK_TASK_OPERING,                                         /* 5������Ʊ������� */
    WFLINK_TASK_END, 	                                         /* 5������Ʊ������� */
    WFLINK_SIMU_OPERING,                                         /* 5��ģ����� */
    WFLINK_SIMU_SCOUT,  		                                 /* 5��ģ����Ѳ�� */
};

typedef VOID (*pfnAlarmCallback)(ALARMER_S *pAlarmer, U16 u16Type, VOID *pData, U32 u32Size, VOID *pUser);

typedef struct
{
    S8    * s8Ip;                                                /**< ���ӷ�����IP */
    U16     u16Port;                                             /**< ���ӷ������˿� */
    S8    * ps8Username;                                         /**< ��¼�û��� */
    S8    * ps8Password;                                         /**< ��¼���� */
    S8    * ps8Name;                                             /**< �ͻ������� */
    S8    * ps8ClientIp;                                         /**< �ͻ���IP */
    U16     u16ClientTalkPort;                                   /**< �ͻ��˿��ŵĶԽ��˿�*/
    HWND    hWnd;                                                /**< ��¼������Ϣ���ʹ���, ��windowsƽ̨ */
    U32     u32Msg;                                              /**< ��¼������Ϣ����, ��windowsƽ̨ */
    VOID  * pUser;                                               /**< �û����� */
    BOOL    bAutoReconn;                                         /**< �Ƿ����� */
    BOOL    bAlarmDesired;                                       /**< �Ƿ����������� */
    pfnAlarmCallback fAlarmCallback;                             /**< �������ݻص� */
    pfnConnCallback  fConnCallback;                              /**< ��¼��Ϣ�ص� */
} NET_CONN_PARAM_S;

/**
 * @brief  ����������ݻص���������
 *
 * @param [out] u16Type ��������
 * @param [out] pData   �������ݵĻ�����ָ��
 * @param [out] u32Size ��������С
 * @param [out] pUser   �û�����
 */
typedef VOID (*pfnWFLinkageCallback)(ALARMER_S *pAlarmer, U16 u16Type, VOID *pData, U32 u32Size, VOID *pUser);

/**
 * @brief  �û��б���Ϣ�ص���������
 *
 * @param [out] pUserInfo �û���Ϣ
 * @param [out] pUser     �û�����
 */
typedef VOID (*pfnUserListCallback)(const USERINFO_RSP_S *pUserRsp, VOID *pUser);
typedef struct
{
    VOID  * pUser;                                               /**< �û����� */
    pfnUserListCallback fUserListCallback;                       /**< �û���Ϣ���ݻص� */
} NET_USER_PARAM_S;

/**
 * @brief  �豸�б���Ϣ�ص���������
 *
 * @param [out] pDevInfo  �豸��Ϣ
 * @param [out] pUser     �û�����
 */
typedef VOID (*pfnDeviceListCallback)(const DEVINFO_RSP_S *pDevInfo, VOID *pUser);
typedef struct
{
    S8    * ps8AddrCode;                                         /**< �豸��ַ�� */
    S8    * ps8DeviceIp;                                         /**< �豸IP */
    S8    * ps8DeviceName;                                       /**< �豸�� */
    U16     u16DevicePort;                                       /**< �豸�˿� */

    VOID  * pUser;                                               /**< �û����� */
    pfnDeviceListCallback fDeviceListCallback;                   /**< �豸�б����ݻص� */
} NET_DEVICE_PARAM_S;
/**
 * Ԥ��
 */

/**
 * @brief  �������ݻص���������
 *
 * @param [out] hWnd    ��ǰ��Ԥ�����
 * @param [out] pData   ������ݵĻ�����ָ��,UT_FRAME_HEAD_S+����Ƶ����
 * @param [out] u32Size ��������С
 * @param [out] pUser   �û�����
 */
typedef VOID (*pfnRealDataCallback)(HWND hWnd, VOID *pData, U32 u32Size, VOID *pUser);

typedef struct
{
    U32     u32ChnNO;                                            /**< ͨ���� 0,1,2,3... */
    U8      u8Type;                                              /**< 0-������, 1-������ */
    U8      u8Content;                                           /**< 0-������, 1-��Ƶ��, 2-��Ƶ�� */

    S8    * ps8FrontAddrCode;                                    /**< ǰ���豸��ַ����, �ǿ�����õ�ַ�뷽ʽ����ǰ���豸���������õ�IP�Ͷ˿ڲ������� */
    S8    * ps8DeviceIp;                                         /**< �豸IP */
    U16     u16DevicePort;                                       /**< �豸�˿� */
    S8    * ps8SubServIp;                                        /**< ������ý�������IP */
    U16     u16SubServPort;                                      /**< ������ý��������˿� */

    U32     u32Proto;                                            /**< 0-TCP,1-UDP */
    BOOL    bAutoReconn;                                         /**< �Ƿ����� */
    VOID  * pUser;                                               /**< �û����� */
    HWND    hWnd;                                                /**< ���Ŵ��ڵľ����ΪNULL��ʾ����ʾͼ�� */
    U32     u32Msg;
    pfnRealDataCallback fRealDataCallback;                       /**< �������ݻص����� */
} NET_PVIEW_PARAM_S;

typedef struct
{
    U32     u32ChnNO;                                            /**< ͨ���� 0,1,2,3... */
    U8      u8Type;                                              /**< 0-������, 1-������ */
    U8      u8Reserved[3];

    S8    * ps8FrontAddrCode;                                    /**< ǰ���豸��ַ���� */
    S8    * ps8DeviceIp;                                         /**< �豸IP��ַ */
    U16     u16DevicePort;                                       /**< �豸�˿ں� */
    S8    * ps8SubServIp;                                        /**< ������ý���������ַ */
    U16     u16SubServPort;                                      /**< ������ý��˿ں� */
} NET_REQI_PARAM_S;

/**
 * Ԥ�������л�
 */
typedef enum
{
    SHOW_MODE_1     = 1,                                         /**< ������ */
    SHOW_MODE_4     = 4,                                         /**< 4���� */
    SHOW_MODE_9     = 9,                                         /**< 9���� */
    SHOW_MODE_16    = 16,                                        /**< 16���� */
} NET_SHOW_MODE_E;

typedef struct
{
    U32     u32Mode;                                             /**< NET_SHOW_MODE_Eֵ */
    U32     u32Channel;                                          /**< Ŀ���л�ͨ��ֵ, һλ��Ӧһ��ͨ�� */

    S8    * ps8DeviceIp;                                         /**< �豸IP��ַ */
    U16     u16DevicePort;                                       /**< �豸�˿ں� */
    S8    * ps8SubServIp;                                        /**< ������ý���������ַ */
    U16     u16SubServPort;                                      /**< ������ý��˿ں� */
} NET_SWITCH_PARAM_S;


/**
 * �����л�
 */
typedef struct
{
    U32     u32InPort;                                           /**< ��������˿ں� */
    U32     u32OutPort;                                          /**< ��������˿ں� */

    S8    * ps8DeviceIp;                                         /**< �豸IP��ַ */
    U16     u16DevicePort;                                       /**< �豸�˿ں� */
    S8    * ps8SubServIp;                                        /**< ������ý���������ַ */
    U16     u16SubServPort;                                      /**< ������ý��˿ں� */
} NET_MATRIX_PARAM_S;

/**
 * �����������л�
 */
typedef struct
{
    U32     u32Mode;                                              /**< ��ʾģʽ��NET_SHOW_MODE_E*/
    U32     u32Channel;                                           /**< ��Ҫ������ʾ��ͨ����һλ��Ӧһ��ͨ�� */
    U32     u32OutPort;                                           /**< ��ʾ����˿�, ��1��ʼ*/

    S8    * ps8DeviceIp;                                          /**< �豸IP��ַ */
    U16     u16DevicePort;                                        /**< �豸�˿ں� */
    S8    * ps8SubServIp;                                         /**< ������ý���������ַ */
    U16     u16SubServPort;                                       /**< ������ý��˿ں� */
} NET_DECODER_PARAM_S;

typedef struct
{
    U32     u32Mode;                                              /**< ��ʾģʽ��NET_SHOW_MODE_E*/
    U32     u32OutPort;                                           /**< ��ʾ����˿� */

    S8    * ps8DeviceIp;                                          /**< �豸IP��ַ */
    U16     u16DevicePort;                                        /**< �豸�˿ں� */
    S8    * ps8SubServIp;                                         /**< ������ý���������ַ */
    U16     u16SubServPort;                                       /**< ������ý��˿ں� */
} NET_DEC_DIS_PARAM_S;

/* ��ý��������������� */
typedef struct
{
    S8    * ps8Url;                                               /**< ��ý�������IP */

    U16     u16Port;	                                          /**< ��ý��������˿� */

    S8    * ps8Username;                                          /**< ��¼�û��� */
    S8    * ps8Password;                                          /**< ��¼������ */

    U32     u32TransProtocol;	                                  /**< ����Э������ 0-TCP��1-UDP*/
} NET_DEC_STREAM_CFG_S;

/* DVR�豸ͨ����Ϣ */
typedef struct
{
    S8    * ps8Ip;                                                /**< DVR IP��ַ */
    U16     u16Port;                                              /**< DVR �˿ں� */

    S8    * ps8Username;                                          /**< DVR ��¼�û��� */
    S8    * ps8Password;                                          /**< DVR ��¼������ */

    U32     u32ChnNo;                                             /**< DVR ͨ����, ��0��ʼ */
    U32     u32TransProtocol;		                              /**< ����Э������0-TCP��1-UDP */
    U32     u32TransMode;			                              /**< ��������ģʽ 0�������� 1�������� */
    U32     u32ProtoType;			                              /**< ǰ���豸Э������ */
} NET_DEC_DVR_CFG_S;

typedef struct
{
    BOOL    bValidStream;                                         /**< ��ý��������Ƿ���� */
    union
    {
        NET_DEC_DVR_CFG_S    stDvrCfg;
        NET_DEC_STREAM_CFG_S stStreamCfg;
    };
} NET_DEC_CHN_PARAM_S;

enum
{
    CAP_BMP_MODE    = 0,                                          /**< BMP��ʽ */
    CAP_JPEG_MODE   = 1                                           /**< jpeg��ʽ */
};

typedef struct
{
    U8      u8PortType;                                           /**< �˿����͡�1-server��2-client */
    S8 *    ps8LocalIp;                                           /**< �Խ�����IP��ַ */
    U16     u16LocalPort;                                         /**< �Խ����ض˿ں� */
    S8 *    ps8RemoteIp;                                          /**< �Խ��Է�IP��ַ */
    U16     u16RemotePort;                                        /**< �Խ��Է��˿ں� */
} PC_TALK_PARAM_S;
/**
 * ��̨����
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
    U32                   u32ChnNO;                               /**< �豸ͨ����, ��0��ʼ,����������ַ��  */
    S8                  * ps8FrontAddrCode;                       /**< ǰ���豸��ַ���� */

    S8                  * ps8DeviceIp;                            /**< �豸IP��ַ */
    U16                   u16DevicePort;                          /**< �豸�˿ں� */
    S8                  * ps8SubServIp;                           /**< ������ý���������ַ */
    U16                   u16SubServPort;                         /**< ������ý��˿ں� */
    union
    {
        PTZ_CTRL_NORMAL_S stNormal;                               /**< ͨ�����Ʋ��� */
        PTZ_CTRL_CRUISE_S stCruise;                               /**< Ѳ�����Ʋ��� */
        PTZ_CTRL_PRESET_S stPreset;                               /**< Ԥ�õ���Ʋ��� */
    };
} NET_PTZ_PARAM_S;

typedef struct
{
    U32         u32ChnNO;                                         /**< �豸ͨ����, ��0��ʼ */
    S8        * ps8FrontAddrCode;                                 /**< ǰ���豸��ַ���� */

    S8        * ps8DeviceIp;                                      /**< �豸IP��ַ */
    U16         u16DevicePort;                                    /**< �豸�˿ں� */
    S8        * ps8SubServIp;                                     /**< ������ý���������ַ */
    U16         u16SubServPort;                                   /**< ������ý��˿ں� */
} NET_CHANNEL_PARAM_S;

/**
 * ¼���ѯ
 */
enum
{
    QRECORD_DETAIL = 1,                                           /**< ���Ҹ���ʱ���¼���б� */
    QRECORD_CALENDAR,                                             /**< ����ĳ����Щ����¼�� */
    QRECORD_DAY_HALFHOUR,                                         /**< ����ĳ���İ�Сʱ��¼�� */
};

/**
 * @brief  ¼���ļ��б�ص���������
 *
 * @param [out] pRsp   ¼���ļ���ϸ��Ϣ
 * @param [out] pUser  �û�����
 *
 */
typedef VOID (*pfnQueryDetailCallback)(const REC_DETAIL_RSP_S   *pRsp, VOID *pUser);

/**
 * @brief  ��Щ����¼��ص���������
 *
 * @param [out] pRsp   ¼���ļ���ϸ��Ϣ
 * @param [out] pUser  �û�����
 *
 */
typedef VOID (*pfnQueryCalendarCallback)(const REC_CALENDAR_RSP_S *pRsp, VOID *pUser);
typedef struct
{
    U32         u32QType;                                         /**< QRECORD_DETAIL, QRECORD_CALENDAR */

    U32         u32ChnNO;                                         /**< �豸ͨ����, ��0��ʼ  */
    U32         u32Type;                                          /**< RECORD_NORMAL, RECORD_MOTION��ֵ */
    DATETIME_S  stBegin;                                          /**< ������ʼʱ�� */
    DATETIME_S  stEnd;                                            /**< ��������ʱ�� */

    S8        * ps8FrontAddrCode;                                 /**< ǰ���豸��ַ���� */
    S8        * ps8DeviceIp;                                      /**< �豸IP��ַ */
    U16         u16DevicePort;                                    /**< �豸�˿ں� */
    S8        * ps8SubServIp;                                     /**< ������ý���������ַ */
    U16         u16SubServPort;                                   /**< ������ý��˿ں� */

    VOID      * pUser;                                            /**< �û����� */
    pfnQueryDetailCallback   fDetailCallback;                     /**< ¼���б�ص� */
    pfnQueryCalendarCallback fCalendarCallback;                   /**< ��Щ����¼��ص� */
} NET_QRECORD_PARAM_S;

typedef struct
{
    NET_QRECORD_PARAM_S * pRecordReq;
    U32                   u32TriggerType;                         /**< 3--�˿ڱ�����4--�ƶ�������5--��Ƶ��ʧ������0x7FFFFFFF--��Ч*/
    U32                   u32TriggerPort;                         /**< u32TriggerType=3ʱ���Ĳ���Ϊ�˿ں�*/
} NET_QRECORD_PARAM_EX_S;


#define NET_PLAYSTART		1                                     /**< ��ʼ���� */
#define NET_PLAYSTOP		2                                     /**< ֹͣ���� */
#define NET_PLAYPAUSE		3                                     /**< ��ͣ���� */
#define NET_PLAYRESTART		4                                     /**< �ָ����� */
#define NET_PLAYFAST		5                                     /**< ��� */
#define NET_PLAYSLOW		6                                     /**< ���� */
#define NET_PLAYNORMAL		7                                     /**< �����ٶ� */
#define NET_PLAYSTARTAUDIO	9                                     /**< ������ */
#define NET_PLAYSTOPAUDIO	10                                    /**< �ر����� */
#define NET_PLAYSETPOS		12                                    /**< �ı��ļ��طŵĽ��� */
#define NET_PLAYSETTIME     13                                    /**< ������ʱ�䶨λ */
#define NET_PLAYSTEP        14                                    /**< ��֡���� */
#define NET_STARTREPOS      15                                    /**< ��ʼ�϶���������������ק����ʱ���� */


/**
 * ¼��ط�
 */

/**
 * @brief  ¼��ط��������ݻص���������
 *
 * @param [out] hWnd    ��ǰ�Ļطž��
 * @param [out] pData   ������ݵĻ�����ָ��
 * @param [out] u32Size ��������С
 * @param [out] pUser   �û�����
 *
 * @return false-�����ڲ�����������������������
 */
typedef BOOL (*pfnRecordDataCallback)(HWND hWnd, VOID *pData, U32 u32Size, VOID *pUser);

typedef struct
{
    U8                          u8PlayMode;                       /**< ����ʽ���ο�REC_PLAY_IP_TIME ... */
    U8                          u8Reserved[3];

    union
    {
        struct
        {
            S8                * ps8FrontAddrCode;                 /**< ǰ���豸��ַ���� */
            union
            {
                S8            * ps8RtspUrl;                       /**< rtsp url·�� */

                struct
                {
                    DATETIME_S  stBegin;                          /**< �طſ�ʼʱ�� */
                    DATETIME_S  stEnd;                            /**< �طŽ���ʱ�� */
                };
            };
        } stAddrMode;

        struct
        {
            union
            {
                S8            * ps8FileName;                      /**< ¼���ļ���*/
                struct
                {
                    U32         u32ChnNO;                         /**< �豸ͨ����, ��0��ʼ */
                    U32         u32Type;                          /**< RECORD_NORMAL, RECORD_ALARM��ֵ */
                    DATETIME_S  stBegin;                          /**< �طſ�ʼʱ�� */
                    DATETIME_S  stEnd;                            /**< �طŽ���ʱ�� */
                };
            };

            S8                * ps8DeviceIp;                      /**< �豸IP��ַ */
            U16                 u16DevicePort;                    /**< �豸�˿ں� */
            S8                * ps8SubServIp;                     /**< ������ý���������ַ */
            U16                 u16SubServPort;                   /**< ������ý��˿ں� */
        } stIpMode;

		struct
		{
            S8            * ps8PlayUrl;                     /**< ¼��url*/
			DATETIME_S    stBegin;                          /**< �طſ�ʼʱ�� */
            DATETIME_S    stEnd;                            /**< �طŽ���ʱ�� */
			S8            * ps8DeviceIp;                    /**< �豸IP��ַ */
            U16           u16DevicePort;                    /**< �豸�˿ں� */
			U16           u16ChnNo;                         /**< �豸ͨ����, ��0��ʼ */
		}stHikMode;
    };

    VOID                      * pUser;                            /**< �û����� */
    HWND                        hWnd;                             /**< ���Ŵ��ڵľ����ΪNULL��ʾ����ʾͼ�� */
    U32                         u32Msg;
    pfnRecordDataCallback       fRecordDataCallback;              /**< �ط����ݻص����� */
} NET_RECPLAY_PARAM_S;

/**
 * ¼������
 */

/** ¼��������Ϣ�ص��붨�� */
enum
{
    MSG_DOWNLOAD_CLOSED    = 1,                    /**< �������ؽ��� */
    MSG_DOWNLOAD_OK        = 2,                    /**< ������һ���ļ� */
    MSG_DOWNLOAD_NOFILE    = 3,                    /**< һ���ļ������� */
    MSG_DOWNLOAD_FAIL      = 4,                    /**< ���ļ�����ʧ�� */
    MSG_DOWNLOAD_STOREFAIL = 5,                    /**< ���ļ��洢ʧ�� */

    MSG_PLAYBACK_END       = 20,                   /**< ���ļ��طŽ��� */
};

/**
 * @brief  ¼�������������ݻص���������
 *
 * @param [out] pData   ������ݵĻ�����ָ��
 * @param [out] u32Size ��������С
 * @param [out] pUser   �û�����
 */
typedef BOOL (*pfnRecordDownloadDataCallback)(VOID *pData, U32 u32Size, VOID *pUser);
/**
 * @brief  ¼������״̬���ݻص���������
 *
 * @param [out] u32Code ¼��������Ϣ�ص���
 * @param [out] pUser   �û�����
 */
typedef BOOL (*pfnRecordDownloadStatusCallback)(U32 u32Code, VOID *pUser);

/**
 * @brief  ¼��ط�״̬���ݻص���������
 *
 * @param [out] u32Code ¼��������Ϣ�ص���
 * @param [out] pUser   �û�����
 */
typedef VOID (*pfnRecordPlaybackStatusCallback)(U32 u32Code, VOID *pUser);

typedef struct
{
    S8        * ps8FrontAddrCode;                  /**< �豸��ַ�� */
    S8        * ps8RecFileName;                    /**< ¼���ļ��� */
    S8        * ps8FileName;                       /**< �����ļ�·�� */
    U32         u32FileSize;                       /**< ¼���ļ����� */

    S8        * ps8DeviceIp;                       /**< �豸IP��ַ */
    U16         u16DevicePort;                     /**< �豸�˿ں� */
    S8        * ps8SubServIp;                      /**< ������ý���������ַ */
    U16         u16SubServPort;                    /**< ������ý��˿ں� */

    VOID      * pUser;                             /**< �û����� */
    pfnRecordDownloadDataCallback   fRecordDataCallback;/**< ����¼�����ݻص� */
    pfnRecordDownloadStatusCallback fStatusCallback;    /**< ����״̬��Ϣ�ص� */
} NET_RECDOWNLOAD_PARAM_S;


typedef struct
{
    S8        * ps8FrontAddrCode;                  /**< �豸��ַ�� */
    S8        * ps8FileName;                       /**< �����ļ�·�� */
    U32         u32ChnNO;                          /**< �豸ͨ����, ��0��ʼ */

    DATETIME_S  stBegin;                           /**< ���ؿ�ʼʱ�� */
    DATETIME_S  stEnd;                             /**< ���ؽ���ʱ�� */

    S8        * ps8DeviceIp;                       /**< �豸IP��ַ */
    U16         u16DevicePort;                     /**< �豸�˿ں� */
    S8        * ps8SubServIp;                      /**< ������ý���������ַ */
    U16         u16SubServPort;                    /**< ������ý��˿ں� */

    VOID      * pUser;                             /**< �û����� */
    pfnRecordDownloadDataCallback   fRecordDataCallback;/**< ����¼�����ݻص� */
    pfnRecordDownloadStatusCallback fStatusCallback;    /**< ����״̬��Ϣ�ص� */

} NET_RECDOWNLOAD_BYTIME_PARAM_S;

typedef struct
{
    DATETIME_S  stTime;                            /**< ͬ��ʱ��ֵ */
    U8          u8TimeZone;                        /**< ʱ�� */
    U8          u8IsSetLoaclTime;                  /**< �Ƿ����ñ���ʱ�� */
    U8          u8Reserved[2];
} NET_SYNTIME_PARAM_S;


enum
{
    DTALK_CONN_OK,                      /**< ����OK */
    DTALK_REQ,                          /**< �������� */
    DTALK_ACK,                          /**< ��������ȷ�ϻ�Ӧ */
    DTALK_NAK,                          /**< ��������ܾ���Ӧ */
    DTALK_STOP,                         /**< ����ֹͣ */
    DTALK_DATA,                         /**< ������������ */
    DTALK_TALKINKG,                     /**< ���ڶԽ� */
    DTALK_NO_ACK,                       /**< �Է�û��Ӧ */
};

/**
 * @brief  �Խ���Ϣ�����ݻص�
 *
 * @param  [out] u32MsgType  ��Ϣ���� DTALK_CONN_OK��DTALK_REQ��DTALK_ACK
 * @param  [out] pFromMsg  �����û������ַ�� ֻ��u32MsgTypeΪDTALK_REQ��DTALK_STOP����ֵ������Ϊ��
 * @param  [out] pUser �û�����
 *
 * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
 *
 * @see
 */
typedef S32(*pfnDTalkMsgCallback)(U32 u32MsgType, DTALK_MSG_S *pFromMsg, VOID *pUser);
typedef struct
{
    U32                 u32CollectFlag;            /**< �Ƿ��ڲ��ɼ�����Ƶ, �ڲ��ɼ���� bit0-�ɼ���Ƶ bit1-�ɼ���Ƶ */
    HWND                hRemoteWnd;                /**< �Է���Ƶ��ʾ���ڵľ����ΪNULL��ʾ����ʾͼ�� */
    HWND                hLocalWnd;                 /**< ���ش�����ʾ��� */
    HWND                hMsgWnd;                   /**< ��Ƶ�ɼ���Ϣ֪ͨ����, WM_VIDEO_CAPTURE��Ϣ */
    pfnDTalkMsgCallback fDTalkMsgCallback;         /**< �Խ���Ϣ�ص����� */
    pfnRealDataCallback fDTalkDataCallback;        /**< �Խ�����Ƶ���ݻص����� */
    VOID              * pUser;                     /**< �û����� */
} NET_DTALK_PARAM_S;

typedef struct
{
    U32         u32MsgType;                        /**< ��Ϣ���� 1-����Խ� 2-ֹͣ�Խ� */
    S8        * ps8LocalName;                      /**< �������� */
    S8        * ps8LocalIp;                        /**< ����IP */

    U8          u8TalkType;                        /**< �Խ���ʽ��0-��Ƶ�Խ� 1-��Ƶ�Խ� 2-����Ƶ�Խ� 3-��Ƶ���� 4-��Ƶ���� 5-����Ƶ���� */
    U8          u8Reserved[1];
    U32         u32UserNum;                        /**< �Խ��û��� */
} NET_DTALK_MSG_S;

typedef struct
{
    S8        * ps8Name;                           /**< �Է�IP */
    S8        * ps8Ip;                             /**< �Է�IP */
    S8        * ps8UserAddrCode;                   /**< �Է��û���ַ���� */
} NET_DTALK_USER_S;

typedef struct
{
    S8        * ps8Ip;                             /**< �豸ip */
    U16         u16Port;                           /**< �豸port */
    U8          u8ChnNo;                           /**< ͨ���� */
} NET_DTALK_DEV_INFO;

//************************************************************************
//�������ƣ�C2DevTalkStatus_CB(int code, void* userdata)
//����˵������ʼ�Խ�״̬�ص�
//����������
//    (in) code ״̬�� 0:�Խ��ɹ�,1:���ӷ�����ʧ��,2:�豸ռ��
//    (in) userdata �û�����
//����ֵ��void
//************************************************************************/
typedef void (*C2DevTalkStatus_CB)(int code, void* userdata);


typedef struct
{
    U32                 u32DevListNum;             /**< �Խ��豸�� */
    NET_DTALK_DEV_INFO *pDevList;                  /**< �Խ��豸�б� */
	C2DevTalkStatus_CB  devTalk_cb;                /**< �Խ�״̬�ص����� */
	void               *userdata;                  /**< �û����� */
} NET_DTALK_DEV_LIST;

/**
 * ����������
 */
enum
{
    PARAM_TYPE_DV = 1,
    PARAM_TYPE_CRUISE,
    PARAM_TYPE_DV_EX,  /* �¹�Լ */
    PARAM_TYPE_DV_EX0815, /* �¹�Լ����Ԥ��λ����*/
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

    /** SDK��ʼ�� */

    /**
     * @brief  ��ʼ��SDK����������SDK������ǰ��
     *
     * @return �ɹ�����TRUE, ����FALSE, ��ȡ���������NET_GetLastError
     *
     * @see    NET_Release
     */
    NET_API BOOL NET_Init();


    /**
     * @brief  �ͷ�SDK��Դ���ڳ������֮ǰ���á�
     *
     * @return �ɹ�����TRUE, ����FALSE, ��ȡ���������NET_GetLastError
     *
     * @see    NET_Init
     */
    NET_API VOID NET_Release();

    /**
     * @brief  �����������Ĵ����롣
     *
     * @return ����ֵΪ������,�����:
     *
     */
    NET_API UINT NET_GetLastError();

    /** �豸���� */

    /**
     * @brief  �û���¼
     * @param [in] pparam ���Ӳ������������Ϣ���ο�NET_CONN_PARAM_S
     * @param [in] u32Mode ����ͬģʽ
     *                      0 - ����˽��ģʽ��Ĭ��
     *                      1 - ����ƽ̨
	 *                      2 - ��ƽ̨��ý��
     * @return �ɹ����ؾ��,��ʾһ���Ѿ����ӵ��豸�� ʧ�ܷ���NULL��
     *
     * @note   ע��Ҫ���淵�صľ��,��������ز�����Ҫ����þ����ע���
     *         NET_ConnStop����ʹ��
     *
     * @see    NET_ConnStop
     */
    NET_API HANDLE NET_ConnStart(const NET_CONN_PARAM_S *pParam, U32 u32Mode = 0);

    /**
     * @brief  �û��˳���¼
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     *
     * @see    NET_ConnStart
     */
    NET_API VOID NET_ConnStop(HANDLE *hConn);

    /**
     * @brief  ��ӱ�����ý�������
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] ip ��ý��ip
	 * @param [in] port ��ý��port
	 *
     * @see    �ɹ�����TRUE,ʧ�ܷ���FALSE
     */
	NET_API BOOL NET_AddStandBy(HANDLE hConn, const char *ip, int port);

    /**
     * @brief  ɾ��������ý�������
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] ip ��ý��ip
	 * @param [in] port ��ý��port
	 *
     * @see    �ɹ�����TRUE,ʧ�ܷ���FALSE
     */
	NET_API BOOL NET_DelStandBy(HANDLE hConn, const char *ip, int port);

    /**
     * @brief  ��ȡ��¼���ص��豸�����Ϣ
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [out] pRsp  �豸�����Ϣ�ṹ�壬�ο�LOGIN_RSP_S
     *
     * @return �ɹ�����TRUE, ����FALSE
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_GetLoginRsp(HANDLE hConn, LOGIN_RSP_S *pRsp);

    /**
     * @brief  ��ȡ�û��б���Ϣ
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] pParam ��ȡ�û���Ϣ�б����
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_GetUserList(HANDLE hConn, const NET_USER_PARAM_S *pParam);

    /**
     * @brief  ��ȡĳ��ϵͳ���豸���������豸�б���Ϣ
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] pParam ��Ҫ��ȡ�豸��Ϣ�б��ϵͳ���豸���Ĳ�������
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_GetDeviceList(HANDLE hConn, const NET_DEVICE_PARAM_S *pParam);

    /** Ԥ�� */

    /**
     * @brief  ����ĳ̨�豸ĳ��ͨ������ƵԤ��
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] pparam����Ԥ����ز������ã��ο�NET_PVIEW_PARAM_S
     *
     * @return �ɹ�����һ���������ʾһ���Ѵ򿪵�ͨ����ʧ�ܷ���NULL��
     *
     * @note   �ɹ����ú�������NET_StopPreview
     *
     * @see    NET_StopPreview, NET_ConnStart
     */
    NET_API HANDLE NET_StartPreview(HANDLE hConn, const NET_PVIEW_PARAM_S *pParam);

    /**
     * @brief  ֹͣĳ̨�豸ĳ��ͨ������ƵԤ��
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] hPView NET_StartPreview����ֵ
     *
     * @see    NET_StartPreview, NET_ConnStart
     */
    NET_API VOID NET_StopPreview(HANDLE  hConn, HANDLE *hPView);

    /**
     * @brief  ����ʵʱ�������ϲ���һ��I֡
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] pparam ����ؼ�֡�������ã��ο�NET_REQI_PARAM_S
     *
     * @return �ɹ�����TRUE, ����FALSE
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_RequestIFrame(HANDLE hConn, const NET_REQI_PARAM_S *pParam);

    /**
     * @brief  Ԥ�������л�
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] pparam �������л���ز������ã��ο�NET_SWITCH_PARAM_S
     *
     * @return �ɹ�����һ���������ʾһ���Ѵ򿪵�ͨ����ʧ�ܷ���NULL��
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_PreviewSwitch(HANDLE hConn, const NET_SWITCH_PARAM_S* pParam);

    /**
     * @brief  ��ȡ��¼���ص��豸�����Ϣ
     *
     * @param [in] hPView NET_StartPreview����ֵ
     * @param [out] pRsp  ��ȡ�ĵ�ǰͨ����Ϣ�ṹ�壬�ο�PVIEW_RSP_S
     *
     * @return �ɹ�����TRUE, ����FALSE
     *
     * @see    NET_StartPreview
     */
    NET_API BOOL NET_GetPreviewRsp(HANDLE hPView, PVIEW_RSP_S *pRsp);

    /**
     * @brief  �������ݲ���ŵ�ָ�����ļ��С�
     *
     * @param [in]  hPView      NET_StartPreview����ֵ
     * @param [out] ps8FileName �ļ�·��ָ��
     *                          ��ý�壺  ��׺��Ϊ.dav
     *                          ����ƽ̨����׺��Ϊ.mp4
     *
     * @return �ɹ�����TRUE, ����FALSE, ��ȡ���������NET_GetLastError
     *
     * @note   �ɹ����ú�������NET_NET_StopSaveRealData
     *
     * @see    NET_StartPreview, NET_StopSaveRealData
     */
    NET_API BOOL NET_SaveRealData(HANDLE hPView, S8 *ps8FileName);

    /**
     * @brief  ֹͣ���ݲ���
     *
     * @param [in]  hPView  NET_StartPreview����ֵ
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     *
     * @see    NET_StartPreview
     */
    NET_API BOOL NET_StopSaveRealData(HANDLE hPView);

    /**
     * @brief  Ԥ��ʱ����֡���ݲ��񲢱����ͼƬ��
     *
     * @param [in] hPView         NET_StartPreview����ֵ
     * @param [in] ps8PicFileName ͼƬ�ļ�·��ָ��
     * @param [in] u32CaptueMode  ץͼͼƬ��ʽ, ����CAP_MODE_E
     *                             0 - CAP_BMP_MODE ,32λλͼ��ʽ
     *                             1 - CAP_JPEG_MODE,jpegͼƬ��ʽ
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     *
     * @see    NET_StartPreview
     */
    NET_API BOOL NET_CapturePicture(HANDLE hPView, S8 *ps8PicFileName, U32 u32CaptueMode = CAP_BMP_MODE);

    /** ��̨ */
    /**
     * @brief  ��̨���Ʋ���
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] u32Cmd ��̨��������,�ο�PTZ_CMD_E
     * @param [in] pparam��̨������ز������ã��ο�NET_PTZ_PARAM_S
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_PTZControl(HANDLE hConn, U32 u32Cmd, const NET_PTZ_PARAM_S *pParam);

    /** ��̨(δ������Ƶͨ��Ԥ����)*/
    /**
     * @brief  ��̨���Ʋ���
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] u32Cmd ��̨��������,�ο�PTZ_CMD_E
     * @param [in] pparam��̨������ز������ã��ο�NET_PTZ_PARAM_S
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_PTZControlEx(HANDLE hConn, U32 u32Cmd, const NET_PTZ_PARAM_S *pParam);

    /** ¼���ѯ */

    /**
     * @brief  ¼���ļ�����
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] pparam¼�������ز������ã��ο�NET_QRECORD_PARAM_S
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_QueryRecord(HANDLE hConn, const NET_QRECORD_PARAM_S *pParam);

    /**
     * @brief  ¼���ļ�����(�ͼ����Ӹ澯���Ͳ���)
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] pparam¼�������ز������ã��ο�NET_QRECORD_PARAM_S
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_QueryRecordEx(HANDLE hConn, const NET_QRECORD_PARAM_EX_S *pParam);

    /** ¼��ط� */
    /**
     * @brief  ��ʼ¼��ط�
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] pparam¼��ط���ز������ã��ο�NET_QRECORD_PARAM_S
     *
     * @return �ɹ�����һ�����,��ʾһ���Ѿ��򿪵Ļط�ͨ���� ���򷵻�NULL, ��ȡ���������NET_GetLastError
     *
     * @see    NET_ConnStart, NET_StopRecordPlay
     */
    NET_API HANDLE NET_StartRecordPlay(HANDLE hConn,  const NET_RECPLAY_PARAM_S *pParam);

    /**
     * @brief  ֹͣ¼��ط�
     *
     * @param [in] hPlayback �Ѿ��������ӵĻطž��, NET_StartRecordPlay����ֵ
     *
     * @see    NET_StartRecordPlay
     */
    NET_API VOID NET_StopRecordPlay(HANDLE *hPlayback);

    /**
     * @brief  ����¼��طŵ�״̬��
     *
     * @param [in]     hPlayback �Ѿ��������ӵľ��, NET_StartRecordPlay����ֵ
     * @param [in]     u32Code    ����¼��ط�״̬����
     * @param [in]     pInVal     ָ�����������ָ��
     * @param [in]     u32InSize  ��������ĳ���
     * @param [out]    pOutVal    ָ�����������ָ��
     * @param [in,out] u32OutSize ��������ĳ���
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     *
     * @see    NET_StartRecordPlay
     */
    NET_API BOOL NET_RecordPlayCtrl(HANDLE hPlayback, U32 u32Code, VOID *pInVal = NULL, U32 u32InSize = 0, VOID *pOutVal = NULL, U32 *u32OutSize = NULL);

    /** ¼������ */

    /**
     * @brief  ��ʼ���ļ�������¼���ļ�
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] pparam¼��������ز������ã��ο�NET_RECDOWNLOAD_PARAM_S
     *
     * @return �ɹ�����һ�����,��ʾһ���Ѿ��򿪵�����ͨ���� ���򷵻�NULL, ��ȡ���������NET_GetLastError
     *
     * @see    NET_ConnStart, NET_StopRecordDownload
     */
    NET_API HANDLE NET_StartRecordDownload(HANDLE hConn,  const NET_RECDOWNLOAD_PARAM_S *pParam);

    /**
     * @brief  ֹͣ¼������
     *
     * @param [in] hDownload �Ѿ��������ӵ����ؾ��, NET_StartRecordDownload����ֵ
     *
     * @see    NET_StartRecordDownload
     */
    NET_API VOID NET_StopRecordDownload(HANDLE *hDownload);

    /**
     * @brief  ��ȡ¼�����ؽ���
     *
     * @param [in] hDownload �Ѿ��������ӵ����ؾ��, NET_StartRecordDownload����ֵ
     *
     * @return -1��ʾʧ�ܣ�0��100��ʾ���صĽ��ȣ�100��ʾ���ؽ�����������Χ0-100���緵��200�������������쳣��
     * 		   ��ȡ���������NET_DVR_GetLastError
     *
     * @see    NET_StartRecordDownload
     */
    NET_API S32 NET_GetDownloadPos(HANDLE hDownload);


    /**
     * @brief  ��ʼ��ʱ�������¼���ļ�
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] pParam¼��������ز������ã��ο�NET_RECDOWNLOAD_BYTIME_PARAM_S
     *
     * @return �ɹ�����һ�����,��ʾһ���Ѿ��򿪵�����ͨ���� ���򷵻�NULL, ��ȡ���������NET_GetLastError
     *
     * @see    NET_ConnStart, NET_StopRecordDownload
     */
    NET_API HANDLE NET_StartRecordDownloadByTime(HANDLE hConn,  const NET_RECDOWNLOAD_BYTIME_PARAM_S *pParam);

    /**
     * @brief  ֹͣ��ʱ���¼������
     *
     * @param [in] hDownload �Ѿ��������ӵ����ؾ��, NET_StartRecordDownloadByTime����ֵ
     *
     * @see    NET_StartRecordDownloadByTime
     */
    NET_API VOID NET_StopRecordDownloadByTime(HANDLE *hDownload);

    /**
     * @brief  �����л�
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] pparam ��������л���ز������ã��ο�NET_MATRIX_PARAM_S
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_MatrixSwitch(HANDLE hConn, const NET_MATRIX_PARAM_S * pParam);

    /**
     * @brief  ��������ʾ�л�
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] pparam ��������л���ز������ã��ο�NET_DECODER_PARAM_S
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_DecoderSwitch(HANDLE hConn, const NET_DECODER_PARAM_S * pParam);

    /**
     * @brief  ��ȡ��������ʾͨ��
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] pparam ��������л���ز������ã��ο�NET_DEC_DIS_PARAM_S
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     *
     * @note   ע����NET_DecoderReleaseDisplay����ʹ��
     *
     * @see    NET_ConnStart, NET_DecoderReleaseDisplay
     */
    NET_API HANDLE NET_DecoderGetDisplay(HANDLE hConn, const NET_DEC_DIS_PARAM_S * pParam);

    /**
     * @brief  �ͷŽ�������ʾͨ������
     *
     * @param [in] hDisplay ��������ʾͨ�����, NET_DecoderGetDisplay����ֵ
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     *
     * @note   ע����NET_DecoderReleaseDisplay����ʹ��
     *
     * @see    NET_DecoderReleaseDisplay
     */
    NET_API VOID NET_DecoderReleaseDisplay(HANDLE *hDisplay);

    /**
     * @brief  ��������ʼ��̬����
     *
     * @param [in] hDisplay  ��ʾͨ�����,NET_DecoderGetDisplay����ֵ
     * @param [in] pparam ��������л���ز������ã��ο�NET_DEC_CHN_PARAM_S
     * @param [in] s32DecChnNo ����ͨ����, ��0��ʼ
     * @param [in] s32DisWndNo ��ʾͨ�����ڴ��ں�, ��0��ʼ
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_DecoderStartDynamic(HANDLE hDisplay, S32 s32DecChnNo, S32 s32DisWndNo, const NET_DEC_CHN_PARAM_S *pParam);

    /**
     * @brief  �������ͷ�ĳһ��ʾͨ��ռ�еĽ���ͨ��
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] pparam ���������ز������ã��ο�NET_DEC_CHN_PARAM_S
     * @param [in] pParam ��ʾͨ����ز���
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     *
     * @see    NET_DecoderStopDynamic
     */
    NET_API BOOL NET_DecoderStopDynamic(HANDLE hConn, const NET_DEC_DIS_PARAM_S * pParam);


    /**
     * @brief  ������������ص�
     *
     * @param [in] hDisplay ��������ʾͨ�����, NET_DecoderGetDisplay����ֵ
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     *
     * @see    NET_DecoderReleaseDisplay
     */
    NET_API BOOL NET_SetWFLinkageCallback(HANDLE hConn, pfnWFLinkageCallback fWFLinkageCallback, const VOID *pUser);

    /**
     * @brief  ���߿ͻ��б�ص���������
     *
     * @param [out] pRsp   ���߿ͻ���ϸ��Ϣ
     * @param [out] pUser  �û�����
     *
     */
    typedef VOID (*pfnGetClientCallback)(const CLIENT_INFO_RSP_S *pRsp, VOID *pUser);
    /**
     * @brief  ��ȡ���߿ͻ���Ϣ
     *
     * @param  [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     *
     * @see
     */
    NET_API BOOL NET_GetClientList(HANDLE hConn, pfnGetClientCallback fClientCallback, VOID *pUser);

    /**
     * @brief  PC�Խ���Ϣ�ص�����
     *
     * @param [out] pRsp   ���߿ͻ���ϸ��Ϣ
     * @param [out] pUser  �û�����
     *
     */
    typedef VOID (*pfnPcTalkCallback)(U8 u8Type, U32 u32SockFd, VOID *pData, U32 u32Size, VOID *pUser);
    /*PC�Խ���������*/
    typedef struct
    {
        S8    * s8LocalName;                           /**< �������� */
        S8    * s8LocalIp;                             /**< ����IP */
        U16     u16LocalPort;                          /**< ���ض˿� */
        S8    * s8RemoteIp;                            /**< �Է�IP */
        U16     u16RemotePort;                         /**< �Է��˿� */
        VOID  * pUser;                                 /**< �û����� */
        pfnPcTalkCallback  fPcTalkCallback;            /**< PC�Խ���Ϣ�ص� */
    } NET_PCTALK_PARAM_S;
    /**
     * @brief  ��ʼ��PC�Խ�����
     *
     * @param [in] pParam  �Խ��������
     *
     * @return �ɹ��������Ӿ��(ָ��), ���򷵻�NULL
     */
    NET_API HANDLE NET_InitPcTalkServer(NET_PCTALK_PARAM_S * pParam);
    /**
     * @brief  ��ʼ��PC�Խ��ͻ���
     *
     * @param [in] pParam  �Խ��������
     *
     * @return �ɹ��������Ӿ��(ָ��), ���򷵻�NULL
     */
    NET_API HANDLE NET_InitPcTalkClient(NET_PCTALK_PARAM_S * pParam);

    /**
     * @brief  �ͷŶԽ������
     *
     * @param [in] hPcTalk  ֮ǰ��ʼ���ĶԽ������
     *
     * @return void
     */
    NET_API VOID NET_ReleasePcTalk(HANDLE hPcTalk);

    /* �Խ���ɫ���Ͷ��� */
#define PCTALK_SERVER     1
#define PCTALK_CLIENT     2

    /* PC�Խ���Ϣ���Ͷ��� */
    enum
    {
        PCTALK_REQUEST = 1,         /**< PC�Խ����� */
        PCTALK_ACK,                 /**< ���նԽ����� */
        PCTALK_NAK,                 /**< �ܾ��Խ����� */
        PCTALK_DATA,                /**< ��Ƶ���� */
        PCTALK_STOP,                /**< �����Խ� */
        PCTALK_OFFLINE              /**< �Խ����� */
    };
    /* PC�Խ�Ӧ�����Ͷ��� */
    enum
    {
        PCTALK_DIALOG = 1,       /**< 1��1�Խ� */
        PCTALK_BROADCAST,      /**< 1�Զະ�� */
    };
    /* PC�Խ���Ϣͷ */
    typedef struct
    {
        U32     u32SockFd;                             /**< socket fd */
        U8      u8SubOrBackType;                       /**< �����ͻ�ظ����� */
        S8      s8LocalName[MAX_NAME_LEN];             /**< �������� */
        U8      u8LocalIp[MAX_IPV4_LEN];               /**< ����IP */
        U8      u8RemoteIp[MAX_IPV4_LEN];              /**< �Է�IP */
        U32     u32DataLen;                            /**< �������ݳ��� */
        U32     u32Reserve1;                           /**< �����ֶ� */
    } NET_PCTALK_MASHEAD;
    /**
     * @brief  ���ͶԽ���Ϣ
     *
     * @param [in] hConn    �Ѿ����������Ӿ����NET_InitPcTalkServer��NET_InitPcTalkClient�ķ���ֵ
     * @param [in] u8Type	��Ϣ���ͣ�1-����Խ���2-ACK, 3-NAK, 4-��Ƶ���ݣ�5-�����Խ�
     * @param [in] pData	����
     * @param [in] u32Size  ���ݳ���
     *
     * @return VOID
     */
    NET_API VOID NET_SendPcTalkMsg(HANDLE hPcTalk, U8 u8Type, VOID *pData, U32 u32Size);

    /**
     * @brief  ����ͬ��ʱ��
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] pParam ͬ��ʱ��������ο�NET_SYNTIME_PARAM_S
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_SynchrTime(HANDLE hConn, const NET_SYNTIME_PARAM_S *pParam);

    /**
     * @brief  ��ѯʱ��
     *
     * @param [in] hConn      �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [out] pDateTime ʱ��
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_QueryTime(HANDLE hConn, DATETIME_S *pDateTime);

    /**
     * @brief  ������Ԥ��
     *
     * @param [in] hPView NET_StartPreview����ֵ
     *
     * @see    NET_StartPreview, NET_ConnStart
     */
    NET_API BOOL NET_OpenSound(HANDLE hPView);

    /**
     * @brief  �ر�����Ԥ��
     *
     * @param [in] hPView NET_StartPreview����ֵ
     *
     * @see    NET_StartPreview, NET_ConnStart
     */
    NET_API BOOL NET_CloseSound(HANDLE hPView);


    /**
     * @brief  ��ȡ�豸����״̬
     *
     * @param [in] hConn     �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] u8Ip      �豸IP��ַ
     * @param [in] u16Port   �豸�˿�
     *
     * @see
     */
    NET_API BOOL NET_GetDeviceState(HANDLE hConn, U8 u8Ip[], U16 u16Port);

    /**
     * @brief  ���߿ͻ��б�ص���������
     *
     * @param [out] pRsp   ��Դ��Ϣ
     * @param [out] pUser  �û�����
     *
     */
    typedef VOID (*pfnResourceCallback)(const RESOURCE_RSP_S *pRsp, VOID *pUser);

    /**
     * @brief  ��Դ��ȡ
     *
     * @param  [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param  [in] pUser �û�����
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     *
     * @see
     */
    NET_API BOOL NET_QueryResource(HANDLE hConn, pfnResourceCallback fResourceCallback, VOID *pUser);


    /* �Խ�������������Ƶ�Ự */
    /**
     * @brief �Խ���ʼ��
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] pParam ��ز������ã��ο�NET_DTALK_PARAM_S
     *
     * @return �ɹ�����һ���������ʾһ���ѽ����ĶԽ�ͨ�������Դ��ʧ�ܷ���NULL��
     *
     * @see    NET_UnInitDualtalk, NET_ConnStart
     */
    NET_API HANDLE NET_InitDualtalk(HANDLE hConn, const NET_DTALK_PARAM_S *pParam);

    /**
     * @brief  �Խ�����ʼ����
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] hDTalk  NET_InitDualtalk����ֵ
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     *
     * @see NET_InitDualtalk, NET_ConnStart
     */
    NET_API VOID NET_UnInitDualtalk(HANDLE hConn, HANDLE *hDTalk);

    /**
     * @brief  ���ͶԽ���Ϣ
     *
     * @param [in] hDTalk   �Ѿ����������Ӿ����NET_InitDualtalk�ķ���ֵ
     * @param [in] pParam   �Խ���Ϣ����
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE
     */
    NET_API BOOL NET_SendDTalkMsg(HANDLE hDTalk, const NET_DTALK_MSG_S *pParam, const NET_DTALK_USER_S *pUserList);

    /**
     * @brief  ���ͶԽ�����Ƶ����
     *
     * @param [in] hDTalk   �Ѿ����������Ӿ����NET_InitDualtalk�ķ���ֵ
     * @param [in] pData    g711a���ݣ�����˽��֡ͷ
     * @param [in] u32Size  ���ݳ���,320+40
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE
     */
    NET_API BOOL NET_SendDTalkData(HANDLE hDTalk, U8 *pu8Data, U32 u32Size);

    /**
     * @brief  �ڲ����hMsgWnd����WM_VIDEO_CAPTURE��Ϣ��Ӧ�ý��յ�����Ϣ���Ե��øýӿ�������Ƶ�ɼ�
     *
     * @param [in] hDTalk   �Ѿ����������Ӿ����NET_InitDualtalk�ķ���ֵ
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE
     */
    NET_API BOOL NET_StartCapture(HANDLE hDTalk);

    /**
     * @brief  ����ת��
     *
     * @param [in] hConn        �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] pLinkTrans   ������Ϣ
     *
     * @see
     */
    NET_API BOOL NET_LinkTransmit(HANDLE hConn, LINKAGE_TRANS_RPT_S* pLinkTrans);

    /**
     * @brief  ��ȡ�豸������Ϣ
     *
     * @param [in] pData        �豸��Ϣָ��
     * @param [in] u32DevNum    �豸����
     *
     * @see
     */
    NET_API CHN_INFO_S* NET_GetDeviceData(HANDLE hConn, U32 *u32DevNum);

    /**
     * @brief  ��������Ʋ���
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] u32Cmd ��̨��������,�ο�PTZ_CMD_E
     * @param [in] pparam��̨������ز������ã��ο�NET_PTZ_PARAM_S
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     *
     * @see    NET_ConnStart
     */
    NET_API BOOL NET_RailCarControl(HANDLE hConn, U32 u32Cmd, const NET_PTZ_PARAM_S *pParam);

    /**
     * @brief Զ��������ֹͣDVR��ĳ��ͨ����¼��
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] pparam �����ͨ����Ϣ���ο�NET_CHANNEL_PARAM_S
     * @param [in] bStart 1-����¼��0-ֹͣ¼��
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     *
     * @see    NET_ControlRecord
     */
    NET_API BOOL NET_ControlRecord(HANDLE hConn, const NET_CHANNEL_PARAM_S *pParam, BOOL bStart);

    /**
     * @brief ��������(������ģʽ)
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] pParam ���ò������ݣ��ο�NET_PARAMSET_PARAM_S
     *
     * @return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     *
     * @see    NET_ControlRecord
     */
    NET_API BOOL NET_SetParameter(HANDLE hConn, const NET_PARAMSET_PARAM_S *pParam);


    /**
     * @brief ��������(jsonģʽ)
     *
     * param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * param [in] pParam ���ò�������
     *
     * return �ɹ�����TRUE, ���򷵻�FALSE, ��ȡ���������NET_GetLastError
     */

    NET_API BOOL NET_SetParameterEx(HANDLE hConn, const S8 * pParam);

    /* ����Ƶ�Խ�״̬ */
    enum
    {
        TALK_STATE_NOTHING = 0,   /**< ���� */
        TALK_STATE_WAIT,      /**< ����������У��ȴ����ܻ򱻽��� */
        TALK_STATE_DIALOG,    /**< ���ڶԽ��� */
        TALK_STATE_BROADCAST, /**< ���ں����� */
    };
    /**
     * @brief ���ÿͻ�������Ƶ�Խ�״̬
     *
     * @param [in] hConn         �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] u32DTalkState ����Ƶ�Խ�״̬
     *
     * @return BOOL
     *
     * @see
     */
    NET_API BOOL NET_SetDTalkState(HANDLE hConn, U32 u32DTalkState);

    /**
     * @brief  Ԥ��λ�б���Ϣ�ص���������
     *
     * @param [out] u32PresetNo   Ԥ��λ��
     * @param [out] ps8PresetName Ԥ��λ����
     */
    typedef VOID (*pfnPresetListCallback)(U32 u32PresetNo, const S8 *ps8PresetName, VOID *pUser);
    typedef struct
    {
        S8                    * ps8FrontAddrCode;                    /**< ǰ���豸��ַ����, �ǿ�����õ�ַ�뷽ʽ����ǰ���豸���������õ�IP�Ͷ˿ڲ������� */
        S8                    * ps8DeviceIp;                         /**< �豸IP */
        U16                     u16DevicePort;                       /**< �豸�˿� */
        S8                    * ps8SubServIp;                        /**< ������ý�������IP */
        U16                     u16SubServPort;                      /**< ������ý��������˿� */

        U32                     u32ChnNo;                            /**< ͨ���� */

        VOID                  * pUser;                               /**< �û����� */
        pfnPresetListCallback fnPresetListCallback;                  /**< �豸�б����ݻص� */
    } NET_GETPRESET_PARAM_S;

    /**
     * @brief  ��ȡ�豸ͨ��Ԥ��λ��Ϣ
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] pParam ��ȡ�豸ͨ����Ϣ���ο�NET_GETPRESET_PARAM_S
     *
     * @see
     */
    NET_API BOOL NET_GetChnPreset(HANDLE hConn, const NET_GETPRESET_PARAM_S *pParam);

    typedef struct
    {
        U32                     u32PresetNo;                         /**< Ԥ��λ�� */
        S8                    * ps8PresetName;                       /**< Ԥ������ */
    } NET_PRESET_INFO_S;

    typedef struct
    {
        S8                    * ps8FrontAddrCode;                    /**< ǰ���豸��ַ����, �ǿ�����õ�ַ�뷽ʽ����ǰ���豸���������õ�IP�Ͷ˿ڲ������� */
        S8                    * ps8DeviceIp;                         /**< �豸IP */
        U16                     u16DevicePort;                       /**< �豸�˿� */
        S8                    * ps8SubServIp;                        /**< ������ý�������IP */
        U16                     u16SubServPort;                      /**< ������ý��������˿� */

        U32                     u32ChnNo;                            /**< ͨ���� */
        U32                     u32PresetNum;                        /**< Ԥ��λ���� */
        NET_PRESET_INFO_S     * pstPresetList;                       /**< Ԥ��λ�б� */
    } NET_SETPRESET_PARAM_S;

    /**
     * @brief  �����豸ͨ��Ԥ��λ��Ϣ
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] pParam �����豸ͨ����Ϣ���ο�NET_GETPRESET_PARAM_S
     *
     * @see
     */
    NET_API BOOL NET_SetChnPreset(HANDLE hConn, const NET_SETPRESET_PARAM_S *pParam);

    /**
     * @brief  Ԥ��λ�б���Ϣ�ص���������(�¹�Լ)
     *
     * @param [out] u32PresetNo   Ԥ��λ��
     * @param [out] ps8PresetName Ԥ��λ����
     * @param [out] u32PresetType  Ԥ��λ����
     */
    typedef VOID (*pfnPresetListCallbackEx)(U32 u32PresetNo, const S8 *ps8PresetName, U32 u32PresetType, VOID *pUser);
    typedef struct
    {
        S8                      * ps8FrontAddrCode;                    /**< ǰ���豸��ַ����, �ǿ�����õ�ַ�뷽ʽ����ǰ���豸���������õ�IP�Ͷ˿ڲ������� */
        S8                      * ps8DeviceIp;                         /**< �豸IP */
        U16                       u16DevicePort;                       /**< �豸�˿� */
        S8                      * ps8SubServIp;                        /**< ������ý�������IP */
        U16                       u16SubServPort;                      /**< ������ý��������˿� */

        U32                       u32ChnNo;                            /**< ͨ���� */

        VOID                    * pUser;                               /**< �û����� */
        pfnPresetListCallbackEx  fnPresetListCallback;                  /**< �豸�б����ݻص� */
    } NET_GETPRESET_PARAM_S_EX;

    /**
     * @brief  ��ȡ�豸ͨ��Ԥ��λ��Ϣ(�¹�Լ)
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] pParam ��ȡ�豸ͨ����Ϣ���ο�NET_GETPRESET_PARAM_S
     *
     * @see
     */
    NET_API BOOL NET_GetChnPresetEx(HANDLE hConn, const NET_GETPRESET_PARAM_S_EX *pParam);

    typedef struct
    {
        U32                     u32PresetNo;                         /**< Ԥ��λ�� */
        S8                    * ps8PresetName;                       /**< Ԥ������ */
        U32                     u32PresetType;                       /**< Ԥ��λ���� */
    } NET_PRESET_INFO_S_EX;

    typedef struct
    {
        S8                    * ps8FrontAddrCode;                    /**< ǰ���豸��ַ����, �ǿ�����õ�ַ�뷽ʽ����ǰ���豸���������õ�IP�Ͷ˿ڲ������� */
        S8                    * ps8DeviceIp;                         /**< �豸IP */
        U16                     u16DevicePort;                       /**< �豸�˿� */
        S8                    * ps8SubServIp;                        /**< ������ý�������IP */
        U16                     u16SubServPort;                      /**< ������ý��������˿� */

        U32                     u32ChnNo;                            /**< ͨ���� */
        U32                     u32PresetNum;                        /**< Ԥ��λ���� */
        NET_PRESET_INFO_S_EX  * pstPresetList;                       /**< Ԥ��λ�б� */
    } NET_SETPRESET_PARAM_S_EX;

    /**
     * @brief  �����豸ͨ��Ԥ��λ��Ϣ(�¹�Լ)
     *
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] pParam �����豸ͨ����Ϣ���ο�NET_GETPRESET_PARAM_S
     *
     * @see
     */
    NET_API BOOL NET_SetChnPresetEx(HANDLE hConn, const NET_SETPRESET_PARAM_S_EX *pParam);

#ifdef _WIN32
    /**
     * @brief  �����ַ�����Ƶ������
     * @param [in] hConn  �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param [in] pParam  �ο�NET_PRESET_DRAW
     */

    typedef struct
    {
        HANDLE *hPView;         /* ���ž��*/
        S8     *ps8PresetName;  /* ����Ԥ��λ�ַ� */
        U8      u8WndNum;       /* ��ǰ������Ŀ */
        U16     u16Top;         /* �ַ���ʾλ��top */
        U16     u16Left;        /* �ַ���ʾλ��left */
    } NET_PRESET_DRAW;

    NET_API VOID NET_DrawPreset(HANDLE hConn, const NET_PRESET_DRAW * pParam);

    /**
     * @brief ���sdk��־
     * @brief dir ���Ŀ¼
     */

    NET_API BOOL NET_SetLogToFile();

    /**
     * @brief ע���ͼ����
     * param [in] hConn �Ѿ��������ӵľ����NET_ConnStart����ֵ
     * param [in] NET_REGDRAW_PARAM ע���ͼ����
     * return ����ֵ���ɹ�TRUE��ʧ��FALSE
     */

    typedef VOID (*pfnDrawFun)(HDC hDC, S32 s32Height, S32 s32Width, VOID * pUser);

    typedef struct
    {
        HANDLE *hPView;         /* ���ž�� */
        pfnDrawFun pfnDraw;     /* ��ͼ���� */
        HANDLE     hUser;       /* �û����� */
    } NET_REGDRAW_PARAM;

    NET_API BOOL NET_RegisterDraw(HANDLE hConn, const NET_REGDRAW_PARAM * pParam);

    /**
     * @brief �������ݻص�����
     * @param [in] hConn �Ѿ��������ӵľ����NET_ConnStart����ֵ
     *
     */

    typedef struct
    {
        U8   u8Type;      /* ��ɫ���� */
        S32  s32Width;
        S32  s32Height;
        S8   *ps8Data;
        U32   u32Len;
    } DEC_FRAME_INFO;

    typedef VOID (*pfnDecDataCallback)(const DEC_FRAME_INFO *pDecFrameInfo, VOID * pUser);

    typedef struct
    {
        HANDLE *hPView;              /* ���ž�� */
        pfnDecDataCallback pfnDecCb; /* �������ݻص� */
        HANDLE   hUser;              /* �û����� */
    } NET_DECFRAME_PARAM;

    NET_API BOOL NET_SetDecDataCallback(HANDLE hConn, const NET_DECFRAME_PARAM * pParam);
#endif

    /**
     * @brief ��ȡsdk�汾��Ϣ
     * @param DWORD ����ֵ ret<0 ʧ��
     * @param SDK�汾��Ϣ��2�����ֽڱ�ʾ���汾��2�����ֽڱ�ʾ�ΰ汾����0x01000007����ʾ�汾Ϊ1.0.0.7��
     */

    NET_API DWORD NET_GetSDKVersion();

    /**
     * @brief ��ȡ¼��ط��Ѳ���ʱ��
     * @param DWORD ����ֵ ¼���ѻط�ʱ��
     * @return param hPlayback NET_StartRecordPlay�ķ���ֵ
     */

    NET_API DWORD NET_GetPlayedTime(HANDLE hPlayback);

    /**
     * @brief ����¼��ط�״̬�ص�����
     * @param BOOL �ɹ�����TRUE,ʧ�ܷ���FALSE
     * @param param fnRecordPbStatusCb ���ûص�����
     * @param pUser �û�����
     */
    NET_API BOOL NET_SetRecordPlaybackStatusCallback(HANDLE hPlayback, pfnRecordPlaybackStatusCallback fnRecordPbStatusCb, VOID *pUser);


    /** < ���ܷ��� */
    typedef struct _NET_IVA_ANALYSE_PARAM_
    {
        S8   * ps8IvaIp;        /* ���ܷ���������ip */
        U16    u16IvaPort;      /* ���ܷ���������port */
        S8   * ps8DevIp;        /* �豸ip */
        U16    u16DevPort;      /* �豸port */
        U8     u8DevChn;        /* �豸ͨ��*/
        U8     u8DevPreset;     /* �豸Ԥ��λ*/
    } NET_IVA_ANALYSE_PARAM;

    /**
     * @brief �������ܷ���(����ʱps8IvaIpΪ"0.0.0.0")
     * @param hConn �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param pParam ���ܷ�������
     * @return ����ֵ���ɹ�TRUE��ʧ��FALSE
     */

    NET_API BOOL NET_StartIvaAnalyse(HANDLE hConn, const NET_IVA_ANALYSE_PARAM *pParam);

    /**
     * @brief ֹͣ���ܷ���(����ʱps8IvaIpΪ"0.0.0.0")
     * @param hConn �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param pParam ���ܷ�������
     * @return ����ֵ���ɹ�TRUE��ʧ��FALSE
     */

    NET_API BOOL NET_StopIvaAnalyse(HANDLE hConn, const NET_IVA_ANALYSE_PARAM *pParam);

    #ifndef _MUX_CALL_TYPE_
    #define _MUX_CALL_TYPE_
    enum MUX_CALL_TYPE{
        MUX_CALL_START,     //��ʼ¼��
        MUX_CALL_END,       //����¼��
    };
    #endif //__MUX_AVFILE_H__

    /**
    * @brief �豸�Խ�����װ����Ƶ�ļ��ص�
    * @param strFilename���ļ�������·����
    * @param type���ص�����
    * @param pUser���û�����
    */
    typedef void(*muxavfDevTalk_callback)(char *strFilename, MUX_CALL_TYPE type, void* pUser);

    /** < �豸�Խ�����װ����Ƶ�ļ� */
    typedef struct 
    {
        S8   * strFilePath;          /* �ļ�·�� ����"E:/"��trFilepath����ΪNULL�򳤶ȳ���1024������Ĭ��Ϊ��./�� */
        U32    nSubpackageTime;      /* �ְ�ʱ����min������С��5min������5min���㣻�ְ����������򣺡�test-1.avi����...��test-n.avi����n��ʾ����test.avi����ĵ�n���ְ� */
        VOID   * pUser;              /* �û����� */
        muxavfDevTalk_callback    callback;      /* �ļ��������رջص� */
    } _NET_DEV_MUXAVF_PARAM_;
    /**
     * @brief ��ʼ���豸�Խ�
     *
     * @param hConn      �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param audioType  ������ʽ���� 0-pcm 1-g711A 2-g711U 3-hkg722
     *
     * @return HANDLE ����ֵ���ɹ��豸�Խ������ʧ��NULL
     */

    NET_API HANDLE NET_InitDevTalk(HANDLE hConn, int audioType = 2);

    /**
     * @brief �ͷ��豸�Խ�
     * @param hDevTalk �豸�Խ����ָ��
     */

    NET_API VOID NET_UninitDevTalk(HANDLE *hDevTalk);

    /**
     * @brief �����豸�Խ�
     * @param hDevTalk �豸�Խ����
     * @param pParam �豸�����б�
     * @return ����ֵ���ɹ�TRUE��ʧ��FALSE
     */

    NET_API BOOL NET_StartDevTalk(HANDLE hDevTalk, const NET_DTALK_DEV_LIST *pParam);

    /**
     * @brief ֹͣ�豸�Խ�
     * @param hDevTalk �豸�Խ����
     * @param pParam �豸�����б�
     * @return ��
     */

    NET_API VOID NET_StopDevTalk(HANDLE hDevTalk, const NET_DTALK_DEV_LIST *pParam);


	/**
     * @brief ������Ƶ����
     * @param hDevTalk �豸�Խ����
     * @param pParam ps8Data
     * @return ��
     */
	NET_API VOID NET_SendDataToDevTalk(HANDLE *hDevTalk, U8 * ps8Data, U16 u16Len);
    
	/**
     * @brief ��ʼ���豸�Խ���װ�ļ���������������װ�ļ�
     * @param hDevTalk �豸�Խ������NET_InitDevTalk���ؾ��
     * @param pParam ��װ�ļ�����
     * @return �ɹ���TRUE��ʧ�ܣ�FALSE
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
        S8    * ps8DevIp;       /* �豸ip */
        U16     u16DevPort;     /* �豸port */
        U8      u8ChnNo;        /* ͨ���� */
        U8      u8PresetNo;     /* Ԥ��λ�� */
        U8      u8Enable;       /* �Ƿ�����*/
        S8    * ps8RuleName;    /* �������� */
        U8      u8RuleId;       /* ����id */
        float   fEmissivity;    /* ������ */
        float   fDistance;      /* ���� */
        float   fReflexValue;   /* �����¶�*/
        NET_AI_THERM_POS stFrom;
        NET_AI_THERM_POS stTo;
    } NET_THERM_RULE_INFO;

    /* ����Ԥ��λ���ò��� */
    typedef struct
    {
        U32                   u32RuleListNum;
        NET_THERM_RULE_INFO * pThermRuleList;
    } NET_THERM_RULE_LIST;

    /**
     * @brief ���ú������Ԥ��λ�¹������
     * @param hConn �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param pParam ��������б�
     * @return ����ֵ���ɹ�TRUE��ʧ��FALSE
     */

    NET_API BOOL NET_SetThermometryRuleCfg(HANDLE hConn, const NET_THERM_RULE_LIST* pParam);

    /* ������ʾģʽ */
    enum ThermDispalyMode
    {
        Therm_Show_WhiteHot = 0,    /* ���� */
        Therm_Show_RainBow  = 10,   /* �ʺ� */
        Therm_Show_IronRed  = 12,   /* ���� */
        Therm_Show_GreenHot = 20,   /* ���� */
    };

    /* �л������ȳ���ģʽ */
    typedef struct
    {
        S8             * ps8DevIp;
        U16              u16DevPort;
        U8               u8ChnNo;
        ThermDispalyMode mode;
    } NET_TERM_SHOWPARAM_S;

    /**
     * @brief ���ú����������ʾģʽ(���졢�ʺ��)
     * @param hConn �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param showMode ��ʾģʽ
     * @return ����ֵ���ɹ�TRUE��ʧ��FALSE
     */

    NET_API BOOL NET_SetThermDisplayMode(HANDLE hConn, const NET_TERM_SHOWPARAM_S * pParam);

    /**
     * @brief ���û�ɾ����ý����Ԥ��λ��Ϣ
     * @param hConn �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param ps8PresetInfo ����Ԥ��λ��Ϣjson��ʽ
     * @param us32Size �����ֽڴ�С
     * @return ����ֵ���ɹ�TRUE��ʧ��FALSE
     */
    NET_API BOOL NET_SendPresetInfo(HANDLE hConn, const S8 * ps8PresetInfo, U32 us32Size);

    /**
     * @brief ��ȡ��ý����Ԥ��λ��Ϣ
     * @param hConn �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param ps8PresetReq ������Ϣ(ȫ�����ߵ���)
     * @param u32Size ������Ϣ�ֽ���
     * @param ps8PresetInfo ����Ԥ��λ��Ϣjson��ʽ
     * @return ����ֵ���ɹ�TRUE��ʧ��FALSE
     */
    NET_API BOOL NET_GetPresetInfo(HANDLE hConn, const S8 * ps8PresetReq, U32 u32Size, S8 ** ps8PresetInfo);

    /**
     * @brief �·���ʢ��(AXF|EST|DV|LINE)
     * @param hConn �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param ps8Resource һ��ͨ��Դ��Ϣjson��ʽ
     * @return ����ֵ���ɹ�TRUE��ʧ��FALSE
     */
    NET_API BOOL NET_SendHSResource(HANDLE hConn, const S8 * ps8Resource, S32 s32Size);

    /**
     * @brief �·������ý��(json��ʽ)
     * @param hConn �Ѿ��������ӵľ��,NET_ConnStart����ֵ
     * @param ps8Info ��������
     * @param s32Size ���ݴ�С
     * @return ����ֵ���ɹ�TRUE��ʧ��FALSE
     */
    NET_API BOOL NET_SendCommand(HANDLE hConn, const S8 * ps8Info, S32 s32Size);
	
	/**
	* @brief ��ȡ���ܷ������ٸ澯
	* @param hConn �Ѿ��������ӵľ��,NET_ConnStart����ֵ
	* @param ps8Req ����
	* @@param u32Size �����С
	* @ps8AlarmInfo ����ֵ
	* @return ����ֵ���ɹ�TRUE��ʧ��FALSE
	*/
	NET_API BOOL NET_GetRemoteIvaAlarmData(HANDLE hConn, const S8 * ps8Req, U32 u32Size, S8 ** ps8AlarmInfo);
	
	
	/**
	* @brief ��ȡ���ܷ�����Ѳ�б�
	* @param hConn �Ѿ��������ӵľ��,NET_ConnStart����ֵ
	* @param ps8Req ����
	* @@param u32Size �����С
	* @@param ps8IvaCruiseList ����ֵ
	* @return ����ֵ���ɹ�TRUE��ʧ��FALSE
	*/
	NET_API BOOL NET_GetIvaCruiseList(HANDLE hConn, const S8 * ps8Req, U32 u32Size, S8 ** ps8IvaCruiseList);

	/**
	* @brief �������ܷ�����Ѳ��ͣ
	* @param hConn �Ѿ��������ӵľ��,NET_ConnStart����ֵ
	* @param ps8Req ����
	* @u32Size �����С
	* @ps8AlarmInfo ����ֵ
	* @return ����ֵ���ɹ�TRUE��ʧ��FALSE
	*/
	NET_API BOOL NET_ControlIvaCruise(HANDLE hConn, const S8 * ps8Req, U32 u32Size);

	/**
	* @brief ��ѯ���ܷ��������б�
	* @param hConn �Ѿ��������ӵľ��,NET_ConnStart����ֵ
	* @param ps8Req ����
	* @u32Size �����С
	* ps8Result ����ֵ
	* @return ����ֵ���ɹ�TRUE��ʧ��FALSE
	*/
	NET_API BOOL NET_QueryIvaRunList(HANDLE hConn, const S8 * ps8Req, U32 u32Size, S8 ** ps8Result);


	/**
	* @brief ��ѯӲ��¼���ͨ����¼��״̬
	* @param hConn �Ѿ��������ӵľ��,NET_ConnStart����ֵ
	* @param ps8Req ����
	* @u32Size �����С
	* ps8ChanRecordState ����ֵ
	* @return ����ֵ���ɹ�TRUE��ʧ��FALSE
	*/
	NET_API BOOL NET_QueryChanRecordState(HANDLE hConn, const S8 * ps8Req, U32 u32Size, S8 ** ps8ChanRecordState);
	
	/**
	* @brief ���������ȡ���ؽ��
	* @param hConn �Ѿ��������ӵľ��,NET_ConnStart����ֵ
	* @param ps8Req ����,eg:{"ChannelNo":33,"DevIp":"192.168.80.235","DevPort":8000,"Method":"GetChannelStatus","version":"v1.00"}
	* @u32Size �����С
	* ps8Rsp ���ջ���
	* u8RspSize ������ջ����С������ʵ�����ݴ�С
	* @return ����ֵ���ɹ�TRUE��ʧ��FALSE
	*/
	NET_API BOOL NET_GetResponse(HANDLE hConn, const S8 * ps8Req, U32 u32Size, S8 *ps8Rsp,U32 *u8RspSize);

#ifdef __cplusplus
}
#endif
#endif
