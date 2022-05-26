#ifndef __ONVIF_GU_H
#define __ONVIF_GU_H
#include  "gu.h"
#include "rtsp_gu.h"


class Pu;
class OnvifrealGu : public RealGu, public RtspGu
{
	enum
	{
		T_EVENTLOOP,
		T_DATAPROLOOP,
	};
public:
	OnvifrealGu(Pu * pu, int channelNo, string subHost, int subPort, int streamType, int streamProtoType);
	OnvifrealGu(Pu *pu, string addrcode, int streamType, int stream);
	virtual ~OnvifrealGu();

	int	 make_uri_withauth(string src_uri, string username, string password, char* dest_uri, unsigned int size_dest_uri);

	virtual int        start(int offset);
	virtual void       stop();
	static void        stream_cb(const uint8_t * data, uint32_t size, uint8_t type, void *puser);

protected:
	virtual void       onTimer();

protected:
	static void      * live555_threadProc(ThreadInfo *arg);
	void               dataProloop();
	


public:
	uint64_t           m_tmRecvTime;
	Thread*             Live555loop;

private:
	TaskScheduler*     scheduler;
	UsageEnvironment*  live555_env;
	
	void             * m_psHandle;
	bool               m_bok;
	time_t             m_lastHeartTime;
	Thread            *m_trdView;
};


#endif
