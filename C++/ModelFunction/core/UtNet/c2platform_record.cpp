// c2platform_record.cpp: implementation of the c2platform_record class.
//
//////////////////////////////////////////////////////////////////////


#include "c2platform_record.h"
#include "c2platform_conn.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

C2PlatformRecord::C2PlatformRecord(C2PlatformConn *pConn)
:BaseRecord()
{
	m_pConn = pConn;
	m_hRecord = NULL;
	m_pfnRecordDataCallback = NULL;
	m_pfnRecordPlaybackStatusCallback = NULL;
	m_tmBeginSec = 0;
	m_tmEndSec = 0;
	m_dwPlayedTime = 0;
}

C2PlatformRecord::~C2PlatformRecord()
{

}

//************************************************************************
//函数名称：C2RecordStreamCallback(HWND wnd, void *data, uint32_t size, void *userdata)
//函数说明：码流数据回调函数
//函数参数：
//    (in) wnd      当前的预览/回放句柄
//    (in) data     存放数据的缓冲区指针, ut_frame_s+音视频数据
//    (in) size     缓冲区大小
//    (in) userdata 用户数据
//返回值：成功返回true，失败返回false
//************************************************************************/
bool C2RecordStreamCallback(HWND wnd, void *data, uint32_t size, void *userdata)
{
	C2PlatformRecord * pRecord = (C2PlatformRecord *)userdata;

	if(NULL != pRecord->m_pfnRecordDataCallback)
	{
		BOOL bRet = pRecord->m_pfnRecordDataCallback(wnd, data, size, pRecord->m_pUserRecord);
		if(TRUE == bRet)
		{
			return true;
		}
	}

	return false;
}

//************************************************************************
//函数名称：C2RecordStatusCallback(int code, void* userdata)
//函数说明：状态回调
//函数参数：
//    (in) code     状态码
//    (in) userdata 用户数据 
//返回值：void
//************************************************************************/
void C2RecordStatusCallback(int code, void* userdata)
{
	C2PlatformRecord * pRecord = (C2PlatformRecord *)userdata;
	int iCode = 0;
	switch(code)
	{
	case CODE_STREAM_END:
		break;	
	}
	if(NULL != pRecord->m_pfnRecordPlaybackStatusCallback)
	{
		pRecord->m_pfnRecordPlaybackStatusCallback(iCode, pRecord->m_pUserStatus);
	}	
}

