#ifndef PUBLICDEF_H
#define PUBLICDEF_H

/*****************************************************************************/
//#define  CALLING_CONVENTION		__cdecl
#define  CALLING_CONVENTION
//#define  CALLING_CONVENTION		__stdcall
/*****************************************************************************/

/*****************************************************************************/
#define  MAX_PLAYER_NUM  128/*64*/
/*****************************************************************************/
#define  MSG_PLAYSDKM4  (WM_USER+0x01FF)	//回放器库消息的默认值

//SDK Return Value
#define TD_OK                   0
#define TD_APPLYMEMORY_FAILED  -5

/************************************************************************
*	TC_PutStreamToPlayer返回值说明                                                                     
************************************************************************/
#define		RET_BUFFER_FULL					(-11)		//	缓冲区满，数据没有放入缓冲区
#define		RET_BUFFER_WILL_BE_FULL			(-18)		//	即将满，降低送入数据的频率
#define		RET_BUFFER_WILL_BE_EMPTY		(-19)		//	即将空，提高送入数据的频率
#define     RET_BUFFER_IS_OK                (-20)       //  正常可以放数据

/*回放器库消息的LPARAM参数取值列表*/
#define  LPARAM_FILE_PLAY_END			0	//	播放完毕
#define  LPARAM_FILE_SEARCH_END			1	//  寻找到文件末尾
#define  LPARAM_NEED_DECRYPT_KEY		2	//	需要解密密码
#define  LPARAM_DECRYPT_KEY_FAILED		3	//	解密密码错误
#define  LPARAM_DECRYPT_SUCCESS			4	//	解密成功
#define  LPARAM_STREAM_SEARCH_END		5   //  流缓冲区中没有数据了
#define  LPARAM_VOD_STREAM_END			6   //  VOD文件流播放完毕


/************************************************************************
* Decrypt frame return value                                                                     
************************************************************************/
#define DECRYPT_SUCCESS				0				//	解密成功
#define DECRYPT_INVALID_PARAM		(-1)			//	参数错误
#define DECRYPT_KEY_NOT_READY		(-2)			//	没有设定解密密钥
#define DECRYPT_FRAME_NOT_ENCRYPT	(-3)			//	帧不是加密帧
#define DECRYPT_FAILED				(-4)			//	帧解密失败
/*****************************************************************************/

#define MODERULE_AUTO_ADJUST_DRAW_YES					(0x01)		//	自适应不同分辨率数据--是
#define MODERULE_AUTO_ADJUST_DRAW_NO					(0x00)		//	自适应不同分辨率数据--否
#define MODERULE_AUTO_ADJUST_STREAM_PLAY_SPEED_YES		(0x11)		//	自适应调整流模式的播放速率--是
#define MODERULE_AUTO_ADJUST_STREAM_PLAY_SPEED_NO		(0x10)		//	自适应调整流模式的播放速率--否
#define MODERULE_STREAM_PLAY_NO_DELAY					(0x22)		//	设置为无延时模式
#define MODERULE_STREAM_PLAY_HIGH_SMOOTH				(0x21)		//	设置为高流畅模式
#define MODERULE_STREAM_PLAY_LOW_DELAY					(0x20)		//	设置为低延时模式

/*****************************************************************************/
#define GET_USERDATA_INFO_MIN                            (0)                           
#define GET_USERDATA_INFO_TIME							 (GET_USERDATA_INFO_MIN )		//获得用户数据中的时间信息
#define GET_USERDATA_INFO_OSD							 (GET_USERDATA_INFO_MIN +1)		//获得用户数据中的OSD信息
#define GET_USERDATA_INFO_GPS                            (GET_USERDATA_INFO_MIN +2)		//获得用户数据中的GPS信息
#define GET_USERDATA_INFO_VCA                            (GET_USERDATA_INFO_MIN +3)		//获得用户数据中的VCA信息
#define GET_USERDATA_INFO_USER_DEFINE                    (GET_USERDATA_INFO_MIN +4)		//获得用户数据中的VCA信息
#define GET_USERDATA_INFO_MAX                            (GET_USERDATA_INFO_MIN +4)	
/*****************************************************************************/
typedef struct SDK_VERSION
{    
	unsigned short    m_ulMajorVersion;
	unsigned short    m_ulMinorVersion;
	unsigned short    m_ulBuilder;
	char*             m_cVerInfo;					//	"X.X.X.XXX"
}*PSDK_VERSION;

