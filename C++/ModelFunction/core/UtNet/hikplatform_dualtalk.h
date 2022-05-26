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
	
		/* �����Խ��ŵ� */
	S32    Start();

		/* �رնԽ����� */
	VOID   Stop();
	
		/* ���ͶԽ���Ϣ */
	BOOL SendDTalkMsg(const NET_DTALK_MSG_S *pParam, const NET_DTALK_USER_S *pUserList);

		/* ���ͶԽ����� */
	BOOL SendDTalkData(U8 *pu8Data, U32 u32DataSize);

		/* ��ʼ���� */
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
		/* ���Ӳ��� */
	BOOL OpenStream();
    
    S32  StartDualtalkLoop();

	    /* �Խ���Ϣѭ�� */
    VOID DualtalkLoop();
    


	static VOID *ThreadEntry(VOID *pArg);

private:
	HikPlatformConn        * m_pConn;                     /**< �豸��������ָ�� */
	BOOL volatile       m_bExit;                     /**< �Ƿ��˳� */
	BOOL volatile       m_bOpened;                   /**< �Ƿ��Ѿ��� */
	BOOL volatile       m_bTalking;                  /**< �Ƿ����ڶԽ� */

	THREAD              m_trdTalk;                   /**< �Խ�ͨ���߳�ID */

	//SyncSocket          m_DTalkSock;                 /**< �Խ���· */

    HWND                m_hLocalWnd;                 /**< ��Ƶ�Ự����ͼ����ʾ���� */
	HWND                m_hRemoteWnd;                /**< ��Ƶ�Ự�Է�ͼ����ʾ���� */
	HWND                m_hMsgWnd;                   /**< ��Ƶ�ɼ���Ϣ֪ͨ���� */
	pfnDTalkMsgCallback m_fMsgCallback;              /**< �Խ���Ϣ�ص� */
	pfnRealDataCallback m_fDTalkDataCallback;        /**< �Խ����ݻص� */
	VOID              * m_pMsgUser;                  /**< �ص��û����� */

	U32                 m_u32CollectFlag;            /**< �ڲ��ɼ���� bit0-�ɼ���Ƶ bit1-�ɼ���Ƶ */
};

#endif

