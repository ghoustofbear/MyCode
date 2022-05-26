#ifndef __CFG_CAMERA_PLATECFG_H__
#define __CFG_CAMERA_PLATECFG_H__
#include "observable.h"
#include "cfg.h"

class CameraPlateCfg : public Cfg, public Observable
{
public:
    CameraPlateCfg(int type);
    virtual ~CameraPlateCfg();

    virtual int        add(const string& json);
    virtual int        del(const string& json);
    virtual int        modify(const string& json);

    virtual int        flush(const string& json);
    virtual int        query(string& json, string condition = "");
    virtual int        count(string condition = "");
    virtual int        clear(const string& json);
};

MS_API extern CameraPlateCfg* cameraPlateCfg;
#endif