typedef struct tagRECTEx
{
	double    left;
	double    top;
	double    right;
	double    bottom;
}RECTEX, *PRECTEX, NEAR *NPRECTEX, FAR *LPRECTEX;
//typedef const RECTEX FAR* LPRECTEX;

//////////////////////////////////////////////////////////////////////////
/***********************  供上层应用程序导出使用，多屏显示 **************/
#define MAX_MONITOR_DESCLEN 512
typedef struct MONITOR_INFO					
{
	char        cDriverDesc[MAX_MONITOR_DESCLEN];
	char        cDriverName[MAX_MONITOR_DESCLEN];	
	RECT		rect;
}*PMONITOR_INFO;                                                                     

//////////////////////////////////////////////////////////////////
/*****************     录像文件中储存的智能分析信息   ***************/
#define MAX_SAVE_TARGET_NUM  16

#define VCAINFO_SEARCHRULE_MIN				0
#define VCAINFO_SEARCHRULE_FRAME			(VCAINFO_SEARCHRULE_MIN+0)
#define VCAINFO_SEARCHRULE_POLYGON			(VCAINFO_SEARCHRULE_MIN+1)
#define VCAINFO_SEARCHRULE_TRIPWIRE			(VCAINFO_SEARCHRULE_MIN+2)
#define VCAINFO_SEARCHRULE_MAX				(VCAINFO_SEARCHRULE_MIN+3)

#define VCA_MAX_POLYGON_POINT_NUM		32				//	每个多边形所允许的最多点个数
#define VCA_MAX_POLYGON_NUM				25/*8*/				//	每条VCA规则所允许的最多的多边形个数

struct vca_TRect
{
	unsigned short usLeft;
	unsigned short usTop;
	unsigned short usRight;
	unsigned short usBottom;
};

struct vca_TTargetInfo
{
	vca_TRect		rect;
	unsigned short	usType;
	unsigned short	usVelocity;
	unsigned short	usDirection;
	unsigned int	id;	
};

struct vca_TTargetSet
{
	unsigned short		usCount;
	vca_TTargetInfo		targets[MAX_SAVE_TARGET_NUM]; 
};

struct vca_TTotalInfo
{
	unsigned short		usType;
	unsigned short		usLength;
	vca_TTargetSet		targetSet;
};


typedef struct DecAVInfo
{
    int iType;
    int iFormat;
    int iWidth;
    int iHeight;
    const unsigned char *pucData;
    int iDataLen;
}*PDecAVInfo;

typedef struct DecAVInfoEx
{
	DecAVInfo	avInfo;

	int			iOtherInfoSize;		//	根据OtherInfoSize判断后面的具体信息结构体, iOtherInfoSize = N*sizeof(int) ,N代表pOtherInfo包含信息个数
	void*		pOtherInfo;	        //unsigned int pOtherInfo[] 数组。目前定义 pOtherInfo[0]：时间戳， pOtherInfo[1]：帧率，pOtherInfo[2]：音频波形AudioSampleValue结构体指针

}*PDecAVInfoEx;

typedef struct AudioSampleValue
{ 
	int iChannel;             //声道数                  
	int iMaxSampleValue1;     //声道1最大采样值  取值介于-300~300之间
	int iMinSampleValue1;	  //声道1最小采样值
	int iIsMaxBeforeMin1;     //标示声道1的采样最大值出现在采样最小值以前
	int iMaxSampleValue2;     //声道2最大采样值
	int iMinSampleValue2;	  //声道2最小采样值
	int iIsMaxBeforeMin2;	  //标示声道2的采样最大值出现在采样最小值以前
}*PAudioSampleValue;

typedef struct PSPACK_INFO
{
	unsigned long nWidth;
	unsigned long nHeight;
	unsigned long nStamp;
	unsigned long nType;
	unsigned long nFrameRate;
	unsigned long nReserved;
}*PPSPACK_INFO;

typedef struct
{
	unsigned long nWidth;    //Video width, audio data is 0；
	unsigned long nHeight;   //Video height, audio data is 0；
	unsigned long nStamp;    //Time stamp(ms)。
	unsigned long nType;     //Audio type，T_AUDIO8,T_YUV420，。
	unsigned long nFrameRate;//Frame rate。
	unsigned long nReserved; //reserve
}TD_FRAME_INFO;

typedef struct __tagExtraInfo
{
	int		m_iInfoLen;
	void*	m_pvExtInfo;	
}FRAME_EXT_INFO, *LPFRAME_EXT_INFO;

