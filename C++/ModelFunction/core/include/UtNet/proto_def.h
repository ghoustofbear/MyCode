/**
 * (C) Copyright 2012, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Thu Aug  9 10:46:20 CST 2012
 *
 * protocol define.
 */

#ifndef __PROTO_DEF_H__
#define __PROTO_DEF_H__
#include "defc1.h"

/* MSG_HEADER_S.u2Version */
#define MsgHeadV1   0 
#define MsgHeadV2   2

/* MSG_HEADER_S.u16AckFlag */
#define ACK_SUCCESS               0         /**< �ɹ� */
#define ACK_FAIL                  1         /**< ʧ�� */
#define ACK_NO_DEFINE             2         /**< ��Ϣδ���� */

#define ACK_LOGIN_HAS_LOGINED     10        /**< �Ѿ���¼ */
#define ACK_LOGIN_NO_USERNAME     11        /**< �û��������� */
#define ACK_LOGIN_AUTH_FAIL       12        /**< ��֤ʧ�� */
#define ACK_LOGIN_PERMDENY        13        /**< û��Ȩ�� */
#define ACK_LOGIN_AAA_AUTH_FAIL   14        /**< AAA��֤ʧ�� */

#define ACK_OPT_PERMDENY          21        /**< û�в���Ȩ�� */
#define ACK_OPT_DOING             22        /**< �����ȼ������ڲ��� */
#define ACK_OPT_INVALID           23        /**< �Ƿ����� */
#define ACK_OPT_NO_DEVICE         24        /**< �豸������ */

#define ACK_DTALK_OK              31        /**< �Խ�����ɹ� */
#define ACK_DTALK_NAK             32        /**< �Խ�����ܾ� */
#define ACK_DTALK_NO_ACK          33        /**< �Խ�����û��Ӧ */

/* MSG_HEADER_S.u16MsgType*/
#define MSG_SEARCH_DEV            11 

#define MSG_LOGIN                 21        /**< ��¼ */
#define MSG_HEARTBEAT             22        /**< ���� */

#define MSG_TCP_PREVIEW           31
#define MSG_GET_UDPADDR           32
#define MSG_UDP_PREVIEW_START     33
#define MSG_UDP_PREVIEW_STOP      34

#define MSG_QUERY_RECORD          41
#define MSG_PLAYBACK              42
#define MSG_PLAY_FEEDBACK         43
#define MSG_PLAY_REPOS            44
#define MSG_DOWNLOAD              45
#define MSG_QUERY_RECORD_EX       46

#define MSG_PTZ_CONTROL           51
#define MSG_REQ_IFRAME            52 
#define MSG_QUERY_PARAM           53
#define MSG_SET_PARAM             54
#define MSG_ALARM_SUBSCRIBE       55
#define MSG_ALARM_REPORT          56
#define MSG_QUERY_LOG             57
#define MSG_REMOTE_CAPTURE        58
#define MSG_PVIEW_SWITCH          59
#define MSG_MATRIX_SWITCH         61 
#define MSG_DEC_SETDISPLAY        62 
#define MSG_DEC_START_DYNAMIC     63 
#define MSG_GET_CLIENT_LIST       64
#define MSG_SYNCHR_TIME           65
#define MSG_QUERY_TIME            66
#define MSG_QUERY_RESOURCE        67
#define MSG_CLIENT_LINKAGE        68
#define MSG_CAR_CONTROL           69
#define MSG_CONTROL_RECORD        70

#define MSG_DUALTALK              71 

#define MSG_PC_TALK               72

#define MSG_SET_DTALK_STATE       73

#define MSG_GET_PRESET            74
#define MSG_SET_PRESET            75

#define MSG_PTZ_CONTROL_EX        76
#define MSG_DEC_STOP_DYNAMIC      77

#define MSG_START_IVAANALYSE      78
#define MSG_STOP_IVAANALYSE       79

#define MSG_GET_PRESET_EX         80
#define MSG_SET_PRESET_EX         81

#define MSG_DEVTALK               82
#define MSG_DEVTALK_START         83
#define MSG_DEVTALK_STOP          84
#define MSG_DTALK_EX_DATA         85

#define MSG_SET_TERM_MODE         86
#define MSG_SET_TERM_RULE         87

#define MSG_SET_PARAM_EX          88

#define MSG_GET_PRESET_JSON       89
#define MSG_SET_HS_RESOURCE       90
#define MSG_DO_PRESET_JSON        91
#define MSG_DO_SEND_COMMAND       92
#define MSG_CALL_IVAALARM         93
#define MSG_GET_IVACRUISELIST     94
#define MSG_CONTROL_IVACRUISE     95
#define MSG_QUERY_IVALIST         96

/* MSG_HEADER_S.u16MsgSubType*/
#define MSG_NO_SUBTYPE    0

/* MSG_PTZ_CONTROL sub type*/
#define MSG_PTZ_UP                5101
#define MSG_PTZ_DOWN              5102
#define MSG_PTZ_LEFT              5103
#define MSG_PTZ_RIGHT             5104
#define MSG_PTZ_UPLEFT            5105
#define MSG_PTZ_UPRIGHT           5106
#define MSG_PTZ_DOWNLEFT          5107
#define MSG_PTZ_DOWNRIGHT         5108
#define MSG_PTZ_ZOOMIN            5109         /**< ������ */ 
#define MSG_PTZ_ZOOMOUT           5110         /**< �����С */
#define MSG_PTZ_FOCUSNEAR         5111         /**< ����ǰ�� */
#define MSG_PTZ_FOCUSFAR          5112         /**< ������ */
#define MSG_PTZ_IRISOPEN          5113         /**< ��Ȧ���� */
#define MSG_PTZ_IRISCLOSE         5114         /**< ��Ȧ��С */
#define MSG_PTZ_AUTOSCAN          5115
#define MSG_PTZ_CRUISE            5116
#define MSG_PTZ_SETPRESET         5117
#define MSG_PTZ_CLEARPRESET       5118
#define MSG_PTZ_CALLPRESET        5119 
#define MSG_PTZ_RAINON            5120
#define MSG_PTZ_RAINOFF           5121

/* MSG_QUERY_RECORD sub type */
#define MSG_RECORD_DETAIL         4101
#define MSG_RECORD_CALENDAR       4102
#define MSG_RECORD_DAYHALFHOUR    4103

/* MSG_PLAY_FEEDBACK sub type */
#define MSG_PLAY_INSUFFICIENT     4201
#define MSG_PLAY_ENOUGH           4202
#define MSG_PLAY_FAST             4203
#define MSG_PLAY_SLOW             4204

/* MSG_ALARM_REPORT sub type */
#define MSG_ALARM_IO              5601 
#define MSG_ALARM_MD              5602 
#define MSG_ALARM_VLOSS           5603 
#define MSG_ALARM_HDD             5604 
#define MSG_ALARM_AI              5605 
#define MSG_ALARM_DEV_STATE       5606 
#define MSG_ALARM_WFLINGAGE       5607
#define MSG_ALARM_LINK_TRANS      5608
#define MSG_ALARM_SYSINFO         5609
#define MSG_ALARM_MANUAL          5610
#define MSG_ALARM_GPS             5611
#define MSG_ALARM_ACCESS          5612
#define MSG_ALARM_CALL            5613
#define MSG_ALARM_PTZ             5614
#define MSG_ALARM_IVASTATE        5615
#define MSG_ALARM_CAMSTATE        5616

/* MSG_QUERY_PARAM sub type */
#define MSG_PARAM_DV              5307
#define MSG_PARAM_USERLIST        5308
#define MSG_PARAM_DEVLIST         5309
#define MSG_PARAM_TOUR            5310
#define MSG_PARAM_DV_EX           5311
#define MSG_PARAM_DV_EX0815       5312
            
/* MSG_DUALTALK sub type */
#define MSG_DTALK_HBEAT           7101
#define	MSG_DTALK_REQ             7102
#define	MSG_DTALK_STOP            7103
#define	MSG_DTALK_DATA            7104           
#define	MSG_DTALK_EXCEP           7105

/* MSG_PC_TALK sub type */
enum
{
	MSG_PCTALK_REQUEST = 5401,
	MSG_PCTALK_ACK,
	MSG_PCTALK_NAK,
	MSG_PCTALK_AUDIO,
	MSG_PCTALK_STOP,
	MSG_PCTALK_OFFLINE
};

