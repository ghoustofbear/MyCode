#include "car_pu.h"
#include "global_str_def.h"
#include "socket.h"
#include "tls_error.h"

CarPu::CarPu(int type, string addrcode, string name, int chnNum, string user, string pswd, string host, int port, int domainType)
    : Pu(type, addrcode, name, chnNum, user, pswd, host)
{
    m_protocol = PROTO_TYPE_UT;
    m_port = port;
    m_domainType = domainType;

    m_sock = sys->createSocket();
    m_sock->setWriteTimeout(2000);

    m_tmPre = 0;
    m_tmNow = 0;

}

CarPu::~CarPu()
{
    if (m_sock)
    {
        delete m_sock;
        m_sock = NULL;
    }
}

int
CarPu::start()
{
    if (Pu::start() < 0)
    {
        return -1;
    }

    m_init = true;
    return 0;
}

void
CarPu::stop()
{
    m_init = false;

    logout();

    Pu::stop();
}



int
CarPu::login()
{
    if (m_login)
        return -1;

    Host host(m_ip, m_port);

    if (m_sock->open(host) < 0 ||
        m_sock->connect() < 0)
    {
        m_sock->close();
        return -1;
    }


    m_login = true;

    writeDevLog(STR_MSV_ON_LINE);

    return 0;
}


void
CarPu::logout()
{
    do
    {
        if (NULL == m_sock)
        {
            break;
        }
        m_sock->close();


    } while (0);

    if (m_login)
    {
        m_login = false;

        writeDevLog(STR_MSV_OFF_LINE);
    }
}




int
CarPu::queryRecord(int channelNo, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json)
{
    return -1;
}

int
CarPu::queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json, string usercode)
{
    return -1;
}

int
CarPu::syncTime(datetime_s &dt, const string &tzone)
{
    return -1;
}

int
CarPu::ptzCtrl(int cmd, string addrcode, int speed, int preset, bool stop)
{

    return  ptzCtrl(cmd, 0, speed, preset, stop);
}

int
CarPu::ptzCtrl(int cmd, int channelNo, int speed, int preset, bool stop)
{
    bool res = true;
    if (!m_login)
    {
        ERRE_MSV("device %s:%d is not on-line.", m_host.c_str(), m_port);
        return -1;
    }


    char sendBuf[7] = { 0 };

    sendBuf[0] = 0xFF;                          /* 起动字符 */
    sendBuf[1] = channelNo;                     /* 地址码，从设备地址来 */
    sendBuf[2] = 0;                             /* 固定 */
    sendBuf[3] = (true == stop) ? 0x0B : 0x09;  /* 操作吗 0x0B-关 0x09-开 0x07-调用 0x05-删除 0x03-保存 */
    sendBuf[4] = 0;                             /* 固定 */

    switch (cmd)
    {
    case PTZ_CMD_UP:
        sendBuf[5] = 0x06;
        break;

    case PTZ_CMD_DOWN:
        sendBuf[5] = 0x05;
        break;

    case PTZ_CMD_LEFT:
        sendBuf[5] = 0x01;
        break;

    case PTZ_CMD_RIGHT:
        sendBuf[5] = 0x02;
        break;

    case PTZ_CMD_SETPRESET:
        sendBuf[3] = 0x03;
        sendBuf[5] = preset;               /* 预置位1---6:0x01--------0x06 辅控位1---6:0x01--------0x06 */
        break;

    case PTZ_CMD_CLEARPRESET:
        sendBuf[3] = 0x05;
        sendBuf[5] = preset;
        break;

    case PTZ_CMD_CALLPRESET:
        sendBuf[3] = 0x07;
        sendBuf[5] = preset;
        break;

    default:
        break;
    };

    int i = 1;
    int sum = 0;
    for (i = 1; i < 6; i++)
    {
        sum += sendBuf[i];
    }
    sendBuf[6] = sum % 256;

    if (m_sock->write(sendBuf, 7) < 0)
    {
        logout();
        return -1;
    }

    return 0;

}


void
CarPu::onTimer()
{
    if (!m_login)
    {
        return;
    }

    char u8SendBuf[7] = { 0 };
    m_tmNow = time(NULL);

    if (m_tmNow - m_tmPre >= 5)
    {
        m_tmPre = m_tmNow;
        if (m_sock->write(u8SendBuf, 7) < 0)
        {
            logout();
        }
    }

    Pu::onTimer();
}



