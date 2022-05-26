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

		/* �����Խ��ŵ� */
		S32    Start();

		/* �رնԽ����� */
		VOID   Stop();

		/* ���öԽ����� */
		VOID   SetDTalkParam(
			U32                 u32CollectFlag,
			HWND                hLWnd,
			HWND                hRWnd,
			HWND                hMWnd,
			pfnDTalkMsgCallback fDTalkMsgCallback, 
			pfnRealDataCallback fDTalkDataCallback, 
			VOID               *pUser);

		/* ���ͶԽ���Ϣ */
		BOOL SendDTalkMsg(const NET_DTALK_MSG_S *pParam, const NET_DTALK_USER_S *pUserList);

		BOOL SendDTalkData(U8 *pu8Data, U32 u32DataSize);

		BOOL StartCapture();

		/* ��Ϣ������� */
		BOOL Transaction(const MsgHeadWrap &reqHead, const VOID *reqData, MsgBuf &rspMsg, DWORD milliseconds = 5000)
		{
			MsgTransaction trans;
			trans.Init(reqHead, rspMsg);
			return Transaction(trans, reqData, milliseconds);
		}
		
		/* ��Ϣ������� */
		BOOL Transaction(MsgTransaction &msgTrans, const VOID *reqData = NULL, DWORD milliseconds = 5000)
  		{
			m_MsgTransQueue.Req(msgTrans);

			BOOL hasRsp = FALSE;
			if (SendReq(*msgTrans.m_pReq, (VOID *)reqData))
				hasRsp = msgTrans.WaitRsp(milliseconds);

			m_MsgTransQueue.Clear(msgTrans);
			return hasRsp;
		}
		/*�ڲ����� */
	protected:
		/* ���Ӳ��� */
		BOOL OpenStream();

		VOID StopMedia();

		S32  StartDualtalkLoop();

		/* �Խ���Ϣѭ�� */
		VOID DualtalkLoop();

		VOID BeginTransaction();

		VOID EndTransaction();

		BOOL SendReq(const MsgHeadWrap &head, VOID *reqData = NULL);

		static VOID *ThreadEntry(VOID *pArg);

	public:
	private:
		Connection        * m_pConn;                     /**< �豸��������ָ�� */
		BOOL volatile       m_bExit;                     /**< �Ƿ��˳� */
		BOOL volatile       m_bOpened;                   /**< �Ƿ��Ѿ��� */
		BOOL volatile       m_bTalking;                  /**< �Ƿ����ڶԽ� */

		THREAD              m_trdTalk;                   /**< �Խ�ͨ���߳�ID */

		SyncSocket          m_DTalkSock;                 /**< �Խ���· */

		MsgTransQueue       m_MsgTransQueue;             /**< ��Ϣ���� */
		U32                 m_u32TalkMode;               /**< �Խ�ģʽ, TALK_MODE_ACTIVE-���� TALK_MODE_PASSIVE-���� */
		U32                 m_u32TalkType;               /**< ��ǰ�Խ����� 0-��Ƶ�Խ� 1-��Ƶ�Խ� 2-����Ƶ�Խ� 3-��Ƶ���� 4-��Ƶ���� 5-����Ƶ���� */

		S32                 m_iPlayerAudio;
		S32                 m_iPlayerVideo;
		S32                 m_iLocalCamera;
		U8                  m_u8Buffer[512*1014];

		HWND                m_hLocalWnd;                 /**< ��Ƶ�Ự����ͼ����ʾ���� */
		HWND                m_hRemoteWnd;                /**< ��Ƶ�Ự�Է�ͼ����ʾ���� */
		HWND                m_hMsgWnd;                   /**< ��Ƶ�ɼ���Ϣ֪ͨ���� */
		pfnDTalkMsgCallback m_fMsgCallback;              /**< �Խ���Ϣ�ص� */
		pfnRealDataCallback m_fDTalkDataCallback;        /**< �Խ����ݻص� */
		VOID              * m_pMsgUser;                  /**< �ص��û����� */

		U32                 m_u32CollectFlag;            /**< �ڲ��ɼ���� bit0-�ɼ���Ƶ bit1-�ɼ���Ƶ */
};

#endif

