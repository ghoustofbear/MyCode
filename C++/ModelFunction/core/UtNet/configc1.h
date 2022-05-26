/**
 * (C) Copyright 2013, XXXX Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 * Tue Feb 26 16:45:16 PST 2013
 *
 * config.h
 */
#ifndef __CONFIG_H__
#define __CONFIG_H__

#include "defc1.h"

#ifdef _WIN32 
#define RS_DVR_D9104		       0x5253000000010100ULL
#define RS_DVR_D9104B		       0x5253000000010101ULL
#define RS_DVR_D9108		       0x5253000000010200ULL
#define RS_DVR_D9108B		       0x5253000000010201ULL
#define RS_DVR_D9016		       0x5253000000010300ULL
#else
#define RS_DVR_D9104		       0x5253000000010100LLU
#define RS_DVR_D9104B		       0x5253000000010101LLU
#define RS_DVR_D9108		       0x5253000000010200LLU
#define RS_DVR_D9108B		       0x5253000000010201LLU
#define RS_DVR_D9016		       0x5253000000010300LLU
#endif

#define  SOFT_VERSION              "V1.00.00.07"
#define  HARD_VERSION              "V1.00.00.07"
#define  IE_VERSION                "V1.00.00.07"
#define  DEVIC_NAME                "UT_ST001"
#define  CONFIG_FILE               "config.ini"
#define  CONFIG_DB                 "config.db"
#define  SQL_LEN                   (1024)

#define  DB_VERSION_CURRENT        (20181022)


#define ADDRCODE_SPLIT(AddrCode) \
	S32 s32ProvCode = 0; \
	S32 s32CityCode = 0; \
	S32 s32SysCode  = 0; \
	S32 s32StatCode = 0; \
	S32 s32ObjCode  = 0; \
	S32 s32DevCode  = 0; \
	S8  s8TmpStr[10] = {0}; \
	strncpy(s8TmpStr, AddrCode, 2); \
	s32ProvCode = atoi(s8TmpStr); \
	memset(s8TmpStr, 0, 10); \
	strncpy(s8TmpStr, &AddrCode[2], 2); \
	s32CityCode = atoi(s8TmpStr); \
	memset(s8TmpStr, 0, 10); \
	strncpy(s8TmpStr, &AddrCode[4], 1); \
	s32SysCode = atoi(s8TmpStr); \
	memset(s8TmpStr, 0, 10); \
	strncpy(s8TmpStr, &AddrCode[5], 5); \
	s32StatCode = atoi(s8TmpStr); \
	memset(s8TmpStr, 0, 10); \
	strncpy(s8TmpStr, &AddrCode[10], 4); \
	s32ObjCode = atoi(s8TmpStr); \
	memset(s8TmpStr, 0, 10); \
	strncpy(s8TmpStr, &AddrCode[14],4); \
	s32DevCode = atoi(s8TmpStr); 

#define POVINCECODE(AddrCode, ProvCode) \
	do \
	{ \
		ADDRCODE_SPLIT(AddrCode) \
		ProvCode = s32ProvCode; \
	} while (0)

#define CITYCODE(AddrCode, CityCode) \
	do \
	{ \
		ADDRCODE_SPLIT(AddrCode) \
		CityCode = s32CityCode; \
	} while (0)

#define SYSTEMCODE(AddrCode, SystemCode) \
	do \
	{ \
	ADDRCODE_SPLIT(AddrCode) \
	SystemCode = s32SysCode; \
	} while (0)

#define STATIONCODE(AddrCode, StatCode) \
	do \
	{ \
		ADDRCODE_SPLIT(AddrCode) \
		StatCode = s32StatCode; \
	} while (0)

#define OBJECTCODE(AddrCode, ObjectCode) \
	do \
	{ \
		ADDRCODE_SPLIT(AddrCode) \
		ObjectCode = s32ObjCode; \
	} while (0)

#define DEVICECODE(AddrCode, DevCode) \
	do \
	{ \
		ADDRCODE_SPLIT(AddrCode) \
		DevCode = s32DevCode; \
	} while (0)

#define IS_MEDIA_SERVER(AddrCode)  (AddrCode[11] == '1')

