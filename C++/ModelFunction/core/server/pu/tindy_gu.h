/**
* @file      hik_gu.h
* @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
* @brief     brief
* @author    chenjiaqi
* @date      2018_2_8
*
* @note
*  tindy.h defines
*/
#ifndef __TD_GU_H__
#define __TD_GU_H__
#include "gu.h"
#include "thread.h"
#include "3rd/tindy/sdk-plat.h"


#ifndef CBK
#define CBK            __stdcall
#endif


class Pu;
class TindyRealGu : public RealGu
{
public:
    TindyRealGu(Pu *pu, int channelNo, string subHost, int subPort, int streamType, int streamPotoType);
    TindyRealGu(Pu *pu, string addrcode, int streamType, int streamPotoType);
    virtual ~TindyRealGu();

    virtual int        start(int offset);
    virtual void       stop();

protected:
    virtual void       timerdriver();
    static  void     * threadEntry(ThreadInfo *arg);

protected:
    static void CBK    realdatacb(unsigned int _ulID, unsigned int _ulStreamType, unsigned char *_cData, int _iLen, void* _iUser, void* _iUserData);
    static void CBK    rawdatacb(unsigned int  realID, uint8_t * data, int  size, RAWFRAME_INFO * _pRawFrameInfo, void *userData);

private:
    long               m_realID;
    Thread             m_threadTest;
};
#endif


