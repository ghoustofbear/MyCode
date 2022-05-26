#ifndef __UTSIP_PU_H__
#define __UTSIP_PU_H__
#include "gw_pu.h"
#include "rtspclient.h"

class UtSipPu : public GwPu
{
public:
    UtSipPu(int type, string addrcode, string name, string host, int sport, int rport, int hport, int rtport);
    virtual ~UtSipPu();

public:
    virtual int        start();

    virtual int        queryResource(string addrcode, int fromIndex, int toIndex, string &json);
    virtual int        queryAlarm(string addrcode, int type, int level, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode);
    virtual int        queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode);
    virtual void       onTimer();
    virtual bool       validate(int channelNo){ return true; }

    static void      * threadProcEx(ThreadInfo *arg);
    void                heartBeatLoop();
protected:
    virtual void       stop();
    virtual int        login();
    virtual void       logout();

protected:

private:
    bool                m_firstSubAlarm;
    uint64_t            m_tmPrevTime;
    uint64_t            m_tmPrevTimeSub;
    Thread              m_thread ;
};
#endif
