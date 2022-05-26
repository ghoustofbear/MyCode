#include "concourrencyc1.h"

Mutex::Mutex()
{
#ifdef _WIN32
	InitializeCriticalSection(&m_Mtx);
#else
	pthread_mutex_init(&m_Mtx, NULL);
#endif
}

Mutex::~Mutex()
{
#ifdef _WIN32
	DeleteCriticalSection(&m_Mtx);
#else
	pthread_mutex_destroy(&m_Mtx);
#endif
}

VOID Mutex::Lock()
{
#ifdef _WIN32
	EnterCriticalSection(&m_Mtx);
#else
	pthread_mutex_lock(&m_Mtx);
#endif
}

VOID Mutex::UnLock()
{
#ifdef _WIN32
	LeaveCriticalSection(&m_Mtx);
#else
	pthread_mutex_unlock(&m_Mtx);
#endif
}

AutoLock::AutoLock(Mutex &mutex):m_Mutex(mutex)
{
	m_Mutex.Lock();
}

AutoLock::~AutoLock()
{
	m_Mutex.UnLock();
}


Event::Event()
{
#ifdef _WIN32
	InitializeCriticalSection(&m_Mtx);
	m_Cond = CreateEvent(NULL, FALSE, FALSE, NULL);
#else
	pthread_mutex_init(&m_Mtx, NULL);
	pthread_cond_init(&m_Cond, NULL);
#endif
	
}

Event::~Event()
{
#ifdef _WIN32
	DeleteCriticalSection(&m_Mtx);
	CloseHandle(m_Cond);
	m_Cond = NULL;
#else
	pthread_mutex_destroy(&m_Mtx);
	pthread_cond_destroy(&m_Cond);
#endif

}

VOID Event::Wait(U32 u32Msec)
{
	if (0 != u32Msec)
	{
#ifdef _WIN32
        WaitForSingleObject(m_Cond, u32Msec);
#else
        struct timespec ts;
        struct timeval tv;
        gettimeofday(&tv, NULL);
        ts.tv_sec  = tv.tv_sec + u32Msec/ 1000;
        ts.tv_nsec = tv.tv_usec * 1000;

        pthread_cond_timedwait(&m_Cond, &m_Mtx, &ts);
#endif
	}
	else
	{
#ifdef _WIN32
		WaitForSingleObject(m_Cond, INFINITE);
#else
		pthread_mutex_lock(&m_Mtx);
		pthread_cond_wait(&m_Cond, &m_Mtx);
		pthread_mutex_unlock(&m_Mtx);
#endif
	}
}

VOID Event::Notify()
{

#ifdef _WIN32
	SetEvent(m_Cond);
#else
	pthread_cond_signal(&m_Cond);
#endif
}


Semaphore::Semaphore(U32 u32Count)
{
#ifdef _WIN32
	m_Sem = CreateSemaphore(NULL, 0, LONG_MAX, NULL);
#else
	sem_init(&m_Sem,0, 0);
#endif
}

Semaphore::~Semaphore()
{
#ifdef _WIN32
	if (m_Sem)
		CloseHandle(m_Sem);	
#else
	sem_destroy(&m_Sem);
#endif
}

BOOL Semaphore::Wait(U32 u32Msec)
{
	if (0 == u32Msec)
	{
#ifdef _WIN32
		return WaitForSingleObject(m_Sem, INFINITE) == WAIT_OBJECT_0;
#else
		return (0 == sem_wait(&m_Sem)) ? TRUE : FALSE;
#endif
	}
	else
	{
#ifdef _WIN32
		return WaitForSingleObject(m_Sem, u32Msec) == WAIT_OBJECT_0;
#else
        struct timespec ts;
        struct timeval tv;
        gettimeofday(&tv, NULL);
        ts.tv_sec  = tv.tv_sec + u32Msec/ 1000;
        ts.tv_nsec = tv.tv_usec * 1000;

        return (0 == sem_timedwait(&m_Sem, &ts)) ? TRUE : FALSE;
#endif
	}
}

VOID Semaphore::Post()
{
#ifdef _WIN32
	ReleaseSemaphore(m_Sem, 1, NULL);
#else
	sem_post(&m_Sem);
#endif

}
