/**
 * @file      hs_superior.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-11-15 14:05:45
 *   
 * @note
 *  hs_superior.h defines 
 */
#ifndef __HS_SUPERIOR_H__
#define __HS_SUPERIOR_H__
#include "superior.h"

class HSSuperior : public Superior 
{
public:
    HSSuperior(string addrcode, string name, string user, string pswd, string host, int sipPort, int rtspPort, 
        int httpPort, int expires, string ifupIp, int bregister, int ivauslocal);
    virtual ~HSSuperior();

protected:
    virtual void       onAIYX(string &json, void *data, uint32_t size);
    virtual void       onAIYC(string &json, void *data, uint32_t size);
protected:
    /* virtual int        regist(); */
    virtual void       timerdriver();

private:
   
};
#endif
