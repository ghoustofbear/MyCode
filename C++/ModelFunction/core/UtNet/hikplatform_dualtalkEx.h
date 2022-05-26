/**
 * (C) Copyright 2012, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Thu Aug  9 10:46:20 CST 2012
 *
 * DualtalkEx(PC���豸��Խ�) defines.
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

		/* �����Խ��ŵ� */
		S32    Start();

		/* �رնԽ����� */
		VOID   Stop();

		/* ���ͶԽ����� */
		BOOL SendDTalkReq(const NET_DTALK_DEV_LIST * pDevList, U32 u32Type);


		BOOL OpenStream();


		/* �����ŵ�ͨ���߳� */
		S32  StartDualtalkLoop();

		/* �Խ�����Ϣѭ�� */
		VOID DualtalkLoop();

        VOID RecvStream();

        static VOID *TalkExFun(VOID *pArg);

        VOID setBTalkEx(BOOL b){m_bTalkEx = b; }

	private:
		HikPlatformConn        * m_pConn;                /**< �豸��������ָ�� */
		NET_DTALK_DEV_INFO      m_talkEx;	
		Mutex                   m_mtxTalkEx;
		THREAD                  m_trdTalkEx;            /**< �Խ�ͨ���߳�ID */
    	BOOL                    m_bTalkEx;               /**< �Խ���*/
	    int                     m_iAudioType;           
		BOOL volatile           m_bExit;                     /**< �Ƿ��˳� */
		BOOL volatile           m_bOpened;                   /**< �Ƿ��Ѿ��� */

    	S8                      m_s8DeviceIp[24];
	    string                  m_strUrl;
        int                     m_retTalk;
};

#endif

