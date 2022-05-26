#ifndef  __BASEPLATFORM_H__
#define  __BASEPLATFORM_H__

#include "ut_net.h"
#include "httppostintf.h"
#include "VideoSDK.h"
#include "common.h"
#include "net_msg.h"
#include "concourrency.h"
#include <map>
using namespace std;

class HikPlatformConn
{

#define HIKPLAT_CONNECTION_STATUS(code) \
	do \
	{ \
		 if (m_fConnCallback) \
		{ \
				m_fConnCallback(code, m_pUser); \
		} \
	} while (0);

public:
	HikPlatformConn();
   ~HikPlatformConn();

   BOOL   Start(const NET_CONN_PARAM_S *pParam);

   VOID   Stop();

   S8 *   GetIp()     { return m_s8Ip; }

   U16    GetPort()   { return m_u16Port; }
   
   S8 *   GetAppKey() { return m_s8AppKey; }
   
   S8 *   GetAppSec() { return m_s8AppSec; }

   HANDLE StartPreview(const NET_PVIEW_PARAM_S *pParam);
   
   VOID   StopPreview(HANDLE *hPView);
		
   BOOL   PTZControl(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam);  
   BOOL   PTZControlEx(U32 u32Cmd, const NET_PTZ_PARAM_S *pParam);

   BOOL   QueryRecord(const NET_QRECORD_PARAM_S *pParam);
   BOOL   QueryRecordEx(const NET_QRECORD_PARAM_EX_S *pParam);

   BOOL   SetDecDataCallBack(const NET_DECFRAME_PARAM *pParam);
 
   string FindCameraIndexByIp(string ip, int port, int chn);

   bool   RegisterAlarm();

protected:
	static VOID *MainMsgFun(VOID *pArg);
	
	VOID   MainMsgLoop();

private:
	BOOL   QueryDefaultOrg();

	BOOL   QueryResource();

private:
	S8    m_s8Ip[MAX_IPV6_LEN];
	U16   m_u16Port;
	S8    m_s8AppKey[MAX_NAME_LEN];
    S8    m_s8AppSec[MAX_NAME_LEN];
	HANDLE    m_mainMsgId;
	pfnAlarmCallback m_pfnAlarmCallback;
	pfnConnCallback  m_fConnCallback; 
	BOOL  m_bExit;
    time_t m_tmPrev;
	VOID * m_pUser;
	S8    m_s8LoalIp[MAX_IPV6_LEN];
	BOOL  m_bConn;

public:
	Mutex  m_mtxCamIdx;
	map<string, string>  m_mapIpChnToCameraIndex;
	Mutex  m_mtxDevIdx;
	map<string, string>  m_mapIpPortToDevCode;
};

#endif 
