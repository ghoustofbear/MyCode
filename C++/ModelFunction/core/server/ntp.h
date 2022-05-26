/**
 * @file      System.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-11-17 15:04:09
 *
 * @note
 *  ntp.h defines
 */
#ifndef __NTP_TIME_H__
#define __NTP_TIME_H__
#include "observer.h"

class NtpTime : public Observer
{
public:
    NtpTime();
    virtual ~NtpTime();

public:
    int                init();
    std::string        allowIp();
    int                beTime();
    int                doTime();
    int                timeInterval();

protected:
    virtual void       update(const std::string &table, const std::string &json, int type);

private:
    std::string        m_allowTimeIp;
    int                m_beTime;
    int                m_doTime;
    int                m_timeInterval;
    int                m_ntpId;
};
extern NtpTime * ntpTime;
#endif
