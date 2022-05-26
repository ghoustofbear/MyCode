#include "easySQLite/sqlite3.h"
#include "update.h"
#include "configc1.h"
#include "log.h"

#define ENABLE_UPDATE_DEBUG
#ifdef  ENABLE_UPDATE_DEBUG 
#ifdef _WIN32
#define UPDATE_DEBUG             printf
#else
#define UPDATE_DEBUG(MSG...)     DEBUG("UPDATE", MSG)
#endif /* end _WIN32 */
#else
#define UPDATE_DEBUG(MSG...)
#endif /* end ENABLE_UPDATE_DEBUG */



#ifndef _WIN32
void CopyFile(const char *src_file, const char *des_file, BOOL bDel)
{
	char cmdline[256];
	sprintf(cmdline, "cp -fr %s %s", src_file, des_file);
	system(cmdline);
}

void DeleteFile(const char *strFile)
{
	remove(strFile);
}

#endif

UpdateDb::UpdateDb()
{
}

UpdateDb::~UpdateDb()
{
}

VOID UpdateDb::Update()
{
	sqlite3 *db    = NULL;
	S32     s32Row = 0;
	S32     s32Col = 0;
	S32     s32Ret = 0;
	S8   ** dbRes  = NULL;
	S8      s8Sql[SQL_LEN] = {0};

	U32 u32Version = 0;

	sqlite3_open(CONFIG_DB, &db);
	snprintf(s8Sql, SQL_LEN, "select DbVersion from SystemInfo;" );
    
	s32Ret = SqliteGetTable(db, s8Sql, &dbRes, &s32Row, &s32Col);
	if (0 != s32Ret || 1 != s32Row)
	{
		return;
	}

	u32Version = atoi(dbRes[1]);
	sqlite3_free_table(dbRes);
	sqlite3_close(db);

	if (u32Version >= DB_VERSION_CURRENT)
	{
		return;
	}

	UPDATE_DEBUG("Database need update..\n");
	g_pLog->SystemLog(Log::SYSLOG_TYPE_SYSTEM, 0 , "数据库升级...");

	/* 备份老的数据库 */
	string strSrc  = "config.db";
	string strDest = "config_old.db";
	CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
	
	switch (u32Version)
	{
	case DB_VERSION_V20140109:
		{
			strSrc  = "config.db";
			strDest = "update.tmp";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "Update\\config_V20140115.db.upd";
			strDest = "config.db";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "update.tmp";
			strDest =  "config.db";
			FromV20140109ToV20140115(strSrc, strDest);
		}
	case DB_VERSION_V20140115:
		{
			strSrc  = "config.db";
			strDest = "update.tmp";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "Update\\config_V20140424.db.upd";
			strDest = "config.db";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "update.tmp";
			strDest =  "config.db";
			FromV20140115ToV20140424(strSrc, strDest);
		}
	case DB_VERSION_V20140424:
		{
			strSrc  = "config.db";
			strDest = "update.tmp";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "Update\\config_V20141113.db.upd";
			strDest = "config.db";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "update.tmp";
			strDest =  "config.db";
			FromV20140424ToV20141113(strSrc, strDest);			
		}
	case DB_VERSION_V20141113:
		{
			strSrc  = "config.db";
			strDest = "update.tmp";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "Update\\config_V20150115.db.upd";
			strDest = "config.db";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "update.tmp";
			strDest =  "config.db";
			FromV20141113ToV20150115(strSrc, strDest);			
		}
	case DB_VERSION_V20150115:
		{
           	strSrc  = "config.db";
			strDest = "update.tmp";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "Update\\config_V20150618.db.upd";
			strDest = "config.db";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "update.tmp";
			strDest =  "config.db";
			FromV20150115ToV20150618(strSrc, strDest);	
		}
	case DB_VERSION_V20150618:
		{
			strSrc  = "config.db";
			strDest = "update.tmp";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "Update\\config_V20150826.db.upd";
			strDest = "config.db";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "update.tmp";
			strDest =  "config.db";
			FromV20150618ToV20150826(strSrc, strDest);
		}
	case DB_VERSION_V20150826:
		{
            strSrc  = "config.db";
			strDest = "update.tmp";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "Update\\config_V20160519.db.upd";
			strDest = "config.db";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "update.tmp";
			strDest =  "config.db";
			FromV20150826ToV20160519(strSrc, strDest);
		}
	case DB_VERSION_V20160519:
		{
            strSrc  = "config.db";
			strDest = "update.tmp";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "Update\\config_V20160711.db.upd";
			strDest = "config.db";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "update.tmp";
			strDest =  "config.db";
			FromV20160519ToV20160711(strSrc, strDest);
		}
	case DB_VERSION_V20160711:
		{
            strSrc  = "config.db";
			strDest = "update.tmp";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "Update\\config_V20161109.db.upd";
			strDest = "config.db";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "update.tmp";
			strDest =  "config.db";
			FromV20160711ToV20161109(strSrc, strDest);
		}
	case DB_VERSION_V20161109:
		{
            strSrc  = "config.db";
			strDest = "update.tmp";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "Update\\config_V20171012.db.upd";
			strDest = "config.db";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "update.tmp";
			strDest =  "config.db";
			FromV20161109ToV20171012(strSrc, strDest);
		}
	case DB_VERSION_V20171012:
		{
            strSrc  = "config.db";
			strDest = "update.tmp";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "Update\\config_V20171115.db.upd";
			strDest = "config.db";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "update.tmp";
			strDest =  "config.db";			
			FromV20171012ToV20171115(strSrc, strDest);			
		}
	case DB_VERSION_V20171115:
		{
		    strSrc  = "config.db";
			strDest = "update.tmp";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "Update\\config_V20180223.db.upd";
			strDest = "config.db";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "update.tmp";
			strDest =  "config.db";			
			FromV20171115ToV20180223(strSrc, strDest);
		}
	case DB_VERSION_V20180223:
		{
			strSrc  = "config.db";
			strDest = "update.tmp";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "Update\\config_V20180625.db.upd";
			strDest = "config.db";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "update.tmp";
			strDest =  "config.db";			
			FromV20180223ToV20180625(strSrc, strDest);
		}
	case DB_VERSION_V20180625:
		{
			strSrc  = "config.db";
			strDest = "update.tmp";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "Update\\config_V20180625.db.upd";
			strDest = "config.db";
			CopyFile(strSrc.c_str(), strDest.c_str(), FALSE);   
			
			strSrc  = "update.tmp";
			strDest =  "config.db";			
			FromV20180625ToV20181022(strSrc, strDest);
		}
	}
	g_pLog->SystemLog(Log::SYSLOG_TYPE_SYSTEM, 0 , "数据库升级成功");
	
	DeleteFile("update.tmp");
}

