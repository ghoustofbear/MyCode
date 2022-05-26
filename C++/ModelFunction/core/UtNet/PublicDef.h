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
#define  MSG_PLAYSDKM4  (WM_USER+0x01FF)	//�ط�������Ϣ��Ĭ��ֵ

//SDK Return Value
#define TD_OK                   0
#define TD_APPLYMEMORY_FAILED  -5

/************************************************************************
*	TC_PutStreamToPlayer����ֵ˵��                                                                     
************************************************************************/
#define		RET_BUFFER_FULL					(-11)		//	��������������û�з��뻺����
#define		RET_BUFFER_WILL_BE_FULL			(-18)		//	�������������������ݵ�Ƶ��
#define		RET_BUFFER_WILL_BE_EMPTY		(-19)		//	�����գ�����������ݵ�Ƶ��
#define     RET_BUFFER_IS_OK                (-20)       //  �������Է�����

/*�ط�������Ϣ��LPARAM����ȡֵ�б�*/
#define  LPARAM_FILE_PLAY_END			0	//	�������
#define  LPARAM_FILE_SEARCH_END			1	//  Ѱ�ҵ��ļ�ĩβ
#define  LPARAM_NEED_DECRYPT_KEY		2	//	��Ҫ��������
#define  LPARAM_DECRYPT_KEY_FAILED		3	//	�����������
#define  LPARAM_DECRYPT_SUCCESS			4	//	���ܳɹ�
#define  LPARAM_STREAM_SEARCH_END		5   //  ����������û��������
#define  LPARAM_VOD_STREAM_END			6   //  VOD�ļ����������


/************************************************************************
* Decrypt frame return value                                                                     
************************************************************************/
#define DECRYPT_SUCCESS				0				//	���ܳɹ�
#define DECRYPT_INVALID_PARAM		(-1)			//	��������
#define DECRYPT_KEY_NOT_READY		(-2)			//	û���趨������Կ
#define DECRYPT_FRAME_NOT_ENCRYPT	(-3)			//	֡���Ǽ���֡
#define DECRYPT_FAILED				(-4)			//	֡����ʧ��
/*****************************************************************************/

#define MODERULE_AUTO_ADJUST_DRAW_YES					(0x01)		//	����Ӧ��ͬ�ֱ�������--��
#define MODERULE_AUTO_ADJUST_DRAW_NO					(0x00)		//	����Ӧ��ͬ�ֱ�������--��
#define MODERULE_AUTO_ADJUST_STREAM_PLAY_SPEED_YES		(0x11)		//	����Ӧ������ģʽ�Ĳ�������--��
#define MODERULE_AUTO_ADJUST_STREAM_PLAY_SPEED_NO		(0x10)		//	����Ӧ������ģʽ�Ĳ�������--��
#define MODERULE_STREAM_PLAY_NO_DELAY					(0x22)		//	����Ϊ����ʱģʽ
#define MODERULE_STREAM_PLAY_HIGH_SMOOTH				(0x21)		//	����Ϊ������ģʽ
#define MODERULE_STREAM_PLAY_LOW_DELAY					(0x20)		//	����Ϊ����ʱģʽ

/*****************************************************************************/
#define GET_USERDATA_INFO_MIN                            (0)                           
#define GET_USERDATA_INFO_TIME							 (GET_USERDATA_INFO_MIN )		//����û������е�ʱ����Ϣ
#define GET_USERDATA_INFO_OSD							 (GET_USERDATA_INFO_MIN +1)		//����û������е�OSD��Ϣ
#define GET_USERDATA_INFO_GPS                            (GET_USERDATA_INFO_MIN +2)		//����û������е�GPS��Ϣ
#define GET_USERDATA_INFO_VCA                            (GET_USERDATA_INFO_MIN +3)		//����û������е�VCA��Ϣ
#define GET_USERDATA_INFO_USER_DEFINE                    (GET_USERDATA_INFO_MIN +4)		//����û������е�VCA��Ϣ
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
/***********************  ���ϲ�Ӧ�ó��򵼳�ʹ�ã�������ʾ **************/
#define MAX_MONITOR_DESCLEN 512
typedef struct MONITOR_INFO					
{
	char        cDriverDesc[MAX_MONITOR_DESCLEN];
	char        cDriverName[MAX_MONITOR_DESCLEN];	
	RECT		rect;
}*PMONITOR_INFO;                                                                     

//////////////////////////////////////////////////////////////////
/*****************     ¼���ļ��д�������ܷ�����Ϣ   ***************/
#define MAX_SAVE_TARGET_NUM  16

