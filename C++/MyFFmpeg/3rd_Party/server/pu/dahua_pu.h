/**
 * @file      hik_pu.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-06-20 11:55:51
 *
 * @note
 *  dahua_pu.h defines
 */

#ifndef __DAHUA_PU_H__
#define __DAHUA_PU_H__

#include "pu.h"
#include "3rd/dahua/dhnetsdk.h"
#include "3rd/dahua/dhconfigsdk.h"
#include "tmap.h"


#define  CBK CALLBACK


class DahuaPu : public Pu
{
public:  
    DahuaPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType, int linktype);
    virtual ~DahuaPu();

    LLONG               getUserID(){ return m_userId; }
    LLONG               getUserSubID(){ return m_lUserSubID; }

    virtual int        start();

protected:
    virtual void       stop();

    virtual int        login();
    virtual void       logout();

    virtual bool       validate(int channelNo);
    virtual bool       validate(string gucode);

    virtual int        ptzCtrl(int cmd, int channelNo ,   int speed, int preset, bool stop);
    virtual int        ptzCtrl(int cmd, string addrcode , int speed, int preset, bool stop);

    virtual int        queryRecord(int channelNo ,  int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json);
    virtual int        queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode);
	int                SetRuleConfig(string &jsonin);

	int				   OnQuerypointinfo(int channelNo, int PresetID, int  RuleID, int type, string &json);
	int				   OnQueryiteminfo(int ruleId, int channelNo, int PresetID, string & json, int ruleCalibType = 0);
	int                startmeasuretemp(int ruleId, int channelNo, int PresetId, string &jout);

	virtual int        onCustomCommand(string& json);
	virtual int        onRequestConfig(string & jin, string& jout);

    virtual int        syncTime(datetime_s &dt, const string &tzone);
    virtual void       onTimer();

    BOOL               ReceiveMessageEx(LLONG lLoginID, LONG lCommand, char *pchDVRIP, LONG nDVRPort, char *pBuf, DWORD dwBufLen);
    BOOL               ReceiveMessage(LLONG lLoginID, LONG lCommand, char *pchDVRIP, LONG nDVRPort, char *pBuf, DWORD dwBufLen);
    
    friend void CBK    AutoConnectFunc(LLONG lLoginID, char *pchDVRIP, LONG nDVRPort, LDWORD dwUser);
    friend void CBK    DisConnectFunc(LLONG lLoginID, char *pchDVRIP, LONG nDVRPort, LDWORD dwUser);
    friend BOOL CBK    AlarmDataCallBack(LONG lCommand, LLONG lUserID, char *pBuf, DWORD dwBufLen, char *ps8DVRIP, LONG nDVRPort, LDWORD dwUser);

    struct USER_ID_S
    {
        LONG     lUserId;
        string   strIp;
        Pu     * pPu;
    };

	struct temprule_s
	{
		int chnNo;
		int ruleId;
		int presetNo;
		int ruleCalibType;
	};


private:
    LLONG					  m_userId;
    LLONG					  m_lUserSubID;
	bool					  m_startmeasure;
	uint64_t				  m_lastpts;
	vector<temprule_s>        m_ruleid;
	Mutex					  temprule_s_mtx;

    NET_DEVICEINFO_Ex  m_m_devinfo;

public:
    bool               m_bNewVer;

    static tmap<long, Pu*> m_userMap;
};

#endif
