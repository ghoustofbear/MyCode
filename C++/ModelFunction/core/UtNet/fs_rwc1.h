/**
 * (C) Copyright 2013, XXXX Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 * Tue Feb 26 16:45:16 PST 2013
 *
 * fs_rw.h
 */
#ifndef __FS_RW__
#define __FS_RW__
#include "stream.h"
#include "fs_head.h"
#include "framec1.h"

class CM_API IWriter
{
	public:
		virtual ~IWriter() {};

		virtual bool open(const char *path) = 0;

		virtual bool write(void* data)     = 0;

		virtual void close() = 0;
};

class RecordWriter : public IWriter
{
	public:
		RecordWriter();
		~RecordWriter();

		bool open(const char *path);

		bool write(void* data);

		void close();
		
	protected:
		void WriteHeadTail(const FS_RECORD_HEAD_INFO_S &headInfo);
		void tidy();

	private:
		FileStream                      m_fStream;
		bool                            m_bNeedKeyFrame;
		uint64_t                        m_u64EndTime;
		uint64_t                        m_u64Offset;
	    vector<FS_RECORD_FRAME_INDEX_S> m_vecFrameIndexs;
};


class CM_API IReader
{
	public:
		virtual ~IReader() {};

		virtual bool open(const char *path) = 0;

		virtual bool readFrame(void* frame, uint32_t size) = 0;

		virtual bool seek(uint64_t time) = 0;

		virtual void close() = 0;

		virtual void getTimeRange(uint64_t &begin, uint64_t &end) = 0;

		virtual uint64_t getCurrentTime() = 0;
};

class RecordReader : public IReader
{
	public:
		RecordReader();
		~RecordReader();

		bool open(const char *path);
		
		bool readFrame(void* frame, uint32_t size);
		
		bool seek(uint64_t time);
		
		void close();
		
		void getTimeRange(uint64_t &begin, uint64_t &end);

		uint64_t getCurrentTime();

	protected:
		void tidy();
		
		bool ReadRecordInfo();
		bool ReadRecordHead();	
		bool ReadRecordHeadInfo();
		bool ReadRecordIndexs();
		bool GenerateRecordIndexs();

		bool ReadFrameHead(UT_FRAME_HEAD_S *head);
		bool ReadFrameData(const UT_FRAME_HEAD_S* head, void* data);


		FS_RECORD_FRAME_INDEX_S* GetFrameIndex(const FS_RECORD_FRAME_INDEX_S& targetFrameIndex);

	private:
		FileStream                      m_fStream;
		string                          m_strPath;

		bool                            m_bStoring;
		bool volatile                   m_bGenerateTailInfo;
		
		FS_RECORD_HEAD_S                m_stRecordHead;
		uint64_t                        m_u64FirstFrameOffset;
		uint64_t                        m_u64BeginTime;
		uint64_t                        m_u64EndTime;
		uint64_t                        m_u64Offset;
		uint64_t                        m_u64CurrentTime;

	    vector<FS_RECORD_FRAME_INDEX_S> m_vecFrameIndexs;
};
#endif
