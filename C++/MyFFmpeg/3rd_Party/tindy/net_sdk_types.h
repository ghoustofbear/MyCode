
#ifndef _NET_SDK_TYPES_H_
#define _NET_SDK_TYPES_H_

#ifndef LEN_2
#define LEN_2   2
#endif

#ifndef LEN_4
#define LEN_4   4
#endif

#ifndef LEN_6
#define LEN_6   6
#endif

#ifndef LEN_8
#define LEN_8   8
#endif


#ifndef LEN_10
#define LEN_10	10
#endif

#ifndef LEN_16
#define LEN_16  16
#endif

#ifndef LEN_17
#define LEN_17  17
#endif

#ifndef LEN_20
#define LEN_20  20
#endif

#ifndef LEN_24
#define LEN_24  24
#endif

#ifndef LEN_31
#define LEN_31  31
#endif

#ifndef LEN_32
#define LEN_32  32
#endif

#ifndef LEN_33
#define LEN_33  33
#endif

#ifndef LEN_36
#define LEN_36  36
#endif

#ifndef LEN_64
#define LEN_64  64
#endif

#ifndef LEN_65
#define LEN_65		65
#endif

#ifndef LEN_128
#define LEN_128 128
#endif

#ifndef LEN_250
#define LEN_250	250
#endif

#ifndef LEN_256
#define LEN_256 256
#endif


#ifndef LEN_255
#define LEN_255 255
#endif

#ifndef LEN_300
#define LEN_300 300
#endif

#ifndef LEN_512
#define LEN_512 512
#endif

#ifndef LEN_1024
#define LEN_1024 1024
#endif

#ifndef LEN_UUID
#define LEN_UUID 64
#endif

#ifndef MAX_SCHEDULE
#define MAX_SCHEDULE 8
#endif

#define LENGTH_IPV4					16	
#define LENGTH_IPV6					32
#define LENGTH_IPV6_V1				64
#define LENGTH_SUB_MASK				16
#define LENGTH_MAC_ADDR     		18

#define MAX_QUERY_PAGE_COUNT					20
#define MAX_QUERY_VCA_CONDITION					32
#define MAX_VCA_FILE_COUNT						5
#define MAX_VCA_ATTR_COUNT						32
#define MAX_QUERY_LIST_COUNT					32

#define MAX_IP_FILTER_NUM				48
#define MAX_IPV6_PACKAGE_FILTER_NUM		16

#ifndef MAIN_STREAM
#define MAIN_STREAM								0
#endif

#ifndef SUB_STREAM
#define SUB_STREAM								1
#endif

#define BOUNDAY_MAIN							10000
#define BOUNDAY_SUB								1000000

#define FACE_MAX_PAGE_COUNT						20
#define FACE_MAX_KEY_COUNT_OLD					33
#define FACE_MAX_KEY_COUNT						300
#define FACE_MAX_FEATURE_COUNT					10
#define FACE_MAX_RECT_POINT_COUNT				10

////1��ͨ�����ƣ��ı��� 2��ʱ������ 3:logo��ɫ 4�������ַ� 5:��ͨר�� 6����ͨ�ϳ�ͼƬ 7����������OSD
#define COMMON_OSD_TYPE_CHANNEL     1
#define COMMON_OSD_TYPE_DATETIME    2
#define COMMON_OSD_TYPE_LOGOCOLOR   3
#define COMMON_OSD_TYPE_ADDITION    4
#define COMMON_OSD_TYPE_ITS         5
#define COMMON_OSD_TYPE_ITSMIX      6
#define COMMON_OSD_TYPE_FACEOSD     7
#define MAX_COMMON_OSD_TYPE         8

typedef struct
{
	unsigned short 		iYear;   				/* Year */
	unsigned short 		iMonth;  				/* Month */
	unsigned short 		iDay;    				/* Day */
	unsigned short 		iHour;   				/* Hour */
	unsigned short 		iMinute; 				/* Minute */
	unsigned short 		iSecond; 				/* Second */
} NVS_FILE_TIME,*PNVS_FILE_TIME;

typedef struct tagQueryChanNo
{
	int 				iChanNo;
	int 				iStream;
}QueryChanNo, ChannelList,*PChannelList,*PQueryChanNo;


typedef struct tagNetFileQueryVca
{
	int					iSize;				//�ṹ���С				
	int					iChanCount;         //ͨ������
	int					iChanSize;			//�ṹ��QueryChanNo��С
	QueryChanNo*		pChanList;			//ͨ���б�
	int					iVcaCount;			//���ܷ����㷨����
	int					iVcaList[MAX_QUERY_LIST_COUNT];		//���ܷ����㷨�б�
	NVS_FILE_TIME		tBegTime;			//��ʼʱ��
	NVS_FILE_TIME		tEndTime;			//����ʱ��
	int					iPageCount;			//ҳ��С
	int					iPageNo;			//ҳ���
	int					iFileType;			//�ļ����ͣ� 0��ȫ����1��¼��2��ͼƬ
	int					iConditionCount;	//��ѯ��������, ���32�� ��ֵΪ 0 ʱ����ʾ��ѯȫ��
	char				cQueryCondition[MAX_QUERY_VCA_CONDITION][LEN_256];	
/*
   �����ܷ�������Ϊ9ʱ: 
	cQueryCondition[0]����ʾ�������� 0-������������1-���¼�������2-��ͼƬ����;
	cQueryCondition[1]��cQueryCondition[0]Ϊ0ʱ ��ʾ���䣬1-���꣬2-���꣬3-���꣬4-����;
	cQueryCondition[0]Ϊ1ʱ 1-������� 2-�����ȶ� 3-İ���� 4-Ƶ�� 5-ʱ��;
	cQueryCondition[2]��cQueryCondition[0]Ϊ0ʱ ��ʾ�Ա�1-�У�2-Ů��3-δ֪; cQueryCondition[0]Ϊ2ʱ ��ʾ���� 1-��ѯ 2-��ȡ���;
	cQueryCondition[3]��cQueryCondition[0]Ϊ0ʱ ��ʾ���壬1-���壬2-��������; cQueryCondition[0]Ϊ2ʱ ��ʾͼƬ���� 1-ָ���ļ� 2-����������ѡ���ļ�;
	cQueryCondition[4]��cQueryCondition[0]Ϊ0ʱ ��ʾ����; cQueryCondition[0]Ϊ2, cQueryCondition[2]Ϊ1ʱ cQueryCondition[3]Ϊ1ʱ ��ʾͼƬ����; cQueryCondition[0]Ϊ2, cQueryCondition[2]Ϊ1ʱ cQueryCondition[3]Ϊ2ʱ ������ͼƬId;
	cQueryCondition[5]��cQueryCondition[0]Ϊ0ʱ����ʾ���۾� 0-Ԥ����1-�����2-δ��� cQueryCondition[0]Ϊ2��cQueryCondition[2]Ϊ1ʱ ��ʾ���ƶ�
	cQueryCondition[6]��cQueryCondition[0]Ϊ0ʱ����ʾ������ 0-Ԥ����1-�����2-δ��� cQueryCondition[0]Ϊ2��cQueryCondition[2]Ϊ1ʱ ��ʾ����ʽ 0-��ʱ������ 1-�����ƶ�����
   ��iVcaType = 22ʱ
	cQueryCondition[0]:��ʾĿ�꣬0x7FFFFFFF-ȫ����1-�ˣ�2-������3-��
	cQueryCondition[1]:��ʾ����, 0x7FFFFFFF-ȫ����1-�ܽ�-���֣�2-�ܽ�-�뿪,3-����
   ��iVcaType = 23ʱ
	cQueryCondition[0]:��ʾ���ƺ���
   ��iVcaType = 30ʱ
	cQueryCondition[0]:��ʾ��ȫñ��ɫ 0x7FFFFFFF-ȫ��,1-��ɫ��2-��ɫ��3-��ɫ��4-��ɫ��5-����
	cQueryCondition[1]:��ʾ��ȫñδ�����0-Ԥ����1-�����2-δ���
  */
}NetFileQueryVca, *pNetFileQueryVca;


typedef struct tagVcaFileAttr
{
	int					iFileIndex;				//��ţ�����ͼʱ��ÿ��ͼ����һ����ţ���10001��ʼ���ɶ������У�顣
	char				cFileName[LEN_256];		//ͼƬ����
	int					iFileSize;				//ͼƬ����
	int					iFileType;				//ͼƬ���ͣ�1-Сͼ��2-��ͼ 
	int					iReserve;				//�����ֶ�
	char				cReserve[LEN_64];		//�����ֶ�
}VcaFileAttr, *pVcaFileAttr;

typedef struct __tagVcaTPoint
{
	int iX;
	int	iY;
} vca_TPoint;

typedef struct __tagVca_TLine
{
	vca_TPoint 	stStart;
	vca_TPoint 	stEnd;
} vca_TLine;						//	sizeof = 2*8 = 16

#ifndef VCA_MAX_POLYGON_POINT_NUMEX
#define VCA_MAX_POLYGON_POINT_NUMEX				32	//Max Tripwire Points Number
#endif
typedef struct __tagVca_TPolygonEx
{
	int 		iPointNum;
	vca_TPoint 	stPoints[VCA_MAX_POLYGON_POINT_NUMEX];   
} vca_TPolygonEx;

typedef struct __tagTPoint
{
	int iX;
	int	iY;
} TPoint;

typedef struct tagNetFileQueryVcaResult
{
	int					iSize;
	int					iChanNo;
	int					iFileAttrCount;
	VcaFileAttr			tFileAttr[MAX_VCA_FILE_COUNT];

	int					iTotal;
	int					iCurPageCount;
	int					iItemIndex;
	int					iFileType;
	int					iVcaType;
	NVS_FILE_TIME		tBegTime;
	NVS_FILE_TIME		tEndTime;
	
	int					iExAttrCount;
	char				cExAttr[MAX_VCA_ATTR_COUNT][LEN_256];
}NetFileQueryVcaResult, *pNetFileQueryVcaResult;

typedef struct tagIpcDevAttr
{
	int					iSize;
	int					iType;
	int					iParam;
}IpcDevAttr, *pIpcDevAttr;


typedef struct tagUniqueAlertAlarmStatClr
{
	int					iSize;
	int					iSceneId;
	int					iTypeEnable[LEN_32];	//Whether the current alert event needs to eliminate the state of the alarm
	int					iEventNo;
}UniqueAlertAlarmStatClr,*pUniqueAlertAlarmStatClr;

typedef struct tagITS_LightStatus
{
	int					iSize;
	int					iLaneNo;
	int					iLightType;
	int					iStatus;
}ITS_LightStatus, *pITS_LightStatus;

typedef struct tagNetBitRate
{
	int					iSize;
	int					iMaxBw;					//NIC maximum capacity bandwidth
	int					iMaxRxBw;				//maximum access bandwidth
	int					iMaxTxBw;				//maximum forwarding bandwidth
} NetBitRate, *pNetBitRate;

typedef struct tagSipLocalServer
{
	int					iSize;
	int					iServerPort;			//local server port,default 5060
	char				cServerID[LEN_64];		//local server ID
	int					iHeartBeatInterval;		//heart beat interval,Unit: s
	int					iHeartBeatTimes;		//heart beat times
} SipLocalServer, *pSipLocalServer;

#define MAX_MACFILTER_COUNT			32
#define MAC_FILTERTYPE_DISAUTH		0			//Disable MAC authentication
#define MAC_FILTERTYPE_DISACCESS	1			//Disable MAC access
#define MAC_FILTERTYPE_ALLOWACCESS	2			//Allow MAC access
typedef struct tagMacAddrFilter
{
	int					iSize;
	int					iFilterType;			//filter type:0-Disable MAC authentication; 1-Disable MAC access; 2-Allow MAC access
	int					iMacCount;				//mac count,supports up to 32 different MAC addresses
	char				cMacAddr[MAX_MACFILTER_COUNT][LEN_64];	//MAC address string
} MacAddrFilter, *pMacAddrFilter;


#define VPN_VIRNETCARD_TUN			0
#define VPN_VIRNETCARD_TAP			1

#define VPN_PROTOCOL_TCP			0
#define VPN_PROTOCOL_UDP			1

#define VPN_COMPRESS_DISABLE		0
#define VPN_COMPRESS_LZO			1
#define VPN_COMPRESS_LZ4V2			2

#define VPN_CERTIFICATE_AUTH		0
#define VPN_USERNAMEPWD_AUTH		1
#define VPN_CERT_NAMEPWD_AUTH		2

#define CERT_USAGE_DEFAULT			0
#define CERT_USAGE_CUSTOME			1
typedef struct tagVpnParam
{
	int					iSize;
	int					iEnable;				//Enable switch:0-disable openvpn, 1-enable openvpn
	char				cServerIp[LEN_64];		//VPN server address
	int					iServerPort;			//VPN server port
	int					iVirNetCardType;		//Virtual network card type:0-TUN, 1-TAP
	int					iProtoType;				//Transfer Protocol:0-TCP, 1-UDP
	int					iTlsEnable;				//Whether to enable ta.key attack defense:0-disable, 1-enable
	int					iCompress;				//Compression method:0-Not enabled; 1-LZO; 2-LZ4_V2
	int					iCipherType;			//Data encryption method
	int					iIdentifyType;			//verification method:0: Certificate authentication 1: User name password authentication 2: Certificate + username password
	char				cUserName[LEN_128];		//User name, valid when the authentication method is 1 or 2 and the length is <=32
	char				cPassword[LEN_128];		//The password is valid when the authentication method is 1 or 2. The password must be encrypted using the base64 algorithm. Password length <= 64 bytes
	int					iCertUsage;				//Certificate usage:0-factory default, 1-custom imported
} VpnParam, *pVpnParam;

typedef struct tagVpnLan
{
	int					iSize;
	int					iLanNo;					//Network card number
	int					iDhcpEnable;			//0-disable; 1-enable
	char				cIP[LEN_32];			//IPV4 IP address
	char 				cMask[LEN_32];			//IPV4 subnet mask
	char 				cGateway[LEN_32];		//IPV4 default gateway
	int					iMtu;					//Mtu value in the tunnel,range:500-65535
} VpnLan, *pVpnLan;

typedef struct tagVpnState
{
	int					iSize;
	int					iLanNo;
	int					iState;					//0: Not connected, 1:Connected
} VpnState, *pVpnState;

#define CHN_STATE_OFFLINE		0
#define CHN_STATE_ONLINE		1
#define CHNOFFLINE_RSN_USRPWDERR	1
#define CHNOFFLINE_RSN_IPBLOCKED	2			//IP is blocked
#define CHNOFFLINE_RSN_ACCTLOCK		3			//Account lockout
#define CHNOFFLINE_RSN_MAXCON		4			//Maximum number of connections
#define CHNOFFLINE_RSN_NETEXCP		5			//network anomaly
#define CHNOFFLINE_RSN_MAXBD		6			//Reach the NVR maximum bandwidth
#define CHNOFFLINE_RSN_CAPUPPER		7			//Performance cap
typedef struct tagChnOnlineState 
{
	int					iSize;
	int					iState;					//OnlineState :0-offline, 1-online
	int					iOffLineReason;			//the reason when the channel is offline
} ChnOnlineState, *pChnOnlineState;

typedef struct tagRetransInfo 
{
	int					iIndex;
	char				cIp[LEN_64];	
}RetransInfo, *pRetransInfo;

typedef struct tagITS_DevOffLine
{
	int					iSize;
	int					iDevType;
	int					iIOIndex;
}ITS_DevOffLine, *pITS_DevOffLine;
#define MAX_SDI_CUTAREA_POINT 15
typedef struct tagITSSDICUTAREA
{
	int iSize;
	int iSceneId;
	int iAreaType;
	int iEnabled;
	int iCount;
    vca_TPoint pcPoint[MAX_SDI_CUTAREA_POINT];
}ITS_SDICUTAREA,*pITS_SDICUTAREA;

#define MAX_REUPLOAD_COUNT 40
typedef struct tagITSREUPLOAD
{
	int iSize;
	int iPlatetype;
	int iReuploadNum;
	int iOrderID[MAX_REUPLOAD_COUNT];
}ITS_REUPLOAD, *pITS_REUPLOAD;

typedef struct tagITSREUPLOADRESULT
{
	int iSize;
	int iFailNum;
	int iOrderID[MAX_REUPLOAD_COUNT];
}ITS_REUPLOADRESULT, *pITS_REUPLOADRESULT;



typedef struct tagZFCmdResult
{
	int					iSize;
	int					iResult;
}ZFCmdResult,*pZFCmdResult;

typedef struct tagDelMultiFile
{
	int					iSize;
	int					iChanCount;
	int					iChanSize;
	QueryChanNo*		pChanList;
	int					iRecType;
	NVS_FILE_TIME		tBegTime;
	NVS_FILE_TIME		tEndTime;
}DelMultiFile,*pDelMultiFile;

typedef struct tagDelMultiFileProgress
{
	int					iSize;
	int					iProgress;
}DelMultiFileProgress,*pDelMultiFileProgress;

typedef struct tagBurnMount
{
	int					iSize;
	int					iDeviceNum;
	int					iCmd;
}BurnMount,*pBurnMount;

typedef struct tagBurnCheckDisk
{
	int					iSize;
	int					iType;
	int					iMode;
	int					iDvdNo;
}BurnCheckDisk,*pBurnCheckDisk;

typedef struct tagBurnBackupStart
{
	int					iSize;
	int					iMode;
	int					iDvdNo;
	int					iRecordType;
	int					iRecCode;
	int					iRecAccFile;
	int					iChanCount;
	int					iChanSize;
	QueryChanNo*		pChanList;
	int					iRecType;
	NVS_FILE_TIME		tBegTime;
	NVS_FILE_TIME		tEndTime;
	char				cDvdLable[LEN_64];
}BurnBackupStart,*pBurnBackupStart;

typedef struct tagBurnBackupState
{
	int					iSize;
	int					iDeviceNum;
	int					iState;
	int					iProgress;
	int					iTotalProgress;
}BurnBackupState,*pBurnBackupState;

typedef struct tagBurnBackupStop
{
	int					iSize;
}BurnBackupStop,*pBurnBackupStop;

typedef struct tagZFVolumeIn
{
	int					iSize;
	int					iChanNo;
	int					iType;
}ZFVolumeIn,*pZFVolumeIn;

typedef struct tagZFCameraType
{
	int					iSize;
	int					iType;
}ZFCameraType,*pZFCameraType;

typedef struct tagBurnBackupStateQuery
{
	int					iSize;
	int					iDvdNo;
}BurnBackupStateQuery,*pBurnBackupStateQuery;

typedef struct tagBurnBackupStateResult
{
	int					iSize;
	int					iResult;
	int					iBackupStat;
	int					iBurnProcess;
	int					iTotalBurnProcess;
	int					iBurnMode;
	int 				iDvdNum;
	int					iRecordType;
	int					iRecCode;
	int					iRecAccFile;
	int					iChanCount;
	QueryChanNo			tChannelList[LEN_32];
	int					iRecType;
	NVS_FILE_TIME		tBegTime;
	NVS_FILE_TIME		tEndTime;
	char				cDvdLable[LEN_64];
}BurnBackupStateResult, *pBurnBackupStateResult;

typedef struct tagSmdEncodeEnable
{
	int 				iEnable;
}SmdEncodeEnable, *pSmdEncodeEnable;

typedef struct tagSmdImageEnable
{
	int 				iEnable;
}SmdImageEnable, *pSmdImageEnable;

typedef struct tagSmdSceneEnable
{
	int 				iEnable;
}SmdSceneEnable, *pSmdSceneEnable;

typedef struct tagSmdAlarmArea
{
	int					iLeftMargin;
	int					iTopMargin;
	int					iRightMargin;
	int					iBottomMargin;
}SmdAlarmArea, *pSmdAlarmArea;

typedef struct tagVerificationCode
{
	char cVerificionCode[LEN_32];
}VerificationCode,*pVerificationCode;

