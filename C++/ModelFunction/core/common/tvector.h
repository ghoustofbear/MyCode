/**
 * @file      tvector.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:29:20
 *   
 * @note
 *  tvector.h defines 
 */
#ifndef __TVECTOR_H__
#define __TVECTOR_H__
#include "concurrency.h"

template <class _T>
class tvector : public std::vector<_T>
{
public:
    Mutex  _mtx;
};

#endif
