/**
 * (C) Copyright 2012, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Thu Aug  9 10:46:20 CST 2012
 *
 * Dualtalk defines.
 */
#ifndef __HIKPLATFORM_DUALTALK_H__
#define __HIKPLATFORM_DUALTALK_H__

#include "ut_net.h"
#include "VideoSDK.h"
#include "basedualtalk.h"

class HikPlatformConn;
class HikPlatformDualtalk : public BaseDualTalk
{
	public:
		HikPlatformDualtalk(HikPlatformConn *pConn);
		virtual ~HikPlatformDualtalk();
	
		/* 启动对讲信道 */
	S32    Start();

		/* 关闭对讲连接 */
	VOID   Stop();
	
		/* 发送对讲信息 */
	BOOL SendDTalkMsg(const NET_DTALK_MSG_S *pParam, const NET_DTALK_USER_S *pUserList);

		/* 发送对讲数据 */
	BOOL SendDTalkData(U8 *pu8Data, U32 u32DataSize);

		/* 开始捕获 */
	BOOL StartCapture();
    
    VOID SetDTalkParam(
							 U32                 u32CollectFlag,
							 HWND                hLWnd,
							 HWND                hRWnd,
							 HWND                hMWnd,
							 pfnDTalkMsgCallback fDTalkMsgCallback, 
							 pfnRealDataCallback fDTalkDataCallback, 
							 VOID *pUser) ;
    
protected: 
		/* 连接操作 */
	BOOL OpenStream();
    
    S32  StartDualtalkLoop();

	    /* 对讲消息循环 */
    VOID DualtalkLoop();
    


	static VOID *ThreadEntry(VOID *pArg);

private:
	HikPlatformConn        * m_pConn;                     /**< 设备主连接类指针 */
	BOOL volatile       m_bExit;                     /**< 是否退出 */
	BOOL volatile       m_bOpened;                   /**< 是否已经打开 */
	BOOL volatile       m_bTalking;                  /**< 是否正在对讲 */

	THREAD              m_trdTalk;                   /**< 对讲通道线程ID */

	//SyncSocket          m_DTalkSock;                 /**< 对讲链路 */

    HWND                m_hLocalWnd;                 /**< 视频会话本地图像显示窗口 */
	HWND                m_hRemoteWnd;                /**< 视频会话对方图像显示窗口 */
	HWND                m_hMsgWnd;                   /**< 视频采集消息通知窗口 */
	pfnDTalkMsgCallback m_fMsgCallback;              /**< 对讲消息回调 */
	pfnRealDataCallback m_fDTalkDataCallback;        /**< 对讲数据回调 */
	VOID              * m_pMsgUser;                  /**< 回调用户数据 */

	U32                 m_u32CollectFlag;            /**< 内部采集标记 bit0-采集音频 bit1-采集视频 */
};

#endif

