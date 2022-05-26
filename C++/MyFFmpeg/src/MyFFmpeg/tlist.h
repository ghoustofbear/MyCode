
#ifndef __TLIST_H__
#define __TLIST_H__
#include "concurrency.h"

template <class T>
class tlist : public std::list<T>
{
public:
    Mutex  _mtx;
};

#endif
