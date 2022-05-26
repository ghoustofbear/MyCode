/**
* @file      hik_gateway_pu.h
* @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
* @brief     brief
* @author    caiwang213@qq.com
* @date      2017-06-20 11:55:51
*
* @note
*  hik_gateway_pu.h defines
*/
#ifndef __HIK_GATEWAY_PU_H__
#define __HIK_GATEWAY_PU_H__

#include "pu.h"
#include "3rd/hik/sdk-plat.h"
#include "xml/XmlBase.h"
class HikGateWayPu : public Pu
{
public:
    HikGateWayPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType);
    virtual ~HikGateWayPu();

    virtual int        start();
	friend void CALLBACK    login_CB(LONG userId, DWORD result, LPNET_DVR_DEVICEINFO_V30  deviceInfo, void *pUser);
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

    virtual int        onCustomCommand(string& json);
	virtual int        onRequestConfig(string& jin, string& jout);

	virtual int        gateWayCtrl(int gateWayIndex, int cmd);//控制摆闸开关状态
	virtual int        gateWayStatus();                       //获取摆闸开关状态
	int                pushGatewayStatus();                   //推送摆闸开关状态
	static void      * m_ThreadEntry(ThreadInfo *arg);
private:
	LONG               m_lUserID;
	Thread             m_thread;
	uint64_t           m_lastTimeStamp;
};
#endif
