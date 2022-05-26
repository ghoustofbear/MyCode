/**
* @file      tindy_pu.h
* @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
* @brief     brief
* @author    chenjiaqi
* @date      2017-06-20 11:55:51
*
* @note
*  tindy_pu.h defines
*/

#ifndef __TINDY_PU_H__

#define __TINDY_PU_H__
#include "3rd/tindy/sdk-plat.h"
#include "pu.h"
#include "tmap.h"



typedef  tmap<long ,Pu*>  UserMap;

class TindyPu: public Pu
{
public:
    TindyPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType);
    virtual ~TindyPu();

    long               getUserID();

    

protected:

    virtual int        start();
    virtual void       stop();

    virtual int        login();
    virtual void       logout();

    virtual bool       validate(int channelNo);
    virtual bool       validate(string gucode);

    virtual int        ptzCtrl(int cmd, int channelNo, int speed, int preset, bool stop);
    virtual int        ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop);

    virtual int        queryRecord(int channelNo, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json);
    virtual int        queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode);

    virtual int        syncTime(datetime_s &dt, const string &tzone);

    virtual void       timerdriver();
 
    friend void        login_cb(int userId, long wParam, void* lParam, void *pUser);
    friend void        alarm_cb(int userId, int chanNo, int alarmState, ALARMTYPE alarmType, void *userData);
    friend void        changeParam_cb(int userId, int chanNo, PARATYPE paramType, STR_Para* strcParam,void *userData);


private:
    string             m_privDomain;
    long               m_userID;

    //string             m_recordFile;
    bool               m_bFindFinish;
    int                m_startTalkChn;
    static UserMap     m_userMap;
    
};




#endif // 0

