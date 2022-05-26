#ifndef __CELLPROCESS_H__
#define __CELLPROCESS_H__

#include <list>
#include <stdint.h>

class CELLProcessMgr
{
public:
    std::list<uint64_t> QueryProcessIDByName(const char *strName);
    void KillProcessByName(const char *strName);
    bool KillProcessByPID(const uint64_t pid);
private:
	std::list<uint64_t> m_listProcessID;
};


#endif // !__CELLPROCESS_H__
