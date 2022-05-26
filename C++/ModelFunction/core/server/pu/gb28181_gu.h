#ifndef __GB28181_GU_H
#define __GB28181_GU_H
#include "gu.h"

class Pu;
class Thread;
class ThreadInfo;

class Gb28181bRealGu : public RealGu
{
public:
    Gb28181bRealGu(Pu *pu, string addrcode, int streamType, int stream);
    virtual ~Gb28181bRealGu();

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
