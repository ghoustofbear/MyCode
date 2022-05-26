#include "bufferev.h"
#include "socket.h"
#ifdef _WIN32
#include "event.h"
#else
#include "event-linux.h"
#endif


#include "sys.h"
/* #include "platform.h" */
#include "tls_error.h"
#include "buffevlistener.h"
#include <thread>
#include <chrono>

#define MAX_MEMPOOL_SIZE     (100*1024*1024)
#define MEMPOOL

void
writecb(struct bufferevent *bev, void *ctx)
{
 //   Bufferev *ev = (Bufferev *)ctx;
 //   {
 //       AutoLock lock(ev->m_streambuf._mtx);
 //       while (0 != ev->m_streambuf.size())
 //       {
 //           Bufferev::bufevent_data *data = *(ev->m_streambuf.begin());
 //           ev->write(data->stream, data->size);

 //           delete[] data->stream;
 //           delete data;
 //           ev->m_streambuf.pop_front();
 //       }
 //   }

	//if (0 != ev->m_wto)
	//{
	//	timeval tv;
	//	tv.tv_sec = ev->m_wto;
	//	tv.tv_usec = 0;

	//	bufferevent_set_timeouts(bev, NULL, &tv);
	//}

//    struct evbuffer *output = bufferevent_get_output(bev);
//    if (evbuffer_get_length(output) == 0)
//    {
//        //if (ev->selfdisconn())
//        //{
//        //    return;
//        //}
//
//
//
//#ifndef USING_EVTHREAD
//        /* NOTE: if there is no data in the evbuffer, writecb will not be called any more */
//        bufferevent_enable(bev, EV_WRITE);
//
//        /* prevent excessive consumption of CPU */
//        /* sys->sleep(1); */
//        /* std::this_thread::sleep_for(std::chrono::milliseconds(1)); */
//        /* std::this_thread::sleep_for(std::chrono::microseconds(10)); */
//        /* std::this_thread::sleep_for(std::chrono::nanoseconds(1)); */
//#endif
//    }

    //sys->sleep(1);
}

void readcb(struct bufferevent *bev, void *ctx)
{
    Bufferev *ev = (Bufferev *)ctx;

    struct evbuffer *input = bufferevent_get_input(bev);
    int    size            = evbuffer_get_length(input);
    char   data[10 * 1024] = {0};

    evbuffer_remove(input, data, size);

    ev->notifyRead(ev, data, size);

    //if (0 != ev->m_rto)
    //{
    //    timeval tv;
    //    tv.tv_sec  = ev->m_rto;
    //    tv.tv_usec = 0;

    //    bufferevent_set_timeouts(bev, &tv, NULL);
    //}
}

void
eventcb(struct bufferevent *bev, short events, void *ctx)
{
    Bufferev *ev = (Bufferev *)ctx;
    if (events & BEV_EVENT_EOF)
    {
        ev->notifyClose(ev);
        //LOGI_MSV("Connection closed. fd: %d pt:%d", ev->m_fd, ev->peerport());
    }
    else if (events & BEV_EVENT_ERROR)
    {
        ev->notifyClose(ev);
        LOGW_MSV("Got an error on the connection: %s. fd: %d pt:%d", strerror(errno), ev->m_fd, ev->peerport());
    }
    else if (events == (BEV_EVENT_TIMEOUT | BEV_EVENT_READING))
    {
        ev->notifyClose(ev);
        LOGI_MSV("Reading timeout. fd: %d, pt:%d", ev->m_fd, ev->peerport());
    }
    else if (events == (BEV_EVENT_TIMEOUT | BEV_EVENT_WRITING))
    {
        ev->notifyClose(ev);
        LOGI_MSV("Writing timeout. fd: %d, pt:%d", ev->m_fd, ev->peerport());
    }
    else
    {
        LOGW_MSV("Got an event on the connection: %d, pt:%d", events, ev->peerport());
    }

    //bufferevent_free(bev);
    //ev->m_bev    = NULL;
    ev->m_active = false;
}

Bufferev::Bufferev(event_base *evbase)
{
    m_evbase   = evbase;
    m_bev      = NULL;
    m_rto      = 0;
    m_wto      = 0;
    m_active   = true;
    m_fd       = -1;
#if 0
    m_isSetBev = false;
    m_r        = false;
    m_w        = false;
#endif
}

