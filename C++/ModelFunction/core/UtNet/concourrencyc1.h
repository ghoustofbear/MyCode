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

		/**< �������� */
		VOID Lock();

		/**< �������� */
		VOID UnLock();

	protected:

	private:
		MUTEX m_Mtx;    /**< ������ */
};

class CM_API AutoLock
{
	public:
		AutoLock(Mutex &mutex);
		~AutoLock();

	protected:

	private:
		Mutex &m_Mutex;   /**< Mutex����  */
};

class CM_API Event
{
	public:
		Event();
		~Event();

		/**< �ȴ��¼����� */
		VOID Wait(U32 u32Msec = 0);


		/**< ֪ͨ�¼����� */
		VOID Notify();

	private:
		MUTEX m_Mtx;               /**< ������ */
		COND  m_Cond;              /**< �������� */
};


class CM_API Semaphore
{
	public:
		Semaphore(U32 u32Count = 0);
		~Semaphore();

		/**< ��ʱP���� */
		BOOL Wait(U32 u32Msec = 0);


		/**< V���� */
		VOID Post();

	private:
		SEM    m_Sem;              /**< �ź��� */
};

#endif
