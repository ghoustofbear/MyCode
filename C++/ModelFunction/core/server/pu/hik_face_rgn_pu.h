#ifndef __HIK_FACE_RNG_PU_H__
#define __HIK_FACE_RNG_PU_H__

#include "pu.h"
#include "3rd/hik/sdk-plat.h"
#include "tmap.h"
#include "tevqueue.h"

typedef NET_DVR_DEVICECFG_V40 HIK_DVR_CFG;
typedef tmap<long, Pu *>      UserMap;
typedef tevent<string, int>   EventMutx;

class HikFaceRngPu : public Pu
{
	friend void CALLBACK    facerng_login_cb(LONG userId, DWORD result, LPNET_DVR_DEVICEINFO_V30  deviceInfo, void *pUser);
	friend void CALLBACK    facerng_alarm_msg_cb(LONG cmd, NET_DVR_ALARMER *alarmer, char *alarmInfo, DWORD bufLen, void *userData);

public:
	HikFaceRngPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType);
	virtual ~HikFaceRngPu();

	long               getUserID();
	HIK_DVR_CFG        getDvrCfg();

	virtual int        start();

protected:
	virtual void       stop();
	virtual int        login();
	virtual void       logout();


	virtual bool       validate(int channelNo){ return true; }
	virtual bool       validate(string gucode) { return true; }
	virtual int        ptzCtrl(int cmd, int channelNo, int speed, int preset, bool stop);
	virtual int        ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop);
	virtual int        queryRecord(int channelNo, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json) { return 0; }
	virtual int        queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode) { return 0; }
	virtual int        syncTime(datetime_s &dt, const string &tzone) { return 0; }

	virtual int        onCustomCommand(string& json);
	virtual int		   getPicSize(int chanNo);

private:
	int                addFaceData(const string& picData, string name, string Id, string IDCard);
	int                delFaceData(string name, string Id);

	/* get face database ID from camera */
	int				   getFDID();
	/* capture a picture 20200908 zhangshenyu add*/
	int                capturePic(string json);

private:
	static UserMap     m_userMap;
	LONG               m_userId;
	LONG               m_alarmHandle;
	EventMutx          m_evtSetFaceData;
	tqueue<NET_VCA_FACESNAP_MATCH_ALARM*> m_alarmList;
	HIK_DVR_CFG        m_dvrCfg;
};

#endif