S32 UpdateDb::ImportDb(string strSrcDb, string strDesDb)
{
	return 0;
}

S32 UpdateDb::SystemInfoTableCopy(sqlite3 *srcDb, sqlite3 *destDb)
{
	S32     s32Row = 0;
	S32     s32Col = 0;
	S32     s32Ret = 0;
	S8   ** dbRes  = NULL;
	S8      s8Sql[SQL_LEN] = {0};

	string strSql = 
		"select AddrCode, DeviceName, SwVersion, HwVersion, IEVersion, "
		"AuthIpAddr, RtpBasePort, SipPort, RtspPort, PrivatePort, WebPort, "
		"HttpPort, RecvIntf, SendIntf, AutoStart, UpdateSec "
		"from SystemInfo;";

	s32Ret = SqliteGetTable(srcDb, (char *)strSql.c_str(), &dbRes, &s32Row, &s32Col);
	if (0 != s32Ret || 1 != s32Row)
	{
		return -1;
	}

	snprintf(s8Sql, SQL_LEN,
			"update SystemInfo set AddrCode='%s', DeviceName='%s', SwVersion='%s', "
			"HwVersion='%s', IEVersion='%s', AuthIpAddr='%s', RtpBasePort=%d, "
			"SipPort=%d, RtspPort=%d, PrivatePort=%d, WebPort=%d, HttpPort=%d, RecvIntf=%d, "
			"SendIntf=%d, AutoStart=%d, UpdateSec=%d;", 
			dbRes[s32Col],
			dbRes[s32Col+1],
			SOFT_VERSION,
			HARD_VERSION,
			IE_VERSION,
			dbRes[s32Col+5],
			atoi(dbRes[s32Col+6]),
			atoi(dbRes[s32Col+7]),
			atoi(dbRes[s32Col+8]),
			atoi(dbRes[s32Col+9]),
			atoi(dbRes[s32Col+10]),
			atoi(dbRes[s32Col+11]),
			atoi(dbRes[s32Col+12]),
			atoi(dbRes[s32Col+13]),
			atoi(dbRes[s32Col+14]),
			atoi(dbRes[s32Col+15])
			);

	SqliteExec(destDb, s8Sql);

	sqlite3_free_table(dbRes);

	return 0;
}

S32 UpdateDb::HelpSameTableCopy(
				sqlite3 * srcDb, 
				sqlite3 * destDb, 
				string    srcTable, 
				string    destTable)
{
	string  strSql;
	sqlite3_stmt* srcStmt  = NULL;

	strSql = "select * from " + srcTable;

	if (sqlite3_prepare_v2(srcDb, strSql.c_str(), strSql.length(), &srcStmt, NULL) != SQLITE_OK) 
	{	
	    sqlite3_finalize(srcStmt);
		return -1;
	}

	int fieldCount = sqlite3_column_count(srcStmt);
	do 
	{
		string strFieldList;
		string strValueList;
		S8     s8Temp[100] = {0};

		int r = sqlite3_step(srcStmt);
		if (r == SQLITE_ROW) 
		{
			for (int i = 0; i < fieldCount; ++i) 
			{
				if(i > 0)
				{
					strFieldList += ",";
					strValueList += ",";
				}
				
				strFieldList += sqlite3_column_name(srcStmt, i);

				/*
				 *  这里需要先判断当前记录当前字段的类型，再根据返回的类型 
				 *  使用不同的API函数获取实际的数据值
				 */
				int vtype = sqlite3_column_type(srcStmt,i);
				if (vtype == SQLITE_INTEGER) 
				{
					int v = sqlite3_column_int(srcStmt,i);
					memset(s8Temp, 0, 100);

					snprintf(s8Temp, 100, "%d", v);
					strValueList += s8Temp;
				} 
				else if (vtype == SQLITE_FLOAT) 
				{
					double v = sqlite3_column_double(srcStmt,i);

					memset(s8Temp, 0, 100);
					snprintf(s8Temp, 100, "%f", v);
					strValueList += s8Temp;
				} 
				else if (vtype == SQLITE_TEXT) 
				{
					const char* v = (const char*)sqlite3_column_text(srcStmt, i);
					strValueList = strValueList + "'" + v + "'";
				} 
				else if (vtype == SQLITE_NULL) 
				{
					/*某数据表后添加新字段,则升级时旧数据表无数据insert该字段无数据失败(2015/08/29)*/
					strValueList = strValueList + "'" + "" + "'";          
					UPDATE_DEBUG("The value is NULL.\n");
				}
			}

			string strDestSql = "insert into " + destTable + \
								 " (" + strFieldList + ") " + "values " + \
								 " (" + strValueList + ")";

			SqliteExec(destDb, (char *)strDestSql.c_str());
		} 
		else if (r == SQLITE_DONE) 
		{
			UPDATE_DEBUG("Select Finished.\n");
			break;
		} 
		else 
		{
			UPDATE_DEBUG("Failed to SELECT.\n");
			sqlite3_finalize(srcStmt);
			sqlite3_close(srcDb);
			return -1;
		}
	} while (true);
	sqlite3_finalize(srcStmt);

	return 0;
}

