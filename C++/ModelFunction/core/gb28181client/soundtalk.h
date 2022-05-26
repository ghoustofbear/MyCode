#ifndef  _SOUND_TALK_H_
#define  _SOUND_TALK_H_

#include "rtptriggerlistener.h"
#include "c2cap.h"
#include "gb28181_sipua.h"
class preview;
class RTPSessionEx;

class SoundTalk : public RTPTriggerListener
{
public:

	friend void cap_audio(int type, void *data, int size, uint64_t pts, void *userdata);

	SoundTalk(GB28181_SipUA *pSipUa, preview * stream);

	~SoundTalk();

	bool  start(const char *tohost, int toport);

	void  stop();

	virtual string     srcip()      { return m_pSipUa->ip(); }
	virtual int        srcport()    { return m_pSipUa->port(); }
	virtual string     destip()     { return ""; }
	virtual int        destport()   { return 0; }

	virtual void       onHeartbeat();
	virtual void       onClose();

protected:
	static void        stream_cb(const uint8_t * data, uint32_t size, uint8_t type, void *puser);

private:
	GB28181_SipUA* m_pSipUa;
	preview      * m_preview;
	RTPSessionEx * m_rtpSession;
	int            m_iConId;
	int            m_iDialId;
	HANDLE         m_hSound;
};

#endif
