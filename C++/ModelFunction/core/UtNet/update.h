/**
 * (C) Copyright 2013, XXXX Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 * Tue Feb 26 16:45:16 PST 2013
 *
 * update.h
 */
#ifndef __UPDATE_H__
#define __UPDATE_H__

#include "defc1.h"

#define  DB_VERSION_V20140109      (20140109)
#define  DB_VERSION_V20140115      (20140115)
#define  DB_VERSION_V20140424      (20140424)
#define  DB_VERSION_V20141113      (20141113)
#define  DB_VERSION_V20150115      (20150115)
#define  DB_VERSION_V20150618      (20150618)
#define  DB_VERSION_V20150826      (20150826)
#define  DB_VERSION_V20160519      (20160519)
#define  DB_VERSION_V20160711      (20160711)
#define  DB_VERSION_V20161109      (20161109)
#define  DB_VERSION_V20171012      (20171012)
#define  DB_VERSION_V20171115      (20171115)
#define  DB_VERSION_V20180223      (20180223)
#define  DB_VERSION_V20180625      (20180625)

typedef struct sqlite3 sqlite3;
class CM_API UpdateDb
{
	public:
		UpdateDb();
		virtual ~UpdateDb();

		VOID Update();

		S32 ImportDb(string strSrcDb, string strDesDb);

	protected:
		S32 FromV20140109ToV20140115(string strSrc, string strDest);
		S32 FromV20140115ToV20140424(string strSrc, string strDest);
		S32 FromV20140424ToV20141113(string strSrc, string strDest);
		S32 FromV20141113ToV20150115(string strSrc, string strDest);
		S32 FromV20150115ToV20150618(string strSrc, string strDest);
        S32 FromV20150618ToV20150826(string strSrc, string strDest);
		S32 FromV20150826ToV20160519(string strSrc, string strDest);
		S32 FromV20160519ToV20160711(string strSrc, string strDest);
		S32 FromV20160711ToV20161109(string strSrc, string strDest);
		S32 FromV20161109ToV20171012(string strSrc, string strDest);
		S32 FromV20171012ToV20171115(string strSrc, string strDest);
		S32 FromV20171115ToV20180223(string strSrc, string strDest);
		S32 FromV20180223ToV20180625(string strSrc, string strDest);
		S32 FromV20180625ToV20181022(string strSrc, string strDest);
		
		S32 HelpSameTableCopy(
				sqlite3 * srcDb, 
				sqlite3 * destDb, 
				string    srcTable, 
				string    destTable);

		S32 SystemInfoTableCopy(sqlite3 *srcDb, sqlite3 *destDb);
};

#endif
