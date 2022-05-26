/**
 * @file      fsrw.h
 * @copyright Copyright (c) 2018, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2018-01-23 18:40:48
 *
 * @note
 *  fsrw.h defines
 */
#ifndef __FSRW_H__
#define __FSRW_H__
#include "stream.h"
#include "fshdr.h"
#include "frame.h"
#include "wav.h"

class CM_API IWriter
{
public:
    virtual ~IWriter() {};

public:
    virtual int        open(const char *path)               = 0;
    virtual int        write(void* data, uint32_t size = 0) = 0;
    virtual void       close()                              = 0;
};

class CM_API IReader
{
public:
    virtual ~IReader() {};

public:
    virtual int        open(const char *path)                = 0;
    virtual int        seek(uint64_t time)                   = 0;
    virtual void       close()                               = 0;

    virtual int        readFrame(void* frame, uint32_t size) = 0;
    virtual void       range(uint64_t &begin, uint64_t &end) = 0;
    virtual uint64_t   current()                             = 0;
    virtual uint32_t   type()                                = 0;
};

typedef vector<fs_frame_idx_s> FSRecordIDXS;
class CM_API RecordWriter : public IWriter
{
public:
    RecordWriter();
    ~RecordWriter();

public:
    virtual int        open(const char *path);
    virtual int        write(void* data, uint32_t size = 0);
    virtual void       close();

protected:
    void writeHeadTail(const fs_record_info_s &headInfo);
    void tidy();

private:
    FileStream         m_stream;
    int                m_neadIFrame;
    uint64_t           m_begin;
    uint64_t           m_end;
    uint64_t           m_offset;
    FSRecordIDXS       m_idxs;
};

class CM_API WavWriter : public IWriter
{
public:
    WavWriter(uint16_t tag, uint16_t channels, uint32_t samplesRate, uint16_t bitsPerSample);
    ~WavWriter();
public:
    virtual int        open(const char *path);
    virtual int        write(void* data, uint32_t size = 0);
    virtual void       close();

protected:
    void               updateHead();

private:
    uint32_t           m_tag;
    uint16_t           m_channels;
    uint32_t           m_samplesRate;
    uint16_t           m_bitsPerSample;

    uint32_t           m_totalSize;

    FileStream         m_stream;
};

class CM_API HikRecordWriter : public IWriter
{
public:
    HikRecordWriter(bool isNew);
    ~HikRecordWriter();

public:
    virtual int        open(const char *path);
    virtual int        write(void* data, uint32_t size = 0);
    virtual void       close();


private:
    FileStream         m_stream;
    bool               m_bNew;
};

class CM_API DahuaRecordWriter : public IWriter
{
public:
    DahuaRecordWriter();
    ~DahuaRecordWriter();

public:
    virtual int        open(const char *path);
    virtual int        write(void* data, uint32_t size = 0);
    virtual void       close();


private:
    FileStream         m_stream;
    bool               m_bNew;
};


class CM_API RecordReader : public IReader
{
public:
    RecordReader();
    ~RecordReader();
public:
    virtual int        open(const char *path);
    virtual int        readFrame(void* frame, uint32_t size);
    virtual int        seek(uint64_t time);
    virtual void       close();
    virtual void       range(uint64_t &begin, uint64_t &end);
    virtual uint64_t   current();
    virtual uint32_t   type();

protected:
    void               tidy();
    int                readInfo();
    int                readHead();
    int                readHeadInfo();
    int                readIdxs();
    int                generateIdxs();
    int                readFrameHead(ut_frame_s *head);
    int                readFrameData(const ut_frame_s* head, void* data);

    fs_frame_idx_s   * frameidx(const fs_frame_idx_s& targetIdx);

private:
    FileStream         m_stream;
    string             m_path;

    bool               m_storing;
    bool volatile      m_hasTail;

    fs_record_head_s   m_rechdr;
    uint64_t           m_firstOffset;
    uint64_t           m_begin;
    uint64_t           m_end;
    uint64_t           m_offset;
    uint64_t           m_current;

    FSRecordIDXS       m_idxs;
};
#endif
