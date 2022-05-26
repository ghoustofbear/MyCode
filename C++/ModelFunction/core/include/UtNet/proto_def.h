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
#define ACK_SUCCESS               0         /**< 成功 */
#define ACK_FAIL                  1         /**< 失败 */
#define ACK_NO_DEFINE             2         /**< 消息未定义 */

#define ACK_LOGIN_HAS_LOGINED     10        /**< 已经登录 */
#define ACK_LOGIN_NO_USERNAME     11        /**< 用户名不存在 */
#define ACK_LOGIN_AUTH_FAIL       12        /**< 认证失败 */
#define ACK_LOGIN_PERMDENY        13        /**< 没有权限 */
#define ACK_LOGIN_AAA_AUTH_FAIL   14        /**< AAA认证失败 */

#define ACK_OPT_PERMDENY          21        /**< 没有操作权限 */
#define ACK_OPT_DOING             22        /**< 高优先级的正在操作 */
#define ACK_OPT_INVALID           23        /**< 非法操作 */
#define ACK_OPT_NO_DEVICE         24        /**< 设备不存在 */

#define ACK_DTALK_OK              31        /**< 对讲请求成功 */
#define ACK_DTALK_NAK             32        /**< 对讲请求拒绝 */
#define ACK_DTALK_NO_ACK          33        /**< 对讲请求没响应 */

/* MSG_HEADER_S.u16MsgType*/
#define MSG_SEARCH_DEV            11 

#define MSG_LOGIN                 21        /**< 登录 */
#define MSG_HEARTBEAT             22        /**< 心跳 */

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
#define MSG_PTZ_ZOOMIN            5109         /**< 焦距变大 */ 
#define MSG_PTZ_ZOOMOUT           5110         /**< 焦距变小 */
#define MSG_PTZ_FOCUSNEAR         5111         /**< 焦点前调 */
#define MSG_PTZ_FOCUSFAR          5112         /**< 焦点后调 */
#define MSG_PTZ_IRISOPEN          5113         /**< 光圈扩大 */
#define MSG_PTZ_IRISCLOSE         5114         /**< 光圈缩小 */
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
	U8  u2Version : 2;  /**< 消息头版本 */
	U8  u6ExtSize : 6;  /**< 消息头扩展大小，实际扩展大小 = ExtSize * 4 */
	U8  u8Reserved;
	U16 u16AckFlag;     /**< 消息回应码 */
	U16 u16MsgType;     /**< 消息类型 */
	U16 u16MsgSubType;  /**< 消息子类型 */
	U32 u32MsgSize;     /**< 消息头后面的数据大小 */
	U32 u32Seq;         /**< 消息序列号，标识一次消息的唯一性，由请求方创建，回应方原值返回 */

	bool operator==(const _MSG_HEADER_S_ &head) const 
	{  
		return u2Version     == head.u2Version
			&& u16MsgType    == head.u16MsgType 
			&& u16MsgSubType == head.u16MsgSubType
			&& u32Seq        == head.u32Seq;
	}
}MSG_HEADER_S;          /**< 消息头，sizeof(MsgHead) = 16 */

/**
 * 登录
 * 请求 MsgType=MsgLogin, MsgSize=sizeof(LoginReq)
 */

typedef struct
{
	S8   s8Username[MAX_NAME_LEN];
	S8   s8Password[MAX_NAME_LEN];
	S8   s8Name[MAX_NAME_LEN];             /**< 客户端名称 */
	U8   u8ClientIp[MAX_IPV4_LEN];         /**< 客户端IP */
	U16  u16ClientTalkPort;                /**< 客户端开放的对讲端口*/
	BOOL bAlarmDesired;
	U8   u8Reserved[5];
} LOGIN_REQ_S;

/* 用户权限各位显示 */
typedef struct
{
	U32 u32MngRightBit;      /**< 管理权限位, 0-基本设置 1-站点管理 2-设备管理 3-权限组管理 4-用户管理
							   5-设备状态 6-用户状态 7-系统状态 8-系统日志 9-报警日志
							   10-设备日志 11-用户日志 */
	U32 u32OptRightBit;      /**< 操作权限位, 0-预览 1-回放 2-下载 3-云台 4-报警 */
	U32 u32PVChannel;        /**< 预览通道 */
	U32 u32PBChannel;        /**< 回放通道 */
	U32 u32PtzChannel;       /**< 云台控制通道 */
	U32 u32AlarmChannel;     /**< 报警上报通道 */
	U8  u8Res1[8];           
	U8  u8MngLevel;          /**< 控制权限等级 */
	U8  u8OptLevel;          /**< 控制权限等级 */
	U8  u8Res2[6];
} USER_RIGHT_S;
C_ASSERT(sizeof(USER_RIGHT_S) == 40);

typedef struct _LOGIN_RSP_S_
{
	S8  s8HWVerison[MAX_NAME_LEN];    /**< 硬件版本 */
	S8  s8IEVerison[MAX_NAME_LEN];    /**< IE版本 */
	S8  s8SWVersion[MAX_NAME_LEN];    /**< 软件版本 */
	S8  s8SystemName[MAX_NAME_LEN];   /**< 系统名称, 可以修改 */
	S8  s8AddrCode[MAX_ADDR_LEN];     /**< 地址编码 */
	S8  s8MacAddr[MAX_NAME_LEN];      /**< 设备MAC地址 */

	U32 u32UserKey;                   /**< 连接分配的用户密钥 */
	USER_RIGHT_S stUserRight;         /**< 用户权限 */

	U32  u32DeviceNum;                /**< 通道数 */
	U32  u32ChnInfoSize;

	U8  u8Reserved2[20];
}LOGIN_RSP_S;
C_ASSERT(sizeof(LOGIN_RSP_S) == 276);


typedef struct
{
	S8         s8DevName[MAX_NAME_LEN];   /**< 设备名称 */
	S8         s8AddrCode[MAX_ADDR_LEN];  /**< 设备地址编码 */
	U8         u8IPAddr[4];

	U16        u16Port;
	U16        u16DevType;                /**< 设备类型 */ 
	U8         u8ChannelNum;              /**< 通道数 */

	U8         u8ManuType;                /**< 设备厂商类型 */ 
	U8         u8AlarmInNum;              /**< 报警输入数 */ 
	U8         u8AlarmOutNum;             /**< 报警输出数(继电器个数) */
	U8         u8IsOnline;                /**< 设备是否在线 */
	U8         u8Reserved[3];
}CHN_INFO_S;


/**< 用户列表项信息 */
typedef struct
{
	U32 u32StSize;                   /**< sizeof(USERINFO_RSP_S) */
	S8  s8Username[MAX_NAME_LEN];    /**< 用户名 */
	S8  s8Password[MAX_NAME_LEN];    /**< 密码 */
	USER_RIGHT_S stUserRight;        /**< 用户权限 */
}USERINFO_RSP_S;