#define MAX_VERSION_COUNT   3
typedef struct tagSnmpPara
{
	int                 iSize;
	int                 iEnable[MAX_VERSION_COUNT];
	int                 iPort;
	char                cReCommunity[LEN_33];
	char                cWrCommunity[LEN_33];
	char                cTrapAddr[LEN_16];
	int                 iTrapPort;
	char                cTrapCommunity[LEN_33];
	char                cReUser[LEN_33];
	int                 iReSecurity;
	int                 iReAuthAlg;
	char                cReAuthPwd[LEN_17];
	int                 iRePrivAlg;
	char                cRePrivPwd[LEN_17];
	char                cWrUser[LEN_33];
	int                 iWrSecurity;
	int                 iWrAuthAlg;
	char                cWrAuthPwd[LEN_17];
	int                 iWrPrivAlg;
	char                cWrPrivPwd[LEN_17];
}SnmpPara, *pSnmpPara;

//server type
#define SERVERTYPE_PRIVATE		(0)
#define SERVERTYPE_ONVIF		(1)
#define SERVERTYPE_PUSH			(2)
#define SERVERTYPE_RTSP			(3)
#define SERVERTYPE_28181GB		(4)
#define SERVERTYPE_LG			(1001)

typedef struct tagVCARule
{
	int					iSceneID;				//scene ID(0~15)
	int					iRuleID;				//rule ID(0~7)
	int					iValid;					//event valid, 0:invalid,1:valid
}VCARule,*pVCARule;

typedef struct __tagVca_TDisplayParam
{
	int					iDisplayRule;			//display Rule,0-display 1-Not display	
	int					iDisplayStat;			//alarm count display,0-display 1-Not display
	int					iColor;					//zone color,1-red 2-green 3-yellow 4-blue 5-Purple 6-cyan-blue 7-black 8-White,(defalit green)
	int					iAlarmColor;			//alarm zone color
}vca_TDisplayParam;	

typedef struct tagVCAColorTrack
{
	int					iSize;
	VCARule				tRule;
	vca_TDisplayParam	tDisplayParam;
	int					iDisplayTarget;
	int					iMaxSize;
	int					iMinSize;
	int					iCalibrationSize;
	int					iSensitiv;
	int					iTargetColor;
	int					iHue;
	int					iSaturation;
	int					iLightness;
	int					iZoomRate;
	int					iSearchTime;
	int					iSearchLoop;
}VCAColorTrack, *pVCAColorTrack;

#define MAX_DETECT_AREA_NUM			 8

typedef struct tagVCACommonPara
{
	int					iDevType;								//0-IPC��1-NVR
	VCARule				tRule;
	vca_TDisplayParam	tDisplayParam;
	int					iDisplayTarget;
	int					iMaxSize;
	int					iMinSize;
	int					iSensitivity;
	int					iRegionNum;
	vca_TPolygonEx		stPoints[MAX_DETECT_AREA_NUM];			//The number of vertices of the polygon area and coordinate
}VCACommonPara, *pVCACommonPara;

typedef struct tagVCAParaProcratorateDuty
{
	VCACommonPara		tVcaCommPara;
	int					iLeaveTime;
	int					iInvalidRegionNum;
	vca_TPolygonEx		stInvalidPoints[MAX_DETECT_AREA_NUM];
	vca_TPolygonEx		stPrisonerRegion;					//Prisoner Region
}VCAParaProcratorateDuty, *pVCAParaProcratorateDuty;

typedef struct tagVCAParaHeightLimit
{
	VCACommonPara		tVcaCommPara;
	int					iLimitTime;
}VCAParaHeightLimit, *pVCAParaHeightLimit;

typedef struct tagVCANewDuty
{
	VCACommonPara		tVcaCommPara;
	int					iLeaveTime;
	int					iDutyNum;
}VCAParaNewDuty, *pVCAParaNewDuty;

typedef struct tagVCAAbnormalNum
{
	VCACommonPara       tCommonInfo;
	int                 iReferNum;
	int                 iMode;
	int                 iLeaveTime;
	int			        iInvalidRegionNum;
	vca_TPolygonEx		stInvalidPoints[MAX_DETECT_AREA_NUM];	
}VCAAbnormalNum, *pVCAAbnormalNum;

typedef struct tagVcaArmTouch
{
	VCACommonPara       tCommonInfo;
	int                 iTimeMin;
	int			        iInvalidRegionNum;
	vca_TPolygonEx		stInvalidPoints[MAX_DETECT_AREA_NUM];	
}VcaArmTouch, *pVcaArmTouch;

typedef struct tagVcaNewFight
{
	VCACommonPara       tCommonInfo;
	int                 iTimeMin;
	int			        iInvalidRegionNum;
	vca_TPolygonEx		stInvalidPoints[MAX_DETECT_AREA_NUM];	
}VcaNewFight, *pVcaNewFight;

typedef struct tagVcaStillDect
{
	VCACommonPara       tCommonInfo;
	int                 iStillTime;
	int			        iInvalidRegionNum;
	vca_TPolygonEx		stInvalidPoints[MAX_DETECT_AREA_NUM];	
}VcaStillDect, *pVcaStillDect;

typedef struct tagVCAParaGetUp
{
	VCACommonPara		tCommonPara;
	vca_TLine			tLinePoins;
}VCAParaGetUp, *pVCAParaGetUp;

typedef struct tagVCAParaSlipUp
{
	VCACommonPara		tCommonPara;//The Slip Up algorithm has only one region.The default value iRegionNum=1.
	int     			iHeightMax;	
}VCAParaSlipUp, *pVCAParaSlipUp;

typedef struct tagVCAParaLeaveBed
{
	VCACommonPara		tCommonPara;
	int     			iLeaveTime;	
}VCAParaLeaveBed, *pVCAParaLeaveBed;

typedef struct tagVCAParaSleep
{
	VCACommonPara		tCommonPara;
	int     			iSleepTime;	
	int					iInvalidRegionNum;
	vca_TPolygonEx		stInvalidPoints[MAX_DETECT_AREA_NUM];
}VCAParaSleep, *pVCAParaSleep;

#define		MAX_LOCAL_NVR_TARGET_NUM   4			//ÿ����Ŀ�����4��
#define		MAX_LOCAL_NVR_REGION_NUM   8			//�������8��
typedef struct tagTargetResult
{
	int				iAlarmState;
	int				iTargetID;
	int				iTargetType;
	int				iLeft; 
	int				iTop; 
	int				iRight; 
	int				iBottom; 	
}TargetResult, *pTargetResult;

typedef struct tagRegionInfo
{
	int				iTargetNum;
	TargetResult	tTargetResult[MAX_LOCAL_NVR_TARGET_NUM];
}RegionInfo, *pRegionInfo;

typedef struct _tagDayScheduleTime
{
	int  				iStartHour;
	int  				iStartMin;
	int  				iStopHour;
	int  				iStopMin;
	int  				iEnable;			
}DayScheduleTime,*PDayScheduleTime;

#define MAX_DAYS								7
#define MAX_EXTRA_SCHEDULE_TIME_SEGMENT			16
typedef struct _tagExtraAlarmSchedule
{
	int					iSize;
	int					iSceneID;
	int					iAlarmType;
	int					iWeekday;						
	int					iParam1;
	int					iParam2;
	int					iSegmentNum;
	DayScheduleTime		tTimeSegment[MAX_EXTRA_SCHEDULE_TIME_SEGMENT];		
}ExtraAlarmSchedule, *PExtraAlarmSchedule;


//faceParam
#define FACE_CMD_MIN								0	
#define FACE_CMD_EDIT								(FACE_CMD_MIN + 0x00)
#define FACE_CMD_DELETE								(FACE_CMD_MIN + 0x01)
#define FACE_CMD_QUERY								(FACE_CMD_MIN + 0x02)
#define FACE_CMD_MODEL								(FACE_CMD_MIN + 0x03)
#define FACE_CMD_LIB_EDIT							(FACE_CMD_MIN + 0x04)
#define FACE_CMD_LIB_DELETE							(FACE_CMD_MIN + 0x05)
#define FACE_CMD_LIB_QUERY							(FACE_CMD_MIN + 0x06)
#define FACE_CMD_FEATURE_QUERY						(FACE_CMD_MIN + 0x07)
#define FACE_CMD_FEATURE_CALC						(FACE_CMD_MIN + 0x08)
#define FACE_CMD_CUT								(FACE_CMD_MIN + 0x09)
#define FACE_CMD_CUT_QUERY							(FACE_CMD_MIN + 0x0A)
#define FACE_CMD_TASK_CREATE						(FACE_CMD_MIN + 0x0B)
#define FACE_CMD_TASK_FREE							(FACE_CMD_MIN + 0x0C)
#define FACE_CMD_LIB_IMPORT							(FACE_CMD_MIN + 0x0D)
#define FACE_CMD_SEARCH								(FACE_CMD_MIN + 0x0E)
#define FACE_CMD_LIB_SYNC_START						(FACE_CMD_MIN + 0x0F)
#define FACE_CMD_LIB_SYNC_STATUS					(FACE_CMD_MIN + 0x10)
#define FACE_CMD_CUT_EX								(FACE_CMD_MIN + 0x11)
#define FACE_CMD_SEARCH_SNAP						(FACE_CMD_MIN + 0x12)
#define FACE_CMD_SEARCH_SNAP_PROCESS				(FACE_CMD_MIN + 0x13)
#define FACE_CMD_SEARCH_SNAP_RESULT 				(FACE_CMD_MIN + 0x14)
#define FACE_CMD_ALARM_PARAM						(FACE_CMD_MIN + 0x15)
#define FACE_CMD_MAX								(FACE_CMD_MIN + 0x16)	

typedef struct tagFaceReply
{
	int					iSize;					//�ṹ���С
	int					iLibKey;				//������keyֵ��iLibKey>0	  
	int 				iFaceKey;				//ͼƬkeyֵ��iFaceKey>0 
	int					iOptType;				//0-������� 1-�༭���� 2-ɾ������ 3-��������� 4-�༭������ 5-ɾ�������� 6-������ģ 7-��ѯ������
												//8-��ѯ���� 9-�������Բ�ѯ 10-����������ȡ 11-������ 12-��ͼ�ѵ�ͼ 13-ͬ�������� 14-��ѯͬ��������״̬
												//15-��ͼ��ץ��ͼ 16-�����⿽��Ǩ�� 17-�������������У�� 18-��������ֵ�ȶ�
	int					iResult;				//0-�ɹ���1-ϵͳ׼���� 2-ʧ�� 3-�������Χ 4-ϵͳ����ɾ�� 5-ϵͳ���ڿ����� 
												//6-ϵͳ����ִ����ͼ��ץ��ͼ���� 7-�������
	char				cLibUUID[LEN_UUID];		//��������ƽ̨��ID
	char				cFaceUUID[LEN_UUID];	//ͼƬ��ƽ̨��ӦID
	int					iDelLibProgress;		//��iResult=4ʱ��Ч����ʾ������ɾ�����ȣ�ȡֵ��Χ0-100
												//��iResult=6ʱ��Ч����ʾ��ͼ��ץ��ͼ�������ȣ�ȡֵ��Χ0-100
												/*��iOptType=0 && iResult=2ʱ��Ч, ���ؽ������
													1�������׿ⲻ����
													2����ͣ���ܷ���ʧ��
													3: �豸���������ͶԽ�
													4: ������ݿ�ʧ��
													5������ͼƬ����
													6����ģ����---��ģͼƬ������Ҫ��
													7����ģ����---��ģͼƬ�㷨û�м������
													8����ģ����---��ģͼƬ�����������֮�󳬹�1280*736
													9����ģ����---ͼƬ�ĸ�ʽ�����ݲ�ƥ��
													10����ģ����---��ģͼƬ���뵽�㷨���ݿ�ʧ��
													11����ģ����---��ģͼƬ���㷨���ݿ�ɾ��ʧ��
													12����ģ����---��jpegͷ����Ϣ��ͼƬ��ߴ���
													13: JPG ת YUV ʧ��
													14: JPG ת ARGB ʧ��
													15����Ч��feature data
													16���㷨���ش���
													17��VGS����ʧ��                                                     
													18: ��ģ����---��ȡ��Ƶ֡ʧ��  
													19: VGS�ֲ����Ż���ԭͼ����ʧ��  
												*/
}FaceReply,*pFaceReply;

typedef struct tagFaceInfo
{
	int					iSize;					//�ṹ���С
	int					iLibKey;				//������keyֵ��iLibKey>0��iLibKeyΪ0��cLibUUIDΪ��ʱ��ʾ��ѯ���п⣩ 
	int 				iFaceKey;				//ͼƬkeyֵ�� iFaceKey>0
	int					iCheckCode;				//�ļ�У���룬iFaceKey=0ʱ��Ч������У������ͼƬ
	int					iFileType;				//�ļ���չ���ͣ�iFaceKey=0ʱ��Ч��0-jpg��1-png  
	int					iModeling;				//�Ƿ�ģ��0-����ģ��1-��ģ�����ŵ���ʱ��1����������ʱ��0
	char				cName[LEN_64];			//����
	int					iSex;					//�Ա�0-δ֪��1-�У�2-Ů
	char				cBirthTime[LEN_16];		//�������ڣ���ʽΪ��2017-04-04�����16���ֽ�
	int					iNation;				//���壬�����ұ�׼���� ������������
	int					iPlace;					//���ᣬ��16λ��ʾʡ����16λ��ʾ�У�����ʡ�ݳ��д�����ֱ𰴹��������������븳ֵ��0��ʾδ֪
	int					iCertType;				//֤�����ͣ�0-δ֪��1-���֤��2-����֤��3-���գ�4-Ա�����
	char				cCertNum[LEN_64];		//֤����
	int					iOptType;				//�������ͣ�1=��ӣ�2=�޸ģ�3=���ƣ�4=Ǩ��
	char				cLibUUID[LEN_UUID];		//��������ƽ̨��ID����Ϊ���Ա��ֶ�Ϊ׼
	char				cFaceUUID[LEN_UUID];	//ͼƬ��ƽ̨��ӦID����Ϊ���Ա��ֶ�Ϊ׼
	char				cLibVersion[LEN_64];	//������汾��
	char				cVerifyCode[LEN_64];	//ͼƬУ���룬ͼƬ�ļ�MD5
	int					iTaskId;				//����ID����ͼ�ѵ�ͼ��Ч
	char				cFileName[LEN_256];		//ͼƬ���ƣ��׿�ͼƬ��
	int					iSimilar;				//���ƶȣ�0-100����ͼ�ѵ�ͼ�е����ƶȣ�
	int					iCountry;				//���ң������Һ͵���ISO 3166-1����������֣�
	char				cAddress[LEN_64];		//��ַ
	char				cCompany[LEN_64];		//��˾
}FaceInfo,*pFaceInfo;

typedef struct tagFaceEdit
{
	int					iSize;					//�ṹ���С
	int					iChanNo;				//ͨ���ţ���0��ʼ
	char				cFacePic[LEN_256];		//����Ҫ�༭��ͼƬ����	
	FaceInfo			tFace;					//ͼƬ��Ϣ
}FaceEdit,*pFaceEdit;

typedef struct tagFaceDelete
{
	int					iSize;					//�ṹ���С
	int					iChanNo;				//ͨ���Ŵ�0��ʼ
	int					iLibKey;				//������keyֵ��iLibKey>0	  
	int 				iFaceKey;				//ͼƬkeyֵ�� iFaceKey>0
	char				cLibUUID[LEN_UUID];		//��������ƽ̨��ID����Ϊ���Ա��ֶ�Ϊ׼
	char				cFaceUUID[LEN_UUID];	//ͼƬ��ƽ̨��ӦID����Ϊ���Ա��ֶ�Ϊ׼
}FaceDelete,*pFaceDelete;

typedef struct tagFaceQuery
{
	int					iSize;					//�ṹ���С
	int					iChanNo;				//ͨ���ţ���0��ʼ
	int					iPageNo;				//ҳ�룬iPageNo>=0
	int					iPageCount;				//��ǰҳ������iPageSize>=0
	int					iLibKey;				//������keyֵ��iLibKey>0��iLibKeyΪ0��cLibUUIDΪ��ʱ��ʾ��ѯ���п⣩
	int 				iModeling;				//��ģ״̬�� 0-��ģ�ɹ���1-��ģʧ�ܣ�2-δ��ģ 
	char				cName[LEN_64];			//����
	int					iSex;					//�Ա�0-δ֪��1-�У�2-Ů
	char				cBirthStart[LEN_16];	//�������ڲ�ѯ��ʼʱ�䣬��ʽΪ��2017-04-04
	char				cBirthEnd[LEN_16];		//�������ڲ�ѯ����ʱ�䣬��ʽΪ��2017-04-04
	int					iNation;				//���壬�����ұ�׼���� 
	int					iPlace;					//���ᣬ��16λ��ʾʡ����16λ��ʾ�У�
												//�ֱ𰴹��������������븳ֵ��0��ʾδ֪  ����ʡ�ݳ��д����
	int					iCertType;				//֤�����ͣ�0-δ֪��1-���֤��2-����֤��3-���գ�4-Ա�����
	char				cCertNum[LEN_64];		//֤����
	char				cLibUUID[LEN_UUID];		//��������ƽ̨��ID
	char				cLibPwd[LEN_64];		//library password
	int					iCountry;				//country
	char				cAddress[LEN_64];		//address
	char				cCompany[LEN_64];		//company
}FaceQuery,*pFaceQuery;

typedef struct tagFaceQueryResult
{
	int					iSize;					//�ṹ���С					
	int					iChanNo;				//ͨ���ţ���0��ʼ
	int					iTotal;					//������
	int					iPageNo;				//ҳ�룬��0��ʼ
	int					iPageCount;				//��ǰҳ����
	int					iIndex;					//ҳ������ţ���0��ʼ
	FaceInfo			tFace;					//������Ϣ�ṹ��
}FaceQueryResult,*pFaceQueryResult;

typedef struct tagFaceLibInfo
{
	int					iSize;					//�ṹ���С
	int					iLibKey;				//������keyֵ��iLibKey=0��ʾ��������⣬>0��ʾ�޸�������  
	char				cName[LEN_64];			//����������
	int					iThreshold;				//��������ֵ
	char				cExtrInfo[LEN_64];		//������Ϣ
	int					iAlarmType;				//�Ƿ��ϴ�ʶ����Ϣ��0-���ϴ���1-�ϴ�
	int					iOptType;				//�������ͣ�1--��������⣬2--�޸������⣬3--��������У�飬4--ͨ��NVR�޸�ǰ�����ƶ�
	char				cLibUUID[LEN_UUID];		//��������ƽ̨��ID����Ϊ���Ա��ֶ�Ϊ׼
	char				cLibVersion[LEN_64];	//������汾��
}FaceLibInfo,*pFaceLibInfo;

typedef struct tagFaceLibEdit
{
	int					iSize;					//�ṹ���С
	int					iChanNo;				//ͨ���ţ���0��ʼ
	FaceLibInfo			tFaceLib;				//��������Ϣ�ṹ�壬��Ϊ���Ա��ֶ�Ϊ׼
}FaceLibEdit,*pFaceLibEdit;

typedef struct tagFaceLibDelete
{
	int					iSize;					//�ṹ���С
	int					iChanNo;				//ͨ���ţ���0��ʼ
	int					iLibKey;				//������keyֵ��iLibKey>0
	char				cLibUUID[LEN_UUID];		//��������ƽ̨��ID
}FaceLibDelete,*pFaceLibDelete;

typedef struct tagFaceLibQuery
{
	int					iSize;			//�ṹ���С
	int					iChanNo;		//ͨ���ţ���0��ʼ
	int					iPageNo;		//��ǰҳ��ţ���0��ʼ
	int					iPageCount;		//��ǰҳ��С
}FaceLibQuery,*pFaceLibQuery;

typedef struct tagFaceLibQueryResult
{
	int					iSize;			//�ṹ���С
	int					iChanNo;		//ͨ���ţ���0��ʼ
	int					iTotal;			//�ܸ���
	int					iPageNo;		//��ǰҳ��ţ���0��ʼ
	int					iIndex;			//ҳ������ţ���0��ʼ
	int					iPageCount;		//��ǰҳ��С
	FaceLibInfo			tFaceLib;		//��������Ϣ�ṹ��	
}FaceLibQueryResult,*pFaceLibQueryResult;


typedef struct tagFaceModeling
{
	int					iSize;			//�ṹ���С
	int					iChanNo;		//ͨ���ţ���0��ʼ
	int					iType;			//��ģ���ͣ�0-���Ž�ģ��1-������ģ��δ��ģ��������2-������ģ��ȫ����������3-ȡ����ģ
	int					iLibKey;		//������keyֵ��iLibKey>0
	int					iFaceNum;		//��ģ����
	int					iFaceKey[FACE_MAX_PAGE_COUNT];	//ͼƬkeyֵ
}FaceModeling,*pFaceModeling;

