/**
 * (C) Copyright 2012, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Thu Aug  9 10:46:20 CST 2012
 *
 * DualtalkEx(PC向设备层对讲) defines.
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

		/* 启动对讲信道 */
		S32    Start();

		/* 关闭对讲链接 */
		VOID   Stop();

		/* 发送对讲申请 */
		BOOL SendDTalkReq(const NET_DTALK_DEV_LIST * pDevList, U32 u32Type);

		BOOL SendDTalkData(U8 *pu8Data, U32 u32DataSize);

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
        
        /* 初始化封装文件参数*/
        BOOL InitMuxAVF(_NET_DEV_MUXAVF_PARAM_ *pParam);

        /*写入远程数据到封装文件*/
        void WriteMuxData(void *data,int len);

        /*获取当前对讲通道，返回-1表示没有开启对讲*/
        int GetTalkChannel();
        
		/*内部调用 */
	protected:
		/* 开启本地音频采集 */
		BOOL OpenSound();

		/* 关闭本地音频采集 */
		VOID StopSound();

		/* 连接操作 */
		BOOL OpenStream();

		/* 自动重连下发请求 */
        VOID AutoSendReq();

		/* 启动信道通信线程 */
		S32  StartDualtalkLoop();

		/* 对讲主消息循环 */
		VOID DualtalkLoop();

		BOOL SendReq(const MsgHeadWrap &head, VOID *reqData = NULL);

		static VOID *ThreadEntry(VOID *pArg);
        
        /*写入本地数据到封装文件*/
        void WriteLocalMuxData(void *data,int len);

        /*创建封装文件*/
        BOOL CreateMux();

        /*关闭封装文件*/
        void CloseMux();

	private:
		Connection        * m_pConn;                     /**< 设备主连接类指针 */
		BOOL volatile       m_bExit;                     /**< 是否退出 */
		BOOL volatile       m_bOpened;                   /**< 是否已经打开 */
		int                 m_iAudioType;                /**< 语音编码格式 */

		THREAD              m_trdTalk;                   /**< 对讲通道线程ID */
		SyncSocket          m_DTalkSock;                 /**< 对讲链路 */
		MsgTransQueue       m_MsgTransQueue;             /**< 消息队列 */
		S32                 m_iPlayerAudio;              /**< 音频句柄 */
		map<string, DTALK_DEV_S>  m_mapDTalkDev;         /**< 对讲设备列表 */

		AudioRecord       * m_audioRecord;

		AudioHelmetGx     * m_audioHelmetGx;

        HANDLE              m_hMuxAVF;
        bool                m_bTalked;                    /**< 对讲中 */
        
        bool                m_bMuxInited;                 /**< 封装参数已初始化*/
        _NET_DEV_MUXAVF_PARAM_ m_muxInitParam;
        int                 m_curChannel;
};

#endif

