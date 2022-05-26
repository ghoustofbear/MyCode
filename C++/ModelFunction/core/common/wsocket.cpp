#include "wsocket.h"
#include "tls_error.h"


WSocket::WSocket()
{

}

WSocket::~WSocket()
{
    close();
}

void WSocket::init()
{
    WORD wVersionRequested;
    WSADATA wsaData;
    int err;

    wVersionRequested = MAKEWORD(2, 2);
    err = WSAStartup(wVersionRequested, &wsaData);

    if (err != 0)
    {
        LOGF_COM("Unable to init sockets");
        abort();
    }
}

int
WSocket::open(Host &rh, int type)
{
    ASSERT(IPPROTO_TCP == type || IPPROTO_UDP == type);

    m_sock = (int)socket(AF_INET, (IPPROTO_TCP) == type ? SOCK_STREAM : SOCK_DGRAM, type);

    if (m_sock == INVALID_SOCKET)
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

    m_remoteAddr.sin_family = AF_INET;
    m_remoteAddr.sin_port = htons(m_host.port);
    m_remoteAddr.sin_addr.S_un.S_addr = htonl(m_host.ip);

    return 0;
}

void
WSocket::close()
{
    if (SOCKET_ERROR != m_sock)
    {
        ::closesocket(m_sock);
        m_sock = SOCKET_ERROR;
    }
}

int
WSocket::bind(Host &h, int type)
{
    ASSERT(IPPROTO_TCP == type || IPPROTO_UDP == type);

    struct sockaddr_in localAddr;

    if (IPPROTO_TCP == type)
    {
        if ((m_sock = (int)socket(PF_INET, SOCK_STREAM, IPPROTO_TCP)) == -1)
        {
            LOGF_COM("Can`t open socket");
        }

        setBlocking(false);
    }
    else
    {
        if ((m_sock = (int)socket(PF_INET, SOCK_DGRAM, IPPROTO_UDP)) == -1)
        {
            LOGE_COM("Can`t open socket");
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

    if (::bind(m_sock, (sockaddr *)&localAddr, sizeof(localAddr)) == SOCKET_ERROR)
    {
        LOGE_COM("Can`t bind socket");
    }

    if (IPPROTO_TCP == type)
    {
        if (::listen(m_sock, SOMAXCONN))
        {
            LOGE_COM("Can`t listen %d", WSAGetLastError());
        }
    }

    m_host = h;

    return 0;
}

int
WSocket::connect()
{
    if (::connect(m_sock, (struct sockaddr *)&m_remoteAddr, sizeof(m_remoteAddr)) == SOCKET_ERROR)
    {
        if (checkTimeout(false, true) <= 0)
        {
            return -1;
        }

        if (selfConnect() < 0)
        {
            return -1;
        }
    }

    return 0;
}

Socket *
WSocket::accept()
{
    int         fromSize = sizeof(sockaddr_in);
    sockaddr_in from;

    int conSock = (int)::accept(m_sock, (sockaddr *)&from, &fromSize);
    if (conSock ==  INVALID_SOCKET)
    {
        return NULL;
    }

    WSocket * ws = new WSocket();
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
WSocket::setReadTimeout(uint32_t ms)
{
    m_readTimeout = ms;
}

void
WSocket::setWriteTimeout(uint32_t ms)
{
    m_writeTimeout = ms;
}

int
WSocket::select(bool r, bool w, int timeoMs)
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
     * return 0 on Windows for write, and return 1 on Linux instead. If remote close the socket, select
     * will return 1 on Windows for read.
     */
    int res = ::select(NULL, &read_fds, &write_fds, NULL, tp);
    if (res == 0)
    {
        setError("[%6s] [%s] ::select time out.", "com", __FUNCTION__);

        return 0;
    }
    else if (res == SOCKET_ERROR)
    {
        ERRE_COM("::select error code: %d.",  WSAGetLastError());

        return -1;
    }

    return res;
}

int
WSocket::checkTimeout(bool r, bool w)
{
    int err = WSAGetLastError();
    if (err == WSAEWOULDBLOCK)
    {
        if (r)
        {
            return select(true, false, (int)m_readTimeout);
        }

        if (w)
        {
            return select(false, true, (int)m_writeTimeout);
        }
    }

    ERRE_COM("::select error code: %d.",  err);
    return -1;
}

void
WSocket::setBlocking(bool yes)
{
    unsigned long op = yes ? 0 : 1;
    ioctlsocket(m_sock, FIONBIO, &op);
}

int
WSocket::read(void *buf, int size)
{
    int rsize = 0;

    while (size)
    {
        int r = ::recv(m_sock, (char *)buf, size, 0);
        if (r == SOCKET_ERROR)
        {
            int ret = checkTimeout(true, false);
            if (ret <= 0)
            {
                /* timeout or error */
                return ret;
            }

            int error =  WSAGetLastError();
            if (0 != error)
            {
                ERRE_COM("::recv error code: %d.",  error);
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
WSocket::write(const void *buf, int size)
{
    int wsize = 0;

    while (size)
    {
        int r = ::send(m_sock, (char *)buf, size, 0);
        if (r == SOCKET_ERROR)
        {
            if (checkTimeout(false, true) <= 0)
            {
                /* timeout or error */
                return -1;
            }
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
WSocket::readFrom(void *buf, int size, sockaddr *sa, int *saLen)
{
    int r = 0;

    if (select(true, false, m_readTimeout) <= 0)
    {
        return -1;
    }

    r = recvfrom(m_sock, (char *)buf, size, 0, sa, saLen);
    if (r < 0)
    {
        LOGE_COM("%d", WSAGetLastError());
    }

    return r;
}

int
WSocket::writeTo(const void *buf, int size, sockaddr *sa, int saLen)
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
        LOGE_COM("%d", WSAGetLastError());
    }

    return r;
}

string
WSocket::getPeerIP()
{
    sockaddr_in sa;
    int salen = sizeof(sockaddr_in);

    if (getpeername(m_sock, (sockaddr *)&sa, &salen) < 0)
    {
        LOGE_COM("::getpeernam error code: %d", WSAGetLastError());
        return "";
    }

    return inet_ntoa(sa.sin_addr);
}

int
WSocket::getPeerPort()
{
    sockaddr_in sa;
    int salen = sizeof(sockaddr_in);

    if (getpeername(m_sock, (sockaddr *)&sa, &salen) < 0)
    {
        LOGE_COM("::getpeernam error code: %d", WSAGetLastError());
        return -1;
    }

    return ntohs(sa.sin_port);
}

//avoid to tcp self-connection
int 
WSocket::selfConnect()
{
    sockaddr_in addr_client;
    int len = sizeof(sockaddr_in);

    if (getsockname(m_sock, (struct sockaddr *)&addr_client, &len))
    {
        return -1;
    }
    if (len != sizeof(sockaddr_in))
    {
        return -1;
    }

    if (addr_client.sin_port == m_remoteAddr.sin_port &&
        addr_client.sin_addr.s_addr == m_remoteAddr.sin_addr.s_addr)
    {
        return -1;
    }

    return 0;
}