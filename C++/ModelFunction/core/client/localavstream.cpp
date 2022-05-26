#include "localavstream.h"
#include "tls_error.h"
#include "c2cap.h"
#include "frame.h"
#include "streamlistener.h"
#include "atalksession.h"
#include "preview.h"




void LocalAVStream::capVideoCB(int type, void *data, int size, uint64_t pts, int fps, void *userdata)
{
    LocalAVStream *pThis = (LocalAVStream *)userdata;

    if (2 == type)
    {
        //264

    }
    else
    {
        //265
    }

    printf("cap_video size:%d, type:%d\n", size, type);
    //dt->notifyVideoStream(data, size, );
}



void LocalAVStream::capAudioCB(int type, void *data, int size, uint64_t pts, void *userdata)
{
    LocalAVStream *pThis = (LocalAVStream *)userdata;
    int encode = 0;
    if (0 == type)
    {
        //wav
        encode = 0;
    }
    else if (1 == type)
    {
        //g711a
        encode = ENCODE_G711A;
    }
    else if (2 == type)
    {
        //g711u
        encode = ENCODE_G711U;
    }
    //printf("cap_audio encode:%d size:%d\n", encode, size);


    //fwrite(data, size, 1, pThis->m_tmp);

    pThis->notifyAudioStream(data, size, encode, pThis->m_sps, pThis->m_channels, pThis->m_bps, pts);
}


LocalAVStream::LocalAVStream()
    :m_bps(16), m_sps(8000), m_channels(1), m_audioHandle(NULL), m_videoHandle(NULL), m_capFlag(1)
{
    //m_tmp = fopen("D:/abc/send1.pcm", "wb");
}
 
LocalAVStream::~LocalAVStream()
{
    //fclose(m_tmp);
}

int
LocalAVStream::start(int falg)
{
    m_capFlag = falg;
    if (m_capFlag)
    {
        m_thread.setEntry(threadProc, this, 0);
        m_thread.start();
    }

    return 0;
}

int
LocalAVStream::stop()
{
    if (m_capFlag)
    {
        m_thread.stop();
        stopAudio();
        {
            AutoLock lock(m_audioListener._mtx);
            std::list<ATalkSession *>::const_iterator i;
            for (i = m_audioListener.begin(); i != m_audioListener.end(); ++i)
            {
                (*i)->stop("");
                delete (*i);
            }
            m_audioListener.clear();
        }
    }
    return 0;
}

int
LocalAVStream::startAudio(int sps, int bps, int channnels)
{
    m_sps = sps;
    m_bps = bps;
    m_channels = channnels;

    if (m_audioHandle)
        return 0;

    cap_audio_s encode = { 0 };
    encode.sps = m_sps;
    encode.channels = m_channels;
    encode.bps = m_bps;
	
	#if 1
    encode.type = 0;
    encode.ssize = 320;//must be 320,or hik dualtalk will be wrong
    m_audioHandle = CAP_start_audio(NULL, &encode, 1, capAudioCB, this);
	#else
	encode.type = 1;
    encode.ssize = 960;
    m_audioHandle = CAP_start_audio(NULL, &encode, 2, capAudioCB, this);
	#endif

    if (m_audioHandle == NULL)
    {
        ERRD_SDK("CAP_start_audio start failed.");
        return -1;
    }
    else
    {
        return 0;
    }
}


int
LocalAVStream::stopAudio()
{
    if (m_audioHandle)
    {
        CAP_stop_audio(m_audioHandle);
        m_audioHandle = NULL;
    }
    return 0;
}


int
LocalAVStream::addAudioListener(ATalkSession* als)
{
    AutoLock lock(m_audioListener._mtx);
    list<ATalkSession *>::const_iterator i = find(m_audioListener.begin(), m_audioListener.end(), als);
    if (i == m_audioListener.end())
    {
        m_audioListener.push_back(als);
    }
    return 0;
}

int
LocalAVStream::delAudioListener(ATalkSession* als)
{
    AutoLock lock(m_audioListener._mtx);
    list<ATalkSession *>::iterator i = find(m_audioListener.begin(), m_audioListener.end(), als);
    if (i != m_audioListener.end())
    {
        m_audioListener.erase(i);
    }
    return 0;
}


HANDLE
LocalAVStream::addAudioTalk(Preview* view, c2_status_cb cb, void* userdata)
{
    AutoLock lock(m_audioListener._mtx);
    std::list<ATalkSession *>::const_iterator i;
    for (i = m_audioListener.begin(); i != m_audioListener.end(); ++i)
    {
        if ((*i)->isSame(view->getCu(), view->getJson()))
        {
            return (*i);
        }
    }

     
    if (i == m_audioListener.end())
    {
        HttpATalkSession *as = new HttpATalkSession(view);
        view->opensound();
        as->init(cb, userdata);
        as->start("");

        addAudioListener(as);
        return as;
    }
    return NULL;
}

void
LocalAVStream::delAudioTalk(Preview* view)
{
    AutoLock lock(m_audioListener._mtx);
    std::list<ATalkSession *>::iterator i;
    for (i = m_audioListener.begin(); i != m_audioListener.end(); ++i)
    {
        if ((*i)->isSame(view->getCu(), view->getJson()))
        {
            view->closesound();
            ATalkSession *as = (ATalkSession*)(*i);
            as->stop("");

            delete (*i);
            m_audioListener.erase(i);
            return;
        }
    }
}


void
LocalAVStream::notifyVideoStream(void *stream, uint32_t size, int encode, int type, int w, int h, int fps, uint64_t pts)
{

}

void
LocalAVStream::notifyAudioStream(void *stream, uint32_t size, int encode, int sps, int channels, int bps, uint64_t pts)
{
    AutoLock lock(m_audioListener._mtx);
    for (std::list<ATalkSession *>::const_iterator i = m_audioListener.begin(); i != m_audioListener.end(); ++i)
    {
        (*i)->onAudioStream(stream, size, encode, sps, channels, bps, pts);
    }
}


void 
LocalAVStream::timeDriver()
{
    uint64_t curr = 0;
    static uint64_t last = 0;

    while (m_thread.active())
    {
        {
            curr = sys->currentTimeMillis();

            AutoLock lock(m_audioListener._mtx);
            int lcnt = m_audioListener.size();

            if (lcnt == 0 && last == 0)
            {
                last = curr;
            }

            if (lcnt > 0)
            {
                last = 0;
                startAudio();
            }

            if (last != 0 && curr - last > 10000)
            {
                stopAudio();
            }
        }

    
        sys->sleep(1000);

    }
}


void *
LocalAVStream::threadProc(ThreadInfo *info)
{
    LocalAVStream * la = (LocalAVStream*)(info->data);

    switch (info->id)
    {
    case 0:
        la->timeDriver();
        break;

    default:
        break;
    }

    return NULL;
}


