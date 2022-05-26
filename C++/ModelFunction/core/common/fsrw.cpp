#include "fsrw.h"
#include "tls_error.h"

static void
generateGuid(uuid_t &uuid)
{
#ifdef _WIN32
    CoCreateGuid(&uuid);
#else
    uuid_generate(uuid);
#endif
}

RecordWriter::RecordWriter()
{
    m_neadIFrame = true;
    m_begin      = 0;
    m_end        = 0;
    m_offset     = 0;
}

RecordWriter::~RecordWriter()
{
    /* close(); */
}

int
RecordWriter::open(const char *path)
{
    try
    {
        m_stream.openWriteReplace(path);
        m_stream.writeCRLF = false;
    }
    catch (StreamException &e)
    {
        ERRE_COM("%s", e.what());
        return -1;
    }

    fs_record_head_s recordHead;
    memset(&recordHead, 0, sizeof(fs_record_head_s));

    recordHead.tag          = FS_RECORD_TAG;
    recordHead.status       = FS_RECORD_DATA;
    recordHead.headInfoSize = sizeof(fs_record_info_s);
    recordHead.tailOffset   = 0;
    generateGuid(recordHead.uuid);

    m_stream.write(&recordHead, sizeof(fs_record_head_s));

    fs_record_info_s hdrInfo;
    memset(&hdrInfo, 0, sizeof(fs_record_info_s));

    m_stream.write(&hdrInfo, sizeof(fs_record_info_s));

    tidy();

    return 0;
}

int
RecordWriter::write(void* data, uint32_t)
{
    ut_frame_s* hdr = (ut_frame_s *)data;
    bool keyFrame = ('I' == hdr->type);

    if (0 == m_begin)
    {
        m_begin = hdr->time;
    }

    if (m_neadIFrame && 'A' != hdr->type)
    {
        if (!keyFrame)
        {
            LOGE_COM("framem_neadIFrame");
            return 0;
        }

        m_neadIFrame = false;
    }

    uint32_t frameSize = sizeof(ut_frame_s) + hdr->size;
    if (frameSize > 1024 * 1024)
    {
        m_neadIFrame = true;

        LOGW_COM("frame is too large: %d", frameSize);
        return -1;
    }

    m_stream.write(data, frameSize);

    /* aligen 8byte */
    uint8_t  pad[8]  = {0};
    uint32_t padSize = 0;
    padSize = frameSize % 8;

    if (0 != padSize)
    {
        padSize = 8 - padSize;
        m_stream.write(pad, padSize);
    }

    /* if (!('A == hdr->type)) */
    if (hdr->time == 0)
    {
        LOGI_COM("frame time is 0");
    }
    else
    {
        m_end = hdr->time;
    }

    if (keyFrame)
    {
        fs_frame_idx_s frameIndex;
        frameIndex.time   = m_end;
        frameIndex.offset = m_offset;

        m_idxs.push_back(frameIndex);
    }

    m_offset += frameSize + padSize;

    return 0;
}

void
RecordWriter::close()
{
    if (m_end != 0)
    {
        fs_record_info_s headInfo;

        headInfo.begin = m_begin;
        headInfo.end   = m_end;
        LOGI_SDK("RecordWriter::close::writeHeadTail");
        writeHeadTail(headInfo);
    }


    m_stream.close();

    tidy();
}

void
RecordWriter::tidy()
{
    m_neadIFrame = true;
    m_end        = 0;
    m_offset     = sizeof(fs_record_head_s) + sizeof(fs_record_info_s);

    m_idxs.clear();
}

#ifndef _WIN32
#define FIELD_OFFSET(type, field)    ((long)(long *)&(((type *)0)->field))
#endif