//************************************************************************
//函数名称：Start(const NET_RECPLAY_PARAM_S *pParam)
//函数说明：远程回放
//函数参数：
//    (in) pParam 回放结构体
//返回值：成功返回TRUE/失败返回FALSE
//************************************************************************/
BOOL C2PlatformRecord::Start(const NET_RECPLAY_PARAM_S *pParam)
{ 	
	if((NULL != pParam) && (NULL != m_pConn) && (NULL != m_pConn->m_hLogin))
	{
		switch(pParam->u8PlayMode)
		{
		case REC_PLAY_IP_TIME: // 0-IP+PORT+时间段回放
			{
				c2_pb_time_s pbTime = {0};
				
				pbTime.channelNo = pParam->stIpMode.u32ChnNO;
			
				switch(pParam->stIpMode.u32Type)
				{
				case RECORD_NORMAL: //0x1 常规录像
					{
						pbTime.type = 0;
					}
					break;
					
				case RECORD_ALARM: //0x2 报警录像
					{
						pbTime.type = 3;   //报警录像
					}
					break;
					
				case RECORD_TIME://0x4 定时录像
					{
						pbTime.type = 1;   //计划录像
					}
					break;
					
				case RECORD_ALL://0xf 所有录像
					{
						pbTime.type = 0;   //所有录像
					}
					break;	
				default:
					{
						pbTime.type = 0;   //所有录像
					}
					break;
				}			
				pbTime.obtainType = 0;// 0-IP/PORT+私有码流, 1-IP/PORT+标准码流 2-地址码+私有码流, 3-地址码+标准码流
				
				pbTime.start.year = pParam->stIpMode.stBegin.u32Year;
				pbTime.start.month = pParam->stIpMode.stBegin.u32Month;
				pbTime.start.day = pParam->stIpMode.stBegin.u32Day;
				pbTime.start.hour = pParam->stIpMode.stBegin.u32Hour;
				pbTime.start.minute = pParam->stIpMode.stBegin.u32Minute;
				pbTime.start.second = pParam->stIpMode.stBegin.u32Second;
				
				pbTime.end.year = pParam->stIpMode.stEnd.u32Year;
				pbTime.end.month = pParam->stIpMode.stEnd.u32Month;
				pbTime.end.day = pParam->stIpMode.stEnd.u32Day;
				pbTime.end.hour = pParam->stIpMode.stEnd.u32Hour;
				pbTime.end.minute = pParam->stIpMode.stEnd.u32Minute;
				pbTime.end.second = pParam->stIpMode.stEnd.u32Second;
				
				pbTime.deviceHost = pParam->stIpMode.ps8DeviceIp;
				pbTime.devicePort = pParam->stIpMode.u16DevicePort;
				pbTime.subServHost = pParam->stIpMode.ps8SubServIp;
				pbTime.subServPort = pParam->stIpMode.u16SubServPort;
				pbTime.addrcode = NULL;
				
				m_pfnRecordDataCallback = pParam->fRecordDataCallback;
				m_pUserRecord = pParam->pUser;
			
				struct tm timeinfo;
				timeinfo.tm_year = pParam->stIpMode.stBegin.u32Year  - 1900;
				timeinfo.tm_mon  = pParam->stIpMode.stBegin.u32Month - 1;
				timeinfo.tm_mday = pParam->stIpMode.stBegin.u32Day;
				timeinfo.tm_hour = pParam->stIpMode.stBegin.u32Hour  ;
				timeinfo.tm_min  = pParam->stIpMode.stBegin.u32Minute;
				timeinfo.tm_sec  = pParam->stIpMode.stBegin.u32Second;
				
				m_tmBeginSec     = mktime(&timeinfo);
				
				timeinfo.tm_year = pParam->stIpMode.stEnd.u32Year  - 1900;
				timeinfo.tm_mon  = pParam->stIpMode.stEnd.u32Month - 1;
				timeinfo.tm_mday = pParam->stIpMode.stEnd.u32Day;
				timeinfo.tm_hour = pParam->stIpMode.stEnd.u32Hour  ;
				timeinfo.tm_min  = pParam->stIpMode.stEnd.u32Minute;
				timeinfo.tm_sec  = pParam->stIpMode.stEnd.u32Second;
				
				m_tmEndSec       = mktime(&timeinfo);
				
				m_hRecord = c2_playback_bytime(m_pConn->m_hLogin, &pbTime,  pParam->hWnd, C2RecordStreamCallback, this, C2RecordStatusCallback, this);
				
				return TRUE;
			}
			break;
		case REC_PLAY_IP_NAME: //1-IP_PORT+文件回放
			{
				c2_pb_name_s pbName = {0};
				pbName.filename = pParam->stIpMode.ps8FileName;
				pbName.obtainType = 0;// 0-IP/PORT+私有码流, 1-IP/PORT+标准码流 2-地址码+私有码流, 3-地址码+标准码流
				
				pbName.deviceHost = pParam->stIpMode.ps8DeviceIp;
				pbName.devicePort = pParam->stIpMode.u16DevicePort;
				pbName.subServHost = pParam->stIpMode.ps8SubServIp;
				pbName.subServPort = pParam->stIpMode.u16SubServPort;
				pbName.addrcode = NULL;

				m_pfnRecordDataCallback = pParam->fRecordDataCallback;
				m_pUserRecord = pParam->pUser;
				
				m_hRecord = c2_playback_byname(m_pConn->m_hLogin, &pbName,  pParam->hWnd, C2RecordStreamCallback, this, C2RecordStatusCallback, this);
				
				return TRUE;
			}
			break;
		case REC_PLAY_ADDR_URL: //2-地址码+RTSP URL回放
			{
				
			}
			break;
		case REC_PLAY_ADDR_TIME: //3-地址码+时间回放
			{
				c2_pb_time_s pbTime = {0};
								
				pbTime.channelNo = 0;
				pbTime.type = 0;	//0-所有录像 1-计划录像 2 手动录像 3-报警录像
				pbTime.obtainType = 0;// 0-IP/PORT+私有码流, 1-IP/PORT+标准码流 2-地址码+私有码流, 3-地址码+标准码流
				
				pbTime.start.year = pParam->stAddrMode.stBegin.u32Year;
				pbTime.start.month = pParam->stAddrMode.stBegin.u32Month;
				pbTime.start.day = pParam->stAddrMode.stBegin.u32Day;
				pbTime.start.hour = pParam->stAddrMode.stBegin.u32Hour;
				pbTime.start.minute = pParam->stAddrMode.stBegin.u32Minute;
				pbTime.start.second = pParam->stAddrMode.stBegin.u32Second;
				
				pbTime.end.year = pParam->stAddrMode.stEnd.u32Year;
				pbTime.end.month = pParam->stAddrMode.stEnd.u32Month;
				pbTime.end.day = pParam->stAddrMode.stEnd.u32Day;
				pbTime.end.hour = pParam->stAddrMode.stEnd.u32Hour;
				pbTime.end.minute = pParam->stAddrMode.stEnd.u32Minute;
				pbTime.end.second = pParam->stAddrMode.stEnd.u32Second;
				 
				pbTime.addrcode = pParam->stAddrMode.ps8FrontAddrCode;

				m_pUserRecord = pParam->pUser;
				m_pfnRecordDataCallback = pParam->fRecordDataCallback;

				struct tm timeinfo;
				timeinfo.tm_year = pParam->stIpMode.stBegin.u32Year  - 1900;
				timeinfo.tm_mon  = pParam->stIpMode.stBegin.u32Month - 1;
				timeinfo.tm_mday = pParam->stIpMode.stBegin.u32Day;
				timeinfo.tm_hour = pParam->stIpMode.stBegin.u32Hour  ;
				timeinfo.tm_min  = pParam->stIpMode.stBegin.u32Minute;
				timeinfo.tm_sec  = pParam->stIpMode.stBegin.u32Second;
				
				m_tmBeginSec = mktime(&timeinfo);
				
				timeinfo.tm_year = pParam->stIpMode.stEnd.u32Year  - 1900;
				timeinfo.tm_mon  = pParam->stIpMode.stEnd.u32Month - 1;
				timeinfo.tm_mday = pParam->stIpMode.stEnd.u32Day;
				timeinfo.tm_hour = pParam->stIpMode.stEnd.u32Hour  ;
				timeinfo.tm_min  = pParam->stIpMode.stEnd.u32Minute;
				timeinfo.tm_sec  = pParam->stIpMode.stEnd.u32Second;
				
				m_tmEndSec = mktime(&timeinfo);
				
				m_hRecord = c2_playback_bytime(m_pConn->m_hLogin, &pbTime,  pParam->hWnd, C2RecordStreamCallback, this, C2RecordStatusCallback, this);
				
				return TRUE;
			}
			break;
		case REC_PLAY_HIKPLATFORM: //海康平台回放
			{
				
			}
			break;
		}
	}
	return FALSE;
}

