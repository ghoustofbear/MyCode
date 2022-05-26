#ifndef  __HIKPLATFORM_CONN_H__
#define  __HIKPLATFORM_CONN_H__

#include "ut_net.h"
#include "httppostintf.h"
#include "VideoSDK.h"
#include "commonc1.h"
#include "net_msg.h"
#include "concourrencyc1.h"
#include <map>
#include "baseplatfrom_conn.h"
using namespace std;

class HikPlatformConn : public BasePlatformConn
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
   
   /* 开始录像回放 */
   HANDLE   StartRecordPaly(const NET_RECPLAY_PARAM_S *pParam);	    
   
   /* 开始录像下载 */
   HANDLE   StartRecordDownload(const NET_RECDOWNLOAD_PARAM_S *pParam);	
   
   /* 根据时间 开始录像下载  */
   HANDLE   StartRecordDownloadByTime(const NET_RECDOWNLOAD_BYTIME_PARAM_S *pParam);
	
   /* 初始化设备对讲*/
   HANDLE InitDevTalk(int audioType);


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
	volatile BOOL  m_bExit;
    time_t m_tmPrev;
	VOID * m_pUser;
	S8    m_s8LoalIp[MAX_IPV6_LEN];
	BOOL  m_bConn;

public:
	Mutex  m_mtxCamIdx;
	map<string, string>  m_mapIpChnToCameraIndex;
	Mutex  m_mtxDevIdx;
	map<string, string>  m_mapIpPortToDevCode;
    map<string, string>  m_mapNameToDevCode;
    map<string, string>  m_mapIpPortChanToDevCode;
    S8                  *m_ps8CamName;
};

#endif
