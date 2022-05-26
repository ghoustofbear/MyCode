/**
 * @file      task.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-05-11 16:58:41
 *
 * @note
 *  task.h defines
 */
#ifndef __TASK_H__
#define __TASK_H__
#include "thread.h"
#include "buffevlistener.h"

class Socket;
class ThreadInfo;
class Task : public BuffevListener
{
public:
    enum
    {
        S_FREE,
        S_ESTABLISHED,
        S_CLOSE_WAIT,
        S_CLOSED,
    };

    Task();
    virtual ~Task();

public:
    int                status();
    void               setbev(Bufferev *bev);

    virtual void       stop();
protected:
    virtual int        start();

    virtual int        onLogin(string json) = 0;
    virtual int        onSubscribe(string json) = 0;
    virtual int        onSubscribeStatus(string json) = 0;
    virtual int        onQueryDevice(string json) = 0;

    virtual int        onPreview(string json) = 0;
    virtual int        onPtzCtrl(string json) = 0;
    virtual int        onQueryRecord(string json) = 0;

    virtual int        onPlayback(string json) = 0;
    virtual int        onDownload(string json) = 0;

    virtual int        onSyncTime(string json) = 0;

    virtual int        process() = 0;

    //songhongliang
    virtual int        onDualTalk(string json) = 0;


protected:
    static void *      threadProc(ThreadInfo *);

protected:
    int                m_status;
    Thread             m_thread;
    Bufferev         * m_bev;
};
#endif
