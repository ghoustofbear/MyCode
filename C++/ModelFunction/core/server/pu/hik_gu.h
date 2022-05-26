/**
 * @file      hik_gu.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-06-20 15:14:00
 *
 * @note
 *  hik_gu.h defines
 */
#ifndef __HIK_GU_H__
#define __HIK_GU_H__
#include "gu.h"
#include "thread.h"
#include "3rd/hik/sdk-plat.h"

#define CBK            CALLBACK 

class Pu;
class HikRealGu : public RealGu
{
public:
    HikRealGu(Pu *pu, int channelNo, string subHost, int subPort, int streamType, int streamPotoType);
    HikRealGu(Pu *pu, string addrcode, int streamType, int streamPotoType);
    virtual ~HikRealGu();

	virtual int        start(int offset);
    virtual void       stop();

protected:
    virtual void       onTimer();

protected:
    static void CBK    realdatacb(LONG realID, DWORD type, BYTE *data, DWORD size, void* userData);
    static void CBK    rawdatacb(LONG realID, DWORD type, uint8_t *data, DWORD  size, void *userData);
    static void CBK    rawdatacb1(LONG recordID, NET_DVR_PACKET_INFO_EX *packet, void *userData);

private:
    long               m_realID;
};
#endif
