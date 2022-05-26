#ifndef __CFG_CAMERA_RELATED_H__
#define __CFG_CAMERA_RELATED_H__
#include "observable.h"
#include "cfg.h"

class CameraRelatedCfg : public Cfg, public Observable
{
public:
	CameraRelatedCfg(int type);
	virtual ~CameraRelatedCfg();

	virtual int        add(const string& json);
	virtual int        del(const string& json);
	virtual int        modify(const string& json);

	virtual int        flush(const string& json);
	virtual int        query(string& json, string condition = "");
	virtual int        count(string condition = "");
	virtual int        clear(const string& json);
};
MS_API extern CameraRelatedCfg* cameraRelatedCfg;
#endif
