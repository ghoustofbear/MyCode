#include "playback.h"
#include "socket.h"
#include "tls_error.h"
#include "cu.h"
#include "frame.h"
#include "c2_player.h"
#include "hik_player.h"
#include "dahua_player.h"
#include "helmet_player.h"
#include "hik_ps_player.h"

#ifdef _WIN32
#include "tindy_player.h"
#endif


Playback::Playback(string filename, Cu *cu)
    : RecordStream(cu)
{
    m_byname     = true;
    m_filename   = filename;
    m_wnd        = 0;
    m_streamCB   = NULL;
    m_streamData = NULL;
    m_statusCB   = NULL;
    m_statusData = NULL;
    m_player     = NULL;
    m_pos        = 0;
    m_speed      = 0;
    m_decodeCB = NULL;
    m_decodeData = NULL;

}

Playback::Playback(datetime_s &start, datetime_s &end, Cu *cu)
    : RecordStream(cu)
{
    m_byname     = false;
    m_start      = start;
    m_end        = end;
    m_wnd        = 0;
    m_streamCB   = NULL;
    m_streamData = NULL;
    m_statusCB   = NULL;
    m_statusData = NULL;
    m_player     = NULL;
    m_pos        = 0;
    m_speed      = 0;
    m_decodeCB   = NULL;
    m_decodeData = NULL;
}

Playback::~Playback()
{
    if (m_player)
    {
        delete m_player;
        m_player = NULL;
    }
}

void
Playback::init(HWND wnd, c2_stream_cb streamCB, void *streamData, c2_status_cb statusCB, void *statusData)
{
    m_wnd        = wnd;
    m_streamCB   = streamCB;
    m_streamData = streamData;
    m_statusCB   = statusCB;
    m_statusData = statusData;
}

void
Playback::stop()
{
    RecordStream::stop();

    if (m_player)
    {
        m_player->close();
    }
}

void
Playback::onStream(void *stream, uint32_t size)
{
    ut_frame_s *hdr = (ut_frame_s *)stream;
    do
    {
        if (!m_wnd && !m_decodeCB)
        {
            break;
        }

        if (!m_player)
        {
#ifdef __ANDROID__
            if (TAG_STD == hdr->tag)
            {
                 m_player = new C2Player(this);                
            }    
#else
            if (TAG_STD == hdr->tag)
            {
                 m_player = new HikPsPlayer(this);
            }
#endif
#ifndef __ANDROID__
            else if (TAG_HIK == hdr->tag || TAG_STD_PS == hdr->tag)
            {
                m_player = new HikPlayer(this, true);
            }
            else if (TAG_HIK_N == hdr->tag)
            {
                m_player = new HikPlayer(this, false);
            }
            else if (TAG_DAHUA == hdr->tag)
            {
                m_player = new Dahualayer(this, false);
            }
#ifdef _WIN32
            else if (TAG_HELMET_GX == hdr->tag)
            {
                m_player = new HelmetPlayer(this);
            }
            else if (TAG_TIANDY == hdr->tag)
            {
                m_player = new TindyPlayer(this, false);
                if (m_player)
                {
                    ((TindyPlayer*)m_player)->m_bPlayBack = true;
                }
            }
#endif
#endif
            if (!m_player)
            {
                STATUS(CODE_PLAY_FAIL);
                break;
            }

            if (m_player->open(m_wnd, Player::STREAM_FILE, hdr->encode, MAX_PBPOOL_SIZE, hdr->buf, hdr->size) < 0)
            {
                delete m_player;
                m_player = NULL;

                STATUS(CODE_PLAY_FAIL);

                break;
            }
            STATUS(CODE_PLAY_SUCCESS);
        }

        

        if (m_player)
        {
            int result = -1;
            if (TAG_STD == hdr->tag)
            {
                result = m_player->inputData(stream, size);
            }
            else
            {
                result = m_player->inputData(stream, size);
            }

            if (result < 0)
            {
                m_player->close();
                delete m_player;
                m_player = NULL;
                //STATUS(CODE_PLAY_FAIL);
            }
        }
    }
    while (0);


    if (m_streamCB)
    {
        m_streamCB(m_wnd, stream, size, m_streamData);
    }
}

long
Playback::pos(datetime_s *start, datetime_s *end)
{
    //if (100 == m_pos)
    //{
    //    return 100;
    //}

    if (m_player)
    {
        int pos = 0;
        if (m_byname)
        {
            pos = m_player->pos(start, end);
        }
        else
        {
            pos = m_player->pos(&m_start, &m_end);
        }

        if (pos < 0)
        {
            return -1;
        }

        m_pos = pos;
    }

    return m_pos;
}