typedef struct _MSG_HEADER_S_
{
	U8  u2Version : 2;  /**< ��Ϣͷ�汾 */
	U8  u6ExtSize : 6;  /**< ��Ϣͷ��չ��С��ʵ����չ��С = ExtSize * 4 */
	U8  u8Reserved;
	U16 u16AckFlag;     /**< ��Ϣ��Ӧ�� */
	U16 u16MsgType;     /**< ��Ϣ���� */
	U16 u16MsgSubType;  /**< ��Ϣ������ */
	U32 u32MsgSize;     /**< ��Ϣͷ��������ݴ�С */
	U32 u32Seq;         /**< ��Ϣ���кţ���ʶһ����Ϣ��Ψһ�ԣ������󷽴�������Ӧ��ԭֵ���� */

	bool operator==(const _MSG_HEADER_S_ &head) const 
	{  
		return u2Version     == head.u2Version
			&& u16MsgType    == head.u16MsgType 
			&& u16MsgSubType == head.u16MsgSubType
			&& u32Seq        == head.u32Seq;
	}
}MSG_HEADER_S;          /**< ��Ϣͷ��sizeof(MsgHead) = 16 */

/**
 * ��¼
 * ���� MsgType=MsgLogin, MsgSize=sizeof(LoginReq)
 */

typedef struct
{
	S8   s8Username[MAX_NAME_LEN];
	S8   s8Password[MAX_NAME_LEN];
	S8   s8Name[MAX_NAME_LEN];             /**< �ͻ������� */
	U8   u8ClientIp[MAX_IPV4_LEN];         /**< �ͻ���IP */
	U16  u16ClientTalkPort;                /**< �ͻ��˿��ŵĶԽ��˿�*/
	BOOL bAlarmDesired;
	U8   u8Reserved[5];
} LOGIN_REQ_S;

/* �û�Ȩ�޸�λ��ʾ */
typedef struct
{
	U32 u32MngRightBit;      /**< ����Ȩ��λ, 0-�������� 1-վ����� 2-�豸���� 3-Ȩ������� 4-�û�����
							   5-�豸״̬ 6-�û�״̬ 7-ϵͳ״̬ 8-ϵͳ��־ 9-������־
							   10-�豸��־ 11-�û���־ */
	U32 u32OptRightBit;      /**< ����Ȩ��λ, 0-Ԥ�� 1-�ط� 2-���� 3-��̨ 4-���� */
	U32 u32PVChannel;        /**< Ԥ��ͨ�� */
	U32 u32PBChannel;        /**< �ط�ͨ�� */
	U32 u32PtzChannel;       /**< ��̨����ͨ�� */
	U32 u32AlarmChannel;     /**< �����ϱ�ͨ�� */
	U8  u8Res1[8];           
	U8  u8MngLevel;          /**< ����Ȩ�޵ȼ� */
	U8  u8OptLevel;          /**< ����Ȩ�޵ȼ� */
	U8  u8Res2[6];
} USER_RIGHT_S;
C_ASSERT(sizeof(USER_RIGHT_S) == 40);

typedef struct _LOGIN_RSP_S_
{
	S8  s8HWVerison[MAX_NAME_LEN];    /**< Ӳ���汾 */
	S8  s8IEVerison[MAX_NAME_LEN];    /**< IE�汾 */
	S8  s8SWVersion[MAX_NAME_LEN];    /**< ����汾 */
	S8  s8SystemName[MAX_NAME_LEN];   /**< ϵͳ����, �����޸� */
	S8  s8AddrCode[MAX_ADDR_LEN];     /**< ��ַ���� */
	S8  s8MacAddr[MAX_NAME_LEN];      /**< �豸MAC��ַ */

	U32 u32UserKey;                   /**< ���ӷ�����û���Կ */
	USER_RIGHT_S stUserRight;         /**< �û�Ȩ�� */

	U32  u32DeviceNum;                /**< ͨ���� */
	U32  u32ChnInfoSize;

	U8  u8Reserved2[20];
}LOGIN_RSP_S;
C_ASSERT(sizeof(LOGIN_RSP_S) == 276);


typedef struct
{
	S8         s8DevName[MAX_NAME_LEN];   /**< �豸���� */
	S8         s8AddrCode[MAX_ADDR_LEN];  /**< �豸��ַ���� */
	U8         u8IPAddr[4];

	U16        u16Port;
	U16        u16DevType;                /**< �豸���� */ 
	U8         u8ChannelNum;              /**< ͨ���� */

	U8         u8ManuType;                /**< �豸�������� */ 
	U8         u8AlarmInNum;              /**< ���������� */ 
	U8         u8AlarmOutNum;             /**< ���������(�̵�������) */
	U8         u8IsOnline;                /**< �豸�Ƿ����� */
	U8         u8Reserved[3];
}CHN_INFO_S;


/**< �û��б�����Ϣ */
typedef struct
{
	U32 u32StSize;                   /**< sizeof(USERINFO_RSP_S) */
	S8  s8Username[MAX_NAME_LEN];    /**< �û��� */
	S8  s8Password[MAX_NAME_LEN];    /**< ���� */
	USER_RIGHT_S stUserRight;        /**< �û�Ȩ�� */
}USERINFO_RSP_S;

typedef struct
{
	S8  s8AddrCode[MAX_ADDR_LEN];    /**< �豸��ַ���� */
	S8  s8DeviceName[MAX_NAME_LEN];  /**< �豸����, �����޸� */

	U8  u8DeviceIp[MAX_IPV4_LEN];    /**< �豸IP */

	U16 u16DevicePort;               /**< �豸�˿� */
}DEVINFO_REQ_S;

/* �豸�б�����Ϣ */
typedef struct
{
	U32 u32StSize;                    /**< sizeof(DEVINFO_RSP_S) */
	
	S8  s8AddrCode[MAX_ADDR_LEN];     /**< �豸���� */
	S8  s8DevName[MAX_NAME_LEN];      /**< �豸��ַ���� */
	S8  s8PareAddrCode[MAX_ADDR_LEN]; /**< ���豸��ַ���� */
	S8  s8DevDesc[100];               /**< �豸���� */
	
	S32 s32ChnNum;                    /**< �豸ͨ���� */
	S32 s32ChnNo;                     /**< �豸ͨ���� */
	U8  u8IPAddr[4];                  /**< �豸IP��ַ */
	
	U16 u16Port;                      /**< �豸�˿ں� */
	U16 u16DevType;                   /**< �豸���� */
	
	S8  s8ManuType;                   /**< �豸�������� */
	S8  s8ProtoType;                  /**< Э������ */
	S8  s8StreamType;                 /**< �������� */ 
} DEVINFO_RSP_S;

/**
 *  ��
 */

/* PACK_HEADER_S.u2Version */
#define PackHeadV1      0
#define PackHeadV2      1

#define MaxPackSize         (8*1024)
#define MaxPackSize_2       (8*1024)
#define MaxPackSizeV1       (8*1024)

/* PACK_HEADER_S.u8Type */
#define FramePack               0 /**< ֡��ֵİ� */    
#define ReposPack               1 /**< �ط�ʱ�ض�λ��������������͵����ض�λ������� */
#define NoDataPack              2 /**< �ط�ʱ��ʾ��ǰ�ط�ʱ�������Ѿ�û��¼������ */
#define FileEndPack             3 /**< ¼������ʱ��ʾһ���ļ�������� */

typedef struct _PACK_HEADER_S_
{
	U8  u2Version : 2;  /**< ��ͷ�汾 */    
	U8  u6ExtSize : 6;  /**< ��ͷ��չ��С��ʵ����չ��С = ExtSize * 4 */
	U8  u8Type;                                                     
	U16 u16PackNum;                                                 
	U32 u32FrameNo;                                                 
	U16 u16PackNo;      /**< 0,1,2,3... */
	U16 u16PackSize;                                                
} PACK_HEADER_S;        /**< ��ͷ, sizeof(PackHead) = 12 */

typedef struct _PACKAGE_S_
{  
	PACK_HEADER_S stHead;
	S8            s8Data[MaxPackSize];
}PACKAGE_S;

/**
 *  Ԥ��
 */

/* PVIEW_PARAM_S.u8Type */
#define PreviewMain	0
#define PreviewSub	1

/* PVIEW_PARAM_S.u8Content */
#define PreviewComposite	0
#define PreviewVideoOnly	1
typedef struct _PVIEW_PARAM_S_
{
	U8  u8Type;                               /**< 0-������, 1-������ */
	U8  u8Content;                            /**< ����������Ƶ�� */
	U8  u8ReqType;                            /**< ����ʽ��0-IP+�˿ڷ�ʽ 1-��ַ�뷽ʽ */
	U8  u8Reserved[1];

	union
	{
		S8  s8FrontAddrCode[MAX_ADDR_LEN];    /**< ǰ���豸��ַ���� */
		struct
		{
			U32 u32ChnNO;                     /**< ͨ���� 0,1,2,3... */
			U8  u8DeviceIp[MAX_IPV4_LEN];     /**< �豸IP��ַ */
			U16 u16DevicePort;                /**< �豸�˿ں� */

			U8  u8SubServIp[MAX_IPV4_LEN];    /**< ������ý���������ַ */
			U16 u16SubServPort;               /**< ������ý��˿ں� */
		};
	};
}PVIEW_PARAM_S;