#define IS_SAME_SERVER(AddrCode1, AddrCode2)  (0 == strncmp(AddrCode1, AddrCode2, 5))

typedef struct _SYSINFO_SET_S_
{
	S8  s8SWVersion[MAX_NAME_LEN];	  /* ����汾 */
	S8  s8HWVerison[MAX_NAME_LEN];    /* Ӳ���汾 */
	S8  s8IEVerison[MAX_NAME_LEN];    /* IE�汾 */
	S8  s8DevName  [MAX_NAME_LEN];    /* �豸��   */
	S8  s8AddrCode [MAX_NAME_LEN];    /* �豸ID */
	U8  u8AuthIP   [MAX_IPV4_LEN];    /* ��֤������IP��ַ */

	U32 u32DbVerison;                 /* ���ݿ�汾 */

	U16 u16PrivatePort;               /* ˽����ý��˿ں� */
	U16 u16RtpBasePort;               /* RTP��ʼ�˿ں� */
	U16 u16SipPort;                   /* SIP�˿ں� */
	U16 u16RtspPort;                  /* RTSP�˿ں� */

	U16 u16HttpPort;                  /* AЭ��HTTP�˿� */
	U16 u16WebPort;                   /* Web�˿ں� */

	U32 u32RecvIntf;                  /* ��������ѡ�� */
	U32 u32SendIntf;                  /* ��������ѡ�� */
	U8  u8RecvIp[MAX_IPV4_LEN];       /* ��������IP */
	U8  u8SendIp[MAX_IPV4_LEN];       /* ��������IP */
 
	S8  s8EnAutoStart;                /* �����Զ����� */
	S8  s8UpdateSec;                  /* ʵʱͨ��ˢ��ʱ��(��) */
	U8  u8CACIp[MAX_IPV4_LEN];        /* CAC IP */
	S8  s8Reserved[2];
}SYSINFO_SET_S;

typedef struct _MISC_SET_S_
{
	U32 u32DevStartCode;              /* �豸��ʼ��� */
	U32 u32OrganStartId;              /* ��֯�ڵ���ʼid*/
}MISC_SET_S;

typedef struct _MAINTAIN_SET_S_
{
	S32 s32TimeInterval;              /* �Զ�Уʱʱ���� */
	U8  u8RecvIP[MAX_IPV4_LEN];       /* ����УʱIP��ַ */
	S8  s8EnRecvTime;                 /* �Ƿ�����ϼ�Уʱָ�� */
	S8  s8EnSendTime;                 /* �Ƿ��·�Уʱָ�� */
	S8  s8WatchDog;                   /* �Ƿ������Ź� */
	S8  s8AllowUpPtzCtrl;             /* �Ƿ�������վ��̨���� */
	S8  s8AllowH264;                  /* �Ƿ�����ͬʱ������׼������ȡ */
	S8  s8QueryResourceType;          /* �������ߺ�ʢ */
	S8  s8UploadCamStatus;            /* ���������״̬ */
	S8  s8Reserved[1];	
}MAINTAIN_SET_S;

typedef struct _NETWORK_S_
{
	U32 u32NetMode;                   /* ����ģʽ */
	U8  u8IPAddr[MAX_IPV4_LEN];       /* IP��ַ */
	U8  u8NetMask[MAX_IPV4_LEN];      /* �������� */
	U8  u8GateWay[MAX_IPV4_LEN];      /* ���� */

	U16 u16RtpBasePort;               /* Rtp��ʼ�˿ں� */
	U16 u16SipRtspPort;               /* Sip/Rtsp�˿ں� */
	U16 u16PrivatePort;               /* �ͻ��˶˿ں� */
	U16 u16WebPort;                   /* Web�˿ں� */
}NETWORK_SET_S;

typedef struct 
{
    U8  u8PriPlatformIp[MAX_IPV4_LEN];   /* ��ƽ̨IP   */
	U16 u16PriPlatformPort;              /* ��ƽ̨�˿� */
	U8  u8SubPlatformIp[MAX_IPV4_LEN];   /* ��ƽ̨IP   */
	U16 u16SubPlatformPort;              /* ��ƽ̨�˿� */
}PLATFROM_SET_S;

