/**
* @file      huadian_pu.h
* @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
* @brief     brief
* @author    caiwang213@qq.com
* @date      2017-06-20 11:55:51
*
* @note
*  huadian_pu.h defines
*/

#ifndef __HUADIAN_PU_H__
#define __HUADIAN_PU_H__

#include "pu.h"

class RtspClient;

class HuaDianPu : public Pu
{
public:
    HuaDianPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType);
    virtual ~HuaDianPu();

    virtual bool       validate(int channelNo) { return true; }
    virtual bool       validate(string gucode) { return true; }

    virtual int        queryRecord(int channelNo, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json) { return 0; }
    virtual int        queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode) { return 0; }

    virtual int        ptzCtrl(int cmd, int channelNo, int speed, int preset, bool stop) { return 0; }
    virtual int        ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop) { return 0; }

    virtual int        syncTime(datetime_s &dt, const string &tzone) { return 0; }

    virtual int        start();
    virtual void       stop();

    virtual int        login();
    virtual void       logout();

    virtual void       onTimer();

private:
    int                heartbeat();

private:
    RtspClient       * m_rtspClient;
    time_t             m_lastTime;
};

#endif

