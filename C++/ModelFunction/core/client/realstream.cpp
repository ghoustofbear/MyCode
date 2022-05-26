#include "realstream.h"
#include "streamlistener.h"
#include "cu.h"
#include "socket.h"
#include "tls_error.h"
#include "frame.h"

RealStream::RealStream(Cu *cu)
{
    ASSERT(cu);

    m_status      = S_UNINIT;
    m_cu          = cu;
    m_lastViewPTS = sys->currentTimeMillis();
    m_frameSize   = 0;
    memset(m_frame, 0, sizeof(ut_frame_s));

    m_bTalking = false;

    m_json.clear();

    m_u64ViewTickTime = 0;
	m_strIvaMsg = "";
}

RealStream::~RealStream()
{
    clear();
    if (m_sock)
    {
        delete m_sock;
        m_sock = NULL;
    }
}

bool
RealStream::isSame(Cu *cu, string json)
{
    return (cu == m_cu) &&
           (0 == m_json.compare(json));
}

int
RealStream::start(string json)
{
    m_json = json;

    m_sock = sys->createSocket();
    m_sock->setReadTimeout(5000);
    m_recvThread.setEntry(threadProc, this, 0);

    m_status = S_STARTING;
    return m_recvThread.start();
}
#ifdef UNIX_DRAW_RECT
int               
RealStream::drawROI(string alarmMsg)
{
    printf("***************RealStream::drawROI ------: %0x test !!!!!\n", this);

	m_strIvaMsg = alarmMsg;

	AutoLock locker(_mtx);
	for (std::list<StreamListener *>::const_iterator i = begin(); i != end(); ++i)
	{
		(*i)->onDrawRoi(m_strIvaMsg);
	}
	return 0;
}

void
RealStream::clearROI(string strMsg)
{
	m_strIvaMsg = "";

	AutoLock locker(_mtx);
	for (std::list<StreamListener *>::const_iterator i = begin(); i != end(); ++i)
	{
        (*i)->onCtrlROI(strMsg);
	}
}
#endif

void
RealStream::stop()
{
    /**
     * You should close the receive thread before the subclass is released, because
     * the virtual function open of the subclass is called in the receive thread.
     */
    m_recvThread.stop();

    if (m_sock)
    {
        m_sock->close();
    }
}

void
RealStream::addStreamListener(StreamListener *listener)
{
    AutoLock locker(_mtx);
    push_back(listener);
}

void
RealStream::delStreamListener(StreamListener *listener)
{
    AutoLock locker(_mtx);
    list<StreamListener *>::iterator i = find(begin(), end(), listener);
    if (i != end())
    {
        erase(i);
    }

    m_lastViewPTS = sys->currentTimeMillis();
}


int
RealStream::openTalk()
{
    m_bTalking = true;


    char afre[128] = {0};

    ut_frame_s *hdr = (ut_frame_s *)afre;
    memcpy(hdr->buf, "testData", sizeof("testData"));

    hdr->tag = TAG_STARTTALK;
    hdr->type = 'A';
    hdr->encode = 7;
    hdr->channel = 0;
    hdr->encrypt = 0;

    hdr->size = sizeof("testData");
    hdr->pts = 0;
    hdr->time = 0;
    hdr->seq = 0;

    hdr->audio.sps = 8000;
    hdr->audio.channels = 1;
    hdr->audio.bps = 16;
    hdr->audio.packLen = sizeof("testData");

    m_sock->write(afre, sizeof(ut_frame_s) + sizeof("testData"));

    return 0;

    //recv response
    //memset(afre, 0, 128);
    //if (m_sock->read(afre, sizeof(ut_frame_s)) <= 0)
    //{
    //    return -1;
    //}

    //hdr = (ut_frame_s *)afre;
    //if (hdr->tag == TAG_RESPONSETALK)
    //{
    //    if (m_sock->read(hdr->buf, hdr->size) <= 0)
    //    {
    //        return -1;
    //    }
    //    else
    //    {
    //        if (hdr->buf == "Y")
    //        {
    //            return 0;
    //        }
    //        else
    //        {
    //            return -1;
    //        }
    //    }

    //}
    //else
    //{
    //    return -1;
    //}
}

