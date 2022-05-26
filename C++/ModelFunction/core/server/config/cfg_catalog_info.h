#ifndef __CFG_CATALOG_INFO_H__
#define __CFG_CATALOG_INFO_H__
#include "observable.h"
#include "cfg.h"

class CatalogInfoCfg : public Cfg, public Observable
{
public:
	CatalogInfoCfg(int type);
	virtual ~CatalogInfoCfg();

	virtual int        add(const string& json);
	virtual int        del(const string& json);
	virtual int        modify(const string& json);

	virtual int        flush(const string& json);
	virtual int        query(string& json, string condition = "");
	virtual int        count(string condition = "");
	virtual int        clear(const string& json);
};

MS_API extern CatalogInfoCfg* catalogInfoCfg;
#endif
