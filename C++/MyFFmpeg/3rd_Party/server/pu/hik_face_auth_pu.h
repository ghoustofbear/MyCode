/**
* @file      hik_face_auth_pu.h
* @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
* @brief     brief
* @author    caiwang213@qq.com
* @date      2017-06-20 11:55:51
*
* @note
*  hik_face_auth_pu.h defines
*/
#ifndef __HIK_FACE_AUTH_PU_H__
#define __HIK_FACE_AUTH_PU_H__

#include "pu.h"
#include "3rd/hik/sdk-plat.h"
#include "tmap.h"
#include "tevqueue.h"

typedef tmap<long, Pu *>      UserMap;
typedef tevent<string, int>   EventMutx;

typedef struct FaceAlarmInfo_S
{
    int iType;
    union
    {
        NET_DVR_ID_CARD_INFO_ALARM * pDVRIdCardInfo;
        NET_DVR_ACS_ALARM_INFO     * pDVRAcsAlarmInfo;
    };
}FaceAlarmInfo_S;

class HikFaceAuthPu : public Pu
{
    friend void CALLBACK    flogin_cb(LONG userId, DWORD result, LPNET_DVR_DEVICEINFO_V30  deviceInfo, void *pUser);
    friend void CALLBACK    alarm_msg_cb(LONG cmd, NET_DVR_ALARMER *alarmer, char *alarmInfo, DWORD bufLen, void *userData);
    friend void CALLBACK    set_card_cb(DWORD dwType, void* lpBuffer, DWORD dwBufLen, void* pUserData);
    friend void CALLBACK    set_face_cb(DWORD dwType, void* lpBuffer, DWORD dwBufLen, void* pUserData);
public:
    HikFaceAuthPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType);
    virtual ~HikFaceAuthPu();

    virtual int        start();

    long getUserID();
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
    virtual void       onTimer();

private:
    void               setCardInfo(NET_DVR_CARD_CFG_V50 & netDvrCardInfo);
    int                setCardNo(string name, string Id, int ivalidNo);
    int                addFaceData(const string& picData, string name, string Id);
    int                delFaceData(string name, string Id);
    int                pushAlarmInfo(FaceAlarmInfo_S*);

private:


    static UserMap     m_userMap;
    LONG               m_userID;
    LONG               m_alarmHandle;
    EventMutx          m_evtSetCard;
    EventMutx          m_evtSetFaceData;
    tqueue<FaceAlarmInfo_S*> m_alarmList;
};
#endif