Bufferev::~Bufferev()
{
    //while (0 != m_streambuf.size())
    //{
    //    Bufferev::bufevent_data *data = *(m_streambuf.begin());
    //    delete[] data->stream;
    //    delete data;
    //    m_streambuf.pop_front();
    //}



    close();
    printf("~Bufferev, port:%d fd:%d %p\n", peerport(), m_fd, this);
}

int
Bufferev::open(int fd, bool r, bool w, int rto, int wto)
{
    if (-1 == fd)
    {
        return -1;
    }

    m_fd = fd;

    if (!m_evbase)
    {
        return -1;
    }

    int options = BEV_OPT_CLOSE_ON_FREE;
#ifdef USING_EVTHREAD
    options |= BEV_OPT_THREADSAFE | BEV_OPT_DEFER_CALLBACKS | BEV_OPT_UNLOCK_CALLBACKS;
#endif
    m_bev = bufferevent_socket_new(m_evbase, fd, options);


    bufferevent_setcb(m_bev, readcb, writecb, eventcb, this);

	//bufferevent_set_max_single_write(m_bev, 2*512 * 1024);
    if (r)
    {
        bufferevent_enable(m_bev, EV_READ);
    }
    else
    {
        bufferevent_disable(m_bev, EV_READ);
    }

    if (w)
    {
        bufferevent_enable(m_bev, EV_WRITE);
    }
    else
    {
        bufferevent_disable(m_bev, EV_WRITE);
    }

    m_active = true;

    m_rto = rto;
    m_wto = wto;

    return 0;
}

void
Bufferev::close()
{
    if (m_bev)
    {
        notifyClose(this);
        /* NOTE: the free operation can only be called in the libevent master thread */
        bufferevent_free(m_bev);
        m_bev = NULL;
    }
}
void
Bufferev::addBuffevListener(BuffevListener *listener)
{
    AutoLock locker(_mtx);
    list<BuffevListener *>::const_iterator i = find(begin(), end(), listener);
    if (i == end())
    {
        push_back(listener);
    }
}

void
Bufferev::delBuffevListener(BuffevListener *listener)
{
    AutoLock locker(_mtx);
    list<BuffevListener *>::iterator i = find(begin(), end(), listener);
    if (i != end())
    {
        erase(i);
    }
}

int
Bufferev::listeners()
{
    AutoLock locker(_mtx);
    return size();
}

void
Bufferev::notifyRead(Bufferev *bev, void *data, uint32_t size)
{
    AutoLock locker(_mtx);
    for (std::list<BuffevListener *>::const_iterator i = begin(); i != end(); ++i)
    {
        (*i)->onRead(bev, data, size);
    }
}

void
Bufferev::notifyWrite(Bufferev *bev)
{
    AutoLock locker(_mtx);
    for (std::list<BuffevListener *>::const_iterator i = begin(); i != end(); ++i)
    {
        (*i)->onWrite(bev);
    }
}

void
Bufferev::notifyClose(Bufferev *bev)
{
    AutoLock locker(_mtx);
    for (std::list<BuffevListener *>::const_iterator i = begin(); i != end(); ++i)
    {
        (*i)->onClose(bev);
    }
}

bool
Bufferev::active()
{
    return m_active;
}

void
Bufferev::enable(bool r, bool w)
{
    if (r)
    {
        bufferevent_enable(m_bev, EV_READ);
    }
    else
    {
        bufferevent_disable(m_bev, EV_READ);
    }

    if (w)
    {
        bufferevent_enable(m_bev, EV_WRITE);
    }
    else
    {
        bufferevent_disable(m_bev, EV_WRITE);
    }
}

int
Bufferev::write(void *stream, uint32_t size)
{
    if (!m_bev || !m_active)
    {
        return -1;
    }

    if (size > 1000*1000*2)
    {
        printf("write size:%d error\n", size);
    }


#ifdef USING_EVTHREAD
    //bufferevent_lock(m_bev);
#endif
    int result = bufferevent_write(m_bev, stream, size);
#ifdef USING_EVTHREAD
     //bufferevent_unlock(m_bev);
#endif
    return result;
}

#if 0
void
Bufferev::stop()
{
    if (m_bev)
    {
        /* NOTE: the free operation can only be called in the libevent master thread */
        bufferevent_free(m_bev);
        m_bev = NULL;
    }
}
#endif