typedef struct
{
	S8  s8AddrCode[MAX_ADDR_LEN];    /**< 设备地址编码 */
	S8  s8DeviceName[MAX_NAME_LEN];  /**< 设备名称, 可以修改 */

	U8  u8DeviceIp[MAX_IPV4_LEN];    /**< 设备IP */

	U16 u16DevicePort;               /**< 设备端口 */
}DEVINFO_REQ_S;

/* 设备列表项信息 */
typedef struct
{
	U32 u32StSize;                    /**< sizeof(DEVINFO_RSP_S) */
	
	S8  s8AddrCode[MAX_ADDR_LEN];     /**< 设备名称 */
	S8  s8DevName[MAX_NAME_LEN];      /**< 设备地址编码 */
	S8  s8PareAddrCode[MAX_ADDR_LEN]; /**< 父设备地址编码 */
	S8  s8DevDesc[100];               /**< 设备描叙 */
	
	S32 s32ChnNum;                    /**< 设备通道数 */
	S32 s32ChnNo;                     /**< 设备通道号 */
	U8  u8IPAddr[4];                  /**< 设备IP地址 */
	
	U16 u16Port;                      /**< 设备端口号 */
	U16 u16DevType;                   /**< 设备类型 */
	
	S8  s8ManuType;                   /**< 设备厂商类型 */
	S8  s8ProtoType;                  /**< 协议类型 */
	S8  s8StreamType;                 /**< 码流类型 */ 
} DEVINFO_RSP_S;

/**
 *  包
 */

/* PACK_HEADER_S.u2Version */
#define PackHeadV1      0
#define PackHeadV2      1

#define MaxPackSize         (8*1024)
#define MaxPackSize_2       (8*1024)
#define MaxPackSizeV1       (8*1024)

/* PACK_HEADER_S.u8Type */
#define FramePack               0 /**< 帧拆分的包 */    
#define ReposPack               1 /**< 回放时重定位包，代表后续发送的是重定位后的数据 */
#define NoDataPack              2 /**< 回放时表示当前回放时间点后面已经没有录像数据 */
#define FileEndPack             3 /**< 录像下载时表示一个文件传输结束 */

typedef struct _PACK_HEADER_S_
{
	U8  u2Version : 2;  /**< 包头版本 */    
	U8  u6ExtSize : 6;  /**< 包头扩展大小，实际扩展大小 = ExtSize * 4 */
	U8  u8Type;                                                     
	U16 u16PackNum;                                                 
	U32 u32FrameNo;                                                 
	U16 u16PackNo;      /**< 0,1,2,3... */
	U16 u16PackSize;                                                
} PACK_HEADER_S;        /**< 包头, sizeof(PackHead) = 12 */

typedef struct _PACKAGE_S_
{  
	PACK_HEADER_S stHead;
	S8            s8Data[MaxPackSize];
}PACKAGE_S;

/**
 *  预览
 */

/* PVIEW_PARAM_S.u8Type */
#define PreviewMain	0
#define PreviewSub	1

/* PVIEW_PARAM_S.u8Content */
#define PreviewComposite	0
#define PreviewVideoOnly	1
typedef struct _PVIEW_PARAM_S_
{
	U8  u8Type;                               /**< 0-主码流, 1-从码流 */
	U8  u8Content;                            /**< 复合流或视频流 */
	U8  u8ReqType;                            /**< 请求方式，0-IP+端口方式 1-地址码方式 */
	U8  u8Reserved[1];

	union
	{
		S8  s8FrontAddrCode[MAX_ADDR_LEN];    /**< 前端设备地址编码 */
		struct
		{
			U32 u32ChnNO;                     /**< 通道号 0,1,2,3... */
			U8  u8DeviceIp[MAX_IPV4_LEN];     /**< 设备IP地址 */
			U16 u16DevicePort;                /**< 设备端口号 */

			U8  u8SubServIp[MAX_IPV4_LEN];    /**< 二级流媒体服务器地址 */
			U16 u16SubServPort;               /**< 二级流媒体端口号 */
		};
	};
}PVIEW_PARAM_S;


/**
 *  TCP预览
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
 *  UDP探测
 *  请求 MsgType=MsgGetUdpAddr
 *  回应 MsgSize=size(GetUdpAddrRsp)
 */
typedef struct _UDPADDR_RSP_S_
{
	SA_IN stCliAddr;
	S32   s32SockFd;
	U8    u8Addr[44];
} UDPADDR_RSP_S;

/**
 *  UDP开始预览
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
 *  UDP停止预览
 */

typedef struct _UDP_STOP_PVIEW_REQ_S_
{
	U32 u32UdpPViewKey;
}UDP_STOP_PVIEW_REQ_S;


/**
 *  请求I帧
 */
typedef struct _IFRAME_REQ_S
{
	U8  u8Type;                               /**< 主从码流,同预览 */
	U8  u8Reserved[2];

	U8  u8ReqType;                            /**< 请求方式，0-IP+端口方式 1-地址码方式 */
	union
	{
		S8  s8FrontAddrCode[MAX_ADDR_LEN];    /**< 前端设备地址编码 */
		struct
		{
			U32 u32ChnNO;                     /**< 通道号 0,1,2,3... */
			U8  u8DeviceIp[MAX_IPV4_LEN];     /**< 设备IP地址 */
			U16 u16DevicePort;                /**< 设备端口号 */

			U8  u8SubServIp[MAX_IPV4_LEN];    /**< 二级流媒体服务器地址 */
			U16 u16SubServPort;               /**< 二级流媒体端口号 */
		};
	};
} IFRAME_REQ_S;

/**
 *  预览画面切换 
 */
typedef struct
{
	U32 u32Mode;                              /**< NET_SWITCH_MODE值 */
	U32 u32Channel;                           /**< 目标切换通道值, 一位对应一个通道 */

	U8  u8DeviceIp[MAX_IPV4_LEN];             /**< 设备IP地址 */
	U16 u16DevicePort;                        /**< 设备端口号 */

	U8  u8SubServIp[MAX_IPV4_LEN];            /**< 二级流媒体服务器地址 */
	U16 u16SubServPort;                       /**< 二级流媒体端口号 */
}SWITCH_REQ_S;

/**
 *  矩阵切换 
 */
typedef struct
{
	U32 u32InPort;                            /**< 矩阵输入端口号 */
	U32 u32OutPort;                           /**< 矩阵输出端口号 */

	U8  u8DeviceIp[MAX_IPV4_LEN];             /**< 设备IP地址 */
	U16 u16DevicePort;                        /**< 设备端口号 */

	U8  u8SubServIp[MAX_IPV4_LEN];            /**< 二级流媒体服务器地址 */
	U16 u16SubServPort;                       /**< 二级流媒体端口号 */
}MATRIX_REQ_S;

/**
 * 解码器画面切换 
 */
typedef struct
{
	U32 u32Mode;                              /**< 显示模式，NET_SHOW_MODE_E*/
	U32 u32Channel;                           /**< 需要解码显示的通道，一位对应一个通道 */
	U32 u32OutPort;                           /**< 显示输出端口 */

	U8  u8DeviceIp[MAX_IPV4_LEN];             /**< 设备IP地址 */
	U16 u16DevicePort;                        /**< 设备端口号 */

	U8  u8SubServIp[MAX_IPV4_LEN];            /**< 二级流媒体服务器地址 */
	U16 u16SubServPort;                       /**< 二级流媒体端口号 */
}DECODER_REQ_S;

