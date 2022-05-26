#ifndef __UT_SUPERIOR_H__
#define __UT_SUPERIOR_H__
#include "superior.h"
class UtSuperior :
    public Superior
{
public:
    UtSuperior(string addrcode, string name, string user, string pswd, string host, int sipPort, int rtspPort,
        int httpPort, int expires, string ifupIp, int bregister, int ivauslocal);

    virtual ~UtSuperior();
    
    virtual int        on_resource(const string &json, string &xml);
    virtual int        push_resource();
protected:
    virtual void       timerdriver();
};


#endif //!__UT_SUPERIOR_H__