/**
 * (C) Copyright 2013, xxx Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 * Thu Jun  6 17:29:46 PDT 2013
 *
 * my template list from std::list
 */

#ifndef __TLIST_H__
#define __TLIST_H__
#include "concourrencyc1.h"

template <class T>
class TList : public std::list<T>
{
	public:
		TList(){};
		~TList(){};

	public:
		VOID  Lock()   { m_Mutex.Lock(); }
		VOID  UnLock() { m_Mutex.UnLock(); }

	private:
	public:
		Mutex  m_Mutex;
};

#endif
