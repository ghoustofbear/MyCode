#include "net_msg.h"

#ifndef _WIN32
extern VOID SetLastError(UINT err);
#endif

UINT volatile g_Seq = 0;
MsgHeadWrap::MsgHeadWrap() {}

MsgHeadWrap::MsgHeadWrap(WORD msgType, WORD msgSubtype, UINT msgSize)
{
	SetMsgHead(msgType, msgSubtype, msgSize);
}

MsgHeadWrap::MsgHeadWrap(WORD msgType, WORD msgSubtype, UINT msgSize, UINT& seq)
{
	memset(this, 0, sizeof(*this));
	u16MsgType = msgType;
	u16MsgSubType = msgSubtype;
	u32MsgSize = msgSize;
	u32Seq = seq++;
}

void MsgHeadWrap::SetMsgHead(WORD msgType, WORD msgSubtype, UINT msgSize)
{
	memset(this, 0, sizeof(*this));
	u16MsgType = msgType;
	u16MsgSubType = msgSubtype;
	u32MsgSize = msgSize;
	u32Seq = g_Seq++;
	//Seq = (UINT)InterlockedIncrement((long volatile*)&gSeq);
}

void MsgHeadWrap::InitExt()
{

}

int MsgHeadWrap::GetHeadLen() const
{
	return sizeof(MSG_HEADER_S);
}

int MsgHeadWrap::GetExtLen() const
{
	return u6ExtSize << 2;
}

int MsgHeadWrap::GetHeadExtLen() const
{
	return GetHeadLen() + GetExtLen();
}

int MsgHeadWrap::GetHeadExtLenEx() const
{
	return MAX(GetHeadExtLen(), (int)sizeof(*this));
}

int MsgHeadWrap::GetDataLen() const
{
	return MIN(u32MsgSize, MAX_MSG_BUF_LEN);
}

int MsgHeadWrap::GetMsgLen() const
{
	return GetHeadExtLenEx() + GetDataLen();
}

BOOL MsgHeadWrap::ValidRsp(const MsgHeadWrap& reqHead) const
{
	return u16MsgType == reqHead.u16MsgType 
		&& u16MsgSubType == reqHead.u16MsgSubType 
		&& u32Seq == reqHead.u32Seq;
}

WORD MsgHeadWrap::GetMsgType() const
{
	return u16MsgType;
}

WORD MsgHeadWrap::GetMsgSubType() const
{
	return u16MsgSubType;
}

BOOL MsgHeadWrap::IsSuccess() const
{
	if (u16AckFlag == ACK_SUCCESS)
		return TRUE;

	switch (u16AckFlag)
	{
		case ACK_LOGIN_AUTH_FAIL:
			SetLastError(ACK_LOGIN_AUTH_FAIL);
			break;

		case ACK_LOGIN_PERMDENY:
			SetLastError(ACK_LOGIN_PERMDENY);
			break;

		case ACK_OPT_PERMDENY:
			SetLastError(ACK_OPT_PERMDENY);
			break;

		case ACK_OPT_DOING:
			SetLastError(ACK_OPT_DOING);
			break;
	}

	return FALSE;
}

WORD MsgHeadWrap::GetAckFlag() const
{
	return u16AckFlag;
}

UINT MsgHeadWrap::GetSeq() const
{
	return u32Seq;
}


void MsgBuf::Init()
{
	memset(this, 0, sizeof(*this));
}

void* MsgBuf::GetHead()
{
	return this;
}

const VOID *MsgBuf::GetHead() const
{
	return this;
}

void* MsgBuf::GetExt()
{
	return (BYTE*)GetHead() + GetHeadLen();
}

void* MsgBuf::GetData()
{
	return (BYTE*)GetHead() + GetHeadExtLenEx();
}

int MsgBuf::GetBufLen() const
{
	return sizeof(*this);
}

void MsgBuf::operator=(const MsgBuf& other)
{
	memcpy(GetHead(), other.GetHead(), other.GetMsgLen());
}


FrameBuf::FrameBuf()
{
	mBuf = NULL;
	mBufLen = 0;

	mNoFrame = TRUE;
	mFrameNo = 0;
	mFramePackNum = 0;
	mFrameUnrecvPackNum = 0;
	mFrameTotalSize = 0;
	mNeedKeyFrame = FALSE;

#ifdef EnableFrameData
	lastpts = 0;
#endif
}

FrameBuf::~FrameBuf()
{
	ReleaseBuf();
}

