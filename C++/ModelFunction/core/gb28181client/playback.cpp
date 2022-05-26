#include "playback.h"
#include "rtpsession.h"
#include "frame.h"
#include "c2fplay.h"
#include "stk/stk_message_rtsp.h"

#define  GB28181_PLAYBACK_VIDEO_SDP_FORMAT \
    "v=0\r\n" \
    "o=%s 0 0 IN IP4 %s\r\n" \
    "s=Playback\r\n" \
    "u=%s:%d\r\n" \
    "c=IN IP4 %s\r\n" \
    "t=%d %d\r\n" \
    "m=video %d RTP/AVP 96 97 98\r\n" \
    "a=rtpmap:96 PS/90000\r\n" \
    "a=rtpmap:97 MPEG4/90000\r\n" \
    "a=rtpmap:98 H264/90000\r\n" \
    "a=recvonly\r\n" \
    "y=0999999999\r\n\r\n"


#define GB28181_RTSP_CONTINUE \
    "PLAY RTSP/1.0\r\n" \
    "CSeq: %d\r\n" \
    "Range: npt=now-\r\n\r\n"

#define GB28181_RTSP_SCALE \
    "PLAY RTSP/1.0\r\n" \
    "CSeq: %d\r\n" \
    "Scale: %s\r\n\r\n"

#define GB28181_RTSP_SETTIME \
    "PLAY RTSP/1.0\r\n" \
    "CSeq: %d\r\n" \
    "Range: npt=%d-\r\n\r\n"

#define GB28181_RTSP_PAUSE \
    "PAUSE RTSP/1.0\r\n" \
    "CSeq: %d\r\n" \
    "PauseTime: now\r\n\r\n"
    




Playback::Playback(GB28181_SipUA *pSipUa)
{
    m_seq = 0;
    m_speed = 0;
    m_streamState = 0;
    m_playState = 0;
    m_pSipUa = pSipUa;
    m_cseq = 1;

    m_rtpSession = new RTPSessionEx();
    m_recordHandle = NULL;

    m_frameBuf = new char[1024 * 1024];
}

Playback::~Playback()
{
    stop();
    delete[] m_frameBuf;
}

bool Playback::start(const char *cameracode, const char *host, int port, uint32_t start, uint32_t end, HWND hWnd)
{
    if (!m_pSipUa || !m_rtpSession)
    {
        return false;
    }

    m_hWnd = hWnd;
#ifdef ANDROID
    m_recordHandle = FPLY_openStream(m_hWnd, C2F_C2PSPLAYER, C2F_STREAM_RECORD);
#else
    m_recordHandle = FPLY_openStream(m_hWnd, C2F_HIKPLAYER, C2F_STREAM_RECORD);
#endif
    if (m_recordHandle == NULL)
    {
        return false;
    }


    string resp;
    char sdp[1024] = { 0 };
    sprintf(sdp, GB28181_PLAYBACK_VIDEO_SDP_FORMAT, 
        cameracode, m_pSipUa->ip().c_str(), 
        "test", 1,
        m_pSipUa->ip().c_str(), 
        start, end,
        m_rtpSession->srcport());

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
        m_iConId = -1;
        m_iDialId = -1;
        return false;
    }

    m_rtpSession->setstreamcallback(stream_cb, this);

    m_strCamCode = cameracode;
    m_host = host;
    m_port = port;

    m_thread.setEntry(threadProc, this, 0);
    m_thread.start();
    

    return true;
}

void Playback::bufferwillemtpy(int type, int fcnt)
{
    streamCtrl(PLAYBACK_CONTINUE, 0);
}

void Playback::bufferwillfull(int type, int fcnt)
{
    streamCtrl(PLAYBACK_PAUSE, 0);
}

void Playback::status_cb(int code, void* user_data)
{

}

void Playback::stop()
{
    if ((-1 != m_iConId) && (-1 != m_iDialId))
    {
        m_pSipUa->bye(m_iConId, m_iDialId);
        m_iConId = -1;
        m_iDialId = -1;
    }

    if (NULL != m_rtpSession)
    {
        m_rtpSession->stop();
        delete m_rtpSession;
        m_rtpSession = NULL;
    }

    m_thread.stop();
    if (m_recordHandle)
    {
        FPLY_closeStream(m_recordHandle);
        m_recordHandle = NULL;
    }
}