void
RecordWriter::writeHeadTail(const fs_record_info_s &headInfo)
{
    ASSERT(m_offset != sizeof(fs_record_head_s) + sizeof(fs_record_info_s));
    ASSERT((m_offset & (FS_RECORD_DATAALIGN - 1)) == 0);

    int result = m_stream.seekTo(FIELD_OFFSET(fs_record_head_s, tailOffset));
    ASSERT(0 == result);

    m_stream.write(&m_offset, sizeof(m_offset));

    result = m_stream.seekTo(sizeof(fs_record_head_s));
    ASSERT(0 == result);

    m_stream.write(&headInfo, sizeof(fs_record_info_s));

    result = m_stream.seekTo((int)m_offset);
    ASSERT(0 == result);

    size_t   frameIndexCount  = m_idxs.size();
    uint32_t frameIndexSize = sizeof(fs_frame_idx_s) * frameIndexCount;

    fs_record_tail_s tail;
    memset(&tail, 0, sizeof(fs_record_tail_s));
    tail.tag              = FS_RECORD_INDEXLISTTAG;
    tail.listHeadInfoSize = 0;
    tail.itemSize         = sizeof(fs_frame_idx_s);
    tail.listSize         = frameIndexSize;

    m_stream.write(&tail, sizeof(fs_record_tail_s));
    m_stream.write(&m_idxs.front(), frameIndexSize);

    result = m_stream.seekTo(FIELD_OFFSET(fs_record_head_s, status));
    ASSERT(0 == result);

    uint32_t status = FS_RECORD_FULL;

    if (0 == m_idxs.size())
    {
        status = FS_RECORD_AUDI;
    }

    m_stream.write(&status, sizeof(status));
}


RecordReader::RecordReader()
{
    tidy();
}

RecordReader::~RecordReader()
{
    /* close(); */
}

int
RecordReader::open(const char *path)
{
    ASSERT(path);

    tidy();

    try
    {
        m_stream.openReadOnly(path);
        m_stream.writeCRLF = false;
    }
    catch (StreamException &e)
    {
        ERRE_COM("%s", e.what());
        return -1;
    }

    m_storing = true;

    m_path = path;

    if (readInfo() < 0)
    {
        return -1;
    }

    int result = m_stream.seekTo((int)m_firstOffset);
    if (result < 0)
    {
        close();
        ERRE_COM("stream seek fail.");
        return -1;
    }

    m_offset = m_firstOffset;
    return 0;
}

int
RecordReader::readFrame(void* frame, uint32_t size)
{
    ut_frame_s *hdr = (ut_frame_s*)frame;
    if (readFrameHead(hdr) < 0)
    {
        return -1;
    }

    if (size < hdr->size + sizeof(ut_frame_s))
    {
        ERRE_COM("user size is insufficient: %d", size);
        return -1;
    }

    if (readFrameData(hdr, hdr + 1) < 0)
    {
        return -1;
    }

    return 0;
}

int
RecordReader::seek(uint64_t time)
{
    if (time < m_begin)
    {
        time = m_begin;
    }
    else if (time > m_end)
    {
        time = m_end;
    }

    uint64_t offset  = m_firstOffset;
    uint64_t current = 0;
    if (FS_RECORD_AUDI == m_rechdr.status)
    {
        double dat = (double)((int64_t)(m_end - m_begin));
        /* offset = (int64_t)(time - m_begin) / dat * (int64_t)(m_rechdr.tailOffset - m_firstOffset); */
        offset = (int64_t)((time - m_begin) / dat * (int64_t)(m_rechdr.tailOffset - m_firstOffset));

        /* align with 360 bytes */
        offset = (offset + 359) / 360 * 360 + m_firstOffset;
    }
    else
    {
        fs_frame_idx_s targetIdx;
        targetIdx.time = time;
        fs_frame_idx_s* idx;

        if (!m_hasTail)
        {
            generateIdxs();
        }

        idx     = frameidx(targetIdx);
        offset  = idx->offset;
        current = idx->time;
    }

    if (m_stream.seekTo((int)offset) < 0)
    {
        ERRE_COM("stream seek fail.");
        return -1;
    }

    m_offset  = offset;
    m_current = current;

    return 0;
}

void
RecordReader::close()
{
    m_stream.close();

    //RepairRecordInfo();

    tidy();
}

void
RecordReader::range(uint64_t &begin, uint64_t &end)
{
    begin = m_begin;
    end   = m_end;
}

uint64_t
RecordReader::current()
{
    return m_current;
}

