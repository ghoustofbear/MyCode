#ifndef __CFG_GATEMGR_H__
#define __CFG_GATEMGR_H__
#include "observable.h"
#include "cfg.h"

class GateCfg : public Cfg, public Observable
{
public:
    GateCfg(int type);
    virtual ~GateCfg();

    virtual int        add(const string& json);
    virtual int        del(const string& json);
    virtual int        modify(const string& json);

    virtual int        flush(const string& json);
    virtual int        query(string& json, string condition = "");
    virtual int        count(string condition = "");
    virtual int        clear(const string& json);
};

MS_API extern GateCfg* gateCfg;
#endif
