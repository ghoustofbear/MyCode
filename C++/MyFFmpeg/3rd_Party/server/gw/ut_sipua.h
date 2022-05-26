#ifndef __UT_SIPUA_H__
#define __UT_SIPUA_H__
#include "gw_sipua.h"
class UtSipUA :
    public GWSipUA
{
public:
    UtSipUA(int transport, string bindip, int port);
    virtual ~UtSipUA();
    int query_resource(const string &fromn, const string &fromh, int fromp, const string &ton, const string &toh, int top, const string &code,
        string &content, int jx = 0, int fromidx = 1, int toidx = 100);
};


#endif //!__UT_SIPUA_H__