/**
 * (C) Copyright 2012, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Thu Aug  9 10:46:20 CST 2012
 *
 * DualtalkEx(PC���豸��Խ�) defines.
 */
#ifndef _DUALTALK_EX_H__
#define _DUALTALK_EX_H__

#include "commonc1.h"
#include "net_msg.h"
#include "ut_net.h"
#include "basedualtalkEx.h"
#include "Mux_AVFile.h"

#define CONN_TIMEOUT	  5000
#define SEND_TIMEOUT	  5000
#define RECV_TIMEOUT	  5000

class Connection;
class AudioRecord;
class AudioHelmetGx;

class DualtalkEx : public BaseDualTalkEx
{
	public:
		DualtalkEx(Connection *pConn, int audioType);
		virtual ~DualtalkEx();

		friend int CALLBACK  AudioDataCallback(int size, const char* data, DWORD dwUser);
		friend void AudioDataCallbackEx(int type, void* data, int size, void* userData);
		friend void AudioDataCallbackExEx(void* data, int size, void* userData);

		/* �����Խ��ŵ� */
		S32    Start();

		/* �رնԽ����� */
		VOID   Stop();

		/* ���ͶԽ����� */
		BOOL SendDTalkReq(const NET_DTALK_DEV_LIST * pDevList, U32 u32Type);

		BOOL SendDTalkData(U8 *pu8Data, U32 u32DataSize);

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
        
        /* ��ʼ����װ�ļ�����*/
        BOOL InitMuxAVF(_NET_DEV_MUXAVF_PARAM_ *pParam);

        /*д��Զ�����ݵ���װ�ļ�*/
        void WriteMuxData(void *data,int len);

        /*��ȡ��ǰ�Խ�ͨ��������-1��ʾû�п����Խ�*/
        int GetTalkChannel();
        
		/*�ڲ����� */
	protected:
		/* ����������Ƶ�ɼ� */
		BOOL OpenSound();

		/* �رձ�����Ƶ�ɼ� */
		VOID StopSound();

		/* ���Ӳ��� */
		BOOL OpenStream();

		/* �Զ������·����� */
        VOID AutoSendReq();

		/* �����ŵ�ͨ���߳� */
		S32  StartDualtalkLoop();

		/* �Խ�����Ϣѭ�� */
		VOID DualtalkLoop();

		BOOL SendReq(const MsgHeadWrap &head, VOID *reqData = NULL);

		static VOID *ThreadEntry(VOID *pArg);
        
        /*д�뱾�����ݵ���װ�ļ�*/
        void WriteLocalMuxData(void *data,int len);

        /*������װ�ļ�*/
        BOOL CreateMux();

        /*�رշ�װ�ļ�*/
        void CloseMux();

	private:
		Connection        * m_pConn;                     /**< �豸��������ָ�� */
		BOOL volatile       m_bExit;                     /**< �Ƿ��˳� */
		BOOL volatile       m_bOpened;                   /**< �Ƿ��Ѿ��� */
		int                 m_iAudioType;                /**< ���������ʽ */

		THREAD              m_trdTalk;                   /**< �Խ�ͨ���߳�ID */
		SyncSocket          m_DTalkSock;                 /**< �Խ���· */
		MsgTransQueue       m_MsgTransQueue;             /**< ��Ϣ���� */
		S32                 m_iPlayerAudio;              /**< ��Ƶ��� */
		map<string, DTALK_DEV_S>  m_mapDTalkDev;         /**< �Խ��豸�б� */

		AudioRecord       * m_audioRecord;

		AudioHelmetGx     * m_audioHelmetGx;

        HANDLE              m_hMuxAVF;
        bool                m_bTalked;                    /**< �Խ��� */
        
        bool                m_bMuxInited;                 /**< ��װ�����ѳ�ʼ��*/
        _NET_DEV_MUXAVF_PARAM_ m_muxInitParam;
        int                 m_curChannel;
};

#endif

