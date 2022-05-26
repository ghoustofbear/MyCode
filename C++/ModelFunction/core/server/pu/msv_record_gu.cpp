#include "msv_record_gu.h"
#include "msv_pu.h"
#include "gumgr.h"
#include "frame.h"
#include "sys.h"
#include "tls_error.h"

void
MsvRecordGu::status_cb(int code, void* userData)
{
    if (CODE_SUCCESS == code)
    {
        LOGI_MSV(c2_get_error());
    }
    else
    {
        LOGI_MSV(c2_get_error());
    }
}

bool
MsvRecordGu::stream_cb(HWND wnd, void *data, uint32_t size, void *userData)
{
    if (!data || size <= 0 || !userData)
    {
        return true;
    }

    MsvRecordGu * gu = (MsvRecordGu *)userData;

    ut_frame_s *hdr = (ut_frame_s *)data;
    if (TAG_STD == hdr->tag)
    {
        switch (hdr->type)
        {
        case 'I':
        case 'P':
            break;
            gu->notifyVideoStream(hdr->buf, hdr->size, hdr->encode, hdr->type, hdr->video.width, hdr->video.height, hdr->video.fps, hdr->pts);
        case 'A':
            break;
            gu->notifyAudioStream(hdr->buf, hdr->size, hdr->encode, hdr->audio.sps, hdr->audio.channels, hdr->audio.bps, hdr->pts);
        default:
            break;
        }
    }
    else
    {
        gu->notifyPrivStream(hdr->buf, hdr->size, hdr->encode, hdr->tag);
    }

    gu->traffic(size);
    gu->m_lastpts = sys->currentTimeMillis();

    return true;
}

MsvRecordGu::MsvRecordGu(Pu *pu, string filename, string addrcode, int stream, int streamType, int mode)
    : RecordGu(pu, filename, addrcode, streamType, mode)
{
    m_record = NULL;
    m_stream = stream;
}

MsvRecordGu::MsvRecordGu(Pu *pu, string addrcode, int type, datetime_s &start, datetime_s &end, int stream, int streamType, int mode)
    : RecordGu(pu, addrcode, type, start, end, streamType, mode)
{
    m_record = NULL;
    m_stream = stream;
}

MsvRecordGu::~MsvRecordGu()
{

}

int
MsvRecordGu::start(int offset)
{
    if (!m_pu->online())
    {
        ERRE_MSV("device ip: %s is not on-line.", m_pu->ip().c_str());

        return -1;
    }

    if (m_byname)
    {
        c2_pb_name_s pb;
        memset(&pb, 0, sizeof(pb));

        if (1 == m_stream)
        {
            pb.obtainType = 3;
        }
        else
        {
            pb.obtainType = 2;
        }

        pb.filename = (char *)m_filename.c_str();
        pb.addrcode = (char *)m_addrcode.c_str();

        m_record = c2_playback_byname(((MsvPu *)m_pu)->m_handle, &pb, 0, stream_cb, this, status_cb, this);
    }
    else
    {
        c2_pb_time_s pb;
        memset(&pb, 0, sizeof(pb));

        if (1 == m_stream)
        {
            pb.obtainType = 3;
        }
        else
        {
            pb.obtainType = 2;
        }

        pb.start    = m_start;
        pb.end      = m_end;
        pb.addrcode = (char *)m_addrcode.c_str();

        m_record = c2_playback_bytime(((MsvPu *)m_pu)->m_handle, &pb, 0, stream_cb, this, status_cb, this);
    }

    if (!m_record)
    {
        ERRE_MSV("%s", c2_get_error());
        return -1;
    }

    recordGuMgr->addGu(this);

    return 0;
}

void
MsvRecordGu::stop()
{
    recordGuMgr->delGu(this);
    if (m_record)
    {
        c2_stop_playback(((MsvPu *)m_pu)->m_handle, m_record);
    }

    m_record = NULL;
}

int
MsvRecordGu::pause()
{
    return -1;
}

int
MsvRecordGu::continuex()
{
    return -1;
}

int
MsvRecordGu::startRepos()
{
    return -1;
}

int
MsvRecordGu::setPos(int pos)
{
    return -1;
}

int
MsvRecordGu::setTime(datetime_s &t)
{
    return -1;
}

void
MsvRecordGu::onTimer()
{

}
