#include "dualtalkstream.h"
#include "socket.h"
#include "tls_error.h"
#include "cu.h"
#include "frame.h"
#include "stk/stk_message_http.h"
#include "avtalk.h"

#ifdef WIN32
#include "c2cap.h"
#endif

void DualtalkStream::cap_video(int type, void *data, int size, uint64_t pts, int fps, void *userdata)
{
    DualtalkStream *dt = (DualtalkStream *)userdata;

    if (2 == type)
    {
        //264
        
    }
    else
    {
        //265 
    }

    //dt->notifyVideoStream(data, size, );
}

void DualtalkStream::cap_audio(int type, void *data, int size, uint64_t pts, void *userdata)
{
    DualtalkStream *dt = (DualtalkStream *)userdata;
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
    dt->notifyAudioStream(data, size, encode, 8000, 1, 16, pts);
}

DualtalkStream::DualtalkStream(Cu* cu)
{
    m_cu        = cu;
    m_tmcnt     = 0;
    m_capVideo  = NULL;
    m_capAudio  = NULL;
    m_status    = S_UNINIT;
}

DualtalkStream::~DualtalkStream()
{
    stop();
}

int
DualtalkStream::start(string json)
{
    if (m_dualType == DS_HELM)
    {
        startCap(true);
    }
    else
    {
        startCap(true);
    }

    //m_timeDriver.setEntry(threadProc, this, 0);
    //m_timeDriver.start();

    return 0;

}

void
DualtalkStream::stop()
{
    if (m_dualType == DS_HELM)
    {
        stopCap(true);
    }
    else
    {
        stopCap(true);
    }

    {
        AutoLock lock(m_audioListers._mtx);
        for (list<AVTalk*>::const_iterator i = m_audioListers.cbegin(); i != m_audioListers.cend(); ++i)
        {
            delete (*i);
        }
        m_audioListers.clear();
    }

    {
        AutoLock lock(m_videoListers._mtx);
        for (list<AVTalk*>::const_iterator i = m_videoListers.cbegin(); i != m_videoListers.cend(); ++i)
        {
            delete (*i);
        }
        m_videoListers.clear();
    }

    //m_timeDriver.stop();    
}

int
DualtalkStream::startCap(bool a)
{
#ifdef WIN32
    if (a)
    {
        if (m_capAudio)
            return 0;

        cap_audio_s encode = { 0 };
        encode.sps = 8000;
        encode.channels = 1;
        encode.bps = 16;
        encode.type = 1;
        //hik
        encode.ssize = 320;
        m_capAudio = CAP_start_audio(NULL, &encode, 1 << 2, cap_audio, this);

        //helmet
        //encode.ssize = 960;
        //m_capAudio = CAP_start_audio(NULL, &encode, 1<<1 , cap_audio, this);
        if (m_capAudio == NULL)
        {
            ERRD_SDK("CAP_start_audio start failed.");
            return -1;
        }
    }
    else
    {
        if (m_capVideo)
            return 0;

        cap_video_s encode = { 0 };
        m_capVideo = CAP_video_start(NULL, 0, &encode, 1 << 0x2, cap_video, this);
        if (m_capVideo == NULL)
        {
            ERRD_SDK("CAP_video_start start failed.");
            return -1;
        }
    }
#endif
    return 0;
}

void
DualtalkStream::stopCap(bool a)
{
#ifdef WIN32
    if (a)
    {
        if (m_capAudio)
        {
            CAP_stop_audio(m_capAudio);
            m_capAudio = NULL;
        }
    }
    else
    {
        if (m_capVideo)
        {
            CAP_video_stop(m_capVideo);
            m_capVideo = NULL;
        }
    }
#endif
}

HANDLE
DualtalkStream::addStreamSpeaker(string json)
{
    AVTalk* ss = NULL;
    AutoLock locker(m_audioListers._mtx);
    std::list<AVTalk *>::const_iterator i;
    for (i = m_audioListers.begin(); i != m_audioListers.end(); ++i)
    {
        if ((*i)->isSame(m_cu))
        {
            ss = *i;
            break;
        }
    }

    if (i == m_audioListers.end())
    {
        ss = new AVTalk(m_cu);
        if (0 == ss->init() && 0 == ss->start(json))
        {
            m_audioListers.push_back(ss);
            return ss;
        }
        else
        {
            delete ss;
            return NULL;
        }
            
    }

    //ss->resume();

    return 0;
}

int
DualtalkStream::delStreamSpeaker(string json)
{
    AVTalk* ss = NULL;
    AutoLock locker(m_audioListers._mtx);
    std::list<AVTalk *>::iterator i;
    for (i = m_audioListers.begin(); i != m_audioListers.end(); ++i)
    {
        if ((*i)->isSame(m_cu))
        {
            ss = *i;
            break;
        }
    }

    //ss->pause();
    //return 0;

    if (i != m_audioListers.end())
    {
        m_audioListers.erase(i);
        delete ss;

        //ss->start(json);
        return 0;
    }
    return -1;
}

