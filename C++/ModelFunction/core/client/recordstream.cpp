#include "recordstream.h"
#include "socket.h"
#include "tls_error.h"
#include "cu.h"
#include "c2sdk.h"

RecordStream::RecordStream(Cu *cu)
{
    m_cu        = cu;
    m_streamPos = 0;
    m_tmcnt     = 0;
    m_streamStatus = 0;
    m_offset = 0;
}

RecordStream::~RecordStream()
{
    if (m_sock)
    {
        delete m_sock;
        m_sock = NULL;
    }
}

int
RecordStream::start(string json)
{
    m_json = json;

    m_sock = sys->createSocket();
    m_sock->setReadTimeout(5000);
    m_recvThread.setEntry(threadProc, this, 0);

    printf("start playback\n");

    m_status = S_STARTING;
    return m_recvThread.start();
}

void
RecordStream::stop()
{
    m_recvThread.stop();

    if (m_sock)
    {
        m_sock->close();
        delete m_sock;
        m_sock = NULL;
    }
}

void
RecordStream::recvLoop()
{
    while (m_recvThread.active())
    {
        /* clear to avoid recursive writing error */
        setError("");

        if (!m_cu->isLogin())
        {
            m_status = S_WAITING_LOGIN;
            //ERRD_SDK("Waiting for login.");
            onStatus(CODE_STREAM_FAIL);

            sys->sleep(200);
            continue;
        }

        if (m_sock->open(m_cu->host()) < 0 ||
                m_sock->connect() < 0)
        {
            m_sock->close();

            m_status = S_CONNECTING;
            ERRE_SDK("Connect failed.");
            onStatus(CODE_STREAM_FAIL);

            sys->sleep(1000);
            continue;
        }

        if (open() < 0)
        {
            m_sock->close();

            m_status = S_OPENING;
            ERRE_SDK("Open stream failed.");
            onStatus(CODE_STREAM_FAIL);

            sys->sleep(1000);
            continue;
        }

        m_status = S_RECVING;
        onStatus(CODE_SUCCESS);

        if (recv() < 0)
        {
            m_status = S_STARTING;

            //if (!m_cu->isLogin())
            //{
            //    /* cu exit them remote close stream  */
            //    continue;
            //}

            m_sock->close();

            ERRE_SDK("Recv stream failed.");
            onStatus(CODE_STREAM_FAIL);

            sys->sleep(1000);
        }

        if (100 == m_streamPos)
        {
            m_status = S_CLOSED;

            onStatus(CODE_STREAM_END);

            m_sock->close();
            break;
        }
    }
}

long
RecordStream::getPosition()
{
    return m_streamPos;
}

void
RecordStream::timerdriver()
{
    m_tmcnt++;
}

void *
RecordStream::threadProc(ThreadInfo *info)
{
    RecordStream * stream = (RecordStream*)(info->data);

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
