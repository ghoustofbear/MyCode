#include "sevent.h"
#include "bufferev.h"
#include "tls_error.h"
#include "event2/listener.h"
#include "event2/thread.h"


void*
eventry(ThreadInfo *arg)
{
    Sevent * evs = (Sevent*)arg->data;

    evs->evloop();

    return NULL;
}

void
timercb(evutil_socket_t fd, short event, void *arg)
{
    Sevent * sevt = (Sevent *)arg;
    if (!sevt->active() || !sevt->m_evtrd.active())
    {
        {
            /* NOTE: the bufferevent_free operation can only be called in the libevent master thread, see evtimer, bufferevent_free */
            AutoLock locker(sevt->_mtx);
            for (list<Bufferev *>::const_iterator i = sevt->begin(); i != sevt->end(); ++i)
            {
                (*i)->close();
            }
        }

        event_base_loopbreak(sevt->m_evbase);
    }

    sevt->evtimer();
	//sys->sleep(10);
}

void
listencb(struct evconnlistener *listener, evutil_socket_t fd, struct sockaddr *sa, int socklen, void *arg)
{
    Sevent *svt = (Sevent *)arg;

    sockaddr_in * srcsin  = (sockaddr_in *)sa;
    string        srcip   = inet_ntoa(srcsin->sin_addr);
    int           srcport = ntohs(srcsin->sin_port);

    sockaddr      addr;
    socklen_t     addrlen = sizeof(addr);
    sockaddr_in * destsin = NULL;

    memset(&addr, 0, sizeof(addr));

    if (0 == getsockname(fd, &addr, &addrlen))
    {
        if (addr.sa_family == AF_INET)
        {
            destsin = (sockaddr_in*)&addr;
        }
    }

    string destip   = "";
    int    destport = 0;
    if (destsin)
    {
        destip   = inet_ntoa(destsin->sin_addr);
        destport = ntohs(destsin->sin_port);
    }

    //LOGI_MSV("new connection srcip: %s, srcport: %d, destip: %s, destport: %d, fd: %d",srcip.c_str(), srcport, destip.c_str(), destport, fd);
             

    svt->onNewConnect(srcip, srcport, destip, destport, fd);
}

Sevent::Sevent()
{
    m_active   = true;
    m_evbase   = NULL;
    m_listener = NULL;

#ifdef USING_EVTHREAD
#ifdef _WIN32
    evthread_use_windows_threads();
#else
    evthread_use_pthreads();
#endif
#endif

#if defined(_WIN32) && defined(USING_OICP)
    struct event_config* cfg = event_config_new();
    event_config_set_flag(cfg, EVENT_BASE_FLAG_STARTUP_IOCP);

    /* SYSTEM_INFO si; */
    /* GetSystemInfo(&si); */
    /* event_config_set_num_cpus_hint(cfg, si.dwNumberOfProcessors); */

    m_evbase = event_base_new_with_config(cfg);
    event_config_free(cfg);
#else
    m_evbase = event_base_new();
#endif

    if (!m_evbase)
    {
        LOGF_MSV("event_base_new fail.");
    }
}

Sevent::~Sevent()
{
    AutoLock locker(_mtx);
    for (list<Bufferev *>::const_iterator i = begin(); i != end(); ++i)
    {
        delete (*i);
    }

    clear();
}

int
Sevent::start()
{
    m_evtrd.setEntry(eventry, this, 0);
    return m_evtrd.start();
}

void
Sevent::stop()
{
    {
        /* NOTE: the free operation can only be called in the libevent master thread, see evtimer, bufferevent_free */
        AutoLock locker(_mtx);
        for (list<Bufferev *>::const_iterator i = begin(); i != end(); ++i)
        {
            (*i)->close();
        }
    }

    m_active = false;
    m_evtrd.stop();
}

bool
Sevent::active()
{
    return m_active;
}

int
Sevent::listen(string bindip, int port)
{
    if (m_listener)
    {
        LOGW_MSV("bindip: %s, port: %d has listened.", bindip.c_str(), port);
        return -1;
    }

    struct sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port   = htons(port);

    if ("" != bindip)
    {
        sin.sin_addr.s_addr = inet_addr(bindip.c_str());
    }

    unsigned flags = LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE;
#ifdef USING_EVTHREAD
    flags |= LEV_OPT_THREADSAFE;
#endif

    m_listener = evconnlistener_new_bind(m_evbase, listencb, this, flags, -1, (struct sockaddr *)&sin, sizeof(sin));
    if (!m_listener)
    {
        LOGE_MSV("Could not create a listener at %d!", port);

        return -1;
    }

    return 0;
}

void
Sevent::addBuffevListener(Bufferev *bev, BuffevListener *listener)
{
    AutoLock locker(_mtx);
    list<Bufferev *>::const_iterator i = find(begin(), end(), bev);
    if (i == end())
    {
        return;
    }

    (*i)->addBuffevListener(listener);
}

void
Sevent::delBuffevListener(Bufferev *bev, BuffevListener *listener)
{
    AutoLock locker(_mtx);
    list<Bufferev *>::const_iterator i = find(begin(), end(), bev);
    if (i == end())
    {
        return;
    }

    (*i)->delBuffevListener(listener);
}

void
Sevent::evtimer()
{
    {
        AutoLock locker(_mtx);
        for (list<Bufferev *>::iterator i = begin(); i != end();)
        {
            if (!(*i)->active() && ((*i)->listeners() == 0))
            {
                printf("delete bev, port:%d\n", (*i)->peerport());
                delete (*i);
                list<Bufferev *>::iterator ti = i++;                
                erase(ti);
            }
            else
            {
                i++;
            }
        }
    }
}

void
Sevent::evloop()
{
    int result = -1;

    event *ev = event_new(m_evbase, -1, EV_PERSIST, timercb, this);

    struct timeval tv = { 0, 500 };
    event_add(ev, &tv);

    result = event_base_dispatch(m_evbase);
    if (result < 0)
    {
        m_active = false;

        AutoLock locker(_mtx);
        for (list<Bufferev *>::const_iterator i = begin(); i != end(); ++i)
        {
            (*i)->close();
        }
    }

    event_free(ev);

    if (m_listener)
    {
        evconnlistener_free(m_listener);
    }

    event_base_free(m_evbase);
    LOGI_MSV("event_loop exit with code %d", result);
}

void
Sevent::onTimer()
{

}
