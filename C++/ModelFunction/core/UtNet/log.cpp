#include "log.h"
#include "easySQLite/sqlite3.h"
#include "configc1.h"
#include "commonc1.h"

#define ENABLE_LOG_DEBUG
#ifdef  ENABLE_LOG_DEBUG 
#ifdef _WIN32
#define LOG_DEBUG             printf
#else
#define LOG_DEBUG(MSG...)     DEBUG("LOG", MSG)
#endif /* end _WIN32 */
#else
#define LOG_DEBUG(MSG...)
#endif /* end ENABLE_LOG_DEBUG */

Log *g_pLog = NULL;
Log::Log()
{
	g_pLog = this;

    SetCurrentDirectory();
	
    sqlite3_open(CONFIG_DB, &m_pDb);

	m_bExit = FALSE;
	CreateJoinableThread(LogProcess, this, &m_trdLog);
}

Log::~Log()
{
	m_bExit = TRUE;
	ThreadJoin(m_trdLog);
	sqlite3_close(m_pDb);
}

S32 Log::SystemLog(U32 u32Type, U32 u32UserId, S8 *ps8LogInfo)
{
	ASSERT(ps8LogInfo);

	S8 s8Sql[SQL_LEN] = {0};
	S8 s8Time[36]     = {0};
	DATETIME_S stTime = {0};

	GetDateTime(&stTime);
	snprintf(s8Time, SQL_LEN, "%04d-%02d-%02d %02d:%02d:%02d",
			stTime.u32Year,
			stTime.u32Month,
			stTime.u32Day,
			stTime.u32Hour,
			stTime.u32Minute,
			stTime.u32Second);

	snprintf(s8Sql, SQL_LEN, "insert into SystemLog \
			(Time, LogType, LogInfo, UserId) \
			values ('%s', %d, '%s', %d);",
			s8Time, u32Type, ps8LogInfo, u32UserId);

	m_logList.Lock();
    m_logList.push_back(s8Sql);
	m_logList.UnLock();

	return 0;

	return SqliteExec(m_pDb, s8Sql);
}

S32 Log::AlarmLog(S8 *ps8AddrCode, U32 u32Type, U32 u32Status, U32 u32Channel)
{
	ASSERT(ps8AddrCode);

	S8 s8Sql[SQL_LEN] = {0};

	string     strTime;

	GetDateTime(strTime);

	snprintf(s8Sql, SQL_LEN, "insert into AlarmLog \
			(AddrCode, Type, State, Level, Time, Channel) \
			values ('%s', %d, %d, %d, '%s', %d);",
			ps8AddrCode, 
			u32Type, 
			u32Status,
			1,
			strTime.c_str(),
			u32Channel);

	m_logList.Lock();
    m_logList.push_back(s8Sql);
	m_logList.UnLock();
	
	return 0;

	return SqliteExec(m_pDb, s8Sql);
}

S32 Log::UserLog(U32 u32UserId, S8 *ps8LogInfo)
{
	ASSERT(ps8LogInfo);

	S8 s8Sql[SQL_LEN] = {0};
	string     strTime;

	GetDateTime(strTime);

	snprintf(s8Sql, SQL_LEN, "insert into UserLog \
			(Time, LogType, LogInfo, UserId) \
			values ('%s', %d, '%s', %d);",
			strTime.c_str(), 0, ps8LogInfo, u32UserId);

	m_logList.Lock();
    m_logList.push_back(s8Sql);
	m_logList.UnLock();
	
	return 0;

	return SqliteExec(m_pDb, s8Sql);
}

S32 Log::DvivceLog(S8 *ps8AddrCode, S8 *ps8LogInfo)
{
	ASSERT(ps8AddrCode);
	ASSERT(ps8LogInfo);

	S8 s8Sql[SQL_LEN] = {0};
	string     strTime;

	GetDateTime(strTime);

	snprintf(s8Sql, SQL_LEN, "insert into DeviceLog \
			(AddrCode, State, Time) \
			values ('%s', '%s', '%s');",
			ps8AddrCode, ps8LogInfo, strTime.c_str());
    m_logList.Lock();
    m_logList.push_back(s8Sql);
	m_logList.UnLock();
	
	return 0;

	return SqliteExec(m_pDb, s8Sql);
}

VOID Log::SaveLog()
{
	while(!m_bExit)
	{
        if (m_logList.size() == 0)
		{
			Msleep(1000);
			continue;
		}
		
		{
            m_logList.Lock();
			
			sqlite3_exec(m_pDb,"BEGIN TRANSACTION",0,0,0);
			
			for (TList<std::string>::iterator begn = m_logList.begin(); begn != m_logList.end(); ++begn)
			{
                std::string data = (*begn);
				SqliteExec(m_pDb, (char*)data.c_str());
			}
			
			m_logList.clear();

			sqlite3_exec(m_pDb, "COMMIT", 0, 0, 0);
			
            m_logList.UnLock();
		}
		
		Msleep(500);
	}

}

VOID * Log::LogProcess(VOID *pArg)
{
    Log * pThis = (Log *)pArg;

    pThis->SaveLog();

    return NULL;
}