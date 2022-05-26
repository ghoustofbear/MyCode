#ifndef  __HIKPLATFORM_RECORD_H__
#define  __HIKPLATFORM_RECORD_H__

#include "ut_net.h"
#include "VideoSDK.h"
#include "baserecord.h"

class HikPlatformConn;
class HikPlatformRecord : public BaseRecord
{
public:

	friend void CALLBACK RecordMsgCallBack(VIDEO_INT64 i64PlayHandle, int iMsg, void* pUserData);
	
	friend void CALLBACK RecordDecodedDataCallback(VIDEO_INT64 i64PlayHandle, const char* pDataArray, int iDataLen, int iWidth, int iHeight, 
		int iFrameType, int iTimeStamp, void* pUserData);
	
	friend void CALLBACK RecordStreamCallBack(VIDEO_INT64 i64PlayHandle, int iStreamDataType, const char* pDataArray, int iDataLen, void* pUserData);

    HikPlatformRecord(HikPlatformConn * pConn);

	~HikPlatformRecord();

	BOOL  Start(const NET_RECPLAY_PARAM_S *pParam);

	VOID  Stop();

	BOOL  PlayCtrl(U32 u32Code, VOID *pInVal = NULL, U32 u32InSize = 0, VOID *pOutVal = NULL, U32 *u32OutSize = NULL);

	DWORD GetPlayedTime();

	BOOL  SetRecPlaybackStatusCb(pfnRecordPlaybackStatusCallback fnRecPbStatusCb, VOID *pUser);

	BOOL  OpenSound();
	
    BOOL  CloseSound();

protected:
	static VOID *RecordPlayFun(VOID *pArg);

	VOID   RecordPlayLoop();

	BOOL   OpenStream();

private:
	HikPlatformConn * m_pConn;
	THREAD          m_trdRecPlay;
	BOOL            m_bExit;
	volatile BOOL   m_bRecordExp;
	volatile BOOL   m_bDownloadOk;
	__int64         m_recPlayerId;
	pfnRecordPlaybackStatusCallback m_pfnRecordPbStatusCb;
	VOID           *m_pUser;
	time_t          m_tmBeginSec;
	time_t          m_tmEndSec; 
	S32             m_iPlaySpeed;
	string          m_strUrl;
	HWND            m_hWnd;
	DATETIME_S      m_stBeginTime;
	DATETIME_S      m_stEndTime;
	string          m_strDeviceIp;
	U16             m_u16DevicePort;
	U16             m_u16ChnNo;
};

#endif