string
Bufferev::srcip()
{
    do
    {
        if (-1 == m_fd)
        {
            ERRW_MSV("invalidate socket fd: %d", m_fd);
            break;
        }

        sockaddr      sa;
        socklen_t     salen = sizeof(sa);
        sockaddr_in * sin   = NULL;

        memset(&sa, 0, sizeof(sa));

        if (0 != getsockname(m_fd, &sa, &salen))
        {
            ERRW_MSV("getsockname err: %d", m_fd);
            break;
        }

        if (sa.sa_family == AF_INET)
        {
            sin = (sockaddr_in*)&sa;
            return inet_ntoa(sin->sin_addr);
        }
        else
        {
            ERRW_MSV("getsockname sa.sa_family: %d", sa.sa_family);
            break;
        }
    }
    while (0);

    return "0.0.0.0";
}

string
Bufferev::peerip()
{
    do
    {
        if (-1 == m_fd)
        {
            ERRW_MSV("invalidate socket fd: %d", m_fd);
            break;
        }

        sockaddr      sa;
        socklen_t     salen = sizeof(sa);
        sockaddr_in * sin = NULL;

        memset(&sa, 0, sizeof(sa));

        if (0 != getpeername(m_fd, &sa, &salen))
        {
            ERRW_MSV("getpeername err: %d", m_fd);
            break;
        }

        if (sa.sa_family == AF_INET)
        {
            sin = (sockaddr_in*)&sa;
            return inet_ntoa(sin->sin_addr);
        }
        else
        {
            ERRW_MSV("getpeername sa.sa_family: %d", sa.sa_family);
            break;
        }
    } while (0);

    return "0.0.0.0";
}

int
Bufferev::srcport()
{
    do
    {
        if (-1 == m_fd)
        {
            ERRW_MSV("invalidate socket fd: %d", m_fd);
            break;
        }

        sockaddr      sa;
        socklen_t     salen = sizeof(sa);
        sockaddr_in * sin   = NULL;

        memset(&sa, 0, sizeof(sa));

        if (0 != getsockname(m_fd, &sa, &salen))
        {
            ERRW_MSV("getsockname err: %d", m_fd);
            break;
        }

        if (sa.sa_family == AF_INET)
        {
            sin = (sockaddr_in*)&sa;
            return ntohs(sin->sin_port);
        }
        else
        {
            ERRW_MSV("getsockname sa.sa_family: %d", sa.sa_family);
            break;
        }
    }
    while (0);

    return -1;
    /* return platform->rport(); */
}

int
Bufferev::peerport()
{
    do
    {
        if (-1 == m_fd)
        {
            ERRW_MSV("invalidate socket fd: %d", m_fd);
            break;
        }

        sockaddr      sa;
        socklen_t     salen = sizeof(sa);
        sockaddr_in * sin = NULL;

        memset(&sa, 0, sizeof(sa));

        if (0 != getpeername(m_fd, &sa, &salen))
        {
            ERRW_MSV("getpeername fd:%d", m_fd);
            break;
        }

        if (sa.sa_family == AF_INET)
        {
            sin = (sockaddr_in*)&sa;
            return ntohs(sin->sin_port);
        }
        else
        {
            ERRW_MSV("getsockname sa.sa_family: %d", sa.sa_family);
            break;
        }
    } while (0);

    return -1;
}

void
Bufferev::onTimer()
{

}

bool
Bufferev::selfdisconn()
{
	AutoLock locker(_mtx);
	if (0 == size())
	{
		bufferevent_free(m_bev);
		m_bev = NULL;
		m_active = false;

		return true;
	}
	return false;
}

void
Bufferev::push(uint8_t *stream, uint32_t size)
{
#if 0
    bufevent_data *buf_stream = new bufevent_data;
    if (NULL == buf_stream)
    {
        return;
    }
    buf_stream->stream = new uint8_t[size];
    if (NULL == buf_stream->stream)
    {
        delete buf_stream;
        return;
    }

    memcpy(buf_stream->stream, stream, size);
    buf_stream->size = size;
    AutoLock lock(m_streambuf._mtx);
    m_streambuf.push_back(buf_stream);
#else
    Bufferev::write(stream, size);
#endif
}