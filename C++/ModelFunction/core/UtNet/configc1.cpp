#include "configc1.h"
#include "inifile.h"
#include "easySQLite/sqlite3.h"
#include "concourrencyc1.h"
#include "commonc1.h"

#define ENABLE_CONFIG_DEBUG
#ifdef  ENABLE_CONFIG_DEBUG 
#ifdef _WIN32
#define CONFIG_DEBUG             printf
#else
#define CONFIG_DEBUG(MSG...)     DEBUG("CONFIG", MSG)
#endif /* end _WIN32 */
#else
#define CONFIG_DEBUG(MSG...)    //
#endif /* end ENABLE_CONFIG_DEBUG */


SYS_PARAM_S *g_pSysParam = NULL;
ComConfig   *g_pConfig = NULL;

S32 InitConfig()
{
	g_pConfig->LoadSettings();

	return 0;
}

Config::Config()
{
}

Config::~Config()
{
}

S32 Config::CheckMisc(MISC_SET_S *pMisc, BOOL bModify)
{
	S32 s32Ret = 0;

	if (pMisc->u32DevStartCode == 0 ||
		pMisc->u32OrganStartId == 0)
	{
		if (bModify)
		{
			pMisc->u32DevStartCode = 1;
			pMisc->u32OrganStartId = 1;
		}
		s32Ret = -1;
	}

	return s32Ret;
}

S32 Config::CheckSysInfo(SYSINFO_SET_S *pSysInfo, BOOL bModify)
{
	S32 s32Ret = 0;

	if (strcmp(pSysInfo->s8SWVersion, "") == 0)
	{
		if (bModify)
		{
			strncpy(pSysInfo->s8SWVersion, 
					SOFT_VERSION, MAX_NAME_LEN);
		}
		s32Ret = -1;
	}

	if (strcmp(pSysInfo->s8HWVerison, "") == 0)
	{
		if (bModify)
		{
			strncpy(pSysInfo->s8HWVerison, 
					HARD_VERSION, MAX_NAME_LEN);
		}
		s32Ret = -1;
	}

	if (strcmp(pSysInfo->s8IEVerison, "") == 0)
	{
		if (bModify)
		{
			strncpy(pSysInfo->s8IEVerison, 
				IE_VERSION, MAX_NAME_LEN);
		}
		s32Ret = -1;
	}

	if (pSysInfo->u32DbVerison == 0)
	{
		if (bModify)
		{
			pSysInfo->u32DbVerison = DB_VERSION_CURRENT;
		}
		s32Ret = -1;
	}

	if (strcmp(pSysInfo->s8DevName, "") == 0)
	{
		if (bModify)
		{
			strncpy(pSysInfo->s8DevName, 
					DEVIC_NAME, MAX_NAME_LEN);
		}
		s32Ret = -1;
	}

	return s32Ret;
}

S32 Config::CheckNetwork(NETWORK_SET_S *pNetwork, BOOL bModify)
{
	S32 s32Ret = 0;

	if (pNetwork->u8IPAddr[0] == 0)
	{
		if (bModify)
		{
			pNetwork->u8IPAddr[0] = 192;
			pNetwork->u8IPAddr[1] = 168;
			pNetwork->u8IPAddr[2] = 225;
			pNetwork->u8IPAddr[3] = 159;
		}
		s32Ret = -1;
	}

	if (pNetwork->u16PrivatePort == 0)
	{
		if (bModify)
		{
			pNetwork->u16PrivatePort = 9000;
		}
		s32Ret = -1;
	}

	if (pNetwork->u16WebPort == 0)
	{
		if (bModify)
		{
			pNetwork->u16WebPort = 80;
		}
		s32Ret = -1;
	}

	return s32Ret;
}

IniConfig::IniConfig(SYS_PARAM_S *pSysParam)
{
	m_pSysParam = pSysParam;
}

IniConfig::~IniConfig()
{

}

S32 IniConfig::SaveSettings()
{
	S32 s32Ret = 0;

	IniFile iniFile;
	if (!iniFile.openWriteReplace(CONFIG_FILE))
	{
		printf("Unable to open ini file");
	}
	else
	{
		printf("Saving settings to:  %s\n", CONFIG_FILE);

		iniFile.writeSection("Misc");
		iniFile.writeIntValue("DevStartCode",  m_pSysParam->stMiscSet.u32DevStartCode);
		iniFile.writeIntValue("OrganStartId",  m_pSysParam->stMiscSet.u32OrganStartId);

		iniFile.writeSection("SysInfo");
		iniFile.writeStrValue("SWVersion", m_pSysParam->stSysInfoSet.s8SWVersion);
		iniFile.writeStrValue("HWVersion", m_pSysParam->stSysInfoSet.s8HWVerison);
		iniFile.writeStrValue("DevName",   m_pSysParam->stSysInfoSet.s8DevName);
		iniFile.writeStrValue("PUID",      m_pSysParam->stSysInfoSet.s8AddrCode);

		S8 s8IPAddr[MAX_IP_STR_LEN] = {0};
		iniFile.writeSection("Network");
		INET_NTOP(m_pSysParam->stSysInfoSet.u8RecvIp, s8IPAddr);
		iniFile.writeStrValue("Recv Address", s8IPAddr);

		memset(s8IPAddr, 0, MAX_IP_STR_LEN);
		INET_NTOP(m_pSysParam->stSysInfoSet.u8SendIp, s8IPAddr);
		iniFile.writeStrValue("Send Address",  s8IPAddr);	
		
		iniFile.writeIntValue("Private Port", m_pSysParam->stNetworkSet.u16PrivatePort);
		iniFile.writeIntValue("Web Port",  m_pSysParam->stNetworkSet.u16WebPort);
		iniFile.writeStrValue("CAC Address",  "0.0.0.0");

		iniFile.writeSection("Miantian");
		
		memset(s8IPAddr, 0, MAX_IP_STR_LEN);
		INET_NTOP(m_pSysParam->stMaintainSet.u8RecvIP, s8IPAddr);
		iniFile.writeStrValue("Time Address",  s8IPAddr);
        iniFile.writeIntValue("RunWatchDog", m_pSysParam->stMaintainSet.s8WatchDog);
		iniFile.writeIntValue("AllowUpPtzCtrl", m_pSysParam->stMaintainSet.s8AllowUpPtzCtrl);
		iniFile.writeIntValue("AllowH264", m_pSysParam->stMaintainSet.s8AllowH264);
		iniFile.writeIntValue("QueryResourceType", m_pSysParam->stMaintainSet.s8QueryResourceType);
		iniFile.writeIntValue("UploadCamStatus", m_pSysParam->stMaintainSet.s8UploadCamStatus);

		iniFile.writeSection("PlatForm");
		
		memset(s8IPAddr, 0, MAX_IP_STR_LEN);
		INET_NTOP(m_pSysParam->stPlatformSet.u8PriPlatformIp, s8IPAddr);
		iniFile.writeStrValue("PriPlatformIp",  s8IPAddr);

		iniFile.writeIntValue("PriPlatformPort",  m_pSysParam->stPlatformSet.u16PriPlatformPort);

		memset(s8IPAddr, 0, MAX_IP_STR_LEN);
		INET_NTOP(m_pSysParam->stPlatformSet.u8SubPlatformIp, s8IPAddr);
		iniFile.writeStrValue("SubPlatformIp",  s8IPAddr);
        
        iniFile.writeIntValue("SubPlatformPort",  m_pSysParam->stPlatformSet.u16SubPlatformPort);

        iniFile.writeSection("Vehicle");
        iniFile.writeStrValue("PlatePath",  m_pSysParam->stVehicleSet.s8PlatePath);
		iniFile.writeIntValue("Port",  m_pSysParam->stVehicleSet.u16Port);
		iniFile.writeIntValue("Time",  m_pSysParam->stVehicleSet.u16RelaySndTime);

		iniFile.writeSection("Face");
        iniFile.writeStrValue("FacePicPath",  m_pSysParam->stFaceSet.s8FacePicPath);

		iniFile.close();
	}

	return s32Ret;
}

S32 IniConfig::LoadSettings()
{
	S32 s32Ret = 0;

	IniFile iniFile;
	if (!iniFile.openReadOnly(CONFIG_FILE))
	{
		s32Ret += WriteMisc(&m_pSysParam->stMiscSet);
		s32Ret += WriteSysInfo(&m_pSysParam->stSysInfoSet);
		s32Ret += WriteNetwork(&m_pSysParam->stNetworkSet);
		/* 看门狗默认为开启 */
		m_pSysParam->stMaintainSet.s8WatchDog  = 1;
		m_pSysParam->stMaintainSet.s8AllowUpPtzCtrl = 1;
		m_pSysParam->stMaintainSet.s8AllowH264 = 1;
		m_pSysParam->stMaintainSet.s8QueryResourceType = 0;
		m_pSysParam->stMaintainSet.s8UploadCamStatus = 0;
		s32Ret += WriteMaintian(&m_pSysParam->stMaintainSet);
		s32Ret += WritePlatform(&m_pSysParam->stPlatformSet);

		m_pSysParam->stVehicleSet.u16Port         = 8080;
		m_pSysParam->stVehicleSet.u16RelaySndTime = 10;

		s32Ret += WriteVehicleInfo(&m_pSysParam->stVehicleSet);

		strcpy(m_pSysParam->stFaceSet.s8FacePicPath, "D:\\PQMPic\\His");
		s32Ret += WriteFaceInfo(&m_pSysParam->stFaceSet);
	}
	else
	{
		s32Ret += ReadMisc(NULL);
		s32Ret += ReadSysInfo(NULL);
		s32Ret += ReadNetwork(NULL);
		s32Ret += ReadMaintian(NULL);
		s32Ret += ReadPlatform(NULL);
		s32Ret += ReadVehicleInfo(NULL);
		s32Ret += ReadFaceInfo(NULL);
	}

	return s32Ret;
}