typedef struct tagFaceModeResult
{
	int					iSize;			//�ṹ���С
	int					iChanNo;		//ͨ���ţ���0��ʼ
	int					iType;			//��ģ���ͣ�0-���Ž�ģ��1-������ģ��δ��ģ��������2-������ģ��ȫ����������3-ȡ����ģ
	int					iLibKey;		//������keyֵ��iLibKey>0
	int					iFaceKey;		//ͼƬkeyֵ
	char				cName[LEN_64];	//����
	int					iRetsult;		//��ģ�����0-�ɹ���1-ϵͳæ��2-ͼƬ��ʽ����3-��������4-������� 5-������
	int					iTotal;			//��ģ������iTotal>=0
	int					iIndex;			//��ģ���ȣ����Խ�ģ����
}FaceModeResult,*pFaceModeResult;


typedef struct tagFaceFeatureQuery
{	
	int					iSize;					//�ṹ���С
	int					iChanNo;				//ͨ���ţ���0��ʼ
	int					iLibKey;				//������keyֵ��iLibKey>0
	int					iFaceKey;				//ͼƬkeyֵ
	char				cLibUUID[LEN_UUID];		//��������ƽ̨��ID����Ϊ���Ա��ֶ�Ϊ׼
	char				cFaceUUID[LEN_UUID];	//ͼƬ��ƽ̨��ӦID����Ϊ���Ա��ֶ�Ϊ׼
}FaceFeatureQuery,*pFaceFeatureQuery;

typedef struct tagFaceFeatureCalc
{	
	int					iSize;					//�ṹ���С
	int					iChanNo;				//ͨ���ţ���0��ʼ
	int					iCheckCode;				//�ļ�У���룬����У������ͼƬ��0��ʾ��У��
	int					iPicType;				//�ļ���չ���ͣ�0-jpg��1-png
	char				cPicPath[LEN_256];		//��Ҫ��ȡ�������Ե�ͼƬ·��
	char				cPicUUID[LEN_UUID];		//ͼƬ���к�
}FaceFeatureCalc,*pFaceFeatureCalc;

//������
typedef struct tagFaceCut
{	
	int					iSize;					//�ṹ���С
	int					iChanNo;				//ͨ���ţ���0��ʼ
	char				cCheckCode[LEN_64];		//�ļ�У���룬����У������ͼƬMD5 
	int					iTaskId;				//����ID
	int					iPicType;				//�ļ���չ���ͣ�0-jpg��1-png
	char				cPicPath[LEN_256];		//��Ҫ��ͼ��ͼƬ·��
}FaceCut,*pFaceCut;

typedef struct tagFaceCutQuery
{
	int					iSize;					//�ṹ���С
	int					iChanNo;				//ͨ���ţ���0��ʼ
	int					iTaskId;				//����ID
	int					iPageNo;				//ҳ���
	int					iPageCount;				//ҳ��С
}FaceCutQuery,*pFaceCutQuery;


typedef struct tagFaceCutEx
{	
	int					iSize;					//�ṹ���С
	int					iChanNo;				//ͨ���ţ���0��ʼ
	char				cCheckCode[LEN_64];		//�ļ�У���룬����У������ͼƬMD5 
	int					iPicType;				//�ļ���չ���ͣ�0-jpg��1-png
	char				cPicPath[LEN_256];		//��Ҫ��ͼ��ͼƬ·��
	int					iPageNo;				//ҳ���
	int					iPageCount;				//ҳ��С
}FaceCutEx,*pFaceCutEx;

typedef struct tagFaceCutQueryResult
{
	int					iSize;					//�ṹ���С
	int					iChanNo;				//ͨ���ţ���0��ʼ
	int					iTaskId;				//����ID
	int					iTotal;					//��ͼ����
	int					iPageNo;				//ҳ���
	int					iIndex;					//��ǰ�ڼ��������ֵ20
	int					iPageCount;				//��ǰҳ����
	char				cFileName[LEN_256];		//ͼƬ���ƣ�ͼƬ������֮ǰ��ͼƬ����Э��
}FaceCutQueryResult,*pFaceCutQueryResult;

//��ͼ�ѵ�ͼ���ظ���ѯ������Ϣ��
typedef struct tagFaceSearch
{
	int					iSize;				//�ṹ���С
	int					iChanNo;			//ͨ���ţ���0��ʼ
	int					iTaskId;			//����ID
	char				cLibKey[LEN_64];	//��������ƽ̨��ID����Ϊ���Ա��ֶ�Ϊ׼
	char				cPicName[LEN_256];	//ͼƬ����
	int					iSimilar;			//���ƶȣ�0-100
	int					iPageNo;			//��ǰҳ�룬��0��ʼ
	int					iPageCount;			//ÿҳ���������20
	int					iLibKey;			//��id��iLibKey>0(iLibKeyΪ0��cLibUUIDΪ��ʱ��ʾ��ѯ���п�)
}FaceSearch,*pFaceSearch;

typedef struct tagFaceTaskFree
{
	int					iSize;				//�ṹ���С
	int					iTaskId;			//����ID
}FaceTaskFree,*pFaceTaskFree;

#define MAX_FEATURE_DATA_LEN		(32*1024)

typedef struct tagFaceFeatureResult
{		
	int					iSize;					//�ṹ���С
	int					iChanNo;				//ͨ���ţ���0��ʼ
	int					iType;					//��ȡ���ͣ�1����ȡ����ͼƬ��feature��2����ѯͼƬ��feature
	int					iLibKey;				//������keyֵ����ȡ����ͼƬfeatureʱ����ֵΪ0
	int					iFaceKey;				//ͼƬkeyֵ����ȡ����ͼƬfeatureʱ����ֵΪ0
	char				cLibUUID[LEN_UUID];		//��������ƽ̨��ID����ȡ����ͼƬfeatureʱ����ֵΪ0
	char				cFaceUUID[LEN_UUID];	//ͼƬ��ƽ̨��ӦID����ͼƬ���к�
	int					iFaceNum;				//������Ŀ
	int					iFaceRectCount[FACE_MAX_FEATURE_COUNT];		//��ǰ�����������
	TPoint				tFaceRectPoint[FACE_MAX_FEATURE_COUNT][FACE_MAX_RECT_POINT_COUNT];	//��ǰ������������
	int					iFaceDataLen[FACE_MAX_FEATURE_COUNT];	//��ǰ����feature���ݵ��ܳ���
	char				cFaceData[FACE_MAX_FEATURE_COUNT][MAX_FEATURE_DATA_LEN];	//feature����������
}FaceFeatureResult,*pFaceFeatureResult;

#define FACE_ALARM_DETECTION				0		//Face detection
#define FACE_ALARM_RECOGNITON_DISCERN		1		//Face recognition-Comparison alarm
#define FACE_ALARM_RECOGNITON_STRANGER		2		//Face recognition-Stranger alarm
#define FACE_ALARM_RECOGNITON_RATEN			3		//Face recognition-Frequency alarm
#define FACE_ALARM_RECOGNITON_DETENTION		4		//Face recognition-Stay alarm

typedef struct tagFaceAlarmParam
{
	int					iSize;					//�ṹ���С
	int					iChanNo;				//ͨ���ţ���0��ʼ
	int					iAlarmType;				//���������б�			
	int					iParam1;				//iType=21��NVR�����ܷ����б�							
	int					iParam2;				//��iType=21�� iParam1=3,4ʱ����ʱ��
	int					iParam3;				//iType =21��iParam2=3ʱ������Ƶ��
	char				cLibkey[LEN_64];		//������keyֵ	
	int					iRecognition;			//�Ƿ��ϴ�ʶ����Ϣ��0-��֧�֣�1-���ϴ���2-�ϴ�
	int					iSimilar;				//���ƶȣ�0--100
	int					iDevType;				//�豸���ͣ�0-IPC��1-NVR	
	int					iEnable;				//ʹ�ܣ�0-��ʹ��  1-ʹ��
	char				cLibUUID[LEN_UUID];		//��������ƽ̨��ID�б�UUID�б����10�������ŷָ�
	int					iLibEnable;				//������ʹ���б�0-��ʹ��  1-ʹ�ܣ�����ͬcLibUUIDList�����ŷָ�  ����1,0,1
}FaceAlarmParam, *pFaceAlarmParam;

typedef struct tagFaceAlarmParamIn
{
	int					iSize;			//�ṹ���С
	int					iType;			//�����������ͣ�0--������⣬1--�ȶԱ�����2--İ���˱�����3--Ƶ�α�����4--��������
	int					iRetCount;		//������Ҫ��ȡ�ı�����Ϣ��Ԫ�ĸ���
}FaceAlarmParamIn, *pFaceAlarmParamIn;

#define  MAX_DETECTAREA_NUM 128
#define  MIN_POINT_NUM  3
typedef struct _tagDetectArea
{
	int				 	iSize;
	int              	iChannelNO;
	int              	iSceneID;
	int              	iStatus;
}DetectArea, *pDetectArea;

typedef struct _tagDetectAreaResult
{
	int				  	iSize;
	int               	iChannelNO;
	int               	iSceneID;
	int               	iAreaNum;
	int               	iState;   
	vca_TPolygonEx    	tPoint;
}DetectAreaResult, *pDetectAreaResult;

typedef struct _tagDetectAreaFinish
{
	int				  	iSize;
	int               	iChannelNO;
	int               	iSceneID;
	int               	iAreaNum; 
	int               	iAreaList[MAX_DETECTAREA_NUM];
}DetectAreaFinish, *pDetectAreaFinish; 

#define MAX_PACK_AREA_LIST      16
#define MAX_PACK_NUM            8
typedef struct _tagDetectAreaListInfo
{
	int                 iPackNo;
	int                 iAreaList[MAX_PACK_AREA_LIST];
}DetectAreaListInfo, *pDetectAreaListInfo;

typedef struct _tagDetectAreaFinishEx
{
	int				  	iSize;
	int               	iChannelNO;
	int               	iSceneID;
	int               	iAreaNum; 
    DetectAreaListInfo  tInfo[MAX_PACK_NUM];
}DetectAreaFinishEx, *pDetectAreaFinishEx;

typedef struct _tagDetectPara
{
	int 				iSize;						//size of struct
	int 				iChannelNo;
	int 				iSceneID;
	int 				iDetectLoop;
	int 				iDetectTime;
} DetectPara, *pDetectPara;


typedef struct _tagSetDetectArea
{
	int            		iSize;						//size of struct
	int            		iChannelNO;
	int            		iSceneID;
	int            		iAreaNum;
	int            		iOperationType; 
	vca_TPolygonEx 		tPoints;
} SetDetectArea, *pSetDetectArea; 

#define  PICTURE_TYPE_CUSTOMIZE       0
#define  PICTURE_TYPE_HEAD_PHOTO      1
#define  PICTURE_TYPE_HALFBODY_PHOTO  2
#define  PICTURE_TYPE_FULLBODY_PHOTO  3
typedef struct _tagVcaTargetPicture
{
	int					iSize;				//size of struct
	int 				iTargetPictureType; //Target image type  0-customize 1-Head Photo 2-Half-body Photo 3-Full-body Photo
	int					iPictureWidth;		//Picture Width(1~5) When iTargetPictureType=0,This param is available.
	int					iFaceHeight;		//Face Height(1~3)	 When iTargetPictureType=0,This param is available.
	int					iBodyHeight;		//Body Height(0~10)	 When iTargetPictureType=0,This param is available.
}VcaTargetPicture,*pVcaTargetPicture;

typedef struct _tagRainFallAlarmInfo
{
	int                 iSize;							//size of struct
	int                 iRainfallValue;     			//80,120,160(mm)
	int                 iAlarmTime;         			//Alarm Time 1-600s
	int                 iRainTimeInter;     			//The Interval Of Rain Times
	int                 iUploadTime;        			//Default Upload Check Time (min)  
	int                 iModerateRainUploadTime;        //Moderate Rain Upload Check Time (min)
	int                 iHeavyRainUploadTime;           //Heavy Rain Upload Check Time(min)
	int                 iTorrentialRainUploadTime;      //Torrential Rain Upload Check Time (min)
	int                 iModerateRainThershold;         //Moderate Rain Thershold (mm/min) 
	int                 iHeavyRainThershold;            //Heavy Rain Thershold mm/min
	int                 iTorrentialRainThershold;       //Torrential Rain Thershold mm/min
}RainFallAlarmInfo, *pRainFallAlarmInfo;

#define DEFAULT_CHECKSTAND                         1
#define DEFAULT_ALERTWATER                         0
#define DEFAULT_ALARMLINKTIME                     120
#define DEFAULT_UPLOADTIME                        360
#define DEFAULT_FIRSTWATERLEVELUPLOADTIME         240
#define DEFAULT_SECWATERLEVELUPLOADTIME           120
#define DEFAULT_THIRDWATERLEVELUPLOADTIME          60
#define DEFAULT_FIRSTWATERLEVELTHERSHOLD           1
#define DEFAULT_SECWATERLEVELTHERSHOLD             5
#define DEFAULT_THIRDWATERLEVELTHERSHOLD           10
#define MAX_WATER_THERSHOLD_NUM                    5

typedef struct _tagAlertWaterAlarmInfo
{
	int                 iSize;							//size of struct
	int                 iAlertWaterValue;			    //Alert Water Level Alarm Thershold
	int                 iAlarmLinkTime;					//Alarm Time 1-600s
	int					iSenceID;						//The ID Of Sence
	int                 iRuleID;					    //The ID Of Rule
	int                 iIsMainChannel;					//Whether Main Channel Or Not(0:NO; 1:Yes)
	int					iCheckStand;					//The Source Of Water Level Alarm Data
	int                 iDefaultUploadTime;             //Default Upload Check Time
	int                 iFirstWaterLevelUploadTime;     //The Upload Check Time Of First Water Level Change(min)
	int                 iSecondWaterLevelUploadTime;    //The Upload Check Time Of Second Water Level Change(min)
	int                 iThirdWaterLevelUploadTime;     //The Upload Check Time Of Third Water Level Change(min)
	int                 iFirstWaterLevelThershold;      //First Water Level Thershold(cm/h)
	int                 iSecondWaterLevelThershold;		//Second Water Level Thershold(cm/h)
	int                 iThirdWaterLevelThershold;		//Third Water Level Thershold(cm/h)
	int					iAlgorithmType;					//Algorithm Type
	int                 iWaterNum;                      //Alert Water Level Alarm Thershold num��
	int                 iWaterAlarm[MAX_WATER_THERSHOLD_NUM]; //Alert Water Data;
}AlertWaterAlarmInfo, *pAlertWaterAlarmInfo;

typedef struct tagIrrigationOverInfo
{
	int					iSize;							//Size Of Struct
	int					iType;							//Type Of Reporting Irrigation Info
	int					iLedEnable;						//Whether Overlying Led Or Not
	int 				iOsdEnable;						//Whether Overlying Video Or Not
}IrrigationOverInfo,*pIrrigationOverInfo;

#define   MAX_PERIPHERAL_PARA_COUNTER       4
typedef struct tagPeripheralPara
{
	int					iSize;							//Size Of Struct
	int					iComNo;							//Com Number
	int 				iPeripheralType;				//Peripheral Type
	int 				iPeripheralAddr;				//address code
	int					iParaCounter;					//Parameter Counter
	char 	         	cPeripheralPara[MAX_PERIPHERAL_PARA_COUNTER][LEN_256]; //Peripheral Parameter
}PeripheralPara,*pPeripheralPara;

#define IRRIGATION_TYPE_RAINFALL				1		//rainfall
#define IRRIGATION_TYPE_RAINDURATION			2		//rain duration
#define IRRIGATION_TYPE_WATERLEVEL				3		//water level
#define IRRIGATION_TYPE_SEDIMENT				4		//depth of water
#define IRRIGATION_TYPE_ALERTWATERLEVEL			5		//alert water level
#define IRRIGATION_TYPE_FLOWRATEVALUE			6		//Flow rate value
#define IRRIGATION_TYPE_BATTERYVOLTAGE			7		//Battery voltage value
#define IRRIGATION_TYPE_AIRPRESSURE 			8		//Air pressure
#define IRRIGATION_TYPE_WINDSPEED				9		//Wind speed
#define IRRIGATION_TYPE_WINDDIRECTION			10		//wind direction
#define IRRIGATION_TYPE_TEMPERATURE				11		//Temperature
#define IRRIGATION_TYPE_HUMIDITY				12		//Humidity
#define IRRIGATION_TYPE_ACIDITYANDALKAL         13
#define IRRIGATION_TYPE_DISSOLVEOXYGEN          14
#define IRRIGATION_TYPE_OXYREDUCTION            15
#define IRRIGATION_TYPE_GPS                     16		//GPS
#define IRRIGATION_TYPE_RTX                     17		//RTX
#define IRRIGATION_TYPE_TURBIDITY               18		//turbidity
#define IRRIGATION_TYPE_AMMONICA                19		//Ammonica
#define IRRIGATION_TYPE_WATERTEMP				20		//Water temp
#define IRRIGATION_TYPE_CONDUCTIVITY			21		//Conductivity
#define IRRIGATION_TYPE_OXYDEMAND				22		//Oxydemand
#define IRRIGATION_TYPE_NITROGEN				23		//Nitrogen
#define IRRIGATION_TYPE_PHOSPHORUS				24		//Phosphorus
#define IRRIGATION_TYPE_PRESSURE_WATERLEVEL		25
#define IRRIGATION_TYPE_WATER_SPEED_FIELD		26
#define MAX_IRRIGATION_TYPE						27

#define IRRIGATION_PARA_SRC_ALGO				0		//algorithm acquisition
#define IRRIGATION_PARA_SRC_EXDEV				1		//peripheral acquisition

typedef union tagIrrigationPara8
{
	int        iWaterLevel;  //water level
	int        iRtxData;     //RTX Height
	int        iCommonData;
}IrrigationPara8, *pIrrigationPara8;

typedef union tagIrrigationPara9
{
	int        iValid;       //0:invalid; 1:valid
	int        iCruiseNum;   
	int        iCommonData;
}IrrigationPara9, *pIrrigationPara9;
typedef struct tagIrrigationPara
{
	int					iSize;
	int					iType;				//Irrigation type 1:����;2:����ʱ��;3:ˮλ;4:��ˮ���;5:������ˮλֵ;6:����;7:��ƿʣ�����;8:��ѹ;9:����;10:����;11:�¶�;12:ʪ��;13:����;14:�ܽ���;15:������ԭ;16:GPS;17:�߳�;18:�Ƕ�;19:����;20:ˮ��;21:�絼��;22:��ѧ������;23:�ܵ�;24:����;25:���ٳ�;
	int					iValue;				//Param2 iType(1,3,4,5)-->Unit:mm;  iType(2)-->Unit:minute ;iType(6)-->Unit:mm/s;iType(7,12)-->Unit:%;
											//		 iType(8)-->Unit:hpa; iType(9)-->Unit:m/s; iType(10)-->Unit:angle��;iType(11)-->Unit:degree centigrade ��;	
	int					iSrc;				//Param3  0:algorithm acquisition; 1:peripheral acquisition
	int					iSceneID;			//Param4  Scene ID
	int					iRuleID;			//Param5  Rule ID
	NVS_FILE_TIME       tUploadTime;		//the time of data uploading
	int                 iStationNum;		//Param7  the station No.(when the type is flowSpeed or WaterLevel)
	IrrigationPara8     tIrriParam8;		//Param8  when the type is flowspeed,this para represents cruise times;when the type is WaterLevel, it represents if data is valid
	IrrigationPara9     tIrriParam9;		//Param9  when the type is flowspeed,this para represents cruise times;when the type is WaterLevel, it represents if data is valid
	int                 iTotalPointNum;		//Param10 (the total detect numbers)
	int                 iCurrentPointNum;	//Param11 (the Current detect Number)
	int                 iBaseNum;			//Param12 the distance of baseLine(when the type is flowSpeed)
	NVS_FILE_TIME       tRecordTime;		//start time of Record file(when the type is flowSpeed)
	char				cFactoryID[LEN_64];	//Param14 Device ID, no more than 64 bytes
	int					iIrriParam6;		//Param6  when iType=25,means ���������
	int					iIrriParam13;		//Param13 when iType=25,means ���ص���ֹ������
	int					iIrriParam15;		//Param15 when iType=25,means ���ص���ֹ������
	int					iIrriParam16;		//Param16 when iType=25,means �ռ����ʼ������
	int					iIrriParam17;		//Param17 when iType=25,means �ռ����ʼ������
	int					iIrriParam18;		//Param18 when iType=25,means �ռ����ֹ������
	int					iIrriParam19;		//Param19 when iType=25,means �ռ����ֹ������
	unsigned long long	ulLastTimeStamp;	//Param20 when iType=25,means Last frame timestamp
	unsigned long long	ulCurTimeStamp;		//Param21 when iType=25,means Current frame timestamp
} IrrigationPara, *pIrrigationPara;




