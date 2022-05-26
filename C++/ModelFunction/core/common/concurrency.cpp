#include "concurrency.h"

Mutex::Mutex()
{
#ifdef _WIN32
    InitializeCriticalSection(&_mtx);
#else
    pthread_mutexattr_t attr;
    pthread_mutexattr_init(&attr);
    pthread_mutexattr_settype(&attr,PTHREAD_MUTEX_RECURSIVE);
    pthread_mutex_init(&_mtx, &attr);
#endif
}

Mutex::~Mutex()
{
#ifdef _WIN32
    DeleteCriticalSection(&_mtx);
#else
    pthread_mutex_destroy(&_mtx);
#endif
}

void
Mutex::lock()
{
#ifdef _WIN32
    EnterCriticalSection(&_mtx);
#else
    pthread_mutex_lock(&_mtx);
#endif
}

void
Mutex::unlock()
{
#ifdef _WIN32
    LeaveCriticalSection(&_mtx);
#else
    pthread_mutex_unlock(&_mtx);
#endif
}

AutoLock::AutoLock(Mutex &mutex): _mutex(mutex)
{
    _mutex.lock();
}

AutoLock::~AutoLock()
{
    _mutex.unlock();
}


Event::Event()
{
#ifdef _WIN32
    InitializeCriticalSection(&m_mtx);
    m_cond = CreateEvent(NULL, FALSE, FALSE, NULL);
#else
    pthread_mutex_init(&m_mtx, NULL);
    pthread_cond_init(&m_cond, NULL);
#endif

}

Event::~Event()
{
#ifdef _WIN32
    DeleteCriticalSection(&m_mtx);
    CloseHandle(m_cond);
    m_cond = NULL;
#else
    pthread_mutex_destroy(&m_mtx);
    pthread_cond_destroy(&m_cond);
#endif

}

void
Event::wait(uint32_t ms)
{
    if (0 != ms)
    {
#ifdef _WIN32
        WaitForSingleObject(m_cond, ms);
#else
        struct timespec ts;
        struct timeval tv;
        gettimeofday(&tv, NULL);

        uint64_t usec = tv.tv_usec + ms * 1000;

        if (usec >= 1000000)
        {
            ts.tv_sec  = tv.tv_sec + usec / 1000000;
            ts.tv_nsec = usec % 1000000 * 1000;
        }
        else
        {
            ts.tv_sec  = tv.tv_sec;
            ts.tv_nsec = usec * 1000;
        }

        pthread_cond_timedwait(&m_cond, &m_mtx, &ts);
#endif
    }
    else
    {
#ifdef _WIN32
        WaitForSingleObject(m_cond, INFINITE);
#else
        pthread_mutex_lock(&m_mtx);
        pthread_cond_wait(&m_cond, &m_mtx);
        pthread_mutex_unlock(&m_mtx);
#endif
    }
}

void
Event::notify()
{

#ifdef _WIN32
    SetEvent(m_cond);
#else
    pthread_cond_signal(&m_cond);
#endif
}


Semaphore::Semaphore(uint32_t cnt)
{
#ifdef _WIN32
    m_sem = CreateSemaphore(NULL, 0, LONG_MAX, NULL);
#else
    sem_init(&m_sem, 0, 0);
#endif
}

Semaphore::~Semaphore()
{
#ifdef _WIN32

    if (m_sem)
    {
        CloseHandle(m_sem);
    }

#else
    sem_destroy(&m_sem);
#endif
}

bool
Semaphore::wait(uint32_t ms)
{
    if (0 == ms)
    {
#ifdef _WIN32
        return WaitForSingleObject(m_sem, INFINITE) == WAIT_OBJECT_0;
#else
        return (0 == sem_wait(&m_sem)) ? true : false;
#endif
    }
    else
    {
#ifdef _WIN32
        return WaitForSingleObject(m_sem, ms) == WAIT_OBJECT_0;
#else
        struct timespec ts;
        struct timeval tv;
        gettimeofday(&tv, NULL);

        uint64_t usec = tv.tv_usec + ms * 1000;

        if (usec >= 1000000)
        {
            ts.tv_sec  = tv.tv_sec + usec / 1000000;
            ts.tv_nsec = usec % 1000000 * 1000;
        }
        else
        {
            ts.tv_sec  = tv.tv_sec;
            ts.tv_nsec = usec * 1000;
        }

        return (0 == sem_timedwait(&m_sem, &ts)) ? true : false;
#endif
    }
}

void
Semaphore::post()
{
#ifdef _WIN32
    ReleaseSemaphore(m_sem, 1, NULL);
#else
    sem_post(&m_sem);
#endif
}
