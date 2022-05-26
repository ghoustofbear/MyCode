/**
 * @file      thread.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:28:11
 *   
 * @note
 *  thread.h defines 
 */

#ifndef __THREAD_H__
#define __THREAD_H__

#include "sys.h"
#include "concurrency.h"

class CM_API Thread
{
public:
    Thread();
    virtual ~Thread();

    void       setEntry(THREAD_FUNC func, void * data, int id);

    int        start();
    void       stop();
    void       detach();

    bool       active();

private:
    bool       _active;
    ThreadInfo _info;
    Mutex      _mtx;
};

#endif