#define  MAX_MANCAR_DETECT_AREA_COUNT  32
#define  MAX_MANCAR_DETECT_TRIPAREA_COUNT  12

#define ARI_PUSHMODE_SPEED_FIRST	1  //iPushMode Speed first
#define ARI_PUSHMODE_QUALITY_FIRST	2  //iPushMode Quality first
#define ARI_PUSHMODE_CUSTOM			3  //iPushMode custom
#define ARI_PUSHMODE_TIMING			4  //iPushMode Timing
#define ARI_PUSHMODE_TOUCHLINE		5  //iPushMode Touch line

#define ARI_SNAPMODE_ALL			1  //iSnapMode 1:snap all
#define ARI_SNAPMODE_QUALITY		2  //iSnapMode 2:snap high quality 
#define ARI_SNAPMODE_CUSTOM			3  //iSnapMode 3:custom
typedef struct tagVcaManCarDetectArithmetic
{
 	int				iSize;				//structure size
 	int				iSceneID;			//Scene id(0~15)
	int				iType;				//Structured algorithm model bit��0-Face��1-pedestrian��2-plate��3-motor vehicles��4-Non-motor vehicle
 	int				iSensitiv;			//Sensitivity(0~5)
 	int				iDisplayTarget;		//display target box, 0-not dispaly, 1-dispaly
 	int				iExposureBright;	//exposure light strength (0~255)
 	int				iDisplayRule;		//0-not display 1-display
 	int				iPushMode;			//push mode 0:reserved 1:Speed first 2:Quality first  3:custom 4:timing 5:Touch line	
 	int				iPushLevel;			//push level effect when iPushMode == 3;(0:reserved,1:fast 2:normal 3:slow)   effect when iPushMode == 4;(value is timing time. range 1~255)
 	int				iSnapMode;			//snap mode (0:reserved 1:snap all 2:snap high quality 3:custom)
 	int				iSnapTimes;			//Snap times(1~10)
 	int				iMinFaceSize;		//minimum face size(1~10000 percentage of image width��10000 indicates the width of entire screen)(This field is 0 and will not be processed.)
	int				iMinPlateSize;		//minimum plate size(1~10000 percentage of image width��10000 indicates the width of entire screen)(This field is 0 and will not be processed.)
 	int				iFaceQuality;		//Facial quality [0-100]
	int				iHumanQuality;		//Pedestrian quality  [0-100]
	int				iPlateQuality;		//Plate quality[0-100]
	int				iVehicleQuality;	//Motor vehicles quality[0-100]
	int				iCycleQuality;		//Non-motor vehicle quality[0-100]
	int				iBigbkimgQuality;	//Background image quality[0-100]
	int				iSmallimgQuality;	//Small image quality[0-100]
	int				iPointNum;			//polygon area vertex number(3��32)
 	TPoint			ptArea[MAX_MANCAR_DETECT_AREA_COUNT]; //x:y percentage of image width
 	int				iTripPointNum;		//Number of vertices of the strike line  effect when iPushMode == 5(0 or 2~12)
 	TPoint			ptTripArea[MAX_MANCAR_DETECT_TRIPAREA_COUNT]; //x:y percentage of image width
    int             iExposureType;      //�ع�����	0-����, 1-��֡, 2-��֡
    int             iExposureEnable;    //�ع�ʹ��	0-��ʹ��, 1-ʹ��
    int             iPlateAlarmType;    //���Ƹ澯����	0-����, 1-���ƺ���������
}VcaManCarDetectArithmetic, *pVcaManCarDetectArithmetic;

typedef struct tagItsIpdWarningMsg
{
	int		iSize;
	int		iSceneId;
	int		iAreaId;
	char	cRoadName[LEN_64];
	char	cPlate[LEN_32];
	int		iPlateType;
	int		iCarType;
	int		iCapTime;
} ItsIpdWarningMsg, *pItsIpdWarningMsg;

typedef struct tagSelfTest
{
	int				iSize;			//structure size
	int				iType;			//Selftest type 1:complete self-test; 2:hardware self-test; 3:software self-test.
	int				iTestResult;	//Test result 1:success 2:failed
}SelfTest, *pSelfTest;

typedef struct tagNetTestPara
{
	int     		iTestType;		//NetWork Test Type 1:ping
	int				iCardNum;		//Card Number 0��Lan1��1��Lan2 ��.7��Lan8; 10000~10000+n:bond0~bondn
	char			cIp[LEN_16];	//Device Ip
	int             iPort;			//Port
	char            cReserve[LEN_64];
}NetTestPara, *pNetTestPara;

typedef struct tagNetTestResult
{
	int     		iTestType;	
	int				iCardNum;
	int   			iReachable;		//0:Reachable   1:unReachable
	int             iDelayTime;		//NetWork Delay Time(ms)
	int 			iLostRate;		//Lose Packet Rate[0, 100]
}NetTestResult, *pNetTestResult;

typedef struct tagLogNum
{
	int				iSize;			//structure size	
	int				iPara1;			//Maximum number of supported logs;
	int             iPara2;			//The current number of logs;
}LogNum, *pLogNum;


#define USERLOGONLOCK_TYPE_MIN			1
#define USERLOGONLOCK_TYPE_MAX			2
typedef struct tagUserLogonLockPara
{
	int				iSize;			//structure size	
	int				iEnable;		//0:reserved; 1:enabled; 2:not enabled
	int             iType;			//1:logon lock-time setting; 2, logon lock-Limit the number of logon
	int				iPara;			//iType=1,lock time;iType=2,Limit the number of logon
}UserLogonLockPara, *pUserLogonLockPara;

#define FACE_LIBSYNC_CHANLIST_COUNT			64

#define FACE_LIBSYNC_STATUS_START			20
#define FACE_LIBSYNC_STATUS_STOP			21
#define FACE_LIBSYNC_STATUS_DELETE			22	//delete sync task
#define FACE_LIBSYNC_STATUS_DELETE_IPCLIB	23	//delete ipc lib

//����ͬ��ǰ��������
typedef struct tagFaceLibSyncStart
{
	int					iSize;					//structure size
	int					iChanListArraySize;		//��Ҫͬ����ͨ���б����
	int 				iChanList[FACE_LIBSYNC_CHANLIST_COUNT];	//ͨ���б�
	int					iLibKey;				//������keyֵ
	char				cLibUUID[LEN_UUID];		//��������ƽ̨��ID����Ϊ���Ա��ֶ�Ϊ׼
	int					iStatus;				//��ʼ״̬��20��ʼ��21ֹͣ��22ɾ������23 ɾ��ǰ��������
	int					iSame;					//���ƶ�	0-100
}FaceLibSyncStart,*pFaceLibSyncStart;

//��ȡͬ��ǰ��������״̬
typedef struct tagFaceLibSyncQuery
{
	int					iSize;				//structure size
	int					iChanNo;			//ͨ���ţ�ȡֵ��Χ���豸���Ͷ���,0x7FFFFFFF��������ͨ��
	int					iQueryResultSize;	//when iChanNo=0x7FFFFFFF,iQueryResultSize=1;else iQueryResultSize is NVR's all channel number.
	int					iLibKey;			//������keyֵ
	char				cLibUUID[LEN_UUID];	//��������ƽ̨��ID����Ϊ���Ա��ֶ�Ϊ׼
}FaceLibSyncQuery,*pFaceLibSyncQuery;

//��ȡͬ��ǰ��������״̬���
typedef struct tagFaceLibSyncQueryResult
{
	int					iSize;						//structure size
	int					iChanNo;					//ͨ���ţ�ȡֵ��Χ���豸���Ͷ���,0x7FFFFFFF��������ͨ��
	int					iLibKey;					//������keyֵ
	char				cLibUUID[LEN_UUID];			//��������ƽ̨��ID����Ϊ���Ա��ֶ�Ϊ׼
	int					iState;						//sync status:0-unsync, 1-synching, 2-sync success, 3-sync failure, 4-to be sync
	int					iProcess;					//Percentage of progress: range:0-100
	int					iSuccNum;					//sync success num		
	int					iFailNum;					//sync failed num
}FaceLibSyncQueryResult,*pFaceLibSyncQueryResult;

#define SINGLE_PRTOCOL_SYNC_RESULT_MAX_NUM 10
//��ȡͬ��ǰ��������ÿ����ͼ����ϸ���(ͬ��ʧ�ܵĽ��)
typedef struct tagFaceLibSyncResult
{
	int					iSize;						//structure size
	int					iChanNo;					//ͨ���ţ�ȡֵ��Χ���豸���Ͷ���,0x7FFFFFFF��������ͨ��
	int					iLibKey;					//������keyֵ
	char				cLibUUID[LEN_UUID];			//��������ƽ̨��ID����Ϊ���Ա��ֶ�Ϊ׼
	int 				iFaceKey;					//ͼƬkeyֵ�� iFaceKey>0
	char				cFaceUUID[LEN_UUID];		//ͼƬ��ƽ̨��ӦID����Ϊ���Ա��ֶ�Ϊ׼
	int					iSyncResult;				//Sync result:0Ԥ����1ͬ���ɹ���2ͬ��ʧ�ܣ�
}FaceLibSyncResult,*pFaceLibSyncResult;

typedef struct tagAlarmOutPortPara
{
	int					iSize;
	int					iMode;		//0:normally open, 1:normally close
	int				    iPulseWidth;
	int					iChanNo;	//0x7fffffff Local param
	int					iPortNo;
} AlarmOutPortPara, *pAlarmOutPortPara;

typedef struct tagAlarmInPortEnable
{
	int    iSize;
	int    iChannelNo;
	int    iPortNo;
	int    iEnable;//0-disenable;1-enable
}AlarmInPortEnable, *pAlarmInPortEnable;

typedef struct tagAlarmOutPortEnable
{
	int    iSize;
	int    iChannelNo;
	int    iPortNo;
	int    iEnable; //0-disenable;1-enable
}AlarmOutPortEnable, *pAlarmOutPortEnable;

typedef struct tagpAlarmInPortState
{
	int    iSize;
	int    iChannelNo;
	int    iPortNo;
	int    iState;//1-Alarm ;0-clear alarm    
}AlarmInPortState, *pAlarmInPortState;

typedef struct tagAlarmOutPortState
{
	int    iSize;
	int    iChannelNo;
	int    iPortNo;
	int    iState;//1-Alarm ;0-clear alarm       
}AlarmOutPortState, *pAlarmOutPortState;

typedef struct tagAlarmOutPortDelayTime
{
	int    iSize;
	int    iChannelNo;
	int	   iPortNo;
	int    iClearType;  //0-Manual cancellation;1-Delay cancellation
	int    iDelayTime ;
}AlarmOutPortDelayTime, *pAlarmOutPortDelayTime;

typedef struct tagAlarmInPortPara
{
	int					iSize;
	int					iMode;		//0:normally open, 1:normally close
	int					iType;		//0:normal port alarm, 1:button alarm
	float               fParam;    //Accuracy of tipping bucket rain gauge	
	int					iChanNo;	//0x7fffffff Local param
	int					iPortNo;
} AlarmInPortPara, *pAlarmInPortPara;

#define INVALID_FLAG	0x7fffffff

//��ͼ��ץ��ͼ
typedef struct tagFaceSearchSnap
{
	int  			    iSize;			//structure size
	int					iChanCount;		//��Ҫ������ͨ������
	int					iChanSize;		//�ṹ��QueryChanNo�Ĵ�С��sizeof(QueryChanNo)
	QueryChanNo*		pChanList;		//ͨ���б��ṹ��QueryChanNo�������
	NVS_FILE_TIME	    tBegTime;		//������ʼʱ��
	NVS_FILE_TIME	    tEndTime;		//��������ʱ��
	char			    cPicturePath[LEN_256];	//��ͼ��ץ��ͼ��Ҫ�����ͼƬ���ơ���ȷ�����ظ�ͼƬ���ڣ������������򿪣�
	int 				iSimilarity;	//��ͼ��ץ��ͼ���ƶ�
	int 			    iSortMode;		//����ʽ��0-��ʱ������1-�����ƶ�����(0-sort by time 1-sort by similarity)
	int                 iTaskId;		//��ͼ��ģ������ID����ֵ�ɿ�ͼ�������
}FaceSearchSnap, *pFaceSearchSnap;

//��ȡ�������ȴ�������ṹ��
typedef struct tagFaceSearchSnapProcess
{
	int    iSize;						//structure size
	int	   iTaskId;						//��ͼ��ģ������ID����ֵ�ɿ�ͼ�������
} FaceSearchSnapProcess, *pFaceSearchSnapProcess;

typedef struct tagFaceSearchSnapQuery
{
	int    iSize;			//structure size
	int    iTaskId;			//��ͼ��ģ������ID����ֵ�ɿ�ͼ�������
	int    iPageSize;		//��ͼ��ץ��ͼÿ�β�ѯ��ҳ��С������ÿ��20����¼
	int    iPageNo;			//��ѯ��ҳ��ţ�0��1��2,3����
}FaceSearchSnapQuery, *pFaceSearchSnapQuery;

typedef struct tagFaceSearchSnapResult{
	int					iSize;				//structure size
	int					iChanNo;			//ͨ���ţ�ȡֵ��Χ���豸���Ͷ���,0x7FFFFFFF��������ͨ��
	int					iTotal;				//�������ļ�¼���ܸ���
	int					iCurPageCount;		//���λ�ȡ�ĸ�����ҳ��С
	int					iItemIndex;			//��ǰҳ��ţ�����ҳ��С����ҳ��š������ܹ�100����¼��ÿ��ȡ20����ҳ�������Ϊ0,1,2,3,4
	NVS_FILE_TIME		tBegTime;			//��¼��ʼʱ��
	NVS_FILE_TIME		tEndTime;			//��¼����ʱ��
	int  				iAge;				//��¼����ֵ
	int                 iSex;				//��¼�Ա�1-�У�2-Ů��3-δ֪
	int					iNation;			//��¼���壺1-���壬2-��������
	int					iWearGlasses;		//��ʾ���۾���0-Ԥ����1-�����2-δ���
	int					iWearMask;			//��ʾ�����֣�0-Ԥ����1-�����2-δ���
	int					iSimilarity;		//��ʾ���ƶȣ���Χ0-100
	VcaFileAttr			tPicSnap;			//ץ��ͼ����
	VcaFileAttr			tPicNeg;			//������ͼ����
	char				cLibUUID[LEN_UUID];	//��������ƽ̨��ID
	char				cFaceUUID[LEN_UUID];//ͼƬ��ƽ̨��ӦID
	char				cName[LEN_64];		//����
}FaceSearchSnapResult, *pFaceSearchSnapResult;

#define TYPE_SNAP_AGRIBANK    20000
typedef struct tagSnapInfo
{
	int         iSize;
	int         iQValue;
	char        cLocalFileName[LEN_256];
}SnapInfo, *pSnapInfo;

typedef struct tagSnapInfoOut
{
	int         iSize;
	int         iQValue;
	char        cRemoteFileName[LEN_64];
}SnapInfoOut, *pSnapInfoOut;

#define MAX_PIC_TYPE_NUM   2
typedef struct tagPicStreamUploadParam
{
	int         iSize;			
	int         iSceneId;		//range:0~15
	int         iRuleNo;		//range:0��7
	int         iPicType;		//Picture type: 0-Background big picture, 1-Small picture
	int         iSnapEnable;	//Send enable:0-not upload, 1-upload
	int         iIsOsd;			//Overlay information: 0-not stack, 1-stack.When iPicType=1,this parameter is invalid.
	int         iQpvalue;		//Snap picture quality range:1-100
	int			iFaceFrameEnable;//Overlay face frame: 0-not stack, 1-stack.When iPicType=1,this parameter is invalid.
}PicStreamUploadParam, *pPicStreamUploadParam;

typedef struct tagITSLatestBkPlate
{
	long long lLicenseID;
	char      cLicense[LEN_32];
}ITSLatestBkPlate, *pITSLatestBkPlate;

typedef struct tagITSAlarmInfo
{
	NVS_FILE_TIME tTime;
	char          cLicense[LEN_32];
	char          cAddressName[LEN_128];
	char          cAreaName[LEN_128];
	char          cAlarmName[LEN_128];
}ITSAlarmInfo, *pITSAlarmInfo;

typedef struct tagHumanDetectArithmetic
{
	int				iSize;			
	int				iSceneID;			//Scene id(0~15)
	int				iRuleNo;			//0-7
	int				iSnapEnable;		//0-disenable,1-enabele
	int				iDisplayTarget;		//display target box, 0-not dispaly, 1-dispaly
	int				iMinSizeEx;			//minimum face size(0~10000 percentage of image width��10000 indicates the width of entire screen)
	int				iMaxSizeEx;			//maximum face size(0~10000 percentage of image width,10000 indicates the width of entire screen)
}HumanDetectArithmetic, *pHumanDetectArithmetic;

typedef struct tagSIPEncrypt
{
	int     iMode;
}SIPEncrypt,*pSIPEncrypt;

typedef struct tagSIPPublicKey
{
	char sPublicKey[LEN_256];
}SIPPublicKey,*pSIPPublicKey;

typedef struct __tagDormancyWork
{
	int iSize;
	int iBeginHour;
	int iBeginMinute;
	int iEndHour;
	int iEndMinute;
	int iWorkType;  //In bits,1-dormancy, 0-non-dormancy:Bit 0-white light, Bit 1-infrared light, Bit 2-laser, Bit 3-fan, Bit 4-speaker, Bit 5-wiper, Bit 6-motor H, Bit 7-motor V
    int iEnable;   //Whether Timing Sleep is Enabled: 0 Enabled, 1 Not Enabled
}TDormancyWork, *pTDormancyWork;

typedef struct __tagDormancySchedule
{	
	int iSize;
	int iWeekDay;  //Sunday - Saturday : 0-6
	TDormancyWork tWork[MAX_SCHEDULE];
}TDormancySchedule, *pTDormancySchedule;

typedef struct __tagDormancyState
{
	int iSize;
	int iEnable;  //whether the device is in Dormancy state:In bits,1-dormancy, 0-non-dormancy:Bit 0-white light, Bit 1-infrared light, Bit 2-laser, Bit 3-fan, Bit 4-speaker, Bit 5-wiper, Bit 6-motor H, Bit 7-motor V
}TDormancyState, *pTDormancyState;

typedef struct __tagNetReduceFlowCtrl
{
	int iSize;
	int iEnable;//Bit 0 - Information report, Bit1 - picture return; 0 - enabled, 1 - no enabled
}TNetReduceFlowCtrl, *pNetReduceFlowCtrl;

typedef struct __tagDormancySet
{
	int iSize;
	int iEnable;//whether the device is in Dormancy state:In bits,1-dormancy, 0-non-dormancy:Bit 0-white light, Bit 1-infrared light, Bit 2-laser, Bit 3-fan, Bit 4-speaker, Bit 5-wiper, Bit 6-motor H, Bit 7-motor V
	int iResult;//0-succ,1-failed,2-over times
}DormancySet, *pDormancySet;

#define FILTER_CONDITION_MAX_NUM    64		//max number of filter conditions
#define FILTER_CONDITION_LEN		16		//Length of a filter condition
typedef struct tagITSIllegalParkFilterPlate
{
	int  iSize;		
	int  iSceneId;		//Scene ID,0-15
	int  iAreaNo;		//Area No,0-7
	int  iEnable;		//License plate filtering enabled,0: not enable,1:enable
	int  iFilterNum;	//number of filter conditions
	char cFilterCondition[FILTER_CONDITION_MAX_NUM][FILTER_CONDITION_LEN]; //Filter field
}ITSIllegalParkFilterPlate, *pITSIllegalParkFilterPlate;