void
RealStream::closeTalk()
{
    m_bTalking = false;

    char afre[128] = { 0 };

    ut_frame_s *hdr = (ut_frame_s *)afre;
    memcpy(hdr->buf, "testData", sizeof("testData"));

    hdr->tag = TAG_STOPTALK;
    hdr->type = 'A';
    hdr->encode = 7;
    hdr->channel = 0;
    hdr->encrypt = 0;

    hdr->size = sizeof("testData");
    hdr->pts = 0;
    hdr->time = 0;
    hdr->seq = 0;

    hdr->audio.sps = 8000;
    hdr->audio.channels = 1;
    hdr->audio.bps = 16;
    hdr->audio.packLen = sizeof("testData");

    int  ret = m_sock->write(afre, sizeof(ut_frame_s) + sizeof("testData"));
    if (ret != sizeof(ut_frame_s) + sizeof("testData"))
    {
    }
    else
    {
    }
}

void
RealStream::doAudioStream(void* frame, int size)
{
    if (S_RECVING == m_status)
    {
        m_sock->write(frame, size);
    }
}

int
RealStream::status()
{
    return m_status;
}

Cu *
RealStream::getCu()
{
    return m_cu;
}

string &
RealStream::getJson()
{
    return m_json;
}

void
RealStream::recvLoop()
{
    while (m_recvThread.active())
    {
        /* clear to avoid recursive writing error */
        setError("");

        if (!m_cu->isLogin())
        {
            m_status = S_WAITING_LOGIN;
            //ERRD_SDK("Waiting for login.");
            notifyStatus(CODE_STREAM_FAIL);

            sys->sleep(200);
            continue;
        }

        uint64_t u64NowTickTime = sys->currentTimeMillis();
        if (u64NowTickTime - m_u64ViewTickTime < 2 * 1000)
        {
            sys->sleep(100);
            continue;
        }

        if (m_sock->open(m_cu->host()) < 0 ||
                m_sock->connect() < 0)
        {
            m_sock->close();

            m_status = S_CONNECTING;
            ERRE_SDK("Connect failed.");
            notifyStatus(CODE_STREAM_FAIL);

            m_u64ViewTickTime = sys->currentTimeMillis();
            continue;
        }

        if (open() < 0)
        {
            m_sock->close();

            m_status = S_OPENING;
            ERRE_SDK("Open stream failed.");
            notifyStatus(CODE_STREAM_FAIL);

            m_u64ViewTickTime = sys->currentTimeMillis();
            continue;
        }

        m_status = S_RECVING;
        notifyStatus(CODE_SUCCESS);

        if (m_bTalking)
        {
            openTalk();
        }

        if (recv() < 0)
        {
            m_sock->close();

            ERRE_SDK("Recv stream failed.");
            notifyStatus(CODE_STREAM_FAIL);
			m_u64ViewTickTime = sys->currentTimeMillis();
        }
    }
}

void
RealStream::notifyStream(void *stream, uint32_t size)
{
    AutoLock locker(_mtx);
    for (std::list<StreamListener *>::const_iterator i = begin(); i != end(); ++i)
    {
        (*i)->onStream(stream, size);
    }
}

void
RealStream::notifyStatus(int code)
{
    AutoLock locker(_mtx);
    for (std::list<StreamListener *>::const_iterator i = begin(); i != end(); ++i)
    {
        (*i)->onStatus(code);
    }
}

void
RealStream::timerdriver()
{
    {
        AutoLock locker(_mtx);

        if (m_status >= S_WAITING_LOGIN &&
                0 != m_lastViewPTS)
        {
            /* delay close real stream */
            if (0 == size())
            {
                if ((sys->currentTimeMillis() - m_lastViewPTS) > DELAY_CLOSE_TIME * 1000)
                {
                    m_status = S_CLOSED;
                }
                else
                {
                    m_status = S_WATTING_CLOSE;
                }
            }
        }
    }
}

void *
RealStream::threadProc(ThreadInfo *info)
{
    RealStream * stream = (RealStream*)(info->data);

    switch (info->id)
    {
    case 0:
        stream->recvLoop();
        break;

    default:
        break;
    }

    return NULL;
}
