/**
 * @file      hik_gu.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-06-20 15:14:00
 *
 * @note
 *  dahua_gu.h defines
 */
#ifndef __DAHUA_GU_H__
#define __DAHUA_GU_H__
#include "gu.h"
#include "thread.h"
#include "3rd/dahua/dhnetsdk.h"

#ifndef CBK
#define CBK            CALLBACK 
#endif

class Pu;
class DahuaRealGu : public RealGu
{
public:
    DahuaRealGu(Pu *pu, int channelNo, string subHost, int subPort, int streamType, int streamPotoType);
    DahuaRealGu(Pu *pu, string addrcode, int streamType, int streamPotoType);

    virtual ~DahuaRealGu();

	virtual int        start(int offset);
    virtual void       stop();

protected:
    virtual void       timerdriver();

protected:

#ifdef __linux__
    static void CBK    RealDataCallBack(LLONG   lRealHandle,
        DWORD   dwDataType,
        BYTE    * pData,
        DWORD   dwDataSize,
        LONG    dwParam,
        long int   dwUser);
#else
    static void CBK    RealDataCallBack(LLONG   lRealHandle,
        DWORD     dwDataType,
        BYTE      * pData,
        DWORD     dwDataSize,
        LONG      dwParam,
        LDWORD     dwUser);
#endif

private:
    LLONG              m_realHandl;
    long               m_realStandarID;
    long               m_realPrivateID;

    char             * m_bufstream;
    int                m_pos;
};
#endif