void Playback::playCtrl(int cmd, int param)
{
    int ret = -1;
    string resp = "";
    char strrtsp[128] = { 0 };

    m_playState = cmd;

    if (m_recordHandle == NULL)
    {
        return;
    }

    switch (m_playState)
    {
        case PLAYBACK_PAUSE:
        {
            streamCtrl(PLAYBACK_PAUSE, 0);
            FPLY_pause(m_recordHandle, true);
        }
        break;

        case PLAYBACK_CONTINUE:
        {
            streamCtrl(PLAYBACK_CONTINUE, 0);
            FPLY_pause(m_recordHandle, false);
        }
        break;

        case PLAYBACK_FAST:
        {
            streamCtrl(PLAYBACK_FAST, 0);
            FPLY_fast(m_recordHandle, &m_speed);
        }
        break;

        case PLAYBACK_SLOW:
        {
            streamCtrl(PLAYBACK_SLOW, 0);
            FPLY_slow(m_recordHandle, &m_speed);
        }
        break;

        case PLAYBACK_SETTIME:
        {

        }
        break;
    default:
        break;
    }
    

}

void Playback::streamCtrl(int cmd, int param)
{
    int ret = -1;
    string resp = "";
    char strrtsp[128] = { 0 };


    if (cmd == m_streamState && (cmd == PLAYBACK_PAUSE || cmd == PLAYBACK_CONTINUE))
    {
        return;
    }

    if (PLAYBACK_PAUSE == cmd)
    {
        snprintf(strrtsp, sizeof(strrtsp), GB28181_RTSP_PAUSE, m_cseq++);
    }
    else if (PLAYBACK_CONTINUE == cmd)
    {
        snprintf(strrtsp, sizeof(strrtsp), GB28181_RTSP_CONTINUE, m_cseq++);
    }
    else if (PLAYBACK_FAST == cmd)
    {
        char s8Scale[10] = { 0 };
        if (m_speed > 0)
        {
            snprintf(s8Scale, 10, "%d", m_speed);
        }
        else if (0 == m_speed)
        {
            snprintf(s8Scale, 10, "%d", 1);
        }
        else if (-1 == m_speed)
        {
            strcpy(s8Scale, "0.5");
        }
        else if (-2 == m_speed)
        {
            strcpy(s8Scale, "0.25");
        }
        else if (-3 == m_speed)
        {
            strcpy(s8Scale, "0.125");
        }
        else if (-4 == m_speed)
        {
            strcpy(s8Scale, "0.0625");
        }

        snprintf(strrtsp, sizeof(strrtsp), GB28181_RTSP_SCALE, m_cseq++, s8Scale);
        
    }
    else if (PLAYBACK_SLOW == cmd)
    {
        char s8Scale[10] = { 0 };
        

        if (m_speed > 0)
        {
            snprintf(s8Scale, 10, "%d", m_speed);
        }
        else if (0 == m_speed)
        {
            snprintf(s8Scale, 10, "%d", 1);
        }
        else if (-1 == m_speed)
        {
            strcpy(s8Scale, "0.5");
        }
        else if (-2 == m_speed)
        {
            strcpy(s8Scale, "0.25");
        }
        else if (-3 == m_speed)
        {
            strcpy(s8Scale, "0.125");
        }
        else if (-4 == m_speed)
        {
            strcpy(s8Scale, "0.0625");
        }

        snprintf(strrtsp, sizeof(strrtsp), GB28181_RTSP_SCALE, m_cseq++, s8Scale);
        
    }
    else if (PLAYBACK_SETTIME == cmd)
    {

    }
    m_streamState = cmd;
    LOGD_COM("\n%s", strrtsp);
    ret = m_pSipUa->info(m_strCamCode, m_host, m_port, m_iDialId, strrtsp, resp);
    LOGD_COM("\n%s", resp.c_str());
}

void Playback::stream_cb(const uint8_t * stream, uint32_t size, uint8_t type, void *puser)
{
    Playback * pthis = (Playback*)puser;



    if (pthis)
    {
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
        

        if (2 == type)
        {
            memcpy(head->buf, stream, size);
            FPLY_inputStream(pthis->m_recordHandle, head, size);
        }
    }
    return;
}

void Playback::onHeartbeat()
{

}

void Playback::onClose()
{
    int a = 0;
    int b = 0;
}

void
Playback::timerdriver()
{

}

void *
Playback::threadProc(ThreadInfo *info)
{

    Playback *Ptr = (Playback *)(info->data);

    switch (info->id)
    {
    case 0:
        Ptr->timerdriver();
        break;

    default:
        break;
    }

    return NULL;
}







