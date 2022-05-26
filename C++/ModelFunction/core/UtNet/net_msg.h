/**
 * (C) Copyright 2012, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Thu Aug  9 10:46:20 CST 2012
 *
 * MSG_HEADER_S defines.
 */
#ifndef __NET_MSG_H__
#define __NET_MSG_H__

#include "proto_def.h"
#include "commonc1.h"
#include "socketc1.h"
#include "framec1.h"
#include "concourrencyc1.h"
#define MAX_MSG_BUF_LEN    (400*1024)

class MsgHeadWrap : public MSG_HEADER_S
{
	public:
		MsgHeadWrap();
		MsgHeadWrap(WORD msgType, WORD msgSubtype = MSG_NO_SUBTYPE, UINT msgSize = 0);
		MsgHeadWrap(WORD msgType, WORD msgSubtype, UINT msgSize, UINT& seq);

		void SetMsgHead(WORD msgType, WORD msgSubtype = MSG_NO_SUBTYPE, UINT msgSize = 0);
		void InitExt();
		int  GetHeadLen() const;
		int  GetExtLen() const;
		int  GetHeadExtLen() const;
		int  GetHeadExtLenEx() const;
		int  GetDataLen() const;
		int  GetMsgLen() const;
		BOOL ValidRsp(const MsgHeadWrap &reqHead) const;
		WORD GetMsgType() const;
		WORD GetMsgSubType() const;
		BOOL IsSuccess() const;
		WORD GetAckFlag() const;
		UINT GetSeq() const;
};


class MsgBuf : public MsgHeadWrap
{
	public:
		/**< 初始化 */
		void  Init();

		/**< 获取消息头指针 */
		void* GetHead();

		const VOID *GetHead() const;

		/**< 获取额外消息指针 */
		void* GetExt();

		/**< 获取消息体指针 */
		void* GetData();

		/**< 获取消息体长度 */
		int   GetBufLen() const;

		/**< 拷贝构造 */
		void  operator=(const MsgBuf &other);

	protected:
		S8  s8Data[MAX_MSG_BUF_LEN];   /**< 消息接收/发送缓冲 */
};

struct PackBuf : PACK_HEADER_S
{
	char ExtHead[256];
	char Data[MaxPackSize];

	BOOL   IsFrame()   { return u8Type == FramePack; }
	BOOL   IsRepos()   { return u8Type == ReposPack; }
	BOOL   IsNoData()  { return u8Type == NoDataPack; }
	BOOL   IsFileEnd() { return u8Type == FileEndPack; }
	VOID * GetData()   { return ExtHead + (u6ExtSize << 2); }


	BOOL RecvPack(SyncSocket &sock, BOOL tcp)
	{
		if (tcp)
			return RecvPackTcp(sock);
		else
			return RecvPackUdp(sock);
	}

	BOOL RecvPackTcp(SyncSocket& sock)
	{
		return sock.Recv(this, sizeof(PACK_HEADER_S)) 
			&& sock.Recv(ExtHead, u6ExtSize << 2) 
			&& u16PackSize <= MaxPackSize 
			&& sock.Recv(ExtHead + (u6ExtSize<<2), u16PackSize) 
			&& ValidPack();
	}

	BOOL RecvPackUdp(SyncSocket& sock)
	{
		int len = sizeof(*this);
		return sock.RecvFrom(this, len) 
			&& (UINT)len == sizeof(PACK_HEADER_S) + (u6ExtSize<<2) + u16PackSize 
			&& ValidPack();
	}

	BOOL ValidPack()
	{
		if (!IsFrame())
			return TRUE;

		//FrameData("u16PackNum=%u,u32FrameNo=%u,u16PackNo=%u,u16PackSize=%u\n",	
		//u16PackNum,u32FrameNo,u16PackNo,u16PackSize);

		if (u16PackNo < u16PackNum && u16PackSize <= MaxPackSize)
			return TRUE;

		//ASSERT(FALSE && "Invalid pack");
		return FALSE;
	}
};

class FrameBuf
{
	public:
		FrameBuf();
		~FrameBuf();
		BOOL   FrameComplete(PackBuf &pack);
		VOID   ClearUnCompleteFrame();
		VOID * GetFrame();
		UINT   GetFrameTotalSize();

	protected:
		VOID ReleaseBuf();

	protected:
		void * mBuf;
		int    mBufLen;

		BOOL   mNoFrame;
		UINT   mFrameNo;
		WORD   mFramePackNum;
		WORD   mFrameUnrecvPackNum;
		UINT   mFrameTotalSize;              /* 包括帧头,注意去掉空白 */
		BOOL   mNeedKeyFrame;

#ifdef EnableFrameData
		ULONGLONG lastpts;
#endif
};

struct MsgTransaction
{
	public:
		/**< 初始化 */
		VOID  Init(const MsgHeadWrap &req, MsgBuf &rsp);
		VOID  Init(const MsgHeadWrap &req, MsgBuf &rsp, Event &e);

		/**< 等待消息回应 */
		BOOL  WaitRsp(U32 milliseconds);

		/**< 通知回应消息 */
		BOOL  TryRsp(MsgBuf &buf);

		/**< 取消等待消息回应 */
		VOID  CancelWaitRsp();

	public:
		BOOL volatile      m_bHasRsp;   /**< 是否已经回应 */
		const MsgHeadWrap *m_pReq;      /**< 请求消息 */
		Event              m_Event;     /**< 同步事情 */
		MsgBuf            *m_pRsp;      /**< 回应消息 */

};

class MsgTransQueue
{
	public:
		/**< 将请求消息加入传输队列 */
		VOID  Req(MsgTransaction &trans);

		/**< 遍历队列匹配回应消息 */
		VOID  Rsp(MsgBuf &buf);

		/**< 清空传输队列 */
		VOID  Clear(MsgTransaction &trans);
		VOID  Clear();

	protected:
		Mutex m_Mutex;                             /**< 传输队列保护锁 */
		std::vector<MsgTransaction*> m_vcMsgQueue; /**< 传输队列 */
};

template <typename T>
class ListDataParser
{
public:
	ListDataParser(MsgBuf& buf)
	{
		Data = (BYTE*)buf.GetData();
		DataSize = buf.GetDataLen();
	}
	
	ListDataParser(VOID* data, UINT dataSize)
	{
		Data = (BYTE*)data;
		DataSize = dataSize;
	}
	
	UINT GetListNum()
	{
		if (DataSize == 0)
			return 0;
		
		T* item = (T*)Data;
		ItemSize = item->u32StSize;
		ASSERT(ItemSize != 0);
	//	ASSERT(DataSize % ItemSize == 0);
		return DataSize / ItemSize;
	}
	
	T* GetAt(UINT index)
	{
		T* itemData = (T*)(Data + index * ItemSize);
		itemData->u32StSize = sizeof(T);
		if (sizeof(T) <= ItemSize)
			return itemData;
		
		memset(&Item, 0, sizeof(T));
		memcpy(&Item, itemData, ItemSize);
		return &Item;
	}
	
protected:
	T Item;
	BYTE* Data;
	UINT  DataSize;
	UINT  ItemSize;
};

BOOL SendMsg(SyncSocket& sock, const MsgHeadWrap& head, const VOID *data = NULL);
BOOL RecvMsg(SyncSocket& sock, MsgBuf& buf);
#endif
