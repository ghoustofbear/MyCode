#ifndef  __HIKPLATFORM_PREVIEW_H__
#define  __HIKPLATFORM_PREVIEW_H__

#include "ut_net.h"
#include "concourrencyc1.h"
#include "VideoSDK.h"
#include "basepview.h"

class HikPlatformConn;
class HikPlatformPreview : public BasePView
{
public:

	friend void CALLBACK PreviewMsgCallBack(VIDEO_INT64 i64PlayHandle, int iMsg, void* pUserData);

	friend void CALLBACK PreviewDecodedDataCallback(VIDEO_INT64 i64PlayHandle, const char* pDataArray, int iDataLen, int iWidth, int iHeight, 
								  int iFrameType, int iTimeStamp, void* pUserData);

	friend void CALLBACK PreviewStreamCallBack(VIDEO_INT64 i64PlayHandle, int iStreamDataType, const char* pDataArray, int iDataLen, void* pUserData);

	HikPlatformPreview(HikPlatformConn * pConn);

    ~HikPlatformPreview();

    //HikPlatformPreview * Start(const NET_PVIEW_PARAM_S *pParam);

    PreviewClient *Start(const NET_PVIEW_PARAM_S *pParam);

	BOOL           Stop(PreviewClient* pPViewClient);

	BOOL           QueryStatus();

	BOOL           OpenSound();

	BOOL           CloseSound();

	static VOID *  PreviewFun(VOID *pArg);
	VOID           PreviewLoop();

	BOOL           OpenStream();

	VOID           RecvStream();

	BOOL           CapturePicture(S8 *ps8PicFileName, U32 u32CaptueMode);

	VOID           SetDecDataCallBack(HANDLE hDecCb, HANDLE hUser);

	/* 开始实时录像 */
	BOOL           SaveRealData(S8 *ps8FileName);

	/* 停下实时录像 */
	BOOL           StopSaveRealData();
private:
	HikPlatformConn * m_pConn;
	Mutex             m_mtxPView;
	THREAD            m_trdPView;
	NET_PVIEW_PARAM_S m_PView;
	volatile BOOL     m_bExit;                     
	time_t            m_tmPrevTime;
	string            m_strUrl;
	__int64           m_PlayerId;
	volatile BOOL     m_bPreviewEx;
	HANDLE            m_hDecCallback;                      
	HANDLE            m_hUser; 
	time_t            m_tmVideoTime;
	PreviewClient     m_PViewClient;
	S8                m_s8DeviceName[24];
	S8                m_s8DeviceIp[24];
    int               m_bSaveRealData;
};

#endif
