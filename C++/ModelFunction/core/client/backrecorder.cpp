#include "backrecorder.h"
#include "cu.h"
#include "streamgr.h"
#include "frame.h"
#include "tls_error.h"
#include "fsrw.h"


BackRecorder::BackRecorder(Cu *cu)
{
    ASSERT(cu);
    m_cu         = cu;
    m_writer     = NULL;

}

BackRecorder::~BackRecorder()
{
    if (m_writer)
    {
        delete m_writer;
    }
        
    m_writer = NULL;
}

int
BackRecorder::start(string json, string filepath)
{
    m_json       = json;    
    m_filePath   = filepath;
    return streamMgr->addRealStream(m_cu, m_json, this);
}

void
BackRecorder::stop()
{
    streamMgr->delRealStream(m_cu, m_json, this);

    if (!m_writer)
    {
        return;
    }

    m_writer->close();
    delete m_writer;
    m_writer = NULL;
}


void
BackRecorder::onStream(void *stream, uint32_t size)
{
    if (!m_writer)
    {
        ut_frame_s *hdr = (ut_frame_s *)stream;
        if (TAG_STD == hdr->tag)
        {
            m_filePath += ".utv";
            m_writer = new RecordWriter();
        }
        else if (TAG_HIK == hdr->tag)
        {
            m_filePath += ".hkv";
            m_writer = new HikRecordWriter(false);
        }
        else if (TAG_HIK_N == hdr->tag)
        {
            m_filePath += ".hkv";
            m_writer = new HikRecordWriter(true);
        }
        else if (TAG_DAHUA == hdr->tag)
        {
            m_filePath += ".dav";
            m_writer = new DahuaRecordWriter();
        }
        else if (TAG_TIANDY == hdr->tag)
        {
            m_filePath += ".tdv";
            m_writer = new DahuaRecordWriter();
        }
        else
        {
            return;
        }

        if (m_writer->open(m_filePath.c_str()) < 0)
        {
            delete m_writer;
            m_writer = NULL;
            return;
        }
    }

    m_writer->write(stream);
}

void
BackRecorder::onStatus(int code)
{
    
}


int
BackRecorder::capture(string filepath, int type)
{
    ERRE_SDK("not competing.");
    return -1;
}

int
BackRecorder::startREC(string filepath)
{
    return -1;
}

void
BackRecorder::stopREC()
{

}


void
BackRecorder::timerdriver()
{

}
