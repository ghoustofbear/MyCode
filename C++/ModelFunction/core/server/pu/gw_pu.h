/**
 * @file      gw_pu.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-12-29 09:50:20
 *
 * @note
 *  gw_pu.h defines
 */
#ifndef __GW_PU_H__
#define __GW_PU_H__
#include "pu.h"

typedef void(*MessageRspCB)(string response, int code, int tid, void *userdata);

class SipUA;
class RTSPCall;
class GwPu : public Pu
{
	friend class GwbRealGu;
	friend class GwSu;
public:
    enum
    {
        T_PREVIEW,
        T_PTZCTRL,
        T_ALARMQUERY,
        T_RECORDQUERY,
        T_ALARMSUB,
        T_PLAYBACK,
        T_DEVTALK,
    };
    struct Param_T
    {
        Thread *m_thread;
        int m_tid;
        int m_type;
        void *m_pThis;
        string m_addrcode;
        Param_T(Thread *thread,int tid, int type, void *pThis, string addrcode)
            :m_thread(thread), m_tid(tid), m_type(type), m_pThis(pThis), m_addrcode(addrcode){
            ;
        }
    };

    struct ParamPtzCtrl_T : public Param_T{
        int m_cmd;
        int m_speed;
        int m_preset;
        bool m_stop;
        ParamPtzCtrl_T(Thread *thread, int tid, int type, void *pThis, string addrcode, int cmd, int speed, int preset, int stop) :Param_T(thread,tid, type, pThis, addrcode), m_cmd(cmd), m_speed(speed), m_preset(preset), m_stop(stop){ ; }
    };

    struct ParamAlarm_T : public Param_T{
        int m_alarmType;
        int m_level;
        datetime_s m_start;
        datetime_s m_end;
        int m_fromIndex;
        int m_toIndex;
        string m_usercode;
        ParamAlarm_T(Thread *thread, int tid, int type, void *pThis, string addrcode, int alarmType, int level, datetime_s start
            , datetime_s end, int fromIndex, int toIndex,  string usercode)
            : Param_T(thread, tid, type, pThis, addrcode), m_alarmType(alarmType), m_level(level), m_start(start), m_end(end), m_fromIndex(fromIndex)
            , m_toIndex(toIndex), m_usercode(usercode){;}
    };

    struct ParamRecord_T : public Param_T {
        int m_recordType;
        datetime_s m_start;
        datetime_s m_end;
        int m_fromIndex;
        int m_toIndex;
        string m_usercode;
        ParamRecord_T(Thread *thread, int tid, int type, void *pThis, string addrcode, datetime_s start, 
            datetime_s end, int fromIndex, int toIndex, string usercode)
            : Param_T(thread, tid, type, pThis, addrcode), m_recordType(type), m_start(start), m_end(end), m_fromIndex(fromIndex),
            m_toIndex(toIndex), m_usercode(usercode){ }
    };

public:
    GwPu(int type, string addrcode, string name, string host, int sport, int rport, int hport, int rtport);
    virtual ~GwPu();

    virtual bool       validate(int channelNo);
    virtual bool       validate(string gucode);

    virtual int        ptzCtrl(int cmd, int channelNo ,   int speed, int preset, bool stop);
    virtual int        ptzCtrl(int cmd, string addrcode , int speed, int preset, bool stop);


    virtual int        queryRecord(int channelNo, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json);
    virtual int        queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode);
	virtual int        queryResource(string addrcode, int fromIndex, int toIndex, string &json);
	virtual int        subscribeEvent(string addrcode, int mode, int alarmtype, int expires);

    virtual int        syncTime(datetime_s &dt, const string &tzone);
    
    virtual int        setMessageCallBack(int tid, MessageRspCB cb,void *userdata);
    void               resetMessageCB();

    int                rport() { return m_rtport; }

    static void      * threadProc(ThreadInfo *arg);
protected:
    int                m_sport;
    int                m_rport;
    int                m_hport;
    int                m_rtport;

protected:  
    RTSPCall         * m_rtspcall;
    MessageRspCB      m_message_cb;
    int                m_tid;
    void             * m_userdata;
	uint64_t           m_tmPrevTime;

public:
    SipUA            * m_sipua;
};

#endif