//************************************************************************
//函数名称：Stop()
//函数说明：停止某台设备某个通道的远程回放
//函数参数：无
//返回值：void
//************************************************************************/
VOID C2PlatformRecord::Stop()
{ 
	if((NULL != m_hRecord) && (NULL != m_pConn) && (NULL != m_pConn->m_hLogin))
	{
		c2_stop_playback(m_pConn->m_hLogin, m_hRecord);		
	}
}

//************************************************************************
//函数名称：PlayCtrl(U32 u32Code, VOID *pInVal, U32 u32InSize, VOID *pOutVal, U32 *u32OutSize)
//函数说明：控制录像回放的状态
//函数参数：
//    (in) u32Code 控制录像回放状态命令
//    (in) pInVal 指向输入参数的指针
//    (in) u32InSize 输入参数的长度
//    (in) pOutVal 指向输出参数的指针
//    (in) u32OutSize 输出参数的长度
//返回值：成功返回TRUE/失败返回FALSE
//************************************************************************/
BOOL C2PlatformRecord::PlayCtrl(U32 u32Code, VOID *pInVal, U32 u32InSize, VOID *pOutVal, U32 *u32OutSize)
{
	if(NULL !=  m_hRecord)
	{
		int code = PLAY_NONE;
		switch(u32Code)
		{		
		case NET_PLAYSTART:/**< 开始播放 */
			{
				code = PLAY_START;
			}
			break;
		case NET_PLAYSTOP:/**< 停止播放 */
			{
				code = PLAY_STOP;
			}
			break;
		case NET_PLAYPAUSE:/**< 暂停播放 */
			{
				code = PLAY_PAUSE;
			}
			break;
		case NET_PLAYRESTART:/**< 恢复播放 */
			{
				code = PLAY_RESTART;
			}
			break;
		case NET_PLAYFAST:/**< 快放 */
			{
				code = PLAY_FAST;
			}
			break;
		case NET_PLAYSLOW:/**< 慢放 */
			{
				code = PLAY_SLOW;
			}
			break;
		case NET_PLAYNORMAL:/**< 正常速度 */
			{
				code = PLAY_NORMAL;
			}
			break;
		case NET_PLAYSTARTAUDIO:/**< 打开声音 */
			{
				code = PLAY_OPENAUDIO;		  
			}
			break;
		case NET_PLAYSTOPAUDIO:/**< 关闭声音 */
			{	
				code = PLAY_CLOSEAUDIO;
			}
			break;
		case NET_PLAYSETPOS:/**< 改变文件回放的进度 */
			{

                c2_playback_control(m_hRecord, PLAY_STARTREPOS, NULL, 0);
                Sleep(100);

                long lRet = c2_playback_control(m_hRecord, PLAY_SETPOS, pInVal, u32InSize);
                if(0 == lRet)
                {
                    return TRUE;
                }
                else
                {
                    return FALSE;
                }
#if 0
				//code = PLAY_SETPOS;
				datetime_s time;
				struct tm * timeinfo;

				int pos = *(int *)pInVal;
				time_t tmOffset = m_tmBeginSec + ((m_tmEndSec - m_tmBeginSec) * pos) / 100;

				timeinfo = localtime(&tmOffset);
				
				time.year   = timeinfo->tm_year + 1900;
				time.month  = timeinfo->tm_mon  + 1;
				time.day    = timeinfo->tm_mday;
				time.hour   = timeinfo->tm_hour;
				time.minute = timeinfo->tm_min;
				time.second = timeinfo->tm_sec;

				code = PLAY_SETTIME;

				long lRet = c2_playback_control(m_hRecord, code, &time, sizeof(datetime_s));
				if(0 == lRet)
				{
					return TRUE;
				}
				else
				{
					return FALSE;
				}
#endif
			}
			break;
		case NET_PLAYSETTIME:/**< 按绝对时间定位 */
			{
				code = PLAY_SETTIME;
			}
			break;
		case NET_PLAYSTEP:/**< 单帧播放 */
			{
				code = PLAY_STEP;
			}
			break;		
        case NET_STARTREPOS: /**< 开始设置进度 */
            {
                code = PLAY_STARTREPOS;
            }
        break;
		}

		long lRet = c2_playback_control(m_hRecord, code, pInVal, u32InSize);
		if(0 == lRet)
		{
			return TRUE;
		}
	}
	return FALSE;
}

