/**
 * @file      tmap.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:28:56
 *   
 * @note
 *  tmap.h defines 
 */
#ifndef __TMAP_H__
#define __TMAP_H__
#include "concurrency.h"

template<class F, class S>
class tmap : public std::map<F, S>
{
public:
    void   lock()   { _mtx.lock();   }
    void   unlock() { _mtx.unlock(); }

public:
    Mutex  _mtx;
};

#endif
