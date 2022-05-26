/**
 * (C) Copyright 2012, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Thu Aug  9 10:46:20 CST 2012
 *
 * DualtalkEx(PC向设备层对讲) defines.
 */
#ifndef __HIKPLATFORM_DUALTALK_EX_H__
#define __HIKPLATFORM_DUALTALK_EX_H__

#include "VideoSDK.h"
#include "ut_net.h"
#include "basedualtalkEx.h"
#include "concourrencyc1.h"

class HikPlatformConn;
class HikPlatformDualtalkEx : public BaseDualTalkEx
{
	public:
		HikPlatformDualtalkEx(HikPlatformConn *pConn, int audioType);
		virtual ~HikPlatformDualtalkEx();

		/* 启动对讲信道 */
		S32    Start();

		/* 关闭对讲链接 */
		VOID   Stop();

		/* 发送对讲申请 */
		BOOL SendDTalkReq(const NET_DTALK_DEV_LIST * pDevList, U32 u32Type);


		BOOL OpenStream();


		/* 启动信道通信线程 */
		S32  StartDualtalkLoop();

		/* 对讲主消息循环 */
		VOID DualtalkLoop();

        VOID RecvStream();

        static VOID *TalkExFun(VOID *pArg);

        VOID setBTalkEx(BOOL b){m_bTalkEx = b; }

	private:
		HikPlatformConn        * m_pConn;                /**< 设备主连接类指针 */
		NET_DTALK_DEV_INFO      m_talkEx;	
		Mutex                   m_mtxTalkEx;
		THREAD                  m_trdTalkEx;            /**< 对讲通道线程ID */
    	BOOL                    m_bTalkEx;               /**< 对讲中*/
	    int                     m_iAudioType;           
		BOOL volatile           m_bExit;                     /**< 是否退出 */
		BOOL volatile           m_bOpened;                   /**< 是否已经打开 */

    	S8                      m_s8DeviceIp[24];
	    string                  m_strUrl;
        int                     m_retTalk;
};

#endif

