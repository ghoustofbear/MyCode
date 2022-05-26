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
		/**< ��ʼ�� */
		void  Init();

		/**< ��ȡ��Ϣͷָ�� */
		void* GetHead();

		const VOID *GetHead() const;

		/**< ��ȡ������Ϣָ�� */
		void* GetExt();

		/**< ��ȡ��Ϣ��ָ�� */
		void* GetData();

		/**< ��ȡ��Ϣ�峤�� */
		int   GetBufLen() const;

		/**< �������� */
		void  operator=(const MsgBuf &other);

	protected:
		S8  s8Data[MAX_MSG_BUF_LEN];   /**< ��Ϣ����/���ͻ��� */
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
		UINT   mFrameTotalSize;              /* ����֡ͷ,ע��ȥ���հ� */
		BOOL   mNeedKeyFrame;

#ifdef EnableFrameData
		ULONGLONG lastpts;
#endif
};

struct MsgTransaction
{
	public:
		/**< ��ʼ�� */
		VOID  Init(const MsgHeadWrap &req, MsgBuf &rsp);
		VOID  Init(const MsgHeadWrap &req, MsgBuf &rsp, Event &e);

		/**< �ȴ���Ϣ��Ӧ */
		BOOL  WaitRsp(U32 milliseconds);

		/**< ֪ͨ��Ӧ��Ϣ */
		BOOL  TryRsp(MsgBuf &buf);

		/**< ȡ���ȴ���Ϣ��Ӧ */
		VOID  CancelWaitRsp();

	public:
		BOOL volatile      m_bHasRsp;   /**< �Ƿ��Ѿ���Ӧ */
		const MsgHeadWrap *m_pReq;      /**< ������Ϣ */
		Event              m_Event;     /**< ͬ������ */
		MsgBuf            *m_pRsp;      /**< ��Ӧ��Ϣ */

};

class MsgTransQueue
{
	public:
		/**< ��������Ϣ���봫����� */
		VOID  Req(MsgTransaction &trans);

		/**< ��������ƥ���Ӧ��Ϣ */
		VOID  Rsp(MsgBuf &buf);

		/**< ��մ������ */
		VOID  Clear(MsgTransaction &trans);
		VOID  Clear();

	protected:
		Mutex m_Mutex;                             /**< ������б����� */
		std::vector<MsgTransaction*> m_vcMsgQueue; /**< ������� */
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
