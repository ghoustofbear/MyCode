/**
 * @file      initializer.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-09-19 08:28:43
 *
 * @note
 *  common library initializer defines
 */
#ifndef __INITIALIZER_H__
#define __INITIALIZER_H__
#include "icommon.h"
#include "concurrency.h"
class Initializer : public icommon
{
    friend class icommon;
public:
    Initializer();
    virtual ~Initializer();

private:
    virtual int        start();
    virtual void       stop();

private:
    volatile bool      m_start;
    Mutex              m_mutex;



};
#endif
