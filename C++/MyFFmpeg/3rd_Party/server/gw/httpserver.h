#ifndef _HTTP_SERVER_H_
#define _HTTP_SERVER_H_

#include "server.h"
#include "sevent.h"

class HttpServer : public Sevent, public Server
{
public:
	HttpServer(int proto, string bindIp, int port);
	virtual ~HttpServer();

public:
	virtual int        start();
	virtual void       stop();

protected:
	virtual void       onNewConnect(string srcip, int srcport, string destip, int destport, int fd);
private:
};

#endif