#define PARKCARNUM_MAX_LEN          32
typedef struct tagITSParkCarNum
{
	int  iSize;
	int  iChannel;
	char cCarNum[PARKCARNUM_MAX_LEN];
	int  iParkStatus;   //0:no changes; 1:start(first report); 2:end(car leaves or record finished).
}ITSParkCarNum, *pITSParkCarNum;

typedef struct __tagVirtualGaugeCalib
{
	int iSize;       
	int iChannel;		 
	int	iScene;	       //scene num:0-15
	int	iType;	       //0:base water point, 1:Ordinary point
	int	iNum;	       //iType=0,iNum=0;iType=1,iNum=0-30
	int	iPosX;	       //per ten thousand (0-10000)
	int	iPosY;	       //per ten thousand (0-10000)
	int iMarkPosX;     //range:0-10000
	int iMarkPosY;     //range:0-10000
}VirtualGaugeCalib, *pVirtualGaugeCalib;

#define MAX_VCA_PEPT_POINT_NUM			 8
typedef struct tagVCAPEPT
{
	VCARule				tRule;					//Rules General Parameters
	vca_TDisplayParam	tDisplayParam;			//Display The Parameter Settings
	int					iDisplayTarget;			//Display Target, 0-Not Dispaly, 1-Dispaly
	int					iMaxSize;				//Max Target Size(Reserved:Percentage of image width)
	int					iMinSize;				//Min Target Size(Reserved:Percentage of image width)
	int					iSensitivity;			//Sensitivity
	int					iModelType;				//bit0: Zone Invasion bit1: Exception residency
	int					iTargetType;			//Target Type,bit1:Car bit2:SUV bit3:Pickup truck bit4:Tanker bit5:Van van bit6:Big truck bit7:Forklift bit8:Excavator bit9:Engineering bit10:Two-wheeler bit11:Tricycle bit31:Other types
	int 				iTimeThreshold;			//Resident Alarm Time
	int					iPointNum;				//Polygon Area Vertex Number
	vca_TPoint     		stPoints[MAX_VCA_PEPT_POINT_NUM];	//Vertex Coordinates		
}VCAPEPT, *pVCAPEPT;

#define MAX_CHEFHATDETECT_POINT_NUM			8
typedef struct tagVCAChefHatDetect
{
	int					iSceneID;				//scene ID(0~31)
	int					iRuleID;				//rule ID(0~15)
	int					iSensitivity;			//Sensitivity 0~100
	int					iPushMode;				//0: Reserved, 1: fastest, 2: best
	int					iPointNum;				//Polygon Area Vertex Number
	vca_TPoint     	stPoints[MAX_CHEFHATDETECT_POINT_NUM]; //Vertex Coordinates		
}VCAChefHatDetect, *pVCAChefHatDetect;

#define MAX_CHEFMASKDETECT_POINT_NUM		8
typedef struct tagVCAChefMaskDetect
{
	int					iSceneID;				//scene ID(0~31)
	int					iRuleID;				//rule ID(0~15)
	int					iSensitivity;			//Sensitivity 0~100
	int					iPushMode;				//0: Reserved, 1: fastest, 2: best
	int					iPointNum;				//Polygon Area point Number
	vca_TPoint     	stPoints[MAX_CHEFMASKDETECT_POINT_NUM]; //Vertex Coordinates		
}VCAChefMaskDetect, *pVCAChefMaskDetect;

#define CHEFDETECT_TYPE_CLOTHES			0	//chef's clothes
#define MAX_CHEFDETECT_TYPE				1	//max chef detect type
#define CHEFDETECT_PUSHPICMODE_RESERVE	0	//push graph strategy reserve
#define CHEFDETECT_PUSHPICMODE_FAST		1	//push graph strategy fast
#define CHEFDETECT_PUSHPICMODE_BEST		2	//push graph strategy best
#define MAX_CHEFDETECT_POINT_COUNT		8	//max number of vertices in polygon area 
typedef struct tagVCAChefDetect
{
	int					iType;					//input para:0: to detect chef's clothes
	int					iSceneID;				//input para:scene ID(0~31)
	int					iRuleID;				//input para:rule ID(0~15)
	int					iSensitivity;			//Sensitivity 0~100
	int					iPushMode;				//0: Reserved, 1: fastest, 2: best
	int					iPointCount;			//Polygon Area point count
	vca_TPoint     		tPoints[MAX_CHEFDETECT_POINT_COUNT]; //Vertex Coordinates		
} VCAChefDetect, *pVCAChefDetect;

typedef struct tagVCASenceSnap
{
	int		iSnapEnable;						//Scene Snap,  1-enable 0-not enable
}VCASenceSnap, *pVCASenceSnap;

typedef struct __tagVirtualGaugeCalibResult
{
	int iSize;
	int iChannel;		
	int	iScene;	       //scene num:0-15
	int	iType;	       //0:base water point, 1:Ordinary point
	int	iNum;	       //iType=0,iNum=0;iType=1,iNum=0-30
	int	iPosX;	       //per ten thousand (0-10000)
	int	iPosY;	       //per ten thousand (0-10000)
	int iMarkPosX;     //range: 0-10000
	int iMarkPosY;     //range: 0-10000
	int iDistance;
	int iAltitude;     //RTK data
	int iTiltAngle;    //Angle
	int iWaterLevel;   //Water Level,iType = 0  valid
	int iRiverWidth;   //the River of Width iType = 0  valid
	int iResult;       //Calib Result,0:success, -1:failed
}VirtualGaugeCalibResult, *pVirtualGaugeCalibResult;


typedef struct __tagVirtualGaugeParam
{
	int  iSize;
	int  iScene;            //scene num:0-15
	int  iType;             //0:base water point, 1:Ordinary point
	int  iSum;              //total num
	int  iNum;              
	int  iPosX;             //per ten thousand (0-10000)
	int  iPosY;             //per ten thousand (0-10000)
	int  iMarkPosX;         //range:0-10000
	int  iMarkPosY;         //range:0-10000
	int  iDistance;
	int  iAltitude;         //RTK data
	int  iTitleAngle;        //Angle
	int  iWaterLevel;       //Water Level,iType = 0  valid
}VirtualGaugeParam, *pVirtualGaugeParam;


typedef struct __tagUploadStateInfo
{
	int      iSize;
	int      iStation;
}UploadStateInfo, *pUploadStateInfo;

typedef struct __tagSetQueryFlow
{
	int               iSize;
	int               iChn;
	int               iPageNo;        //page num
	int               iPageSize;      //info num in every page
    NVS_FILE_TIME     tStartTime;     //query from time
	NVS_FILE_TIME     tEndTime;       //query end time
}SetQueryFlow, *pSetQueryFlow;

typedef struct __tagSetQueryFlowEmbed
{
	int               iSize;
	int               iChn;
	int               iPageNo;
	int               iPageSize;
	int               iStartTime;
	int               iEndTime;
}SetQueryFlowEmbed, *pSetQueryFlowEmbed;

typedef struct __tagQueryFlowInfo
{
	int                iSize;
	int                iTotal;	       //total info num
	int  	           iPageNo;	       //current page no
	int                iPageSize;	       //current info num in current page
	int    	           iIndex;           //info index
	int                iPlace;           //Station number
	int                iScene;           //Scene num
	unsigned int       iGroup;           
	NVS_FILE_TIME      tRecordTime;     //Record time
	int                iWaterSpeed;	   //Water Speed
	int                iWaterLevel;	   //Water Level
	int                iDistance;	
	int                iSource;			//0 Algorithm Get; 1 Peripheral Get
	int                iRuleID;			//0~7
	int		           iDetectNum;        //Detection points  1~30
	int                iCurrentNum;		//Current nod number
	NVS_FILE_TIME      tStartTime;     //Video start time
}QueryFlowInfo, *pQueryFlowInfo;

typedef struct __tagQueryFlowInfoEmbed
{
	int                iSize;
	int                iTotal;	       //total info num
	int  	           iPageNo;	       //current page no
	int                iPageSize;	       //current info num in current page
	int    	           iIndex;           //info index
	int                iPlace;           //Station number
	int                iScene;           //Scene num
	unsigned int       iGroup;           
	int                iRecordTime;     //Record time
	int                iWaterSpeed;	   //Water Speed
	int                iWaterLevel;	   //Water Level
	int                iDistance;	
	int                iSource;			//0 Algorithm Get; 1 Peripheral Get
	int                iRuleID;			//0~7
	int		           iDetectNum;        //Detection points  1~30
	int                iCurrentNum;		//Current nod number
	int                iStartTime;     //Video start time
}QueryFlowInfoEmbed, *pQueryFlowInfoEmbed;

typedef struct __tagAutoJust
{
	int					iSize;
	int					iChanNo; //channel no
}AutoJust, *pAutoJust;

typedef struct __tagSceneHeight
{
	int                 iSize;
	int                 iChannel;
}SceneHeight, *pSceneHeight;

typedef struct __tagSceneHeightResult
{
	int                 iSize;
	int                 iChannel;
	int                 iResult;
}SceneHeightResult, *pSceneHeightResult;

typedef struct _tagSinglePic
{
	int                 iSize;
	int                 iVoDevId; //1-VGA/HDMI0, 17-HDMI1(bt1120), 33-CVBS,49-VC,50-VC1��H264�� ,Default:VC
	int                 iAct;     //1:Entry SinglePic 2:Exit SinglePic
}SinglePic, *pSinglePic;



typedef struct tagGpsCoordinate
{
	int				iDirection;
	int				iDegree;
	int				iMinute;
	int				iSecond;
}GpsCoordinate, *pGpsCoordinate;

typedef struct tagGpsPointInfo
{
	GpsCoordinate		tLongitudeInfo;	//Longitude iDirection:(0-e, 1-w), degrees (0-180), minutes (0-59), seconds (0-59.99*100)
	GpsCoordinate		tLatitudeInfo;	//Latitude iDirection:(0-s, 1-n), degrees (0-180), minutes (0-59), seconds (0-59.99*100)
}GpsPointInfo, *pGpsPointInfo;

#define MAX_GPS_AREA_NUM			 128
#define MAX_GPS_POINT_NUM			 8
typedef struct tagGpsDangerArea
{
	int					iSize;
	int					iChanNo;	//channel no
	int					iAreaId;	//Area Id,range:0-127
	int					iPointNum;	//Number of vertices in the region,0 indicates the removal of the area.
	GpsPointInfo		tPointInfo[MAX_GPS_POINT_NUM];	//Point coordinates
}GpsDangerArea, *pGpsDangerArea;

#define MAX_POLYGON_POINT_NUM		 16
typedef struct tagNavigationShipDetection
{
	int					iSize;
	int					iChanNo;		//channel no
	int					iSceneId;		//range:0~31
	int					iRuleNo;		//range:0��15
	int					iSensitiv;		//sensitive:range:0~100
	int					iPointNum;		//Number of vertices in a polygon area,rang:3��8
	TPoint				tPoint[MAX_POLYGON_POINT_NUM];	//Point coordinates
}NavigationShipDetection, *pNavigationShipDetection;

typedef struct tagLocate3DPrediction
{
	int					iSize;
	int					iChanNo;		//channel no
	int					iTime;			//Prediction time: unit/s 1-30s
	int					iMultiple;		//Area magnification 1-5
}Locate3DPrediction, *pLocate3DPrediction;



#define MAX_CRUISE_ADD_COUNT  2
#define SET_AREA         1
#define CALL_AREA        2
#define DELETE_AREA      3
typedef struct __tagDetectAreaPoseX
{
	int                 iSize;
	int                 iSceneId;  //Scene Num:0-15
	int                 iCruiseAddMode; // 0:Auto;1:Manual
	int                 iAreaNum;  //0-127
	int                 iOperationType;//1-Set Area 2-Call Area 3-Detect Area
}DetectAreaPoseX, *pDetectAreaPoseX;

typedef struct __tagDetectAreaPoseXResult
{
	int                 iSize;
	int                 iResult;  //0:success, 1:fail
	int                 iSceneID;
	int                 iCruiseAddMode; //0:Auto;1:Manual
	int                 iOperationType;////1-Set Area 2-Call Area 3-Detect Area
	int                 iAreaNum;//0-127
    int                 iPtzP;
	int                 iPtzT;
	int                 iPtzZ;
	int                 iResultInfo;	//If the iresult field is not 0, it is valid to indicate the error information of the corresponding error code; if iresult = 3, it is the current diagonal distance in meters
}DetectAreaPoseXResult, *pDetectAreaPoseXResult;

typedef struct __tagDetectAreaParaX
{
	int                 iSize;
	int                 iSceneId;  //Scene Num:0-15
	int                 iCruiseAddMode; //0:Auto;1:Manual
	int                 iAreaNum;  //0-127
	int                 iState;//0:unset, 1;set
	int                 iPtzP;
	int                 iPtzT;
	int                 iPtzZ;
}DetectAreaParaX, *pDetectAreaParaX;

typedef struct tagCallParamResult
{
	int		iBufSize;					//Call gengeral parameters structure size
	int		iType;						//0-Call Scene ID��1-Call privacy shelter location 2- call vca scene 3 - enable intelligent scene quickly 4-quickly start warning scene, 5-fire detection shielding area, 6-smoke detection shielding area, 7-temperature difference detection shielding area
	int		iValue;						//Scene ID range��0~15�� privacy shelter location range��0~23
	int		iSceneType;					//0-vca 1-alert
	int     iResult;                    //0:success;other:failed
}CallParamResult, *pCallParamResult;

typedef struct tagHolidayScheTime
{
	int     iSize;
	int     iStartHour;
	int     iStartMin;
	int     iEndHour;
	int     iEndMin;
}HolidayScheTime, *pHolidayScheTime;

#define MAX_HOLIDAY_PLAN_ID   8
#define MAX_HOLIDAY_PARAM_ID  32
typedef struct tagHolidaySche
{
	int               iSize;
	int               iPlanID; //planid:0-7
	int               iEnable; //0:not set Sche, 1:SetSchedule
	char              pcPlanName[LEN_32];
	HolidayScheTime   tScheDule;
}HolidaySche, *pHolidaySche;

typedef struct tagSetPlanByDate
{
	int              iYear;
	int              iMonth;
	int              iDay;
}SetPlanByDate, *pSetPlanByDate;

typedef struct tagSetPlanByWeek
{
	int              iMonthofYear;
	int              iWeekofMonth;
	int              iDayofWeek;
}SetPlanByWeek, *pSetPlanByWeek;

typedef struct tagSetPlanByMonth
{
	int            iMonth;
	int            iDay;
}SetPlanByMonth, *pSetPlanByMonth;

typedef union tagPlanTime
{
	SetPlanByDate   tSetByDate;
	SetPlanByWeek   tSetByWeek;
    SetPlanByMonth  tSetByMonth;
	int             iCommonInt[LEN_4];
}PlanTime, *pPlanTime;

#define SET_HOLIDAY_BYDATE    0
#define SET_HOLIDAY_BYWEEK    1
#define SET_HOLIDAY_BYMONTH   2
#define SET_MAX_HOLIDAY_MODE  3
typedef struct tagHolidayPlan
{
	int               iSize;
	int               iHolidayID;
	int               iEnable;
	int               iPlanID;
	int               iSetMode;   ////Time format,0:By date (year month day) 1:By week(month  week) 2:By month(Month day)
	char              pcHolidayName[LEN_32];
	PlanTime          tStartTime;
	PlanTime          tStopTime;
}HolidayPlan, *pHolidayPlan;

typedef struct tagChannelSrc
{
	 int			iChnDst;
	 int			iSrcDst;
	 int			iEnable;
}ChannelSrc, *pChannelSrc;

typedef struct tagTrafficFlowQuery
{
	int            iSize;
	int            iChn;
	int            iPageNo;
	int            iPageSize;
	NVS_FILE_TIME  tStartTime;
	NVS_FILE_TIME  tEndTime;
}TrafficFlowQuery, *pTrafficFlowQuery;

typedef struct tagCarTypeNum
{
	int            iSize;
	int            iCarType;
	int            iCarNum;
}CarTypeNum, *pCarTypeNum;

typedef struct tagTrafficFlowInfo
{
	int            iSize;
	int            iChn;           //chn Number
	int            iTotalNum;      //total info numbers
	int            iPageNo;        //current page No
	int            iPageSize;      //info numbers in this pageNo
	int            iIndex;         //info No
	int            iLaneNo;        //0-5
	char           pcRoadName[LEN_64]; 
	NVS_FILE_TIME  tStartTime;
	NVS_FILE_TIME  tEndTime;
	int            iFlow;
	int            iHoldRate;      //percent
	int            iSpeed;         //Km/h
	int            iDistance;      //Second
	int            iCarTypeTotal;  
	CarTypeNum     tCarTypeNum[LEN_32];//carType1:num1,......carTypen:numN
	int            iCarQueueLength;         //mm
	int            iHeadDistance;           //mm
	int            iRoomRate;               //Spacing percent
	int            iRunState;               //state:0:Unknown 1:stop 2:slow 3:No car 4:fluent
	int            iSceneID;       //0~31
}TrafficFlowInfo, *pTrafficFlowInfo;

typedef struct tagTencentInfo
{
	int   iSize;
	char  pcURL[LEN_64];         //url address
	char  pcID[LEN_64];          //ID
	char  pcKey[LEN_64];
}TencentInfo, *pTencentInfo;

#define MAX_POLYGON_AREA_POINT_NUM		 8
typedef struct tagGranaryVehicleDetect
{
	int					iSize;
	int					iChanNo;		//channel no
	int					iSceneId;		//range:0~31
	int					iRuleNo;		//range:0��15
	int					iSensitiv;		//sensitive:range:0~100
	int					iPointNum;		//Number of vertices in a polygon area,rang:3��8
	TPoint				tPoint[MAX_POLYGON_AREA_POINT_NUM];	//Point coordinates
}GranaryVehicleDetect, *pGranaryVehicleDetect;

typedef struct tagGranaryVehicleDelay
{
	int					iSize;
	int					iChanNo;		//channel no
	int					iTime;			//unit:s 3D positioning operation is carried out after the vehicle's stationary time reaches the set time
	int					iPreset;		//Preset num
}GranaryVehicleDelay, *pGranaryVehicleDelay;

typedef struct tagModifyFrontPsd
{
	int           iSize;
	char          pcUserName[LEN_16];
	char          pcOldPassword[LEN_16];
	char          pcNewPassword[LEN_16];
}ModifyFrontPsd, *pModifyFrontPsd;

#define MAX_VCA_STRANDED_POINT_NUM			 10
typedef struct tagVCAStranded
{
	int					iDevType;				//0-IPC��1-NVR
	VCARule				tRule;					//Rules General Parameters
	vca_TDisplayParam	tDisplayParam;			//Display The Parameter Settings
	int					iDisplayTarget;			//Display Target, 0-Not Dispaly, 1-Dispaly
	int					iMaxSize;				//Max Target Size(Reserved:Percentage of image width)
	int					iMinSize;				//Min Target Size(Reserved:Percentage of image width)
	int					iSensitivity;			//Sensitivity
	int					iAlarmTime;				//Alarm Time
	int					iPointNum;				//Polygon Area Vertex Number
	vca_TPoint     		stPoints[MAX_VCA_STRANDED_POINT_NUM];	//Vertex Coordinates		
}VCAStranded, *pVCAStranded;

typedef struct tagVCAAlone
{
	VCACommonPara		tVCACommonPara;
	int					iAlarmTime;				//Alarm Time	
}VCAAlone, *pVCAAlone;

#define MAX_VCA_DELIVERGOODS_POINT_NUM			 10
typedef struct tagVCADeliverGoods
{
	int					iDevType;				//0-IPC��1-NVR
	VCARule				tRule;					//Rules General Parameters
	vca_TDisplayParam	tDisplayParam;			//Display The Parameter Settings
	int					iDisplayTarget;			//Display Target, 0-Not Dispaly, 1-Dispaly
	int					iMaxSize;				//Max Target Size(Reserved:Percentage of image width)
	int					iMinSize;				//Min Target Size(Reserved:Percentage of image width)
	int					iSensitivity;			//Sensitivity
	int					iPointNum;				//Polygon Area Vertex Number
	vca_TPoint     		stPoints[MAX_VCA_DELIVERGOODS_POINT_NUM];	//Vertex Coordinates		
}VCADeliverGoods, *pVCADeliverGoods;