/**
 *  TCPԤ��
 */
typedef struct _TCP_PVIEW_REQ_S_
{
	PVIEW_PARAM_S stParam;
	U32 u32UserKey;
}TCP_PVIEW_REQ_S;


typedef struct _PVIEW_RSP_S_
{
	CHN_INFO_S stChnInfo;
}PVIEW_RSP_S;

/**
 *  UDP̽��
 *  ���� MsgType=MsgGetUdpAddr
 *  ��Ӧ MsgSize=size(GetUdpAddrRsp)
 */
typedef struct _UDPADDR_RSP_S_
{
	SA_IN stCliAddr;
	S32   s32SockFd;
	U8    u8Addr[44];
} UDPADDR_RSP_S;

/**
 *  UDP��ʼԤ��
 */
typedef struct _UDP_START_PVIEW_REQ_S_
{
	UDPADDR_RSP_S stUdpRsp;
	PVIEW_PARAM_S stParam;
}UDP_START_PVIEW_REQ_S;

typedef struct _UDP_START_PVIEW_RSP_S_
{
	U32 u32UdpPViewKey;
}UDP_START_PVIEW_RSP_S;

/**
 *  UDPֹͣԤ��
 */

typedef struct _UDP_STOP_PVIEW_REQ_S_
{
	U32 u32UdpPViewKey;
}UDP_STOP_PVIEW_REQ_S;


/**
 *  ����I֡
 */
typedef struct _IFRAME_REQ_S
{
	U8  u8Type;                               /**< ��������,ͬԤ�� */
	U8  u8Reserved[2];

	U8  u8ReqType;                            /**< ����ʽ��0-IP+�˿ڷ�ʽ 1-��ַ�뷽ʽ */
	union
	{
		S8  s8FrontAddrCode[MAX_ADDR_LEN];    /**< ǰ���豸��ַ���� */
		struct
		{
			U32 u32ChnNO;                     /**< ͨ���� 0,1,2,3... */
			U8  u8DeviceIp[MAX_IPV4_LEN];     /**< �豸IP��ַ */
			U16 u16DevicePort;                /**< �豸�˿ں� */

			U8  u8SubServIp[MAX_IPV4_LEN];    /**< ������ý���������ַ */
			U16 u16SubServPort;               /**< ������ý��˿ں� */
		};
	};
} IFRAME_REQ_S;

/**
 *  Ԥ�������л� 
 */
typedef struct
{
	U32 u32Mode;                              /**< NET_SWITCH_MODEֵ */
	U32 u32Channel;                           /**< Ŀ���л�ͨ��ֵ, һλ��Ӧһ��ͨ�� */

	U8  u8DeviceIp[MAX_IPV4_LEN];             /**< �豸IP��ַ */
	U16 u16DevicePort;                        /**< �豸�˿ں� */

	U8  u8SubServIp[MAX_IPV4_LEN];            /**< ������ý���������ַ */
	U16 u16SubServPort;                       /**< ������ý��˿ں� */
}SWITCH_REQ_S;

/**
 *  �����л� 
 */
typedef struct
{
	U32 u32InPort;                            /**< ��������˿ں� */
	U32 u32OutPort;                           /**< ��������˿ں� */

	U8  u8DeviceIp[MAX_IPV4_LEN];             /**< �豸IP��ַ */
	U16 u16DevicePort;                        /**< �豸�˿ں� */

	U8  u8SubServIp[MAX_IPV4_LEN];            /**< ������ý���������ַ */
	U16 u16SubServPort;                       /**< ������ý��˿ں� */
}MATRIX_REQ_S;

/**
 * �����������л� 
 */
typedef struct
{
	U32 u32Mode;                              /**< ��ʾģʽ��NET_SHOW_MODE_E*/
	U32 u32Channel;                           /**< ��Ҫ������ʾ��ͨ����һλ��Ӧһ��ͨ�� */
	U32 u32OutPort;                           /**< ��ʾ����˿� */

	U8  u8DeviceIp[MAX_IPV4_LEN];             /**< �豸IP��ַ */
	U16 u16DevicePort;                        /**< �豸�˿ں� */

	U8  u8SubServIp[MAX_IPV4_LEN];            /**< ������ý���������ַ */
	U16 u16SubServPort;                       /**< ������ý��˿ں� */
}DECODER_REQ_S;

typedef struct
{
	U32 u32Mode;                              /**< ��ʾģʽ��NET_SHOW_MODE_E*/
	U32 u32OutPort;                           /**< ��ʾ����˿� */

	U8  u8DeviceIp[MAX_IPV4_LEN];             /**< �豸IP��ַ */
	U16 u16DevicePort;                        /**< �豸�˿ں� */

	U8  u8SubServIp[MAX_IPV4_LEN];            /**< ������ý���������ַ */
	U16 u16SubServPort;                       /**< ������ý��˿ں� */
}DEC_DISPLAY_REQ_S;

/* ��ý��������������� */
typedef struct
{
	S8   s8Url[MAX_URL_LEN];                  /**< ��ý���������ַ */
	U16  u16Port;	                          /**< ��ý��������˿� */

	S8  s8Username[MAX_NAME_LEN];             /**< ��¼�û��� */
	S8  s8Password[MAX_NAME_LEN];             /**< ��¼������ */

	U32  u32TransProtocol;	                  /**< ����Э������ 0-TCP��1-UDP*/
}DEC_STREAM_CFG_S;

/* DVR�豸ͨ����Ϣ */
typedef struct
{
	U8  u8Ip[MAX_IPV4_LEN];                   /**< DVR IP��ַ */
	U16 u16Port;                              /**< DVR �˿ں� */

	S8  s8Username[MAX_NAME_LEN];             /**< DVR ��¼�û��� */
	S8  s8Password[MAX_NAME_LEN];             /**< DVR ��¼������ */

	U32 u32ChnNo;                             /**< DVR ͨ����, ��0��ʼ */
	U32 u32TransProtocol;		              /**< ����Э������0-TCP��1-UDP */
	U32 u32TransMode;			              /**< ��������ģʽ 0�������� 1�������� */
	U32 u32ProtoType;			              /**< ǰ����Э������ */
}DEC_DVR_CFG_S;

typedef struct
{
	S32 s32DecChnNo;                          /**< ����ͨ����, ��0��ʼ */
	S32 s32DisOutPort;                        /**< < ��ʾ���ͨ�� */ 
	S32 s32DisWndNo;                          /**< ��ʾͨ�����ڴ��ں�, ��0��ʼ */

	BOOL bValidStream;                        /**< ��ý��������Ƿ���� */
	union
	{
		DEC_DVR_CFG_S    stDvrCfg;
		DEC_STREAM_CFG_S stStreamCfg;         
	}stDecChnCfg;                             /**< ����ͨ����Ƶ������Ϣ */

	U8  u8DeviceIp[MAX_IPV4_LEN];             /**< �豸IP��ַ */
	U16 u16DevicePort;                        /**< �豸�˿ں� */

	U8  u8SubServIp[MAX_IPV4_LEN];            /**< ������ý���������ַ */
	U16 u16SubServPort;                       /**< ������ý��˿ں� */
}DEC_DYNAMIC_REQ_S;

typedef struct
{
	S32  s32DisOutPort;                        /**< < ��ʾ���ͨ�� */ 
	U8   u8DeviceIp[MAX_IPV4_LEN];             /**< �豸IP��ַ */
	U16  u16DevicePort;                        /**< �豸�˿ں� */
	
	U8   u8SubServIp[MAX_IPV4_LEN];            /**< ������ý���������ַ */
	U16  u16SubServPort;                       /**< ������ý��˿ں� */
}DEC_DYNAMIC_STOP_REQ_S;

/* 
 * ¼���ѯ 
 */

/* ¼������ */
#define RECORD_ALL		0xFFFFFFFF            /**< Only for req, equal to RECORD_NORMAL|RECORD_ALARM... */