S32 UpdateDb::FromV20140109ToV20140115(string strSrc, string strDest)
{
	sqlite3 *srcDb  = NULL;
	sqlite3 *destDb = NULL;
	
	sqlite3_open(strSrc.c_str(),  &srcDb);
	sqlite3_open(strDest.c_str(), &destDb);
	
	if (SystemInfoTableCopy(srcDb, destDb) < 0)
	{
		return -1;
	}
	
	HelpSameTableCopy(srcDb, destDb, "DeviceInfo", "DeviceInfo");
	HelpSameTableCopy(srcDb, destDb, "Station", "Station");
	HelpSameTableCopy(srcDb, destDb, "User", "User");
	HelpSameTableCopy(srcDb, destDb, "RightGroup", "RightGroup");
	HelpSameTableCopy(srcDb, destDb, "UserGroup", "UserGroup");
	HelpSameTableCopy(srcDb, destDb, "ManuType", "ManuType");
	HelpSameTableCopy(srcDb, destDb, "ProtocolType", "ProtocolType");
	HelpSameTableCopy(srcDb, destDb, "Maintain", "Maintain");

// 	HelpSameTableCopy(srcDb, destDb, "AlarmLog", "AlarmLog");
// 	HelpSameTableCopy(srcDb, destDb, "DeviceLog", "DeviceLog");
// 	HelpSameTableCopy(srcDb, destDb, "SystemLog", "SystemLog");
// 	HelpSameTableCopy(srcDb, destDb, "UserLog", "UserLog");
	
	sqlite3_close(srcDb);
	sqlite3_close(destDb);
	
	return 0;
}

S32 UpdateDb::FromV20140115ToV20140424(string strSrc, string strDest)
{
	sqlite3 *srcDb  = NULL;
	sqlite3 *destDb = NULL;
	
	sqlite3_open(strSrc.c_str(),  &srcDb);
	sqlite3_open(strDest.c_str(), &destDb);
	
	if (SystemInfoTableCopy(srcDb, destDb) < 0)
	{
		return -1;
	}
	
	HelpSameTableCopy(srcDb, destDb, "DeviceInfo", "DeviceInfo");
	HelpSameTableCopy(srcDb, destDb, "Station", "Station");
	HelpSameTableCopy(srcDb, destDb, "User", "User");
	HelpSameTableCopy(srcDb, destDb, "RightGroup", "RightGroup");
	HelpSameTableCopy(srcDb, destDb, "UserGroup", "UserGroup");
	HelpSameTableCopy(srcDb, destDb, "ManuType", "ManuType");
	HelpSameTableCopy(srcDb, destDb, "ProtocolType", "ProtocolType");
	
// 	HelpSameTableCopy(srcDb, destDb, "AlarmLog", "AlarmLog");
// 	HelpSameTableCopy(srcDb, destDb, "DeviceLog", "DeviceLog");
// 	HelpSameTableCopy(srcDb, destDb, "SystemLog", "SystemLog");
// 	HelpSameTableCopy(srcDb, destDb, "UserLog", "UserLog");
	
	sqlite3_close(srcDb);
	sqlite3_close(destDb);
	
	return 0;
}

S32 UpdateDb::FromV20140424ToV20141113(string strSrc, string strDest)
{
	sqlite3 *srcDb  = NULL;
	sqlite3 *destDb = NULL;
	
	sqlite3_open(strSrc.c_str(),  &srcDb);
	sqlite3_open(strDest.c_str(), &destDb);
	
	if (SystemInfoTableCopy(srcDb, destDb) < 0)
	{
		return -1;
	}
	
	HelpSameTableCopy(srcDb, destDb, "DeviceInfo", "DeviceInfo");
	HelpSameTableCopy(srcDb, destDb, "Station", "Station");
	HelpSameTableCopy(srcDb, destDb, "User", "User");
	HelpSameTableCopy(srcDb, destDb, "RightGroup", "RightGroup");
	HelpSameTableCopy(srcDb, destDb, "UserGroup", "UserGroup");
	HelpSameTableCopy(srcDb, destDb, "ManuType", "ManuType");
	HelpSameTableCopy(srcDb, destDb, "ProtocolType", "ProtocolType");
	HelpSameTableCopy(srcDb, destDb, "Organization", "Organization");
	HelpSameTableCopy(srcDb, destDb, "OrganDevice", "OrganDevice");
	HelpSameTableCopy(srcDb, destDb, "CruiseLine", "CruiseLine");
	HelpSameTableCopy(srcDb, destDb, "CruiseTime", "CruiseTime");
	HelpSameTableCopy(srcDb, destDb, "CruiseStep", "CruiseStep");
	
	// 	HelpSameTableCopy(srcDb, destDb, "AlarmLog", "AlarmLog");
	// 	HelpSameTableCopy(srcDb, destDb, "DeviceLog", "DeviceLog");
	// 	HelpSameTableCopy(srcDb, destDb, "SystemLog", "SystemLog");
	// 	HelpSameTableCopy(srcDb, destDb, "UserLog", "UserLog");
	
	sqlite3_close(srcDb);
	sqlite3_close(destDb);
	
	return 0;
}