typedef struct
{
    S8  s8PlatePath[260];           /* MAX_PATH,����д��λ��·��(�����ں������ܽ�ͨ��) */
	U16 u16Port;                    /* nfs���������ط���˿�,nfs�Ͽ��޸� */
	U16 u16RelaySndTime;            /* ���ͳ��Ƹ澯��ʱʱ�� */
}VEHICLE_SET_S;

typedef struct  
{
	S8  s8FacePicPath[260];
}FACE_SET_S;


typedef struct _DEVICE_SET_S_
{
	S8  s8AddrCode[MAX_ADDR_LEN];
	S8  s8DevName[MAX_NAME_LEN];
	S8  s8PareAddrCode[MAX_ADDR_LEN];
	S8  s8DevDesc[100];

	S32 s32ChnNum;
	S32 s32ChnNo;
	U8  u8IPAddr[4];

	U16 u16Port;
	U16 u16DevType;


	S8  s8Username[MAX_NAME_LEN];
	S8  s8Password[MAX_NAME_LEN];

	S8  s8ManuType;
	S8  s8ProtoType;
	S8  s8LoginType;
	S8  s8StreamType;

	S8  s8Organ;
	S8  s8DisLevel;
	S8  s8AlarmInNum;
	S8  s8AlarmOutNum;

	U16 u16HttpPort;
	U16 u16RtspPort;
	U16 u16RegInterval; 
	U16 u16RegExpires;

	U16 u16KeepAlive;

	S8  s8RetrackAddrCode[MAX_ADDR_LEN];
	S8  s8DomainType;
	S8  s8Domain[MAX_FILENAME_LEN];

	S8  s8AreaName[MAX_NAME_LEN];

	bool operator < (const _DEVICE_SET_S_ &rlv) const
	{
		if(strcmp(s8AddrCode, rlv.s8AddrCode) > 0)  return false;
		return true;
	}
} DEVICE_SET_S;

typedef struct _DEVICE_CHN_S_
{
    S8   s8Ip[MAX_IP_STR_LEN];
	U16  u16Chn;
	U16  u16DevType;
	S8   s8DevName[MAX_NAME_LEN];
	S8   s8DevDesc[MAX_NAME_LEN];
	U16  u16ProtoType;
	S8   s8ChnIp[MAX_IP_STR_LEN];
	U16  u16ChnPort;
	S8   s8AreaName[MAX_NAME_LEN];

	bool operator < (const _DEVICE_CHN_S_ &rlv) const
	{
		if(u16Chn > rlv.u16Chn) 
		{
			return false;
		}
		else
		{
            return true;
		}		
	}
}DEVICE_CHN_S;

typedef struct _GROUP_SET_S_
{
	U32 u32Id;
	S8  s8Name[MAX_NAME_LEN];
	U32 u32RightBit;
	S8  S8Type;
	S8  S8Level;
	S8  s8Organ;
	S8  s8Reserved[5];
} GROUP_SET_S;

typedef struct _USER_SET_S_
{
	U32 u32Id;
	S8  s8Name[MAX_NAME_LEN];
	S8  s8Alias[MAX_NAME_LEN];
	S8  s8RealName[MAX_NAME_LEN];
	S8  s8Password[MAX_NAME_LEN];
	S8  s8Identity[MAX_NAME_LEN];
	S8  s8Phone[MAX_NAME_LEN];
	S8  s8Email[MAX_NAME_LEN];
	S8  s8FixedIp[MAX_IP_STR_LEN];
	S8  s8EnFixedIp;
	S8  s8Reserved[5];
} USER_SET_S;

typedef struct _STATION_SET_S_
{
	U32 u32StatCode;
	U32 u32ProvCode;
	U32 u32CityCode;
	S8  s8StatName[MAX_NAME_LEN];
	S8  s8ProvName[MAX_NAME_LEN];
	S8  s8CityName[MAX_NAME_LEN];
	S8  s8StatDesc[100];
} STATION_SET_S;

typedef struct _SYS_PARAM_S_
{
	MISC_SET_S     stMiscSet;
	SYSINFO_SET_S  stSysInfoSet;
	NETWORK_SET_S  stNetworkSet;
	MAINTAIN_SET_S stMaintainSet;
	PLATFROM_SET_S stPlatformSet;
	VEHICLE_SET_S  stVehicleSet;
	FACE_SET_S     stFaceSet;
} SYS_PARAM_S;


