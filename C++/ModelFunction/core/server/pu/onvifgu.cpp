#include "gsessmgr.h"
#include "json/json.h"
#include "onvifgu.h"
#include "thread.h"
#include "session.h"
#include "system.h"
#include "gumgr.h"
#include "platform.h"
#include "PsProcessApi.h"
#include "onvifPu.h"
#include "BasicUsageEnvironment.hh"


OnvifrealGu::OnvifrealGu(Pu * pu, int channelNo, string subHost, int subPort, int streamType, int streamProtoType)
	: RealGu(pu, channelNo, subHost, subPort, streamType, streamProtoType)
{
    scheduler = BasicTaskScheduler::createNew();
	live555_env = BasicUsageEnvironment::createNew(*scheduler);
	Live555loop = new Thread();
}



OnvifrealGu::~OnvifrealGu()
{
	
	
}



int OnvifrealGu::make_uri_withauth(string src_uri,string username, string password, char* dest_uri, unsigned int size_dest_uri)
{
	int result = 0;
	unsigned int needBufSize = 0;
	memset(dest_uri, 0x00, size_dest_uri);
	needBufSize = src_uri.size() + username.size() + password.size() + 3;    // ¼ì²é»º´æÊÇ·ñ×ã¹»£¬°üÀ¨¡®:¡¯ºÍ¡®@¡¯ºÍ×Ö·û´®½áÊø·û
	if (size_dest_uri < needBufSize) {
		result = -1;
		goto EXIT;
	}
	char *p = strstr((char *)src_uri.c_str(), "//");
	if (NULL == p) {
			result = -1;
			goto EXIT;
	}
	p += 2;

	memcpy(dest_uri, src_uri.c_str(), p - src_uri.c_str());
	sprintf(dest_uri + strlen(dest_uri), "%s:%s@", username.c_str(), password.c_str());
	strcat(dest_uri, p);

EXIT:

	return result;
}
int OnvifrealGu::start(int offset)
{
	onvifPu * pPu = (onvifPu *)m_pu;
	OnvifClientDevice* clientdevice = pPu->Getonvifclient();
	_trt__GetProfilesResponse* profiles = pPu->Getprofiles();
	OnvifClientMedia* media = pPu->Getonvifclientmedia();
	
	tt__StreamSetup StreamSetup;
	_trt__GetStreamUriResponse StreamUriResponse;
		
	StreamSetup.Transport =new(struct tt__Transport);
	StreamSetup.__anyAttribute = NULL;
	StreamSetup.Stream = tt__StreamType__RTP_Unicast;
	StreamSetup.Transport->Protocol = tt__TransportProtocol__UDP;
	StreamSetup.Transport->Tunnel = NULL;
	
	if ((profiles->Profiles.size() >= this->m_channelNo) && (media) && (profiles))
	{
		media->GetStreamUri(StreamUriResponse, StreamSetup, profiles->Profiles[this->m_channelNo]->token);
	}
	else
	{
		if (StreamSetup.Transport)
			delete StreamSetup.Transport;
		return -1;
	}
	if (StreamSetup.Transport)
			delete StreamSetup.Transport;
	char authreplayUrl[256];
	make_uri_withauth(StreamUriResponse.MediaUri->Uri, pPu->userName(), pPu->passWord(), authreplayUrl, 256);
	bool res = this->createMyRTSPClient(authreplayUrl, live555_env, stream_cb,this);

	if (res == true)
	{
		RtspGu::startGu(offset);
		Live555loop->setEntry(live555_threadProc, this, T_DATAPROLOOP);
		int result = Live555loop->start();
		realGuMgr->addGu(this);
		return result;
	}
	return -1;
}


void*
OnvifrealGu::live555_threadProc(ThreadInfo *arg)
{
	OnvifrealGu * ua = (OnvifrealGu *)(arg->data);

	switch (arg->id)
	{
	case T_DATAPROLOOP:
		ua->dataProloop();
		break;
	default:
		break;
	}

	return NULL;
}

void
OnvifrealGu::dataProloop()
{
	while (Live555loop->active())
	{
		if (live555_env)
			live555_env->taskScheduler().doEventLoop();
	}
}
static FILE* test = NULL;

void
OnvifrealGu::stream_cb(const uint8_t * data, uint32_t size, uint8_t type, void *puser)
{
	
	if (!data || size <= 0 || !puser)
	{
		return;
	}
	OnvifrealGu *gu = (OnvifrealGu *)puser;

	switch (type)
	{
	case 1:
		LOGT_MSV("type: %02x%02x%02x%02x%02x", data[0], data[1], data[2], data[3], data[4]);
		gu->getH264Resolution((uint8_t *)data + 4, size - 4, gu->m_s32Height, gu->m_s32Width);
		gu->notifyVideoStream((uint8_t *)data, size, ENCODE_H264, 'I', gu->m_s32Width, gu->m_s32Height, 25, sys->currentTimeMillis());
		break;

	case 3:
		LOGT_MSV("type: %02x%02x%02x%02x%02x", data[0], data[1], data[2], data[3], data[4]);
		gu->notifyVideoStream((uint8_t *)data, size, ENCODE_H264, 'P', gu->m_s32Width, gu->m_s32Height, 25, sys->currentTimeMillis());
		break;

	case 10:
		gu->notifyAudioStream((uint8_t *)data, size, ENCODE_G711A, 8000, 1, 16, sys->currentTimeMillis());
		break;

	default:
		return;
		break;
	}
	gu->traffic(size);
	gu->m_lastpts = sys->currentTimeMillis();
}



void
OnvifrealGu::stop()
{
	realGuMgr->delGu(this);
	Live555loop->stop();
}

void
OnvifrealGu::onTimer()
{
	Gu::onTimer();
}