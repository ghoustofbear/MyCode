#include "playback.h"
#include "gw_sipua.h"
#include "rtpsession.h"
#include "frame.h"
#include "h264parse.h"

Playback::Playback(SipUA *pSipUa)
{
	m_seq = 0;
	m_speed = 0;
	m_firstpts = 0;
	m_width = 0;
	m_height = 0;
	m_pSipUa = pSipUa;
	m_rtspClient = new RtspClient();
	m_rtpSession = new RTPSessionEx();
	m_recordHandle = PLAY_create(STREAM_FILE_NET, 1);
}

Playback::~Playback()
{
    stop();
}

bool Playback::start(const char *url, HWND hWnd)
{
    if (!m_rtspClient)
    {
        return false;
    }

#ifdef _WIN32
    if (PLAY_addwnd(m_recordHandle, hWnd, C2_RENDER_D3D) < 0)
#elif defined UNIX
    if (PLAY_addwnd(m_recordHandle, hWnd, C2_RENDER_SDL) < 0)
#elif defined ANDROID
    if (PLAY_addwnd(m_recordHandle, hWnd, C2_RENDER_OPENGL) < 0)
#endif
    {
        return false;
    }

    m_hWnd = hWnd;

    if (m_rtspClient->start(url, this) < 0)
    {
        return false;
    }
    if (m_rtspClient->doOption() < 0)
    {
        return false;
    }
    if (m_rtspClient->doDescribe() < 0)
    {
        return false;
    }
    int32_t s32BasePort = m_rtpSession->srcport();
    int32_t s32DestPort = 0;
    if (m_rtspClient->doSetup("RTP/AVP", "unicast", s32BasePort, s32DestPort) < 0)
    {
        return false;
    }	
    if (m_rtspClient->doPlay(NULL, "clock=0-") < 0)
    {
        return false;
    }

    char *requrl = (char *)strchr(url, '@');
    if (NULL == requrl)
    {
        return false;
    }
    char ipport[1024] = { 0 };
    strncpy(ipport, url, (requrl - url));
    char host[64] = { 0 };
    char temp[64] = { 0 };
    strcpy(temp, strstr(ipport, "://") + 3);

    int len = (strrchr(temp, ':') - temp);
    strncpy(host, temp, len);

    int ret = m_rtpSession->addDestination(host, s32DestPort, this);
    if (ret < 0)
    {
        m_rtspClient->doTeardown();
        return false;
    }

    PLAY_start(m_recordHandle, status_cb, this);
    PLAY_set_threshold(m_recordHandle, 0, 30, bufferwillemtpy, this);
    PLAY_set_threshold(m_recordHandle, 1, 1000, bufferwillfull, this);

    m_rtpSession->setstreamcallback(stream_cb, this);
    return true;
}

void Playback::bufferwillemtpy(int type, int fcnt, void* userdata)
{
    Playback *pthis = (Playback*)userdata;
    if (pthis)
    {
        pthis->m_rtspClient->doPlay(NULL, "clock=0-");
    }
}

void Playback::bufferwillfull(int type, int fcnt, void* userdata)
{
    Playback *pthis = (Playback*)userdata;
    if (pthis)
    {
        pthis->m_rtspClient->doPause();
    }
}

void Playback::status_cb(int code, void* user_data)
{

}

void Playback::stop()
{
    if (m_rtspClient)
    {
        m_rtspClient->doTeardown();
        m_rtspClient->stop();
        delete m_rtspClient;
        m_rtspClient = NULL;
    }

    if (NULL != m_rtpSession)
    {
        m_rtpSession->stop();
        delete m_rtpSession;
        m_rtpSession = NULL;
    }
    if (NULL != m_recordHandle)
    {
        PLAY_delwnd(m_recordHandle, m_hWnd);
        PLAY_stop(m_recordHandle);
        PLAY_destroy(m_recordHandle);
        m_recordHandle = NULL;
    }
}

void Playback::playCtrl(int cmd)
{
	if (m_recordHandle)
	{
		if (PLAYBACK_PAUSE == cmd)
		{
			PLAY_pause(m_recordHandle, true);
			m_rtspClient->doPause();
		}
		else if (PLAYBACK_CONTINUE == cmd)
		{
			PLAY_pause(m_recordHandle, false);
			m_rtspClient->doPlay(NULL, "clock=0-");
		}
		else if (PLAYBACK_FAST == cmd)
		{				
			PLAY_fast(m_recordHandle, &m_speed);

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
			
			m_rtspClient->doPlay(s8Scale, "clock=0-");
		}
		else if (PLAYBACK_SLOW == cmd)
		{
			char s8Scale[10] = { 0 };
			PLAY_slow(m_recordHandle, &m_speed);

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

			m_rtspClient->doPlay(s8Scale, "clock=0-");		
		}
	}
}

void Playback::stream_cb(const uint8_t * stream, uint32_t size, uint8_t type, void *puser)
{
    Playback * pthis = (Playback*)puser;
    if (pthis &&
        pthis->m_recordHandle)
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
            hdr->video.fps = 20;
            hdr->video.system = 'N';

            PLAY_inputdata(pthis->m_recordHandle, buf, (sizeof(ut_frame_s)+size), false);
        }
        else if (1 == type)
        {
            char buf[1024 * 100] = { 0 };
            ut_frame_s *hdr = (ut_frame_s *)buf;
            memcpy(hdr->buf, stream, size);

            hdr->tag = TAG_STD;

            hdr->type = 'A';
            hdr->encode = ENCODE_G711U;
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

            PLAY_inputdata(pthis->m_recordHandle, buf, (sizeof(ut_frame_s)+size), false);
        }
    }
}

void Playback::onHeartbeat()
{

}

void Playback::onClose()
{

}