S32 UpdateDb::FromV20141113ToV20150115(string strSrc, string strDest)
{
	sqlite3 *srcDb  = NULL;
	sqlite3 *destDb = NULL;
	
	sqlite3_open(strSrc.c_str(),  &srcDb);
	sqlite3_open(strDest.c_str(), &destDb);
	
	if (SystemInfoTableCopy(srcDb, destDb) < 0)
	{
		return -1;
	}
	
	HelpSameTableCopy(srcDb, destDb, "DeviceInfo", "DeviceInfo");
	HelpSameTableCopy(srcDb, destDb, "Station", "Station");
	HelpSameTableCopy(srcDb, destDb, "User", "User");
	HelpSameTableCopy(srcDb, destDb, "RightGroup", "RightGroup");
	HelpSameTableCopy(srcDb, destDb, "UserGroup", "UserGroup");
	HelpSameTableCopy(srcDb, destDb, "ManuType", "ManuType");
	HelpSameTableCopy(srcDb, destDb, "ProtocolType", "ProtocolType");
	HelpSameTableCopy(srcDb, destDb, "Organization", "Organization");
	HelpSameTableCopy(srcDb, destDb, "OrganDevice", "OrganDevice");
	HelpSameTableCopy(srcDb, destDb, "CruiseLine", "CruiseLine");
	HelpSameTableCopy(srcDb, destDb, "CruiseTime", "CruiseTime");
	HelpSameTableCopy(srcDb, destDb, "CruiseStep", "CruiseStep");
	
	// 	HelpSameTableCopy(srcDb, destDb, "AlarmLog", "AlarmLog");
	// 	HelpSameTableCopy(srcDb, destDb, "DeviceLog", "DeviceLog");
	// 	HelpSameTableCopy(srcDb, destDb, "SystemLog", "SystemLog");
	// 	HelpSameTableCopy(srcDb, destDb, "UserLog", "UserLog");
	
	sqlite3_close(srcDb);
	sqlite3_close(destDb);
	
	return 0;
}

S32 UpdateDb::FromV20150115ToV20150618(string strSrc, string strDest)
{
    sqlite3 *srcDb  = NULL;
	sqlite3 *destDb = NULL;
	
	sqlite3_open(strSrc.c_str(),  &srcDb);
	sqlite3_open(strDest.c_str(), &destDb);
	
	if (SystemInfoTableCopy(srcDb, destDb) < 0)
	{
		return -1;
	}
	
	sqlite3_exec(destDb,"BEGIN TRANSACTION",0,0,0);

	HelpSameTableCopy(srcDb, destDb, "DeviceInfo", "DeviceInfo");
	HelpSameTableCopy(srcDb, destDb, "Station", "Station");
	HelpSameTableCopy(srcDb, destDb, "User", "User");
	HelpSameTableCopy(srcDb, destDb, "RightGroup", "RightGroup");
	HelpSameTableCopy(srcDb, destDb, "UserGroup", "UserGroup");
	HelpSameTableCopy(srcDb, destDb, "ManuType", "ManuType");
	HelpSameTableCopy(srcDb, destDb, "ProtocolType", "ProtocolType");
	HelpSameTableCopy(srcDb, destDb, "Organization", "Organization");
	HelpSameTableCopy(srcDb, destDb, "OrganDevice", "OrganDevice");
	HelpSameTableCopy(srcDb, destDb, "CruiseLine", "CruiseLine");
	HelpSameTableCopy(srcDb, destDb, "CruiseTime", "CruiseTime");
	HelpSameTableCopy(srcDb, destDb, "CruiseStep", "CruiseStep");
	HelpSameTableCopy(srcDb, destDb, "PresetInfo", "PresetInfo");
	HelpSameTableCopy(srcDb, destDb, "AIRelation", "AIRelation");
	
	// 	HelpSameTableCopy(srcDb, destDb, "AlarmLog", "AlarmLog");
	// 	HelpSameTableCopy(srcDb, destDb, "DeviceLog", "DeviceLog");
	// 	HelpSameTableCopy(srcDb, destDb, "SystemLog", "SystemLog");
	// 	HelpSameTableCopy(srcDb, destDb, "UserLog", "UserLog");
	
	sqlite3_exec(destDb, "COMMIT", 0, 0, 0);
	
	sqlite3_close(srcDb);
	sqlite3_close(destDb);
	
	return 0;
}

S32 UpdateDb::FromV20150618ToV20150826(string strSrc, string strDest)
{
    sqlite3 *srcDb  = NULL;
	sqlite3 *destDb = NULL;
	
	sqlite3_open(strSrc.c_str(),  &srcDb);
	sqlite3_open(strDest.c_str(), &destDb);
	
	if (SystemInfoTableCopy(srcDb, destDb) < 0)
	{
		return -1;
	}
	
	sqlite3_exec(destDb,"BEGIN TRANSACTION",0,0,0);

	HelpSameTableCopy(srcDb, destDb, "DeviceInfo", "DeviceInfo");
	HelpSameTableCopy(srcDb, destDb, "Station", "Station");
	HelpSameTableCopy(srcDb, destDb, "User", "User");
	HelpSameTableCopy(srcDb, destDb, "RightGroup", "RightGroup");
	HelpSameTableCopy(srcDb, destDb, "UserGroup", "UserGroup");
	HelpSameTableCopy(srcDb, destDb, "ManuType", "ManuType");
	HelpSameTableCopy(srcDb, destDb, "ProtocolType", "ProtocolType");
	HelpSameTableCopy(srcDb, destDb, "Organization", "Organization");
	HelpSameTableCopy(srcDb, destDb, "OrganDevice", "OrganDevice");
	HelpSameTableCopy(srcDb, destDb, "CruiseLine", "CruiseLine");
	HelpSameTableCopy(srcDb, destDb, "CruiseTime", "CruiseTime");
	HelpSameTableCopy(srcDb, destDb, "CruiseStep", "CruiseStep");
	HelpSameTableCopy(srcDb, destDb, "PresetInfo", "PresetInfo");
	HelpSameTableCopy(srcDb, destDb, "AIRelation", "AIRelation");
	
	// 	HelpSameTableCopy(srcDb, destDb, "AlarmLog", "AlarmLog");
	// 	HelpSameTableCopy(srcDb, destDb, "DeviceLog", "DeviceLog");
	// 	HelpSameTableCopy(srcDb, destDb, "SystemLog", "SystemLog");
	// 	HelpSameTableCopy(srcDb, destDb, "UserLog", "UserLog");
	
	sqlite3_exec(destDb, "COMMIT", 0, 0, 0);

	sqlite3_close(srcDb);
	sqlite3_close(destDb);
	
	return 0;
}

