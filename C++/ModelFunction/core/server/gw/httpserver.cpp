#include "httpserver.h"
#include "httpaction.h"

HttpServer::HttpServer(int proto, string bindIp, int port)
	: Server(IPPROTO_TCP, bindIp, port)
{

}

HttpServer::~HttpServer()
{
	stop();
}

int
HttpServer::start()
{
	int result = 0;
	result += Sevent::start();
	result += listen(m_bindIp, m_listenPort);

	return -1;
}

void
HttpServer::stop()
{
	Sevent::stop();
}

void
HttpServer::onNewConnect(string srcip, int srcport, string destip, int destport, int fd)
{
	HttpAction *action = new HttpAction(m_evbase);
	if (!action)
	{
		return;
	}

	if (action->open(fd, true, true, 5, 5) < 0)
	{
		delete action;
		return;
	}

	action->addBuffevListener(action);

	{
		AutoLock locker(tlist<Bufferev *>::_mtx);
		list<Bufferev *>::push_back(action);
	}
}