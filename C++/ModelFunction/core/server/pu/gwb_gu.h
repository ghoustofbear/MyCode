/**
 * @file      msv_gu.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-01-06 16:26:17
 *
 * @note
 *  stream    0: private stream 1:standard stream
 */
#ifndef __GWB_GU_H
#define __GWB_GU_H
#include "gu.h"

class Pu;
class Thread;
class ThreadInfo;

class GwbRealGu : public RealGu
{
public:
    GwbRealGu(Pu *pu, string addrcode, int streamType, int stream);
    virtual ~GwbRealGu();

	virtual int        start(int offset);
    virtual void       stop();

    void               previewfun();

    bool               livestart();
    void               livestop();

protected:
    virtual void       onTimer();

protected:
    static void        stream_cb(const uint8_t * data, uint32_t size, uint8_t type, void *puser);
    static void        status_cb(int code, void* userData);

    static void *      threadProc(ThreadInfo *info);

public:
    uint64_t           m_tmRecvTime;

private:
    int                m_stream;
    string             m_strCallId;
    int                m_iConId;
    int                m_iDialId;
    string             m_strSessId;
    Thread            *m_trdView;
};
#endif
