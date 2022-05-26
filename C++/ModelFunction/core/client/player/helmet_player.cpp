#include "helmet_player.h"
#ifndef __ANDROID__
#include "c2sdk.h"
#include "tls_error.h"
#include "frame.h"

bool   HelmetPlayer::m_initPlayLib = false;
HelmetPlayer::HelmetPlayer(PlayerListener *listener)
    : Player(listener)
{
    m_playHandle = NULL;
    
    m_wnd     = 0;
    m_recfile = NULL;

    int ret = VARender_Startup(NULL);

}

HelmetPlayer::~HelmetPlayer()
{
    VARender_Cleanup();
    {
        AutoLock locker(m_recvQue._mtx);
        while (m_recvQue.size())
        {
#ifdef MEMPOOL
            mempool->free(m_recvQue.front());
#else
            free(m_recvQue.front());
#endif
            m_recvQue.pop();
        }
    }
}

int
HelmetPlayer::open(HWND wnd, int streamMode, int encode, int poolSize, void *head, int size)
{
    if (m_isOpen)
    {
        return 0;
    }

    int ret = VARender_Open(&m_playHandle);
    if (ret != 0)
    {
        goto  exit;
    }

    ret = VARender_SetWindow(m_playHandle, wnd);
    if (ret != 0)
    {
        goto  exit;
    }

    m_wnd = wnd;
    m_streamMode = streamMode;
    m_encode = encode;
    m_isOpen = true;
    m_speed = 0;

    VARender_StartVideo(m_playHandle);
    VARender_StartAudio(m_playHandle);

    if (streamMode == STREAM_REAL)
    {
        VARender_SetPlayMode(m_playHandle, VARENDER_PLAYMODE_REALTIME);
        closesound();
    }
    else if (streamMode == STREAM_FILE)
    {
        VARender_SetPlayMode(m_playHandle, VARENDER_PLAYMODE_REPLAY);
        VARender_SetSpeed(m_playHandle, 0);
    }


    
    m_inputThread.setEntry(threadProc, this, 0);
    m_inputThread.start();

    return 0;

exit:
    ERRE_SDK("hik player opened fail.");
    if (NULL != m_playHandle)
    {
        VARender_Close(m_playHandle);
        m_playHandle = NULL;
    }

    return -1;
}

int
HelmetPlayer::open(HWND wnd, string filename, int poolSize)
{
 
    //VARender_SetPlayMode(m_playHandle, VARENDER_PLAYMODE_REPLAY);
    return -1;
}

void
HelmetPlayer::close()
{
    stopREC();

    if (NULL != m_playHandle)
    {
        VARender_Close(m_playHandle);        
        m_playHandle = NULL;
        m_inputThread.stop();
    }

    m_listener = NULL;
    m_status = S_STOP;
}

int
HelmetPlayer::inputData(void *hdr, uint32_t size)
{
    if (NULL == m_playHandle)
    {
        ERRE_SDK("Invalidate player ID.");
        return -1;
    }
    
#ifdef MEMPOOL
    uint8_t *buf = (uint8_t *)mempool->alloc(size);
#else
    uint8_t *buf = (uint8_t *)malloc(size);
#endif
    if (buf)
    {
        memcpy(buf, hdr, size);
        {
            AutoLock locker(m_recvQue._mtx);
            m_recvQue.push(buf);
        }
    }


    


    //int ret = 0;
    //if (header->type == 'A')
    //{
    //    ret = VARender_PumpAudioFrame(m_playHandle, (char*)stream, size);
    //}
    //else
    //{
    //    ret = VARender_PumpVideoFrame(m_playHandle, (char*)stream, size);
    //}
    //
    //if (VARENDER_E_OK == ret)
    //{
    //    
    //}
    //else if (ret == VARENDER_E_WOULDBLOCK)
    //{
    //    ERRE_SDK("VARender_PumpVideoFrame buffer is full, please check, type:%d", header->type);
    //    //notifyFull(2);
    //}

    m_status = S_PLAY;

    void *stream = ((ut_frame_s *)hdr)->buf;
    size = ((ut_frame_s *)hdr)->size;
    REC(stream, size);

    return 0;
}

