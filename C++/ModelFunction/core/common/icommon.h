/**
 * @file      icommon.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-11-27 11:39:29
 *
 * @note
 *  icommon.h defines
 */
#ifndef __ICOMMON_H__
#define __ICOMMON_H__
#include "def.h"

class CM_API icommon
{
public:
    virtual int        start() = 0;
    virtual void       stop()  = 0;

public:
    static  bool       inited();
    static  icommon  * instance();


    static icommon* m_self;
};

#endif