class CM_API Config
{
	public:
		Config();
		virtual ~Config();

		/* ��ȡ������Ϣ */
		virtual S32 LoadSettings() = 0;

		/* ��дϵͳ��Ϣ */
		virtual S32 ReadSysInfo (SYSINFO_SET_S *pSysInfo) = 0;
		virtual S32 WriteSysInfo(SYSINFO_SET_S *pSysInfo) = 0;

		/* ��д������Ϣ */
		virtual S32 ReadNetwork (NETWORK_SET_S *pNetwork) = 0;
		virtual S32 WriteNetwork(NETWORK_SET_S *pNetwork) = 0;

		/* ��д�豸��Ϣ */
		virtual S32 ReadDevice(DEVICE_SET_S  *pDevice) = 0;
		virtual S32 WriteDevice(DEVICE_SET_S *pDevice) = 0;
	public:
		/* �����Ϸ��Լ�� */
		S32 CheckMisc(MISC_SET_S *pMisc, BOOL bModify);
		S32 CheckSysInfo(SYSINFO_SET_S *pSysInfo, BOOL bModify);
		S32 CheckNetwork(NETWORK_SET_S *pNetwork, BOOL bModify);

	protected:
		SYS_PARAM_S *m_pSysParam;                 /* ϵͳ��Ϣ�ṹ��ָ�� */
};

class IniConfig : public Config
{
	public:
		IniConfig(SYS_PARAM_S *pSysParam);
		virtual ~IniConfig();

		/* ��ȡ������Ϣ */
		virtual S32 LoadSettings();

		/* ��дϵͳ��Ϣ */
		virtual S32 ReadSysInfo (SYSINFO_SET_S *pSysInfo);
		virtual S32 WriteSysInfo(SYSINFO_SET_S *pSysInfo);

		/* ��д������Ϣ */
		virtual S32 ReadNetwork (NETWORK_SET_S *pNetwork);
		virtual S32 WriteNetwork(NETWORK_SET_S *pNetwork);

		/* ��д�豸��Ϣ */
		virtual S32 ReadDevice(DEVICE_SET_S  *pDevice) { return 0; };
		virtual S32 WriteDevice(DEVICE_SET_S *pDevice) { return 0; };

		/* д�������ù������Ϣ */
		virtual S32 ReadMisc (MISC_SET_S *pMisc);
		virtual S32 WriteMisc(MISC_SET_S *pMisc);

		virtual S32 ReadMaintian (MAINTAIN_SET_S *pMaintain);
		virtual S32 WriteMaintian(MAINTAIN_SET_S *pMaintain);

		virtual S32 ReadPlatform (PLATFROM_SET_S *pPlatform);
		virtual S32 WritePlatform(PLATFROM_SET_S *pPlatform);

		/* ������Ϣ */
		virtual S32 ReadVehicleInfo(VEHICLE_SET_S  *pVehicle);
		virtual S32 WriteVehicleInfo(VEHICLE_SET_S *pVehicle);

		/* ������Ϣ */
		virtual S32 ReadFaceInfo(FACE_SET_S * pFaceParam);
		virtual S32 WriteFaceInfo(FACE_SET_S * pFaceParam);

	protected:
		S32 SaveSettings();

};

typedef struct sqlite3 sqlite3;
class DbConfig : public Config
{
	public:
		DbConfig(SYS_PARAM_S *pSysParam);
		virtual ~DbConfig();

		/* ��ȡ������Ϣ */
		virtual S32 LoadSettings();

		/* ��дϵͳ��Ϣ */
		virtual S32 ReadSysInfo (SYSINFO_SET_S *pSysInfo);
		virtual S32 WriteSysInfo(SYSINFO_SET_S *pSysInfo);

		/* ��д������Ϣ */
		virtual S32 ReadNetwork (NETWORK_SET_S *pNetwork);
		virtual S32 WriteNetwork(NETWORK_SET_S *pNetwork);