typedef struct
{
	U32 u32Mode;                              /**< 显示模式，NET_SHOW_MODE_E*/
	U32 u32OutPort;                           /**< 显示输出端口 */

	U8  u8DeviceIp[MAX_IPV4_LEN];             /**< 设备IP地址 */
	U16 u16DevicePort;                        /**< 设备端口号 */

	U8  u8SubServIp[MAX_IPV4_LEN];            /**< 二级流媒体服务器地址 */
	U16 u16SubServPort;                       /**< 二级流媒体端口号 */
}DEC_DISPLAY_REQ_S;

/* 流媒体服务器基本配置 */
typedef struct
{
	S8   s8Url[MAX_URL_LEN];                  /**< 流媒体服务器地址 */
	U16  u16Port;	                          /**< 流媒体服务器端口 */

	S8  s8Username[MAX_NAME_LEN];             /**< 登录用户名 */
	S8  s8Password[MAX_NAME_LEN];             /**< 登录用密码 */

	U32  u32TransProtocol;	                  /**< 传输协议类型 0-TCP，1-UDP*/
}DEC_STREAM_CFG_S;

/* DVR设备通道信息 */
typedef struct
{
	U8  u8Ip[MAX_IPV4_LEN];                   /**< DVR IP地址 */
	U16 u16Port;                              /**< DVR 端口号 */

	S8  s8Username[MAX_NAME_LEN];             /**< DVR 登录用户名 */
	S8  s8Password[MAX_NAME_LEN];             /**< DVR 登录用密码 */

	U32 u32ChnNo;                             /**< DVR 通道号, 从0开始 */
	U32 u32TransProtocol;		              /**< 传输协议类型0-TCP，1-UDP */
	U32 u32TransMode;			              /**< 传输码流模式 0－主码流 1－子码流 */
	U32 u32ProtoType;			              /**< 前端设协议类型 */
}DEC_DVR_CFG_S;

typedef struct
{
	S32 s32DecChnNo;                          /**< 解码通道号, 从0开始 */
	S32 s32DisOutPort;                        /**< < 显示输出通道 */ 
	S32 s32DisWndNo;                          /**< 显示通道所在窗口号, 从0开始 */

	BOOL bValidStream;                        /**< 流媒体服务器是否可用 */
	union
	{
		DEC_DVR_CFG_S    stDvrCfg;
		DEC_STREAM_CFG_S stStreamCfg;         
	}stDecChnCfg;                             /**< 解码通道视频配置信息 */

	U8  u8DeviceIp[MAX_IPV4_LEN];             /**< 设备IP地址 */
	U16 u16DevicePort;                        /**< 设备端口号 */

	U8  u8SubServIp[MAX_IPV4_LEN];            /**< 二级流媒体服务器地址 */
	U16 u16SubServPort;                       /**< 二级流媒体端口号 */
}DEC_DYNAMIC_REQ_S;

typedef struct
{
	S32  s32DisOutPort;                        /**< < 显示输出通道 */ 
	U8   u8DeviceIp[MAX_IPV4_LEN];             /**< 设备IP地址 */
	U16  u16DevicePort;                        /**< 设备端口号 */
	
	U8   u8SubServIp[MAX_IPV4_LEN];            /**< 二级流媒体服务器地址 */
	U16  u16SubServPort;                       /**< 二级流媒体端口号 */
}DEC_DYNAMIC_STOP_REQ_S;

/* 
 * 录像查询 
 */

/* 录像类型 */
#define RECORD_ALL		0xFFFFFFFF            /**< Only for req, equal to RECORD_NORMAL|RECORD_ALARM... */

typedef struct _QUERY_RECORD_REQ_S_
{
	U8  u8ReqType;                            /**< 请求方式，0-IP+端口方式 1-地址码方式 */
	U8  u8Reserved[3];

	U32        u32Type;
	DATETIME_S stBegin;
	DATETIME_S stEnd;

	union
	{
		S8  s8FrontAddrCode[MAX_ADDR_LEN];    /**< 前端设备地址编码 */
		struct
		{
			U32        u32ChnNO;              /**< 通道号 0,1,2,3... */
			U8  u8DeviceIp[MAX_IPV4_LEN];     /**< 设备IP地址 */
			U16 u16DevicePort;                /**< 设备端口号 */

			U8  u8SubServIp[MAX_IPV4_LEN];    /**< 二级流媒体服务器地址 */
			U16 u16SubServPort;               /**< 二级流媒体端口号 */
		};
	};
}QUERY_RECORD_REQ_S;

