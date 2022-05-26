#ifndef __HUADIAN_GU_H__
#define __HUADIAN_GU_H__

#include "gu.h"

class Pu;
class RtspClient;


class HuaDianGu : public RealGu
{
public:
    HuaDianGu(Pu *pu, int channelNo, string subHost, int subPort, int streamType, int streamPotoType);
    HuaDianGu(Pu *pu, string addrcode, int streamType, int streamPotoType);
    virtual ~HuaDianGu();

    virtual int        start(int offset);
    virtual void       stop();

protected:
    static void        stream_cb(const uint8_t * data, uint32_t size, uint8_t type, void *puser);

    virtual void       onTimer();

    static  void       pspackdatacallback(uint8_t *data, int size, void* userdata, int type, uint64_t pts, uint64_t frameTime);

private:
    RtspClient       * m_rtspClient;
    void             * m_psHandle;
    bool               m_bok;
    time_t             m_lastHeartTime;
};

#endif