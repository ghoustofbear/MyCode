/**
 * (C) Copyright 2012, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Thu Aug  9 10:46:20 CST 2012
 *
 * Concourrency defines.
 */
#ifndef __CONCOURRENCY_H__
#define __CONCOURRENCY_H__

#include "defc1.h"

#ifdef _WIN32
typedef CRITICAL_SECTION    MUTEX;
typedef HANDLE              COND;
typedef HANDLE              SEM;
#else
typedef pthread_mutex_t     MUTEX;
typedef pthread_cond_t      COND;
typedef sem_t               SEM;
#endif

class CM_API Mutex
{
	public:
		Mutex();
		~Mutex();

		/**< 加锁操作 */
		VOID Lock();

		/**< 解锁操作 */
		VOID UnLock();

	protected:

	private:
		MUTEX m_Mtx;    /**< 互斥锁 */
};

class CM_API AutoLock
{
	public:
		AutoLock(Mutex &mutex);
		~AutoLock();

	protected:

	private:
		Mutex &m_Mutex;   /**< Mutex引用  */
};

class CM_API Event
{
	public:
		Event();
		~Event();

		/**< 等待事件操作 */
		VOID Wait(U32 u32Msec = 0);


		/**< 通知事件操作 */
		VOID Notify();

	private:
		MUTEX m_Mtx;               /**< 互斥锁 */
		COND  m_Cond;              /**< 条件变量 */
};


class CM_API Semaphore
{
	public:
		Semaphore(U32 u32Count = 0);
		~Semaphore();

		/**< 超时P操作 */
		BOOL Wait(U32 u32Msec = 0);


		/**< V操作 */
		VOID Post();

	private:
		SEM    m_Sem;              /**< 信号量 */
};

#endif
