#ifndef __ONVIF_PU_H__
#define __ONVIF_PU_H__
#include "pu.h"
#include "onvif/onvifclient.h"
#include "dahua_pu.h"
#include "tls_error.h"
#include<map>
class onvifPu : public Pu
{
public:
	onvifPu(string host, int sport, string user, string password);

	onvifPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType, int linktype);

	virtual ~onvifPu();
public:
	virtual int        start();

	virtual void       onTimer();
	virtual bool       validate(int channelNo){ return true; }
	virtual bool       validate(string gucode) { return true; }

	virtual int        ptzCtrl(int cmd, int channelNo, int speed, int preset, bool stop);
	virtual int        ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop) { return 0; }

	virtual int        queryResource(string addrcode, int fromIndex, int toIndex, string &json){ return 0; }
	virtual int        queryRecord(int channelNo, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json);
	virtual int        queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode){ return 0; }

	virtual int        syncTime(datetime_s &dt, const string &tzone) { return 0; }
	virtual void       UpdateAliveTime();
	OnvifClientDevice* Getonvifclient()      { return onvifDevice; }
	OnvifClientMedia*  Getonvifclientmedia() { return m_media; }
	_trt__GetProfilesResponse * Getprofiles(){ return m_profiles; }


protected:
	virtual void       stop();
	virtual int        login();
	virtual void       logout();

private:
	OnvifClientDevice* onvifDevice;         //获取设备信息
	OnvifClientMedia*  m_media;
	OnvifClientPTZ*    m_Ptz;
	_trc__GetRecordingsResponse recordings;
	OnvifClientRecording* m_Recording;
	OnvifClientReplay* m_Replay;
	string replayUrl;
	_trt__GetProfilesResponse* m_profiles; //媒体信息
	DahuaPu* m_DahuaPu;
	std::map<string, string>m_rtsp;
	
};
#endif