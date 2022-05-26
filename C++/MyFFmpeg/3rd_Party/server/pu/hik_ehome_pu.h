#ifndef __HIK_EHOME_PU_H__
#define __HIK_EHOME_PU_H__

#include "pu.h"
#include "tmap.h"
#include "tevqueue.h"
#include "hik_ehome_gu.h"
#include "3rd/hikehome/HCISUPCMS.h"
#include "3rd/hikehome/HCISUPPublic.h"
#include "json/json.h"

class HikEhomePu : public Pu
{

public:
    HikEhomePu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType);
    virtual ~HikEhomePu();
	tmap<string, LONG> m_deviceIDToUserId;
	tmap<string, LONG> m_deviceIDToTalkHandle;
    virtual int        start();
	virtual void       stop();


protected:
    virtual int        login();
    virtual void       logout();
	virtual bool       validate(int channelNo){ return true; };
	virtual bool       validate(string gucode){ return true; };

	virtual int        ptzCtrl(int cmd, int channelNo, int speed, int preset, bool stop);
	virtual int        ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop);

	virtual int        queryRecord(int channelNo, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json);
	virtual int        queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode);

	virtual int        syncTime(datetime_s &dt, const string &tzone){ return 0; };
	static BOOL CALLBACK RegisterCallBack(//×¢²á»Øµ÷º¯Êý
		LONG   iUserID,
		DWORD  dwDataType,
		void * pOutBuffer,
		DWORD  dwOutLen,
		void * pInBuffer,
		DWORD  dwInLen,
		void * pUser);

	BOOL   m_bStartCMS;
	LONG   m_lListenCMS;//CMS¼àÌý¾ä±ú
	LONG   m_lListenSMS;//SMS¼àÌý¾ä±ú
public:
	static string ExternalHost;
	static int    ExternalPort;

	static bool   m_isInit;

	int                initEhome(int cport, int sport);

	tevqueue<long, SHL_Json::Value>     m_cqueue;
	
	
};
extern HikEhomePu *EHome_Pu;
#endif
