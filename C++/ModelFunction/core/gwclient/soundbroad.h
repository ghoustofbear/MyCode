#ifndef  _SOUND_BROADCAST_H_
#define  _SOUND_BROADCAST_H_

#include "rtptriggerlistener.h"
#include "c2cap.h"
#include "tlist.h"
#include "rtpsession.h"
#include "sipua.h"

typedef struct  
{
	int cid;
	int did;
}RTPSession_Param;

class SoundBroadcast : RTPTriggerListener, tlist<RTPSession_Param*>
{
public:

	friend void cap_audio(int type, void *data, int size, uint64_t pts, void *userdata);

	SoundBroadcast(SipUA *pSipUa);

	~SoundBroadcast();

	bool start(const char *json, int size);

	void stop();

	void sendcapdata(void *data, int size);

	virtual string     srcip()      { return m_pSipUa->ip(); }
	virtual int        srcport()    { return m_pSipUa->port(); }
	virtual string     destip()     { return ""; }
	virtual int        destport()   { return 0; }

	virtual void       onHeartbeat();
	virtual void       onClose();

private:
	HANDLE         m_hSound;
	SipUA        * m_pSipUa;
	RTPSessionEx * m_rtpsess;
};

#endif
