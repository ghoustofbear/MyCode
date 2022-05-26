#ifndef __OPENAPI_SERVER_H__
#define __OPENAPI_SERVER_H__
#include "server.h"
#include "sevent.h"
#include "tlist.h"

class Task;

class OpenAPIServer : public tlist<Task *>, public Sevent, public Server
{
public:
	OpenAPIServer(string bindip, int port);
	virtual ~OpenAPIServer();

public:
	void               onTimer();

public:
	virtual int        start();
	virtual void       stop();

private:
	virtual void       onNewConnect(string srcip, int srcport, string destip, int destport, int fd);
};
extern OpenAPIServer * openApiSVR;
#endif