S32 IniConfig::ReadSysInfo (SYSINFO_SET_S *pSysInfo)
{
	IniFile iniFile;

	if (!iniFile.openReadOnly(CONFIG_FILE))
	{
		return -1;
	}

	while (iniFile.readNext())
	{
		if (iniFile.isName("SWVersion"))
		{
			strncpy(m_pSysParam->stSysInfoSet.s8SWVersion, iniFile.getStrValue(), MAX_NAME_LEN);
            S8 s8SoftwareV[512] = {0};
            GetPrivateProfileString("Install", "version", "V1.00.00.00", s8SoftwareV, 512, "./Version.ini");
			strncpy(m_pSysParam->stSysInfoSet.s8SWVersion, s8SoftwareV, MAX_NAME_LEN);
		}
		else if (iniFile.isName("HWVersion"))
		{
			strncpy(m_pSysParam->stSysInfoSet.s8HWVerison, iniFile.getStrValue(), MAX_NAME_LEN);
            S8 s8SoftwareV[512] = {0};
            GetPrivateProfileString("Install", "version", "V1.00.00.00", s8SoftwareV, 512, "./Version.ini");
			strncpy(m_pSysParam->stSysInfoSet.s8SWVersion, s8SoftwareV, MAX_NAME_LEN);
		}
		else if (iniFile.isName("DevName"))
		{
			strncpy(m_pSysParam->stSysInfoSet.s8DevName, 
					iniFile.getStrValue(), MAX_NAME_LEN);
		}
		else if (iniFile.isName("PUID"))
		{
			strncpy(m_pSysParam->stSysInfoSet.s8AddrCode, 
					iniFile.getStrValue(), MAX_NAME_LEN);
		}
	}
	iniFile.close();
	if (pSysInfo)
	{
		*pSysInfo = m_pSysParam->stSysInfoSet;
	}

	return 0;
}

S32 IniConfig::WriteSysInfo(SYSINFO_SET_S *pSysInfo)
{
	printf("SwVersion: %s, HwVersion: %s\n",
			pSysInfo->s8SWVersion,
			pSysInfo->s8HWVerison);

	ASSERT(pSysInfo);

	IniFile iniFile;

	if ( CheckSysInfo(pSysInfo, TRUE) < 0)
	{
	}

	m_pSysParam->stSysInfoSet = *pSysInfo;

	SaveSettings();

	return 0;
}


S32 IniConfig::ReadMisc (MISC_SET_S *pMisc)
{
	IniFile iniFile;

	if (!iniFile.openReadOnly(CONFIG_FILE))
	{
		return -1;
	}

	while (iniFile.readNext())
	{
		if (iniFile.isName("DevStartCode"))
		{
			m_pSysParam->stMiscSet.u32DevStartCode = iniFile.getIntValue();
		}
		if (iniFile.isName("OrganStartId"))
		{
			m_pSysParam->stMiscSet.u32OrganStartId = iniFile.getIntValue();
		}
	}
	iniFile.close();
	if (pMisc)
	{
		*pMisc = m_pSysParam->stMiscSet;
	}

	return 0;
}

S32 IniConfig::WriteMisc(MISC_SET_S *pMisc)
{
	ASSERT(pMisc);

	if ( CheckMisc(pMisc, TRUE) < 0)
	{
	}

	m_pSysParam->stMiscSet = *pMisc;

	SaveSettings();

	return 0;
}


S32 IniConfig::ReadNetwork (NETWORK_SET_S *pNetwork)
{
	IniFile iniFile;

	if (!iniFile.openReadOnly(CONFIG_FILE))
	{
		return -1;
	}

	while (iniFile.readNext())
	{
		if (iniFile.isName("Recv Address"))
		{
			S8 s8IpAddr[MAX_IP_STR_LEN] = {0};
			strncpy(s8IpAddr, iniFile.getStrValue(), MAX_IP_STR_LEN);
			INET_PTON(s8IpAddr, m_pSysParam->stSysInfoSet.u8RecvIp);	
		}
		else if (iniFile.isName("Send Address"))
		{
			S8 s8IpAddr[MAX_IP_STR_LEN] = {0};
			strncpy(s8IpAddr, iniFile.getStrValue(), MAX_IP_STR_LEN);
			INET_PTON(s8IpAddr, m_pSysParam->stSysInfoSet.u8SendIp);
		}
		else if (iniFile.isName("Private Port"))
		{
			m_pSysParam->stNetworkSet.u16PrivatePort = iniFile.getIntValue();
		}
		else if (iniFile.isName("Web Port"))
		{
			m_pSysParam->stNetworkSet.u16WebPort = iniFile.getIntValue();
		}
		else if (iniFile.isName("CAC Address"))
		{
            S8 s8IpAddr[MAX_IP_STR_LEN] = {0};
			strncpy(s8IpAddr, iniFile.getStrValue(), MAX_IP_STR_LEN);
			INET_PTON(s8IpAddr, m_pSysParam->stSysInfoSet.u8CACIp);
		}
	}
	iniFile.close();

	if (pNetwork)
	{
		*pNetwork = m_pSysParam->stNetworkSet;
	}

	return 0;
}

S32 IniConfig::WriteNetwork(NETWORK_SET_S *pNetwork)
{
	if ( CheckNetwork(pNetwork, TRUE) < 0)
	{
	}

	m_pSysParam->stNetworkSet= *pNetwork;

	SaveSettings();

	return 0;
}

S32 IniConfig::ReadMaintian (MAINTAIN_SET_S *pMaintain)
{
	IniFile iniFile;
	
	if (!iniFile.openReadOnly(CONFIG_FILE))
	{
		return -1;
	}

	BOOL bFindWatchDog = FALSE;
	BOOL bFindUpAllPtzCtrl = FALSE;
	BOOL bFindAllowH264 = FALSE;
	BOOL bQueryResourceType = FALSE;
	BOOL bUploadCamStatus   = FALSE;
	
	while (iniFile.readNext())
	{
		if (iniFile.isName("Time Address"))
		{
			S8 s8IpAddr[MAX_IP_STR_LEN] = {0};
			strncpy(s8IpAddr, iniFile.getStrValue(), MAX_IP_STR_LEN);
			INET_PTON(s8IpAddr, m_pSysParam->stMaintainSet.u8RecvIP);
		}	
		else if (iniFile.isName("RunWatchDog"))
		{
            m_pSysParam->stMaintainSet.s8WatchDog = iniFile.getIntValue();
			bFindWatchDog = TRUE;
		}
		else if (iniFile.isName("AllowUpPtzCtrl"))
		{
			m_pSysParam->stMaintainSet.s8AllowUpPtzCtrl = iniFile.getIntValue();
			bFindUpAllPtzCtrl = TRUE;
		}
		else if (iniFile.isName("AllowH264"))
		{
			m_pSysParam->stMaintainSet.s8AllowH264 = iniFile.getIntValue();
			bFindAllowH264 = TRUE;
		}
		else if (iniFile.isName("QueryResourceType"))
		{
			m_pSysParam->stMaintainSet.s8QueryResourceType = iniFile.getIntValue();
			bQueryResourceType = TRUE;
		}
		else if (iniFile.isName("UploadCamStatus"))
		{
			m_pSysParam->stMaintainSet.s8UploadCamStatus = iniFile.getIntValue();
			bUploadCamStatus = TRUE;
		}
	}

	iniFile.close();

	if (!bFindWatchDog)
	{
		m_pSysParam->stMaintainSet.s8WatchDog = 1;
        SaveSettings();
	}

	if (!bQueryResourceType)
	{
		m_pSysParam->stMaintainSet.s8QueryResourceType = 0;
        SaveSettings();
	}

	if (!bFindUpAllPtzCtrl)
	{
		m_pSysParam->stMaintainSet.s8AllowUpPtzCtrl = 1;
        SaveSettings();
	}
	
    if (!bFindAllowH264)
	{
        m_pSysParam->stMaintainSet.s8AllowH264 = 1;
        SaveSettings();
	}

	if (!bUploadCamStatus)
	{
		m_pSysParam->stMaintainSet.s8UploadCamStatus = 0;
        SaveSettings();
	}

	if (pMaintain)
	{
		*pMaintain = m_pSysParam->stMaintainSet;
	}
	return 0;
}

S32 IniConfig::WriteMaintian(MAINTAIN_SET_S *pMaintain)
{			
	m_pSysParam->stMaintainSet = *pMaintain;
	
	SaveSettings();

	return 0;
}

S32 IniConfig::ReadPlatform(PLATFROM_SET_S *pPlatform)
{
    IniFile iniFile;
	
	if (!iniFile.openReadOnly(CONFIG_FILE))
	{
		return -1;
	}
	
	S8 s8IpAddr[MAX_IP_STR_LEN] = {0};

	while (iniFile.readNext())
	{
		if (iniFile.isName("PriPlatformIp"))
		{		
			strncpy(s8IpAddr, iniFile.getStrValue(), MAX_IP_STR_LEN);
			INET_PTON(s8IpAddr, m_pSysParam->stPlatformSet.u8PriPlatformIp);
		}
		else if (iniFile.isName("PriPlatformPort"))
		{
			m_pSysParam->stPlatformSet.u16PriPlatformPort = iniFile.getIntValue();
		}
		else if (iniFile.isName("SubPlatformIp"))
		{
			strncpy(s8IpAddr, iniFile.getStrValue(), MAX_IP_STR_LEN);
			INET_PTON(s8IpAddr, m_pSysParam->stPlatformSet.u8SubPlatformIp);
		}
		else if (iniFile.isName("SubPlatformPort"))
		{
			m_pSysParam->stPlatformSet.u16SubPlatformPort = iniFile.getIntValue();
		}
	}
	iniFile.close();
	
	if (pPlatform)
	{
		*pPlatform = m_pSysParam->stPlatformSet;
	}
	return 0;
}

S32 IniConfig::WritePlatform(PLATFROM_SET_S *pPlatform)
{	
	m_pSysParam->stPlatformSet = *pPlatform;
	
	SaveSettings();
	
	return 0;
}