long
Playback::getPlayTime(datetime_s &time)
{
    if (m_player)
    {
        return m_player->time(time);
    }

    return -1;
}

void
Playback::onStatus(int code)
{
    STATUS(code);
}

long
Playback::control(int code, void *val, int valSize)
{
    int result = 0;
    result += streamCtrl(code, val, valSize);
    if (NULL != m_player)
    {
        result += m_player->control(code, val, valSize);
    }

    //if (PLAY_SETPOS == code || PLAY_SETTIME == code)
    //{
    //    if (!m_reposem.wait(5000))
    //    {
    //        ERRE_SDK("repose time out.");
    //        result = -1;
    //    }

    //    if (NULL != m_player)
    //    {
    //        m_player->reposEnd(true);
    //    }
    //}

    return result;
}

long
Playback::capture(string filepath, int type)
{
    if (m_player)
    {
        return m_player->capture(filepath, type);
    }

    ERRE_SDK("not playing.");
    return -1;
}


int
Playback::changeWndSize(int width, int height)
{
    if (m_player)
    {
        return m_player->changeWndSize(width, height);
    }

    ERRE_SDK("not playing.");

    return -1;
}

int Playback::changeWndPos(int x, int y)
{
    if (m_player)
    {
        return m_player->changeWndPos(x, y);
    }

    ERRE_SDK("not playing.");

    return -1;
}

string
Playback::getSpeedStr(int speed)
{
    string str;
    switch (speed)
    {
    case 0:
        str = "1";
        break;
    case 1:
        str = "2";
        break;
    case 2:
        str = "4";
        break;
    case 3:
        str = "8";
        break;
    case 4:
        str = "16";
        break;
    case -1:
        str = "0.5";
        break;
    case -2:
        str = "0.25";
        break;
    case -3:
        str = "0.125";
        break;
    case -4:
        str = "0.0625";
        break;
    default:
        str = "1";
        break;
    }
    return str;
}

void
Playback::onEmpty(uint32_t size)
{
    //LOGI_SDK("%s, size %d", getError(), size);
    //if (100 == m_streamPos)
    //{
    //    return;
    //}
    continuex();
}

void
Playback::onFull(uint32_t size)
{
    //LOGI_SDK("%s, size %d", getError(), size);
    pause();
}

void
Playback::onEnd()
{
    if (m_pos >= 99 || 100 == m_streamPos)
    {
        m_pos = 100;
        LOGI_SDK("Play back end %s, pos %d", getError(), m_pos);
        onStatus(CODE_PLAY_END);
    }
    else
    {
        continuex();
    }
    
}

void
Playback::onError()
{
    onStatus(10);
}

void
Playback::onYUV(int type, int w, int h, void *yuv, int size)
{
    if (m_decodeCB)
    {
        m_decodeCB(type, w, h, yuv, size, m_decodeData);
    }
}

void
Playback::onHDC(HDC hdc, HWND wnd, int w, int h)
{

}

int 
Playback::setDecCallback(c2_decode_cb decodeCB, void *userData)
{
    if (m_player)
    {
        m_player->enableDecode();
    }

    m_decodeCB = decodeCB;
    m_decodeData = userData;

    return 0;
}

void
Playback::onReposEnd()
{
    //m_reposem.post();
}

int
Playback::addWnd(HWND wnd)
{
    if (m_player)
    {
        return m_player->addWnd(wnd);
    }
    return -1;
}

int
Playback::delWnd(HWND wnd)
{
    if (m_player)
    {
        return m_player->delWnd(wnd);
    }
    return -1;
}

int
Playback::opensound()
{
    if (m_player)
    {
        return m_player->opensound();
    }
    return -1;
}

int
Playback::closesound()
{
    if (m_player)
    {
        return m_player->closesound();
    }
    return -1;
}

int
Playback::setVolume(int vol)
{
    if (m_player)
    {
        return m_player->setVolume(vol);
    }
    return -1;

}

void
Playback::timerdriver()
{
    if (m_player)
    {
        m_player->timerdriver();
    }

    /* send heartbeat every 5 seconds */
    /* if (0 == (m_tmcnt * 50 % 5000)) */
    /* { */
    /*     heartbeat(); */
    /* } */

    /* RecordStream::timerdriver(); */
}
