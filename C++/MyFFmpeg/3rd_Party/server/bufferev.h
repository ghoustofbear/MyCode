/**
 * @file      bufferev.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-06-28 15:21:43
 *
 * @note
 * srcip()   The ip of a locally received data network interface
 * srcport() The port of a locally received data network interface, == platfrom->rport()
 */
#ifndef __BUFFEREV_H__
#define __BUFFEREV_H__
#include "def.h"
#include "concurrency.h"
#include "tlist.h"

//#define USING_EVTHREAD
//#define USING_OICP

struct event_base;
struct bufferevent;

class Socket;
class BuffevListener;

class Bufferev : public tlist<BuffevListener *>
{
public:
    Bufferev(event_base *evbase);
    virtual ~Bufferev();

    friend void        writecb(struct bufferevent *bev, void *user_data);
    friend void        readcb(struct bufferevent *bev, void *ctx);
    friend void        eventcb(struct bufferevent *bev, short events, void *user_data);

public:
    int                open(int fd, bool r, bool w, int rto = 0, int wto = 0);
    void               close();

    void               addBuffevListener(BuffevListener *listener);
    void               delBuffevListener(BuffevListener *listener);
    int                listeners();

    void               notifyRead(Bufferev *bev, void *data, uint32_t size);
    void               notifyWrite(Bufferev *bev);
    void               notifyClose(Bufferev *bev);

    void               enable(bool r, bool w);
    int                write(void *stream, uint32_t size);

    void               push(uint8_t *stream, uint32_t size);

    bool               active();
    bool               selfdisconn();

    string             srcip();
    int                srcport();

    string             peerip();
    int                peerport();

public:
    virtual void       onTimer();
	int                m_fd;

protected:
    volatile bool      m_active;
    event_base       * m_evbase;
    bufferevent      * m_bev;
    int                m_rto;
    int                m_wto;
    
    
    typedef struct bufevent_data
    {
        uint8_t *stream;
        uint32_t size;
    }bufevent_data;

    tlist<bufevent_data*>  m_streambuf;
};
#endif
