#include "fs_rwc1.h"

static void generateGuid(uuid_t &uuid)
{
#ifdef _WIN32
	CoCreateGuid(&uuid);
#else
	uuid_generate(uuid);
#endif
}

RecordWriter::RecordWriter()
{	
	m_bNeedKeyFrame  = true;
	m_u64EndTime     = 0;	
	m_u64Offset      = 0;	
}

RecordWriter::~RecordWriter()
{
	close();
}

bool RecordWriter::open(const char *path)
{
	try 
	{
		m_fStream.openWriteReplace(path);
#ifndef _WIN32
		m_fStream.writeCRLF = false;
#endif

	}
	catch(StreamException &)
	{
		return false;
	}

	FS_RECORD_HEAD_S  recordHead = {0};

	recordHead.tag          = FS_RECORD_TAG;
	recordHead.status       = FS_RECORD_DATA;
	recordHead.headInfoSize = sizeof(FS_RECORD_HEAD_INFO_S);
	recordHead.tailOffset   = 0;
	generateGuid(recordHead.uuid);

	m_fStream.write(&recordHead, sizeof(FS_RECORD_HEAD_S));

	FS_RECORD_HEAD_INFO_S recordHeadInfo = {0};
	m_fStream.write(&recordHeadInfo, sizeof(FS_RECORD_HEAD_INFO_S));

	tidy();

	return true;
}

bool RecordWriter::write(void* data)
{
	UT_FRAME_HEAD_S* head = (UT_FRAME_HEAD_S *)data;
	bool bKeyFrame = (FRAME_I == head->FrameType);

	if (m_bNeedKeyFrame)
	{
		if (!bKeyFrame)
		{
			return true;
		}

		m_bNeedKeyFrame = false;
	}

	uint32_t totalFrameSize = sizeof(UT_FRAME_HEAD_S) + head->FrameSize;
	if (totalFrameSize > 1024 *1024) 
	{
		m_bNeedKeyFrame = true;
		return false;
	}

	m_fStream.write(data, totalFrameSize);

	/* ×·¼Ó×Ö½Ú£¬²¹Æë8byte */
	uint8_t  pad[8]  = {0};
	uint32_t padSize = 0;
	padSize = totalFrameSize % 8;
	if (0 != padSize)
	{
		padSize = 8 - padSize;
		m_fStream.write(pad, padSize);
	}

	if (!(FRAME_A == head->FrameType))
	{
		m_u64EndTime = head->Time;
	}

	if (bKeyFrame)
	{
		FS_RECORD_FRAME_INDEX_S frameIndex;
		frameIndex.time   = m_u64EndTime;
		frameIndex.offset = m_u64Offset;

		m_vecFrameIndexs.push_back(frameIndex);
	}

	m_u64Offset += totalFrameSize + padSize;

	return TRUE;
}

void RecordWriter::close()
{
	if (m_u64EndTime != 0)
	{
		FS_RECORD_HEAD_INFO_S headInfo;
		headInfo.begin = m_vecFrameIndexs[0].time;
		headInfo.end   = m_u64EndTime;

		WriteHeadTail(headInfo);
	}

	m_fStream.close();

	tidy();
}

void RecordWriter::tidy()
{
	m_bNeedKeyFrame = true;
	m_u64EndTime    = 0;
	m_u64Offset     = sizeof(FS_RECORD_HEAD_S) + sizeof(FS_RECORD_HEAD_INFO_S);
	m_vecFrameIndexs.clear();
}

#ifndef _WIN32
#define FIELD_OFFSET(type, field)    ((long)(long *)&(((type *)0)->field))
#endif

void RecordWriter::WriteHeadTail(const FS_RECORD_HEAD_INFO_S &headInfo)
{
	ASSERT(m_u64Offset != sizeof(FS_RECORD_HEAD_S) + sizeof(FS_RECORD_HEAD_INFO_S));
	ASSERT((m_u64Offset & (FS_RECORD_DATAALIGN - 1)) == 0);

	int ret = m_fStream.seekTo(FIELD_OFFSET(FS_RECORD_HEAD_S, tailOffset));
	ASSERT(0 == ret);

	m_fStream.write(&m_u64Offset, sizeof(m_u64Offset));

	ret = m_fStream.seekTo(sizeof(FS_RECORD_HEAD_S));
	ASSERT(0 == ret);

	m_fStream.write(&headInfo, sizeof(FS_RECORD_HEAD_INFO_S));

	ret = m_fStream.seekTo(m_u64Offset);
	ASSERT(0 == ret);

	size_t   frameIndexCount  = m_vecFrameIndexs.size();
	uint32_t frameIndexSize = sizeof(FS_RECORD_FRAME_INDEX_S) * frameIndexCount;

	FS_RECORD_TAIL_S recordTail;
	recordTail.tag = FS_RECORD_INDEXLISTTAG;
	recordTail.listHeadInfoSize = 0;
	recordTail.itemSize = sizeof(FS_RECORD_FRAME_INDEX_S);
	recordTail.listSize = frameIndexSize;

	m_fStream.write(&recordTail, sizeof(FS_RECORD_TAIL_S));
	m_fStream.write(&m_vecFrameIndexs.front(), frameIndexSize);

	ret = m_fStream.seekTo(FIELD_OFFSET(FS_RECORD_HEAD_S, status));
	ASSERT(0 == ret);

	const uint32_t status = FS_RECORD_FULL;

	m_fStream.write(&status, sizeof(status));
}


