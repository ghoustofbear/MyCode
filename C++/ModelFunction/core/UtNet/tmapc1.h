/**
 * (C) Copyright 2013, xxx Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 * Thu Jun  6 17:29:46 PDT 2013
 *
 * my template map from std::map
 */

#ifndef __TMAP_H__
#define __TMAP_H__
#include "concourrency.h"


template<class T1, class T2>
class TMap : public std::map<T1, T2>
{
	public:
		TMap(){};
		~TMap(){};
		
	public:
		VOID  Lock()   { m_Mutex.Lock(); }
		VOID  UnLock() { m_Mutex.UnLock(); }
		
	private:
	public:
		Mutex  m_Mutex;
};

#endif
