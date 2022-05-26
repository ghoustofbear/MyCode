#ifndef  _GW_SU_H_
#define  _GW_SU_H_

#include "su.h"
#include "thread.h"

class Pu;

class GwSu : public Su
{
public:
	enum
	{
		T_PUSH_VIDEO
	};

	typedef struct
	{
		char buf[1024];
		int  size;
	}audioframe;

	GwSu(Pu *pu, int channelNo);
	GwSu(Pu *pu, string addrcode);
	virtual ~GwSu();

	virtual int        start();
	virtual void       stop();
	virtual int        reset();

	virtual void       doVideoStream(void *stream, uint32_t size);
	virtual void       doAudioStream(void *stream, uint32_t size, int flag);
	virtual void       doStatus(int code);

protected:
	static void        stream_cb(const uint8_t * data, uint32_t size, uint8_t type, void *puser);

	void               onPushVideo();
	static void *      threadProc(ThreadInfo *info);

private:
	int                m_stream;
	string             m_strCallId;
	int                m_iConId;
	int                m_iDialId;
	string             m_strSessId;
	Thread          *  m_rtpThread;
	tlist<audioframe*> m_listPushAudio;
	tlist<audioframe*> m_listPopAudio;
};

#endif
