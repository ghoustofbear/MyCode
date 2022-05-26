// pc_talk.h: interface for the PcTalk class.
// 定义PC对讲协议类
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

	/* 开启PC对讲服务 */
	BOOL IniPcTalk(U8 u8Type, NET_PCTALK_PARAM_S * pParam);
	/* 停止当前对讲 */
	VOID StopTalk(S32 s32SockFd);
	/* 发送PC对讲消息 */
	VOID SendPcTalkMsg(U8 u8Type, U8 *pData, U32 u32Size);
	/* 音频数据打包 */
	void SetNetParam(string szLocalIp, U16 u16LocalPort, string szRemoteIp, U16 u16RemotePort);
public: 
    /* 音频数据打包 */
	void SendAudioData(U8 *pData, U32 u32DataLength);
	/* 发送请求 0:对讲 1:喊话 */
	void SendRequest(U8 *pData, U32 u32DataLength); 
	/* 发送结束 */
	void SendStop(U8 *pData, U32 u32DataLength);
	/* 发送ack */
	void SendAck(U8 *pData, U32 u32DataLength);
	/* 发送nak */
	void SendNak(U8 *pData, U32 u32DataLength);

	VOID ProcessPcTalk(MsgBuf &buf, S32 s32SockFd);

	VOID ProcessOffLine(S32 s32SockFd);


	list<TalkClient> m_listTalkCliet;
	void EditClientList( S32 s32SockFd, S8 *pS8ClientIp, BOOL bAdd=TRUE);
	void ClearClientList();

private:
	U8               m_u8TalkType;     /**< 对讲类型，1-PC对讲(P2P)，2-喊话(广播) */
	UtNetClient *    m_pClient;          /**< 客户端连接类指针 */
	UtNetServer *    m_pServer;        /**< 服务器指针 */

	string           m_szName;         /**< 本地名称 */
	string           m_szRemoteIP;     /**< 远方IP */
	U16              m_u16RemotePort;  /**< 远方端口 */
	string           m_szLocalIp;      /**< 本地IP */
	U16              m_u16LocalPort;   /**< 本地端口 */

	VOID *           m_pTalkUser;      /**< 用户数据 */
	pfnPcTalkCallback m_fPcTalkCallback; /** < PC对讲消息回调函数入口*/
	S32 m_s32SockFd;
};

#endif