S32 IniConfig::ReadVehicleInfo(VEHICLE_SET_S *pVehicle)
{
    IniFile iniFile;

	if (!iniFile.openReadOnly(CONFIG_FILE))
	{
		return -1;
	}

	while (iniFile.readNext())
	{
		if (iniFile.isName("PlatePath"))
		{
			strncpy(m_pSysParam->stVehicleSet.s8PlatePath, 
					iniFile.getStrValue(), 
					sizeof(m_pSysParam->stVehicleSet.s8PlatePath));	
		}
		else if (iniFile.isName("Port"))
		{
            m_pSysParam->stVehicleSet.u16Port = iniFile.getIntValue();
		}
		else if (iniFile.isName("Time"))
		{
            m_pSysParam->stVehicleSet.u16RelaySndTime = iniFile.getIntValue();

			if (m_pSysParam->stVehicleSet.u16RelaySndTime >= 60)
			{
			    m_pSysParam->stVehicleSet.u16RelaySndTime = 60;
			}
			else if (m_pSysParam->stVehicleSet.u16RelaySndTime == 0)
			{
				m_pSysParam->stVehicleSet.u16RelaySndTime = 10;
			}			
		}
	}
	iniFile.close();

	if (pVehicle)
	{
		*pVehicle = m_pSysParam->stVehicleSet;
	}

	return 0;
}

S32 IniConfig::WriteVehicleInfo(VEHICLE_SET_S *pVehicle)
{
    m_pSysParam->stVehicleSet = *pVehicle;

	SaveSettings();

	return 0;
}

S32 IniConfig::ReadFaceInfo(FACE_SET_S * pFaceParam)
{
	IniFile iniFile;
	
	if (!iniFile.openReadOnly(CONFIG_FILE))
	{
		return -1;
	}
	
	while (iniFile.readNext())
	{
		if (iniFile.isName("FacePicPath"))
		{	
			strncpy(m_pSysParam->stFaceSet.s8FacePicPath, 
			    	iniFile.getStrValue(), 
					sizeof(m_pSysParam->stFaceSet.s8FacePicPath));	
			
			break;
		}
	}
	iniFile.close();
	
	if (pFaceParam)
	{
		*pFaceParam = m_pSysParam->stFaceSet;
	}
	
	return 0;
}

S32 IniConfig::WriteFaceInfo(FACE_SET_S * pFaceParam)
{
	m_pSysParam->stFaceSet = *pFaceParam;
	
	SaveSettings();
	
	return 0;
}

void InitDB(sqlite3 **db, const char * dbname)
{
    sqlite3_open(dbname, db);
}

Mutex g_MtxSqlite;
S32 SqliteExec(sqlite3* db, char *sql)
{
	S32    s32Count = 0;
    S32    s32Ret   = 0;
	char * pErrMsg  = NULL;
	AutoLock lock(g_MtxSqlite);

    do 
    {
        s32Ret = sqlite3_exec(db , sql , NULL, NULL, &pErrMsg);
        if (s32Ret == SQLITE_BUSY)
        {
			CONFIG_DEBUG("%s\n", pErrMsg);
            Msleep(1);
			if(++s32Count > 100)
			{
				return -1;
			}
            continue;
        }
        break;
    }while(1);

	return s32Ret;
}

S32 SqliteGetTable(sqlite3* db, char *sql, char ***pResult, int *nRow, int *nCol)
{
	S32    s32Count = 0;
    S32    s32Ret   = 0;
	char * pErrMsg  = NULL;
	AutoLock lock(g_MtxSqlite);

    do 
    {
        s32Ret = sqlite3_get_table(db, sql, pResult, nRow, nCol, &pErrMsg);	
        if (s32Ret == SQLITE_BUSY)
        {
			CONFIG_DEBUG("%s\n", pErrMsg);
            Msleep(1);
			if(++s32Count > 100)
			{
				return -1;
			}
            continue;
        }
        break;
    }while(1);
	
	return s32Ret;
}

DbConfig::DbConfig(SYS_PARAM_S *pSysParam)
{
	m_pSysParam = pSysParam;
	m_pDb       = NULL;
}

DbConfig::~DbConfig()
{
	if (m_pDb)
	{
		sqlite3_close(m_pDb);
	}
}

S32 DbConfig::LoadSettings()
{
	S32 s32Ret = 0;
	
    InitDB(&m_pDb, "config.db");

	s32Ret += ReadSysInfo(NULL);
	s32Ret += ReadNetwork(NULL);
	s32Ret += ReadMaintain(NULL);

	return s32Ret;
}

S32 DbConfig::ReadSysInfo (SYSINFO_SET_S *pSysInfo)
{
    S32   s32Row = 0;
	S32   s32Col = 0;
	S32   s32Ret = 0;
    S8 ** dbRes  = NULL;
	S8    s8Sql[SQL_LEN] = {0};

	snprintf(s8Sql, SQL_LEN, "select * from SystemInfo");
	CONFIG_DEBUG("sql %s\n", s8Sql);

    s32Ret = SqliteGetTable(m_pDb, s8Sql, &dbRes, &s32Row, &s32Col);
    printf("nRow: %d, nCol: %d\n", s32Row, s32Col);
    if ((0 != s32Ret) || (1 != s32Row))
    {
		CONFIG_DEBUG("SqliteGetTable error!\n");
        sqlite3_free_table(dbRes);
		return -1;
    }

	strncpy(m_pSysParam->stSysInfoSet.s8AddrCode, dbRes[s32Col], MAX_NAME_LEN);
	strncpy(m_pSysParam->stSysInfoSet.s8DevName, dbRes[s32Col+1], MAX_NAME_LEN);

	/* 版本信息从配置文件读取 */
// 	strncpy(m_pSysParam->stSysInfoSet.s8SWVersion, dbRes[s32Col+2], MAX_NAME_LEN);
// 	strncpy(m_pSysParam->stSysInfoSet.s8HWVerison, dbRes[s32Col+3], MAX_NAME_LEN);
// 	strncpy(m_pSysParam->stSysInfoSet.s8IEVerison, dbRes[s32Col+4], MAX_NAME_LEN);

	m_pSysParam->stSysInfoSet.u32DbVerison = atoi(dbRes[s32Col+5]);

	INET_PTON(dbRes[s32Col+6], m_pSysParam->stSysInfoSet.u8AuthIP);

	m_pSysParam->stSysInfoSet.u16RtpBasePort = atoi(dbRes[s32Col+7]);
	m_pSysParam->stSysInfoSet.u16SipPort     = atoi(dbRes[s32Col+8]);
	m_pSysParam->stSysInfoSet.u16RtspPort    = atoi(dbRes[s32Col+9]);
	m_pSysParam->stSysInfoSet.u16PrivatePort = atoi(dbRes[s32Col+10]);
	m_pSysParam->stSysInfoSet.u16WebPort     = atoi(dbRes[s32Col+11]);
	m_pSysParam->stSysInfoSet.u16HttpPort    = atoi(dbRes[s32Col+12]);
	m_pSysParam->stSysInfoSet.u32RecvIntf    = atoi(dbRes[s32Col+13]);
	m_pSysParam->stSysInfoSet.u32SendIntf    = atoi(dbRes[s32Col+14]);
	m_pSysParam->stSysInfoSet.s8EnAutoStart  = atoi(dbRes[s32Col+15]);
	m_pSysParam->stSysInfoSet.s8UpdateSec    = atoi(dbRes[s32Col+16]);

	sqlite3_free_table(dbRes);

	return 0;
}

S32 DbConfig::WriteSysInfo(SYSINFO_SET_S *pSysInfo)
{
	S8 s8Sql[SQL_LEN] = {0};

	if ( CheckSysInfo(pSysInfo, TRUE) < 0)
	{
	}

	S8 s8AuthIp[MAX_IP_STR_LEN] = {0};
	INET_NTOP(pSysInfo->u8AuthIP, s8AuthIp);

	snprintf(s8Sql, SQL_LEN, 
		"Begin Transaction; "
		"delete from SystemInfo; "
		"insert into SystemInfo (AddrCode, DeviceName, SwVersion, HwVersion, "
		"IEVersion, DbVersion, AuthIpAddr, RtpBasePort, SipPort, RtspPort, PrivatePort, "
		"WebPort, HttpPort, RecvIntf, SendIntf, AutoStart, UpdateSec ) values "
		"('%s', '%s', '%s', '%s', '%s', %d, '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d); "
		"Commit Transaction;",
		pSysInfo->s8AddrCode,
		pSysInfo->s8DevName,
		IE_VERSION,
		HARD_VERSION,
		IE_VERSION,
		DB_VERSION_CURRENT,
		s8AuthIp,
		pSysInfo->u16RtpBasePort,
		pSysInfo->u16SipPort,
		pSysInfo->u16RtspPort,
		pSysInfo->u16PrivatePort,
		pSysInfo->u16WebPort,
		pSysInfo->u16HttpPort,
		pSysInfo->u32RecvIntf,
		pSysInfo->u32SendIntf,
		pSysInfo->s8EnAutoStart,
		pSysInfo->s8UpdateSec);
	CONFIG_DEBUG("s8Sql: %s\n", s8Sql);
	SqliteExec(m_pDb, s8Sql);
	m_pSysParam->stSysInfoSet = *pSysInfo;

	return 0;
}

S32 DbConfig::ReadNetwork (NETWORK_SET_S *pNetwork)
{
//    S8 *s8Sql = "select * from Network";


	return 0;
}

S32 DbConfig::WriteNetwork(NETWORK_SET_S *pNetwork)
{
	return 0;
}

