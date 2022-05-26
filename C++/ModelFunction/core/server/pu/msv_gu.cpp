#include "msv_gu.h"
#include "msv_pu.h"
#include "tls_error.h"
#include "session.h"
#include "frame.h"
#include "gumgr.h"

void
MsvRealGu::status_cb(int code, void* userData)
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
MsvRealGu::stream_cb(HWND wnd, void *data, uint32_t size, void *userData)
{
    if (!data || size <= 0 || !userData)
    {
        return true;
    }

    MsvRealGu * gu = (MsvRealGu *)userData;

    ut_frame_s *hdr = (ut_frame_s *)data;
    if (TAG_STD == hdr->tag)
    {
        switch (hdr->type)
        {
        case 'I':
        case 'P':
            gu->notifyVideoStream(hdr->buf, hdr->size, hdr->encode, hdr->type, hdr->video.width, hdr->video.height, hdr->video.fps, hdr->pts);
            break;
        case 'A':
            gu->notifyAudioStream(hdr->buf, hdr->size, hdr->encode, hdr->audio.sps, hdr->audio.channels, hdr->audio.bps, hdr->pts);
            break;
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

MsvRealGu::MsvRealGu(Pu *pu, string addrcode, int streamType, int stream, int streamProtoType)
    : RealGu(pu, addrcode, streamType, streamProtoType)
{
    m_view   = NULL;
}

MsvRealGu::MsvRealGu(Pu * pu, int channelNo, string subHost, int subPort, int streamType, int streamProtoType)
    : RealGu(pu, channelNo, subHost, subPort, streamType, streamProtoType)
{
    m_view = NULL;
    //m_streamType = streamType;
    //m_streamProto = streamProtoType;
}


MsvRealGu::~MsvRealGu()
{
}

int
MsvRealGu::start(int offset)
{
    if (!m_pu->online())
    {
        ERRE_MSV("device ip: %s is not on-line.", m_pu->ip().c_str());

        return -1;
    }

    c2_preview_s viewParam;
    memset(&viewParam, 0, sizeof(c2_preview_s));

    viewParam.channelNo = m_channelNo;
    viewParam.deviceHost = (char*)m_subHost.c_str();
    viewParam.devicePort = m_subPort;
    viewParam.streamType = m_streamType;
    viewParam.obtainType = m_streamProtoType;





    m_view = c2_start_preview(((MsvPu *)m_pu)->m_handle, &viewParam, 0, stream_cb, this, status_cb, this);
    if (!m_view)
    {
        ERRE_MSV("%s", c2_get_error());
        return -1;
    }

    realGuMgr->addGu(this);

    return 0;
}

void
MsvRealGu::stop()
{
    realGuMgr->delGu(this);

    if (NULL != m_view)
    {
        c2_stop_preview(((MsvPu *)m_pu)->m_handle, m_view);
    }

    m_view = NULL;
}

void
MsvRealGu::onTimer()
{
    /* comment */
    /* notifyStatus(StreamListener::S_GU_OFFLINE); */

    Gu::onTimer();
}