RecordReader::RecordReader()
{

}

RecordReader::~RecordReader()
{
	close();
}

bool RecordReader::open(const char *path)
{
	ASSERT(path);
	
	tidy();
	
	try 
	{
		m_fStream.openReadOnly(path);
#ifndef _WIN32
		m_fStream.writeCRLF = false;
#endif
		
	}
	catch(StreamException &)
	{
		return false;
	}
	
	m_bStoring = TRUE;
	
	m_strPath = path;
	
	if (ReadRecordInfo())
	{
		int iRet = m_fStream.seekTo(m_u64FirstFrameOffset);
		if (iRet < 0)
		{
			close();
			return false;
		}
	}
	
	m_u64Offset = m_u64FirstFrameOffset;
	return true;	
}

bool RecordReader::readFrame(void* frame, uint32_t size)
{
	UT_FRAME_HEAD_S *frameHead = (UT_FRAME_HEAD_S*)frame;
	if (!ReadFrameHead(frameHead))
		return false;
	
	if (size < frameHead->FrameSize + sizeof(UT_FRAME_HEAD_S))
		return false;
	
	if (!ReadFrameData(frameHead, frameHead + 1))
		return false;
	
	return TRUE;	
}

bool RecordReader::seek(uint64_t time)
{		
	FS_RECORD_FRAME_INDEX_S targetFrameIndex;
	targetFrameIndex.time = time;
	FS_RECORD_FRAME_INDEX_S* frameIndex;
	
	if (!m_bGenerateTailInfo)
	{
		frameIndex = GetFrameIndex(targetFrameIndex);
	}	
	else
	{}
	
	int iRet = m_fStream.seekTo(frameIndex->offset);
	ENSURE_RETURN_VAL(iRet >= 0, false);
	
	m_u64Offset = frameIndex->offset;
	return true;
}

void RecordReader::close()
{
	m_fStream.close();
	
	//RepairRecordInfo();
	
	tidy();
}

void RecordReader::getTimeRange(uint64_t &begin, uint64_t &end)
{
	begin = m_u64BeginTime;
	end   = m_u64EndTime;
}

uint64_t RecordReader::getCurrentTime()
{
	return m_u64CurrentTime;
}

void RecordReader::tidy()
{
	m_strPath  = "";
	m_bStoring = false;
	
	memset(&m_stRecordHead, 0, sizeof(m_stRecordHead));

	m_u64FirstFrameOffset = 0;
	m_u64BeginTime        = 0;
	m_u64EndTime          = 0;
	m_u64Offset           = 0;
	m_u64CurrentTime      = 0;
	m_bGenerateTailInfo   = false;
	m_vecFrameIndexs.clear();
}

bool RecordReader::ReadRecordInfo()
{
	if (!ReadRecordHead())
	{
		return false;
	}
	
	if (!ReadRecordHeadInfo())
	{
		return false;
	}
	
	
	m_u64FirstFrameOffset = sizeof(FS_RECORD_HEAD_S) + m_stRecordHead.headInfoSize;
	m_u64Offset = m_u64FirstFrameOffset;

	if (FS_RECORD_FULL == m_stRecordHead.status)
	{
		return ReadRecordIndexs();
	}

	
	return GenerateRecordIndexs();
}

bool RecordReader::ReadRecordHead()
{
	int iRet = m_fStream.read(&m_stRecordHead, sizeof(FS_RECORD_HEAD_S));
	if (iRet < 0)
	{
		return false;
	}
	
	if (FS_RECORD_TAG != m_stRecordHead.tag)
	{
		return false;
	}
		
	return true;
}

bool RecordReader::ReadRecordHeadInfo()
{
	uint32_t headInfoSize = m_stRecordHead.headInfoSize;
	ENSURE_RETURN_VAL(headInfoSize >= sizeof(FS_RECORD_HEAD_INFO_S), false);
	
	FS_RECORD_HEAD_INFO_S recordHeadInfo = {0};
	char* newBuf  = NULL;
	void* readBuf = NULL;
	
	if (headInfoSize == sizeof(FS_RECORD_HEAD_INFO_S))
	{
		readBuf = &recordHeadInfo;
	}
	else
	{
		newBuf  = new char[headInfoSize];
		readBuf = newBuf;
	}
	
	int iRet = m_fStream.read(readBuf, headInfoSize);
	if (iRet < 0)
	{
		if (newBuf)
			delete []newBuf;

		return false;
	}
	
	FS_RECORD_HEAD_INFO_S* headInfo = (FS_RECORD_HEAD_INFO_S *)readBuf;
	m_u64BeginTime = headInfo->begin;
	m_u64EndTime   = headInfo->end;
	
	if (newBuf)
		delete [] newBuf;
	
	return true;
}

