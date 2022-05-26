#include "utsip_pu.h"
#include "ut_sipua.h"
#include "system.h"
#include "sipua.h"

UtSipPu::UtSipPu(int type, string addrcode, string name, string host, int sport, int rport, int hport, int rtport)
    :GwPu(type, addrcode, name, host, sport, rport, hport, rtport)
{
    m_protocol = PROTO_TYPE_GW_B_UT;
    m_firstSubAlarm = true;
    m_tmPrevTime    = 0;
    m_tmPrevTimeSub = 0;
}

UtSipPu::~UtSipPu()
{
	stop();
}

int
UtSipPu::start()
{
    if (Pu::start() < 0)
    {
        return -1;
    }

    if (!m_sipua)
    {
        m_sipua = new UtSipUA(IPPROTO_UDP, "", -1);
        m_sipua->start();
    }

    m_init = true;
    return 0;
}

void
UtSipPu::stop()
{
    m_init = false;
    m_thread.stop();

    logout();

    Pu::stop();
}

int
UtSipPu::login()
{
    //if (m_login)
    //{
    //    return 0;
    //}
    //
    //string xml;
    //int Ret = ((UtSipUA *)m_sipua)->query_resource(systm->addrcode(), "", m_sipua->port(), m_addrcode, m_ip, m_sport, m_addrcode, xml, 0, 1, 1);
    //if (Ret < 0)
    //{
    //    return -1;
    //}

    //subscribeEvent(addrcode(), 0, 0xffffffff, 3600);
    //subscribeEvent(addrcode(), 1, 0, 3600);

    //m_login = true;
    m_thread.setEntry(threadProcEx, this, 1);
    m_thread.start();
    return 0;
}

void
UtSipPu::logout()
{

}


int       
UtSipPu::queryResource(string addrcode, int fromIndex, int toIndex, string &json)
{
    int Ret = ((UtSipUA *)m_sipua)->query_resource(systm->addrcode(), "", m_sipua->port(), m_addrcode, m_ip, m_sport, m_addrcode, json, 0, fromIndex, toIndex);
    return Ret;
}

int        
UtSipPu::queryAlarm(string addrcode, int type, int level, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode)
{
    Thread *thread = new Thread;
    ParamAlarm_T *param = new ParamAlarm_T(thread,m_tid, T_ALARMQUERY, this, addrcode, type, level, start, end, fromIndex, toIndex, usercode);
    thread->setEntry(threadProc, param, T_ALARMQUERY);
    thread->start();
    thread->detach();
    return 0;
}

int
UtSipPu::queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode)
{
    Thread *thread = new Thread;
    ParamRecord_T *param = new ParamRecord_T(thread, m_tid, type, this, addrcode, start, end, fromIndex, toIndex, usercode);
    thread->setEntry(threadProc, param, T_RECORDQUERY);
    thread->start();
    thread->detach();

    return 0;
}

void                
UtSipPu::heartBeatLoop()
{
    while (m_thread.active())
    {
        sys->sleep(10);
        uint64_t tmNowTime = sys->currentTimeMillis();

        //Heart beat
        if (tmNowTime - m_tmPrevTime >= 60 * 1000)
        {
            string xml;
            int Ret = ((UtSipUA *)m_sipua)->query_resource(systm->addrcode(), "", m_sipua->port(), m_addrcode, m_ip, m_sport, m_addrcode, xml, 0, 1, 1);
            if (Ret < 0)
            {
                m_login = false;
            }
            else
                m_login = true;
            m_tmPrevTime = tmNowTime;
        }

        if (m_login)
        {
#if 0
            //subscribe alarm  and status
            if (m_firstSubAlarm)
            {
                subscribeEvent(addrcode(), 0, 0xffffffff, 3600);
                subscribeEvent(addrcode(), 1, 0, 3600);
                m_firstSubAlarm = false;
            }
            else
            {
                static bool bAlarm = true;
                if (tmNowTime - m_tmPrevTimeSub >= 1500 * 1000)
                {
                    if (bAlarm)
                        subscribeEvent(addrcode(), 0, 0xffffffff, 3600);
                    else
                        subscribeEvent(addrcode(), 1, 0, 3600);
                    bAlarm = !bAlarm;
                }
                m_tmPrevTimeSub = tmNowTime;
            }
#endif
        }
    }
}

void
UtSipPu::onTimer()
{
    Pu::onTimer();
}

void *
UtSipPu::threadProcEx(ThreadInfo *info)
{
    UtSipPu *pu = (UtSipPu*)info->data;
    switch (info->id)
    {
    case 1:
        pu->heartBeatLoop();
        break;
    default:
        break;
    }
    return NULL;
}