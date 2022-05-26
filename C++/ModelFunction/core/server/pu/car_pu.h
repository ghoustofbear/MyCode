/**
* @file      car_pu.h
* @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
* @brief     brief
* @author    caiwang213@qq.com
* @date      2017-06-20 11:55:51
*
* @note
*  car_pu.h defines
*/


#ifndef __CAR_PU_H__
#define __CAR_PU_H__

#include "pu.h"

class CarPu : public Pu 
{
public:
    CarPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType);
    virtual ~CarPu();


    virtual int        ptzCtrl(int cmd, int channelNo, int speed, int preset, bool stop);
    virtual int        ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop);

    virtual bool       validate(int channelNo){ return false; }
    virtual bool       validate(string gucode){ return false; }

    virtual int        queryRecord(int channelNo, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json);
    virtual int        queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode);
    virtual int        syncTime(datetime_s &dt, const string &tzone);


    virtual int        start();

protected:
    virtual void       stop();
    virtual int        login();
    virtual void       logout();

    virtual void       onTimer();


private:
    Socket           * m_sock;
    time_t             m_tmPre;
    time_t             m_tmNow;
};

#endif