typedef struct _QUERY_RECORD_REQ_S_
{
	U8  u8ReqType;                            /**< ����ʽ��0-IP+�˿ڷ�ʽ 1-��ַ�뷽ʽ */
	U8  u8Reserved[3];

	U32        u32Type;
	DATETIME_S stBegin;
	DATETIME_S stEnd;

	union
	{
		S8  s8FrontAddrCode[MAX_ADDR_LEN];    /**< ǰ���豸��ַ���� */
		struct
		{
			U32        u32ChnNO;              /**< ͨ���� 0,1,2,3... */
			U8  u8DeviceIp[MAX_IPV4_LEN];     /**< �豸IP��ַ */
			U16 u16DevicePort;                /**< �豸�˿ں� */

			U8  u8SubServIp[MAX_IPV4_LEN];    /**< ������ý���������ַ */
			U16 u16SubServPort;               /**< ������ý��˿ں� */
		};
	};
}QUERY_RECORD_REQ_S;

typedef struct _QUERY_RECORD_REQ_EX_S_
{
	U8  u8ReqType;                            /**< ����ʽ��0-IP+�˿ڷ�ʽ 1-��ַ�뷽ʽ */
	U8  u8Reserved[3];

	U32        u32Type;
	DATETIME_S stBegin;
	DATETIME_S stEnd;

	U32        u32TriggerType;
	U32        u32TriggerPort;

	union
	{
		S8  s8FrontAddrCode[MAX_ADDR_LEN];    /**< ǰ���豸��ַ���� */
		struct
		{
			U32        u32ChnNO;              /**< ͨ���� 0,1,2,3... */
			U8  u8DeviceIp[MAX_IPV4_LEN];     /**< �豸IP��ַ */
			U16 u16DevicePort;                /**< �豸�˿ں� */

			U8  u8SubServIp[MAX_IPV4_LEN];    /**< ������ý���������ַ */
			U16 u16SubServPort;               /**< ������ý��˿ں� */
		};
	};
}QUERY_RECORD_REQ_EX_S;

typedef struct _REC_DETAIL_RSP_S_
{
	U32        u32StSize;                     /**< sizeof(REC_DETAIL_RSP_S) */
	U32        u32ChnNO;
	DATETIME_S stBegin;
	DATETIME_S stEnd;
	U32        u32Type; 
	U32        u32Size; 
	S8         s8FileName[MAX_FILENAME_LEN];
}REC_DETAIL_RSP_S;

typedef struct
{
	U32        u32StSize;                     /**< REC_CALENDAR_RSP_S */
	DATETIME_S stDay;
}REC_CALENDAR_RSP_S;

typedef struct
{
	UINT StructSize;                          /**< sizeof(REC_HALFHOUR_RSP_S) */
	WORD Year;                                /**< 2011 */
	BYTE Month;                               /**< 1 ~ 12 */
	BYTE Day;                                 /**< 1 ~ 31 */
	BYTE HarfHour;                            /**< 0 ~ 47 */
	BYTE Minute;                              /**< 0 ~ 59 */
	BYTE Second;                              /**< 0 ~ 59 */
	BYTE Type;
}REC_HALFHOUR_RSP_S;


/**
 * ¼������
 * ���� MsgType=MSG_DOWNLOAD, MsgSize=sizeof(REC_DOWNLOAD_REQ_S)+n*sizeof(REC_DETAIL_RSP_S)
 * ��Ӧ �ɹ�AckFlag=ACK_SUCCESS ʧ��AckFlag=ACK_FAIL
 * ��������: file1,FileEndPack; file2,FileEndPack; ... filen,FileEndPack
 */
typedef struct
{
	U32        u32UserKey;
	U8         u8DownloadMode;                                 /**< ���ط�ʽ��1:˽��ģʽ 2:��׼ģʽ */
	U8         u8Reserved[3];

	union
	{
		struct  
		{
            U32     u32ChnNO;	
			U8      u8DeviceIp[MAX_IPV4_LEN];
			U16     u16DevicePort;
			U8      u8SubServIp[MAX_IPV4_LEN];
			U16     u16SubServPort;
		};
		
		struct  
		{
			S8      s8FrontAddrCode[MAX_ADDR_LEN];
		};
	};

}REC_DOWNLOAD_REQ_S;

/**
 * ¼��ط�
 * 
 * ��ʼ�ط�
 * ���� MsgType=MsgRecordPlay, MsgSize=sizeof(RecordPlayReq)
 * ��Ӧ �ɹ�AckFlag=ACK_SUCCESS, ʧ��AckFlag=ACK_FAIL, ��Ӧ�󲻷�������,ͳһ���յ�MsgRecordDataInsufficient����
 *
 */
enum 
{
	REC_PLAY_IP_TIME = 0,                                      /**<  0-IP+PORT+ʱ��λط� */
	REC_PLAY_IP_NAME,                                          /**<  1-IP_PORT+�ļ��ط� */
	REC_PLAY_ADDR_URL,                                         /**<  2-��ַ��+RTSP URL�ط� */
	REC_PLAY_ADDR_TIME,                                        /**<  3-��ַ��+ʱ��ط� */
	REC_PLAY_HIKPLATFORM,                                      /**< ����ƽ̨�ط� */
};                                                             
                                                               
typedef struct _REC_PLAY_REQ_S_
{
	U32                        u32UserKey;                     /**< �û��� */
	U8                         u8PlayMode;                     /**< ����ʽ, REC_PLAY_IP_TIME ... */
	U8                         u8Reserved[3];

	union
	{
		struct
		{
			S8                 s8FrontAddrCode[MAX_ADDR_LEN];  /**< ǰ���豸��ַ���� */
			union
			{
				S8             s8RstpUrl[MAX_FILENAME_LEN];    /**< RTSP ·�� */
				struct
				{
					DATETIME_S stBegin;                        /**< ��ʼʱ�� */
					DATETIME_S stEnd;                          /**< ����ʱ�� */
				};
			};
		}stAddrMode;

		struct
		{
			union
			{
				S8             s8FileName[MAX_FILENAME_LEN];   /**< ¼���ļ��� */
				struct
				{
					U32        u32ChnNO;                       /**< ͨ���� 0,1,2,3... */
					U32        u32Type;
					DATETIME_S stBegin;                        /**< ��ʼʱ�� */
					DATETIME_S stEnd;                          /**< ����ʱ�� */
				};
			};
			U8                 u8DeviceIp[MAX_IPV4_LEN];       /**< �豸IP��ַ */
			U16                u16DevicePort;                  /**< �豸�˿ں� */

			U8                 u8SubServIp[MAX_IPV4_LEN];      /**< ������ý���������ַ */
			U16                u16SubServPort;                 /**< ������ý��˿ں� */
		}stIpMode;

		struct  
		{
			S8             s8FileName[MAX_FILENAME_LEN];   /**< ¼���ļ��� */
			DATETIME_S     stBegin;                        /**< ��ʼʱ�� */
			DATETIME_S     stEnd;                          /**< ����ʱ�� */
		}stHikPlatMode;	
	};

	U8                         u8ExactFrame[40];
}REC_PLAY_REQ_S;

/**
 * �طŷ���
 * ���� MsgType=MsgRecordPlayFeedback, MsgSubType=MsgRecordDataInsufficient,MsgRecordDataEnough
 * ����Ҫ��Ӧ
 * 
 * �ض�λ¼������
 * ���� MsgType=MsgReposRecordData, MsgSize=sizeof(ReposRecordDataReq)
 * ����Ҫ��Ӧ,�յ�����Ϣ��,����ֹͣ����֮ǰ������(���������Ƿ�����һ�������İ�֮��),
 * ���ŷ���ReposPack,����Ƿ����µ�����
 */
typedef struct
{
	DATETIME_S stBegin;
	DATETIME_S stEnd;
}REC_REPOSE_REQ_S;

/**
 * ��̨����
 */

#define PTZMinSpeed			0
#define PTZMaxSpeed			100
#define PTZMinCruiseNo		0
#define PTZMaxCruiseNo		63
#define PTZMinPresetIndex	0
#define PTZMaxPresetIndex	255

typedef struct _PTZ_CTRL_NORMAL_S_
{
	U32 u32Flag;                                               /**< 0-start, 1-stop */
	U32 u32Speed;
}PTZ_CTRL_NORMAL_S;

typedef struct _PTZ_CTRL_CRUISE_S_
{
	U32 u32Flag;
	U32 u32No;
}PTZ_CTRL_CRUISE_S;

typedef struct _PTZ_CTRL_PRESET_S_
{
	U32 u32Index;
}PTZ_CTRL_PRESET_S;

