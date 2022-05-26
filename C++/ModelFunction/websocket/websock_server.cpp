#include "websock_server.h"
#include "bufferev.h"
#include "tls_error.h"
#include "websock_task.h"
#include "sha.h"
#include "base64_ex.h"
#include <regex>

WebSocketServer::WebSocketServer(string bindIp, int port)
    : Server(IPPROTO_TCP, bindIp, port)
{
    m_type = SV_WEBSOCK;
}

WebSocketServer::~WebSocketServer()
{
    stop();
}

int
WebSocketServer::start()
{
    int result = 0;
    result += listen(m_bindIp, m_listenPort);
    result += Sevent::start();

    return -1;
}

void
WebSocketServer::stop()
{
    Sevent::stop();
}

void
WebSocketServer::onNewConnect(string srcip, int srcport, string destip, int destport, int fd)
{
    Task *task = new WebSocketTask();

    Bufferev *bev = new Bufferev(m_evbase);
    bev->addBuffevListener(task);

    if (bev->open(fd, true, true, 5, 5) < 0)
    {
        LOGE_MSV("Bufferev open failed. %s", getError());
        bev->delBuffevListener(task);
        delete bev;
        delete task;
        return;
    }
  
    task->setbev(bev);

    {
        AutoLock locker(tlist<Bufferev *>::_mtx);
        list<Bufferev *>::push_back(bev);
    }

    {
        AutoLock locker(tlist<Task *>::_mtx);
        list<Task *>::push_back(task);
    }
}

void
WebSocketServer::onTimer()
{
    {
        AutoLock locker(tlist<Task *>::_mtx);

        for (list<Task *>::iterator i = list<Task *>::begin(); i != list<Task *>::end();)
        {
            if (Task::S_CLOSED == (*i)->status())
            {
                (*i)->stop();
                delete (*i);
                list<Task *>::iterator ti = i++;
                list<Task *>::erase(ti);
            }
            else
            {
                i++;
            }
        }
    }
}
