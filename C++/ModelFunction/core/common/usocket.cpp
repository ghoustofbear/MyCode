#include "usocket.h"
#include <sys/socket.h>
#include <sys/un.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <fcntl.h>
#include <errno.h>
#include "tls_error.h"

USocket::USocket()
{
    m_flags = -1;
}

USocket::~USocket()
{
    close();
}

void USocket::init()
{
}

int
USocket::open(Host &rh, int type)
{
    ASSERT(IPPROTO_TCP == type || IPPROTO_UDP == type);

    m_sock = socket(AF_INET, (IPPROTO_TCP) == type ? SOCK_STREAM : SOCK_DGRAM, type);
    ASSERT(m_sock != -1);

    if (m_sock == -1)
    {
        LOGE_COM("Can`t open socket");
        return -1;
    }

    setBlocking(false);

#ifdef DISABLE_NAGLE
    setNagle(false);
#endif

    m_host = rh;

    memset(&m_remoteAddr, 0, sizeof(m_remoteAddr));

    m_remoteAddr.sin_family      = AF_INET;
    m_remoteAddr.sin_port        = htons(m_host.port);
    m_remoteAddr.sin_addr.s_addr = htonl(m_host.ip);

    return 0;
}

void
USocket::close()
{
    if (-1 != m_sock)
    {
        ::close(m_sock);
        m_sock = -1;
    }
}