BOOL FrameBuf::FrameComplete(PackBuf& pack)
{
	if (!pack.IsFrame())
	{
		mNoFrame = TRUE;
		//mNeedKeyFrame = TRUE;
		return FALSE;
	}
	UINT maxPackSize = MaxPackSize;
	if (pack.u2Version == PackHeadV1)
		maxPackSize = MaxPackSizeV1;
	if (mNoFrame || pack.u32FrameNo != mFrameNo)
	{
#ifdef EnableFrameData
		if (pack.u32FrameNo != mFrameNo+1)
			FrameData("last frame no=%u, cur frame no=%u\n", mFrameNo, pack.u32FrameNo);
		if (pack.u32FrameNo != mFrameNo && !mNoFrame)
			FrameData("mFrameNo=%u, mFramePackNum=%u, mFrameUnrecvPackNum=%u\n", 
					mFrameNo, mFramePackNum, mFrameUnrecvPackNum);
#endif

		if (mFrameUnrecvPackNum != 0 && mBuf != NULL)
		{
			UT_FRAME_HEAD_S* frame = (UT_FRAME_HEAD_S*)mBuf;
			if (frame->FrameType == FRAME_I)
				mNeedKeyFrame = TRUE;
		}

		int newBufLen = (pack.u16PackNum * maxPackSize + 65535) & ~65535;
		if (newBufLen > mBufLen)
		{
			ReleaseBuf();
#ifdef _WIN32
			mBuf = VirtualAlloc(NULL, newBufLen, MEM_COMMIT, PAGE_READWRITE);
#else
			mBuf = malloc(newBufLen);
#endif
			mBufLen = newBufLen;
		}

		mNoFrame = FALSE;
		mFrameNo = pack.u32FrameNo;
		mFramePackNum = pack.u16PackNum;
		mFrameUnrecvPackNum = pack.u16PackNum;
		mFrameTotalSize = 0;

		memset(mBuf, 0, sizeof(mBuf));
	}

	memcpy((BYTE*)mBuf + pack.u16PackNo * maxPackSize, pack.GetData(), pack.u16PackSize);
	mFrameTotalSize += pack.u16PackSize;

	if (--mFrameUnrecvPackNum == 0)
	{
		mNoFrame = TRUE;
		UT_FRAME_HEAD_S* frame = (UT_FRAME_HEAD_S*)GetFrame();
		mFrameTotalSize = frame->FrameSize + 40;

#if defined(EnableFrameData) && 0
		//if (frame->FrameType != FRAME_A)
		{
			ULONGLONG curPts = frame->Pts/1000;
			ULONGLONG difPts = curPts - lastpts;
			//if (difPts > 0)
			FrameData("no=%u, size=%u, type=%c, time=%I64u, lastpts=%I64u, difpts=%I64u, pts=%I64u\n", 
					frame->u32FrameNo, frame->FrameSize, frame->FrameType, frame->Time, lastpts, difPts, curPts);
			lastpts = curPts;
		}


		static UINT vfnum = 0;
		static UINT afnum = 0;
		static UINT lastnumtime = timeGetTime();
		static UINT seconds = 0;

		if (frame->FrameType == FRAME_A)
			++afnum;
		else
			++vfnum;

		DWORD curnumtime = timeGetTime();
		if (curnumtime - lastnumtime >= 1000)
		{
			++seconds;
			FrameData("%u vframe/s, %u aframe/s\n", vfnum/seconds, afnum/seconds);
			lastnumtime = curnumtime;
		}
#endif

		if (mNeedKeyFrame)
		{
			if (frame->FrameType == FRAME_I)
			{
				mNeedKeyFrame = FALSE;
				return TRUE;
			}

			return FALSE;
		}

		return TRUE;
	}

	return FALSE;
}

void FrameBuf::ClearUnCompleteFrame()
{
	mNoFrame = TRUE;
	mNeedKeyFrame = TRUE;
	if (mBuf != NULL && mBufLen > 0)
		memset(mBuf, 0, sizeof(mBuf));
}

void* FrameBuf::GetFrame()
{
	ASSERT(mBuf != NULL);
	ASSERT(mNoFrame);
	return mBuf;
}

UINT FrameBuf::GetFrameTotalSize()
{
	return mFrameTotalSize;
}

void FrameBuf::ReleaseBuf()
{
	if (mBuf != NULL)
#ifdef _WIN32
		VirtualFree(mBuf, 0, MEM_RELEASE);
#else
	free(mBuf);
#endif

	mBuf = NULL;
	mBufLen = 0;
}

void MsgTransaction::Init(const MsgHeadWrap &req, MsgBuf &rsp)
{
	m_pReq   = &req;
	m_pRsp    = &rsp;
	m_bHasRsp = FALSE;
	//m_Event.Reset();
}