typedef struct _QUERY_RECORD_REQ_EX_S_
{
	U8  u8ReqType;                            /**< 请求方式，0-IP+端口方式 1-地址码方式 */
	U8  u8Reserved[3];

	U32        u32Type;
	DATETIME_S stBegin;
	DATETIME_S stEnd;

	U32        u32TriggerType;
	U32        u32TriggerPort;

	union
	{
		S8  s8FrontAddrCode[MAX_ADDR_LEN];    /**< 前端设备地址编码 */
		struct
		{
			U32        u32ChnNO;              /**< 通道号 0,1,2,3... */
			U8  u8DeviceIp[MAX_IPV4_LEN];     /**< 设备IP地址 */
			U16 u16DevicePort;                /**< 设备端口号 */

			U8  u8SubServIp[MAX_IPV4_LEN];    /**< 二级流媒体服务器地址 */
			U16 u16SubServPort;               /**< 二级流媒体端口号 */
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
 * 录像下载
 * 请求 MsgType=MSG_DOWNLOAD, MsgSize=sizeof(REC_DOWNLOAD_REQ_S)+n*sizeof(REC_DETAIL_RSP_S)
 * 回应 成功AckFlag=ACK_SUCCESS 失败AckFlag=ACK_FAIL
 * 传输数据: file1,FileEndPack; file2,FileEndPack; ... filen,FileEndPack
 */
typedef struct
{
	U32        u32UserKey;
	U8         u8DownloadMode;                                 /**< 下载方式，1:私有模式 2:标准模式 */
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
 * 录像回放
 * 
 * 开始回放
 * 请求 MsgType=MsgRecordPlay, MsgSize=sizeof(RecordPlayReq)
 * 回应 成功AckFlag=ACK_SUCCESS, 失败AckFlag=ACK_FAIL, 回应后不发送数据,统一在收到MsgRecordDataInsufficient后发送
 *
 */
enum 
{
	REC_PLAY_IP_TIME = 0,                                      /**<  0-IP+PORT+时间段回放 */
	REC_PLAY_IP_NAME,                                          /**<  1-IP_PORT+文件回放 */
	REC_PLAY_ADDR_URL,                                         /**<  2-地址码+RTSP URL回放 */
	REC_PLAY_ADDR_TIME,                                        /**<  3-地址码+时间回放 */
	REC_PLAY_HIKPLATFORM,                                      /**< 海康平台回放 */
};                                                             
                                                               
typedef struct _REC_PLAY_REQ_S_
{
	U32                        u32UserKey;                     /**< 用户码 */
	U8                         u8PlayMode;                     /**< 请求方式, REC_PLAY_IP_TIME ... */
	U8                         u8Reserved[3];

	union
	{
		struct
		{
			S8                 s8FrontAddrCode[MAX_ADDR_LEN];  /**< 前端设备地址编码 */
			union
			{
				S8             s8RstpUrl[MAX_FILENAME_LEN];    /**< RTSP 路径 */
				struct
				{
					DATETIME_S stBegin;                        /**< 开始时间 */
					DATETIME_S stEnd;                          /**< 结束时间 */
				};
			};
		}stAddrMode;

		struct
		{
			union
			{
				S8             s8FileName[MAX_FILENAME_LEN];   /**< 录像文件名 */
				struct
				{
					U32        u32ChnNO;                       /**< 通道号 0,1,2,3... */
					U32        u32Type;
					DATETIME_S stBegin;                        /**< 开始时间 */
					DATETIME_S stEnd;                          /**< 结束时间 */
				};
			};
			U8                 u8DeviceIp[MAX_IPV4_LEN];       /**< 设备IP地址 */
			U16                u16DevicePort;                  /**< 设备端口号 */

			U8                 u8SubServIp[MAX_IPV4_LEN];      /**< 二级流媒体服务器地址 */
			U16                u16SubServPort;                 /**< 二级流媒体端口号 */
		}stIpMode;

		struct  
		{
			S8             s8FileName[MAX_FILENAME_LEN];   /**< 录像文件名 */
			DATETIME_S     stBegin;                        /**< 开始时间 */
			DATETIME_S     stEnd;                          /**< 结束时间 */
		}stHikPlatMode;	
	};

	U8                         u8ExactFrame[40];
}REC_PLAY_REQ_S;

/**
 * 回放反馈
 * 请求 MsgType=MsgRecordPlayFeedback, MsgSubType=MsgRecordDataInsufficient,MsgRecordDataEnough
 * 不需要回应
 * 
 * 重定位录像数据
 * 请求 MsgType=MsgReposRecordData, MsgSize=sizeof(ReposRecordDataReq)
 * 不需要回应,收到此消息后,即刻停止发送之前的数据(不过必须是发送完一个完整的包之后),
 * 接着发送ReposPack,最后是发送新的数据
 */
typedef struct
{
	DATETIME_S stBegin;
	DATETIME_S stEnd;
}REC_REPOSE_REQ_S;

/**
 * 云台控制
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
	U8                        u8ReqType;                       /**< 请求方式，0-IP+端口方式 1-地址码方式 */

	union
	{
		S8                    s8FrontAddrCode[MAX_ADDR_LEN];   /**< 前端设备地址编码 */
		struct
		{
			U32               u32ChnNO;                        /**< 通道号 0,1,2,3... */
			U8                u8DeviceIp[MAX_IPV4_LEN];        /**< 设备IP地址 */
			U16               u16DevicePort;                   /**< 设备端口号 */

			U8                u8SubServIp[MAX_IPV4_LEN];       /**< 二级流媒体服务器地址 */
			U16               u16SubServPort;                  /**< 二级流媒体端口号 */
		};
	};

	union
	{
		PTZ_CTRL_NORMAL_S     stNormal;                        /**< MsgPTZUp...MsgPTZAutoScan */
		PTZ_CTRL_CRUISE_S     stCruise;                        /**< MsgPTZCruise */
		PTZ_CTRL_PRESET_S     stPreset;                        /**< MsgPTZSetPreset...MsgPTZCallPreset */
	};
}PTZ_CTRL_REQ_S;

/* 参数配置 */
typedef struct _CHN_PARAM_REQ_S_
{
	U32                       u32StSize;
	U32                       u32ChnMask;
	S8                        s8AddrCode[MAX_ADDR_LEN];
}CHN_PARAM_REQ_S;

/**
 * 报警上报
 */
typedef struct{
	S8                        s8DeviceName[MAX_NAME_LEN];      /**< 报警设备名 */
	U8                        u8DeviceIp[MAX_IPV4_LEN];        /**< 报警设备IP */

	U16                       u16DevicePort;                   /**< 报警设备端口 */
	U8                        u8Res[2];
}ALARMER_S;

typedef struct
{
	DATETIME_S                stTime;                          /**< 报警产生时间 */
	U8                        u8ChnNo;                         /**< 报警通道号 */
	U8                        u8Status;                        /**< 报警状态，0-开始 1-结束 */
}ALARM_IO_RPT_S;

typedef struct
{
	DATETIME_S                stTime;                          /**< 报警产生时间 */
	U8                        u8ChnNo;                         /**< 报警通道号 */
	U8                        u8Status;                        /**< 报警状态，0-开始 1-结束 */
}ALARM_MD_RPT_S;

typedef struct
{
	DATETIME_S                stTime;                          /**< 报警产生时间 */
	U8                        u8ChnNo;                         /**< 报警通道号 */
	U8                        u8Status;                        /**< 报警状态，0-开始 1-结束 */
}ALARM_VLOSS_RPT_S;

typedef struct
{
	DATETIME_S                stTime;                          /**< 报警产生时间 */
	U8                        u8HddNo;                         /**< 报警硬盘号 */
	U8                        u8Status;                        /**< 报警状态，0-开始 1-结束 */
}ALARM_HDD_RPT_S;

enum 
{
	AI_TYPE_TRIPWIRE  = 4,                                     /**< 拌线 */ 
	AI_TYPE_PERIMETER,                                         /**< 周界 */ 
	AI_TYPE_ITEMLEFT,                                          /**< 物品遗留丢失 */
	AI_TYPE_PLATE,                                             /**< 车牌检测 */
	AI_TYPE_HOVER,                                             /**< 徘徊检测  */
	AI_TYPE_ANTITHEFT,                                         /**< 防盗检测   */
	AI_TYPE_OFFJOB,                                            /**< 脱岗检测   */
	AI_TYPE_MAINTENAREA,                                       /**< 检修区域检测 */
	AI_TYPE_PRESSPLATE,                                        /**< 压板投退状态检测 */ 
	AI_TYPE_INDICATOR,                                         /**< 分合指示牌检测 */
	AI_TYPE_DISCONNECTOR,                                      /**< 隔离开关(触头)状态检测 */
	AI_TYPE_LIGHT,                                             /**< 指示灯状态检测  */
	AI_TYPE_HELMET,                                            /**< 安全帽检测 */   
    AI_TYPE_OCRTEXT,                                          /**< 文本识别 */
	AI_TYPE_GNDWIRE,                                          /**< 接地线识别 */
	AI_TYPE_SAFETYRULE,                                       /**< 安规事件 */  
    AI_TYPE_TOWERLEAN,                                        /**< 构架倾斜识别 */
	AI_TYPE_SNAPMATCH,                                        /**< 人脸比对识别 */
	AI_TYPE_ELECTRICFENCE,                                    /** < 电子围栏 */
	AI_TYPE_GATE,                                             /**< 道闸状态 */
	AI_TYPE_JUEYUAN,                                           /**< 外观检测-绝缘子 */
	AI_TYPE_GANTA,											   /**< 外观检测-杆塔 */
	AI_TYPE_THERMOMETRYALARM,                                  /**< 红外温度告警 */
	AI_TYPE_PTZ_ALARM,                                        /**< 云台控制告警 */
	AI_YX_COUNT,
	AI_TYPE_LEDDIGIT  = 30,                                    /**< 表计读数检测(七段数码管) */
	AI_TYPE_METER,                                             /**< 表计读数检测(刻度盘) */
    AI_TYPE_NUMPEOPLE,                                         /**< 人数统计 */
	AI_TYPE_THERMOMETRY,                                       /**< 测温 */
	AI_TYPE_AREAPEOPLE,                                       /**< 区域人数统计 */
	AI_TYPE_PASSFLOW,                                        /**< 客流量人数统计 */
};

enum
{
    PLATE_TIANDY_STANDARD = 1,                                 /**< 天地车牌识别机(标准) */
	PLATE_DAHUA_STARNDARD,                                     /**< 大华车牌识别机(标准) */
	PLATE_HIK_STARNDARD,                                       /**< 海康车牌识别机(标准) */
	PLATE_HIK_NORMAL,                                          /**< 海康车牌识别机(普通) */
};

typedef struct
{
	S8                       s8Plate[12];                      /**< 车牌号 */
	U8                       u8PlateType;                      /**< 车牌类型 */
	U8                       u8PlateColor;                     /**< 车牌颜色 */
	U8                       u8CarColor;                       /**< 车辆颜色 */
	U8                       u8DevType;                        /**< 设备类型 如:PLATE_TIANDY_STANDARD*/
	U16                      u16Millsecond;                    /**< 抓拍毫秒 */
	U8                       u8CarType;                        /**< 车辆类型 1:小汽车 2:卡车 3:面包车 0:其它*/
	U8                       u8Direct;                         /**< 进出类型 1:进 2:出 0:未知*/
	U32                      u32PicNo;                         /**< 天地伟业ftp抓拍图片名称序号 */
}AI_PLATE_S;

/**< 智能分析点坐标 */
typedef struct
{
	U16                      u16Px;
	U16                      u16Py;
}AI_POINT_S;

/**< 智能分析矩形报警 */
typedef struct
{
    AI_POINT_S   stFrom;                                       /**< 矩形左上点 */
	AI_POINT_S   stTo;                                         /**< 矩形右下点 */
	U8          u8Enable;                                      /**< 是否有效 */
	U8          u8Alarm;                                       /**< 是否报警 */
	U8          u8Reserved[2];
}AI_RECTANGLE_S;

/**< 智能分析报警颜色 */
enum AI_COLOR_S
{
	AI_COLOR_BLACK = 0,                                        /**< 黑色 */
	AI_COLOR_GRAY,                                             /**< 灰色 */
	AI_COLOR_WHITE,                                            /**< 白色 */
	AI_COLOR_RED,                                              /**< 红色 */
	AI_COLOR_YELLOW,                                           /**< 黄色 */
	AI_COLOR_GREEN,                                            /**< 绿色 */
	AI_COLOR_BLUE,                                             /**< 蓝色 */
	AI_COLOR_PURPLE,                                           /**< 紫色 */
	AI_COLOR_DARKGRAY,                                         /**< 深灰*/
    AI_COLOR_BKAIR,                                            /**< 民航黑色(海康) */
	AI_COLOR_SILVER,                                           /**< 银色 */
	AI_COLOR_DARKBLUE,                                         /**< 深蓝 */
	AI_COLOR_BROWN,                                            /**< 棕色 */
	AI_COLOR_PINK,                                             /**< 粉色 */
	AI_COLOR_CYAN,                                             /**< 青色 */
    AI_COLOR_ORANGE_RED,                                       /**< 橙红 */
	AI_COLOR_ORGANE,                                           /**< 橙色 */
    AI_COLOR_LIGHT_BLUE,                                       /**< 淡蓝 */
	AI_COLOR_LIGHT_YELLOW,                                     /**< 淡黄 */
	AI_COLOR_LIGHT_CYAN,                                       /**< 淡青 */
	AI_COLOR_LIGHT_GREEN,                                      /**< 淡绿 */
	AI_COLOR_LIGHT_PURPLE,                                     /**< 淡紫 */
	AI_COLOR_YELLOW_GREEN,                                     /**< 黄绿 */
	AI_COLOR_GOLDEN,                                           /**< 金色 */
	AI_COLOR_BLUE_GRAY,                                        /**< 蓝灰 */
	AI_COLOR_GREEN_GRAY,                                       /**< 绿灰 */
	AI_COLOR_CREAM,                                            /**< 米色 */
	AI_COLOR_CYAN_GREEN,                                       /**< 青绿 */
	AI_COLOR_DARK_RED,                                         /**< 深红 */
	AI_COLOR_SKY_BLUE,                                         /**< 天蓝 */
	AI_COLOR_SILVER_GRAY,                                      /**< 银灰 */
	AI_COLOR_PURPLE_RED,                                       /**< 紫红 */
	AI_COLOR_BROWN_RED,                                        /**< 棕红 */
	AI_COLOR_BROWN_YELLOW,                                     /**< 棕黄 */
	AI_COLOR_FLESH,                                            /**< 肉色 */
	AI_COLOR_UNKNOWN = 100,
};

/**< 拌线坐标定义 */
typedef struct
{
    AI_POINT_S               stFrom;                           /**< 起始点   */
	AI_POINT_S               stTo;                             /**< 终点     */

	U8                       u8Direct ;                        /**< 是否双向 */
	U8                       u8Enable ;                        /**< 是否有效 */	
	U8                       u8Alarm;                          /**< 是否报警 */
	U8                       u8Reserved[1];
}AI_TRIPWIRE_S;


typedef struct
{
	union
	{
		struct
		{
			AI_POINT_S       stFrom;                           /**< 矩形左上点 */
			AI_POINT_S       stTo;                             /**< 矩形右下点 */
		}stAiRect;

		AI_POINT_S           stPts[10];                        /**< 多边形点序列(最多10个点) */
	};

	U8                       u8Type;                           /**< 区域类型 1-矩形、2-多边形*/
	U8                       u8Enable;                         /**< 是否有效 */
	U8                       u8Alarm;                          /**< 是否报警 */
	U8                       u8Reserved[1];
}AI_PERIMETER_S;

typedef struct
{
	union
	{
		struct
		{
			AI_POINT_S       stFrom;                           /**< 矩形左上点 */
			AI_POINT_S       stTo;                             /**< 矩形右下点 */
		}stAiRect;

		AI_POINT_S           stPts[10];                        /**< 多边形点序列(最多10个点) */
	};

	U8                       u8Type;                           /**< 区域类型 1-矩形、2-多边形*/
	U8                       u8Enable;                         /**< 是否有效 */
	U8                       u8Alarm;                          /**< 是否报警 */
	U8                       u8Reserved[1];
}AI_ITEMLEFT_S;

typedef struct
{
	union
	{
		struct
		{
			AI_POINT_S       stFrom;                           /**< 矩形左上点 */
			AI_POINT_S       stTo;                             /**< 矩形右下点 */
		}stAiRect;

		AI_POINT_S           stPts[10];                        /**< 多边形点序列(最多10个点) */
	};

	U8                       u8Type;                           /**< 区域类型 1-矩形、2-多边形*/
	U8                       u8Enable;                         /**< 是否有效 */
	U8                       u8Alarm;                          /**< 是否报警 */
	U8                       u8Reserved[1];
}AI_HOVER_S;

typedef struct
{
	union
	{
		struct
		{
			AI_POINT_S       stFrom;                           /**< 矩形左上点 */
			AI_POINT_S       stTo;                             /**< 矩形右下点 */
		}stAiRect;

		AI_POINT_S           stPts[10];                        /**< 多边形点序列(最多10个点) */
	};

	U8                       u8Type;                           /**< 区域类型 1-矩形、2-多边形*/
	U8                       u8Enable;                         /**< 是否有效 */
	U8                       u8Alarm;                          /**< 是否报警 */
	U8                       u8Reserved[1];
}AI_ANTITHEFT_S;

typedef struct
{
	union
	{
		struct
		{
			AI_POINT_S       stFrom;                           /**< 矩形左上点 */
			AI_POINT_S       stTo;                             /**< 矩形右下点 */
		}stAiRect;
		
		AI_POINT_S           stPts[10];                        /**< 多边形点序列(最多10个点) */
	};
	
	U8                       u8Type;                           /**< 区域类型 1-矩形、2-多边形*/
	U8                       u8Enable;                         /**< 是否有效 */
	U8                       u8Alarm;                          /**< 是否报警 */
	U8                       u8Reserved[1];
}AI_OFFJOB_S;

typedef struct
{
	union
	{
		struct
		{
			AI_POINT_S       stFrom;                           /**< 矩形左上点 */
			AI_POINT_S       stTo;                             /**< 矩形右下点 */
		}stAiRect;
		
		AI_POINT_S           stPts[10];                        /**< 多边形点序列(最多10个点) */
	};
	
	U8                       u8Type;                           /**< 区域类型 1-矩形、2-多边形*/
	U8                       u8Enable;                         /**< 是否有效 */
	U8                       u8Alarm;                          /**< 是否报警 */
	U8                       u8Reserved[1];
}AI_MAINTENAREA_S;

typedef struct
{
	AI_POINT_S               stFrom;                           /**< 矩形左上点 */
	AI_POINT_S               stTo;                             /**< 矩形右下点 */
	AI_COLOR_S               enAiColor;                        /**< 颜色 */
	U8                       u8OnOff;                          /**< 亮灭或开关状态 */
	U8                       u8Enable;                         /**< 是否有效 */
	U8                       u8Reserved[1];
}AI_LIGHT_S;

typedef struct
{
	AI_POINT_S               stPts[3];                         /**< 中点，始末点*/
	U8                       u8DialType;                       /**< 刻度盘类型  */
	U8                       u8Enable;                         /**< 是否有效 */
	U8                       u8Reserved[2];	
	   
	FLOAT                    fDialValue;                       /**< 刻度盘当前值 */
}AI_METER_S;

typedef struct
{
    AI_POINT_S               stFrom;                           /**< 起始点   */
	AI_POINT_S               stTo;                             /**< 终点     */
	U8                       u8Enable;                         /**< 是否有效 */
	U8                       u8Alarm;                          /**< 是否报警 */
	U8                       u8Reserved[2];
}AI_INDICATOR_S;

typedef struct  
{
    AI_POINT_S               stFrom;                           /**< 起始点   */
	AI_POINT_S               stTo;                             /**< 终点     */

	U8                       u8Enable;                         /**< 是否有效 */
	U8                       u8Alarm;                          /**< 是否报警 */
	FLOAT                    fValue;                           /**< 表记数值 */
	U8                       u8DecNum;                         /**< 小数位数 */
	U8                       u8Reserved[3];
}AI_LEDDIGIT_S;

typedef struct
{
	U8						s8IsChange[MAX_OBJ_PRESSPLATE];/** < 判断是否改变,1改变，0不变*/
	S8						s8CurState[MAX_OBJ_PRESSPLATE];   /**< 当前状态 0(压板退出状态),1(压板投入状态) */
	U8						u8Enable[MAX_OBJ_PRESSPLATE];     /**< 当前状态是否有效*/
	U8						u8PressPlateNum;             /**< 压板数目 */
	U8						u8Reserved[3];
}AI_PRESSPLATE_S;

typedef struct
{
	AI_POINT_S        stFrom;                                  /**< 起始点   */
    AI_POINT_S        stTo;                                    /**< 终点     */

	U8                u8Alarm;                                 /**< 报警标记 0:未报警 1:报警 */
	U8                u8Enable;
	U8                u8Reserved[2];                           /**< 预留 */
}AI_DISCONNECTOR_S;

typedef struct
{
	AI_RECTANGLE_S    stMovPosition;                           /**< 移动轮廓矩形框 */      
	U8                u8Alarm;                                 /**< 报警标记 0:未报警 1:报警 */
	U8                u8Enable;
	U8                u8Reserved[2];                           /**< 预留 */
}AI_HELMET_S;

typedef struct
{
	AI_POINT_S        stFrom;                                  /**< 起始点   */
	AI_POINT_S        stTo;                                    /**< 终点     */

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
	DATETIME_S               stTime;                           /**< 报警产生时间 */

	U8                       u8AiType;                         /**< 智能分析类型, 1-拌线 2-周界 */
	U8                       u8ChnNo;                          /**< 智能分析通道号 */
	U8                       u8Preset;                         /**< 智能分析通道预置位 */
	U8                       u8AreaType;                       /**< 智能分析目标区域类型 LINE:0, RECT:1, POLY:2 */

    U16                      u16PicHeight;	                   /**< 智能分析图像分辨率高度 */
	U16                      u16PicWidth;                      /**< 智能分析图像分辨率宽度 */
	U32                      s32Status;                        /**< 0:告警结束, 1:告警开始 */ 
 //   U8                       u8AlarmType;                    /**< 报警类型 1:报警 0:轮廓 */	

	union
	{

        AI_TRIPWIRE_S        AiTripwite[MAX_AIOBJ_NUM];          /**< 拌线报警 */
		AI_PERIMETER_S       AiPerimeter[MAX_AIOBJ_NUM];         /**< 周界报警 */
        AI_ITEMLEFT_S        AiItemleft[MAX_AIOBJ_NUM];          /**< 物品遗留 */
		AI_PLATE_S           AiPlate;                            /**< 车牌信息 */
		AI_HOVER_S           AiHover[MAX_AIOBJ_NUM];             /**< 徘徊检测  */
	    AI_ANTITHEFT_S       AiAntitheft[MAX_AIOBJ_NUM];         /**< 防盗检测   */
	    AI_OFFJOB_S          AiOffjob[MAX_AIOBJ_NUM];            /**< 脱岗检测   */
	    AI_MAINTENAREA_S     AiMaintenarea[MAX_AIOBJ_NUM];       /**< 检修区域检测 */
	    AI_PRESSPLATE_S      AiPressplate;                       /**< 压板投退状态检测 */ 
	    AI_INDICATOR_S       AiIndicator[MAX_AIOBJ_NUM];         /**< 分合指示牌检测 */
	    AI_DISCONNECTOR_S    AiDisconnector[MAX_AIOBJ_NUM];      /**< 隔离开关(触头)状态检测 */
	    AI_LIGHT_S           AiLight[MAX_AIOBJ_NUM];             /**< 指示灯状态检测  */
	    AI_LEDDIGIT_S        AiLeddigit[MAX_AIOBJ_NUM];          /**< 表计读数检测(七段数码管) */
	    AI_METER_S           AiMeter[MAX_AIOBJ_NUM];             /**< 表计读数检测(刻度盘) */
	    AI_HELMET_S          AiHelmet[MAX_AIOBJ_NUM][20];        /**< 安全帽检测 */
		AI_NUMPEOPLE_S       AiNumPeople[MAX_AIOBJ_NUM];         /**< 人数统计 */
		AI_THERMOMETRY_S     AiThermometry[MAX_AIOBJ_NUM];       /**< 测温 */
		AI_OCRTEXT_S         AiOcrText[MAX_AIOBJ_NUM];           /**< 文本识别 */
        AI_GNDWIRE_S         AiGndWire[MAX_AIOBJ_NUM];           /**< 接地线识别 */
		AI_SAFETYRULE_S      AiSafetyRule[MAX_AIOBJ_NUM];        /**< 安规事件 */
        AI_AREAPEOPLE_S      AiAreaPeople[MAX_AIOBJ_NUM];        /**< 区域人数统计 */
		AI_TOWERLEAN_S       AiTowerLean[MAX_AIOBJ_NUM];         /**< 架构倾斜识别 */
		AI_SNAPMATCH_S       AiSnapMatch;                        /**< 人脸比对识别 */
		AI_PASSFLOW_S        AiPassFlow;                         /**< 客流量人数统计 */
		AI_ELECTRICFENCE_S   AiEleCtriCfence[MAX_AIOBJ_NUM];     /**< 电子围栏 */
		AI_GATE_S			 AiGate[MAX_AIOBJ_NUM];		         /**< 道闸状态 */
		AI_THERMOMETRY_ALARM_S AiThermometryAlarm[MAX_AIOBJ_NUM];/**<红外温度告警*/		
		AI_JUEYUAN_S		 AiJueYuan[MAX_AIOBJ_NUM];		     /**< 外观检测-绝缘子 */
		AI_GANTA_S			 AiGanTa[MAX_AIOBJ_NUM];		     /**< 外观检测-杆塔 */
	};

	S8                      s8PicName[MAX_PATH];             /**< 智能分析抓拍图片文件名称 */

}ALARM_AI_RPT_S;

/**< 系统运行参数 */
typedef struct  
{
	time_t      stStartTime;                                   /**< 系统开启时间 */
	time_t      stRunTime;                                     /**< 系统运行时间 */
	U32         u32ClientNo;                                   /**< 客户端链接数 */
	U32         u32TranTaskNo;                                 /**< 转发任务数 */
	U32         u32CpuUsed;                                    /**< 系统使用cpu率 */
	U32         u32TranCpuUsed;                                /**< 转发任务使用cpu率 */
	U32         u32PhMemTotal;                                 /**< 物理内存总量 */
	U32         u32PhMemRemain;                                /**< 物理内存可用量 */
	U32         u32VirMemTotal;                                /**< 虚拟内存总量 */
	U32         u32VirMemRemain;                               /**< 虚拟内存可用 */
	FLOAT       fSendNetworkTotal;                             /**< 网络发送总量 */
	FLOAT       fSendNetworkRemain;                            /**< 网络可用量 */
	FLOAT       fRecvNetworkTotal;                             /**< 网络接收总量 */
	FLOAT       fRecvNetworkRemain;                            /**< 网络接收可用量 */
}ALARM_SYS_RPT_S;

/* 设备状态上传结构 */
typedef struct
{
	DATETIME_S              stTime;                            /**< 报警产生时间 */
	U8                      u8DeviceIp[MAX_IPV4_LEN];          /**< 报警设备IP */
	
	U16                     u16DevicePort;                     /**< 报警设备*/
	U8                      u8Status;                          /**< 报警状态，0-离线 1-在线 */
}ALARM_DEVSTATE_RPT_S;

typedef struct{
	S8                      s8TaskName[MAX_FILENAME_LEN];      /**< 联动任务名 */

	U8                      u8DeviceIp[MAX_IPV4_LEN];          /**< 联动设备IP */

	U8                      u8ChnNO;                           /**< 联动设备通道号 */
	U8                      u8PresetNo;                        /**< 联动设备预置点号 */
	U16                     u16TaskId;                         /**< 联动任务ID */
}LINKAGE_RPT_S;

/* 客户端联动转发推送信息 */
typedef struct{
    S8                      s8SrcName[MAX_NAME_LEN];           /**< 联动转发源(客户端)名称 */
	U8                      u8SrcIp[MAX_IPV4_LEN];             /**< 联动转发源(客户端)IP */
	S8                      s8Desc[MAX_FILENAME_LEN];          /**< 联动描述 */
	U32                     u32DeviceNum;                      /**< 联动设备个数, 1-16 */			
	U32                     u32CameraId[16];                   /**< 联动设备通道ID号 */
	U8                      u8PresetNo[16];                    /**< 联动设备预置点号 */
}LINKAGE_TRANS_RPT_S;

typedef struct
{
	DATETIME_S              stTime;                            /**< 报警产生时间 */
	U8                      u8ChnNo;                           /**< 报警通道号 */
	U8                      u8Status;                          /**< 报警状态，0-开始 1-结束 */
}ALARM_MANUAL_RPT_S;

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
}ALARM_GPS_RPT_S;

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
	S8                      s8CardImgUrl[MAX_NAME_LEN*2];      /**< 身份图像url */
	S8                      s8CarImgUrl[MAX_NAME_LEN*2];       /**< 车辆图像url */
	S8                      s8Time[MAX_NAME_LEN*2];            /**< 时间 */
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
	ALARM_CAMERA_STATUS camStatus[64];
}ALARM_CAMERA_RPT_S;

typedef struct
{
	U32                     u32StSize;                         /**< 结构体长度 */
	S8                      s8Name[MAX_NAME_LEN];
	S32                     s32LocalUserId;                    /**< 本地登录用户ID */
	S8                      s8IP[MAX_IP_STR_LEN];              /**< 远程客户的IP地址 */
	U16                     u16Port;                           /**< 远程客户的端口号 */
	U32                     u32ClientKey;                      /**< 分配客户关键码 */
	U32                     u32DTalkState;                     /**< 音视频对讲状态 */
} CLIENT_INFO_RSP_S;

typedef struct
{
	U32                     u32UserKey;                        /**< 连接分配的用户密钥 */
	DATETIME_S              stTime;                            /**< 同步时间值 */
	U8                      u8TimeZone;                        /**< 时区 */
	U8                      u8IsSetLoaclTime;                  /**< 是否设置本地时间 */
	U8                      u8Reserved[2];
}SYNCHR_TIME_REQ_S;

typedef struct
{
	U32                     u32StSize;                         /**< 结构体长度 */
	S8                      s8AddrCode[MAX_ADDR_LEN];          /**< 设备地址编码 */
	S8                      s8DevName[MAX_NAME_LEN];           /**< 设备名称  */
	S32                     s32DecoderTag;                     /**< 解码插件标签 */
	S32                     s32Longitude;                      /**< 经度 */
	S32                     s32Latitude;                       /**< 纬度 */
}RESOURCE_RSP_S;

typedef struct
{
	U32                     u32UserKey;                        /**< 连接分配的用户密钥 */
}DTALK_REQ_S;

typedef struct
{
	U8                      u8ReqType;                         /**< 请求方式，0-IP+名称 1-地址码方式 */
	U8                      u8TalkType;                        /**< 对讲方式，0-音频对讲 1-视频对讲 2-音视频对讲 3-音频喊话 4-视频喊话 5-音视频喊话 */
	S8                      s8LocalName[MAX_NAME_LEN];         /**< 本地名称 */
	U8                      u8LocalIp[MAX_IPV4_LEN];           /**< 本地IP */
	U32                     u32UserSize;                       /**< 用户数据长度 */
}DTALK_MSG_S;

typedef struct
{
	union
	{
		S8                  s8UserAddrCode[MAX_ADDR_LEN];      /**< 对方对讲用户地址编码 */
		struct
		{
			S8              s8Name[MAX_NAME_LEN];              /**< 对方名称 */
			U8              u8Ip[MAX_IPV4_LEN];                /**< 对方IP */
		};
	};
}DTALK_USER_S;

typedef struct
{
    U8                      u8DevIp[MAX_IPV4_LEN];           /**< 设备ip */
	U16                     u16DevPort;                      /**< 设备port */
	U8                      u8ChnNo;                         /**< 通道号 */
    U8                      u8Res[1];
}DTALK_DEV_S; 

typedef struct
{
	S8                      s8FrontAddrCode[MAX_ADDR_LEN];     /**< 前端设备地址编码 */
	
	U32                     u32ChnNO;                          /**< 通道号 0,1,2,3... */
	U8                      u8DeviceIp[MAX_IPV4_LEN];          /**< 设备IP地址 */
	U16                     u16DevicePort;                     /**< 设备端口号 */
	
	U8                      u8SubServIp[MAX_IPV4_LEN];         /**< 二级流媒体服务器地址 */
	U16                     u16SubServPort;                    /**< 二级流媒体端口号 */
	BOOL                    bStart;                            /**< 启动或停止录像 */
}CONTROL_RECORD_REQ_S;

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
}DV_PARAMSET_S;

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
}DV_PARAMSET_S_EX;

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
	U8                      u8AreaType;                        /**< 区域类型 */
	U8                      u8Res;
	U32                     u32PresetType;                     /**< 预置位类型 */
	S8                      s8Desc[MAX_OBJECTNAME_LEN];        /**< 间隔描述 */
}DV_PARAMSET_S_EX_0815;

