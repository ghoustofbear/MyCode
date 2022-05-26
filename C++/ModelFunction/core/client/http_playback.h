/**
 * @file      http_playback.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-02 09:20:39
 *
 * @note
 *  http_playback.h defines
 */
#ifndef __HTTP_PLAYBACK_H__
#define __HTTP_PLAYBACK_H__
#include "playback.h"

class Httplayback : public Playback
{
#define STATUS(code) \
    do \
    { \
        if (m_statusCB) \
        { \
            m_statusCB(code, m_statusData); \
        } \
    } while(0);

public:
    Httplayback(string filename, Cu *cu);
    Httplayback(datetime_s &start, datetime_s &end, Cu *cu);
    virtual ~Httplayback();

protected:
    virtual int        open();

    virtual int        recv();
    virtual int        pause();
    virtual int        continuex();
    virtual int        heartbeat();
    virtual int        streamCtrl(int code, void *val, int valSize);

protected:
    int                recvMsg(string &method, string &url, int &status, string &json);
    int                recvFrame();


};

#endif
