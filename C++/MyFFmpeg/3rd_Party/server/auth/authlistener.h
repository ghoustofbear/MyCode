/**
 * @file      authlistener.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-08-28 16:58:59
 *
 * @note
 *  authlistener.h defines
 */
#include "def.h"
#ifndef __AUTHLISTENER_H__
#define __AUTHLISTENER_H__
class AuthListener
{
public:
    virtual void       onChanged(const string &json) = 0;

    virtual string     peerip()   = 0;
    virtual int        peerport() = 0;
    virtual string     peertime() = 0;
};
#endif
