/**
 * (C) Copyright 2013, XXXX Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 * Tue Feb 26 16:45:16 PST 2013
 *
 * log.h 
 */
#ifndef __LOG_H__
#define __LOG_H__

#include "defc1.h"
#include "commonc1.h"
#include "tlistc1.h"
#include <string>

typedef struct sqlite3 sqlite3;
class CM_API Log 
{
	public:
		Log();
		virtual ~Log();

		enum
		{
			LOG_TYPE_SYSTEM = 0,
			LOG_TYPE_ALARM,
			LOG_TYPE_USER,
			LOG_TYPE_DEVICE
		};
		
		enum
		{
			SYSLOG_TYPE_ALL = 0,
			SYSLOG_TYPE_SYSTEM,
			SYSLOG_TYPE_OPERATE,
			SYSLOG_TYPE_EVENT
		};

	S32 SystemLog(U32 u32Type, U32 u32UserId, S8 *ps8LogInfo);
	S32 AlarmLog(S8 *ps8AddrCode, U32 u32Type, U32 u32Status, U32 u32Channel);
	S32 UserLog(U32 u32UserId, S8 *ps8LogInfo);
	S32 DvivceLog(S8 *ps8AddrCode, S8 *ps8LogInfo);

	static VOID *LogProcess(VOID *pArg);

	void   SaveLog();

	private:
		sqlite3  *m_pDb;
		THREAD    m_trdLog;
		BOOL      m_bExit;

		TList<std::string>  m_logList;
		
};
CM_API extern Log *g_pLog;
#endif
