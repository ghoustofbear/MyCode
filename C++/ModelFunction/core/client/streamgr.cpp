#include "streamgr.h"
#include "tls_error.h"
#include "cu.h"
#include "http_realstream.h"
#include "tls_error.h"
#include "c2cap.h"
#include "frame.h"
#include "asl.h"


#if 0
extern "C"
{
    typedef long(*_cap_set_error)(const char* fmt, ...);
    typedef const char *(*_cap_get_error)();
    typedef long(*_cap_enum_device)(int *count, cap_device_s *devices);
    typedef HANDLE(*_cap_video_start)(cap_device_s *device, HWND wnd, cap_video_s *encode, uint32_t mode, cap_video_cb video_cb, void *userdata);
    typedef long(*_cap_video_stop)(HANDLE hp);
    typedef HANDLE(*_cap_start_audio)(cap_device_s *device, cap_audio_s *encode, uint32_t mode, cap_audio_cb video_cb, void *userdata);
    typedef long(*_cap_stop_audio)(HANDLE hp);
}

ASL_LIBRARY_BEGIN(C2Cap)
ASL_SYMBOL_OPTIONAL(_cap_set_error,   CAP_set_error)
ASL_SYMBOL_OPTIONAL(_cap_get_error,   CAP_get_error)
ASL_SYMBOL_EXPLICIT(_cap_enum_device, CAP_enum_device)
ASL_SYMBOL_OPTIONAL(_cap_video_start, CAP_video_start)
ASL_SYMBOL_OPTIONAL(_cap_video_stop,  CAP_video_stop)
ASL_SYMBOL_OPTIONAL(_cap_start_audio, CAP_start_audio)
ASL_SYMBOL_OPTIONAL(_cap_stop_audio,  CAP_stop_audio)
ASL_LIBRARY_END()

static C2Cap c2cap;
#endif

uint64_t  TalkStream::m_seq = 0;
void TalkStream::cap_audio(int type, void *data, int size, uint64_t pts, void *userdata)
{
    TalkStream *dt = (TalkStream *)userdata;
    int encode = 0;
    if (0 == type)  //pcm
    {
        //pcm
        encode = 0;
        dt->notifyAudioStream(data, size, pts);
    }
}


TalkStream::TalkStream()
{
    m_capAudio = NULL;
    m_aframe = new char[1024];
}

TalkStream::~TalkStream()
{
    stop();
    if (m_aframe)
    {
        delete[] m_aframe;
        m_aframe = NULL;
    }
}
int
TalkStream::start()
{
    if (m_capAudio)
    {
        return 0;
    }

    cap_audio_s encode = { 0 };
    encode.sps = 8000;
    encode.channels = 1;
    encode.bps = 16;
    encode.type = 0;
    encode.ssize = 320;
    m_capAudio = CAP_start_audio(NULL, &encode, 0x1, cap_audio, this);

    if (m_capAudio == NULL)
    {
        ERRD_SDK("CAP_start_audio start failed.");
        return -1;
    }
    else
    {
        return 0;
    }
}

void
TalkStream::stop()
{
    if (m_capAudio)
    {
        CAP_stop_audio(m_capAudio);
        m_capAudio = NULL;
    }
}

bool
TalkStream::isCapture()
{
    return m_capAudio == NULL ? false : true;
}

int
TalkStream::addTalkStream(RealStream *listener)
{
    if (!isCapture())
    {
        if (0 != start())
        {
            return -1;
        }
    }
    AutoLock lock(m_audios._mtx);

    list<RealStream *>::const_iterator i = find(m_audios.cbegin(), m_audios.cend(), listener);
    if (i == m_audios.cend())
    {
        listener->openTalk();
        m_audios.push_back(listener);        
    }

    return 0;
}

void
TalkStream::delTalkStream(RealStream *listener)
{
    AutoLock lock(m_audios._mtx);
    list<RealStream *>::iterator i = find(m_audios.begin(), m_audios.end(), listener);
    if (i != m_audios.end())
    {
        m_audios.erase(i);
        listener->closeTalk();
    }

    if (m_audios.size() == 0)
    {
        stop();
    }
}

void
TalkStream::buildFrame(uint32_t tag, void* data, int size, uint64_t pts)
{
    memset(m_aframe, 0, 1024);
    ut_frame_s *hdr = (ut_frame_s *)m_aframe;

    //ASSERT(size <= 1024 - 40);

    memcpy(hdr->buf, (char*)data, size);

    hdr->tag = tag;
    hdr->type = 'A';
    hdr->encode = 7;
    hdr->channel = 0;
    hdr->encrypt = 0;

    hdr->size = size;
    hdr->pts = pts;
    hdr->time = pts;
    hdr->seq = m_seq++;

    hdr->audio.sps = 8000;
    hdr->audio.channels = 1;
    hdr->audio.bps = 16;
    hdr->audio.packLen = size;
}

