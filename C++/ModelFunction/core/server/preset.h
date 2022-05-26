#ifndef __PRESET_H__
#define __PRESET_H__

#include "observer.h"
#include <map>

typedef struct
{
	string		addrcode;
	string		name;
	int			idx;
	int			type;
	int			isIVS;
} PresetInfo;


class Preset :public Observer
{
public:
	Preset();
	~Preset();

	int init();
	string getName(string addrcode,int idx);


protected:
	virtual void       update(const std::string &table, const std::string &json, int type);

private:
	map<string, PresetInfo> m_map;


};
extern Preset * pset;

#endif