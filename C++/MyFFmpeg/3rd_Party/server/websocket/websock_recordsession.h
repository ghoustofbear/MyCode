#ifndef __WEBSOCK_RECORDSESSION_H__
#define __WEBSOCK_RECORDSESSION_H__

#include "recordsession.h"

class Client;
class WebsockRecordSession : public RecordSession
{
public:
	WebsockRecordSession(Client *client, Bufferev *bev,int download = 0);
	virtual ~WebsockRecordSession();

	virtual  void      onRead(Bufferev *bev, void *data, uint32_t size);

	virtual  void      onResponsePacket(Bufferev *bev, uint8_t * data, uint32_t inlen);

private:
	//virtual void       stop();
};

#endif