typedef struct tagVCATops
{
	int					iChanNo;
	int					iDevType;				//0-IPC��1-NVR
	int					iSceneID;				//
	int					iVcaType;
	int					iTops;					//Reference value of calculation force occupied by this algorithm, range 1-100
	int					iMaxCnt;				//The maximum number of this algorithm to open, range 1-100
	int					iUsedCnt;				//The number of this algorithm has been opened, range 0-100
}VCATops, *pVCATops;


typedef struct tagShdbRunState
{
	int					iSize;
	int					iRunState;				//Running state, 0: not running; 1: running 
}ShdbRunState, *pShdbRunState;


#define MAX_SHDB_CHAN_NUM   1024 
typedef struct tagShdbAlarmPic
{
	int					iSize;
	int					iEnable;				//Alarm linkage enable, 0: not enable; 1: Enable 
	int					iPreTm;					//Pre shooting time, settable: 0, 1, 2, 3, 4, 5, 10, 20, 30, 40, 50, 60s 
	int					iDelayTm;				//Delay time, settable: 0, 1, 2, 3, 4, 5, 10, 20, 30, 40, 50, 60s 
	int					iIntervalTm;			//Time interval, settable: 3,4,5,10,20,30,40,50,60s 
	int					iChanCount;				//channel number
	int					iChanList[MAX_SHDB_CHAN_NUM];
}ShdbAlarmPic, *pShdbAlarmPic;


#define MAX_SHDB_TIMESNAPGROUPS_NUM   4
#define CUR_SHDB_TIMESNAPGROUP_NUM    2
typedef struct tagShdbTimeSnapGroups
{
	int					iTmEnable;				//Point in time enable, 0: not enable, 1: Enable 
	int					iPicCnt;				//Capture times, temporarily fixed to 1 
	int					iBeginTm;				//Start time unit s 
	int					iEndTm;					//End time in seconds 
	int					iChanCount;				//channel number
	int					iChanList[MAX_SHDB_CHAN_NUM];
}ShdbTimeSnapGroups, *pShdbTimeSnapGroups;

typedef struct tagShdbTimeSnap
{
	int					iSize;
	int					iEnable;								//Daily Photo enabled, 0: not enabled; 1: enabled 
	int					iTmSnapGrpNum;							//Number of time point capture groups, maximum 4 groups, currently only 2 groups are open 
	ShdbTimeSnapGroups  tParam[MAX_SHDB_TIMESNAPGROUPS_NUM];	//Time point capture group number structure 
}ShdbTimeSnap, *pShdbTimeSnap;



typedef struct tagQueryShdbApprepairSys
{
	int					iSize;
	int					iRepairName;		    //Name of repair reporting system, 0: building intercom; 1: perimeter alarm; 2: networking alarm; 3: local alarm; 4: video security; 5: access control; 6: electronic patrol system; 7: other security systems; 
	int					iRepairType;		    //Repair type: 0: front end; 1: terminal; 2: system;
}QueryShdbApprepairSys, *pQueryShdbApprepairSys;

typedef struct tagShdbApprepairSysResult
{
	int					iSize;
	int					iRepairName;		   //Name of repair reporting system, 0: building intercom; 1: perimeter alarm; 2: networking alarm; 3: local alarm; 4: video security; 5: access control; 6: electronic patrol system; 7: other security systems; 
	int					iRepairType;           //Repair type: 0: front end; 1: terminal; 2: system;
	int					iResult;               //Execution result, 0: success; - 1: failure 
}ShdbApprepairSysResult, *pShdbApprepairSysResult;



typedef struct tagQueryShdbServiceType 
{
	int					iSize;
	int					iMaintainType;		  //Maintenance type, 0: normal; 1: exception repaired; 2: exception not repaired; 3: not selected 
	int					iRepairType;          //Repair type, 0: normal; 1: abnormal repaired; 2: abnormal not repaired; 3: not selected 
}QueryShdbServiceType, *pQueryShdbServiceType;

typedef struct tagShdbServiceTypeResult
{
	int					iSize;
	int					iMaintainType;		 //Maintenance type, 0: normal; 1: exception repaired; 2: exception not repaired; 3: not selected 
	int					iRepairType;		 //Repair type, 0: normal; 1: abnormal repaired; 2: abnormal not repaired; 3: not selected 
	int					iResult;			 //Execution result, 0: success; - 1: failure 
}ShdbServiceTypeResult, *pShdbServiceTypeResult;



typedef struct tagQueryShdbTestMainTain
{
	int					iSize;
	int					iOperType;			 //Operation type: 0: upload test picture; 1: upload maintenance picture
	int					iChanCount;			 //channel number
	int					iChanSize;			 //struct ChannelList size
	ChannelList*		pChanList;			 //channel list
}QueryShdbTestMainTain, *pQueryShdbTestMainTain;

typedef struct tagShdbTestMainTainResult
{
	int					iSize;
	int					iOperType;			 //Operation type: 0: upload test picture; 1: upload maintenance picture
	int					iChanCount;          //channel number
	int					iChanSize;			 //struct QueryChanNo size
	ChannelList*		pChanList;			 //channel list
	int					iResult;			 //Execution result, 0: success; - 1: failure
}ShdbTestMainTainResult, *pShdbTestMainTainResult;


#define  MAX_SHDB_POLICELD_NUM    32
#define  MAX_SHDB_PASSWD_NUM      32


typedef struct tagQueryShdbCheckManage
{
	int					iSize;
	char				cPoliceId[MAX_SHDB_POLICELD_NUM];	//Officer, 32 Max 
	char				cPasswd[MAX_SHDB_PASSWD_NUM];		//Password, up to 32 bits 
	int					iChanCount;							//channel number
	int					iChanSize;							//struct ChannelList size
	ChannelList*		pChanList;							//channel list
}QueryShdbCheckManage, *pQueryShdbCheckManage;

typedef struct tagShdbCheckManageResult
{
	int					iSize;
	char				cPoliceId[MAX_SHDB_POLICELD_NUM];   //Officer, 32 Max 
	char				cPasswd[MAX_SHDB_PASSWD_NUM];		//Password, up to 32 bits 
	int					iChanCount;							//channel number
	int					iChanSize;							//struct ChannelList size
	ChannelList*		pChanList;							//channel list
	int					iResult;							//Execution result, 0: success; - 1: failure
}ShdbCheckManageResult, *pShdbCheckManageResult;

typedef struct tagHsjjFocusstate
{
	int						iSize;
	int						iRunState;							//0��δ�۽���ɣ�1���۽����
}HsjjFocusstate, *pHsjjFocusstate;

typedef struct tagAlmLoopDetec
{
	int                 iSize;        
	int                 iChannelNo;                   //channel number��0~N-1��ipc parameters of corresponding IPC channel�� 
	                                                  //0x7fffffff��NVR local parameters
	int                 iPortNo;                      //port number
	int                 iEnable;                      //whether to open��0, close��1, open
} AlmLoopDetec, *pAlmLoopDetec;

typedef struct tagIdentificationType
{
	int                 iSize;
	int                 iChannelNo;                    //channel number��0~N-1��ipc parameters of corresponding IPC channel��
	                                                   //0x7fffffff��NVR local parameters
	int                 iProtocol;		               //protocol type, 0��rtsp��1��http
	int                 ibasicConfig;	               //basic auth config, 0��forbid ��1��allow
	int                 iDigestlConfig;                //digest��֤����,  0��forbid ��1��allow
}IdentificationType, *pIdentificationType;

typedef struct tagDevAutoTimingParam
{
	int					iSize;
	int					iType;				//Timing type, 0-never, 1-year cycle, 2-month cycle, 3-week, 4-day 
	int					iWeekDay;			//Starting time: 1-7 from Monday to Sunday, valid when itype is 3 
	int					iMonth;				//Month, value range is 1-12, valid when itype = 1 
	int					iDay;				//Day, value range is 1-31, valid when itype = 1,2
	int					iHour; 				//Hour,value range is 0-23,valid When itype = 1,2,3,4
	int					iMinute;			//Minute,value range is 0-23,valid When itype = 1,2,3,4
	int					iSecond;			//Second,value range is 0-23,valid When itype = 1,2,3,4
}DevAutoTimingParam, *pDevAutoTimingParam;

typedef struct __tagIpFilter
{
	int iSize;
	char cIp[LENGTH_IPV4];
	char cSubMask[LENGTH_SUB_MASK];
	int iFilterType;								//Filter typr:0,Disable IP permission qualification;1,IP access is disabled;2,Allow IP access
	int iIpNum;										//Filter IP number,the maximum support 16 different IP addresses
	char cFilterIp[MAX_IP_FILTER_NUM][LENGTH_IPV4];	//Network address string, such as��pcFilterIp[0]=��192.168.1.10��
}TIpFilter, * pTIpFilter;

typedef struct __tagIpv6Filter
{
	int iSize;
	int iIpNum;												//Filter IP number,the maximum support 16 different IP addresses
	int iFilterType;										//Filter typr:0,Disable IP permission qualification;1,IP access is disabled;2,Allow IP access
	char pcFilterIp[MAX_IP_FILTER_NUM][LENGTH_IPV6_V1];		//Network address string, such as��pcFilterIp[0]=��1234:5678:9abc:def::1��
}TIpv6Filter, * pTIpv6Filter;

typedef struct tagIpv6FilterParam
{
	int					iSize;				
	int					iFilterType;														//��������(0-����IPȨ���趨,1-��ֹIP����,2-����IP����)
	int					iIpNum;																//����ip����(iIpNum ���֧��48����ͬ��IP��ַ)
	int					iPackageNum;														//�ܰ���
	int					iListIndex;															//�б����(��1��ʼ)
	int					iParamNum;															//�����б���(ÿ�η��͵��б���Ŀ)
	char				pcFilterIp[MAX_IPV6_PACKAGE_FILTER_NUM][LENGTH_IPV6_V1];			//IPv6��ַ(�����ַ�ַ������磺pcFilterIp1=��1234:5678:9abc:def::1����192.168.1.10������󳤶�Ϊ64�ֽڣ�)
}Ipv6FilterParam, * pIpv6FilterParam;

typedef struct tagCloudAutoDetect
{
	int					iSize;
	int					iChannelNo;				//Channel number, 1-NVR when upgrading ipc channel number starts from 0, 2-NVR cloud upgrade channel number is 0x7FFFFFFF
	int					iNewVerStat;			//is there a new version, 0-no new version, 1-new version
	char				cNewVer[LEN_64];		//new version, 64 bytes
	char				cReleaseData[LEN_32];	//Release time, 32 bytes, YYYY-MM-DD
}CloudAutoDetect, *pCloudAutoDetect;

typedef struct tagDeviceName
{
	int                 iSize;
	int                 iChannelNo;              //channel number��0~N-1��ipc parameters of corresponding IPC channel��
	                                             //0x7fffffff��NVR local parameters
	char                pcDeviceName[LEN_32];    //device name
}DeviceName, *pDeviceName;

typedef struct tagChnDevInfo
{
	int					iSize;
	int					iChannelNo;					// output para,Channel number, iChannelNo according to the actual situation to determine iChannelNo= 0~n-1
	char				cMac[LEN_64];				// output para,Mac address, no more than 64 bytes
	char				cFactoryID[LEN_64];			// output para,Factory ID, no more than 64 bytes
	char				cBarCode[LEN_64];			// output para,Bar code, no more than 64 bytes
}ChnDevInfo, *pChnDevInfo;

typedef struct tagDomainParsePara
{
	int				iSize;
	char			cUserID[LEN_16];							//username
	char			cPassWord[LEN_16];							//password
	char			cHostName[LEN_32];							//device name
	int				iInterval;									//Registration interval
	char			cDomain1[LEN_32];							//Server 1 domain name
	char			cDomain2[LEN_32];							//Server 2 domain name
	int				iPort1;										//Server port 1
	int				iPort2;										//Server port 2
	char			cDomain1v6[LENGTH_IPV6_V1];					//Server 1 domain name
	char			cDomain2v6[LENGTH_IPV6_V1];					//Server 2 domain name
}DomainParsePara, *pDomainParsePara;

typedef struct	tagNVS_NFS_DEVEx
{
	int		iSize;
	char	cDeviceIP[LEN_16];					//NFS IP
	char	cStorePath[LEN_250];				//NFS Path
	char	cUsername[LEN_16];					//User Name
	char	cPassword[LEN_16];					//Password
	int		iState;								
	char	cDeviceIPv6[LENGTH_IPV6_V1];		//IPv6
}NVS_NFS_DEVEx,*pNVS_NFS_DEVEx;

typedef struct tagWaterLevelSource
{
	int				iSize;				//input para: Structure waterlevelsource size 
	int				iChannelNo;			//input para: Channel number 
	int				iType;				//input para: Water level data source type, 0-reserved; 1-manual setting; 2-network access; 3-peripheral access,4-water level algorithm acquisition
	int				iValue;				//input para: Water level in mm, valid when itype = 1 	
}WaterLevelSource, *pWaterLevelSource;

typedef struct tagWaterLevelSourceID
{
	int				iSize;				//input para: Structure waterlevelsourceid size 
	int				iChannelNo;			//input para: Channel number 
	char			cFactoryID[LEN_64];	//input para: Factory id, no more than 64 bytes 
}WaterLevelSourceID, *pWaterLevelSourceID;


#define IPV6_ADDRLIST_NUM_MAX				16

#define IPV6_ADDRESS_MANUAL_MODE				1	//MANUAL
#define IPV6_ADDRESS_DHCP_MODE					2	//DHCP
#define IPV6_ADDRESS_ANNOUNCEMENT_MODE			3	//Routing announcement mode
#define IPV6_ADDRESS_MODE_MAX					4	//Mode max num

typedef struct tagIPV6AddrAndPrefix
{
	char			cIP[LENGTH_IPV6_V1];	
	int				iPrefixLen;			
}IPV6AddrAndPrefix, *pIPV6AddrAndPrefix;

typedef struct tagIPV6AddrList
{
	int					iSize;									//input para: Structure waterlevelsourceid size 
	int					iChannelNo;								//input para: Channel number 
	int					iLanNo;									//input para: Lan number 0-MAX_LAN_NUM
	int					iIPMode;								//input para: 1-MANUAL mode 2-DHCP mode ,3-Routing announcement mode
	int					iIPListNum;								//output para: IP num
	IPV6AddrAndPrefix	tIPInfo[IPV6_ADDRLIST_NUM_MAX];			//ooutput para: IP address list
}IPV6AddrList, *pIPV6AddrList;


#define MAX_SMOKEDETECT_POINT_NUM			8
typedef struct tagVCASmokeDetect
{
	int					iSceneID;						     //scene ID(0~31)
	int					iRuleID;						     //rule ID(0~15)
	int                 iValid;					             //0;invalid; 1:valid(default:0)
	int					iSensitivity;					     //Sensitivity 0~100
	int					iPointNum;						     //Polygon Area point Number
	vca_TPoint     	    stPoints[MAX_SMOKEDETECT_POINT_NUM]; //Vertex Coordinates		
}VCASmokeDetect, *pVCASmokeDetect;

#define MAX_PHONEDETECT_POINT_NUM		8
typedef struct tagVCAPhoneDetect
{
	int					iSceneID;						     //scene ID(0~31)
	int					iRuleID;						     //rule ID(0~15)
	int                 iValid;					             //0;invalid; 1:valid(default:0)
	int					iSensitivity;					     //Sensitivity 0~100
	int					iPointNum;						     //Polygon Area point Number
	vca_TPoint     	    stPoints[MAX_PHONEDETECT_POINT_NUM]; //Vertex Coordinates		
}VCAPhoneDetect, *pVCAPhoneDetect;

#define MAX_SLUICEGATE_CEIL_COUNT	10	//MAX_VCA_SLUICEGATE_COUNT*2

typedef struct tagCeilLineInfo
{	
	vca_TPoint			tPointStart;			
	vca_TPoint			tPointEnd;	
}CeilLineInfo, *pCeilLineInfo;

typedef struct tagVcaSluiceGateCeil
{
	int					iSize;										//input para:Structure size
	VCARule				tRule;										//input para
	int					iCeilLineCount;								//input para:Ceil line count
	CeilLineInfo		tCeilLineInfo[MAX_SLUICEGATE_CEIL_COUNT];	//output para:Ceil line point info
}VcaSluiceGateCeil, *pVcaSluiceGateCeil;

typedef struct tagOsdClarity
{
	int				iSize;								// input para Structure size
	int				iOsdType;							// input para Character type, 0: channel name 1: Date Time 2: additional character 1 3: additional character 2 4: additional character 3 5: additional character 4 6: additional character 5 
	int				iBClarity;							// input para Background transparency, 0-100, smaller, more transparent 
}OsdClarity, *pOsdClarity;

typedef struct tagLedPowerLimit
{
	int				iSize;							// output para Structure size
	int				iNearLEDPower;					// output para The power consumption per unit brightness of the approach lamp is increased by 1000 times. For example, when the power consumption is 0.05W, the value of this field is 50, the same below 
	int				iFarLEDPower;					// output para The power consumption per unit brightness of the far lamp is increased by 1000 times 
	int				iBasePower;						// output para The basic power consumption of the system is increased by 1000 times 
	int				iBasePowerRatio;				// output para Basic power consumption coefficient of the system 
	int				iOtherPower;					// output para Other power consumption, actual power consumption increased by 1000 times 
	int				iOtherRatio;					// output para Other coefficient, multiple of other power consumption when calculating power consumption 
	int				iLimitPower;					// output para Limit the power consumption value, and increase the actual power consumption value by 1000 times 
}LedPowerLimit, *pLedPowerLimit;

typedef struct tagFaceOsdTimeFormat
{
	int             iSize;  // input para Structure size
	int             iCapType;  
	char            cYearText[LEN_8];  //Format symbol after year
	char            cMonthText[LEN_8]; //Format symbol after month
	char            cDayText[LEN_8];   //Format symbol after day
	char            cHourText[LEN_8];  //Format symbol after hour
	char            cMinText[LEN_8];   //Format symbol after min
	char            cSecText[LEN_8];   //Format symbol after sec
	char            cMilsecText1[LEN_8];//Format symbol after milsec1
	char            cMilsecText2[LEN_8];//Format symbol after milsec2
}FaceOsdTimeFormat, *pFaceOsdTimeFormat;

typedef struct tagFaceOsdWordOverlay
{
	int             iSize;        //input para Structure size
	int             iOsdDisplayIndex;   //
	int             iEnable;            //control context show
	char            pcOsdName[LEN_32];  //osd name
	int             iPosX;              //PosX
	int             iPosY;              //PosY
	int             iZoomScale;         //[0-500].100 represents no change
	int             iLineCharNum;       //Maximum characters per line
	int             iBlankCharNum;      //Number of starting spaces after line brea
	int             iOsdPosNo;          //osd Postion Number
	int             iCapType;           //capture type
	int             iFacePicType;       //small picture size:0,default;1.small;2.middle;3,big
}FaceOsdWordOverlay, *pFaceOsdWordOverlay;

typedef struct tagFaceOsdCommonDev
{
	int             iSize;  //input para Structure size
	int             iOsdType;//0-reserved, 1-mechanism code, 2-mechanism name, 3-capture image character overlay bit, 6-composite image character overlay bit, 7-face overlay OSD
	char            cCommonInfo[LEN_64];//ods content
	int             iCapType;//capture type
}FaceOsdCommonDev, *pFaceOsdCommonDev;

typedef struct tagImageLocation
{
        int            iSize; //input para Structure size
        int            iLocation; //0-normal��1-turnover��2-h_mirror��3-v_mirror
}ImageLocation, *pImageLocation;

typedef struct tagPtzResult
{
	int iSize;
	int iPtzP; //0~3600000 [0-360]��accurancy��0.0001
	int iPtzT; //100000~1900000 [-90 - 90]accurancy��0.0001
	int iPtzZ; //0~100000 [0-1000]��accurancy:0.01
}PtzResult, *pPtzResult;