		/* ��д�豸��Ϣ */
		virtual S32 ReadDevice(DEVICE_SET_S  *pDevice);
		virtual S32 ReadDevice_2(DEVICE_SET_S  *pDevice);
		virtual S32 WriteDevice(DEVICE_SET_S *pDevice);
		virtual S32 DeleteDevice(S8 *ps8AddrCode);
		virtual void BeginComTrans();
		virtual void EndComTrans();

		/* �����ɾ��ʱ,�������������ͷ������ϵ */
		virtual S32 DetachReTrackCarToCamera(S8 * ps8AddrCode);
		virtual S32 DeleteRePresetInfo(S8 *ps8AddrCode);
		
		/* ��дվ����Ϣ */
		virtual S32 WriteStation(STATION_SET_S *pStation);
		virtual S32 DeleteStation(U32 u32StatCode);

		/* ��д�û�����Ϣ */
		virtual S32 WriteUserGroup(U32 u32UserId, U32 u32GroupId, U32 u32GroupType);
		virtual S32 DeleteUserGroup(U32 u32UserId, U32 u32GroupId, U32 u32GroupType);

		/* ��дϵͳά����Ϣ */
		virtual S32 ReadMaintain (MAINTAIN_SET_S *pMaintian);
		virtual S32 WriteMaintain(MAINTAIN_SET_S *pMaintian);

		virtual S32 GetProvinceCode(S8 *ps8ProvName);
		virtual S32 GetProvinceName(S32 s32ProvCode, S8 *ps8ProvName);

		virtual S32 GetCityCode(S8 *ps8ProvName, S8 *ps8CityName);
		virtual S32 GetCityName(S32 s32ProvCode, S32 s32CityCode, S8 *ps8CityName);

		virtual S32 GetStationCode(S32 s32ProvCode, S32 s32CityCode, S8 *ps8StationName);
		virtual S32 GetStationName(S32 s32StationCode, S8 *ps8StationName);

		virtual S32 GetObjectCode(S8 *ps8ObjectName);
		virtual S32 GetObjectName(S32 s32ObjectCode, S8 *ps8ObjectName);

		virtual S32 GetDevTypeCode(S8 *ps8DevTypeName);
		virtual S32 GetDevTypeName(S32 s32DevTypeCode, S8 *ps8DevTypeName);

		virtual S32 GetManuTypeCode(S8 *ps8ManuTypeName);
		virtual S32 GetManuTypeName(S32 s32ManuTypeCode, S8 *ps8ManuTypeName);

		virtual S32 GetProtoTypeCode(S8 *ps8ProtoTypeName);
		virtual S32 GetProtoTypeName(S32 s32ProtoTypeCode, S8 *ps8ProtoTypeName);

		virtual S32 GetStreamTypeCode(S8 *ps8StreamName);
		virtual S32 GetStreamTypeName(S32 s32StreamCode, S8 *ps8StreamName);

		virtual S32 GetDeviceAddrCode(S8 *ps8ParentAddrCode, S32 s32ChnNo, S8 *ps8AddrCode, S8 *ps8DevName = NULL);

		virtual S32 GetDeviceList(string strAddrCode, vector<DEVICE_SET_S> &vecDev);
	private:
		sqlite3  *m_pDb;
};


class CM_API ComConfig : public Config
{
	public:
		ComConfig();
		virtual ~ComConfig();

		/* ��ȡ������Ϣ */
		virtual S32 LoadSettings();

		/* д�������ù������Ϣ */
		virtual S32 ReadMisc (MISC_SET_S *pMisc);
		virtual S32 WriteMisc(MISC_SET_S *pMisc);

		/* ��дϵͳ��Ϣ */
		virtual S32 ReadSysInfo (SYSINFO_SET_S *pSysInfo);
		virtual S32 WriteSysInfo(SYSINFO_SET_S *pSysInfo);

		/* ��д������Ϣ */
		virtual S32 ReadNetwork (NETWORK_SET_S *pNetwork);
		virtual S32 WriteNetwork(NETWORK_SET_S *pNetwork);

