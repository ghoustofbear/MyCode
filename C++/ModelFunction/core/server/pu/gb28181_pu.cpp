#include "gb28181_pu.h"
#include "system.h"


GB28181Pu::GB28181Pu(int type, string addrcode, string name, string host, int sport, int rport, int hport, int rtport)
    :Pu(type, addrcode, name, 0, "", "", host)
{
    m_protocol = PROTO_TYPE_GB28181;
    m_tmPrevTime    = 0;
    m_tmPrevTimeSub = 0;


    m_sport = sport;
    m_rport = rport;
    m_hport = hport;
    m_rtport = rtport;
    m_port = m_rport;

    m_sipua = NULL;
    m_tmPrevTime = 0;
    m_aliveTime = sys->currentTimeMillis();
}

GB28181Pu::~GB28181Pu()
{
	stop();
}

int
GB28181Pu::start()
{
    if (Pu::start() < 0)
    {
        return -1;
    }

    if (!m_sipua)
    {
        m_sipua = new GB28181_SipUA(IPPROTO_UDP, "", -1);
        m_sipua->start();

        //m_sipua->subscribe_envent(systm->addrcode(), "", m_sipua->port(), m_addrcode, m_ip, m_sport, 0,sys->currentTime())
    }

    m_init = true;
    return 0;
}

void
GB28181Pu::stop()
{
    m_init = false;
    m_thread.stop();

    logout();

    Pu::stop();
}

int
GB28181Pu::login()
{
    m_thread.setEntry(threadProcEx, this, T_HEARTBEAT);
    m_thread.start();
    return 0;
}

void
GB28181Pu::logout()
{

}

int        
GB28181Pu::ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop)
{
    int param1 = 0;
    int param2 = 0;
    int param3 = 0;
    switch (cmd)
    {
    case PTZ_CMD_UP:            cmd = stop ? GB28181_PTZ_CMD_ALLSTOP_PTZ : GB28181_PTZ_CMD_UP; param2 = speed;       break;
    case PTZ_CMD_DOWN:          cmd = stop ? GB28181_PTZ_CMD_ALLSTOP_PTZ : GB28181_PTZ_CMD_DOWN; param2 = speed;       break;
    case PTZ_CMD_LEFT:          cmd = stop ? GB28181_PTZ_CMD_ALLSTOP_PTZ : GB28181_PTZ_CMD_LEFT;  param1 = speed;      break;
    case PTZ_CMD_RIGHT:         cmd = stop ? GB28181_PTZ_CMD_ALLSTOP_PTZ : GB28181_PTZ_CMD_RIGHT; param1 = speed;       break;
    case PTZ_CMD_UPLEFT:        cmd = stop ? GB28181_PTZ_CMD_ALLSTOP_PTZ : GB28181_PTZ_CMD_UPLEFT;  param2 = speed;       break;
    case PTZ_CMD_UPRIGHT:       cmd = stop ? GB28181_PTZ_CMD_ALLSTOP_PTZ : GB28181_PTZ_CMD_UPRIGHT; param2 = speed;         break;
    case PTZ_CMD_DOWNLEFT:      cmd = stop ? GB28181_PTZ_CMD_ALLSTOP_PTZ : GB28181_PTZ_CMD_DOWNLEFT;  param2 = speed;      break;
    case PTZ_CMD_DOWNRIGHT:     cmd = stop ? GB28181_PTZ_CMD_ALLSTOP_PTZ : GB28181_PTZ_CMD_DOWNRIGHT; param2 = speed;        break;
    case PTZ_CMD_ZOOMIN:        cmd = stop ? GB28181_PTZ_CMD_ALLSTOP_LENS : GB28181_PTZ_CMD_ZOOMIN; param3 = speed;       break;
    case PTZ_CMD_ZOOMOUT:       cmd = stop ? GB28181_PTZ_CMD_ALLSTOP_LENS : GB28181_PTZ_CMD_ZOOMOUT; param3 = speed;        break;
    case PTZ_CMD_FOCUSNEAR:     cmd = stop ? GB28181_PTZ_CMD_ALLSTOP_LENS : GB28181_PTZ_CMD_FOCUSNEAR; param2 = speed;        break;
    case PTZ_CMD_FOCUSFAR:      cmd = stop ? GB28181_PTZ_CMD_ALLSTOP_LENS : GB28181_PTZ_CMD_FOCUSFAR; param2 = speed;        break;
    case PTZ_CMD_IRISOPEN:      cmd = stop ? GB28181_PTZ_CMD_ALLSTOP_LENS : GB28181_PTZ_CMD_IRISOPEN; param1 = speed;        break;
    case PTZ_CMD_IRISCLOSE:     cmd = stop ? GB28181_PTZ_CMD_ALLSTOP_LENS : GB28181_PTZ_CMD_IRISCLOSE; param1 = speed;       break;
    case PTZ_CMD_SETPRESET:     cmd = GB28181_PTZ_CMD_SETPRESET; param2 = preset; break;
    case PTZ_CMD_CLEARPRESET:   cmd = GB28181_PTZ_CMD_CLEARPRESET; param2 = preset; break;
    case PTZ_CMD_CALLPRESET:    cmd = GB28181_PTZ_CMD_CALLPRESET; param2 = preset; break;
    default:
        break;
    }
    int ret = ((GB28181_SipUA*)(m_sipua))->ctrl_camera(systm->addrcode(), "", 0, m_addrcode, m_ip, m_sport, addrcode, cmd, param1, param2, param3);

    return ret;
}

int       
GB28181Pu::queryResource(string addrcode, int fromIndex, int toIndex, string &json)
{
    int Ret = ((GB28181_SipUA *)m_sipua)->query_resource(systm->addrcode(), "", m_sipua->port(), m_addrcode, m_ip, m_sport, m_addrcode, json, 0, fromIndex, toIndex);
    return Ret;
}


void                
GB28181Pu::heartBeatLoop()
{
    while (m_thread.active())
    {
        sys->sleep(10);
        uint64_t tmNowTime = sys->currentTimeMillis();

        //Heart beat
        if (tmNowTime - m_tmPrevTime >= 30 * 1000)
        {
            if (tmNowTime - m_aliveTime>120 * 1000)
            {
                m_login = false;
            }
            else
            {
                m_login = true;
            }
            m_tmPrevTime = tmNowTime;
        }
    }
}

int
GB28181Pu::queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode)
{
    int ret = ((GB28181_SipUA *)(m_sipua))->query_record(systm->addrcode(), m_addrcode, m_sipua->port(), m_addrcode, m_ip,
        m_sport, addrcode, sys->datetime2int(start), sys->datetime2int(end), type, json);

    if (m_clienQueryRecord_cb)
    {
        m_clienQueryRecord_cb(ret, this->protocol(), json, m_clientUserdata);
    }
    return ret;
}

void
GB28181Pu::onTimer()
{
    Pu::onTimer();
}

void                
GB28181Pu::UpdateAliveTime()
{
    m_aliveTime = sys->currentTimeMillis();
}

void *
GB28181Pu::threadProcEx(ThreadInfo *info)
{
    GB28181Pu *pu = (GB28181Pu*)info->data;
    switch (info->id)
    {
    case T_HEARTBEAT:
        pu->heartBeatLoop();
        break;
    default:
        break;
    }
    return NULL;
}