S32 DbConfig::ReadDevice(DEVICE_SET_S  *pDevice)
{
	S32  s32Row    = 0;
	S32  s32Col    = 0;
	S32  s32Ret    = 0;
	S8 **dbRes     = NULL;
	S8   s8Sql[SQL_LEN] = {0};

	if (strcmp(pDevice->s8AddrCode, "") != 0)
	{
		snprintf(s8Sql, SQL_LEN, 
		"select * from DeviceInfo where AddrCode='%s';", 
		pDevice->s8AddrCode);
	}
	else if (strcmp(pDevice->s8PareAddrCode,"") != 0 )
	{
		snprintf(s8Sql, SQL_LEN, 
			"select * from DeviceInfo "
			"where ParentAddrCode='%s' and ChannelNo=%d;", 
			pDevice->s8PareAddrCode,
			pDevice->s32ChnNo);
	}
	else
	{
		return -1;
	}

	CONFIG_DEBUG("sql %s\n", s8Sql);

	s32Ret = SqliteGetTable(m_pDb, s8Sql, &dbRes, &s32Row, &s32Col);
	printf("nRow: %d, nCol: %d\n", s32Row, s32Col);
	if ((0 != s32Ret) || (1 != s32Row))
	{
		CONFIG_DEBUG("SqliteGetTable error!\n");
		sqlite3_free_table(dbRes);
		return -1;
	}

	strncpy(pDevice->s8AddrCode, dbRes[s32Col], MAX_ADDR_LEN);
	strncpy(pDevice->s8DevName,  dbRes[s32Col+1], MAX_NAME_LEN);
	pDevice->s32ChnNum  = atoi(dbRes[s32Col+2]);
	pDevice->s32ChnNo   = atoi(dbRes[s32Col+3]);
	if (dbRes[s32Col+4])
		strncpy(pDevice->s8PareAddrCode, dbRes[s32Col+4], MAX_ADDR_LEN);
	pDevice->s8DisLevel = atoi(dbRes[s32Col+5]);
	pDevice->u16DevType = atoi(dbRes[s32Col+6]);
	if (dbRes[s32Col+7])
		strncpy(pDevice->s8DevDesc, dbRes[s32Col+7], 100);
	pDevice->s8ManuType = atoi(dbRes[s32Col+8]);
	INET_PTON(dbRes[s32Col+9], pDevice->u8IPAddr);
	pDevice->u16Port = atoi(dbRes[s32Col+10]);
	pDevice->s8LoginType = atoi(dbRes[s32Col+11]);
	if(dbRes[s32Col+12])
		strncpy(pDevice->s8Username, dbRes[s32Col+12], 36);
	if(dbRes[s32Col+13])
		strncpy(pDevice->s8Password, dbRes[s32Col+13], 36);
	pDevice->s8StreamType   = atoi(dbRes[s32Col+14]);
	pDevice->s8AlarmInNum   = atoi(dbRes[s32Col+15]);
	pDevice->s8AlarmOutNum  = atoi(dbRes[s32Col+16]);
	pDevice->s8Organ        = atoi(dbRes[s32Col+17]);
	pDevice->s8ProtoType    = atoi(dbRes[s32Col+18]);
	pDevice->u16HttpPort    = atoi(dbRes[s32Col+19]);
	pDevice->u16RtspPort    = atoi(dbRes[s32Col+20]);
	pDevice->u16RegInterval = atoi(dbRes[s32Col+21]);
	pDevice->u16RegExpires  = atoi(dbRes[s32Col+22]);
	pDevice->u16KeepAlive   = atoi(dbRes[s32Col+23]);
	
	if(dbRes[s32Col+24])
       strncpy(pDevice->s8RetrackAddrCode, dbRes[s32Col+24], 36);
	pDevice->s8DomainType   = atoi(dbRes[s32Col+25]);
	if(dbRes[s32Col+26])
       strncpy(pDevice->s8Domain, dbRes[s32Col+26], 36);

	sqlite3_free_table(dbRes);

	return 0;
}

S32 DbConfig::ReadDevice_2(DEVICE_SET_S  *pDevice)
{
	S32  s32Row    = 0;
	S32  s32Col    = 0;
	S32  s32Ret    = 0;
	S8 **dbRes     = NULL;
	S8   s8Sql[SQL_LEN] = {0};
	
	S8 s8DeviceIp[MAX_IP_STR_LEN] = {0};
	INET_NTOP(pDevice->u8IPAddr, s8DeviceIp);

	if (strcmp(pDevice->s8AddrCode, "") != 0)
	{
		snprintf(s8Sql, SQL_LEN, 
			"select * from DeviceInfo where AddrCode='%s' or "
			"(Ip='%s' AND Port=%d) or DeviceName='%s';", 
			pDevice->s8AddrCode,s8DeviceIp,pDevice->u16Port,\
			pDevice->s8DevName);
	}
	else if (strcmp(pDevice->s8PareAddrCode,"") != 0 )
	{
		snprintf(s8Sql, SQL_LEN, 
			"select * from DeviceInfo "
			"where ParentAddrCode='%s' and ChannelNo=%d;", 
			pDevice->s8PareAddrCode,
			pDevice->s32ChnNo);
	}
	else
	{
		return -1;
	}
	
	CONFIG_DEBUG("sql %s\n", s8Sql);
	
	s32Ret = SqliteGetTable(m_pDb, s8Sql, &dbRes, &s32Row, &s32Col);
	printf("nRow: %d, nCol: %d\n", s32Row, s32Col);

	if(s32Ret != 0 || s32Row > 1)
	{
		CONFIG_DEBUG("SqliteGetTable error!\n");
		sqlite3_free_table(dbRes);
		return -2;
	}
	if(s32Row == 0)  return -1;

	if(strcmp(pDevice->s8AddrCode,dbRes[s32Col]) != 0) return -2;
	
	strncpy(pDevice->s8AddrCode, dbRes[s32Col], MAX_ADDR_LEN);
	strncpy(pDevice->s8DevName,  dbRes[s32Col+1], MAX_NAME_LEN);
	pDevice->s32ChnNum  = atoi(dbRes[s32Col+2]);
	pDevice->s32ChnNo   = atoi(dbRes[s32Col+3]);
	if (dbRes[s32Col+4])
		strncpy(pDevice->s8PareAddrCode, dbRes[s32Col+4], MAX_ADDR_LEN);
	pDevice->s8DisLevel = atoi(dbRes[s32Col+5]);
	pDevice->u16DevType = atoi(dbRes[s32Col+6]);
	if (dbRes[s32Col+7])
		strncpy(pDevice->s8DevDesc, dbRes[s32Col+7], 100);
	pDevice->s8ManuType = atoi(dbRes[s32Col+8]);
	INET_PTON(dbRes[s32Col+9], pDevice->u8IPAddr);
	pDevice->u16Port = atoi(dbRes[s32Col+10]);
	pDevice->s8LoginType = atoi(dbRes[s32Col+11]);
	if(dbRes[s32Col+12])
		strncpy(pDevice->s8Username, dbRes[s32Col+12], 36);
	if(dbRes[s32Col+13])
		strncpy(pDevice->s8Password, dbRes[s32Col+13], 36);
	pDevice->s8StreamType   = atoi(dbRes[s32Col+14]);
	pDevice->s8AlarmInNum   = atoi(dbRes[s32Col+15]);
	pDevice->s8AlarmOutNum  = atoi(dbRes[s32Col+16]);
	pDevice->s8Organ        = atoi(dbRes[s32Col+17]);
	pDevice->s8ProtoType    = atoi(dbRes[s32Col+18]);
	pDevice->u16HttpPort    = atoi(dbRes[s32Col+19]);
	pDevice->u16RtspPort    = atoi(dbRes[s32Col+20]);
	pDevice->u16RegInterval = atoi(dbRes[s32Col+21]);
	pDevice->u16RegExpires  = atoi(dbRes[s32Col+22]);
	pDevice->u16KeepAlive   = atoi(dbRes[s32Col+23]);
	
	if(dbRes[s32Col+24])
       strncpy(pDevice->s8RetrackAddrCode, dbRes[s32Col+24], 36);
	pDevice->s8DomainType   = atoi(dbRes[s32Col+25]);
	if(dbRes[s32Col+26])
       strncpy(pDevice->s8Domain, dbRes[s32Col+26], 36);
	
	sqlite3_free_table(dbRes);
	
	return 0;
}

void DbConfig::BeginComTrans()
{
   sqlite3_exec(m_pDb, "BEGIN TRANSACTION", 0, 0, 0);
}

void DbConfig::EndComTrans()
{
   sqlite3_exec(m_pDb, "COMMIT", 0, 0, 0);
}

S32 DbConfig::WriteDevice(DEVICE_SET_S *pDevice)
{
	ASSERT(pDevice);
	
	S8   s8Sql[SQL_LEN] = {0};
	
	S8 s8IpAddr[MAX_IP_STR_LEN] = {0};
	INET_NTOP(pDevice->u8IPAddr, s8IpAddr);
	
	snprintf(s8Sql, SQL_LEN, 
		"replace into DeviceInfo "
		"(AddrCode, DeviceName, ChannelNum, ChannelNo, ParentAddrCode, "
		"DisLevel, DeviceType, DeviceDesc, ManuType, Ip, Port, "
		"LoginType, Username, Password, StreamType, Organization, "
		"AlarmInNum, AlarmOutNum, ProtoType, HttpPort, "
		"RtspPort, RegInterval, RegExpires, KeepAlive, ReTrackAddrCode, DomainType, Domain, AreaName) "
		"values ('%s', '%s', %d, %d, '%s', %d, %d, '%s', %d, '%s', %d, "
		"%d, '%s', '%s', %d, %d, %d, %d, %d, %d, %d, %d, %d, %d, '%s', %d, '%s', '%s');", 
		pDevice->s8AddrCode,
		pDevice->s8DevName,
		pDevice->s32ChnNum,
		pDevice->s32ChnNo,
		pDevice->s8PareAddrCode,
		pDevice->s8DisLevel,
		pDevice->u16DevType,
		pDevice->s8DevDesc,
		pDevice->s8ManuType,
		s8IpAddr,
		pDevice->u16Port,
		pDevice->s8LoginType,
		pDevice->s8Username,
		pDevice->s8Password,
		pDevice->s8StreamType,
		pDevice->s8Organ,
		pDevice->s8AlarmInNum,
		pDevice->s8AlarmOutNum,
		pDevice->s8ProtoType,
		pDevice->u16HttpPort,
		pDevice->u16RtspPort,
		pDevice->u16RegInterval,
		pDevice->u16RegExpires,
		pDevice->u16KeepAlive,
		pDevice->s8RetrackAddrCode,
		pDevice->s8DomainType,
		pDevice->s8Domain,
		pDevice->s8AreaName
		);

	CONFIG_DEBUG("sql %s\n", s8Sql);
	
	return SqliteExec(m_pDb, s8Sql);
}

