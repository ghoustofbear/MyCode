/**
* @file      launch_pu.h
* @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
* @brief     brief
* @author    caiwang213@qq.com
* @date      2017-06-20 11:55:51
*
* @note
*  launch_pu.h defines
*/
#ifndef __LAUNCH_PU_H__
#define __LAUNCH_PU_H__

#include "pu.h"
#include "curl/curl.h"
#include <mutex>
#include "launch_server.h"
#include "tmap.h"
typedef struct tagDeviceStatus{
	string device_id     = "";
	int    online_status = -1;
    int    fault_status = -1;
    int    run_status = -1;
	int    ctrl_mode     = -1;
	int    ctrl_priority = -1;
}DeviceStatus;
class LaunchPu : public Pu
{
public:
    LaunchPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType);
    virtual ~LaunchPu();
    virtual int        start();
	int                queryDeviceStatus(string device_id,DeviceStatus&status);          
	int                subscribeDeviceStatus(int msg_type[], int msg_num,string dest_url);
	int                unsubscribeDeviceStatus(int msg_type[], int msg_num);               
	size_t  static     read_data(void* buffer, size_t size, size_t nmemb, void* userdata);
	size_t  static     write_data(void* buffer, size_t size, size_t nmemb, void* userdata);
protected:
    virtual void       stop();
    virtual int        login();
    virtual void       logout();


    virtual bool       validate(int channelNo){ return true; }
    virtual bool       validate(string gucode) { return true; }
    virtual int        ptzCtrl(int cmd, int channelNo, int speed, int preset, bool stop) { return 0; }
    virtual int        ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop) { return 0; }
    virtual int        queryRecord(int channelNo, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json) { return 0; }
    virtual int        queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode) { return 0; }
    virtual int        syncTime(datetime_s &dt, const string &tzone) { return 0; }

public:
    virtual int        onCustomCommand(string& json);
	virtual int        onRequestConfig(string& jin, string& jout);
	static void      * heartbeat(ThreadInfo *arg);
	static void      * statusEntry(ThreadInfo *arg);


private:
	int                curlPost(string jin, string url, string &jout);
	int                curlGet(string &url, string &jout);
private:
	Thread             m_thread;
	uint64_t           m_lastTimeStamp;
	string             m_token;
	int                m_expiretime;
	LaunchServer      *launchSVR;
	string             m_launchHost;
	int                m_launchPort;

	Thread             m_statusThread;
	int                m_statusinterval;
	uint64_t           m_lastStatusTimeStamp;
	tmap<string, int>  m_deviceStatus;
	string             m_proxyHost;
	int                m_proxyPort;
};
extern LaunchPu *launchPu;
#endif