typedef struct _PTZ_CTRL_REQ_S_
{
	U32                       u32UserKey;
	U8                        u8ReqType;                       /**< ����ʽ��0-IP+�˿ڷ�ʽ 1-��ַ�뷽ʽ */

	union
	{
		S8                    s8FrontAddrCode[MAX_ADDR_LEN];   /**< ǰ���豸��ַ���� */
		struct
		{
			U32               u32ChnNO;                        /**< ͨ���� 0,1,2,3... */
			U8                u8DeviceIp[MAX_IPV4_LEN];        /**< �豸IP��ַ */
			U16               u16DevicePort;                   /**< �豸�˿ں� */

			U8                u8SubServIp[MAX_IPV4_LEN];       /**< ������ý���������ַ */
			U16               u16SubServPort;                  /**< ������ý��˿ں� */
		};
	};

	union
	{
		PTZ_CTRL_NORMAL_S     stNormal;                        /**< MsgPTZUp...MsgPTZAutoScan */
		PTZ_CTRL_CRUISE_S     stCruise;                        /**< MsgPTZCruise */
		PTZ_CTRL_PRESET_S     stPreset;                        /**< MsgPTZSetPreset...MsgPTZCallPreset */
	};
}PTZ_CTRL_REQ_S;

/* �������� */
typedef struct _CHN_PARAM_REQ_S_
{
	U32                       u32StSize;
	U32                       u32ChnMask;
	S8                        s8AddrCode[MAX_ADDR_LEN];
}CHN_PARAM_REQ_S;

/**
 * �����ϱ�
 */
typedef struct{
	S8                        s8DeviceName[MAX_NAME_LEN];      /**< �����豸�� */
	U8                        u8DeviceIp[MAX_IPV4_LEN];        /**< �����豸IP */

	U16                       u16DevicePort;                   /**< �����豸�˿� */
	U8                        u8Res[2];
}ALARMER_S;

typedef struct
{
	DATETIME_S                stTime;                          /**< ��������ʱ�� */
	U8                        u8ChnNo;                         /**< ����ͨ���� */
	U8                        u8Status;                        /**< ����״̬��0-��ʼ 1-���� */
}ALARM_IO_RPT_S;

typedef struct
{
	DATETIME_S                stTime;                          /**< ��������ʱ�� */
	U8                        u8ChnNo;                         /**< ����ͨ���� */
	U8                        u8Status;                        /**< ����״̬��0-��ʼ 1-���� */
}ALARM_MD_RPT_S;

typedef struct
{
	DATETIME_S                stTime;                          /**< ��������ʱ�� */
	U8                        u8ChnNo;                         /**< ����ͨ���� */
	U8                        u8Status;                        /**< ����״̬��0-��ʼ 1-���� */
}ALARM_VLOSS_RPT_S;

typedef struct
{
	DATETIME_S                stTime;                          /**< ��������ʱ�� */
	U8                        u8HddNo;                         /**< ����Ӳ�̺� */
	U8                        u8Status;                        /**< ����״̬��0-��ʼ 1-���� */
}ALARM_HDD_RPT_S;

enum 
{
	AI_TYPE_TRIPWIRE  = 4,                                     /**< ���� */ 
	AI_TYPE_PERIMETER,                                         /**< �ܽ� */ 
	AI_TYPE_ITEMLEFT,                                          /**< ��Ʒ������ʧ */
	AI_TYPE_PLATE,                                             /**< ���Ƽ�� */
	AI_TYPE_HOVER,                                             /**< �ǻ����  */
	AI_TYPE_ANTITHEFT,                                         /**< �������   */
	AI_TYPE_OFFJOB,                                            /**< �Ѹڼ��   */
	AI_TYPE_MAINTENAREA,                                       /**< ���������� */
	AI_TYPE_PRESSPLATE,                                        /**< ѹ��Ͷ��״̬��� */ 
	AI_TYPE_INDICATOR,                                         /**< �ֺ�ָʾ�Ƽ�� */
	AI_TYPE_DISCONNECTOR,                                      /**< ���뿪��(��ͷ)״̬��� */
	AI_TYPE_LIGHT,                                             /**< ָʾ��״̬���  */
	AI_TYPE_HELMET,                                            /**< ��ȫñ��� */   
    AI_TYPE_OCRTEXT,                                          /**< �ı�ʶ�� */
	AI_TYPE_GNDWIRE,                                          /**< �ӵ���ʶ�� */
	AI_TYPE_SAFETYRULE,                                       /**< �����¼� */  
    AI_TYPE_TOWERLEAN,                                        /**< ������бʶ�� */
	AI_TYPE_SNAPMATCH,                                        /**< �����ȶ�ʶ�� */
	AI_TYPE_ELECTRICFENCE,                                    /** < ����Χ�� */
	AI_TYPE_GATE,                                             /**< ��բ״̬ */
	AI_TYPE_JUEYUAN,                                           /**< ��ۼ��-��Ե�� */
	AI_TYPE_GANTA,											   /**< ��ۼ��-���� */
	AI_TYPE_THERMOMETRYALARM,                                  /**< �����¶ȸ澯 */
	AI_TYPE_PTZ_ALARM,                                        /**< ��̨���Ƹ澯 */
	AI_YX_COUNT,
	AI_TYPE_LEDDIGIT  = 30,                                    /**< ��ƶ������(�߶������) */
	AI_TYPE_METER,                                             /**< ��ƶ������(�̶���) */
    AI_TYPE_NUMPEOPLE,                                         /**< ����ͳ�� */
	AI_TYPE_THERMOMETRY,                                       /**< ���� */
	AI_TYPE_AREAPEOPLE,                                       /**< ��������ͳ�� */
	AI_TYPE_PASSFLOW,                                        /**< ����������ͳ�� */
};

enum
{
    PLATE_TIANDY_STANDARD = 1,                                 /**< ��س���ʶ���(��׼) */
	PLATE_DAHUA_STARNDARD,                                     /**< �󻪳���ʶ���(��׼) */
	PLATE_HIK_STARNDARD,                                       /**< ��������ʶ���(��׼) */
	PLATE_HIK_NORMAL,                                          /**< ��������ʶ���(��ͨ) */
};

typedef struct
{
	S8                       s8Plate[12];                      /**< ���ƺ� */
	U8                       u8PlateType;                      /**< �������� */
	U8                       u8PlateColor;                     /**< ������ɫ */
	U8                       u8CarColor;                       /**< ������ɫ */
	U8                       u8DevType;                        /**< �豸���� ��:PLATE_TIANDY_STANDARD*/
	U16                      u16Millsecond;                    /**< ץ�ĺ��� */
	U8                       u8CarType;                        /**< �������� 1:С���� 2:���� 3:����� 0:����*/
	U8                       u8Direct;                         /**< �������� 1:�� 2:�� 0:δ֪*/
	U32                      u32PicNo;                         /**< ���ΰҵftpץ��ͼƬ������� */
}AI_PLATE_S;

/**< ���ܷ��������� */
typedef struct
{
	U16                      u16Px;
	U16                      u16Py;
}AI_POINT_S;

/**< ���ܷ������α��� */
typedef struct
{
    AI_POINT_S   stFrom;                                       /**< �������ϵ� */
	AI_POINT_S   stTo;                                         /**< �������µ� */
	U8          u8Enable;                                      /**< �Ƿ���Ч */
	U8          u8Alarm;                                       /**< �Ƿ񱨾� */
	U8          u8Reserved[2];
}AI_RECTANGLE_S;

