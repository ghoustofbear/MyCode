#ifndef __GW_HIKUA_H__
#define __GW_HIKUA_H__

#include "gw_sipua.h"

class HikSipUA : public GWSipUA
{
public:
	HikSipUA(int transport, string bindip, int port);
	virtual ~HikSipUA();

public:
	int                on_resource(const string &json, string &xml, Superior *super = NULL);
};


#endif