int
HelmetPlayer::control(int code, void *val, int valSize)
{
    if (NULL == m_playHandle)
    {
        ERRE_SDK("Invalidate player ID.");
        return -1;
    }

    int res = 1;
    switch (code)
    {
    case PLAY_PAUSE:
        //res = PlayM4_Pause(m_playID, true);
        m_status = S_PAUSE;
        break;

    case PLAY_RESTART:
        //res = PlayM4_Pause(m_playID, false);
        m_status = S_PLAY;
        break;

    case PLAY_FAST:
        m_speed++;
        if (m_speed > 4)
        {
            m_speed = 4;
        }

        VARender_SetSpeed(m_playHandle, m_speed);
        //res = PlayM4_Fast(m_playID);
        break;

    case PLAY_SLOW:
        m_speed--;
        if (m_speed < -4)
        {
            m_speed = -4;
        }
        VARender_SetSpeed(m_playHandle, m_speed);
        //res = PlayM4_Slow(m_playID);
        break;

    case PLAY_NORMAL:
    {
        /* note: PlayM4_SetPlayMode is no longer support in new hik sdk */
        /* res = PlayM4_SetPlayMode(m_playID, true); */
        int cnt = abs(m_speed);
        for (int i = 0; i < cnt; ++i)
        {
            if (m_speed > 0)
            {
                //res = PlayM4_Slow(m_playID);
                m_speed--;
            }
            else
            {
                //res = PlayM4_Fast(m_playID);
                m_speed++;
            }
        }
        m_status = S_PLAY;
        break;
    }

    case PLAY_STARTREPOS:
        m_repos = true;
        //res     = PlayM4_ResetSourceBuffer(m_playID);
        break;

    case PLAY_SETPOS:
    case PLAY_SETTIME:
        if ((LOCAL_FILE == m_streamMode) && val)
        {
            int pos = *((int *)val);
            //res = PlayM4_SetPlayPos(m_playID, pos / (float)100);
        }
        else
        {
            //res = PlayM4_ResetSourceBuffer(m_playID);
        }
        break;

    case PLAY_OPENAUDIO:
        res = opensound();
        break;

    case PLAY_CLOSEAUDIO:
        res = closesound();
        break;

    default:
        ERRE_SDK("Unknow control commond.");
        return -1;
    }

    if (!res)
    {
        ERRE_SDK("PlayM4_xxx call failed err: %d.", -1);
    }

    return (res == 1) ? 0 : -1;
}

int
HelmetPlayer::capture(string filepath, int type)
{
    VARender_Snapshot(m_playHandle, filepath.c_str());
    return 0;
}

int
HelmetPlayer::pos(datetime_s *start, datetime_s *end)
{
    if (m_repos)
    {
        ERRE_SDK("Waiting reposing.");
        return -1;
    }

    if (start && end)
    {
        tm tmStart = { 0 };
        tm tmEnd = { 0 };

        tmStart.tm_year = start->year - 1900;
        tmStart.tm_mon = start->month - 1;
        tmStart.tm_mday = start->day;
        tmStart.tm_hour = start->hour;
        tmStart.tm_min = start->minute;
        tmStart.tm_sec = start->second;

        time_t sstart = mktime(&tmStart);

        tmEnd.tm_year = end->year - 1900;
        tmEnd.tm_mon = end->month - 1;
        tmEnd.tm_mday = end->day;
        tmEnd.tm_hour = end->hour;
        tmEnd.tm_min = end->minute;
        tmEnd.tm_sec = end->second;
        time_t send = mktime(&tmEnd);

        datetime_s hikTime = { 0 };
        if (0 == time(hikTime))

        {
            tm tmHik = { 0 };
            tmHik.tm_year = hikTime.year - 1900;
            tmHik.tm_mon = hikTime.month - 1;
            tmHik.tm_mday = hikTime.day;
            tmHik.tm_hour = hikTime.hour;
            tmHik.tm_min = hikTime.minute;
            tmHik.tm_sec = hikTime.second;

            time_t current = mktime(&tmHik);

            /* m_pos = (int)((current - sstart) / float(send - sstart + 1) * 100 + 0.5); */
            m_pos = (int)((current - sstart) / float(send - sstart + 1) * 100);
        }

        return m_pos;
    }
    else
    {
        return 0;
    }
}

