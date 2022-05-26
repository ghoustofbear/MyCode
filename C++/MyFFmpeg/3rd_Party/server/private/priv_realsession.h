/**
 * @file      privsession.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-05-24 17:39:48
 *
 * @note
 *  privsession.h defines
 */
#ifndef __PRIV_REALSESSION_H__
#define __PRIV_REALSESSION_H__
#include "realsession.h"

class Client;
class PrivRealSession : public RealSession
{
public:
    PrivRealSession(Client *client, Bufferev* bev);
    virtual ~PrivRealSession();

//private:
//    virtual void       stop();
};
#endif
