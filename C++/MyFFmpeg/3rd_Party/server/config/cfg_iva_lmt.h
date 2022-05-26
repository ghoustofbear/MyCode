#ifndef __CFG_IVA_LMT_H__
#define __CFG_IVA_LMT_H__
#include "observable.h"
#include "cfg.h"

class IvaLmtCfg : public Cfg, public Observable
{
public:
    IvaLmtCfg(int type);
    virtual ~IvaLmtCfg();

    virtual int        add(const string& json);
    virtual int        del(const string& json);
    virtual int        modify(const string& json);

    virtual int        flush(const string& json);
    virtual int        query(string& json, string condition = "");
    virtual int        count(string condition = "");
    virtual int        clear(const string& json);
};

MS_API extern IvaLmtCfg* ivalmtCfg;
#endif