void
TalkStream::notifyAudioStream(void* data, int size, uint64_t pts)
{
    
    buildFrame(TAG_STD, data, size, pts);

    AutoLock lock(m_audios._mtx);
    for (list<RealStream *>::const_iterator i = m_audios.cbegin(); i != m_audios.cend(); ++i)
    {
        (*i)->doAudioStream(m_aframe, size + sizeof(ut_frame_s));
    }
}

///////////////////////////////////////////////////////////////////
StreamMgr::StreamMgr()
{

}

StreamMgr::~StreamMgr()
{

}

int
StreamMgr::start()
{
    int result = 0;
    m_collecThread.setEntry(threadEntry, this, T_COLLECTOR);
    m_timerThread.setEntry(threadEntry, this, T_TIMER);

    result += m_collecThread.start();
    result += m_timerThread.start();

    return result;
}

void
StreamMgr::stop()
{
    m_timerThread.stop();
    m_collecThread.stop();
}
#ifdef UNIX_DRAW_RECT
void
StreamMgr::drawRoi(string devIp, int devPort, int chnNo, string alarmStr, int dwtype)
{
	if (ALARM_STARTED_IVA != dwtype)
	{
		return;
	}

	AutoLock locker(_mtx);

	list<RealStream *>::const_iterator it;
	for (it = begin(); it != end(); ++it)
	{
        if ((chnNo == ((*it)->getChannelNo())) &&
			(devIp == (*it)->getDeviceIp()) &&
			(devPort == (*it)->getDevicePort()))
		{
			(*it)->drawROI(alarmStr);
		}
	}
}

void 
StreamMgr::clearRoi(string msg, string devIp, int devPort, int chnNo)
{
	AutoLock locker(_mtx);

	list<RealStream *>::const_iterator it;
	for (it = begin(); it != end(); ++it)
	{
		if ((chnNo == (*it)->getChannelNo()) &&
			(devIp == (*it)->getDeviceIp()) &&
			(devPort == (*it)->getDevicePort()))
		{
            (*it)->clearROI(msg);
		}
	}
}
#endif
int
StreamMgr::addRealStream(Cu *cu, string json, StreamListener *listener)
{
    RealStream *stream = NULL;

    AutoLock locker(_mtx);

    list<RealStream *>::const_iterator i;
    for (i = begin(); i != end(); ++i)
    {
        if ((*i)->isSame(cu, json))
        {
            stream = *i;
            break;
        }
    }

    if (i == end())
    {
        if (Cu::P_HTTP == cu->protocol())
        {
            stream = new HttpRealStream(cu);
        }
        else
        {
            ERRE_SDK("protocol %d is not implement!", cu->protocol());
            return -1;
        }

        if (!stream)
        {
            ERRE_SDK("new HttpRealStream(cu) failed");
            return -1;
        }

        if (stream->start(json) < 0)
        {
            ERRE_SDK("HttpRealStream(cu).start() failed");
            delete stream;
            return -1;
        }

        push_back(stream);
    }

    stream->addStreamListener(listener);

    return 0;
}

void
StreamMgr::delRealStream(Cu *cu, string json, StreamListener *listener)
{
    AutoLock locker(_mtx);

    for (list<RealStream *>::const_iterator i = begin(); i != end(); ++i)
    {
        if (cu == (*i)->getCu() &&
                0 == (*i)->getJson().compare(json))
        {
            (*i)->delStreamListener(listener);
            break;
        }
    }
}


void
StreamMgr::release(Cu *cu)
{
    AutoLock locker(_mtx);

    for (list<RealStream *>::iterator i = begin();  i != end();)
    {
        if (cu == (*i)->getCu())
        {
            (*i)->stop();
            delete (*i);
            list<RealStream *>::iterator ti = i++;
            erase(ti);
        }
        else
        {
            i++;
        }
    }
}

int
StreamMgr::collector()
{
    uint64_t cnt = 0;
    uint32_t len = 50;

    while (m_collecThread.active())
    {
        if (empty() && (0 == cnt % 5000))
        {
            /* LOGT_SDK("RealStream list is empty!"); */
        }
        else
        {
            AutoLock locker(_mtx);

            list<RealStream *>::iterator i;
            for (i = begin();  i != end();)
            {
                if ((*i)->status() == RealStream::S_CLOSED && (*i)->size() == 0)
                {
                    (*i)->stop();
                    delete (*i);
                    list<RealStream *>::iterator ti = i++;
                    erase(ti);
                }
                else
                {
                    i++;
                }
            }
        }

        sys->sleep(len);
        cnt += len;
    }

    return 0;
}

void
StreamMgr::timerdriver()
{
    while (m_timerThread.active())
    {
        {
            AutoLock locker(_mtx);

            for (list<RealStream *>::const_iterator i = begin(); i != end(); ++i)
            {
                (*i)->timerdriver();
            }
        }

        sys->sleep(50);
    }
}

void *
StreamMgr::threadEntry(ThreadInfo *info)
{
    StreamMgr * mgr = (StreamMgr*)(info->data);

    switch (info->id)
    {
    case T_COLLECTOR:
        mgr->collector();
        break;

    case T_TIMER:
        mgr->timerdriver();
        break;

    default:
        break;
    }

    return NULL;
}
