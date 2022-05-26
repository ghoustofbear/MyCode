#ifndef __CFG_CAMERA_BELONG_PLATFORM_H__
#define __CFG_CAMERA_BELONG_PLATFORM_H__
#include "observable.h"
#include "cfg.h"

class CameraBelongPlatformCfg : public Cfg, public Observable
{
public:
	CameraBelongPlatformCfg(int type);
	virtual ~CameraBelongPlatformCfg();

	virtual int        add(const string& json);
	virtual int        del(const string& json);
	virtual int        modify(const string& json);

	virtual int        flush(const string& json);
	virtual int        query(string& json, string condition = "");
	virtual int        count(string condition = "");
	virtual int        clear(const string& json);
};

MS_API extern CameraBelongPlatformCfg* cameraBelongPlatformCfg;
#endif