typedef struct
{
	int         m_iStructSize;        //Structure size
	char        m_cVersion[LEN_32];       //Server version number
	char		m_cUIVersion[LEN_32];	  //UI Server version number
	char		m_cSlaveVersion[LEN_32];  //From film version number
	char        m_cPlugInVersion[LEN_32]; //Plug-in version number 
	char		m_cSCMVersion[LEN_32];	  //Single chip version number
	char 		m_cSCGuiVersion[LEN_32];  //UI version number
	char		m_cMediaVersion[LEN_32];  //T2 Camera Media Version
	char		m_cMethodVersion[LEN_64];		//Method Version
	char		m_cCpldVersion[LEN_64];			//Cpld Version
	char		m_cDomeDriveVersion[LEN_64];	//DomeDrive Version
	char		m_cDigitalMovementVersion[LEN_64];  //Digital Movement Version
	char		m_cTradeP2PVersion[LEN_64];			//p2p Version
	char		m_cTradeOnvifVersion[LEN_64];		//Onvif Version
	char		m_cTradeItsVersion[LEN_64];			//Its Version
	char		m_cTrade28181Version[LEN_64];		//28181 Version
	char		m_cTradeZFZJVersion[LEN_64];		
	char		m_cTradeH323Version[LEN_64];		
	char		m_cTradeRTSPVersion[LEN_64];		
	char		m_cTradeRTMPVersion[LEN_64];
	char		m_cHpdVersion[LEN_64];
	char		m_cFrontPanelVersion[LEN_64];
	char		m_cAlgoModelVersion[LEN_64];
	char		m_cIntelligentModuleVersion[LEN_64];	//Intelligent module Version
	char		m_cLandmarkVersion[LEN_64];				//Landmark Version
	char        m_cNetSdkVersion[LEN_32];
	char        m_cNetOcxVersion[LEN_32];
	char		m_cSecondDomeDriverVersion[LEN_64];				//2nd M3  Driver
	char		m_cEncrypChipSupportFirmwareVersion[LEN_64];
}SERVER_VERSION;

#define BURN_BACKUP_FILE_ALL         0
#define BURN_BACKUP_FILE_ONLYPEN     1
typedef struct tagBurnBackupFile
{
    int 	  iSize;  			//[IN]�ṹ���С
    int	      iBurnFile;	    //[IN] BURN_BACKUP_FILE_ALL���������������ļ�  BURN_BACKUP_FILE_ONLYPEN��ֻ���ݱ�¼�ļ�
}BurnBackupFile, *pBurnBackupFile;

#define RET_BURN_SUCCESS       0 //���óɹ�
#define RET_BURN_FAILED        1 //����ʧ��
#define RET_BURN_RUNING        2 //��ǰ���ڿ�¼���������޸�����


#define EASYDDNS_LINK_OFFLINE	0
#define	EASYDDNS_LINK_ONLINE	1
typedef struct tagEasyDDNSLinkState
{
	int iSize;
	int iLinkState;	//0:offline, 1:online
} EasyDDNSLinkState, *pEasyDDNSLinkState;

typedef struct tagModulePowerLimit
{
	int				iSize;				// output para �ṹ���С
	int				iLimitPower;		// output para �����ܹ���ֵ, ʵ�ʹ���ֵ����1000��
	int				iModuleType;		// output paraģ�����ͣ�0������ƣ�1���׹�ƣ�2������ƣ�3-9��Ԥ��
	int				iPowerRatio1;		// output para ģ�鹦��ϵ��1, ģ��Ϊ�����׹��ʱ��ʾ������ÿ��λ���ȱ仯�Ĺ���ֵ��ģ��Ϊ�����ʱ��ʾ�����ÿ��λ���ȱ仯�Ĺ���ֵ��ʵ������1000������֧����Ϊ0������ģ�������������
	int				iPowerRatio2;		// output para ģ�鹦��ϵ��2, ģ��Ϊ�����׹��ʱ��ʾԶ����ÿ��λ���ȱ仯�Ĺ���ֵ��ʵ������1000������֧����Ϊ0������ģ�������������
	int				iPowerRatio3;		// output para ģ�鹦��ϵ��3, ģ��Ϊ�����ʱ��ʾ�й���ÿ��λ���ȱ仯�Ĺ���ֵ��ʵ������1000������֧����Ϊ0������ģ�������������
	int				iPowerRatio4;		// output para ģ�鹦��ϵ��4, Ԥ������֧����Ϊ0
	int				iLightID;			// output para ������, 0��ʾ����1��1��ʾ����2
}ModulePowerLimit, *pModulePowerLimit;

typedef struct tagManualPortMap
{
    int  iSize;
    int  iHttpWanPort;			// httpЭ������ӳ��˿ں�
    int  iRtspWanPort;			//rtspЭ������ӳ��˿ں�
    int  iServerWanPort;		//˽��Э������ӳ��˿ں�
    int  iHttpsWanPort;			// httpsЭ������ӳ��˿ں�
    int  iRtmpWanPort;		    // rtmpЭ������ӳ��˿ں�
} ManualPortMap, *pManualPortMap, ManualPortMapResult, *pManualPortMapResult;

#define OSDTYPE_TIME						0x01
#define OSDTYPE_TITLE						0x02
#define OSDTYPE_LOGO						0x04
#define OSDTYPE_ITS							0x05

typedef struct tagNorthAngle
{
	int iSize;
	int iAngle; //0~3600000 [0-360]��accurancy��0.0001
}NorthAngle, *pNorthAngle;

typedef struct tagCommonRECT 
{ 
	int left; 
	int top; 
	int right; 
	int bottom; 
}CommonRECT, *pCommonRECT;

typedef struct tagIEEE8021X
{
	int iSize;
	int iLanNo; //0��lan1  1:lan2��.
	int iEnable;	//0:not enable   1:enable
	int iConnectType;	//0:automic  1:manual
	int iEapType;		//0��EAP-MD5
	int iEapolVersion;	//1��IEEEStd 802.1X-2001  2��IEEEStd 802.1X-2004
	char cUserName[LEN_64];	//user name
	char cPassWord[LEN_64];	//password

}IEEE8021X, *pIEEE8021X;

#define    MAX_ADDRESS_TYPE			2
typedef struct tagCommonRtmp
{
	int iSize;
	int iStreamType;	//0:main 1:sub  2:three
	int iAddressType; //1Custom 2Non-custom 
	int iEnable; //	0:not enable  1:enable
	int iPort; //0~65535
	char cIpAddress[LEN_256]; //Ip Address
	char cUserName[LEN_64];	//user name
	char cPassWord[LEN_64];	//password
	char cKey[LEN_64];	//key
}CommonRtmp, *pCommonRtmp;

typedef struct tagIEEE8021XState
{
	int iSize;
	int iLanNo;			 //0��lan1  1:lan2��.
	int iConnectState;	//0:not connect 1:connected  2:connecting 3:connect failed
}IEEE8021XState, *pIEEE8021XState;


typedef struct tagWaterSpeedFieldInfo
{
	int					iSize;			
	int					iChanNo;		//channel no
	int					iEnable;		//0-close;1-open
	int					iResult;		//output para:0-success;1-failed
}WaterSpeedFieldInfo, *pWaterSpeedFieldInfo;

#define MAX_AUTOTESTMULT_PARAM_NUM    16
typedef struct tagAutotestMultQuery
{
	int                 iSize;
	int					iChannelNo;			
	int					iTestItem;		//Debugging items:1-Calibration of PTG pot cover;
					                    //2-Calibration of PTG temperature; 3-Calibration of PTG distance;
	int					iParamNum;		                                
	char			    pcParam[MAX_AUTOTESTMULT_PARAM_NUM][LEN_65]; 
}AutotestMultQuery, *pAutotestMultQuery;

typedef struct tagAutotestMultResult
{
	int                 iSize;
	int					iChannelNo;			
	int					iTestItem;		//Debugging items:1-Calibration of PTG pot cover;
										//2-Calibration of PTG temperature; 3-Calibration of PTG distance;
	int					iTestResult;
	char                cTestInfo[LEN_65];
	int					iParamNum;		                                
	char			    pcParam[MAX_AUTOTESTMULT_PARAM_NUM][LEN_65]; 
}AutotestMultResult, *pAutotestMultResult;

#define MAX_TEMP_MODEL_TYPE 3
typedef struct tagVCATemDetect
{
	int                 iSize;
	int					iSceneID;						     //scene ID(0~31)
	int					iRuleID;						     //rule ID(0~15)
	int                 iValid;					             //0:invalid; 1:valid(default:0)
	int					iDisplayTemScaleEnable;			     //0:not display 1:display (default:0)
	int					iHighTemColor;						 //1:red 2:green 3:yellow 4:blue 5:purple 6:cyan 7:black 8:white (default:1��
	int					iLowTemColor;						 //1:red 2:green 3:yellow 4:blue 5:purple 6:cyan 7:black 8:white (default:1��		
	int					iModelType;						     //1:Ambient temperature difference alarm 2:Environmental high temperature alarm 3:human high temperature alarm	
	int					iTemUnit;						     //0:centigrade 1:Fahrenheit; 		
	int  				iTemThreshold;	                     //The threshold value of temperature alarm is determined according to the imodeltype, and the value is the actual temperature * 100 
	int					iWaitTime;                           //Unit: Second	
	int                 iTempLoseEnable;                     //Temperature abnormal alarm enable, 0-disable, 1-enable.(default:0)
}VCATemDetect, *pVCATemDetect;

typedef struct tagVcaScanList
{
	int                iSize;
	int                iVcaType;
	int                iSupport;                             //0-not support 1-support scan list
}VcaScanList, *pVcaScanList;

typedef struct tagVcaFireWork
{
	int               iSize;
	int               iSceneId;//range:[0-31],default:[0-15]
	int               iRuleID; //range:[0-15],default:0
	int               iValid;  //0:unvalid,1:valid
	int               iDisplayRule;//0:not display,1:display
	int               iDisplayStat;//0:not display,1:display
	int               iDisplayTarget;//0:not display,1:display
	int               iColor;      //1��red 2��green 3��yellow 4��blue 5��purple 6��Cyan 7��black 8��white  ��default:2��
	int               iAlarmColor; //1��red 2��green 3��yellow 4��blue 5��purple 6��Cyan 7��black 8��white  ��default:1��
	int               iFireSensitiv;//fire sensitive,range:[0-100],default:50
	int               iSmogSensitiv;//smoke sensitive,range:[0-100],default:50
	int               iModelType;//fire work test model:0:default��1:fire point detection��2:Smoke detection��3:fire point detection or Smoke detection��4:fire point detection and Smoke detection
	int               iWaitTime;//find alarm waiting time,range[0-600]s,default:0
}VcaFireWork, *pVcaFireWork;


#define  MAX_MASKAREA_TYPE  4
#define  MAX_MASKAREA_NO    48
typedef struct _tagMaskAreaEnable
{
	int             iSize;						//size of struct
    int             iMaskType;                  //0-reserved, 1-pyrotechnic detection - fire point shielding area, 2-pyrotechnic detection - Smoke shielding area, 3-temperature detection shielding area
	int             iEnable;                    //0��unenable  1��enable  (default:0)
}MaskAreaEnable, *pMaskAreaEnable;

#define SDK_MAX_MASKAREA_NUM   12
typedef struct _tagMaskAreaPara
{
	int             iSize;
    int             iMaskType; //0-reserved, 1-pyrotechnic detection - fire point shielding area, 2-pyrotechnic detection - Smoke shielding area, 3-temperature detection shielding area
    int             iAreaNo;//AreaNo:1-48
	vca_TPolygonEx  tPoint;
}MaskAreaPara, *pMaskAreaPara;


typedef struct tagSingleScreenCalParam
{
	int                 iChanNo;
	TPoint              tPoint;               
}SingleScreenCalParam, *pSingleScreenCalParam;

#define MAX_CALIBRATION_SCREEN_COUNT    32
typedef struct tagCalibrationInfo
{
	int                     iSize;           
	int                     iCalPtCount;	   //Calibration points count, max value 16.
	int                     iCalPtNo;          //Current calibration point number, 1~16.
	int                     iCalScreenCount;   //Screens count of calibration channel, max value 32.
	SingleScreenCalParam    tParam[MAX_CALIBRATION_SCREEN_COUNT]; //Every screen parameter, including channel number & points information corresponding to the each screen.
}CalibrationInfo, *pCalibrationInfo;

typedef struct tagVisualRange
{
	int iSize;
	int iRange;				//visual range
	int iHview;				//hortional view angle, 0~36000
	int	iVview;				//vertical view angle, 0~36000
}VisualRange, *pVisualRange;

#define VIDEO_CONVOER_AREA			8	//��Ƶ�ڵ�������

typedef struct tagVideoCover
{
	int			iSize;
	int			iStreamNo;
	CommonRECT tRect[VIDEO_CONVOER_AREA];
}VideoCover, *pVideoCover;

typedef struct tagRadarStatusInfo
{
	int		iSize;			
	int		iType;						//radar type 0-reserve  1-�����״�
	char	cRadarVersion[LEN_64];		//Version
	int		iRadarStatus;				//Status 1-online, 0-offline
}RadarStatusInfo, *pRadarStatusInfo;

typedef struct tagTemperatureScaleType
{
	int			iSize;
	int         iChanNo;
	int         iTempStandard;          //temperature scale type, 0-reserved, 1-celsius, 2-fahrenheit, 3-kelvin 
}TemperatureScaleType, *pTemperatureScaleType;

typedef struct tagSingleBlackbodyParam
{
	int         iBlackBodyId;           //black body ID
	int         iBlackBodyTemp;         //temperature value * 100
	int         iBlackBodyTempUnit;     //temperature unit: 0-reserved, 1-celsius, 2-fahrenheit, 3-kelvin
	int         iBlackBodyDistance;     //unit: cm
	CommonRECT  tRect;
}SingleBlackbodyParam, *pSingleBlackbodyParam;

#define MAX_BLACKBODY_COUNT         2
typedef struct tagBlackbodyCorrection
{
	int			iSize;
	int         iChanNo;
	int         iBlackBodyCorrectEnable;//black body correct enable: 0-disable, 1-enable   
	int         iBlackBodyCorrectType;  //0-reserved, 1-single correction, 2-continuous correction 
	int         iBlackBodyNum;          //black body number: 1 or 2
	SingleBlackbodyParam tParam[MAX_BLACKBODY_COUNT];
}BlackbodyCorrection, *pBlackbodyCorrection;

typedef struct tagBodyTempCorrect
{
	int			iSize;
	int         iChanNo;
	int         iBodyTempCorrectEnable;          //body temperature correction enable:  0-disable, 1-enable 
	int         iBodyTempCorrectSensitivity;     //body temperature correction sensitivity: 0-100
}BodyTempCorrect, *pBodyTempCorrect;

typedef struct tagIntelligentCorretct
{
	int			iSize;
	int         iChanNo;
	int         iIntelligentCorrectEnable;		 //intelligent correction enable: 0-disable, 1-enable
	int         iIntelligentCorrectSensitivity;  //intelligent correction correction sensitivity: 0-100
}IntelligentCorretct, *pIntelligentCorretct;

typedef struct tagVcaScanArea
{
	int			iSize;
	int			iSceneId;		//scene id 0-15
	int			iOrderTyp;		//0:top area, 1:bottom area, 2:left area, 3:right area
	int			iCmdType;		//commond type, 0:set, 1:call
}VcaScanArea, *pVcaScanArea;

typedef struct tagVcaScanPara
{
	int			iSize;
	int			iSceneId;		//scene id 0-15
	int			iMode;			//scan type,0:manual, 1:auto
	int			iWaitTime;		//wait time (s)
	int			iPanStep;		//Pan Step [0,36000],Corresponding floating point range [0,360], unit: degree,(software use divide by 100), accuracy: 0.01
	int			iTiltStep;		//Tilt Step [0--9000],Corresponding floating point range [0,90], unit: degree,(software use divide by 100), accuracy: 0.01
}VcaScanPara, *pVcaScanPara;

typedef struct tagRtmpClientLinkState
{
    int				iSize;							// �ṹ���С
    int				iStateValue;					// ����״ֵ̬
} RtmpClientLinkState, *pRtmpClientLinkState;

#define RADAR_EVENT_TYPE_PARK			1
#define RADAR_EVENT_TYPE_CONGESTION		2
#define RADAR_EVENT_TYPE_MAX			3
typedef struct tagRadarEventPara
{
	int					iSize;
	int					iChanNo;		//channel no
	int					iEventType;		//0- reservation 1- parking event 2- congestion event
	int					iTime;			//iEventType=1 range [1,43200], Unit s
										//iEventType=2 range [1,3600], Unit s
}RadarEventPara, *pRadarEventPara;

#define RADAR_ROAD_MAXNUM		24
#define RADAR_CROSS_SECTION		 8

typedef struct tagRadarRoadDir
{
	int				iId;		//[0,11]
	int				iDir;		//0�C���򳵵�, 1�Cȥ�򳵵�
}RadarRoadDir, *pRadarRoadDir;

typedef struct tagRadarCrossSection
{
	int				iId;		//����Id,	��Χ [0,3]
	int				iPos;		//����λ��, ��Χ [0,250], Ĭ��ֵ0��ʾδ���ö��棬��λm
}RadarCrossSection, *pRadarCrossSection;

typedef struct tagRadarDevicePara
{
	int					iSize;
	int					iChanNo;							//channel no
	int					iRadarType;							//1-�����״�
	int					iRoadNum;							//�������� [1,12]
	int					iRadarRoadWidth;					//������� [100,400]	��λ cm
	RadarRoadDir		tRoadDir[RADAR_ROAD_MAXNUM];		//��������
	int					iRadarMeasureMax;					//������� ��Χ [100,28000], Ĭ��ֵ 25000�� ��λcm
	int					iRadarMeasureMin;					//������� ��Χ [100,25600], Ĭ��ֵ 3000����λcm
	int					iCrossSectionNum;					//�������
	RadarCrossSection	tCrossSection[RADAR_CROSS_SECTION];	//������Ϣ
	int					iRadarHeight;						//�״ﰲװ�߶� ��Χ [100,1000]Ĭ��ֵ 700 ��λcm 
	int					iRadarAngleDelta;					//�״�Ƕ����� ��Χ [0,20000] Ĭ��ֵ 10000 ��λ(��*100) (ʵ�ʶ�Ӧ��Χ[-100��,100��])
	int					iRadarCoordDelta;					//�״��������� ��Χ [0,2000], Ĭ��ֵ 1000 ��λcm (ʵ�ʶ�Ӧ��Χ[-1000,1000])
	int					iTrafficJamStartLine;				//ӵ���ж���ʼλ�� ��Χ [0,250], Ĭ��ֵ0����λm
	int					iTrafficJamTerminationLine;			//ӵ���ж���ֹλ�� ��Χ [0,250], Ĭ��ֵ0����λm
	int					iQueueLengthThreshold;				//�Ŷӳ�����ֵ ��Χ [0,8], Ĭ��ֵ0
	int					iTrafficJamCarNumber;				//ӵ�³�����ֵ ��Χ [0,128], Ĭ��ֵ0
	int					iLsolationBeltDriveway;				//��������ڳ��� ��Χ [0,12], Ĭ��ֵ0
	int					iLeftToTheLane;						//������򳵵���Ŀ ��Χ [0,8], Ĭ��ֵ0 ����ȫ��Ϊ����
	int					iNonmotorValue;						//·�ڷǻ�����������ֵ ��Χ [0,20], Ĭ��ֵ 0����Ӧ��Χ[-10,10] ��-10�����ˣ�ֵԽ�����Ч��Խǿ
	int					iMotorValue;						//�г����ǻ�����������ֵ ����, ��Χ [0,20], Ĭ��ֵ 0����Ӧ��Χ[-10,10] ��-10�����ˣ�ֵԽ�����Ч��Խǿ
	int					iShieldDistance;					//�������ξ��� [0,250],��λm�� Ĭ��ֵ80��0��ʾ�˹�����Ч
}RadarDevicePara, *pRadarDevicePara;	

#define MAX_CROP_NUM 2
typedef struct _tagCropCoding
{
	int                iSize;
	int                iEnable;                             //0����ʹ�ܣ�1��ʹ��
	TPoint             tPoint[MAX_CROP_NUM];                //ʹ����ֱȱ�ʾ��ȡֵ��Χ0��10000�����ο�ʵ�ʷֱ�����СΪ352*288,���򶥵�����
}CropCoding, *pCropCoding;

#define MAX_VAR_GAUGE_POINT_NUM		8

