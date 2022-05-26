/**
 * @file      rtptriggerlistener.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-12-20 11:50:16
 *
 * @note
 *  rtptriggerlistener.h defines
 */
#ifndef __RTPTRIGGERLISTENER_H__
#define __RTPTRIGGERLISTENER_H__
#include <string>
using namespace std;

class RTPTriggerListener
{
public:
    virtual string     srcip()       = 0;
    virtual int        srcport()     = 0;
    virtual string     destip()      = 0;
    virtual int        destport()    = 0;

    virtual void       onHeartbeat() = 0;
    virtual void       onClose()     = 0;
};
#endif
