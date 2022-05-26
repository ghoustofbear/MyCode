#include "launch_server.h"
#include "bufferev.h"
#include "tls_error.h"
#include "launch_task.h"

LaunchServer*launchSVR = NULL;
LaunchServer::LaunchServer(string bindIp, int port)
	: Server(IPPROTO_TCP, bindIp, port)
{
	m_type = SV_WEB;
}

LaunchServer::~LaunchServer()
{
	stop();
}

int
LaunchServer::start()
{
	int result = 0;
	result += listen(m_bindIp, m_listenPort);
	result += Sevent::start();

	return -1;
}

void
LaunchServer::stop()
{
	Sevent::stop();
}

void
LaunchServer::onNewConnect(string srcip, int srcport, string destip, int destport, int fd)
{
	LaunchTask *task = new LaunchTask(this);
	task->setPu(m_pu);
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


void LaunchServer::setPu(Pu*pu)
{
	m_pu = pu;
}

void
LaunchServer::onTimer()
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