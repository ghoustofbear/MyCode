#include "priv_server.h"
#include "bufferev.h"
#include "tls_error.h"
#include "priv_task.h"

PrivServer::PrivServer(string bindIp, int port)
    : Server(IPPROTO_TCP, bindIp, port)
{
    m_type = SV_PRIV_NEW;
}

PrivServer::~PrivServer()
{
    stop();
}

int
PrivServer::start()
{
    int result = 0;
    result += listen(m_bindIp, m_listenPort);
    result += Sevent::start();

    return -1;
}

void
PrivServer::stop()
{
    Sevent::stop();
}

void
PrivServer::onNewConnect(string srcip, int srcport, string destip, int destport, int fd)
{
    do
    {
        Bufferev *bev = new Bufferev(m_evbase);
        if (NULL == bev)
        {
            break;
        }

		{
			AutoLock locker(tlist<Bufferev *>::_mtx);
			list<Bufferev *>::push_back(bev);
		}

        Task *task = new PrivTask(bev);
        assert(NULL != task);
       {
            AutoLock locker(tlist<Task *>::_mtx);
            list<Task *>::push_back(task);
       }

       if (bev->open(fd, true, true, 5, 5) < 0)
       {
           LOGE_MSV("Bufferev open failed. srcip:%s srcport:%d destip:%s destport:%d", srcip.c_str(), srcport, destip.c_str(), destport);
           delete bev;
           break;
       }
    } while (0);  
}

void
PrivServer::onTimer()
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
