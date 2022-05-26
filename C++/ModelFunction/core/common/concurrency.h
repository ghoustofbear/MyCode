/**
 * @file      concurrency.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:26:40
 *
 * @note
 *  concurrency.h defines
 */
#ifndef __CONCURRENCY_H__
#define __CONCURRENCY_H__

#include "def.h"

#ifdef _WIN32
typedef CRITICAL_SECTION     MUTEX;
typedef HANDLE               COND;
typedef HANDLE               SEM;
#else
#include <semaphore.h>
#include <sys/time.h>
typedef pthread_mutex_t      MUTEX;
typedef pthread_cond_t       COND;
typedef sem_t                SEM;
#endif

class CM_API Mutex
{
    friend class AutoLock;
public:
    Mutex();
    ~Mutex();

protected:
    void               lock();
    void               unlock();

private:
    MUTEX              _mtx;
};

class CM_API AutoLock
{
public:
    AutoLock(Mutex &mutex);
    ~AutoLock();

private:
    Mutex            & _mutex;
};

class CM_API Event
{
public:
    Event();
    ~Event();

    void               wait(uint32_t ms = 0);
    void               notify();

private:
    MUTEX              m_mtx;
    COND               m_cond;
};


class CM_API Semaphore
{
public:
    Semaphore(uint32_t cnt = 0);
    ~Semaphore();

public:
    bool               wait(uint32_t ms = 0);
    void               post();

private:
    SEM                m_sem;
};

#endif