S32 UpdateDb::FromV20150826ToV20160519(string strSrc, string strDest)
{
    sqlite3 *srcDb  = NULL;
	sqlite3 *destDb = NULL;
	
	sqlite3_open(strSrc.c_str(),  &srcDb);
	sqlite3_open(strDest.c_str(), &destDb);
	
	if (SystemInfoTableCopy(srcDb, destDb) < 0)
	{
		return -1;
	}
	
	sqlite3_exec(destDb,"BEGIN TRANSACTION",0,0,0);

	HelpSameTableCopy(srcDb, destDb, "DeviceInfo", "DeviceInfo");
	HelpSameTableCopy(srcDb, destDb, "Station", "Station");
	HelpSameTableCopy(srcDb, destDb, "User", "User");
	HelpSameTableCopy(srcDb, destDb, "RightGroup", "RightGroup");
	HelpSameTableCopy(srcDb, destDb, "UserGroup", "UserGroup");
	HelpSameTableCopy(srcDb, destDb, "ManuType", "ManuType");
	HelpSameTableCopy(srcDb, destDb, "ProtocolType", "ProtocolType");
	HelpSameTableCopy(srcDb, destDb, "Organization", "Organization");
	HelpSameTableCopy(srcDb, destDb, "OrganDevice", "OrganDevice");
	HelpSameTableCopy(srcDb, destDb, "CruiseLine", "CruiseLine");
	HelpSameTableCopy(srcDb, destDb, "CruiseTime", "CruiseTime");
	HelpSameTableCopy(srcDb, destDb, "CruiseStep", "CruiseStep");
	HelpSameTableCopy(srcDb, destDb, "PresetInfo", "PresetInfo");
	HelpSameTableCopy(srcDb, destDb, "AIRelation", "AIRelation");
	HelpSameTableCopy(srcDb, destDb, "AreaType", "AreaType");
	HelpSameTableCopy(srcDb, destDb, "PresetType", "PresetType");
	
	// 	HelpSameTableCopy(srcDb, destDb, "AlarmLog", "AlarmLog");
	// 	HelpSameTableCopy(srcDb, destDb, "DeviceLog", "DeviceLog");
	// 	HelpSameTableCopy(srcDb, destDb, "SystemLog", "SystemLog");
	// 	HelpSameTableCopy(srcDb, destDb, "UserLog", "UserLog");
	
	sqlite3_exec(destDb, "COMMIT", 0, 0, 0);

	sqlite3_close(srcDb);
	sqlite3_close(destDb);
	
	return 0;
}

S32 UpdateDb::FromV20160519ToV20160711(string strSrc, string strDest)
{
    sqlite3 *srcDb  = NULL;
	sqlite3 *destDb = NULL;
	
	sqlite3_open(strSrc.c_str(),  &srcDb);
	sqlite3_open(strDest.c_str(), &destDb);
	
	if (SystemInfoTableCopy(srcDb, destDb) < 0)
	{
		return -1;
	}
	
	sqlite3_exec(destDb,"BEGIN TRANSACTION",0,0,0);

	HelpSameTableCopy(srcDb, destDb, "DeviceInfo", "DeviceInfo");
	HelpSameTableCopy(srcDb, destDb, "Station", "Station");
	HelpSameTableCopy(srcDb, destDb, "User", "User");
	HelpSameTableCopy(srcDb, destDb, "RightGroup", "RightGroup");
	HelpSameTableCopy(srcDb, destDb, "UserGroup", "UserGroup");
	HelpSameTableCopy(srcDb, destDb, "ManuType", "ManuType");
	HelpSameTableCopy(srcDb, destDb, "ProtocolType", "ProtocolType");
	HelpSameTableCopy(srcDb, destDb, "Organization", "Organization");
	HelpSameTableCopy(srcDb, destDb, "OrganDevice", "OrganDevice");
	HelpSameTableCopy(srcDb, destDb, "CruiseLine", "CruiseLine");
	HelpSameTableCopy(srcDb, destDb, "CruiseTime", "CruiseTime");
	HelpSameTableCopy(srcDb, destDb, "CruiseStep", "CruiseStep");
	HelpSameTableCopy(srcDb, destDb, "PresetInfo", "PresetInfo");
	HelpSameTableCopy(srcDb, destDb, "AIRelation", "AIRelation");
	HelpSameTableCopy(srcDb, destDb, "AreaType", "AreaType");
	HelpSameTableCopy(srcDb, destDb, "PresetType", "PresetType");
	
	// 	HelpSameTableCopy(srcDb, destDb, "AlarmLog", "AlarmLog");
	// 	HelpSameTableCopy(srcDb, destDb, "DeviceLog", "DeviceLog");
	// 	HelpSameTableCopy(srcDb, destDb, "SystemLog", "SystemLog");
	// 	HelpSameTableCopy(srcDb, destDb, "UserLog", "UserLog");
	
	sqlite3_exec(destDb, "COMMIT", 0, 0, 0);

	sqlite3_close(srcDb);
	sqlite3_close(destDb);
	
	return 0;
}