/**< ���ܷ���������ɫ */
enum AI_COLOR_S
{
	AI_COLOR_BLACK = 0,                                        /**< ��ɫ */
	AI_COLOR_GRAY,                                             /**< ��ɫ */
	AI_COLOR_WHITE,                                            /**< ��ɫ */
	AI_COLOR_RED,                                              /**< ��ɫ */
	AI_COLOR_YELLOW,                                           /**< ��ɫ */
	AI_COLOR_GREEN,                                            /**< ��ɫ */
	AI_COLOR_BLUE,                                             /**< ��ɫ */
	AI_COLOR_PURPLE,                                           /**< ��ɫ */
	AI_COLOR_DARKGRAY,                                         /**< ���*/
    AI_COLOR_BKAIR,                                            /**< �񺽺�ɫ(����) */
	AI_COLOR_SILVER,                                           /**< ��ɫ */
	AI_COLOR_DARKBLUE,                                         /**< ���� */
	AI_COLOR_BROWN,                                            /**< ��ɫ */
	AI_COLOR_PINK,                                             /**< ��ɫ */
	AI_COLOR_CYAN,                                             /**< ��ɫ */
    AI_COLOR_ORANGE_RED,                                       /**< �Ⱥ� */
	AI_COLOR_ORGANE,                                           /**< ��ɫ */
    AI_COLOR_LIGHT_BLUE,                                       /**< ���� */
	AI_COLOR_LIGHT_YELLOW,                                     /**< ���� */
	AI_COLOR_LIGHT_CYAN,                                       /**< ���� */
	AI_COLOR_LIGHT_GREEN,                                      /**< ���� */
	AI_COLOR_LIGHT_PURPLE,                                     /**< ���� */
	AI_COLOR_YELLOW_GREEN,                                     /**< ���� */
	AI_COLOR_GOLDEN,                                           /**< ��ɫ */
	AI_COLOR_BLUE_GRAY,                                        /**< ���� */
	AI_COLOR_GREEN_GRAY,                                       /**< �̻� */
	AI_COLOR_CREAM,                                            /**< ��ɫ */
	AI_COLOR_CYAN_GREEN,                                       /**< ���� */
	AI_COLOR_DARK_RED,                                         /**< ��� */
	AI_COLOR_SKY_BLUE,                                         /**< ���� */
	AI_COLOR_SILVER_GRAY,                                      /**< ���� */
	AI_COLOR_PURPLE_RED,                                       /**< �Ϻ� */
	AI_COLOR_BROWN_RED,                                        /**< �غ� */
	AI_COLOR_BROWN_YELLOW,                                     /**< �ػ� */
	AI_COLOR_FLESH,                                            /**< ��ɫ */
	AI_COLOR_UNKNOWN = 100,
};

/**< �������궨�� */
typedef struct
{
    AI_POINT_S               stFrom;                           /**< ��ʼ��   */
	AI_POINT_S               stTo;                             /**< �յ�     */

	U8                       u8Direct ;                        /**< �Ƿ�˫�� */
	U8                       u8Enable ;                        /**< �Ƿ���Ч */	
	U8                       u8Alarm;                          /**< �Ƿ񱨾� */
	U8                       u8Reserved[1];
}AI_TRIPWIRE_S;


typedef struct
{
	union
	{
		struct
		{
			AI_POINT_S       stFrom;                           /**< �������ϵ� */
			AI_POINT_S       stTo;                             /**< �������µ� */
		}stAiRect;

		AI_POINT_S           stPts[10];                        /**< ����ε�����(���10����) */
	};

	U8                       u8Type;                           /**< �������� 1-���Ρ�2-�����*/
	U8                       u8Enable;                         /**< �Ƿ���Ч */
	U8                       u8Alarm;                          /**< �Ƿ񱨾� */
	U8                       u8Reserved[1];
}AI_PERIMETER_S;

typedef struct
{
	union
	{
		struct
		{
			AI_POINT_S       stFrom;                           /**< �������ϵ� */
			AI_POINT_S       stTo;                             /**< �������µ� */
		}stAiRect;

		AI_POINT_S           stPts[10];                        /**< ����ε�����(���10����) */
	};

	U8                       u8Type;                           /**< �������� 1-���Ρ�2-�����*/
	U8                       u8Enable;                         /**< �Ƿ���Ч */
	U8                       u8Alarm;                          /**< �Ƿ񱨾� */
	U8                       u8Reserved[1];
}AI_ITEMLEFT_S;

typedef struct
{
	union
	{
		struct
		{
			AI_POINT_S       stFrom;                           /**< �������ϵ� */
			AI_POINT_S       stTo;                             /**< �������µ� */
		}stAiRect;

		AI_POINT_S           stPts[10];                        /**< ����ε�����(���10����) */
	};

	U8                       u8Type;                           /**< �������� 1-���Ρ�2-�����*/
	U8                       u8Enable;                         /**< �Ƿ���Ч */
	U8                       u8Alarm;                          /**< �Ƿ񱨾� */
	U8                       u8Reserved[1];
}AI_HOVER_S;

typedef struct
{
	union
	{
		struct
		{
			AI_POINT_S       stFrom;                           /**< �������ϵ� */
			AI_POINT_S       stTo;                             /**< �������µ� */
		}stAiRect;

		AI_POINT_S           stPts[10];                        /**< ����ε�����(���10����) */
	};

	U8                       u8Type;                           /**< �������� 1-���Ρ�2-�����*/
	U8                       u8Enable;                         /**< �Ƿ���Ч */
	U8                       u8Alarm;                          /**< �Ƿ񱨾� */
	U8                       u8Reserved[1];
}AI_ANTITHEFT_S;

typedef struct
{
	union
	{
		struct
		{
			AI_POINT_S       stFrom;                           /**< �������ϵ� */
			AI_POINT_S       stTo;                             /**< �������µ� */
		}stAiRect;
		
		AI_POINT_S           stPts[10];                        /**< ����ε�����(���10����) */
	};
	
	U8                       u8Type;                           /**< �������� 1-���Ρ�2-�����*/
	U8                       u8Enable;                         /**< �Ƿ���Ч */
	U8                       u8Alarm;                          /**< �Ƿ񱨾� */
	U8                       u8Reserved[1];
}AI_OFFJOB_S;

typedef struct
{
	union
	{
		struct
		{
			AI_POINT_S       stFrom;                           /**< �������ϵ� */
			AI_POINT_S       stTo;                             /**< �������µ� */
		}stAiRect;
		
		AI_POINT_S           stPts[10];                        /**< ����ε�����(���10����) */
	};
	
	U8                       u8Type;                           /**< �������� 1-���Ρ�2-�����*/
	U8                       u8Enable;                         /**< �Ƿ���Ч */
	U8                       u8Alarm;                          /**< �Ƿ񱨾� */
	U8                       u8Reserved[1];
}AI_MAINTENAREA_S;

typedef struct
{
	AI_POINT_S               stFrom;                           /**< �������ϵ� */
	AI_POINT_S               stTo;                             /**< �������µ� */
	AI_COLOR_S               enAiColor;                        /**< ��ɫ */
	U8                       u8OnOff;                          /**< ����򿪹�״̬ */
	U8                       u8Enable;                         /**< �Ƿ���Ч */
	U8                       u8Reserved[1];
}AI_LIGHT_S;

typedef struct
{
	AI_POINT_S               stPts[3];                         /**< �е㣬ʼĩ��*/
	U8                       u8DialType;                       /**< �̶�������  */
	U8                       u8Enable;                         /**< �Ƿ���Ч */
	U8                       u8Reserved[2];	
	   
	FLOAT                    fDialValue;                       /**< �̶��̵�ǰֵ */
}AI_METER_S;

typedef struct
{
    AI_POINT_S               stFrom;                           /**< ��ʼ��   */
	AI_POINT_S               stTo;                             /**< �յ�     */
	U8                       u8Enable;                         /**< �Ƿ���Ч */
	U8                       u8Alarm;                          /**< �Ƿ񱨾� */
	U8                       u8Reserved[2];
}AI_INDICATOR_S;

typedef struct  
{
    AI_POINT_S               stFrom;                           /**< ��ʼ��   */
	AI_POINT_S               stTo;                             /**< �յ�     */

	U8                       u8Enable;                         /**< �Ƿ���Ч */
	U8                       u8Alarm;                          /**< �Ƿ񱨾� */
	FLOAT                    fValue;                           /**< �����ֵ */
	U8                       u8DecNum;                         /**< С��λ�� */
	U8                       u8Reserved[3];
}AI_LEDDIGIT_S;

typedef struct
{
	U8						s8IsChange[MAX_OBJ_PRESSPLATE];/** < �ж��Ƿ�ı�,1�ı䣬0����*/
	S8						s8CurState[MAX_OBJ_PRESSPLATE];   /**< ��ǰ״̬ 0(ѹ���˳�״̬),1(ѹ��Ͷ��״̬) */
	U8						u8Enable[MAX_OBJ_PRESSPLATE];     /**< ��ǰ״̬�Ƿ���Ч*/
	U8						u8PressPlateNum;             /**< ѹ����Ŀ */
	U8						u8Reserved[3];
}AI_PRESSPLATE_S;

typedef struct
{
	AI_POINT_S        stFrom;                                  /**< ��ʼ��   */
    AI_POINT_S        stTo;                                    /**< �յ�     */

	U8                u8Alarm;                                 /**< ������� 0:δ���� 1:���� */
	U8                u8Enable;
	U8                u8Reserved[2];                           /**< Ԥ�� */
}AI_DISCONNECTOR_S;

typedef struct
{
	AI_RECTANGLE_S    stMovPosition;                           /**< �ƶ��������ο� */      
	U8                u8Alarm;                                 /**< ������� 0:δ���� 1:���� */
	U8                u8Enable;
	U8                u8Reserved[2];                           /**< Ԥ�� */
}AI_HELMET_S;