S32 DbConfig::DeleteDevice(S8 *ps8AddrCode)
{
	ASSERT(ps8AddrCode);
	
	S8   s8Sql[SQL_LEN] = {0};
	
	snprintf(s8Sql, SQL_LEN, "delete from DeviceInfo \
		where AddrCode='%s' or ParentAddrCode='%s';",
			ps8AddrCode,
			ps8AddrCode);
	
	CONFIG_DEBUG("sql %s\n", s8Sql);
	
	return SqliteExec(m_pDb, s8Sql);
}

S32 DbConfig::DetachReTrackCarToCamera(S8 * ps8AddrCode)
{
	ASSERT(ps8AddrCode);

	S8    s8Sql[SQL_LEN] = {0};
	snprintf(s8Sql, SQL_LEN, "update DeviceInfo set ReTrackAddrCode = '' "
		            "where ReTrackAddrCode = '%s'; ", ps8AddrCode);
    CONFIG_DEBUG("sql %s\n", s8Sql);
	
	return SqliteExec(m_pDb, s8Sql);
}

S32 DbConfig::DeleteRePresetInfo(S8 *ps8AddrCode)
{
	vector<string> vecAddrCode;

	S32     s32Row = 0;
	S32     s32Col = 0;
	S32     s32Ret = 0;
	S32     s32I   = 0;
	S8   ** dbRes  = NULL;
	
	S8      s8Sql[SQL_LEN] = {0};
	
	snprintf(s8Sql, SQL_LEN, "select AddrCode from DeviceInfo where ParentAddrCode='%s';",
		ps8AddrCode);
	CONFIG_DEBUG("sql %s\n", s8Sql);
	
    s32Ret = SqliteGetTable(m_pDb, s8Sql, &dbRes, &s32Row, &s32Col);
    printf("nRow: %d, nCol: %d\n", s32Row, s32Col);
    if (0 == s32Ret)
    {
		for (s32I = 0; s32I < s32Row; s32I++)
		{
			 string strAddrCode = dbRes[(s32I+1)*s32Col + 0];
             vecAddrCode.push_back(strAddrCode);
		}
    }
	else
	{
		sqlite3_free_table(dbRes);
		return -1;
	}
	sqlite3_free_table(dbRes);

	for (vector<string>::iterator itAddr = vecAddrCode.begin(); itAddr!=vecAddrCode.end(); ++itAddr)
	{
		snprintf(s8Sql, SQL_LEN, "delete from PresetInfo where AddrCode='%s';",
			        (*itAddr).c_str());
		CONFIG_DEBUG("sql %s\n", s8Sql);

        return SqliteExec(m_pDb, s8Sql);
	}

	return 0;
}

S32 DbConfig::WriteStation(STATION_SET_S *pStation)
{
	ASSERT(pStation);

	S8   s8Sql[SQL_LEN] = {0};

	if (pStation->u32StatCode == 0)
	{
		snprintf(s8Sql, SQL_LEN, 
			"replace into Station "
			"(ProvinceCode, CityCode, StationName, Describe) "
			"values (%d, %d, '%s', '%s');", 
			pStation->u32ProvCode,
			pStation->u32CityCode,
			pStation->s8StatName,
			pStation->s8StatDesc);
	}
	else
	{
		snprintf(s8Sql, SQL_LEN, 
			"replace into Station "
			"(StationCode, ProvinceCode, CityCode, StationName, Describe) "
			"values (%d, %d, %d, '%s', '%s');", 
			pStation->u32StatCode,
			pStation->u32ProvCode,
			pStation->u32CityCode,
			pStation->s8StatName,
			pStation->s8StatDesc);
	}

	CONFIG_DEBUG("sql %s\n", s8Sql);

	return SqliteExec(m_pDb, s8Sql);
}

S32 DbConfig::DeleteStation(U32 u32StatCode)
{
	S8   s8Sql[SQL_LEN] = {0};
	
	snprintf(s8Sql, SQL_LEN, 
	"delete from Station where StationCode=%d;", 
	u32StatCode);
	
	CONFIG_DEBUG("sql %s\n", s8Sql);
	
	return SqliteExec(m_pDb, s8Sql);
}

S32 DbConfig::WriteUserGroup(U32 u32UserId, U32 u32GroupId, U32 u32GroupType)
{
	S8   s8Sql[SQL_LEN] = {0};
	
	snprintf(s8Sql, SQL_LEN, 
		"replace into UserGroup "
		"(UserId, GroupId, GroupType) "
		"values (%d, %d, %d);", 
		u32UserId,
		u32GroupId,
		u32GroupType);
	
	CONFIG_DEBUG("sql %s\n", s8Sql);
	
	return SqliteExec(m_pDb, s8Sql);
}


S32 DbConfig::DeleteUserGroup(U32 u32UserId, U32 u32GroupId, U32 u32GroupType)
{
	S8   s8Sql[SQL_LEN] = {0};
	
	snprintf(s8Sql, SQL_LEN, 
		"delete from UserGroup "
		"where UserId=%d and GroupId=%d and GroupType=%d;",
		u32UserId,
		u32GroupId,
		u32GroupType);
	
	CONFIG_DEBUG("sql %s\n", s8Sql);
	
	return SqliteExec(m_pDb, s8Sql);
}

S32 DbConfig::ReadMaintain (MAINTAIN_SET_S *pMaintian)
{
	S32   s32Row = 0;
	S32   s32Col = 0;
	S32   s32Ret = 0;
    S8 ** dbRes  = NULL;
	S8    s8Sql[SQL_LEN] = {0};
	
	snprintf(s8Sql, SQL_LEN, "select * from Maintain");
	CONFIG_DEBUG("sql %s\n", s8Sql);
	
    s32Ret = SqliteGetTable(m_pDb, s8Sql, &dbRes, &s32Row, &s32Col);
    printf("nRow: %d, nCol: %d\n", s32Row, s32Col);
    if ((0 != s32Ret) || (1 != s32Row))
    {
		CONFIG_DEBUG("SqliteGetTable error!\n");
        sqlite3_free_table(dbRes);
		return -1;
    }
	
	m_pSysParam->stMaintainSet.s8EnRecvTime    = atoi(dbRes[s32Col]);
	m_pSysParam->stMaintainSet.s8EnSendTime    = atoi(dbRes[s32Col+1]);
	m_pSysParam->stMaintainSet.s32TimeInterval = atoi(dbRes[s32Col+2]);

	if (pMaintian)
	{
		*pMaintian = m_pSysParam->stMaintainSet;
	}
	
	sqlite3_free_table(dbRes);
	
	return 0;
}

S32 DbConfig::WriteMaintain(MAINTAIN_SET_S *pMaintian)
{
	S8 s8Sql[SQL_LEN] = {0};	
	
	snprintf(s8Sql, SQL_LEN, 
		"Begin Transaction; "
		"delete from Maintain; "
		"insert into Maintain (EnableRecvTime, EnableSendTime, AutoTimeInterv) values "
		"(%d, %d, %d); "
		"Commit Transaction;",
		pMaintian->s8EnRecvTime,
		pMaintian->s8EnSendTime,
		pMaintian->s32TimeInterval);
	CONFIG_DEBUG("s8Sql: %s\n", s8Sql);
	SqliteExec(m_pDb, s8Sql);

	m_pSysParam->stMaintainSet = *pMaintian;
	
	return 0;
}

S32 DbConfig::GetProvinceCode(S8 *ps8ProvName)
{
	ASSERT(ps8ProvName);
	
	S32  s32Row = 0;
	S32  s32Col = 0;
	S32  s32Ret = 0;
    S8 **dbRes  = NULL;
	S8   s8Sql[SQL_LEN] = {0};
	
	snprintf(s8Sql, SQL_LEN, 
		"Select ProvinceCode from Province "
		"where ProvinceName='%s'", 
		ps8ProvName);
	
	CONFIG_DEBUG("sql %s\n", s8Sql);
	
	s32Ret = SqliteGetTable(m_pDb, s8Sql, &dbRes, &s32Row, &s32Col);
    CONFIG_DEBUG("Row: %d, Col: %d\n", s32Row, s32Col);
    if ((0 != s32Ret) || (1 != s32Row))
    {
		CONFIG_DEBUG("SqliteGetTable error!\n");
        sqlite3_free_table(dbRes);
		return -1;
    }
	
	s32Ret = atoi(dbRes[s32Col]);
	sqlite3_free_table(dbRes);
	
	return s32Ret;
}

S32 DbConfig::GetProvinceName(S32 s32ProvCode, S8 *ps8ProvName)
{
	S32  s32Row = 0;
	S32  s32Col = 0;
	S32  s32Ret = 0;
    S8 **dbRes  = NULL;
	S8   s8Sql[SQL_LEN] = {0};
	
	if (!ps8ProvName)
	{
		return -1;
	}
	
	snprintf(s8Sql, SQL_LEN, 
		"Select ProvinceName from Province "
		"where ProvinceCode=%d;", s32ProvCode);
	
	CONFIG_DEBUG("sql %s\n", s8Sql);
	
	s32Ret = SqliteGetTable(m_pDb, s8Sql, &dbRes, &s32Row, &s32Col);
    CONFIG_DEBUG("Row: %d, Col: %d\n", s32Row, s32Col);
    if ((0 != s32Ret) || (1 != s32Row))
    {
		CONFIG_DEBUG("SqliteGetTable error!\n");
        sqlite3_free_table(dbRes);
		return -1;
    }
	
	strncpy(ps8ProvName, dbRes[s32Col], MAX_NAME_LEN);
	sqlite3_free_table(dbRes);
	
	return 0;
}

