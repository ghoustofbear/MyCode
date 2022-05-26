/**
 * @file      hs_pu.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-12-29 10:09:00
 *
 * @note
 *  hs_pu.h defines
 */
#ifndef __HS_PU_H__
#define __HS_PU_H__
#include "gw_pu.h"

class HsPu : public GwPu
{
public:
    HsPu(int type, string addrcode, string name, string host, int sport, int rport, int hport, int rtport);
    virtual ~HsPu();

public:
    virtual int        start();

protected:
    virtual void       stop();

    virtual int        login();
    virtual void       logout();

protected:
    virtual int        queryRecord(string addrcode, int type, datetime_s &start, datetime_s &end, int fromIndex, int toIndex, string &json);

private:
    /* data */
};
#endif
