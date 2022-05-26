#ifndef __CFG_IVATASKRESOURCES_H__
#define __CFG_IVATASKRESOURCES_H__
#include "observable.h"
#include "cfg.h"

class IVATaskResourcesCfg : public Cfg, public Observable
{
public:
    IVATaskResourcesCfg(int type);
    virtual ~IVATaskResourcesCfg();

    virtual int        add(const string& json);
    virtual int        del(const string& json);
    virtual int        modify(const string& json);

    virtual int        flush(const string& json);
    virtual int        query(string& json, string condition = "");
    virtual int        count(string condition = "");
    virtual int        clear(const string& json);
};

MS_API extern IVATaskResourcesCfg* ivaTaskResourcesCfg;
#endif