void MsgTransaction::Init(const MsgHeadWrap &req, MsgBuf &rsp, Event &e)
{
}

BOOL MsgTransaction::WaitRsp(U32 milliseconds)
{
	m_Event.Wait(milliseconds);
	return m_bHasRsp;
}

BOOL MsgTransaction::TryRsp(MsgBuf &buf)
{
	if (buf.ValidRsp(*m_pReq))
	{
		m_pRsp->Init();
		*m_pRsp   = buf;
		m_bHasRsp = TRUE;
		m_Event.Notify();

		return TRUE;
	}
	return FALSE;
}

void MsgTransaction::CancelWaitRsp()
{
	//m_Event.Set();
}


void MsgTransQueue::Req(MsgTransaction &trans)
{
	AutoLock lock(m_Mutex);
	m_vcMsgQueue.push_back(&trans);
}

void MsgTransQueue::Rsp(MsgBuf &buf)
{
	AutoLock lock(m_Mutex);
	for (size_t i = 0; i < m_vcMsgQueue.size(); ++i)
	{
		if (m_vcMsgQueue[i]->TryRsp(buf))
			break;
	}
}

void MsgTransQueue::Clear(MsgTransaction &trans)
{
	AutoLock lock(m_Mutex);
	vector<MsgTransaction *>::iterator i = find(m_vcMsgQueue.begin(), m_vcMsgQueue.end(), &trans);
	if (i != m_vcMsgQueue.end())
	{
		m_vcMsgQueue.erase(i);
		trans.CancelWaitRsp();
	}
}

void MsgTransQueue::Clear()
{
	AutoLock lock(m_Mutex);
	for (size_t i = 0; i < m_vcMsgQueue.size(); ++i)
		m_vcMsgQueue[i]->CancelWaitRsp();

	m_vcMsgQueue.clear();
}


BOOL SendMsg(SyncSocket& sock, const MsgHeadWrap& head, const VOID *data)
{
	if (0)
	{
#if 0
		if(head.Version == MsgHeadV2)
		{
			BYTE headBuf[128];
			memcpy(headBuf,(BYTE*)&head,128);
			INT rt = YSFEncode(headBuf+2, head.GetHeadExtLen() - 2, 2, 2);
			if(rt != S_OK)
			{
				return FALSE;
			}
			else
			{
				BYTE dataBuf[MaxMsgBufLen];
				memcpy(dataBuf,data,head.GetDataLen());
				INT rt = YSFEncode(dataBuf, head.GetDataLen(), 2, 2);
				if(rt != S_OK)
				{
					return FALSE;
				}
				else
				{
					return sock.Send(headBuf, head.GetHeadExtLen(), dataBuf, head.GetDataLen());
				}
			}
		}
		else
			return sock.Send(&head, head.GetHeadExtLen(), data, head.GetDataLen());
#endif
	}else
		return sock.Send((VOID *)&head, head.GetHeadExtLen(), (VOID *)data, head.GetDataLen());
}

BOOL RecvMsg(SyncSocket& sock, MsgBuf& buf)
{
	buf.Init();
	if(sock.Recv(buf.GetHead(), buf.GetHeadLen()) && 
			sock.Recv(buf.GetExt(), buf.GetExtLen()))
	{
		if (0)
		{
#if 0
			if(buf.Version == MsgHeadV2)
			{
				//½âÃÜ
				BYTE msgBuf[128];
				memcpy(msgBuf,buf.GetHead(),128);
				INT rt = YSFDecode(msgBuf+2, buf.GetHeadExtLen()-2, 2, 2);
				if(rt == S_OK)
				{
					memcpy((void*)&buf,msgBuf,buf.GetHeadExtLen());
				}
				else
				{
					return FALSE;
				}

				if(sock.Recv(buf.GetData(), buf.GetDataLen()))
				{
					BYTE dataBuf[MaxMsgBufLen];
					if(buf.GetDataLen() > MaxMsgBufLen)
					{
						return FALSE;
					}
					memcpy(dataBuf,buf.GetData(),buf.GetDataLen());
					INT rt = YSFDecode(dataBuf, buf.GetDataLen(), 2, 2);
					if(rt == S_OK)
					{
						memcpy(buf.GetData(),dataBuf,buf.GetDataLen());
						return TRUE;
					}
					else
					{
						return FALSE;
					}
				}
			}
			else
				return sock.Recv(buf.GetData(), buf.GetDataLen());
#endif
		}
		else
			return sock.Recv(buf.GetData(), buf.GetDataLen());
	}

	return FALSE;	
}
