/**
 * @file      priv_recordsession.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-06-07 16:11:42
 *
 * @note
 *  priv_recordsession.h defines
 */
#ifndef __PRIV_RECORDSESSION_H__
#define __PRIV_RECORDSESSION_H__
#include "recordsession.h"

class Client;
class PrivRecordSession : public RecordSession
{
public:
    PrivRecordSession(Client *client, Bufferev *bev, int download = 0);
    virtual ~PrivRecordSession();

private:
    virtual void       stop();
};
#endif