int
HelmetPlayer::time(datetime_s &time)
{

    if (NULL == m_playHandle)
    {
        ERRE_SDK("Invalidate player ID.");
        return -1;
    }

    uint32_t ct = 0;
    if (VARENDER_E_OK != VARender_GetLatestPlayVideoTimeStamp(m_playHandle, &ct))
    {
        ERRE_SDK("not playing");
        return -1;
    }

    time_t  ctt = ct * 1000;
    tm *p = localtime(&ctt);

    time.year   = p->tm_year + 1900;
    time.month  = p->tm_mon;
    time.day    = p->tm_mday;
    time.hour   = p->tm_hour;
    time.minute = p->tm_min;
    time.second = p->tm_sec;

    return 0;
}

int
HelmetPlayer::startREC(string filepath)
{
    filepath += ".hel";

    lock_guard<mutex> locker(m_wrmtx);
    m_recfile = fopen(filepath.c_str(), "wb");
    if (!m_recfile)
    {
        return -1;
    }

    return 0;
}

void
HelmetPlayer::stopREC()
{
    lock_guard<mutex> locker(m_wrmtx);
    if (!m_recfile)
    {
        return;
    }

    fclose(m_recfile);
    m_recfile = NULL;
}

int
HelmetPlayer::changeWndSize(int width, int height)
{
#ifdef UNIX
    if (m_playHandle)
        VARender_Refresh(m_playHandle);
#endif
    return 0;
}


void
HelmetPlayer::REC(void *frame, size_t size)
{
    lock_guard<mutex> locker(m_wrmtx);
    if (!m_recfile)
    {
        return;
    }

    fwrite(frame, size, 1, m_recfile);
}


int
HelmetPlayer::opensound()
{
    m_sound = 1;
    if (m_playHandle)
        VARender_StartAudio(m_playHandle);
    return -1;
}

int
HelmetPlayer::closesound()
{
    m_sound = 0;
    if (m_playHandle)
    {
        LOGE_SDK("HelmetPlayer::closesound()");
        VARender_StopAudio(m_playHandle);
    }
    return  0;
}

void
HelmetPlayer::enableDraw(bool enable)
{

}

void
HelmetPlayer::enableDecode(bool enable)
{


}

void
HelmetPlayer::timerdriver()
{
    do
    {
        if (NULL == m_playHandle)
        {
            break;
        }

        if (S_STOP == m_status)
        {
            break;
        }

        if (m_repos)
        {
            break;
        }

        if (STREAM_FILE != m_streamMode)
        {
            break;
        }

        break;

    }
    while (0);
}

void VARENDER_STDCALL vRenderVideoCallback(VARENDERUSER_HANDLE hUser, int nWidth, int nHeight, char *pBuf, int iBufLen, char cFormat)
{
    HelmetPlayer *player = (HelmetPlayer *)hUser;
    if (player)
    {
        int type = 0;

        player->notifyYUV(type, nWidth, nHeight, pBuf, iBufLen);

        player->m_picw = nWidth;
        player->m_pich = nHeight;
    }
}

void
HelmetPlayer::onInputdata()
{
    int ret = 0;

    while (m_inputThread.active())
    {
        
        if (m_recvQue.size() == 0)
        {
            sys->sleep(10);
            continue;
        }

        {
            AutoLock locker(m_recvQue._mtx);

            while (m_recvQue.size() != 0)
            {
                uint8_t *buf = m_recvQue.front();
                ut_frame_s *hdr = (ut_frame_s *)buf;

                if (hdr->type == 'A')
                {
                    if (m_sound == 1)
                    {
                        VARender_PumpAudioFrame(m_playHandle, hdr->buf, hdr->size);
                    }
                }
                else
                {
                    VARender_SetVideoCallBack(m_playHandle, vRenderVideoCallback, this, 2);
                    while (1)
                    {
                        ret = VARender_PumpVideoFrame(m_playHandle, hdr->buf, hdr->size);
                        if (VARENDER_E_OK == ret)
                        {
                            break;
                        }
                    }
                }

#ifdef MEMPOOL
                mempool->free(buf);
#else
                free(buf);
#endif
                m_recvQue.pop(); 
            }
        }
    }
}

void *
HelmetPlayer::threadProc(ThreadInfo *info)
{
    HelmetPlayer * player = (HelmetPlayer *)(info->data);

    switch (info->id)
    {
    case 0:
        player->onInputdata();
        break;



    default:
        break;
    }

    return NULL;
}




#endif