typedef struct
{
	U32                     u32StSize; 
	U32                     u32Id;	                 	       /**< 巡视路线ID或巡视步骤ID*/
	U32                     u32Type;			               /**< 巡视对象类型，参照UT_TOUR_TYPE*/
	S8                      s8Name[MAX_OBJECTNAME_LEN];	       /**< 巡视路线名称或*巡视设备名称 */
	S32                     u32No;	                 	       /**< 巡视步骤序号 */
	S32                     s32DevId;                          /**< 巡视设备序号 */
	U32                     u32ParentId;                       /**< 无父节点则为0*/
	U8	                    u8Hour;                            /**< 巡视时间*/
	U8	                    u8Minute;
	U8	                    u8Second;             
	
	U8                      u8Reseverd[1];                            
}TL_PARAMSET_S;

typedef struct
{
	U32                     u32UserKey;
	U8                      u8ReqType;                          /**< 请求方式，0-IP+端口方式 1-地址码方式 */
	
	union
	{
		S8                  s8FrontAddrCode[MAX_ADDR_LEN];      /**< 前端设备地址编码 */
		struct
		{
			U32             u32ChnNO;                           /**< 通道号 0,1,2,3... */
			U8              u8DeviceIp[MAX_IPV4_LEN];           /**< 设备IP地址 */
			U16             u16DevicePort;                      /**< 设备端口号 */
			
			U8              u8SubServIp[MAX_IPV4_LEN];          /**< 二级流媒体服务器地址 */
			U16             u16SubServPort;                     /**< 二级流媒体端口号 */
		};
	};
}GET_PRESET_REQ_S;

