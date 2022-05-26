/**
 * (C) Copyright 2012, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Thu Aug  9 10:46:20 CST 2012
 *
 *  defines.
 */
#ifndef __PLAYBACK_H__ 
#define __PLAYBACK_H__
#include "commonc1.h"
#include "net_msg.h"
#include "ut_net.h"
#include "baserecord.h"

#include "helmet_interface_player.h"
#include <list>
using namespace std;


class Connection;

class RecordPlay : public BaseRecord
{
#ifdef _WIN32
	friend BOOL RecordDataCallback(HWND hWnd, VOID *pData, U32 u32Size, VOID *pUser);
	friend void CALLBACK SourceBufFun(long nPort, DWORD nBufSize, DWORD dwUser, void *pContext);
	friend void CALLBACK SourceBufFun(UINT message, void* userParam);
	friend VOID *PlayCtrlFun(VOID *pArg);
	friend VOID *HelmetPlayCtrlFun(VOID *pArg);
#endif

public:
	/* �������Ͷ��� */
	enum
	{
		MANU_TYPE_HIK,
		MANU_TYPE_DAHUA,	
		MANU_TYPE_UT,
		MANU_TYPE_TIANDY,
	};

	/* ý��֡���Ͷ��� */
	enum
	{
		FRAME_TYPE_HIK,
		FRAME_TYPE_DAHUA,	
		FRAME_TYPE_UT,
		FRAME_TYPE_TIANDY,
		FRAME_TYPE_HI_H264,
		FRAME_TYPE_STD_H264,
		FRAME_TYPE_STD_H264_FU,
		FRAME_TYPE_STD_H264_FU_PAYLOAD,
		FRAME_TYPE_HELMET_GX,
	};

	/* ���캯�� */
	RecordPlay(Connection *pConn);

	/* ��ʼ�ط� */
	BOOL Start(const NET_RECPLAY_PARAM_S *pParam);

	/* ֹͣ�ط� */
	VOID Stop();

	/* ���ſ��� */
	BOOL PlayCtrl(U32 u32Code, VOID *pInVal = NULL, U32 u32InSize = 0, VOID *pOutVal = NULL, U32 *u32OutSize = NULL);

	/* �ض�λ */
	VOID ReposRecordData(const REC_REPOSE_REQ_S *pReq);

	/* ¼���Ѳ���ʱ�� */
	DWORD GetPlayedTime();

    /* ����¼��ط�״̬�ص�  */
	BOOL  SetRecPlaybackStatusCb(pfnRecordPlaybackStatusCallback fnRecPbStatusCb, VOID *pUser);

	BOOL  OpenSound();

    BOOL  CloseSound();

protected:
	static VOID *RecordPlayFun(VOID *pArg);
	static VOID *SendFun(VOID *pArg);

	/* ����¼������ */
	VOID    ReqRecordData();

	/* ¼�����ݽ������߳� */
	VOID   RecordPlayLoop();

	/* ¼���������� */
	BOOL   OpenStream();

	/* ¼�����ݽ��ղ��� */
	VOID   Recv();

	VOID   Send();

	/* ��Ϣ���Ͳ��� */
	BOOL   SendMsg(const MsgHeadWrap &head, const VOID *data = NULL);

public:

protected:

	NET_RECPLAY_PARAM_S m_stRecPlayParam;

	string          m_strDeviceIp;
	string          m_strSubServIp;
	string          m_strFileName;
	string          m_strRtspUrl;
	string          m_strFrontAddrCode;

	Connection    * m_pConn;                       /**< ������ */

	BOOL volatile   m_bAutoReconn;                 /**< �Ƿ����� */
	BOOL volatile   m_bCanSendMsg;
	BOOL volatile   m_bNeedData;                   /**< �Ƿ���Ҫ���� */
	BOOL volatile   m_bNoData;                     /**< �Ƿ��ļ����� */
	BOOL volatile   m_bReposing;                   /**< �Ƿ����ض�λ�� */
	BOOL volatile   m_bExit;                       /**< �Ƿ��˳� */

	U32             m_u32PlayMode;                 /**< �ط�ģʽ 0-��ʱ��� 1-���ļ� 2-��RTSP URL�ط� */

	U64             m_LastKeyFrameTime;
	UT_FRAME_HEAD_S m_LastRecvFrame;

	THREAD          m_trdRecPlay;                  /**< ¼�����ݽ����߳�ID */
	THREAD          m_trdSend;                     /**< ����������Ϣ�����߳�ID */
	
	Event           m_evReposDone;                 /**< �ض�λͬ���¼� */
	Mutex           m_mtxRepos;                    /**< �ض�λͬ���� */

	list<S8 *>      m_lsMsgSendQueue;              /**< ����������Ϣ���Ͷ��� */
	Semaphore       m_semMsgSendQueue;
	Mutex           m_mtxMsgSendQueue;

	SyncSocket      m_sockRecPlay;                 /**< �ط�ͨ����· */
	Mutex           m_mtxSock;
	Mutex           m_mtxReqData;

	time_t          m_tmBeginSec;                  /**< ��ʼʱ�� */
	time_t          m_tmEndSec;                    /**< ����ʱ��ʱ�� */
	U32             m_u32Pos;                      /**< ���Ž��� */
	time_t          m_tmHelmetBeginGx;

	U32             m_u32ManuType;                 /**< �������� */
	U32             m_u32FrameType;                /**< ֡���� */

	LONG            m_lPort;

	int             m_iPlayerID;
	int             m_iDataLen;
	unsigned char   m_cDataBuffer[1600];
	THREAD          m_trdPlayCtrl;
	BOOL            m_bTrdPlayCtrlExit;
	BOOL            m_bDownloadPause;
	S32             m_iPlaySpeed;
	DWORD           m_dwFirstUTFrameTime;         /**< H264�豣���һ֡ʱ�� */
	DWORD           m_dwPlayedTime;
    DWORD           m_dwPreTagTime;               /**< �������϶�ʱ�� */
	BOOL            m_bFileDownloadEnd;           /**< �ļ��ط����ؽ�������ʱ¼���������� */
	pfnRecordPlaybackStatusCallback m_pfnRecordPbStatusCb;
	VOID           *m_pUser;

	VARENDER_HANDLE m_hPlayerHandle;             /**< ����Ƶ��� */
     
	Mutex               m_mtxHelmet;
	list< void * >      m_listHelmetData;
	HANDLE              m_hHelmetPlayCtrl;
	bool                m_bHelmetPlayCtrlTime;
};	
#endif

