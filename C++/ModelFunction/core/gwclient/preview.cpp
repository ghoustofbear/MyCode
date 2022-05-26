#include "preview.h"
#include "gw_sipua.h"
#include "rtpsession.h"
#include "frame.h"
#include "h264parse.h"

#define  GW_VIDEO_SDP_FORMAT \
    "v=0\r\n" \
    "o=- 0 0 IN IP4 %s\r\n" \
    "s=-\r\n" \
    "i=The SIP CALL\r\n" \
    "c=IN IP4 %s\r\n" \
    "t=0 0\r\n" \
    "m=video %d RTP/AVP 100\r\n" \
    "a=rtpmap:100 H264/90000\r\n" \
    "a=fmtp:100 CIF=1;4CIF=1;F=1;K=1\r\n" \
    "a=sendrecv\r\n\r\n"

preview::preview(SipUA *pSipUa)
{
    m_width = 0;
    m_height = 0;
    m_iConId = -1;
    m_iDialId = -1;
    m_seq = 0;
    m_hWnd = NULL;
    m_pSipUa = pSipUa;
    m_rtpSession = new RTPSessionEx();
    m_realHandle = PLAY_create(STREAM_REAL, 0);
    //m_thread.setEntry();
}

preview::~preview()
{
    stop();
}

bool preview::start(const char *cameracode, const char *host, int port, HWND hWnd)
{
    if (!m_pSipUa || !m_rtpSession)
    {
        return false;
    }

#ifdef _WIN32
    if (PLAY_addwnd(m_realHandle, hWnd, C2_RENDER_D3D) < 0)
#elif defined UNIX
    if (PLAY_addwnd(m_realHandle, hWnd, C2_RENDER_SDL) < 0)
#elif defined ANDROID
    if (PLAY_addwnd(m_realHandle, hWnd, C2_RENDER_OPENGL) < 0)
#endif
    {
        return false;
    }

    m_hWnd = hWnd;

    string resp;
    char sdp[1024] = { 0 };
    sprintf(sdp, GW_VIDEO_SDP_FORMAT, m_pSipUa->ip().c_str(), m_pSipUa->ip().c_str(), m_rtpSession->srcport());
    int ret = m_pSipUa->invite(cameracode, host, port, sdp, resp);
    if (ret < 0)
    {
        return false;
    }
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(resp, rvalue, false))
    {
        return  false;
    }
    m_iConId = atoi(rvalue["cid"].asString().c_str());
    m_iDialId = atoi(rvalue["did"].asString().c_str());
    int destport = rvalue["videoport"].asInt();

    ret = m_rtpSession->addDestination(host, destport, this);
    if (ret < 0)
    {
        m_pSipUa->bye(m_iConId, m_iDialId);
        m_iConId  = -1;
        m_iDialId = -1;
        return false;
    }

    PLAY_start(m_realHandle, NULL, NULL);
    m_rtpSession->setstreamcallback(stream_cb, this);

    m_strCamCode = cameracode;
    return true;
}

void preview::stream_cb(const uint8_t * stream, uint32_t size, uint8_t type, void *puser)
{
    preview * pthis = (preview*)puser;
    if (pthis && 
        pthis->m_realHandle)
    {
        if (0 == type)
        {
            char buf[800 * 1024] = { 0 };
            ut_frame_s *hdr = (ut_frame_s *)buf;
            memcpy(hdr->buf, stream, size);

            long pts = sys->currentTimeMillis();
            hdr->tag = TAG_STD;

            hdr->type = 'I';
            hdr->encode = ENCODE_H264;
            hdr->channel = 0;
            hdr->encrypt = 0;

            hdr->size = size;
            hdr->pts = pts;
            hdr->time = pts;
            hdr->seq = pthis->m_seq++;

            int ret = parseh264resolution((unsigned char*)(stream + 4), size - 4, pthis->m_height, pthis->m_width);;
            if (0 == pthis->m_width ||
                0 == pthis->m_height)
            {
                return;
            }				

            hdr->video.width = pthis->m_width;
            hdr->video.height = pthis->m_height;
            hdr->video.fps = 25;
            hdr->video.system = 'N';

            PLAY_inputdata(pthis->m_realHandle, buf, (sizeof(ut_frame_s) + size), false);
        }
        else if (1 == type)
        {
            char buf[1024 * 100] = { 0 };
            ut_frame_s *hdr = (ut_frame_s *)buf;
            memcpy(hdr->buf, stream, size);

            hdr->tag = TAG_STD;

            hdr->type = 'A';
            hdr->encode = ENCODE_G711A;
            hdr->channel = 0;
            hdr->encrypt = 0;

            long pts = sys->currentTimeMillis();
            hdr->size = size;
            hdr->pts = pts;
            hdr->time = pts;
            hdr->seq = pthis->m_seq++;

            hdr->audio.sps = 8000;
            hdr->audio.channels = 1;
            hdr->audio.bps = 16;
            hdr->audio.packLen = size;

            PLAY_inputdata(pthis->m_realHandle, buf, (sizeof(ut_frame_s) + size), false);
        }
    }
}

void preview::stop()
{
    if ((-1 != m_iConId) && (-1 != m_iDialId))
    {
        m_pSipUa->bye(m_iConId, m_iDialId);
        m_iConId  = -1;
        m_iDialId = -1;
    }
    
    if (NULL != m_rtpSession)
    {
        m_rtpSession->stop();
        delete m_rtpSession;
        m_rtpSession = NULL;
    }
    if (NULL != m_realHandle)
    {
        PLAY_delwnd(m_realHandle, m_hWnd);
        PLAY_stop(m_realHandle);
        PLAY_destroy(m_realHandle);
        m_realHandle = NULL;
    }
}

void preview::onHeartbeat()
{

}

void preview::onClose()
{

}