void
DualtalkStream::addStreamSpeaker(AVTalk *listener, bool a, bool v)
{
    //new avtalk
    //if (listener->speakerType() == StreamSpeaker::SS_A_SPEAKER || listener->speakerType() == StreamSpeaker::SS_AV_SPEAKER)
    //{
    //    AutoLock locker(m_audioListers._mtx);
    //    list<StreamSpeaker *>::const_iterator i = find(m_audioListers.begin(), m_audioListers.end(), listener);
    //    if (i == m_audioListers.end())
    //    {
    //        m_audioListers.push_back(listener);
    //    }
    //}

    //if (listener->speakerType() == StreamSpeaker::SS_V_SPEAKER || listener->speakerType() == StreamSpeaker::SS_AV_SPEAKER)
    //{
    //    AutoLock locker(m_videoListers._mtx);
    //    list<StreamSpeaker *>::const_iterator i = find(m_videoListers.begin(), m_videoListers.end(), listener);
    //    if (i == m_videoListers.end())
    //    {
    //        m_videoListers.push_back(listener);
    //    }
    //}
}

void
DualtalkStream::delStreamSpeaker(AVTalk *listener, bool a, bool v)
{
    //if (listener->speakerType() == StreamSpeaker::SS_A_SPEAKER || listener->speakerType() == StreamSpeaker::SS_AV_SPEAKER)
    //{
    //    AutoLock locker(m_audioListers._mtx);
    //    list<StreamSpeaker *>::const_iterator i = find(m_audioListers.begin(), m_audioListers.end(), listener);
    //    if (i == m_audioListers.end())
    //    {
    //        m_audioListers.erase(i);
    //    }
    //    m_asesspts = sys->currentTimeMillis();
    //}

    //if (listener->speakerType() == StreamSpeaker::SS_V_SPEAKER || listener->speakerType() == StreamSpeaker::SS_AV_SPEAKER)
    //{
    //    AutoLock locker(m_videoListers._mtx);
    //    list<StreamSpeaker *>::const_iterator i = find(m_videoListers.begin(), m_videoListers.end(), listener);
    //    if (i == m_videoListers.end())
    //    {
    //        m_videoListers.erase(i);
    //    }
    //    m_vsesspts = sys->currentTimeMillis();
    //}
}

void
DualtalkStream::notifyVideoStream(void *stream, uint32_t size, int encode, int type, int w, int h, int fps, uint64_t pts)
{
    //AutoLock locker(m_videoListers._mtx);
    //for (std::list<AVTalk *>::const_iterator i = m_videoListers.begin(); i != m_videoListers.end(); ++i)
    //{
    //    (*i)->doStream(stream, size);
    //}
}

void
DualtalkStream::notifyAudioStream(void *stream, uint32_t size, int encode, int sps, int channels, int bps, uint64_t pts)
{
    static uint32_t    m_seq = 0;
    uint32_t dsize = size + sizeof(ut_frame_s);
//#ifdef MEMPOOL
//    uint8_t *buf = (uint8_t *)mempool->alloc(dsize);;
//#else
//    uint8_t *buf = (uint8_t *)malloc(dsize);
//#endif
//    if (!buf)
//    {
//        return;
//    }

    memset(m_aframe, 0, 1024);
    

    ut_frame_s *hdr = (ut_frame_s *)m_aframe;
    memcpy(hdr->buf, stream, size);

    hdr->tag = TAG_STD;
    hdr->type = 'A';
    hdr->encode = encode;
    hdr->channel = 0;
    hdr->encrypt = 0;

    hdr->size = size;
    hdr->pts = pts;
    hdr->time = pts;
    hdr->seq = m_seq++;

    hdr->audio.sps = sps;
    hdr->audio.channels = channels;
    hdr->audio.bps = bps;
    hdr->audio.packLen = size;

    {
        AutoLock locker(m_audioListers._mtx);
        for (std::list<AVTalk *>::const_iterator i = m_audioListers.begin(); i != m_audioListers.end(); ++i)
        {
            (*i)->doStream(m_aframe, dsize);
        }
    }
 

//#ifdef MEMPOOL
//    mempool->free(buf);
//#else
//    free(buf);
//#endif
}

void
DualtalkStream::notifyStatus(int code)
{
    //AutoLock locker(_mtx);
    //for (std::list<StreamSpeaker *>::const_iterator i = begin(); i != end(); ++i)
    //{
    //    (*i)->onStatus(code);
    //}
}



void
DualtalkStream::timerdriver()
{
    uint64_t pts = sys->currentTimeMillis();
    {
        AutoLock locker(m_audioListers._mtx);
        if (0 == m_audioListers.size())
        {
            /* delay close real stream */
            if ((pts - m_vsesspts) > 30 * 1000)
            {
                //m_status = StreamSpeaker::S_GU_CLOSED;
                //stop audio
               
            }
        }

    }

    {
        AutoLock locker(m_videoListers._mtx);
        if (0 == m_videoListers.size())
        {
            /* delay close real stream */
            if ((pts - m_vsesspts) > 30 * 1000)
            {
                //m_status = StreamSpeaker::S_GU_CLOSED;
                //stop video
                
            }
        }
    }
}

void
DualtalkStream::timedriverLoop()
{
    while (m_timeDriver.active())
    {
        //timerdriver();
        sys->sleep(50);
    }
}



void *
DualtalkStream::threadProc(ThreadInfo *info)
{
    DualtalkStream * stream = (DualtalkStream*)(info->data);

    switch (info->id)
    {
    case 0:
        stream->timedriverLoop();
        break;

    default:
        break;
    }

    return NULL;
}
