#ifndef  __HIKPLATFORM_ALARM_H__
#define  __HIKPLATFORM_ALARM_H__

#include <map>
#include "concourrencyc1.h"
using namespace std;

class HikPlatformAlarm
{
public:
	static HikPlatformAlarm * getInstance();

    bool startService(char * ip, int port);

	void registerAlarm(void * pfnAlarmCallback, void * pUser);

protected:
	HikPlatformAlarm() {}

	static VOID *TcpListenPro(VOID *pArg);
	
	VOID   TcpListenLoop();
	
private:
	HikPlatformAlarm(const HikPlatformAlarm & rv) {}
    HikPlatformAlarm & operator=(const HikPlatformAlarm & rv) {}

	void   RecvAlarmMsg(S32 s32Sock);

private:
	static HikPlatformAlarm * m_instance;
	Mutex m_mtxAlarm;
    map<void *, void *> m_mapToAlarmUser;
	S32  m_s32TcpListenFd;
	HANDLE  m_trdTcpListen;
	volatile bool m_bExit;
};

#endif