typedef struct
{
	U32                     u32StSize;
	U32                     u32PresetNo;                        /**< 预置点号 */
	S8                      s8PresetName[MAX_NAME_LEN];         /**< 预置点名称 */
}GET_PRESET_RSP_S;

typedef struct
{
	U32                     u32StSize;
	U32                     u32PresetNo;                        /**< 预置点号 */
	S8                      s8PresetName[MAX_NAME_LEN];         /**< 预置点名称 */
	U32                     u32PresetType;                      /**< 预置位类型 */
}GET_PRESET_RSP_S_EX;  /* 新规约 */

/* 智能分析 */
typedef struct
{
	U32             u32ChnNO;                           /**< 通道号 0,1,2,3... */
	U8              u8IvaIp[MAX_IPV4_LEN];              /**< 智能分析IP */
	U16             u16IvaPort;                         /**< 智能分析port */
	U8              u8DeviceIp[MAX_IPV4_LEN];           /**< 设备IP地址 */
	U16             u16DevicePort;                      /**< 设备端口号 */
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
	float           fEmissivity;    /* 发射率 */
	float           fDistance;      /* 距离 */
	float           fReflexValue;   /* 反射温度*/
    U16             u16DevPort;	    /* 设备port*/
	U8              u8ChnNo;   
	U8              u8PresetNo;
	U8              u8RuleId;
	U8              u8Enable;
	U8              u8Reserved[2];
}TERM_RULE_INFO;

/* 测温显示模式(铁红、彩虹等) */
typedef struct
{
	U32             u32ChnNo;                          /**< 通道号 0,1,2,3... */
	U8              u8DevIp[MAX_IPV4_LEN];             /**< 设备ip */
	U16             u16DevPort;                        /**< 设备port */
	U8              u8TermMode;                        /**< 调色板显示模式 */
	U8              u8Reserved;
}TERM_SHOWMODE_PARAM;

#endif
