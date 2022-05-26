#ifndef __LAUNCH_SERVER_H__
#define __LAUNCH_SERVER_H__
#include "server.h"
#include "sevent.h"
#include "tlist.h"
#include "pu.h"
class Task;

class LaunchServer : public tlist<Task *>, public Sevent, public Server
{
public:
	LaunchServer(string bindip, int port);
	virtual ~LaunchServer();

public:
	void               onTimer();

public:
	virtual int        start();
	virtual void       stop();
	virtual void       setPu(Pu*pu);
private:
	virtual void       onNewConnect(string srcip, int srcport, string destip, int destport, int fd);
private:
	Pu *m_pu;
};
//extern LaunchServer * launchSVR;
#endif