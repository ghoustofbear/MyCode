/**
 * @file      tqueue.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-11-14 08:47:52
 *
 * @note
 *  tqueue.h defines
 */
#ifndef __TQUEUE_H__
#define __TQUEUE_H__
#include "concurrency.h"

template <class _T>
class tqueue : public std::queue<_T>
{
public:
    void       clear();

public:
    Mutex      _mtx;
};

template <class _T>
void
tqueue<_T>::clear()
{
    queue<_T> empty;
    std::swap(*this, empty);
}

#endif
