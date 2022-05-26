#include "preview.h"
#include "gb28181_sipua.h"
#include "rtpsession.h"
#include "frame.h"
#include "c2fplay.h"
#include "gb28181_common.h"

#define  GB28181_VIDEO_SDP_FORMAT \
    "v=0\r\n" \
    "o=%s 0 0 IN IP4 %s\r\n" \
    "s=Play\r\n" \
    "c=IN IP4 %s\r\n" \
    "t=0 0\r\n" \
    "m=video %d RTP/AVP 96 97 98\r\n" \
    "a=rtpmap:96 PS/90000\r\n" \
    "a=rtpmap:97 MPEG4/90000\r\n" \
    "a=rtpmap:98 H264/90000\r\n" \
    "a=recvonly\r\n" \
    "y=%s\r\n\r\n"

preview::preview(GB28181_SipUA *pSipUa)
{
    m_width = 0;
    m_height = 0;
    m_iConId = -1;
    m_iDialId = -1;
    m_seq = 0;
    m_hWnd = NULL;
    m_pSipUa = pSipUa;
    m_rtpSession = new RTPSessionEx();
    //m_thread.setEntry();
    m_nPort = -1;

    m_strCamCode = "";
    m_host = "";
    m_port = 0;
    m_frameBuf = new char[1024 * 1024];
}

preview::~preview()
{
    stop();
    delete[] m_frameBuf;
}

bool preview::start(const char *cameracode, const char *host, int port, HWND hWnd)
{
    if (!m_pSipUa || !m_rtpSession)
    {
        LOGE_SIP("preview::start() failed, port:%d, addr:", port, cameracode);
        return false;
    }


    m_hWnd = hWnd;

#ifdef ANDROID
    m_playHandle = FPLY_openStream(m_hWnd, C2F_C2PSPLAYER, C2F_STREAM_REAL);
#else
    m_playHandle = FPLY_openStream(m_hWnd, C2F_HIKPLAYER, C2F_STREAM_REAL);//use STREAM_FILE instead of STREAM_REAL
    FPLY_openAudio(m_playHandle);//HikPlayer do not play sound default,so we need to open it for dualtalk and broadcast
#endif
    if (m_playHandle == NULL)
    {
        LOGE_SIP("preview::FPLY_openStream() failed");
        return false;
    }


    string ssrc = GenerateSSRC(m_pSipUa->addrcode());
    string resp;
    char sdp[1024] = { 0 };
    sprintf(sdp, GB28181_VIDEO_SDP_FORMAT, cameracode, m_pSipUa->ip().c_str(), m_pSipUa->ip().c_str(), m_rtpSession->srcport(),ssrc.c_str());
    int ret = m_pSipUa->invite(cameracode, host, port, sdp, resp);
    if (ret < 0)
    {
        LOGE_SIP("preview::invite() failed");
        return false;
    }
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(resp, rvalue, false))
    {
        LOGE_SIP("preview::response() failed");
        return  false;
    };
    m_iConId = atoi(rvalue["cid"].asString().c_str());
    m_iDialId = atoi(rvalue["did"].asString().c_str());
    int destport = rvalue["videoport"].asInt();
    //m_rtpSession->setSsrc(rvalue["ssrc"].asString());

    ret = m_rtpSession->addDestination(host, destport, this);
    if (ret < 0)
    {
        m_pSipUa->bye(m_iConId, m_iDialId);
        m_iConId  = -1;
        m_iDialId = -1;

        LOGE_SIP("preview::addDestination() failed, host:%s", host);
        return false;
    }

    m_rtpSession->setstreamcallback(stream_cb, this);

    m_strCamCode = cameracode;
    m_host = host;
    m_port = port;

    LOGD_SIP("preview::start() success");
    return true;
}

#ifdef WIN32
void CALLBACK DrawFunCB(long nPort, HDC hDc, LONG nUser)
{
    preview *lp = (preview *)nUser;
    if (lp)
    {
        lp->DrawRectCb(nPort, hDc);
    }
}
#endif


void preview::stream_cb(const uint8_t * stream, uint32_t size, uint8_t type, void *puser)
{
    preview * pthis = (preview*)puser;
    if (pthis)
    {
        memset(pthis->m_frameBuf, 0, 1024*1024);
        ut_frame_s* head = (ut_frame_s*)pthis->m_frameBuf;

        head->tag = TAG_STD;
        head->type = 'I';
        head->encode = ENCODE_H264;
        head->channel = 0;
        head->encrypt = 0;
        head->seq = 0;
        head->size = size;
        head->pts = sys->currentTimeMillis();
        head->time = sys->currentTimeMillis();

        head->video.width = 1920;
        head->video.height = 1080;
        head->video.fps = 25;

        if (2 == type)
        {
            memcpy(head->buf, stream, size);
            FPLY_inputStream(pthis->m_playHandle, head, size + 40);
        }
    }

    return;
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

    if (m_playHandle)
    {
        FPLY_closeStream(m_playHandle);
        m_playHandle = NULL;
    }
}

void preview::onHeartbeat()
{

}

void preview::onClose()
{

}

void preview::changeWinSize()
{
    FPLY_change_wndsize(m_playHandle,m_hWnd,m_width,m_height );
}

int             
preview::DragZoom(int startX, int startY, int endX, int endY, int winW, int winH)
{
    int midPointX = (startX + endX) / 2;
    int midPointY = (startY + endY) / 2;
    int lenX = abs(startX - endX);
    int lenY = abs(startY - endY);
    bool zoomIn = startX < endX ? true : false;
    return m_pSipUa->drag_zoom(m_strCamCode, m_host, m_port, m_strCamCode, zoomIn, winW, winH, midPointX, midPointY, lenX, lenY);
}

void            
preview::DrawRect(int x1, int y1, int x2, int y2, bool enable)
{
    AutoLock lock(m_muxRect);
    m_rectX1 = x1;
    m_rectY1 = y1;
    m_rectX2 = x2;
    m_rectY2 = y2;
    m_rectEnable = enable;
}

void        
preview::DrawRectCb(long nPort, HDC hDc)
{
#ifdef WIN32
    AutoLock lock(m_muxRect);
    if (m_rectEnable)
    {
        SetBkMode(hDc, TRANSPARENT);
        SetTextColor(hDc, RGB(200, 0, 0));

        RECT rctA = { m_rectX1, m_rectY1, m_rectX2, m_rectY2 };
        LPRECT rFrntFace = &rctA;
        ::DrawEdge(hDc, rFrntFace, EDGE_BUMP, BF_ADJUST | BF_BOTTOMLEFT | BF_TOPRIGHT);
    }
#endif
}
