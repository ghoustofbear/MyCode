#include "rtspserver.h"
#include "gw_rtspsession.h"
#include <curl/curl.h>
#include "bufferev.h"

RTSPServer::RTSPServer(int proto, string bindIp, int port)
    : Server(IPPROTO_TCP, bindIp, port)
{
    curl_global_init(CURL_GLOBAL_ALL);
}

RTSPServer::~RTSPServer()
{
    stop();
}

int
RTSPServer::start()
{
    int result = 0;
    result += Sevent::start();
    result += listen(m_bindIp, m_listenPort);

    return -1;
}

void
RTSPServer::stop()
{
    Sevent::stop();
}

void
RTSPServer::onNewConnect(string srcip, int srcport, string destip, int destport , int fd)
{
    Bufferev *bev = new Bufferev(m_evbase);
    RTSPSession *sess = new GWRTSPSession();
    if (!sess)
    {
        return;
    }

    if (bev->open(fd, true, true, 5, 5) < 0)
    {
        delete sess;
        delete bev;
        return;
    }

    bev->addBuffevListener(sess);
    sess->setbev(bev);

    {
        AutoLock locker(tlist<Bufferev *>::_mtx);
        list<Bufferev *>::push_back(bev);
    }

    {
        AutoLock locker(tlist<RTSPSession *>::_mtx);
        list<RTSPSession *>::push_back(sess);
    }
}

void
RTSPServer::onTimer()
{
    AutoLock locker(tlist<RTSPSession *>::_mtx);

    for (tlist<RTSPSession*>::iterator it = tlist<RTSPSession*>::begin(); it != tlist<RTSPSession*>::end(); )
    {
        if (!(*it)->active())
        {
            delete (*it);
            list<RTSPSession *>::iterator ti = it++;
            list<RTSPSession *>::erase(ti);
        }
        else
        {
            it++;
        }
    }
}
