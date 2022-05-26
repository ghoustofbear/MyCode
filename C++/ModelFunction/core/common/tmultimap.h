/**
 * @file      tmultimap.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-07-03 13:48:49
 *
 * @note
 *  tmultimap.h defines
 */
#ifndef __TMULTIMAP_H__
#define __TMULTIMAP_H__
#include "concurrency.h"

template<class F, class S>
class tmultimap : public std::multimap<F, S>
{
public:
    Mutex  _mtx;
};

#endif