typedef struct tagVirGaugedIs
{
	int					iSize;
	int					iEnable;		//0, off; 1, on 
	int					iResult;		//0, setting succeeded; 1, setting failed
}VirGaugedIs, *pVirGaugedIs;

#define  MAX_VIDEO_COVER_EX     4
typedef struct tagVideoCoverEx
{
	int					iSize;
	int					iStreamNo;
	int					iAreaCnt;		//effictive area count
	CommonRECT			tCommonRECT[MAX_VIDEO_COVER_EX];		//area point
}VideoCoverEx, *pVideoCoverEx;

typedef struct __tagInterestedArea
{
	int iSize;
	int iIndex;
	int iEnable;
	int iAbsQp;
	int iQp;
	int iLeft;
	int iTop;
	int iRight;
	int iBottom;
	int iDstFrameRate;	//destination frame rate��1-current framerate
}TInterestedArea, *pTInterestedArea;

typedef struct __tagVcaFlowSpeedParam
{
	int					iSize;
	int					iChanNo;
	VCARule			    tRule;
	int		            iDisplayRule;	
	int                 iColor;
	int                 iSensitive;
	int                 iLinkRecord;
	int              	iCruiseAddMode;
	int              	iCruiseNum;
	int                 iInterval;
	vca_TPolygonEx      tPonit;
	int                 iApplyScene;	//0: large scene 1: small scene
	int                 iDetectStep;	//0 ~ 100000 unit mm, valid when icruiseaddmode is automatic
	int                 iMiddleWidth;	//0-20000 unit mm
	int                 iDetectAltitude;//The elevation unit of the initial detection area is mm, and the actual value range is [- 10000000 ~ 10000000]. If it is sent on the network, add 10000000, and then subtract 10000000 after the software value
	vca_TLine			tLinePonit;
	int                 iHorizontalWidth;	//Horizontal  Width
	int                 iVerticalWidth;		//Vertical  Width
	int                 iHypotenuseWidth;	//Hypotenuse  Width
	int                 iAngelInputMode;	//0: enter slope angle 1: enter right angle slope
	int                 iNightFocusMode;	//0: off 1: on
}VcaFlowSpeedParam, *pVcaFlowSpeedParam;

#define MAX_PRESET_NUM_EX		32

// (water level detection) RULEEVENT15
#define MAX_REFERPOINT_NUM 10
#define	EVENT_BUF_RESERVED		2048			//reserve size
#define MAX_WATER_SAMPLE_POINT 16


typedef struct tagWaterRange // level range
{
	int iStart; // start range
	int iEnd; // End range
} WaterRange;

typedef struct tagReferPoint // The water level reference
{
	int iPointX; // abscissa
	int iPointY; // ordinate
	int iValue; // The reading on the ruler
}ReferPoint;

typedef struct __tagVca_TPolygon
{
	int 		iPointNum;
	vca_TPoint 	stPoints[VCA_MAX_POLYGON_POINT_NUMEX];   
} vca_TPolygon;						//	sizeof = 16*8+4 = 132 



typedef struct tagWaterPresetInfo
{
	int                 iSize;
	int iPresetId; // Preset ID
	WaterRange stGaugeRange; // Water level range iStart: iEnd
	CommonRECT rcGaugeRect; // Diagonal coordinate of the scale area
	CommonRECT rcAssistRect; // auxiliary area diagonal coordinates
	CommonRECT rcGaugeLine; // Coordinate of the ruler reference line
	int iReferNum; // number of reference points
	ReferPoint stReferPoint [MAX_REFERPOINT_NUM]; // reference points 1, 2, 3
}WaterPresetInfo;

#define  MAX_RULE_REGION_NUM	4
typedef struct vca_tLeaveDetect
{
	int iValid; // Whether this event detection is valid, 0: invalid 1: valid
	int iLeaveAlarmTime; // off-duty alarm time, 60 ~ 3600, in seconds, the default: 120S
	int iRuturnClearAlarmTime; // back to the police alarm time, 10 to 300, in seconds Default: 15S
	int iAreaNum; // Total number of rule fields, 1 ~ 4
	vca_TPolygon		stRegion[MAX_RULE_REGION_NUM];			//	 Range	
}vca_tLeaveDetect, *LPvca_tLeaveDetect;



typedef  struct WaterSamplePoint
{
	int iSize;
	VCARule	stRule;
	vca_TDisplayParam	stDisplayParam;		//Parameter dispaly
	int iSamplePointId;
	int iSnapInterval;
	int iBaseValue;			//base water level value
	int iCurrentPresetId;	//current preset id
	int	iGaugeType;			//Gauge type:0-Reserved, 1-Wide gauge,New gauge, 2-Narrow gauge,Old gauge, 3-Special water ruler, self-calibration 4-gauge-type-squad 5-gauge-type-pile
	int iPresetCount;
	WaterPresetInfo stPresetInfo[MAX_PRESET_NUM_EX];
	int	iGaugeLength;
	int iGaugeCount;        //this param is valid when the gauge type is pile gauge
	int iDirection;         //0��left 1��right
	int iVirGaugeMode;		//Virtual Gauge Detect Mode 0-algorithm detection mode 1-manual calibration mode
	int iVirGaugePointNum;  //2--8,Effective in manual calibration mode 
	vca_TPoint tVirGaugePoint[MAX_VAR_GAUGE_POINT_NUM];
}WaterSamplePoint, *LPWaterSamplePoint;

#define BANDWIDTH_ADAPTING         0    // 20M/40M ����Ӧ
#define BANDWIDTH_FIX              1    //�̶�20M Ƶ��
#define MAX_WIFI_AP_CLIENT        16


//��ѯ/�ظ�AP client�豸�б�
typedef struct tagWIFIAPClientResult
{
    int     iSize;              // �ṹ���С
    int	    iCount;             // ������
    int     iNo;                // ��ǰ��Ŵӿ�ʼ
    char    cDevName[LEN_64]; // �豸����
    char    cIp[LEN_64];      // ip��ַ
    char    cMac[LEN_64];     // mac��ַ
    int     iSpeed;            // �ٶ�Mbps
    int     iBandWidth;        // ����BANDWIDTH_ADAPTING/BANDWIDTH_FIX
    int     iConnectTime;       // ����ʱ�� s

} WIFIAPClientResult, *pWIFIAPClientResult;


typedef struct tagWifiAPClientList
{
    int				    iSize; // �ṹ���С
    WIFIAPClientResult	tResult[MAX_WIFI_AP_CLIENT];      
}WifiAPClientList, *pWifiAPClientList;

#define SPEED_ADAPTING         0    // 20M/40M ����Ӧ
#define SPEED_FIX              1    //�̶�20M Ƶ��
#define MAX_WIFI_SEARCH_AP     32

typedef struct tagWIFISearchAPResult
{
    int     iSize;               // �ṹ���С
    int     iCount;              // ������
    int     iNo;                 // ��ǰ��Ŵӿ�ʼ
    char    cESSID[LEN_33];      //ESSID  LEN_33
    char    cEncryption[LEN_16]; //wifi encrypttion flag; NULL no encryption��"WEP" encryption
    int     iChannel;            // 0��Auto�������ŵ����ŵ���
    int     iRSSL;               // Mbps
    int     iSpeed;              // �ٶ�Mbps SPEED_ADAPTING/SPEED_FIX

} WIFISearchAPResult, *pWIFISearchAPResult;

typedef struct tagWIFISearchAPResultList
{
    int		            iSize;   // �ṹ���С
    WIFISearchAPResult  tResult[MAX_WIFI_SEARCH_AP];
} WIFISearchAPResultList,*pWIFISearchAPResultList;

#define LOCATION_NEM						1	//·�ڱ��/�ص�����
#define LANE_NUMBER							2	//�������/��������
#define DEVICE_NUMBER						3	//�豸���/�豸����
#define DRIVE_DIRECTION_TYPE				4	//��ʻ�������ͣ�
#define DICTIONARY_VEHICLE_TYPE				5	//�������ͣ�
#define SNAP_TYPE							6	//ץ�����ͣ�
#define VEHICLE_BRAND						7	//����Ʒ�ƣ�
#define VEHICLE_PLATE						8	//���ƣ�
#define VEHICLE_COLOR						9	//������ɫ
#define PIC_NAME_FORMATE					10	//ͼƬ���Ƹ�ʽ��
#define PLATFORM_VERSION					11	//ƽ̨�汾��
#define VEHICLE_SUB_BRAND					12	//������Ʒ�ƣ�
#define EXPORT_PIC_ROUTE_FORMATE			13	//����ͼƬ·����ʽ��
#define PLATFORM_TYPE						14	//ƽ̨���ͣ�
#define DEVICE_MODE							15	//�豸�ͺţ�
#define LED_INSERT_LABEL_ITEM				16	//Led�ַ����ӿ��ٲ����ǩ�
#define LED_DEV_MODE						17	//Led�豸���ͣ�
#define LED_HINT_MODE						18	//Led��ʾģʽ��
#define SCH_ALRAM_MODE						19	//���ر������� C
#define SEPARATOR_TYPE						20	//�ָ������ͣ�
#define FTP_UPLOAD_TYPE						21	//FTP�ϴ��ļ����ͣ�
#define VEHICLE_FORM						22	//�������Σ�
#define OSD_COLOR							23	//OSD������ɫ��
#define LANE_TYPE							24	//�������ͣ�
#define PLATE_COLOR							25	//������ɫ��
#define ITS_LOG_LEVEL						26	//��ͨ��־�ȼ���
#define CAM_NAME							27	//������ƣ�
#define LOCATION_NAME						28	//·�����ƣ�
#define OSD_OVERLAY_TYPE					29	//Osd������Ϣ���ͣ�
#define MD_CARD_VERSION						30	//Mq���ڰ汾��
#define MD_ILLEGAL_VERSION					31	//MqΥ���汾��
#define COMPOSTION_VIDESIZE_TYPE 			32	//�ϳɷֱ�������

#define MAX_DATA_PIC_ITEM  10

typedef struct tagDataDicionaryItem
{
	int		            iSize;   // �ṹ���С
	int					iFuncType;		//�������ͣ�����
	int		            iTotal;   // ��ѯ����
	int		            iIndexNo;   // �ֶ�����0~(N-1)
	int		            iFuncTypeValueNo;   // ��ǰ���
	char				cParam[LEN_128];   // �ַ������������ݸ�ʽ���ݾ���iFuncType�Լ�iFuncTypeValueNo�Լ�����
}DataDicionaryItem,*pDataDicionaryItem;


#define			MAX_HINT_INFO_TYPE_NUM   2
typedef struct tagHintInfo
{
	int		            iSize;   // �ṹ���С
	int					iHintType;//��ʾ���� 0���г� 1���޳�
	char				cDisplayInfo[LEN_128];
	int					iHintTime;		//��ʾʱ��,��λ����
} HintInfo,*pHintInfo;


typedef struct tagLedDevOsdInfo
{
	int     iSize;               // �ṹ���С
	int     iIdNum;              // ����ʾ���ں˸�ֵ�����ʱĬ��0���༭��ɾ��ʱ���ݻ�ȡֵ͸�����ɣ���1��ʼ�������ʹ�ã�
	int     iActionType;         // 0����ӣ�1���༭��2��ɾ�� 
	int		iLedTypeId;			//LED����Ӧ�����ͱ��(��ӦDATADICTIONARYITEM��ѯ����17�����iFuncTypeValueNo)����1��ʼ���磺1-��·Ԥ������2-ӵ������3-��������Ҳ����1-��������2-��·Ԥ������������ֵ����DATADICTIONARYITEM��ѯ�ϱ�����
	char    cLedDevNo[LEN_64];  //��ʾ�����
	char    cledDevName[LEN_128];  //��ʾ������
	int     iAlarmTypeId;            // ��ʾģʽ,��ʾģʽ(��ӦDATADICTIONARYITEM��ѯ����18�����iFuncTypeValueNo)����1��ʼ���磺1-��·Ԥ��
	int     iFontWidth;               // ������  16,24,26,28��30��32,48,64
	int     iFontHeight;              // ����߶�  16,24,26,28��30��32,48,64
	int     iFontColor;				//������ɫLED����������ɫ(��ӦDATADICTIONARYITEM��ѯ����23�����iFuncTypeValueNo)����1��ʼ���磺1-��ɫ
	char    cLedSize[LEN_64];		//��Ļ�ߴ�   ���磺800*600  ��Led�豸���������
	HintInfo tHintInfo[MAX_HINT_INFO_TYPE_NUM];//Ĭ��tHintInfo[0]�г� tHintInfo[1]�޳�
	int		iActionResult;		//��������״̬  0���ɹ� ��-1 ��ʧ��
} LedDevOsdInfo, *pLedDevOsdInfo;

typedef struct tagLedDevOsdParamList
{
	int		            iSize;   // �ṹ���С
	int					iTotal;
	int					iIndexNo;		//��ʾʱ��,��λ����
	LedDevOsdInfo       tLedDevOsdInfo;
} LedDevOsdParamList,*pLedDevOsdParamList;

#define RADAR_PARAMID_MAXNUM 256
typedef struct tagRadarAdvancedPara
{
	int		iSize;			
	int		iChanNo;
	int		iParamID;			//radar paramID 0-255
	char	cValue[LEN_64];		//paramID Value
}RadarAdvancedPara, *pRadarAdvancedPara;

#define LED_DEVICE_MAX_NUM		10

#define LED_ACTION_TYPE_ADD		0
#define LED_ACTION_TYPE_EDIT	1
#define LED_ACTION_TYPE_DEL		2
#define LED_ACTION_TYPE_TEST	3
#define LED_ACTION_TYPE_QUERY	4
#define LED_ACTION_TYPE_MAX		5

typedef struct tagLedDevInfo
{
	int     iSize;               
	int		iChanNo;
	int     iIdNo;					//reserved
	char    cLedDevNo[LEN_64];		//��ʾ�����
	char    cLedDevName[LEN_128];   //��ʾ������
	int		iLedDevType;			//��ʾ����Ӧ������
	int     iLedDevWidth;           //��Ļ���  ��λ������(1-10000)
	int     iLedDevHeight;          //��Ļ�߶�  ��λ������(1-10000)
	int     iLedDevModel;			//��ʾ���ͺ�
	char    cIpAddr[LEN_64];		//IP��ַ
	int		iPort;					//�˿ں�
}LedDevInfo, *pLedDevInfo;

typedef struct tagLedDevReply
{
	int		            iSize;
	int					iChanNo;
	int					iActionType;		//opt type 0����ӣ�1���༭��2��ɾ����3������
	char				cLedDevNo[LEN_64];			//��ʾ�����
	int					iActionResult;		//��������״̬ 0���ɹ���1:ʧ��
}LedDevReply,*pLedDevReply;

typedef struct tagLedDevParamResult
{
	int		            iSize;   
	int					iTotal;
	int					iIndexNo;
	LedDevInfo			tLedDevInfo;
}LedDevParamResult,*pLedDevParamResult;

typedef struct tagLedDevParamOpt
{
	int		            iSize;   
	int					iActionType;		//opt type 0����ӣ�1���༭��2��ɾ����3�����ԣ� 4,��ѯ
	LedDevInfo			tLedDevInfo;
}LedDevParamOpt,*pLedDevParamOpt;

#define MAX_RADAR_CALIBRATE_POINT_NUM		16

typedef struct tagRadarPointInfo
{	
	vca_TPoint			tVideoPosition;	//��ֱ�����
	vca_TPoint			tRealPosition;	//��ֵ[0~100000]��Ӧ[-50000cm~50000cm] ��ֵ50000��Ӧ0cm
}RadarPointInfo, *pRadarPointInfo;

typedef struct tagItsRadarCalibrate
{
	int		            iSize;   
	int					iChanNo;
	int					iPointNum;								  //point Number
	RadarPointInfo     	tPoints[MAX_RADAR_CALIBRATE_POINT_NUM];   //point info
}ItsRadarCalibrate, *pItsRadarCalibrate;

typedef struct tagBlackBodyDetect
{
	int		iSize;   
	int		iChanNo;
	int		iEnable;		//0-disenable 1-enable
	int		iThreshold;		//[0-100]
	int		iSharkTime;		//[0-100s] unit:s
}BlackBodyDetect, *pBlackBodyDetect;

typedef struct tagDefaultDevParam
{
	int		iSize;   
	int		iChanNo;
	int		iType;			//input para 1-�ȳ���ģ��ָ�Ĭ��  2-4GС��վFDDģ��ָ����� 3-4GС��վTDDģ��ָ����� 4-4GС��վFDDģ������ 5-4GС��վTDDģ������
	int		iResult;		//output para 0-success��1-failed
}DefaultDevParam, *pDefaultDevParam;


typedef struct tagWaterSpeedDeteUpdate
{
	int		iSize;   
	int		iSceneId;		//0~31 
	int		iDetectStep;	//0~100000,mm		
}WaterSpeedDeteUpdate, *pWaterSpeedDeteUpdate;

typedef struct tagWaterSpeedDeteUpdateResult
{
	int		iSize;   
	int		iSceneId;		//0~31 
	int		iDetectStep;	//0~100000,mm		
	int		iResult;	//0-succ 1-failed		
}WaterSpeedDeteUpdateResult, *pWaterSpeedDeteUpdateResult;

	
typedef struct tagSmallCellInfo
{
	int     iSize;
	int     iTerminalType;     //Module type in 4G small base station,0-FDD 1-TDD	
	int     iIsDspHearbeatRecv;//if Receive Dsp heart beat,0:no,1:yes
	int     iRfEnable;         //RF status 0 indicates RF off, 1 indicates RF on
}SmallCellInfo, *pSmallCellInfo;

typedef struct tagSmallCellImsi_Notify
{
	int      iSize;
	int      iDeviceId;       //Device ID
	int      iMsgId;          //Msg ID,prevent overlap
	int      iTime;           //timestamp
	char     pcImsiNo[LEN_32];
	int      iOwnerProvince;  //Provice Code
	int      iOwnerCity;      //City Code
	int      iOwnerOperator;  //0-Unknown 1-Mobile 2-Unicom 3-Telecom
	int      iAccessType;     //1-EpsAtt 2-EPSImsiAtt 6-EpsEmerAtt 16-Tau 17-TaLau 18-TaLauImsi 19-PeriU 24-TaUBr 25-TaLaUBr 26-TaLaImsiBr 27-PeriUBr
	int      iBaseStationType;//1-FDD 2-TDD
	char     pcImeiNo[LEN_32];
	char     pcStmsiNo[LEN_32];
	int      iLibCheck;       //0-under list 1-black list 2-white list
	int      iIsRealTime;     //1-upload ontime 2-upload storage
	int      iIsOwnerWarn;    //1- Place of attribution warning 2- Not place of attribution warning
}SmallCellImsi_Notify, *pSmallCellImsi_Notify;

typedef struct tagVcaHDSchedule
{
    int     iSize;
    int     iChannelNo; 			//ȡֵ��Χ���豸���Ͷ���
    int     iSceneId;               // ��16λΪ0ʱ����16λ��ʾ���ܷ�������Χ0~31;��16λΪ1ʱ����16λ��ʾ���䣬��Χ0~3
    int     iType;			        //ģ������ 0=����, 1=��֡, 2=��֡
    int     iEnable;		        //0=��ʹ��, 1=ʹ��
    int     iDayId;			        // ��ģ��ID '-1=��Ч, ��Χ[0,31]
    int     iNightId;			    // ҹģ��ID '-1=��Ч, ��Χ[0,31]
} VcaHDSchedule, *pVcaHDSchedule, VcaHDScheduleResult, *pVcaHDScheduleResult;

#define MAX_VCA_FOCUS_AREA 16
typedef struct tagVcaFocusArea
{
    int                 iSize;
    int                 iChannelNo; 				//ȡֵ��Χ���豸���Ͷ���
    int                 iSceneId;                   // ��16λΪ0ʱ����16λ��ʾ���ܷ�������Χ0~31,��16λΪ1ʱ����16λ��ʾ���䣬��Χ0~3
    int	                iAreaNum;                   //������� ȡֵ��Χ[0-16]
    vca_TPolygonEx		stAreas[MAX_VCA_FOCUS_AREA];	
} VcaFocusArea, *pVcaFocusArea, VcaFocusAreaResult, *pVcaFocusAreaResult;


#endif

