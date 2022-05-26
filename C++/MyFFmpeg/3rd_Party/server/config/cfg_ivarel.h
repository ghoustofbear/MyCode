#ifndef __CFG_IVAREL_H__
#define __CFG_IVAREL_H__
#include "observable.h"
#include "cfg.h"

class IVArelCfg : public Cfg, public Observable
{
public:
    IVArelCfg(int type);
    virtual ~IVArelCfg();

    virtual int        add(const string& json);
    virtual int        del(const string& json);
    virtual int        modify(const string& json);

    virtual int        flush(const string& json);
    virtual int        query(string& json, string condition = "");
    virtual int        count(string condition = "");
    virtual int        clear(const string& json);
};

MS_API extern IVArelCfg* ivarelCfg;
#endif
