/*
 * @Author: liuxu
 * @Date: 2022-04-02 10:26:26
 * @LastEditors: liuxu
 * @LastEditTime: 2022-04-02 10:26:27
 * @FilePath: \C++\MyFFmpeg\include\x64\common\tlist.h
 * @Description: 
 * 
 * Copyright (c) 2022 by liuxu/1056203382@qq.com, All Rights Reserved. 
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
