#ifndef __GWA_HTTPSERVER_H__
#define __GWA_HTTPSERVER_H__

#include "server.h"
#include "sevent.h"

class GWAHttpServer : public Sevent, public Server
{
public:
	GWAHttpServer(int proto, string bindIp, int port);

	~GWAHttpServer();

public:
	virtual int        start();
	virtual void       stop();

protected:
	virtual void       onNewConnect(string srcip, int srcport, string destip, int destport, int fd);

private:

};

#endif