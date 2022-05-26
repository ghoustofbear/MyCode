/**
 * @file      tlist.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:28:20
 *
 * @note
 *  tlist.h defines
 */
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