int
USocket::bind(Host &h, int type)
{
    ASSERT(IPPROTO_TCP == type || IPPROTO_UDP == type);

    struct sockaddr_in localAddr;

    if (IPPROTO_TCP == type)
    {
        if ((m_sock = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        {
            LOGF_COM("Can`t open socket");
            return -1;
        }

        setBlocking(false);
    }
    else
    {
        if ((m_sock = socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        {
            LOGE_COM("Can`t open socket");
            return -1;
        }
    }

    setReuse(true);

    memset(&localAddr, 0, sizeof(localAddr));
    localAddr.sin_family = AF_INET;
    localAddr.sin_port   = htons(h.port);

    if (h.ip)
    {
        localAddr.sin_addr.s_addr = inet_addr(h.getIPStr().c_str());
    }
    else
    {
        localAddr.sin_addr.s_addr = INADDR_ANY;
    }

    if (::bind(m_sock, (sockaddr *)&localAddr, sizeof(localAddr)) == -1)
    {
        LOGF_COM("Can`t bind socket");
        return -1;
    }

    if (IPPROTO_TCP == type)
    {
        if (::listen(m_sock, SOMAXCONN))
        {
            LOGF_COM("Can`t listen %d", errno);
            return -1;
        }
    }

    m_host = h;

    return 0;
}

int
USocket::connect()
{
    if (::connect(m_sock, (struct sockaddr *)&m_remoteAddr, sizeof(m_remoteAddr)) == -1)
    {
        if (checkTimeout(false, true) <= 0)
        {
            return -1;
        }
    }

    return 0;
}

Socket *
USocket::accept()
{
    socklen_t   fromSize = sizeof(sockaddr_in);
    sockaddr_in from;

    int conSock = ::accept(m_sock, (sockaddr *)&from, &fromSize);
    if (conSock == -1)
    {
        return NULL;
    }

    USocket * ws = new USocket();
    ws->setSocket(conSock);

    Host host(ntohl(from.sin_addr.s_addr), from.sin_port);
    ws->setHost(host);

    ws->setBlocking(false);

#ifdef DISABLE_NAGLE
    ws->setNagle(false);
#endif

    return ws;
}

void
USocket::setReadTimeout(uint32_t ms)
{
    m_readTimeout = ms;
}

void
USocket::setWriteTimeout(uint32_t ms)
{
    m_writeTimeout = ms;
}

int
USocket::select(bool r, bool w, int timeoMs)
{
    timeval timeout;
    fd_set read_fds;
    fd_set write_fds;

    timeout.tv_sec  = timeoMs / 1000;
    timeout.tv_usec = timeoMs % 1000 * 1000;

    FD_ZERO(&write_fds);

    if (w)
    {
        FD_SET(m_sock, &write_fds);
    }

    FD_ZERO(&read_fds);

    if (r)
    {
        FD_SET(m_sock, &read_fds);
    }

    timeval *tp = NULL;
    if (timeout.tv_sec || timeout.tv_usec)
    {
        tp = &timeout;
    }

    /**
     * On nonblock connect return socket select will return the number of file descriptors contained
     * in the fd_sets, that is not as Windows platform. If the socket is not reachable, select will
     * return 0 on Windows, and return 1 on Linux instead.
     */
    int res = ::select(m_sock + 1, &read_fds, &write_fds, NULL, tp);
    if (res == 0)
    {
        //throw TimeoutException();
        setError("[%3s] [%s] ::select time out.", "com", __FUNCTION__);

        return 0;
    }
    else if (res == -1)
    {
        //throw SockException("select failed.");
        ERRE_COM("::select: %s.", strerror(errno));
        return -1;
    }

    return res;
}

int
USocket::checkTimeout(bool r, bool w)
{
    if (r)
    {
        return select(true, false, (int)m_readTimeout);
    }

    if (w)
    {
        return select(false, true, (int)m_writeTimeout);
    }

    return -1;
}

void
USocket::setBlocking(bool yes)
{
    if (yes)
    {
        if (-1 != m_flags)
        {
            fcntl(m_sock, F_SETFL, m_flags);
        }
    }
    else
    {
        m_flags = fcntl(m_sock, F_GETFL, 0);
        fcntl(m_sock, F_SETFL, m_flags | O_NONBLOCK);
    }
}

int
USocket::read(void *buf, int size)
{
    int rsize = 0;

    while (size)
    {
        int r = recv(m_sock, (char *)buf, size, 0);
        if (r == -1)
        {
			int ret = checkTimeout(true, false);
            if (ret <= 0)
            {
                /* timeout or error */
                return ret;
            }

            if (EAGAIN != errno)
            {
                ERRE_COM("::recv: %s.", strerror(errno));
                return -1;
            }
        }
        else if (r == 0)
        {
            /* eof */
            return 0;
        }
        else
        {
            rsize += r;
            size  -= r;
            buf    = (char *)buf + r;
        }
    }

    return rsize;
}

int
USocket::write(const void *buf, int size)
{
    int wsize = 0;

    while (size)
    {
        int r = ::send(m_sock, (char *)buf, size, 0);
        if (r == -1)
        {
            if (checkTimeout(false, true) <= 0)
            {
                /* timeout or error */
                return -1;
            }

            ERRE_COM("::send: %s.", strerror(errno));

            return -1;
        }
        else if (r == 0)
        {
            //throw SockException("Closed on write");
            return 0;
        }

        size  -= r;
        wsize += r;
        buf    = (char *)buf + r;
    }

    return wsize;
}

int
USocket::readFrom(void *buf, int size, sockaddr *sa, int *saLen)
{
    int r = 0;

    if (select(true, false, m_readTimeout) <= 0)
    {
        return -1;
    }

    r = recvfrom(m_sock, (char *)buf, size, 0, sa, (socklen_t *)saLen);
    if (r < 0)
    {
        LOGE_COM("%d", errno);
    }

    return r;
}

int
USocket::writeTo(const void *buf, int size, sockaddr *sa, int saLen)
{
    int r = 0;

    if (!sa)
    {
        sa    = (sockaddr *)&m_remoteAddr;
        saLen = sizeof(sockaddr);
    }

    r = sendto(m_sock, (char *)buf, size, 0, sa, saLen);
    if (r < 0)
    {
        LOGE_COM("%d", errno);
    }

    return r;
}

string
USocket::getPeerIP()
{
    sockaddr_in sa;
    socklen_t salen = sizeof(sockaddr_in);

    if (getpeername(m_sock, (sockaddr *)&sa, &salen) < 0)
    {
        LOGE_COM("::getpeernam error code: %s", strerror(errno));
        return "";
    }

    return inet_ntoa(sa.sin_addr);
}

int
USocket::getPeerPort()
{
    sockaddr_in sa;
    socklen_t salen = sizeof(sockaddr_in);

    if (getpeername(m_sock, (sockaddr *)&sa, &salen) < 0)
    {
        LOGE_COM("::getpeernam error code: %s", strerror(errno));
        return -1;
    }

    return ntohs(sa.sin_port);
}
