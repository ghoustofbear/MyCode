/**
 * (C) Copyright 2012, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Thu Aug  9 10:46:20 CST 2012
 *
 * Dualtalk defines.
 */
#ifndef _DUALTALK_H__
#define _DUALTALK_H__

#include "commonc1.h"
#include "net_msg.h"
#include "ut_net.h"
#include "basedualtalk.h"

#define CONN_TIMEOUT	  5000
#define SEND_TIMEOUT	  5000
#define RECV_TIMEOUT	  5000
#define DOWNLOAD_TIMEOUT  10000
#define MSG_CAP_START     1
#define MSG_CAP_STOP      2

class Connection;
class Dualtalk : public BaseDualTalk
{
	public:
		Dualtalk(Connection *pConn);
		virtual ~Dualtalk();

		friend int CALLBACK DualSpeakCallback(int size, const char* data, DWORD dwUser);
		friend VOID CapDataCallback(long lDataType, BYTE *pData, long lDataSize, VOID *pUser);

		enum
		{
			TALK_MODE_ACTIVE  = 1, 
			TALK_MODE_PASSIVE
		};

		/* 启动对讲信道 */
		S32    Start();

		/* 关闭对讲连接 */
		VOID   Stop();

		/* 设置对讲参数 */
		VOID   SetDTalkParam(
			U32                 u32CollectFlag,
			HWND                hLWnd,
			HWND                hRWnd,
			HWND                hMWnd,
			pfnDTalkMsgCallback fDTalkMsgCallback, 
			pfnRealDataCallback fDTalkDataCallback, 
			VOID               *pUser);

		/* 发送对讲消息 */
		BOOL SendDTalkMsg(const NET_DTALK_MSG_S *pParam, const NET_DTALK_USER_S *pUserList);

		BOOL SendDTalkData(U8 *pu8Data, U32 u32DataSize);

		BOOL StartCapture();

		/* 消息传输操作 */
		BOOL Transaction(const MsgHeadWrap &reqHead, const VOID *reqData, MsgBuf &rspMsg, DWORD milliseconds = 5000)
		{
			MsgTransaction trans;
			trans.Init(reqHead, rspMsg);
			return Transaction(trans, reqData, milliseconds);
		}
		
		/* 消息传输操作 */
		BOOL Transaction(MsgTransaction &msgTrans, const VOID *reqData = NULL, DWORD milliseconds = 5000)
  		{
			m_MsgTransQueue.Req(msgTrans);

			BOOL hasRsp = FALSE;
			if (SendReq(*msgTrans.m_pReq, (VOID *)reqData))
				hasRsp = msgTrans.WaitRsp(milliseconds);

			m_MsgTransQueue.Clear(msgTrans);
			return hasRsp;
		}
		/*内部调用 */
	protected:
		/* 连接操作 */
		BOOL OpenStream();

		VOID StopMedia();

		S32  StartDualtalkLoop();

		/* 对讲消息循环 */
		VOID DualtalkLoop();

		VOID BeginTransaction();

		VOID EndTransaction();

		BOOL SendReq(const MsgHeadWrap &head, VOID *reqData = NULL);

		static VOID *ThreadEntry(VOID *pArg);

	public:
	private:
		Connection        * m_pConn;                     /**< 设备主连接类指针 */
		BOOL volatile       m_bExit;                     /**< 是否退出 */
		BOOL volatile       m_bOpened;                   /**< 是否已经打开 */
		BOOL volatile       m_bTalking;                  /**< 是否正在对讲 */

		THREAD              m_trdTalk;                   /**< 对讲通道线程ID */

		SyncSocket          m_DTalkSock;                 /**< 对讲链路 */

		MsgTransQueue       m_MsgTransQueue;             /**< 消息队列 */
		U32                 m_u32TalkMode;               /**< 对讲模式, TALK_MODE_ACTIVE-主动 TALK_MODE_PASSIVE-被动 */
		U32                 m_u32TalkType;               /**< 当前对讲类型 0-音频对讲 1-视频对讲 2-音视频对讲 3-音频喊话 4-视频喊话 5-音视频喊话 */

		S32                 m_iPlayerAudio;
		S32                 m_iPlayerVideo;
		S32                 m_iLocalCamera;
		U8                  m_u8Buffer[512*1014];

		HWND                m_hLocalWnd;                 /**< 视频会话本地图像显示窗口 */
		HWND                m_hRemoteWnd;                /**< 视频会话对方图像显示窗口 */
		HWND                m_hMsgWnd;                   /**< 视频采集消息通知窗口 */
		pfnDTalkMsgCallback m_fMsgCallback;              /**< 对讲消息回调 */
		pfnRealDataCallback m_fDTalkDataCallback;        /**< 对讲数据回调 */
		VOID              * m_pMsgUser;                  /**< 回调用户数据 */

		U32                 m_u32CollectFlag;            /**< 内部采集标记 bit0-采集音频 bit1-采集视频 */
};

#endif

