#ifndef __GB28181_PU_H__
#define __GB28181_PU_H__
#include "pu.h"
#include "gb28181_sipua.h"
#include "gw_pu.h"

class GB28181Pu : public Pu
{
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
        T_HEARTBEAT,
        T_SOURCE,
    };
    struct Param_T;
    struct ParamSource_T;
public:
    GB28181Pu(int type, string addrcode, string name, string host, int sport, int rport, int hport, int rtport);
    virtual ~GB28181Pu();

public:
    virtual int        start();

    virtual int        queryResource(string addrcode, int fromIndex, int toIndex, string &json);
    virtual void       onTimer();
    virtual bool       validate(int channelNo){ return true; }
    virtual bool       validate(string gucode) { return true; }

    virtual int        ptzCtrl(int cmd, int channelNo, int speed, int preset, bool stop) { return 0; }
    virtual int        ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop);

    virtual int        queryRecord(int channelNo, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json) { return 0; }
    virtual int        queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode);

    virtual int        syncTime(datetime_s &dt, const string &tzone) { return 0; }
    virtual void       UpdateAliveTime();

    static void      * threadProcEx(ThreadInfo *arg);
    void                heartBeatLoop();

    int                 sport(){ return m_sport; }
    int                 rport() { return m_rtport; }
protected:
    virtual void       stop();
    virtual int        login();
    virtual void       logout();

public:
    GB28181_SipUA    * m_sipua;

protected:
    int                m_sport;
    int                m_rport;
    int                m_hport;
    int                m_rtport;

private:
    uint64_t            m_tmPrevTime;
    uint64_t            m_tmPrevTimeSub;
    Thread              m_thread;

    MessageRspCB      m_message_cb;
    int                m_tid;
    void             * m_userdata;
    uint64_t           m_aliveTime;
};
#endif