typedef struct
{
	AI_POINT_S        stFrom;                                  /**< ��ʼ��   */
	AI_POINT_S        stTo;                                    /**< �յ�     */

	U32               u32NumPeople;
	U8                u8Enable;
	U8                u8Direct;
	U8                u8Reserved[2];
}AI_NUMPEOPLE_S;

typedef struct
{
	FLOAT            fValue;
	U8               u8Enable;
	U8               u8Reserved[3];
}AI_THERMOMETRY_S;

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
	DATETIME_S               stTime;                           /**< ��������ʱ�� */

	U8                       u8AiType;                         /**< ���ܷ�������, 1-���� 2-�ܽ� */
	U8                       u8ChnNo;                          /**< ���ܷ���ͨ���� */
	U8                       u8Preset;                         /**< ���ܷ���ͨ��Ԥ��λ */
	U8                       u8AreaType;                       /**< ���ܷ���Ŀ���������� LINE:0, RECT:1, POLY:2 */

    U16                      u16PicHeight;	                   /**< ���ܷ���ͼ��ֱ��ʸ߶� */
	U16                      u16PicWidth;                      /**< ���ܷ���ͼ��ֱ��ʿ�� */
	U32                      s32Status;                        /**< 0:�澯����, 1:�澯��ʼ */ 
 //   U8                       u8AlarmType;                    /**< �������� 1:���� 0:���� */	

	union
	{

        AI_TRIPWIRE_S        AiTripwite[MAX_AIOBJ_NUM];          /**< ���߱��� */
		AI_PERIMETER_S       AiPerimeter[MAX_AIOBJ_NUM];         /**< �ܽ籨�� */
        AI_ITEMLEFT_S        AiItemleft[MAX_AIOBJ_NUM];          /**< ��Ʒ���� */
		AI_PLATE_S           AiPlate;                            /**< ������Ϣ */
		AI_HOVER_S           AiHover[MAX_AIOBJ_NUM];             /**< �ǻ����  */
	    AI_ANTITHEFT_S       AiAntitheft[MAX_AIOBJ_NUM];         /**< �������   */
	    AI_OFFJOB_S          AiOffjob[MAX_AIOBJ_NUM];            /**< �Ѹڼ��   */
	    AI_MAINTENAREA_S     AiMaintenarea[MAX_AIOBJ_NUM];       /**< ���������� */
	    AI_PRESSPLATE_S      AiPressplate;                       /**< ѹ��Ͷ��״̬��� */ 
	    AI_INDICATOR_S       AiIndicator[MAX_AIOBJ_NUM];         /**< �ֺ�ָʾ�Ƽ�� */
	    AI_DISCONNECTOR_S    AiDisconnector[MAX_AIOBJ_NUM];      /**< ���뿪��(��ͷ)״̬��� */
	    AI_LIGHT_S           AiLight[MAX_AIOBJ_NUM];             /**< ָʾ��״̬���  */
	    AI_LEDDIGIT_S        AiLeddigit[MAX_AIOBJ_NUM];          /**< ��ƶ������(�߶������) */
	    AI_METER_S           AiMeter[MAX_AIOBJ_NUM];             /**< ��ƶ������(�̶���) */
	    AI_HELMET_S          AiHelmet[MAX_AIOBJ_NUM][20];        /**< ��ȫñ��� */
		AI_NUMPEOPLE_S       AiNumPeople[MAX_AIOBJ_NUM];         /**< ����ͳ�� */
		AI_THERMOMETRY_S     AiThermometry[MAX_AIOBJ_NUM];       /**< ���� */
		AI_OCRTEXT_S         AiOcrText[MAX_AIOBJ_NUM];           /**< �ı�ʶ�� */
        AI_GNDWIRE_S         AiGndWire[MAX_AIOBJ_NUM];           /**< �ӵ���ʶ�� */
		AI_SAFETYRULE_S      AiSafetyRule[MAX_AIOBJ_NUM];        /**< �����¼� */
        AI_AREAPEOPLE_S      AiAreaPeople[MAX_AIOBJ_NUM];        /**< ��������ͳ�� */
		AI_TOWERLEAN_S       AiTowerLean[MAX_AIOBJ_NUM];         /**< �ܹ���бʶ�� */
		AI_SNAPMATCH_S       AiSnapMatch;                        /**< �����ȶ�ʶ�� */
		AI_PASSFLOW_S        AiPassFlow;                         /**< ����������ͳ�� */
		AI_ELECTRICFENCE_S   AiEleCtriCfence[MAX_AIOBJ_NUM];     /**< ����Χ�� */
		AI_GATE_S			 AiGate[MAX_AIOBJ_NUM];		         /**< ��բ״̬ */
		AI_THERMOMETRY_ALARM_S AiThermometryAlarm[MAX_AIOBJ_NUM];/**<�����¶ȸ澯*/		
		AI_JUEYUAN_S		 AiJueYuan[MAX_AIOBJ_NUM];		     /**< ��ۼ��-��Ե�� */
		AI_GANTA_S			 AiGanTa[MAX_AIOBJ_NUM];		     /**< ��ۼ��-���� */
	};

	S8                      s8PicName[MAX_PATH];             /**< ���ܷ���ץ��ͼƬ�ļ����� */

}ALARM_AI_RPT_S;

/**< ϵͳ���в��� */
typedef struct  
{
	time_t      stStartTime;                                   /**< ϵͳ����ʱ�� */
	time_t      stRunTime;                                     /**< ϵͳ����ʱ�� */
	U32         u32ClientNo;                                   /**< �ͻ��������� */
	U32         u32TranTaskNo;                                 /**< ת�������� */
	U32         u32CpuUsed;                                    /**< ϵͳʹ��cpu�� */
	U32         u32TranCpuUsed;                                /**< ת������ʹ��cpu�� */
	U32         u32PhMemTotal;                                 /**< �����ڴ����� */
	U32         u32PhMemRemain;                                /**< �����ڴ������ */
	U32         u32VirMemTotal;                                /**< �����ڴ����� */
	U32         u32VirMemRemain;                               /**< �����ڴ���� */
	FLOAT       fSendNetworkTotal;                             /**< ���緢������ */
	FLOAT       fSendNetworkRemain;                            /**< ��������� */
	FLOAT       fRecvNetworkTotal;                             /**< ����������� */
	FLOAT       fRecvNetworkRemain;                            /**< ������տ����� */
}ALARM_SYS_RPT_S;

/* �豸״̬�ϴ��ṹ */
typedef struct
{
	DATETIME_S              stTime;                            /**< ��������ʱ�� */
	U8                      u8DeviceIp[MAX_IPV4_LEN];          /**< �����豸IP */
	
	U16                     u16DevicePort;                     /**< �����豸*/
	U8                      u8Status;                          /**< ����״̬��0-���� 1-���� */
}ALARM_DEVSTATE_RPT_S;

typedef struct{
	S8                      s8TaskName[MAX_FILENAME_LEN];      /**< ���������� */

	U8                      u8DeviceIp[MAX_IPV4_LEN];          /**< �����豸IP */

	U8                      u8ChnNO;                           /**< �����豸ͨ���� */
	U8                      u8PresetNo;                        /**< �����豸Ԥ�õ�� */
	U16                     u16TaskId;                         /**< ��������ID */
}LINKAGE_RPT_S;

/* �ͻ�������ת��������Ϣ */
typedef struct{
    S8                      s8SrcName[MAX_NAME_LEN];           /**< ����ת��Դ(�ͻ���)���� */
	U8                      u8SrcIp[MAX_IPV4_LEN];             /**< ����ת��Դ(�ͻ���)IP */
	S8                      s8Desc[MAX_FILENAME_LEN];          /**< �������� */
	U32                     u32DeviceNum;                      /**< �����豸����, 1-16 */			
	U32                     u32CameraId[16];                   /**< �����豸ͨ��ID�� */
	U8                      u8PresetNo[16];                    /**< �����豸Ԥ�õ�� */
}LINKAGE_TRANS_RPT_S;

typedef struct
{
	DATETIME_S              stTime;                            /**< ��������ʱ�� */
	U8                      u8ChnNo;                           /**< ����ͨ���� */
	U8                      u8Status;                          /**< ����״̬��0-��ʼ 1-���� */
}ALARM_MANUAL_RPT_S;

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
}ALARM_GPS_RPT_S;

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
	S8                      s8CardImgUrl[MAX_NAME_LEN*2];      /**< ���ͼ��url */
	S8                      s8CarImgUrl[MAX_NAME_LEN*2];       /**< ����ͼ��url */
	S8                      s8Time[MAX_NAME_LEN*2];            /**< ʱ�� */
}ALARM_ACCESS_RPT_S;

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
	ALARM_CAMERA_STATUS camStatus[64];
}ALARM_CAMERA_RPT_S;