uint32_t
RecordReader::type()
{
    if (FS_RECORD_AUDI == m_rechdr.status)
    {
        /* pure audio file */
        return 1;
    }
    else
    {
        /* video and audio mixed file */
        return 0;
    }
}

void
RecordReader::tidy()
{
    m_path        = "";
    m_storing     = false;

    m_firstOffset = 0;
    m_begin       = 0;
    m_end         = 0;
    m_offset      = 0;
    m_current     = 0;
    m_hasTail     = false;

    memset(&m_rechdr, 0, sizeof(m_rechdr));
    m_idxs.clear();
}

int
RecordReader::readInfo()
{
    if (readHead() < 0)
    {
        return -1;
    }

    if (readHeadInfo() < 0)
    {
        return -1;
    }


    m_firstOffset = sizeof(fs_record_head_s) + m_rechdr.headInfoSize;
    m_offset = m_firstOffset;

    if (FS_RECORD_FULL == m_rechdr.status)
    {
        return readIdxs();
    }


    return generateIdxs();
}

int
RecordReader::readHead()
{
    int result = m_stream.read(&m_rechdr, sizeof(fs_record_head_s));
    if (result < 0)
    {
        ERRE_COM("stream read fail.");
        return -1;
    }

    if (FS_RECORD_TAG != m_rechdr.tag)
    {
        ERRE_COM("unknow record tag: %s", m_rechdr.tag);
        return -1;
    }

    return 0;
}

int
RecordReader::readHeadInfo()
{
    int result = 0;
    do
    {
        uint32_t headInfoSize = m_rechdr.headInfoSize;
        if (headInfoSize < sizeof(fs_record_info_s))
        {
            ERRE_COM("headInfoSize < sizeof(fs_record_info_s)");
            result = -1;
            break;
        }

        fs_record_info_s hdrInfo;
        memset(&hdrInfo, 0, sizeof(fs_record_info_s));

        char* newBuf  = NULL;
        void* readBuf = NULL;

        if (headInfoSize == sizeof(fs_record_info_s))
        {
            readBuf = &hdrInfo;
        }
        else
        {
            newBuf  = new char[headInfoSize];
            readBuf = newBuf;
        }

        int result = m_stream.read(readBuf, headInfoSize);
        if (result < 0)
        {
            ERRE_COM("stream read fail.");

            if (newBuf)
            {
                delete []newBuf;
            }

            result = -1;
            break;
        }

        fs_record_info_s* headInfo = (fs_record_info_s *)readBuf;
        m_begin = headInfo->begin;
        m_end   = headInfo->end;

        if (newBuf)
        {
            delete []newBuf;
        }
    }
    while (0);

    return result;
}

int
RecordReader::readIdxs()
{
    int result = 0;
    do
    {
        uint64_t tailOffset = m_rechdr.tailOffset;
        if (m_stream.seekTo((int)tailOffset) < 0)
        {
            ERRE_COM("stream seek fail.");
            result = -1;
            break;
        }

        fs_record_tail_s tail;
        memset(&tail, 0, sizeof(fs_record_tail_s));

        if (m_stream.read(&tail, sizeof(fs_record_tail_s)) < 0)
        {
            ERRE_COM("stream read fail.");
            result = -1;
            break;
        }

        uint32_t listSize         = tail.listSize;
        uint16_t listHeadInfoSize = tail.listHeadInfoSize;

        if (listSize < listHeadInfoSize)
        {
            ERRE_COM("listSize >= listHeadInfoSize");
            result = -1;
            break;
        }

        if (FS_RECORD_INDEXLISTTAG != tail.tag)
        {
            ERRE_COM("FS_RECORD_INDEXLISTTAG != tail.tag");
            result = -1;
            break;
        }

        if (0 != listHeadInfoSize)
        {
            char listHeadInfo[512] = {0};
            if (m_stream.read(listHeadInfo, listHeadInfoSize) < 0)
            {
                ERRE_COM("stream read fail.");
                result = -1;
                break;
            }

            listSize -= listHeadInfoSize;
        }

        uint16_t itemSize = tail.itemSize;
        if (itemSize < sizeof(fs_frame_idx_s))
        {
            ERRE_COM("itemSize < sizeof(fs_frame_idx_s)");
            result = -1;
            break;
        }

        uint32_t itemCount = listSize / itemSize;
        if (0 == itemCount)
        {
            ERRE_COM("0 == itemCount");
            result = -1;
            break;
        }
        m_idxs.resize(itemCount, fs_frame_idx_s());

        if (itemSize == sizeof(fs_frame_idx_s))
        {
            m_stream.read(&m_idxs.front(), listSize);

#ifdef _D_TEST
            for (size_t i = 0; i < itemCount; ++i)
            {
                fs_frame_idx_s curItem = m_idxs[i];
            }
#endif
        }
        else
        {
            char* listItems = new char[listSize];

            if (m_stream.read(listItems, listSize) < 0)
            {
                ERRE_COM("m_stream.read(listItems, listSize) < 0");
                delete []listItems;
                result = -1;
                break;
            }

            fs_frame_idx_s *curItem = (fs_frame_idx_s *)listItems;
            for (size_t i = 0; i < itemCount; ++i)
            {
                m_idxs[i] = *curItem;
                curItem++;
            }

            delete [] listItems;
        }

    }
    while (0);

    return result;
}