S32 UpdateDb::FromV20160711ToV20161109(string strSrc, string strDest)
{
    sqlite3 *srcDb  = NULL;
	sqlite3 *destDb = NULL;
	
	sqlite3_open(strSrc.c_str(),  &srcDb);
	sqlite3_open(strDest.c_str(), &destDb);
	
	if (SystemInfoTableCopy(srcDb, destDb) < 0)
	{
		return -1;
	}
	
	sqlite3_exec(destDb,"BEGIN TRANSACTION",0,0,0);

	HelpSameTableCopy(srcDb, destDb, "DeviceInfo", "DeviceInfo");
	HelpSameTableCopy(srcDb, destDb, "Station", "Station");
	HelpSameTableCopy(srcDb, destDb, "User", "User");
	HelpSameTableCopy(srcDb, destDb, "RightGroup", "RightGroup");
	HelpSameTableCopy(srcDb, destDb, "UserGroup", "UserGroup");
	HelpSameTableCopy(srcDb, destDb, "ManuType", "ManuType");
	HelpSameTableCopy(srcDb, destDb, "ProtocolType", "ProtocolType");
	HelpSameTableCopy(srcDb, destDb, "Organization", "Organization");
	HelpSameTableCopy(srcDb, destDb, "OrganDevice", "OrganDevice");
	HelpSameTableCopy(srcDb, destDb, "PresetInfo", "PresetInfo");
	HelpSameTableCopy(srcDb, destDb, "AIRelation", "AIRelation");
	HelpSameTableCopy(srcDb, destDb, "AreaType", "AreaType");
	
	HelpSameTableCopy(srcDb, destDb, "DYDJType", "DYDJType");
	HelpSameTableCopy(srcDb, destDb, "FirePrevType", "FirePrevType");
	HelpSameTableCopy(srcDb, destDb, "AxfResource", "AxfResource");
	HelpSameTableCopy(srcDb, destDb, "ESTDevType", "ESTDevType");
	HelpSameTableCopy(srcDb, destDb, "AreaCode", "AreaCode");
	HelpSameTableCopy(srcDb, destDb, "EstResource", "EstResource");
	HelpSameTableCopy(srcDb, destDb, "ExtRegResource", "ExtRegResource");
	HelpSameTableCopy(srcDb, destDb, "CruiseResource", "CruiseResource");
	
	// 	HelpSameTableCopy(srcDb, destDb, "AlarmLog", "AlarmLog");
	// 	HelpSameTableCopy(srcDb, destDb, "DeviceLog", "DeviceLog");
	// 	HelpSameTableCopy(srcDb, destDb, "SystemLog", "SystemLog");
	// 	HelpSameTableCopy(srcDb, destDb, "UserLog", "UserLog");
	
	sqlite3_exec(destDb, "COMMIT", 0, 0, 0);

	sqlite3_close(srcDb);
	sqlite3_close(destDb);
	
	return 0;
}

S32 UpdateDb::FromV20161109ToV20171012(string strSrc, string strDest)
{
    sqlite3 *srcDb  = NULL;
	sqlite3 *destDb = NULL;
	
	sqlite3_open(strSrc.c_str(),  &srcDb);
	sqlite3_open(strDest.c_str(), &destDb);
	
	if (SystemInfoTableCopy(srcDb, destDb) < 0)
	{
		return -1;
	}
	
	sqlite3_exec(destDb,"BEGIN TRANSACTION",0,0,0);

	HelpSameTableCopy(srcDb, destDb, "DeviceInfo", "DeviceInfo");
	HelpSameTableCopy(srcDb, destDb, "Station", "Station");
	HelpSameTableCopy(srcDb, destDb, "User", "User");
	HelpSameTableCopy(srcDb, destDb, "RightGroup", "RightGroup");
	HelpSameTableCopy(srcDb, destDb, "UserGroup", "UserGroup");
	HelpSameTableCopy(srcDb, destDb, "ManuType", "ManuType");
	HelpSameTableCopy(srcDb, destDb, "ProtocolType", "ProtocolType");
	HelpSameTableCopy(srcDb, destDb, "Organization", "Organization");
	HelpSameTableCopy(srcDb, destDb, "OrganDevice", "OrganDevice");
	HelpSameTableCopy(srcDb, destDb, "PresetInfo", "PresetInfo");
	HelpSameTableCopy(srcDb, destDb, "AIRelation", "AIRelation");
	HelpSameTableCopy(srcDb, destDb, "AreaType", "AreaType");	
	HelpSameTableCopy(srcDb, destDb, "DYDJType", "DYDJType");
	HelpSameTableCopy(srcDb, destDb, "FirePrevType", "FirePrevType");
	HelpSameTableCopy(srcDb, destDb, "AxfResource", "AxfResource");
	HelpSameTableCopy(srcDb, destDb, "AreaCode", "AreaCode");
	HelpSameTableCopy(srcDb, destDb, "EstResource", "EstResource");
	HelpSameTableCopy(srcDb, destDb, "ExtRegResource", "ExtRegResource");
	HelpSameTableCopy(srcDb, destDb, "CruiseResource", "CruiseResource");
	HelpSameTableCopy(srcDb, destDb, "RecordRelation", "RecordRelation");
	
	// 	HelpSameTableCopy(srcDb, destDb, "AlarmLog", "AlarmLog");
	// 	HelpSameTableCopy(srcDb, destDb, "DeviceLog", "DeviceLog");
	// 	HelpSameTableCopy(srcDb, destDb, "SystemLog", "SystemLog");
	// 	HelpSameTableCopy(srcDb, destDb, "UserLog", "UserLog");

	sqlite3_exec(destDb, "COMMIT", 0, 0, 0);
	
	sqlite3_close(srcDb);
	sqlite3_close(destDb);
	
	return 0;
}