bool RecordReader::ReadRecordIndexs()
{
	uint64_t tailOffset = m_stRecordHead.tailOffset;
	ENSURE_RETURN_VAL(tailOffset != 0, false);
	
	int iRet = m_fStream.seekTo(tailOffset);
	ENSURE_RETURN_VAL(iRet >= 0, false);
	
	FS_RECORD_TAIL_S recordTaill = {0};
	
	
	iRet = m_fStream.read(&recordTaill, sizeof(FS_RECORD_TAIL_S));
	ENSURE_RETURN_VAL(iRet >= 0, false);
	
	uint32_t listSize         = recordTaill.listSize;
	uint16_t listHeadInfoSize = recordTaill.listHeadInfoSize;
	ENSURE_RETURN_VAL(listSize >= listHeadInfoSize, false);

	ENSURE_RETURN_VAL(FS_RECORD_INDEXLISTTAG == recordTaill.tag, false);
	
	if (0 != listHeadInfoSize)
	{
		char listHeadInfo[512] = {0};
		iRet =  m_fStream.read(listHeadInfo, listHeadInfoSize);
		ENSURE_RETURN_VAL(iRet >= 0, false);
		
		listSize -= listHeadInfoSize;
	}
	
	uint16_t itemSize = recordTaill.itemSize;
	ENSURE_RETURN_VAL(itemSize >= sizeof(FS_RECORD_FRAME_INDEX_S), false);
	
	uint32_t itemCount = listSize / itemSize;
	ENSURE_RETURN_VAL(itemCount != 0, false);
	m_vecFrameIndexs.resize(itemCount, FS_RECORD_FRAME_INDEX_S());
	
	if (itemSize == sizeof(FS_RECORD_FRAME_INDEX_S))
	{
		iRet =  m_fStream.read(&m_vecFrameIndexs.front(), listSize);
		ENSURE_RETURN_VAL(iRet >= 0, false);

#ifdef _D_TEST
		for (size_t i = 0; i < itemCount; ++i)
		{
			FS_RECORD_FRAME_INDEX_S curItem = m_vecFrameIndexs[i];
		}
#endif
	}
	else
	{
		char* listItems = new char[listSize];
		ENSURE_RETURN_VAL(listItems, false);
		
		iRet =  m_fStream.read(listItems, listSize);
		if (iRet < 0)
		{
			delete []listItems;
			return false;
		}
		
		FS_RECORD_FRAME_INDEX_S *curItem = (FS_RECORD_FRAME_INDEX_S *)listItems;
		for (size_t i = 0; i < itemCount; ++i)
		{
			m_vecFrameIndexs[i] = *curItem;
			curItem++;
		}
		
		delete [] listItems;
	}

	return true;
}

bool RecordReader::GenerateRecordIndexs()
{
	return false;
}

bool  RecordReader::ReadFrameHead(UT_FRAME_HEAD_S *head)
{
	if (m_u64Offset >= m_stRecordHead.tailOffset)
	{
		return false;
	}
		
	int iRet = m_fStream.read(head, sizeof(UT_FRAME_HEAD_S));
	if (iRet < 0)
	{
		return false;
	}
		
	ENSURE_RETURN_VAL(FRAME_TAG_STD_H264 == head->FrameTag, false);
	m_u64Offset += sizeof(UT_FRAME_HEAD_S);

	if (FRAME_I == head->FrameType ||
		FRAME_P == head->FrameType)
	{
		m_u64CurrentTime = head->Time;
	}

	return true;
}

bool  RecordReader::ReadFrameData(const UT_FRAME_HEAD_S* head, void* data)
{
	uint32_t frameSize = (head->FrameSize + FS_RECORD_DATAALIGN - 1) & ~(FS_RECORD_DATAALIGN - 1);
	
	int iRet = m_fStream.read(data, frameSize);
	if (iRet < 0)
	{
		return false;
	}
	
	m_u64Offset += frameSize;

	return true;
}

static bool CompareFrameIndex(const FS_RECORD_FRAME_INDEX_S& frameIndex1, const FS_RECORD_FRAME_INDEX_S& frameIndex2)
{
	return frameIndex1.time < frameIndex2.time;
}

FS_RECORD_FRAME_INDEX_S* RecordReader::GetFrameIndex(const FS_RECORD_FRAME_INDEX_S& targetFrameIndex)
{
	FS_RECORD_FRAME_INDEX_S* frameIndex = 
		upper_bound(
		&m_vecFrameIndexs.front(), 
		&m_vecFrameIndexs.front() + m_vecFrameIndexs.size(),
		targetFrameIndex, CompareFrameIndex);
	
	if (frameIndex != &m_vecFrameIndexs.front())
		--frameIndex;
	
	return frameIndex;
}
