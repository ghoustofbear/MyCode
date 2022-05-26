/**
 * @file      Platform.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-11-17 15:04:09
 *
 * @note
 *  Platform.h defines
 */
#ifndef __PLATFORM_H__
#define __PLATFORM_H__
#include "observer.h"

class Platform : public Observer
{
public:
    Platform();
    virtual ~Platform();

public:
    int                init();

    std::string        addrcode();
    int                sport();
    int                rport();
    int                hport();
    int                rtport();
	std::string        ip();

protected:
    virtual void       update(const std::string &table, const std::string &json, int type);

private:
    bool               m_enable;
    int                m_protocol;
    int                m_sport;
    int                m_rport;
    int                m_hport;
    int                m_rtport;
    std::string        m_aaaip;
	std::string        m_sendip;
};

extern Platform * platform;
#endif
