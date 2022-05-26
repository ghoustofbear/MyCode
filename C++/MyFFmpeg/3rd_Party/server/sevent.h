/**
 * @file      sevent.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     private event from libevnet
 * @author    caiwang213@qq.com
 * @date      2017-06-28 15:13:01
 *
 * @note
 *  sevent.h defines
 */

#ifndef __SEVENT_H__
#define __SEVENT_H__
#include "tlist.h"
#include "thread.h"
#ifdef _WIN32
#include "event.h"
#else
#include "event-linux.h"
#endif

struct event_base;
class Bufferev;
class BuffevListener;

class Sevent : public tlist<Bufferev *>
{
public:
    Sevent();
    virtual ~Sevent();

public:
    bool               active();
    int                listen(string bindip, int port);

    void               addBuffevListener(Bufferev *bev, BuffevListener *listener);
    void               delBuffevListener(Bufferev *bev, BuffevListener *listener);

    void               onTimer();
protected:
    int                start();
    virtual void       stop();

protected:
    virtual void       onNewConnect(string srcip, int srcport, string destip, int destport , int fd) = 0;

protected:
    void               evloop();
    void               evtimer();

    friend void        timercb(evutil_socket_t fd, short event, void *arg);
    friend void        listencb(struct evconnlistener *, evutil_socket_t, struct sockaddr *, int socklen, void *arg);  
    friend void      * eventry(ThreadInfo *arg);

protected:
    volatile bool      m_active;
    event_base       * m_evbase;
    evconnlistener   * m_listener;

private:
    Thread             m_evtrd;
};
#endif