S32 UpdateDb::FromV20171012ToV20171115(string strSrc, string strDest)
{
    sqlite3 *srcDb  = NULL;
	sqlite3 *destDb = NULL;
	
	sqlite3_open(strSrc.c_str(),  &srcDb);
	sqlite3_open(strDest.c_str(), &destDb);
	
	if (SystemInfoTableCopy(srcDb, destDb) < 0)
	{
		return -1;
	}
	
	sqlite3_exec(destDb,"BEGIN TRANSACTION",0,0,0);

	HelpSameTableCopy(srcDb, destDb, "DeviceInfo", "DeviceInfo");
	HelpSameTableCopy(srcDb, destDb, "Station", "Station");
	HelpSameTableCopy(srcDb, destDb, "User", "User");
	HelpSameTableCopy(srcDb, destDb, "RightGroup", "RightGroup");
	HelpSameTableCopy(srcDb, destDb, "UserGroup", "UserGroup");
	HelpSameTableCopy(srcDb, destDb, "ManuType", "ManuType");
	HelpSameTableCopy(srcDb, destDb, "ProtocolType", "ProtocolType");
	HelpSameTableCopy(srcDb, destDb, "Organization", "Organization");
	HelpSameTableCopy(srcDb, destDb, "OrganDevice", "OrganDevice");
	HelpSameTableCopy(srcDb, destDb, "PresetInfo", "PresetInfo");
	HelpSameTableCopy(srcDb, destDb, "AIRelation", "AIRelation");
	HelpSameTableCopy(srcDb, destDb, "AreaType", "AreaType");
	HelpSameTableCopy(srcDb, destDb, "DYDJType", "DYDJType");
	HelpSameTableCopy(srcDb, destDb, "FirePrevType", "FirePrevType");
	HelpSameTableCopy(srcDb, destDb, "AxfResource", "AxfResource");
	HelpSameTableCopy(srcDb, destDb, "AreaCode", "AreaCode");
	HelpSameTableCopy(srcDb, destDb, "EstResource", "EstResource");
	HelpSameTableCopy(srcDb, destDb, "ExtRegResource", "ExtRegResource");
	HelpSameTableCopy(srcDb, destDb, "CruiseResource", "CruiseResource");
	HelpSameTableCopy(srcDb, destDb, "RecordRelation", "RecordRelation");
	HelpSameTableCopy(srcDb, destDb, "videolink", "videolink");
	HelpSameTableCopy(srcDb, destDb, "mtaccount", "mtaccount");
	
	// 	HelpSameTableCopy(srcDb, destDb, "AlarmLog", "AlarmLog");
	// 	HelpSameTableCopy(srcDb, destDb, "DeviceLog", "DeviceLog");
	// 	HelpSameTableCopy(srcDb, destDb, "SystemLog", "SystemLog");
	// 	HelpSameTableCopy(srcDb, destDb, "UserLog", "UserLog");
	
	sqlite3_exec(destDb, "COMMIT", 0, 0, 0);

	sqlite3_close(srcDb);
	sqlite3_close(destDb);
	
	return 0;
}

S32 UpdateDb::FromV20171115ToV20180223(string strSrc, string strDest)
{
    sqlite3 *srcDb  = NULL;
	sqlite3 *destDb = NULL;
	
	sqlite3_open(strSrc.c_str(),  &srcDb);
	sqlite3_open(strDest.c_str(), &destDb);
	
	if (SystemInfoTableCopy(srcDb, destDb) < 0)
	{
		return -1;
	}
	
	sqlite3_exec(destDb,"BEGIN TRANSACTION",0,0,0);

	HelpSameTableCopy(srcDb, destDb, "DeviceInfo", "DeviceInfo");
	HelpSameTableCopy(srcDb, destDb, "Station", "Station");
	HelpSameTableCopy(srcDb, destDb, "User", "User");
	HelpSameTableCopy(srcDb, destDb, "RightGroup", "RightGroup");
	HelpSameTableCopy(srcDb, destDb, "UserGroup", "UserGroup");
	HelpSameTableCopy(srcDb, destDb, "ManuType", "ManuType");
	HelpSameTableCopy(srcDb, destDb, "ProtocolType", "ProtocolType");
	HelpSameTableCopy(srcDb, destDb, "Organization", "Organization");
	HelpSameTableCopy(srcDb, destDb, "OrganDevice", "OrganDevice");
	HelpSameTableCopy(srcDb, destDb, "PresetInfo", "PresetInfo");
	HelpSameTableCopy(srcDb, destDb, "AIRelation", "AIRelation");
	HelpSameTableCopy(srcDb, destDb, "AreaType", "AreaType");
	HelpSameTableCopy(srcDb, destDb, "DYDJType", "DYDJType");
	HelpSameTableCopy(srcDb, destDb, "FirePrevType", "FirePrevType");
	HelpSameTableCopy(srcDb, destDb, "AxfResource", "AxfResource");
	HelpSameTableCopy(srcDb, destDb, "AreaCode", "AreaCode");
	HelpSameTableCopy(srcDb, destDb, "EstResource", "EstResource");
	HelpSameTableCopy(srcDb, destDb, "ExtRegResource", "ExtRegResource");
	HelpSameTableCopy(srcDb, destDb, "CruiseResource", "CruiseResource");
	HelpSameTableCopy(srcDb, destDb, "RecordRelation", "RecordRelation");
	HelpSameTableCopy(srcDb, destDb, "videolink", "videolink");
	HelpSameTableCopy(srcDb, destDb, "mtaccount", "mtaccount");
	
	// 	HelpSameTableCopy(srcDb, destDb, "AlarmLog", "AlarmLog");
	// 	HelpSameTableCopy(srcDb, destDb, "DeviceLog", "DeviceLog");
	// 	HelpSameTableCopy(srcDb, destDb, "SystemLog", "SystemLog");
	// 	HelpSameTableCopy(srcDb, destDb, "UserLog", "UserLog");
	
	sqlite3_exec(destDb, "COMMIT", 0, 0, 0);

	sqlite3_close(srcDb);
	sqlite3_close(destDb);
	
	return 0;
}