//解码后的音视频数据
typedef void (*DECYUV_NOTIFY_V4)(unsigned int _ulID,unsigned char *_cData, int _iLen, const TD_FRAME_INFO *_pFrameInfo, int _iUser);

/*****************************************************************************/
typedef void (CALLING_CONVENTION *pfCBPlayEnd)(int _iID);
typedef void (CALLING_CONVENTION *pfCBGetDecAV)(int _iID, const DecAVInfo* _pDecAVInfo);
typedef void (CALLING_CONVENTION *pfCBVideoEdit)(int _iNotifyCode);
typedef void (CALLING_CONVENTION *pfCBDrawFun)(long _lHandle,HDC _hDc,long _lUserData);

typedef int (CALLING_CONVENTION* pfCBCommonEventNotify)(int _iPlayerID, int _iEventMessage);
typedef int (CALLING_CONVENTION* pfCBCommonEventNotifyEx)(int _iPlayerID, int _iEventMessage, void* _lpUserData);
typedef int (CALLING_CONVENTION* PSPACK_NOTIFY)(int _iPlayerID, unsigned char* _ucData, int _iSize, PSPACK_INFO* _pPsPackInfo, void* _pContext);

typedef void (__stdcall *pfAudioCaptureCbk)(unsigned char* _pucData, int _iLen, void* _pvUserData);
typedef void (__stdcall *pfRawFrameCbk)(int _iPlayerID, unsigned char* _pucData, int _iLen, void* _pvUserData);

//音频采集，用于对讲
typedef int (*RECORD_VOICE_NOTIFY)(unsigned char *_pucAuidoBuffer, int _iAudioLen);
/********************************************************************************/

/************************************************************************/
/* Capture File                                                                     */
/************************************************************************/
#define REC_FILE_TYPE_NORMAL	0
#define REC_FILE_TYPE_AVI		1
#define REC_FILE_TYPE_ASF		2
#define REC_FILE_TYPE_AUDIO     3
#define REC_FILE_TYPE_WAV       4
#define REC_FILE_TYPE_MP3       5

/**************************************************************************/

//按需创建播放器的类型标志
typedef enum __tagPlayModeCmd
{
	E_FILE_ONLYDECODE	= 0,	//文件模式下仅解码
	E_STREAM_ONLYDECODE	= 1,	//流模式下仅解码
	E_YUV_PLAYER        = 2,    //YUV播放器
}PlayModeCmd, *LPPlayModeCmd;

//Number of Show Video
#define MAX_DRAW_NUM	2

//#define T_AUDIO8					0
#define T_YUV420					1
#define T_YUV422					2



#define		PLAYMODE_FILE			0			//	文件播放模式
#define		PLAYMODE_VOD_FILE		1			//	VOD播放模式
#define		PLAYMODE_STREAM			2			//	流播放模式

struct FILE_PLAYER
{
	char* m_pcFileName;		/*文件名*/
	int   m_iDownloadBufSize;    /*下载缓冲区大小*/
	int   m_iFileTrueSize;    /*文件大小*/
	int*  m_piNowFileSize; /*当前文件大小*/
	int   m_iLastFrmNo;    /*最后一帧帧序号*/
	int*  m_piCompleteFlag;  /*是否结束接收*/
};

struct STREAM_PLAYER
{
	char m_VideoHeader[88];   /*视频头*/
	int m_iStreamBufferSize;  /*拼帧缓存区大小 传0为默认*/
};

typedef struct __tagTStreamData
{
	int  iSize;
	char cStreamData[64];
	int   iStreamLen;
} TStreamData, *PTStreamData;

typedef struct __tagPlayerHeader
{
	int  iSize;
	HWND hwnd;   //显示句柄
	int iWidth;
	int iHeight;
} TPlayerHeader, *PTPlayerHeader;

/************************************************************************/
/* VOD播放控制码                                                        */
/************************************************************************/
#define CTRL_VOD_SETOVERFLAG	0			//设置播放结束标志
#define CTRL_VOD_CLEANBUFF		1			//清空VOD BUFFER缓冲区

/************************************************************************/

/************************************************************************/
/* 返回值错误码含义                                                     */
/************************************************************************/
#define RET_INVALID_PLAYERID		-1			//非法PLAYER ID
#define RET_ERR_PLAYMODE			-2			//错误的播放模式
#define RET_INVALID_PARAM			-3			//非法参数

/************************************************************************/

/************************************************************************/
/* 刷新功能控制码                                                       */
/************************************************************************/
#define CTRL_REFRESHPLAYER		0			//初始化PLAYER窗口为黑色背景



#endif