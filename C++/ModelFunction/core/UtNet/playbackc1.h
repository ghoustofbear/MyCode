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
	/* 厂商类型定义 */
	enum
	{
		MANU_TYPE_HIK,
		MANU_TYPE_DAHUA,	
		MANU_TYPE_UT,
		MANU_TYPE_TIANDY,
	};

	/* 媒体帧类型定义 */
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

	/* 构造函数 */
	RecordPlay(Connection *pConn);

	/* 开始回放 */
	BOOL Start(const NET_RECPLAY_PARAM_S *pParam);

	/* 停止回放 */
	VOID Stop();

	/* 播放控制 */
	BOOL PlayCtrl(U32 u32Code, VOID *pInVal = NULL, U32 u32InSize = 0, VOID *pOutVal = NULL, U32 *u32OutSize = NULL);

	/* 重定位 */
	VOID ReposRecordData(const REC_REPOSE_REQ_S *pReq);

	/* 录像已播放时间 */
	DWORD GetPlayedTime();

    /* 设置录像回放状态回调  */
	BOOL  SetRecPlaybackStatusCb(pfnRecordPlaybackStatusCallback fnRecPbStatusCb, VOID *pUser);

	BOOL  OpenSound();

    BOOL  CloseSound();

protected:
	static VOID *RecordPlayFun(VOID *pArg);
	static VOID *SendFun(VOID *pArg);

	/* 请求录像数据 */
	VOID    ReqRecordData();

	/* 录像数据接收主线程 */
	VOID   RecordPlayLoop();

	/* 录像数据请求 */
	BOOL   OpenStream();

	/* 录像数据接收操作 */
	VOID   Recv();

	VOID   Send();

	/* 消息发送操作 */
	BOOL   SendMsg(const MsgHeadWrap &head, const VOID *data = NULL);

public:

protected:

	NET_RECPLAY_PARAM_S m_stRecPlayParam;

	string          m_strDeviceIp;
	string          m_strSubServIp;
	string          m_strFileName;
	string          m_strRtspUrl;
	string          m_strFrontAddrCode;

	Connection    * m_pConn;                       /**< 主链接 */

	BOOL volatile   m_bAutoReconn;                 /**< 是否重连 */
	BOOL volatile   m_bCanSendMsg;
	BOOL volatile   m_bNeedData;                   /**< 是否需要数据 */
	BOOL volatile   m_bNoData;                     /**< 是否文件结束 */
	BOOL volatile   m_bReposing;                   /**< 是否在重定位中 */
	BOOL volatile   m_bExit;                       /**< 是否退出 */

	U32             m_u32PlayMode;                 /**< 回放模式 0-按时间段 1-按文件 2-按RTSP URL回放 */

	U64             m_LastKeyFrameTime;
	UT_FRAME_HEAD_S m_LastRecvFrame;

	THREAD          m_trdRecPlay;                  /**< 录像数据接收线程ID */
	THREAD          m_trdSend;                     /**< 流量控制消息发送线程ID */
	
	Event           m_evReposDone;                 /**< 重定位同步事件 */
	Mutex           m_mtxRepos;                    /**< 重定位同步锁 */

	list<S8 *>      m_lsMsgSendQueue;              /**< 流量控制消息发送队列 */
	Semaphore       m_semMsgSendQueue;
	Mutex           m_mtxMsgSendQueue;

	SyncSocket      m_sockRecPlay;                 /**< 回放通信链路 */
	Mutex           m_mtxSock;
	Mutex           m_mtxReqData;

	time_t          m_tmBeginSec;                  /**< 开始时间 */
	time_t          m_tmEndSec;                    /**< 结束时间时间 */
	U32             m_u32Pos;                      /**< 播放进度 */
	time_t          m_tmHelmetBeginGx;

	U32             m_u32ManuType;                 /**< 厂商类型 */
	U32             m_u32FrameType;                /**< 帧类型 */

	LONG            m_lPort;

	int             m_iPlayerID;
	int             m_iDataLen;
	unsigned char   m_cDataBuffer[1600];
	THREAD          m_trdPlayCtrl;
	BOOL            m_bTrdPlayCtrlExit;
	BOOL            m_bDownloadPause;
	S32             m_iPlaySpeed;
	DWORD           m_dwFirstUTFrameTime;         /**< H264需保存第一帧时间 */
	DWORD           m_dwPlayedTime;
    DWORD           m_dwPreTagTime;               /**< 进度条拖动时间 */
	BOOL            m_bFileDownloadEnd;           /**< 文件回放下载结束，此时录像正播放中 */
	pfnRecordPlaybackStatusCallback m_pfnRecordPbStatusCb;
	VOID           *m_pUser;

	VARENDER_HANDLE m_hPlayerHandle;             /**< 音视频句柄 */
     
	Mutex               m_mtxHelmet;
	list< void * >      m_listHelmetData;
	HANDLE              m_hHelmetPlayCtrl;
	bool                m_bHelmetPlayCtrlTime;
};	
#endif