typedef struct
{
	U32                     u32StSize;                         /**< �ṹ�峤�� */
	S8                      s8Name[MAX_NAME_LEN];
	S32                     s32LocalUserId;                    /**< ���ص�¼�û�ID */
	S8                      s8IP[MAX_IP_STR_LEN];              /**< Զ�̿ͻ���IP��ַ */
	U16                     u16Port;                           /**< Զ�̿ͻ��Ķ˿ں� */
	U32                     u32ClientKey;                      /**< ����ͻ��ؼ��� */
	U32                     u32DTalkState;                     /**< ����Ƶ�Խ�״̬ */
} CLIENT_INFO_RSP_S;

typedef struct
{
	U32                     u32UserKey;                        /**< ���ӷ�����û���Կ */
	DATETIME_S              stTime;                            /**< ͬ��ʱ��ֵ */
	U8                      u8TimeZone;                        /**< ʱ�� */
	U8                      u8IsSetLoaclTime;                  /**< �Ƿ����ñ���ʱ�� */
	U8                      u8Reserved[2];
}SYNCHR_TIME_REQ_S;

typedef struct
{
	U32                     u32StSize;                         /**< �ṹ�峤�� */
	S8                      s8AddrCode[MAX_ADDR_LEN];          /**< �豸��ַ���� */
	S8                      s8DevName[MAX_NAME_LEN];           /**< �豸����  */
	S32                     s32DecoderTag;                     /**< ��������ǩ */
	S32                     s32Longitude;                      /**< ���� */
	S32                     s32Latitude;                       /**< γ�� */
}RESOURCE_RSP_S;

typedef struct
{
	U32                     u32UserKey;                        /**< ���ӷ�����û���Կ */
}DTALK_REQ_S;

typedef struct
{
	U8                      u8ReqType;                         /**< ����ʽ��0-IP+���� 1-��ַ�뷽ʽ */
	U8                      u8TalkType;                        /**< �Խ���ʽ��0-��Ƶ�Խ� 1-��Ƶ�Խ� 2-����Ƶ�Խ� 3-��Ƶ���� 4-��Ƶ���� 5-����Ƶ���� */
	S8                      s8LocalName[MAX_NAME_LEN];         /**< �������� */
	U8                      u8LocalIp[MAX_IPV4_LEN];           /**< ����IP */
	U32                     u32UserSize;                       /**< �û����ݳ��� */
}DTALK_MSG_S;

typedef struct
{
	union
	{
		S8                  s8UserAddrCode[MAX_ADDR_LEN];      /**< �Է��Խ��û���ַ���� */
		struct
		{
			S8              s8Name[MAX_NAME_LEN];              /**< �Է����� */
			U8              u8Ip[MAX_IPV4_LEN];                /**< �Է�IP */
		};
	};
}DTALK_USER_S;

typedef struct
{
    U8                      u8DevIp[MAX_IPV4_LEN];           /**< �豸ip */
	U16                     u16DevPort;                      /**< �豸port */
	U8                      u8ChnNo;                         /**< ͨ���� */
    U8                      u8Res[1];
}DTALK_DEV_S; 

typedef struct
{
	S8                      s8FrontAddrCode[MAX_ADDR_LEN];     /**< ǰ���豸��ַ���� */
	
	U32                     u32ChnNO;                          /**< ͨ���� 0,1,2,3... */
	U8                      u8DeviceIp[MAX_IPV4_LEN];          /**< �豸IP��ַ */
	U16                     u16DevicePort;                     /**< �豸�˿ں� */
	
	U8                      u8SubServIp[MAX_IPV4_LEN];         /**< ������ý���������ַ */
	U16                     u16SubServPort;                    /**< ������ý��˿ں� */
	BOOL                    bStart;                            /**< ������ֹͣ¼�� */
}CONTROL_RECORD_REQ_S;

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
}DV_PARAMSET_S;

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
}DV_PARAMSET_S_EX;

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
	U8                      u8AreaType;                        /**< �������� */
	U8                      u8Res;
	U32                     u32PresetType;                     /**< Ԥ��λ���� */
	S8                      s8Desc[MAX_OBJECTNAME_LEN];        /**< ������� */
}DV_PARAMSET_S_EX_0815;

typedef struct
{
	U32                     u32StSize; 
	U32                     u32Id;	                 	       /**< Ѳ��·��ID��Ѳ�Ӳ���ID*/
	U32                     u32Type;			               /**< Ѳ�Ӷ������ͣ�����UT_TOUR_TYPE*/
	S8                      s8Name[MAX_OBJECTNAME_LEN];	       /**< Ѳ��·�����ƻ�*Ѳ���豸���� */
	S32                     u32No;	                 	       /**< Ѳ�Ӳ������ */
	S32                     s32DevId;                          /**< Ѳ���豸��� */
	U32                     u32ParentId;                       /**< �޸��ڵ���Ϊ0*/
	U8	                    u8Hour;                            /**< Ѳ��ʱ��*/
	U8	                    u8Minute;
	U8	                    u8Second;             
	
	U8                      u8Reseverd[1];                            
}TL_PARAMSET_S;

typedef struct
{
	U32                     u32UserKey;
	U8                      u8ReqType;                          /**< ����ʽ��0-IP+�˿ڷ�ʽ 1-��ַ�뷽ʽ */
	
	union
	{
		S8                  s8FrontAddrCode[MAX_ADDR_LEN];      /**< ǰ���豸��ַ���� */
		struct
		{
			U32             u32ChnNO;                           /**< ͨ���� 0,1,2,3... */
			U8              u8DeviceIp[MAX_IPV4_LEN];           /**< �豸IP��ַ */
			U16             u16DevicePort;                      /**< �豸�˿ں� */
			
			U8              u8SubServIp[MAX_IPV4_LEN];          /**< ������ý���������ַ */
			U16             u16SubServPort;                     /**< ������ý��˿ں� */
		};
	};
}GET_PRESET_REQ_S;

typedef struct
{
	U32                     u32StSize;
	U32                     u32PresetNo;                        /**< Ԥ�õ�� */
	S8                      s8PresetName[MAX_NAME_LEN];         /**< Ԥ�õ����� */
}GET_PRESET_RSP_S;

typedef struct
{
	U32                     u32StSize;
	U32                     u32PresetNo;                        /**< Ԥ�õ�� */
	S8                      s8PresetName[MAX_NAME_LEN];         /**< Ԥ�õ����� */
	U32                     u32PresetType;                      /**< Ԥ��λ���� */
}GET_PRESET_RSP_S_EX;  /* �¹�Լ */

/* ���ܷ��� */
typedef struct
{
	U32             u32ChnNO;                           /**< ͨ���� 0,1,2,3... */
	U8              u8IvaIp[MAX_IPV4_LEN];              /**< ���ܷ���IP */
	U16             u16IvaPort;                         /**< ���ܷ���port */
	U8              u8DeviceIp[MAX_IPV4_LEN];           /**< �豸IP��ַ */
	U16             u16DevicePort;                      /**< �豸�˿ں� */
	U8              u8DevPreset;
	U8              u8Res[3];
}IVA_ANALYSE_PARAM;

typedef struct
{
	float          fxRatio;
	float          fyRatio;
}AI_THERM_POS;

typedef struct
{
	AI_THERM_POS    stFrom;
	AI_THERM_POS    stTo;
	S8              s8RuleName[MAX_NAME_LEN];
	U8              u8DevIp[MAX_IPV4_LEN];
	float           fEmissivity;    /* ������ */
	float           fDistance;      /* ���� */
	float           fReflexValue;   /* �����¶�*/
    U16             u16DevPort;	    /* �豸port*/
	U8              u8ChnNo;   
	U8              u8PresetNo;
	U8              u8RuleId;
	U8              u8Enable;
	U8              u8Reserved[2];
}TERM_RULE_INFO;

/* ������ʾģʽ(���졢�ʺ��) */
typedef struct
{
	U32             u32ChnNo;                          /**< ͨ���� 0,1,2,3... */
	U8              u8DevIp[MAX_IPV4_LEN];             /**< �豸ip */
	U16             u16DevPort;                        /**< �豸port */
	U8              u8TermMode;                        /**< ��ɫ����ʾģʽ */
	U8              u8Reserved;
}TERM_SHOWMODE_PARAM;

#endif