S32 DbConfig::GetCityCode(S8 *ps8ProvName, S8 *ps8CityName)
{
	ASSERT(ps8ProvName);
	ASSERT(ps8CityName);

	S32  s32Row = 0;
	S32  s32Col = 0;
	S32  s32Ret = 0;
    S8 **dbRes  = NULL;
	S8   s8Sql[SQL_LEN] = {0};
		
	snprintf(s8Sql, SQL_LEN, 
		"Select CityCode from City "
		"inner join Province on Province.ProvinceCode=City.ProvinceCode "
		"where Province.ProvinceName='%s' and City.CityName='%s';", 
		ps8ProvName, ps8CityName);
	
	CONFIG_DEBUG("sql %s\n", s8Sql);
	
	s32Ret = SqliteGetTable(m_pDb, s8Sql, &dbRes, &s32Row, &s32Col);
    CONFIG_DEBUG("Row: %d, Col: %d\n", s32Row, s32Col);
    if ((0 != s32Ret) || (1 != s32Row))
    {
		CONFIG_DEBUG("SqliteGetTable error!\n");
        sqlite3_free_table(dbRes);
		return -1;
    }
	
	s32Ret = atoi(dbRes[s32Col]);
	sqlite3_free_table(dbRes);

	return s32Ret;
}

S32 DbConfig::GetCityName(S32 s32ProvCode, S32 s32CityCode, S8 *ps8CityName)
{
	S32  s32Row = 0;
	S32  s32Col = 0;
	S32  s32Ret = 0;
    S8 **dbRes  = NULL;
	S8   s8Sql[SQL_LEN] = {0};
	
	if (!ps8CityName)
	{
		return -1;
	}
	
	snprintf(s8Sql, SQL_LEN, 
		"Select CityName from City "
		"where ProvinceCode=%d and CityCode=%d;",
		s32ProvCode, s32CityCode);
	
	CONFIG_DEBUG("sql %s\n", s8Sql);
	
	
	s32Ret = SqliteGetTable(m_pDb, s8Sql, &dbRes, &s32Row, &s32Col);
    CONFIG_DEBUG("Row: %d, Col: %d\n", s32Row, s32Col);
    if ((0 != s32Ret) || (1 != s32Row))
    {
		CONFIG_DEBUG("SqliteGetTable error!\n");
        sqlite3_free_table(dbRes);
		return -1;
    }
	
	strncpy(ps8CityName, dbRes[s32Col], MAX_NAME_LEN);
	sqlite3_free_table(dbRes);
	
	return 0;
}

S32 DbConfig::GetStationCode(S32 s32ProvCode, S32 s32CityCode, S8 *ps8StationName)
{
	ASSERT(ps8StationName);

	S32  s32Row = 0;
	S32  s32Col = 0;
	S32  s32Ret = 0;
    S8 **dbRes  = NULL;
	S8   s8Sql[SQL_LEN] = {0};
		
	snprintf(s8Sql, SQL_LEN, 
		"Select StationCode from Station "
		"where StationName='%s' and ProvinceCode=%d and CityCode=%d;", 
		ps8StationName, s32ProvCode, s32CityCode);
	
	CONFIG_DEBUG("sql %s\n", s8Sql);

	s32Ret = SqliteGetTable(m_pDb, s8Sql, &dbRes, &s32Row, &s32Col);
    CONFIG_DEBUG("Row: %d, Col: %d\n", s32Row, s32Col);
    if ((0 != s32Ret) || (1 != s32Row))
    {
		CONFIG_DEBUG("SqliteGetTable error!\n");
        sqlite3_free_table(dbRes);
		return -1;
    }
	
	s32Ret = atoi(dbRes[s32Col]);
	sqlite3_free_table(dbRes);

	return s32Ret;
}

S32 DbConfig::GetStationName(S32 s32StationCode, S8 *ps8StationName)
{
	S32  s32Row = 0;
	S32  s32Col = 0;
	S32  s32Ret = 0;
    S8 **dbRes  = NULL;
	S8   s8Sql[SQL_LEN] = {0};
	
	if (!ps8StationName)
	{
		return -1;
	}
	
	snprintf(s8Sql, SQL_LEN, 
		"Select StationName from Station "
		"where StationCode=%d;", 
		s32StationCode);
	
	CONFIG_DEBUG("sql %s\n", s8Sql);
		
	s32Ret = SqliteGetTable(m_pDb, s8Sql, &dbRes, &s32Row, &s32Col);
    CONFIG_DEBUG("Row: %d, Col: %d\n", s32Row, s32Col);
    if ((0 != s32Ret) || (1 != s32Row))
    {
		CONFIG_DEBUG("SqliteGetTable error!\n");
        sqlite3_free_table(dbRes);
		return -1;
    }
	
	strncpy(ps8StationName, dbRes[s32Col], MAX_NAME_LEN);
	sqlite3_free_table(dbRes);
	
	return 0;
}

S32 DbConfig::GetObjectCode(S8 *ps8ObjectName)
{
	ASSERT(ps8ObjectName);
	
	S32  s32Row = 0;
	S32  s32Col = 0;
	S32  s32Ret = 0;
    S8 **dbRes  = NULL;
	S8   s8Sql[SQL_LEN] = {0};
	
	snprintf(s8Sql, SQL_LEN, 
		"select ObjectCode from Object "
		"where ObjectName='%s';", 
		ps8ObjectName);
	
	CONFIG_DEBUG("sql %s\n", s8Sql);
	
	s32Ret = SqliteGetTable(m_pDb, s8Sql, &dbRes, &s32Row, &s32Col);
    CONFIG_DEBUG("Row: %d, Col: %d\n", s32Row, s32Col);
    if ((0 != s32Ret) || (1 != s32Row))
    {
		CONFIG_DEBUG("SqliteGetTable error!\n");
        sqlite3_free_table(dbRes);
		return -1;
    }
	
	s32Ret = atoi(dbRes[s32Col]);
	sqlite3_free_table(dbRes);
	
	return s32Ret;
}

S32 DbConfig::GetObjectName(S32 s32ObjectCode, S8 *ps8ObjectName)
{
	S32  s32Row = 0;
	S32  s32Col = 0;
	S32  s32Ret = 0;
    S8 **dbRes  = NULL;
	S8   s8Sql[SQL_LEN] = {0};
	
	if (!ps8ObjectName)
	{
		return -1;
	}
	
	snprintf(s8Sql, SQL_LEN, 
		"Select ObjectName from Object "
		"where ObjectCode=%d;", 
		s32ObjectCode);
	
	CONFIG_DEBUG("sql %s\n", s8Sql);
	
	
	s32Ret = SqliteGetTable(m_pDb, s8Sql, &dbRes, &s32Row, &s32Col);
    CONFIG_DEBUG("Row: %d, Col: %d\n", s32Row, s32Col);
    if ((0 != s32Ret) || (1 != s32Row))
    {
		CONFIG_DEBUG("SqliteGetTable error!\n");
        sqlite3_free_table(dbRes);
		return -1;
    }
	
	strncpy(ps8ObjectName, dbRes[s32Col], MAX_NAME_LEN);
	sqlite3_free_table(dbRes);
	
	return 0;
}

S32 DbConfig::GetStreamTypeCode(S8 *ps8StreamName)
{
	ASSERT(ps8StreamName);

	S32  s32Row = 0;
	S32  s32Col = 0;
	S32  s32Ret = 0;
    S8 **dbRes  = NULL;
	S8   s8Sql[SQL_LEN] = {0};
		
	snprintf(s8Sql, SQL_LEN, 
		"Select StreamTypeCode from StreamType "
		"where StreamTypeName='%s';", 
		ps8StreamName);
	
	CONFIG_DEBUG("sql %s\n", s8Sql);

	s32Ret = SqliteGetTable(m_pDb, s8Sql, &dbRes, &s32Row, &s32Col);
    CONFIG_DEBUG("Row: %d, Col: %d\n", s32Row, s32Col);
    if ((0 != s32Ret) || (1 != s32Row))
    {
		CONFIG_DEBUG("SqliteGetTable error!\n");
        sqlite3_free_table(dbRes);
		return -1;
    }
	
	s32Ret = atoi(dbRes[s32Col]);
	sqlite3_free_table(dbRes);

	return s32Ret;
}

S32 DbConfig::GetStreamTypeName(S32 s32StreamCode, S8 *ps8StreamName)
{
	S32  s32Row = 0;
	S32  s32Col = 0;
	S32  s32Ret = 0;
    S8 **dbRes  = NULL;
	S8   s8Sql[SQL_LEN] = {0};
	
	if (!ps8StreamName)
	{
		return -1;
	}
	
	snprintf(s8Sql, SQL_LEN, 
		"Select StreamTypeName from StreamType "
		"where StreamTypeCode=%d;", 
		s32StreamCode);
	
	CONFIG_DEBUG("sql %s\n", s8Sql);
	
	s32Ret = SqliteGetTable(m_pDb, s8Sql, &dbRes, &s32Row, &s32Col);
    CONFIG_DEBUG("Row: %d, Col: %d\n", s32Row, s32Col);
    if ((0 != s32Ret) || (1 != s32Row))
    {
		CONFIG_DEBUG("SqliteGetTable error!\n");
        sqlite3_free_table(dbRes);
		return -1;
    }
	
	strncpy(ps8StreamName, dbRes[s32Col], MAX_NAME_LEN);
	sqlite3_free_table(dbRes);
	
	return 0;
}

S32 DbConfig::GetDevTypeCode(S8 *ps8DevTypeName)
{
	ASSERT(ps8DevTypeName);
	
	S32  s32Row = 0;
	S32  s32Col = 0;
	S32  s32Ret = 0;
    S8 **dbRes  = NULL;
	S8   s8Sql[SQL_LEN] = {0};
	
	snprintf(s8Sql, SQL_LEN, 
		"select DevTypeCode from DeviceType "
		"where DevTypeName='%s';", 
		ps8DevTypeName);
	
	CONFIG_DEBUG("sql %s\n", s8Sql);
	
	s32Ret = SqliteGetTable(m_pDb, s8Sql, &dbRes, &s32Row, &s32Col);
    CONFIG_DEBUG("Row: %d, Col: %d\n", s32Row, s32Col);
    if ((0 != s32Ret) || (1 != s32Row))
    {
		CONFIG_DEBUG("SqliteGetTable error!\n");
        sqlite3_free_table(dbRes);
		return -1;
    }
	
	s32Ret = atoi(dbRes[s32Col]);
	sqlite3_free_table(dbRes);
	
	return s32Ret;
}