//************************************************************************
//函数名称：GetPlayedTime()
//函数说明：获取播放时间
//函数参数：无
//返回值：DWORD 返回值 录像已回放时间
//************************************************************************/
DWORD C2PlatformRecord::GetPlayedTime()
{	
	if((NULL != m_hRecord) && (NULL != m_pConn) && (NULL != m_pConn->m_hLogin))
	{
		datetime_s playTime = {0};
		long lRet = c2_get_playbacktime(m_hRecord, &playTime);	
		if(0 == lRet)
		{
			struct tm timeinfo;
			timeinfo.tm_year = playTime.year - 1900;
			timeinfo.tm_mon = playTime.month - 1;
			timeinfo.tm_mday = playTime.day;
			timeinfo.tm_hour = playTime.hour  ;
			timeinfo.tm_min = playTime.minute;
			timeinfo.tm_sec = playTime.second;
			
            time_t tmPlayTimeNow = mktime(&timeinfo);

			if (tmPlayTimeNow < m_tmBeginSec)
			{
				return m_dwPlayedTime;
			}			
			m_dwPlayedTime = tmPlayTimeNow - m_tmBeginSec;

			return m_dwPlayedTime;
		}
	}
	return 0;
}

//************************************************************************
//函数名称：SetRecPlaybackStatusCb(pfnRecordPlaybackStatusCallback fnRecPbStatusCb, VOID *pUser)
//函数说明：设置录像回放状态回调函数
//函数参数：
//    (in) fnRecPbStatusCb 录像播放状态回调函数
//    (in) pUser 用户数据
//返回值：成功返回TRUE/失败返回FALSE
//************************************************************************/
BOOL C2PlatformRecord::SetRecPlaybackStatusCb(pfnRecordPlaybackStatusCallback fnRecPbStatusCb, VOID *pUser)
{	
	m_pfnRecordPlaybackStatusCallback = fnRecPbStatusCb;
	m_pUserStatus = pUser;
	return FALSE;
}


//打开声音
BOOL C2PlatformRecord::OpenSound()
{
	return FALSE;
}

//关闭声音
BOOL C2PlatformRecord::CloseSound()
{
	return FALSE;
}

//录像数据接收主线程
VOID C2PlatformRecord::RecordPlayLoop()
{

}

//录像数据请求
BOOL C2PlatformRecord::OpenStream()
{
	return FALSE;
}
