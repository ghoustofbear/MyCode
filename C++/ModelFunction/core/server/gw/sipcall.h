/**
 * @file      sipcall.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-12-20 11:33:05
 *
 * @note
 *  sipcall.h defines
 */
#ifndef __SIPCALL_H__
#define __SIPCALL_H__
#include <string>
#include <stdint.h>
#include "rtptriggerlistener.h"
using namespace std;

class GSession;
class SipCall : public RTPTriggerListener
{
public:
    enum
    {
        S_UNINIT,
        S_RINGING,
        S_TALKING,
        S_CLOSED,
    };

    SipCall(string &callid, string &fromn, string &fromh, int fromp, string &ton, string &toh, int top);
    virtual ~SipCall();

public:
    int                status();
    string             callid() { return m_callid; }

    string             fromname();
    string             fromhost();
    int                fromport();

    string             toname();
    string             tohost();
    int                toport();

    int                vcmode();

    void               setmedia(int srcport, string &destip, int destport, int vcmode);

    void               timerdriver();

public:
    virtual string     srcip();
    virtual int        srcport();
    virtual string     destip();
    virtual int        destport();

protected:
    virtual void       onHeartbeat();
    virtual void       onClose();

private:
    int                m_status;
    string             m_callid;
    string             m_fromn;
    string             m_fromh;
    int                m_fromp;
    string             m_ton;
    string             m_toh;
    int                m_top;

    int                m_srcport;
    string             m_destip;
    int                m_destport;
    int                m_vcmode;

    uint64_t           m_lastpts;
};
#endif
