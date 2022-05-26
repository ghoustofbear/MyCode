#include "hikplatform_alarm.h"
#include "commonc1.h"

HikPlatformAlarm * HikPlatformAlarm::m_instance = 0;

HikPlatformAlarm * HikPlatformAlarm::getInstance()
{
	if (0 == m_instance)
	{
		m_instance = new HikPlatformAlarm();
	}

	return m_instance;
}

bool HikPlatformAlarm::startService(char * ip, int port)
{
	m_bExit = false;

    char servPort[64] = {0};
	sprintf(servPort, "%d", port);

	m_s32TcpListenFd = TcpListen(NULL, servPort, NULL);
	if (m_s32TcpListenFd < 0)
	{
		return false;
	}

	CreateJoinableThread(TcpListenPro, this, &m_trdTcpListen);

	return true;
}

VOID * HikPlatformAlarm::TcpListenPro(VOID *pArg)
{
    HikPlatformAlarm * pThis = (HikPlatformAlarm *)pArg;

	pThis->TcpListenLoop();

	return 0;
}

void HikPlatformAlarm::registerAlarm(void * pfnAlarmCallback, void * pUser)
{
     AutoLock lock(m_mtxAlarm);

	 map<void*, void*>::iterator iter = m_mapToAlarmUser.find(pfnAlarmCallback);
	 if (iter == m_mapToAlarmUser.end())
	 {
		 m_mapToAlarmUser[pfnAlarmCallback] = pUser;
	 }
}

VOID HikPlatformAlarm::TcpListenLoop()
{
	S32         s32Ret = -1;
	S32         s32SockFd;
	sockaddr_in stPeerAddr;
	S32         s32SALen = sizeof(stPeerAddr);
	S8          s8PeerIP[MAX_IP_STR_LEN]     = {0};
	S8          s8PeerPort[MAX_PORT_STR_LEN] = {0};
	
	while (!m_bExit)
	{
		s32SockFd = accept( m_s32TcpListenFd, (sockaddr *)&stPeerAddr, (socklen_t *)&s32SALen );
		if ( s32SockFd < 0)
		{
			continue;
		}

        RecvAlarmMsg(s32SockFd);
	}
}

void HikPlatformAlarm::RecvAlarmMsg(S32 s32Sock)
{
	char buf[100*1024] = {0};
	S32 s32Ret = ReadData(s32Sock, buf, sizeof(buf), 2);
	if (0 == s32Ret)
	{

	}

	closesocket(s32Sock);
}