S32 DbConfig::GetDevTypeName(S32 s32DevTypeCode, S8 *ps8DevTypeName)
{
	S32  s32Row = 0;
	S32  s32Col = 0;
	S32  s32Ret = 0;
    S8 **dbRes  = NULL;
	S8   s8Sql[SQL_LEN] = {0};
	
	if (!ps8DevTypeName)
	{
		return -1;
	}
	
	snprintf(s8Sql, SQL_LEN, 
		"Select DevTypeName from DeviceType "
		"where DevTypeCode=%d;", 
		s32DevTypeCode);
	
	CONFIG_DEBUG("sql %s\n", s8Sql);
	
	s32Ret = SqliteGetTable(m_pDb, s8Sql, &dbRes, &s32Row, &s32Col);
    CONFIG_DEBUG("Row: %d, Col: %d\n", s32Row, s32Col);
    if ((0 != s32Ret) || (1 != s32Row))
    {
		CONFIG_DEBUG("SqliteGetTable error!\n");
        sqlite3_free_table(dbRes);
		return -1;
    }
	
	strncpy(ps8DevTypeName, dbRes[s32Col], MAX_NAME_LEN);
	sqlite3_free_table(dbRes);
	
	return 0;
}

S32 DbConfig::GetManuTypeCode(S8 *ps8ManuTypeName)
{
	ASSERT(ps8ManuTypeName);
	
	S32  s32Row = 0;
	S32  s32Col = 0;
	S32  s32Ret = 0;
    S8 **dbRes  = NULL;
	S8   s8Sql[SQL_LEN] = {0};
	
	snprintf(s8Sql, SQL_LEN, 
		"select ManuTypeCode from ManuType "
		"where ManuTypeName='%s';", 
		ps8ManuTypeName);
	
	CONFIG_DEBUG("sql %s\n", s8Sql);
	
	s32Ret = SqliteGetTable(m_pDb, s8Sql, &dbRes, &s32Row, &s32Col);
    CONFIG_DEBUG("Row: %d, Col: %d\n", s32Row, s32Col);
    if ((0 != s32Ret) || (1 != s32Row))
    {
		CONFIG_DEBUG("SqliteGetTable error!\n");
        sqlite3_free_table(dbRes);
		return -1;
    }
	
	s32Ret = atoi(dbRes[s32Col]);
	sqlite3_free_table(dbRes);
	
	return s32Ret;
}

S32 DbConfig::GetManuTypeName(S32 s32ManuTypeCode, S8 *ps8ManuTypeName)
{
	S32  s32Row = 0;
	S32  s32Col = 0;
	S32  s32Ret = 0;
    S8 **dbRes  = NULL;
	S8   s8Sql[SQL_LEN] = {0};
	
	if (!ps8ManuTypeName)
	{
		return -1;
	}
	
	snprintf(s8Sql, SQL_LEN, 
		"Select ManuTypeName from ManuType "
		"where ManuTypeCode=%d;", 
		s32ManuTypeCode);
	
	CONFIG_DEBUG("sql %s\n", s8Sql);
	
	
	s32Ret = SqliteGetTable(m_pDb, s8Sql, &dbRes, &s32Row, &s32Col);
    CONFIG_DEBUG("Row: %d, Col: %d\n", s32Row, s32Col);
    if ((0 != s32Ret) || (1 != s32Row))
    {
		CONFIG_DEBUG("SqliteGetTable error!\n");
        sqlite3_free_table(dbRes);
		return -1;
    }
	
	strncpy(ps8ManuTypeName, dbRes[s32Col], MAX_NAME_LEN);
	sqlite3_free_table(dbRes);
	
	return 0;
}

S32 DbConfig::GetProtoTypeCode(S8 *ps8ProtoTypeName)
{
	ASSERT(ps8ProtoTypeName);
	
	S32  s32Row = 0;
	S32  s32Col = 0;
	S32  s32Ret = 0;
    S8 **dbRes  = NULL;
	S8   s8Sql[SQL_LEN] = {0};
	
	snprintf(s8Sql, SQL_LEN, 
		"select ProtoTypeCode from ProtocolType "
		"where ProtoTypeName='%s';", 
		ps8ProtoTypeName);
	
	CONFIG_DEBUG("sql %s\n", s8Sql);
	
	s32Ret = SqliteGetTable(m_pDb, s8Sql, &dbRes, &s32Row, &s32Col);
    CONFIG_DEBUG("Row: %d, Col: %d\n", s32Row, s32Col);
    if ((0 != s32Ret) || (1 != s32Row))
    {
		CONFIG_DEBUG("SqliteGetTable error!\n");
        sqlite3_free_table(dbRes);
		return -1;
    }
	
	s32Ret = atoi(dbRes[s32Col]);
	sqlite3_free_table(dbRes);
	
	return s32Ret;
}

S32 DbConfig::GetProtoTypeName(S32 s32ProtoTypeCode, S8 *ps8ProtoTypeName)
{
	S32  s32Row = 0;
	S32  s32Col = 0;
	S32  s32Ret = 0;
    S8 **dbRes  = NULL;
	S8   s8Sql[SQL_LEN] = {0};
	
	if (!ps8ProtoTypeName)
	{
		return -1;
	}
	
	snprintf(s8Sql, SQL_LEN, 
		"Select ProtoTypeName from ProtocolType "
		"where ProtoTypeCode=%d;", s32ProtoTypeCode);
	
	CONFIG_DEBUG("sql %s\n", s8Sql);
	
	
	s32Ret = SqliteGetTable(m_pDb, s8Sql, &dbRes, &s32Row, &s32Col);
    CONFIG_DEBUG("Row: %d, Col: %d\n", s32Row, s32Col);
    if ((0 != s32Ret) || (1 != s32Row))
    {
		CONFIG_DEBUG("SqliteGetTable error!\n");
        sqlite3_free_table(dbRes);
		return -1;
    }
	
	strncpy(ps8ProtoTypeName, dbRes[s32Col], MAX_NAME_LEN);
	sqlite3_free_table(dbRes);
	
	return 0;
}

S32 DbConfig::GetDeviceAddrCode(S8 *ps8ParentAddrCode, S32 s32ChnNo, S8 *ps8AddrCode, S8 *ps8DevName)
{
	S32  s32Row    = 0;
	S32  s32Col = 0;
	S32  s32Ret    = 0;
    S8 **dbRes     = NULL;
	S8   s8Sql[SQL_LEN] = {0};

	ASSERT(ps8ParentAddrCode);
	ASSERT(ps8AddrCode);
	
	snprintf(s8Sql, SQL_LEN, 
		"select AddrCode, DeviceName from DeviceInfo "
		"where ParentAddrCode='%s' and ChannelNo=%d;", 
		ps8ParentAddrCode, s32ChnNo);

	CONFIG_DEBUG("sql %s\n", s8Sql);
	
    s32Ret = SqliteGetTable(m_pDb, s8Sql, &dbRes, &s32Row, &s32Col);
    printf("nRow: %d, nCol: %d\n", s32Row, s32Col);
    if ((0 != s32Ret) || (1 != s32Row))
    {
		CONFIG_DEBUG("SqliteGetTable error!\n");
        sqlite3_free_table(dbRes);
		return -1;
    }
	
	if (ps8AddrCode)
		strncpy(ps8AddrCode, dbRes[s32Col], MAX_ADDR_LEN);

	if (ps8DevName)
		strncpy(ps8DevName,  dbRes[s32Col+1], MAX_NAME_LEN);
	
	sqlite3_free_table(dbRes);

	return 0;
}

S32 DbConfig::GetDeviceList(string strAddrCode, vector<DEVICE_SET_S> &vecDev)
{
	S32  s32Row    = 0;
	S32  s32Col    = 0;
	S32  s32Ret    = 0;
	S8 **dbRes     = NULL;
	S8   s8Sql[SQL_LEN] = {0};
	
	//AlarmInNum做为启用标记
	snprintf(s8Sql, SQL_LEN, "select * from DeviceInfo "
		"where ParentAddrCode='%s' and DisLevel!=2 and AlarmInNum!=1;", 
		strAddrCode.c_str());

	CONFIG_DEBUG("sql %s\n", s8Sql);
	
	s32Ret = SqliteGetTable(m_pDb, s8Sql, &dbRes, &s32Row, &s32Col);
	printf("nRow: %d, nCol: %d\n", s32Row, s32Col);
	if ((0 != s32Ret) || (0 == s32Row))
	{
		CONFIG_DEBUG("SqliteGetTable error!\n");
		sqlite3_free_table(dbRes);
		return -1;
	}
	
	for (S32 s32I = 0; s32I < s32Row; s32I++)
	{
		DEVICE_SET_S stDevSet;
		memset(&stDevSet, 0, sizeof(stDevSet));

		strncpy(stDevSet.s8AddrCode, dbRes[(s32I+1)*s32Col], MAX_ADDR_LEN);
		strncpy(stDevSet.s8DevName,  dbRes[(s32I+1)*s32Col+1], MAX_NAME_LEN);
		if (dbRes[s32Col+4])
			strncpy(stDevSet.s8PareAddrCode, dbRes[(s32I+1)*s32Col+4], MAX_ADDR_LEN);
		if (dbRes[s32Col+7])
			strncpy(stDevSet.s8DevDesc, dbRes[(s32I+1)*s32Col+7], 100);
		stDevSet.s32ChnNum  = atoi(dbRes[(s32I+1)*s32Col+2]);
		stDevSet.s32ChnNo   = atoi(dbRes[(s32I+1)*s32Col+3]);
		stDevSet.u16DevType = atoi(dbRes[(s32I+1)*s32Col+6]);
		stDevSet.s8ManuType = atoi(dbRes[(s32I+1)*s32Col+8]);

		INET_PTON(dbRes[(s32I+1)*s32Col+9], stDevSet.u8IPAddr);
		stDevSet.u16Port    = atoi(dbRes[(s32I+1)*s32Col+10]);

		vecDev.push_back(stDevSet);
	}

	
	sqlite3_free_table(dbRes);

	return 0;
}