int
RecordReader::generateIdxs()
{
    ERRE_COM("RecordReader::generateIdxs()");
    /* m_hasTail = true; */
    return -1;
}

int
RecordReader::readFrameHead(ut_frame_s *hdr)
{
    if (m_offset >= m_rechdr.tailOffset)
    {
        ERRW_COM("read file end");
        return -1;
    }

    if (m_stream.read(hdr, sizeof(ut_frame_s)) < 0)
    {
        ERRE_COM("stream read fail");
        return -1;
    }

    /* ENSURE_RETURN_VAL(FRAME_TAG_STD_H264 == hdr->FrameTag, false); */
    m_offset += sizeof(ut_frame_s);

    if ('I' == hdr->type || 'P' == hdr->type)
    {
        m_current = hdr->time;
    }

    if ('A' == hdr->type)
    {
        m_current = hdr->time;
    }

    return 0;
}

int
RecordReader::readFrameData(const ut_frame_s* hdr, void* data)
{
    uint32_t frameSize = (hdr->size + FS_RECORD_DATAALIGN - 1) & ~(FS_RECORD_DATAALIGN - 1);

    int result = m_stream.read(data, frameSize);
    if (result < 0)
    {
        ERRE_COM("stream read fail");
        return -1;
    }

    m_offset += frameSize;

    return 0;
}

static bool
compare_index(const fs_frame_idx_s& frameIndex1, const fs_frame_idx_s& frameIndex2)
{
    return frameIndex1.time < frameIndex2.time;
}

fs_frame_idx_s*
RecordReader::frameidx(const fs_frame_idx_s& targetIdx)
{
    fs_frame_idx_s* frameIndex = upper_bound(&m_idxs.front(), &m_idxs.front() + m_idxs.size(), targetIdx, compare_index);

    if (frameIndex != &m_idxs.front())
    {
        --frameIndex;
    }

    return frameIndex;
}

WavWriter::WavWriter(uint16_t tag, uint16_t channels, uint32_t samplesRate, uint16_t bitsPerSample)
{
    m_tag           = tag;
    m_channels      = channels;
    m_samplesRate   = samplesRate;
    m_bitsPerSample = bitsPerSample;

    m_totalSize = 0;
}

WavWriter::~WavWriter()
{
    /* close(); */
}