		/* ��д�豸��Ϣ */
		virtual S32 ReadDevice(DEVICE_SET_S  *pDevice);
        virtual S32 ReadDevice_2(DEVICE_SET_S  *pDevice);       //added by wk 2014/10/28
		virtual S32 WriteDevice(DEVICE_SET_S *pDevice);
		virtual S32 DeleteDevice(S8 *ps8AddrCode);
		virtual void BeginComTrans();
		virtual void EndComTrans();

		/* �����ɾ��ʱ,�������������ͷ������ϵ */
		virtual S32 DetachReTrackCarToCamera(S8 * ps8AddrCode);
		virtual S32 DeleteRePresetInfo(S8 *ps8AddrCode);

		/* ��дվ����Ϣ */
		virtual S32 WriteStation(STATION_SET_S *pStation);
		virtual S32 DeleteStation(U32 u32StatCode);

		/* ��д�û�����Ϣ */
		virtual S32 WriteUserGroup(U32 u32UserId, U32 u32GroupId, U32 u32GroupType);
		virtual S32 DeleteUserGroup(U32 u32UserId, U32 u32GroupId, U32 u32GroupType);

		/* ��дϵͳά����Ϣ */
		virtual S32 ReadMaintain (MAINTAIN_SET_S *pMaintian);
		virtual S32 WriteMaintain(MAINTAIN_SET_S *pMaintian);

		/* ��дƽ̨��Ϣ */
		virtual S32 ReadPlatform (PLATFROM_SET_S *pPlatform);
		virtual S32 WritePlatform(PLATFROM_SET_S *pPlatform);

		/* ������Ϣ */
		virtual S32 ReadVehicleInfo(VEHICLE_SET_S  *pVehicle);
		virtual S32 WriteVehicleInfo(VEHICLE_SET_S *pVehicle);

		virtual S32 GetProvinceCode(S8 *ps8ProvName);
		virtual S32 GetProvinceName(S32 s32ProvCode, S8 *ps8ProvName);

		virtual S32 GetCityCode(S8 *ps8ProvName, S8 *ps8CityName);
		virtual S32 GetCityName(S32 s32ProvCode, S32 s32CityCode, S8 *ps8CityName);

		virtual S32 GetStationCode(S32 s32ProvCode, S32 s32CityCode, S8 *ps8StationName);
		virtual S32 GetStationName(S32 s32StationCode, S8 *ps8StationName);

		virtual S32 GetObjectCode(S8 *ps8ObjectName);
		virtual S32 GetObjectName(S32 s32ObjectCode, S8 *ps8ObjectName);

		virtual S32 GetDevTypeCode(S8 *ps8DevTypeName);
		virtual S32 GetDevTypeName(S32 s32DevTypeCode, S8 *ps8DevTypeName);

		virtual S32 GetManuTypeCode(S8 *ps8ManuTypeName);
		virtual S32 GetManuTypeName(S32 s32ManuTypeCode, S8 *ps8ManuTypeName);
		
		virtual S32 GetProtoTypeCode(S8 *ps8ProtoTypeName);
		virtual S32 GetProtoTypeName(S32 s32ProtoTypeCode, S8 *ps8ProtoTypeName);

		virtual S32 GetStreamTypeCode(S8 *ps8StreamName);
		virtual S32 GetStreamTypeName(S32 s32StreamCode, S8 *ps8StreamName);

		virtual S32 GetDeviceAddrCode(S8 *ps8ParentAddrCode, S32 s32ChnNo, S8 *ps8AddrCode, S8 *ps8DevName = NULL);

		virtual S32 GetDeviceList(string strAddrCode, vector<DEVICE_SET_S> &vecDev);
	protected:
		S32 CheckSysParam(SYS_PARAM_S *pSysParam, BOOL bModify);

	protected:
		SYS_PARAM_S * m_pSysParam;
		IniConfig   * m_pIniCfg;
		DbConfig    * m_pDbCfg;
};

CM_API S32 InitConfig();
CM_API S32 SqliteExec(sqlite3* db, char *sql);
CM_API S32 SqliteGetTable(sqlite3* db, char *sql, char ***pResult, int *nRow, int *nCol);

CM_API void SetCurrentDirectory();

CM_API extern SYS_PARAM_S *g_pSysParam;
CM_API extern ComConfig   *g_pConfig;
#endif
