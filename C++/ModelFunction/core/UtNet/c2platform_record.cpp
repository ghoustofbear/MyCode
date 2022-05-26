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
//�������ƣ�C2RecordStreamCallback(HWND wnd, void *data, uint32_t size, void *userdata)
//����˵�����������ݻص�����
//����������
//    (in) wnd      ��ǰ��Ԥ��/�طž��
//    (in) data     ������ݵĻ�����ָ��, ut_frame_s+����Ƶ����
//    (in) size     ��������С
//    (in) userdata �û�����
//����ֵ���ɹ�����true��ʧ�ܷ���false
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
//�������ƣ�C2RecordStatusCallback(int code, void* userdata)
//����˵����״̬�ص�
//����������
//    (in) code     ״̬��
//    (in) userdata �û����� 
//����ֵ��void
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
//�������ƣ�Start(const NET_RECPLAY_PARAM_S *pParam)
//����˵����Զ�̻ط�
//����������
//    (in) pParam �طŽṹ��
//����ֵ���ɹ�����TRUE/ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformRecord::Start(const NET_RECPLAY_PARAM_S *pParam)
{ 	
	if((NULL != pParam) && (NULL != m_pConn) && (NULL != m_pConn->m_hLogin))
	{
		switch(pParam->u8PlayMode)
		{
		case REC_PLAY_IP_TIME: // 0-IP+PORT+ʱ��λط�
			{
				c2_pb_time_s pbTime = {0};
				
				pbTime.channelNo = pParam->stIpMode.u32ChnNO;
			
				switch(pParam->stIpMode.u32Type)
				{
				case RECORD_NORMAL: //0x1 ����¼��
					{
						pbTime.type = 0;
					}
					break;
					
				case RECORD_ALARM: //0x2 ����¼��
					{
						pbTime.type = 3;   //����¼��
					}
					break;
					
				case RECORD_TIME://0x4 ��ʱ¼��
					{
						pbTime.type = 1;   //�ƻ�¼��
					}
					break;
					
				case RECORD_ALL://0xf ����¼��
					{
						pbTime.type = 0;   //����¼��
					}
					break;	
				default:
					{
						pbTime.type = 0;   //����¼��
					}
					break;
				}			
				pbTime.obtainType = 0;// 0-IP/PORT+˽������, 1-IP/PORT+��׼���� 2-��ַ��+˽������, 3-��ַ��+��׼����
				
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
		case REC_PLAY_IP_NAME: //1-IP_PORT+�ļ��ط�
			{
				c2_pb_name_s pbName = {0};
				pbName.filename = pParam->stIpMode.ps8FileName;
				pbName.obtainType = 0;// 0-IP/PORT+˽������, 1-IP/PORT+��׼���� 2-��ַ��+˽������, 3-��ַ��+��׼����
				
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
		case REC_PLAY_ADDR_URL: //2-��ַ��+RTSP URL�ط�
			{
				
			}
			break;
		case REC_PLAY_ADDR_TIME: //3-��ַ��+ʱ��ط�
			{
				c2_pb_time_s pbTime = {0};
								
				pbTime.channelNo = 0;
				pbTime.type = 0;	//0-����¼�� 1-�ƻ�¼�� 2 �ֶ�¼�� 3-����¼��
				pbTime.obtainType = 0;// 0-IP/PORT+˽������, 1-IP/PORT+��׼���� 2-��ַ��+˽������, 3-��ַ��+��׼����
				
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
		case REC_PLAY_HIKPLATFORM: //����ƽ̨�ط�
			{
				
			}
			break;
		}
	}
	return FALSE;
}

//************************************************************************
//�������ƣ�Stop()
//����˵����ֹͣĳ̨�豸ĳ��ͨ����Զ�̻ط�
//������������
//����ֵ��void
//************************************************************************/
VOID C2PlatformRecord::Stop()
{ 
	if((NULL != m_hRecord) && (NULL != m_pConn) && (NULL != m_pConn->m_hLogin))
	{
		c2_stop_playback(m_pConn->m_hLogin, m_hRecord);		
	}
}

//************************************************************************
//�������ƣ�PlayCtrl(U32 u32Code, VOID *pInVal, U32 u32InSize, VOID *pOutVal, U32 *u32OutSize)
//����˵��������¼��طŵ�״̬
//����������
//    (in) u32Code ����¼��ط�״̬����
//    (in) pInVal ָ�����������ָ��
//    (in) u32InSize ��������ĳ���
//    (in) pOutVal ָ�����������ָ��
//    (in) u32OutSize ��������ĳ���
//����ֵ���ɹ�����TRUE/ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformRecord::PlayCtrl(U32 u32Code, VOID *pInVal, U32 u32InSize, VOID *pOutVal, U32 *u32OutSize)
{
	if(NULL !=  m_hRecord)
	{
		int code = PLAY_NONE;
		switch(u32Code)
		{		
		case NET_PLAYSTART:/**< ��ʼ���� */
			{
				code = PLAY_START;
			}
			break;
		case NET_PLAYSTOP:/**< ֹͣ���� */
			{
				code = PLAY_STOP;
			}
			break;
		case NET_PLAYPAUSE:/**< ��ͣ���� */
			{
				code = PLAY_PAUSE;
			}
			break;
		case NET_PLAYRESTART:/**< �ָ����� */
			{
				code = PLAY_RESTART;
			}
			break;
		case NET_PLAYFAST:/**< ��� */
			{
				code = PLAY_FAST;
			}
			break;
		case NET_PLAYSLOW:/**< ���� */
			{
				code = PLAY_SLOW;
			}
			break;
		case NET_PLAYNORMAL:/**< �����ٶ� */
			{
				code = PLAY_NORMAL;
			}
			break;
		case NET_PLAYSTARTAUDIO:/**< ������ */
			{
				code = PLAY_OPENAUDIO;		  
			}
			break;
		case NET_PLAYSTOPAUDIO:/**< �ر����� */
			{	
				code = PLAY_CLOSEAUDIO;
			}
			break;
		case NET_PLAYSETPOS:/**< �ı��ļ��طŵĽ��� */
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
		case NET_PLAYSETTIME:/**< ������ʱ�䶨λ */
			{
				code = PLAY_SETTIME;
			}
			break;
		case NET_PLAYSTEP:/**< ��֡���� */
			{
				code = PLAY_STEP;
			}
			break;		
        case NET_STARTREPOS: /**< ��ʼ���ý��� */
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
//�������ƣ�GetPlayedTime()
//����˵������ȡ����ʱ��
//������������
//����ֵ��DWORD ����ֵ ¼���ѻط�ʱ��
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
//�������ƣ�SetRecPlaybackStatusCb(pfnRecordPlaybackStatusCallback fnRecPbStatusCb, VOID *pUser)
//����˵��������¼��ط�״̬�ص�����
//����������
//    (in) fnRecPbStatusCb ¼�񲥷�״̬�ص�����
//    (in) pUser �û�����
//����ֵ���ɹ�����TRUE/ʧ�ܷ���FALSE
//************************************************************************/
BOOL C2PlatformRecord::SetRecPlaybackStatusCb(pfnRecordPlaybackStatusCallback fnRecPbStatusCb, VOID *pUser)
{	
	m_pfnRecordPlaybackStatusCallback = fnRecPbStatusCb;
	m_pUserStatus = pUser;
	return FALSE;
}


//������
BOOL C2PlatformRecord::OpenSound()
{
	return FALSE;
}

//�ر�����
BOOL C2PlatformRecord::CloseSound()
{
	return FALSE;
}

//¼�����ݽ������߳�
VOID C2PlatformRecord::RecordPlayLoop()
{

}

//¼����������
BOOL C2PlatformRecord::OpenStream()
{
	return FALSE;
}