int
WavWriter::open(const char *path)
{
    try
    {
        m_stream.openWriteReplace(path);
        m_stream.writeCRLF = false;
    }
    catch (StreamException &e)
    {
        ERRE_COM("%s", e.what());
        return -1;
    }

    wav_head_s hdr;
    memset(&hdr, 0, sizeof(wav_head_s));

    hdr.riff.riff[0] = 'R';
    hdr.riff.riff[1] = 'I';
    hdr.riff.riff[2] = 'F';
    hdr.riff.riff[3] = 'F';

    //hdr.riff.riff = ((*(uint32_t*)"RIFF"));
    hdr.riff.size = 44;
    hdr.riff.wave[0] = 'W';
    hdr.riff.wave[1] = 'A';
    hdr.riff.wave[2] = 'V';
    hdr.riff.wave[3] = 'E';

    hdr.format.fmt[0] = 'f';
    hdr.format.fmt[1] = 'm';
    hdr.format.fmt[2] = 't';

    /* NOTE: here is a space, not 0 */
    hdr.format.fmt[3] = ' ';

    hdr.format.fmtLen         = 16;
    hdr.format.tag            = FORMAT_ALAW;
    hdr.format.channels       = m_channels;
    hdr.format.samplesRate    = m_samplesRate;
    hdr.format.bitsPerSample  = m_bitsPerSample;
    hdr.format.bytesPerSample = m_bitsPerSample / 8;
    hdr.format.bytesRate      = m_samplesRate * m_bitsPerSample / 8;

    hdr.data[0] = 'd';
    hdr.data[1] = 'a';
    hdr.data[2] = 't';
    hdr.data[3] = 'a';
    hdr.length  = 1024 * 1024;

    m_stream.write(&hdr, sizeof(wav_head_s));

    return 0;
}

int
WavWriter::write(void* data, uint32_t)
{
    ut_frame_s* hdr = (ut_frame_s *)data;
    if ('A' != hdr->type)
    {
        return 0;
    }

    uint32_t frameSize = hdr->size;
    m_stream.write((uint8_t *)data + 40, frameSize);

    /* align 8byte */
    uint8_t  pad[8]  = { 0 };
    uint32_t padSize = 0;

    padSize = frameSize % 8;
    if (0 != padSize)
    {
        padSize = 8 - padSize;
        m_stream.write(pad, padSize);
    }

    m_totalSize += frameSize;
    m_totalSize += padSize;

    return 0;
}

void
WavWriter::close()
{
    updateHead();
    m_stream.close();
}

void
WavWriter::updateHead()
{

    int result = m_stream.seekTo(FIELD_OFFSET(wav_riff_s, size));
    ASSERT(0 == result);

    uint32_t size = 36 + m_totalSize;
    m_stream.write(&size, sizeof(size));

    result = m_stream.seekTo(FIELD_OFFSET(wav_head_s, length));
    ASSERT(0 == result);

    m_stream.write(&size, sizeof(m_totalSize));
}

HikRecordWriter::HikRecordWriter(bool bNew) :m_bNew(bNew)
{
    
}

HikRecordWriter::~HikRecordWriter()
{
    
        
}

int
HikRecordWriter::open(const char *path)
{
    try
    {
        m_stream.openWriteReplace(path);
        m_stream.writeCRLF = false;
    }
    catch (StreamException &e)
    {
        ERRE_COM("%s", e.what());
        return -1;
    }

    if (!m_bNew)
    {
        m_stream.write(hikOldHdr, 40);
    }
    else
    {
        m_stream.write(hikNewHdr, 40);
    }

    return 0;
}

int
HikRecordWriter::write(void* data, uint32_t size)
{
    ut_frame_s* hdr = (ut_frame_s *)data;
    m_stream.write(hdr->buf, hdr->size);
    return 0;   
}

void
HikRecordWriter::close()
{
    m_stream.close();
}

DahuaRecordWriter::DahuaRecordWriter()
{

}

DahuaRecordWriter::~DahuaRecordWriter()
{


}

int
DahuaRecordWriter::open(const char *path)
{
    try
    {
        m_stream.openWriteReplace(path);
        m_stream.writeCRLF = false;
    }
    catch (StreamException &e)
    {
        ERRE_COM("%s", e.what());
        return -1;
    }



    return 0;
}

int
DahuaRecordWriter::write(void* data, uint32_t)
{
    ut_frame_s* hdr = (ut_frame_s *)data;
    m_stream.write(hdr->buf, hdr->size);
    return 0;
}

void
DahuaRecordWriter::close()
{
    m_stream.close();
}