#define VCAINFO_SEARCHRULE_MIN				0
#define VCAINFO_SEARCHRULE_FRAME			(VCAINFO_SEARCHRULE_MIN+0)
#define VCAINFO_SEARCHRULE_POLYGON			(VCAINFO_SEARCHRULE_MIN+1)
#define VCAINFO_SEARCHRULE_TRIPWIRE			(VCAINFO_SEARCHRULE_MIN+2)
#define VCAINFO_SEARCHRULE_MAX				(VCAINFO_SEARCHRULE_MIN+3)

#define VCA_MAX_POLYGON_POINT_NUM		32				//	ÿ���������������������
#define VCA_MAX_POLYGON_NUM				25/*8*/				//	ÿ��VCA��������������Ķ���θ���

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

	int			iOtherInfoSize;		//	����OtherInfoSize�жϺ���ľ�����Ϣ�ṹ��, iOtherInfoSize = N*sizeof(int) ,N����pOtherInfo������Ϣ����
	void*		pOtherInfo;	        //unsigned int pOtherInfo[] ���顣Ŀǰ���� pOtherInfo[0]��ʱ����� pOtherInfo[1]��֡�ʣ�pOtherInfo[2]����Ƶ����AudioSampleValue�ṹ��ָ��

}*PDecAVInfoEx;

typedef struct AudioSampleValue
{ 
	int iChannel;             //������                  
	int iMaxSampleValue1;     //����1������ֵ  ȡֵ����-300~300֮��
	int iMinSampleValue1;	  //����1��С����ֵ
	int iIsMaxBeforeMin1;     //��ʾ����1�Ĳ������ֵ�����ڲ�����Сֵ��ǰ
	int iMaxSampleValue2;     //����2������ֵ
	int iMinSampleValue2;	  //����2��С����ֵ
	int iIsMaxBeforeMin2;	  //��ʾ����2�Ĳ������ֵ�����ڲ�����Сֵ��ǰ
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
	unsigned long nWidth;    //Video width, audio data is 0��
	unsigned long nHeight;   //Video height, audio data is 0��
	unsigned long nStamp;    //Time stamp(ms)��
	unsigned long nType;     //Audio type��T_AUDIO8,T_YUV420����
	unsigned long nFrameRate;//Frame rate��
	unsigned long nReserved; //reserve
}TD_FRAME_INFO;

typedef struct __tagExtraInfo
{
	int		m_iInfoLen;
	void*	m_pvExtInfo;	
}FRAME_EXT_INFO, *LPFRAME_EXT_INFO;

//����������Ƶ����
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

//��Ƶ�ɼ������ڶԽ�
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

//���贴�������������ͱ�־
typedef enum __tagPlayModeCmd
{
	E_FILE_ONLYDECODE	= 0,	//�ļ�ģʽ�½�����
	E_STREAM_ONLYDECODE	= 1,	//��ģʽ�½�����
	E_YUV_PLAYER        = 2,    //YUV������
}PlayModeCmd, *LPPlayModeCmd;

//Number of Show Video
#define MAX_DRAW_NUM	2

//#define T_AUDIO8					0
#define T_YUV420					1
#define T_YUV422					2



#define		PLAYMODE_FILE			0			//	�ļ�����ģʽ
#define		PLAYMODE_VOD_FILE		1			//	VOD����ģʽ
#define		PLAYMODE_STREAM			2			//	������ģʽ

struct FILE_PLAYER
{
	char* m_pcFileName;		/*�ļ���*/
	int   m_iDownloadBufSize;    /*���ػ�������С*/
	int   m_iFileTrueSize;    /*�ļ���С*/
	int*  m_piNowFileSize; /*��ǰ�ļ���С*/
	int   m_iLastFrmNo;    /*���һ֡֡���*/
	int*  m_piCompleteFlag;  /*�Ƿ��������*/
};

struct STREAM_PLAYER
{
	char m_VideoHeader[88];   /*��Ƶͷ*/
	int m_iStreamBufferSize;  /*ƴ֡��������С ��0ΪĬ��*/
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
	HWND hwnd;   //��ʾ���
	int iWidth;
	int iHeight;
} TPlayerHeader, *PTPlayerHeader;

/************************************************************************/
/* VOD���ſ�����                                                        */
/************************************************************************/
#define CTRL_VOD_SETOVERFLAG	0			//���ò��Ž�����־
#define CTRL_VOD_CLEANBUFF		1			//���VOD BUFFER������

/************************************************************************/

/************************************************************************/
/* ����ֵ�����뺬��                                                     */
/************************************************************************/
#define RET_INVALID_PLAYERID		-1			//�Ƿ�PLAYER ID
#define RET_ERR_PLAYMODE			-2			//����Ĳ���ģʽ
#define RET_INVALID_PARAM			-3			//�Ƿ�����

/************************************************************************/

/************************************************************************/
/* ˢ�¹��ܿ�����                                                       */
/************************************************************************/
#define CTRL_REFRESHPLAYER		0			//��ʼ��PLAYER����Ϊ��ɫ����



#endif