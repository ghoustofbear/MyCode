// pc_talk.h: interface for the PcTalk class.
// ����PC�Խ�Э����
// 
//////////////////////////////////////////////////////////////////////

#ifndef __PC_TALK_H__
#define __PC_TALK_H__

#include "commonc1.h"
#include "net_msg.h"
#include "ut_net.h"
#include "ut_netserver.h"

class TalkClient
{
public:
	TalkClient();
	virtual~TalkClient();

	S8    * m_pS8TalkClietIp;
	S32     m_s32TalkSockFd;
	void Init();
	
	VOID * ThreadFun(VOID *pArg);
};
typedef TalkClient* TalkClientPtr;

class PcTalk  
{
public:
	PcTalk();
	virtual ~PcTalk();

	/* ����PC�Խ����� */
	BOOL IniPcTalk(U8 u8Type, NET_PCTALK_PARAM_S * pParam);
	/* ֹͣ��ǰ�Խ� */
	VOID StopTalk(S32 s32SockFd);
	/* ����PC�Խ���Ϣ */
	VOID SendPcTalkMsg(U8 u8Type, U8 *pData, U32 u32Size);
	/* ��Ƶ���ݴ�� */
	void SetNetParam(string szLocalIp, U16 u16LocalPort, string szRemoteIp, U16 u16RemotePort);
public: 
    /* ��Ƶ���ݴ�� */
	void SendAudioData(U8 *pData, U32 u32DataLength);
	/* �������� 0:�Խ� 1:���� */
	void SendRequest(U8 *pData, U32 u32DataLength); 
	/* ���ͽ��� */
	void SendStop(U8 *pData, U32 u32DataLength);
	/* ����ack */
	void SendAck(U8 *pData, U32 u32DataLength);
	/* ����nak */
	void SendNak(U8 *pData, U32 u32DataLength);

	VOID ProcessPcTalk(MsgBuf &buf, S32 s32SockFd);

	VOID ProcessOffLine(S32 s32SockFd);


	list<TalkClient> m_listTalkCliet;
	void EditClientList( S32 s32SockFd, S8 *pS8ClientIp, BOOL bAdd=TRUE);
	void ClearClientList();

private:
	U8               m_u8TalkType;     /**< �Խ����ͣ�1-PC�Խ�(P2P)��2-����(�㲥) */
	UtNetClient *    m_pClient;          /**< �ͻ���������ָ�� */
	UtNetServer *    m_pServer;        /**< ������ָ�� */

	string           m_szName;         /**< �������� */
	string           m_szRemoteIP;     /**< Զ��IP */
	U16              m_u16RemotePort;  /**< Զ���˿� */
	string           m_szLocalIp;      /**< ����IP */
	U16              m_u16LocalPort;   /**< ���ض˿� */

	VOID *           m_pTalkUser;      /**< �û����� */
	pfnPcTalkCallback m_fPcTalkCallback; /** < PC�Խ���Ϣ�ص��������*/
	S32 m_s32SockFd;
};

#endif