S32 UpdateDb::FromV20180223ToV20180625(string strSrc, string strDest)
{
	sqlite3 *srcDb  = NULL;
	sqlite3 *destDb = NULL;
	
	sqlite3_open(strSrc.c_str(),  &srcDb);
	sqlite3_open(strDest.c_str(), &destDb);
	
	if (SystemInfoTableCopy(srcDb, destDb) < 0)
	{
		return -1;
	}
	
	sqlite3_exec(destDb,"BEGIN TRANSACTION",0,0,0);
	
	HelpSameTableCopy(srcDb, destDb, "DeviceInfo", "DeviceInfo");
	HelpSameTableCopy(srcDb, destDb, "Station", "Station");
	HelpSameTableCopy(srcDb, destDb, "User", "User");
	HelpSameTableCopy(srcDb, destDb, "RightGroup", "RightGroup");
	HelpSameTableCopy(srcDb, destDb, "UserGroup", "UserGroup");
	HelpSameTableCopy(srcDb, destDb, "ManuType", "ManuType");
	HelpSameTableCopy(srcDb, destDb, "ProtocolType", "ProtocolType");
	HelpSameTableCopy(srcDb, destDb, "Organization", "Organization");
	HelpSameTableCopy(srcDb, destDb, "OrganDevice", "OrganDevice");
	HelpSameTableCopy(srcDb, destDb, "PresetInfo", "PresetInfo");
	HelpSameTableCopy(srcDb, destDb, "AIRelation", "AIRelation");
	HelpSameTableCopy(srcDb, destDb, "AreaType", "AreaType");
	HelpSameTableCopy(srcDb, destDb, "DYDJType", "DYDJType");
	HelpSameTableCopy(srcDb, destDb, "FirePrevType", "FirePrevType");
	HelpSameTableCopy(srcDb, destDb, "AxfResource", "AxfResource");
	HelpSameTableCopy(srcDb, destDb, "AreaCode", "AreaCode");
	HelpSameTableCopy(srcDb, destDb, "EstResource", "EstResource");
	HelpSameTableCopy(srcDb, destDb, "ExtRegResource", "ExtRegResource");
	HelpSameTableCopy(srcDb, destDb, "CruiseResource", "CruiseResource");
	HelpSameTableCopy(srcDb, destDb, "RecordRelation", "RecordRelation");
	HelpSameTableCopy(srcDb, destDb, "videolink", "videolink");
	HelpSameTableCopy(srcDb, destDb, "mtaccount", "mtaccount");
	
	// 	HelpSameTableCopy(srcDb, destDb, "AlarmLog", "AlarmLog");
	// 	HelpSameTableCopy(srcDb, destDb, "DeviceLog", "DeviceLog");
	// 	HelpSameTableCopy(srcDb, destDb, "SystemLog", "SystemLog");
	// 	HelpSameTableCopy(srcDb, destDb, "UserLog", "UserLog");
	
	sqlite3_exec(destDb, "COMMIT", 0, 0, 0);
	
	sqlite3_close(srcDb);
	sqlite3_close(destDb);
	
	return 0;
} 

S32 UpdateDb::FromV20180625ToV20181022(string strSrc, string strDest)
{
	sqlite3 *srcDb  = NULL;
	sqlite3 *destDb = NULL;
	
	sqlite3_open(strSrc.c_str(),  &srcDb);
	sqlite3_open(strDest.c_str(), &destDb);
	
	if (SystemInfoTableCopy(srcDb, destDb) < 0)
	{
		return -1;
	}
	
	sqlite3_exec(destDb,"BEGIN TRANSACTION",0,0,0);
	
	HelpSameTableCopy(srcDb, destDb, "DeviceInfo", "DeviceInfo");
	HelpSameTableCopy(srcDb, destDb, "Station", "Station");
	HelpSameTableCopy(srcDb, destDb, "User", "User");
	HelpSameTableCopy(srcDb, destDb, "RightGroup", "RightGroup");
	HelpSameTableCopy(srcDb, destDb, "UserGroup", "UserGroup");
	HelpSameTableCopy(srcDb, destDb, "ManuType", "ManuType");
	HelpSameTableCopy(srcDb, destDb, "ProtocolType", "ProtocolType");
	HelpSameTableCopy(srcDb, destDb, "Organization", "Organization");
	HelpSameTableCopy(srcDb, destDb, "OrganDevice", "OrganDevice");
	HelpSameTableCopy(srcDb, destDb, "PresetInfo", "PresetInfo");
	HelpSameTableCopy(srcDb, destDb, "AIRelation", "AIRelation");
	HelpSameTableCopy(srcDb, destDb, "AreaType", "AreaType");
	HelpSameTableCopy(srcDb, destDb, "DYDJType", "DYDJType");
	HelpSameTableCopy(srcDb, destDb, "FirePrevType", "FirePrevType");
	HelpSameTableCopy(srcDb, destDb, "AxfResource", "AxfResource");
	HelpSameTableCopy(srcDb, destDb, "AreaCode", "AreaCode");
	HelpSameTableCopy(srcDb, destDb, "EstResource", "EstResource");
	HelpSameTableCopy(srcDb, destDb, "ExtRegResource", "ExtRegResource");
	HelpSameTableCopy(srcDb, destDb, "CruiseResource", "CruiseResource");
	HelpSameTableCopy(srcDb, destDb, "RecordRelation", "RecordRelation");
	HelpSameTableCopy(srcDb, destDb, "videolink", "videolink");
	HelpSameTableCopy(srcDb, destDb, "mtaccount", "mtaccount");
	HelpSameTableCopy(srcDb, destDb, "AiPresetParam", "AiPresetParam");
	
	// 	HelpSameTableCopy(srcDb, destDb, "AlarmLog", "AlarmLog");
	// 	HelpSameTableCopy(srcDb, destDb, "DeviceLog", "DeviceLog");
	// 	HelpSameTableCopy(srcDb, destDb, "SystemLog", "SystemLog");
	// 	HelpSameTableCopy(srcDb, destDb, "UserLog", "UserLog");
	
	sqlite3_exec(destDb, "COMMIT", 0, 0, 0);
	
	sqlite3_close(srcDb);
	sqlite3_close(destDb);
	
	return 0;
} 