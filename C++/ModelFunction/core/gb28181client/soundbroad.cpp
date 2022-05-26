#include "soundbroad.h"
#include "rtpsession.h"
#include "frame.h"
#include "gb28181_sipua.h"
#include "preview.h"
#include "gb28181_common.h"
#define GB28181_AUDIO_SDP_FORMAT \
	"v=0\r\n" \
	"o=%s 0 0 IN IP4 %s\r\n" \
	"s=Play\r\n" \
	"c=IN IP4 %s\r\n" \
	"t=0 0\r\n" \
	"m=audio %d RTP/AVP 8\r\n" \
	"a=rtpmap:8 PCMA/8000\r\n" \
	"a=sendonly\r\n" \
	"y=%s\r\n" \
	"f=v/////a/1/8/1\r\n\r\n"

SoundBroadcast::SoundBroadcast(GB28181_SipUA *pSipUa)
{
	m_pSipUa  = pSipUa;
	m_rtpsess = new RTPSessionEx();
}

SoundBroadcast::~SoundBroadcast()
{
	stop();
}

void cap_audio_ex(int type, void *data, int size, uint64_t pts, void *userdata)
{
	SoundBroadcast *pthis = (SoundBroadcast *)userdata;
	if (pthis)
	{
		pthis->sendcapdata(data, size);
	}
}

void SoundBroadcast::sendcapdata(void *data, int size)
{
	if (m_rtpsess)
	{
		m_rtpsess->sendByFU(ENCODE_G711A, 'A', data, size, 0);
	}
}
bool SoundBroadcast::start(const char *json, int size)
{
	SHL_Json::Reader reader;
	SHL_Json::Value  rvalue;
	if (!reader.parse(json, rvalue, false))
	{
		return  false;
	}

	string strVersion = rvalue["Version"].asString();
	string strMethod = rvalue["Method"].asString();
	if ("SoundBroad" != strMethod)
	{
		return false;
	}
	SHL_Json::Value  itemlist = rvalue["Item"];

	int srcport = m_rtpsess->srcport();
	string ssrc = GenerateSSRC(m_pSipUa->addrcode());
	for (int index = 0; index < itemlist.size(); ++index)
	{
		SHL_Json::Value item = itemlist[index];
		string cameracode = item["camaddrcode"].asString();
		string tohost     = item["tohost"].asString();
		int    toport     = item["toport"].asInt();

		RTPSession_Param * parasess = new RTPSession_Param();
		if (NULL == parasess)
		{
			continue;
		}
		string resp;
		char sdp[1024] = { 0 };
		sprintf(sdp, GB28181_AUDIO_SDP_FORMAT, cameracode.c_str(), m_pSipUa->ip().c_str(), m_pSipUa->ip().c_str(),srcport, ssrc.c_str());
		int ret = m_pSipUa->invite(cameracode, tohost, toport, sdp, resp);
		if (ret < 0)
		{
			delete parasess;
			continue;
		}
		SHL_Json::Reader reader;
		SHL_Json::Value  rvalue;
		if (!reader.parse(resp, rvalue, false))
		{
			continue;
		}

		int iConId   = atoi(rvalue["cid"].asString().c_str());
		int iDialId  = atoi(rvalue["did"].asString().c_str());
		int destport = rvalue["audioport"].asInt();
		m_rtpsess->setSsrc(rvalue["ssrc"].asString());
		ret = m_rtpsess->addDestination(tohost, destport, this);
		if (ret < 0)
		{
			m_pSipUa->bye(iConId, iDialId);
			delete parasess;
			continue;
		}
		m_rtpsess->setstreamcallback(NULL, this);

		AutoLock locker(tlist<RTPSession_Param*>::_mtx);
		push_back(parasess);
	}

	//start localsound capture
	cap_audio_s encode = { 0 };
	encode.sps = 8000;
	encode.channels = 1;
	encode.bps = 16;
	encode.ssize = 320;

	m_hSound = CAP_start_audio(NULL, &encode, 4, cap_audio_ex, this);
	if (NULL == m_hSound)
	{
		for (tlist<RTPSession_Param*>::iterator it = begin(); it != end(); ++it)
		{
			m_pSipUa->bye((*it)->cid, (*it)->did);
			delete (*it);
		}
		clear();
		return false;
	}

	return true;
}

void SoundBroadcast::stop()
{
	if (NULL != m_hSound)
	{
		CAP_stop_audio(m_hSound);
		m_hSound = NULL;
	}

	{
		AutoLock locker(tlist<RTPSession_Param*>::_mtx);
		for (tlist<RTPSession_Param*>::iterator it = begin(); it != end(); ++it)
		{
			m_pSipUa->bye((*it)->cid, (*it)->did);
			delete (*it);
		}
		clear();

		if (NULL != m_rtpsess)
		{
			m_rtpsess->stop();
			delete m_rtpsess;
			m_rtpsess = NULL;
		}
	}
}

void SoundBroadcast::onHeartbeat()
{

}

void SoundBroadcast::onClose()
{

}