ComConfig::ComConfig()
{

	g_pConfig   = this;
	m_pSysParam = new SYS_PARAM_S;
	g_pSysParam = m_pSysParam;
	memset(m_pSysParam, 0, sizeof(SYS_PARAM_S));

	m_pIniCfg = new IniConfig(m_pSysParam);
	m_pDbCfg  = new DbConfig(m_pSysParam);
}

ComConfig::~ComConfig()
{
	if (m_pSysParam)
	{
		delete m_pSysParam;
		g_pSysParam = NULL;
	}

	if (m_pIniCfg)
	{
		delete m_pIniCfg;
		m_pIniCfg = NULL;
	}

	if (m_pDbCfg)
	{
		delete m_pDbCfg;
		m_pDbCfg = NULL;
	}
}

S32 ComConfig::LoadSettings()
{
	m_pIniCfg->LoadSettings();
	m_pDbCfg->LoadSettings();

	return 0;
}

S32 ComConfig::ReadMisc (MISC_SET_S *pMisc)
{
	m_pIniCfg->ReadMisc(pMisc);

	return 0;
}

S32 ComConfig::WriteMisc(MISC_SET_S *pMisc)
{
	m_pIniCfg->WriteMisc(pMisc);
	
	return 0;
}

S32 ComConfig::ReadSysInfo (SYSINFO_SET_S *pSysInfo)
{
	m_pIniCfg->ReadSysInfo(pSysInfo);
	m_pDbCfg->ReadSysInfo(pSysInfo);

	return 0;
}

S32 ComConfig::WriteSysInfo(SYSINFO_SET_S *pSysInfo)
{
	m_pIniCfg->WriteSysInfo(pSysInfo);
	m_pDbCfg->WriteSysInfo(pSysInfo);
	
	return 0;
}

S32 ComConfig::ReadNetwork (NETWORK_SET_S *pNetwork)
{
	m_pIniCfg->ReadNetwork(pNetwork);
	m_pDbCfg->ReadNetwork(pNetwork);

	return 0;
}

S32 ComConfig::WriteNetwork(NETWORK_SET_S *pNetwork)
{
	m_pIniCfg->WriteNetwork(pNetwork);
	m_pDbCfg->WriteNetwork(pNetwork);
	
	return 0;
}

S32 ComConfig::ReadDevice(DEVICE_SET_S  *pDevice)
{
	return m_pDbCfg->ReadDevice(pDevice);
}

S32 ComConfig::ReadDevice_2(DEVICE_SET_S  *pDevice)
{
	return m_pDbCfg->ReadDevice_2(pDevice);
}

S32 ComConfig::WriteDevice(DEVICE_SET_S *pDevice)
{
	return m_pDbCfg->WriteDevice(pDevice);
}

void ComConfig::BeginComTrans()
{
    m_pDbCfg->BeginComTrans();
}

void ComConfig::EndComTrans()
{
    m_pDbCfg->EndComTrans();
}

S32 ComConfig::DeleteDevice(S8 *ps8AddrCode)
{
	return m_pDbCfg->DeleteDevice(ps8AddrCode);
}

S32 ComConfig::DetachReTrackCarToCamera(S8 * ps8AddrCode)
{
	return m_pDbCfg->DetachReTrackCarToCamera(ps8AddrCode);
}

S32 ComConfig::DeleteRePresetInfo(S8 *ps8AddrCode)
{
	return m_pDbCfg->DeleteRePresetInfo(ps8AddrCode);
}

S32 ComConfig::WriteStation(STATION_SET_S *pStation)
{
	return m_pDbCfg->WriteStation(pStation);
}

S32 ComConfig::DeleteStation(U32 u32StatCode)
{
	return m_pDbCfg->DeleteStation(u32StatCode);
}

S32 ComConfig::WriteUserGroup(U32 u32UserId, U32 u32GroupId, U32 u32GroupType)
{
	return m_pDbCfg->WriteUserGroup(u32UserId, u32GroupId, u32GroupType);
}

S32 ComConfig::DeleteUserGroup(U32 u32UserId, U32 u32GroupId, U32 u32GroupType)
{
	return m_pDbCfg->DeleteUserGroup(u32UserId, u32GroupId, u32GroupType);
}

S32 ComConfig::ReadMaintain (MAINTAIN_SET_S *pMaintian)
{
	return m_pDbCfg->ReadMaintain(pMaintian);
}

S32 ComConfig::WriteMaintain(MAINTAIN_SET_S *pMaintian)
{
	m_pDbCfg->WriteMaintain(pMaintian);
	m_pIniCfg->WriteMaintian(pMaintian);
	return 0;
}

S32 ComConfig::ReadPlatform(PLATFROM_SET_S *pPlatform)
{
	return m_pIniCfg->ReadPlatform(pPlatform);
}

S32 ComConfig::WritePlatform(PLATFROM_SET_S *pPlatform)
{
	m_pIniCfg->WritePlatform(pPlatform);
	return 0;
}

S32 ComConfig::ReadVehicleInfo(VEHICLE_SET_S *pVehicle)
{
	return m_pIniCfg->ReadVehicleInfo(pVehicle);
}

S32 ComConfig::WriteVehicleInfo(VEHICLE_SET_S *pVehicle)
{
	return m_pIniCfg->WriteVehicleInfo(pVehicle);
}

S32 ComConfig::GetProvinceCode(S8 *ps8ProvName)
{
	return m_pDbCfg->GetProvinceCode(ps8ProvName);
}

S32 ComConfig::GetProvinceName(S32 s32ProvCode, S8 *ps8ProvName)
{
	return m_pDbCfg->GetProvinceName(s32ProvCode, ps8ProvName);
}

S32 ComConfig::GetCityCode(S8 *ps8ProvName, S8 *ps8CityName)
{
	return m_pDbCfg->GetCityCode(ps8ProvName, ps8CityName);
}

S32 ComConfig::GetCityName(S32 s32ProvCode, S32 s32CityCode, S8 *ps8CityName)
{
	return m_pDbCfg->GetCityName(s32ProvCode, s32CityCode, ps8CityName);
}

S32 ComConfig::GetObjectCode(S8 *ps8ObjectName)
{
	return m_pDbCfg->GetObjectCode(ps8ObjectName);
}

S32 ComConfig::GetObjectName(S32 s32ObjectCode, S8 *ps8ObjectName)
{
	return m_pDbCfg->GetObjectName(s32ObjectCode, ps8ObjectName);
}

S32 ComConfig::GetStationCode(S32 s32ProvCode, S32 s32CityCode, S8 *ps8StationName)
{
	return m_pDbCfg->GetStationCode(s32ProvCode, s32CityCode, ps8StationName);
}

S32 ComConfig::GetStationName(S32 s32StationCode, S8 *ps8StationName)
{
	return m_pDbCfg->GetStationName(s32StationCode, ps8StationName);
}

S32 ComConfig::GetDevTypeCode(S8 *ps8DevTypeName)
{
	return m_pDbCfg->GetDevTypeCode(ps8DevTypeName);
}

S32 ComConfig::GetDevTypeName(S32 s32DevTypeCode, S8 *ps8DevTypeName)
{
	return m_pDbCfg->GetDevTypeName(s32DevTypeCode, ps8DevTypeName);
}

S32 ComConfig::GetStreamTypeCode(S8 *ps8StreamTypeName)
{
	return m_pDbCfg->GetStreamTypeCode(ps8StreamTypeName);
}

S32 ComConfig::GetStreamTypeName(S32 s32StreamTypeCode, S8 *ps8StreamTypeName)
{
	return m_pDbCfg->GetStreamTypeName(s32StreamTypeCode, ps8StreamTypeName);
}

S32 ComConfig::GetManuTypeCode(S8 *ps8ManuTypeName)
{
	return m_pDbCfg->GetManuTypeCode(ps8ManuTypeName);
}

S32 ComConfig::GetManuTypeName(S32 s32ManuTypeCode, S8 *ps8ManuTypeName)
{
	return m_pDbCfg->GetManuTypeName(s32ManuTypeCode, ps8ManuTypeName);
}

S32 ComConfig::GetProtoTypeCode(S8 *ps8ProtoTypeName)
{
	return m_pDbCfg->GetProtoTypeCode(ps8ProtoTypeName);
}

S32 ComConfig::GetProtoTypeName(S32 s32ProtoTypeCode, S8 *ps8ProtoTypeName)
{
	return m_pDbCfg->GetProtoTypeName(s32ProtoTypeCode, ps8ProtoTypeName);
}

S32 ComConfig::GetDeviceAddrCode(S8 *ps8ParentAddrCode, S32 s32ChnNo, S8 *ps8AddrCode, S8 *ps8DevName)
{
	return m_pDbCfg->GetDeviceAddrCode(ps8ParentAddrCode, s32ChnNo, ps8AddrCode, ps8DevName);
}

S32 ComConfig::GetDeviceList(string strAddrCode, vector<DEVICE_SET_S> &vecDev)
{
	return m_pDbCfg->GetDeviceList(strAddrCode, vecDev);
}

void SetCurrentDirectory()
{
#ifdef _WIN32
	S8 s8Module[MAX_PATH] = {0};
	GetModuleFileName(NULL, s8Module, MAX_PATH); 
	
	string str;
	str.assign(s8Module);

	S32 s32Pos     = str.find_last_of("/\\");
	string strPath = str.substr(0, s32Pos);
	
	SetCurrentDirectory(strPath.c_str());
#endif
}