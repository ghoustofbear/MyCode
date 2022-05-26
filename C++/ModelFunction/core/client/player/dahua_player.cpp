#include "dahua_player.h"
#include "c2sdk.h"
#include "tls_error.h"
#include "frame.h"


#if defined(__linux__)
void
Dahualayer::decdatacb(int port, char *data, int size, FRAME_INFO_DH *info, void *reserved1, int reserved2)
#elif defined(_WIN32)
void CBK
Dahualayer::decdatacb(long port, char *data, long size, FRAME_INFO_DH *info, void *reserved1, long reserved2)
#endif
{
    Dahualayer *player = (Dahualayer *)reserved1;
    if (player == NULL)
    {
        return;
    }

    int  type = 0;
    if (T_IYUV == info->nType)
    {
        type = 0;
    }
    else if (T_RGB32 == info->nType)
    {
        type = 2;
    }
    else
    {
        return;
    }

    player->notifyYUV(type, info->nWidth, info->nHeight, data, size);
    player->m_picw = info->nWidth;
    player->m_pich = info->nHeight;
}

#if defined(__linux__)
void
Dahualayer::drawdatacb(int handle, void* hdc, void *userData)
#elif defined(_WIN32)
void CBK
Dahualayer::drawdatacb(long handle, HDC hdc, void* userData)
#endif
{
    Dahualayer *player = (Dahualayer *)userData;
    if (player == NULL)
    {
        return;
    }

    player->notifyHDC((HDC)hdc, (HWND)handle, player->m_picw, player->m_pich);
}

Dahualayer::Dahualayer(PlayerListener *listener, bool isOld)
    : Player(listener)
{
    m_playID  = -1;
    m_isOld   = isOld;
    m_wnd     = 0;
    m_recfile = NULL;
}

Dahualayer::~Dahualayer()
{

}

int
Dahualayer::open(HWND wnd, int streamMode, int encode, int poolSize, void *head, int size)
{
    m_wnd        = wnd;
    m_streamMode = streamMode;
    m_encode     = encode;

    if (m_isOpen)
    {
        return 0;
    }

#ifdef _WIN32
    if (!PLAY_GetFreePort((long *)&m_playID))
#else
    if (!PLAY_GetFreePort((int*)&m_playID))
#endif
    {
        goto exit;
    }

    if (STREAM_FILE == streamMode)
    {
        if (!PLAY_SetStreamOpenMode(m_playID, STREAME_FILE))
        {
            goto exit;
        }
    }
    else if (STREAM_REAL == streamMode)
    {
        if (!PLAY_SetStreamOpenMode(m_playID, STREAME_REALTIME))
        {
            goto exit;
        }
    }

    // PLAY_SetStreamOpenMode(m_playID, STREAME_REALTIME);


  
    if (!PLAY_OpenStream(m_playID, NULL, 0, poolSize))//1024 * 900))// poolSize))
    {
        goto exit;
    }



    //if (!PLAY_SetDecCBStream(m_playID, 3))
    //{
    //    goto exit;
    //}


    if (!PLAY_Play(m_playID, m_wnd))
    {
        goto exit;
    }

    LOGI_PLY("Dahualayer::open() PlayID:%d", m_playID);

    m_speed = 0;
    m_status = S_PLAY;
    return 0;

exit:
    ERRE_SDK("dahua player opened fail.");
    if (-1 != m_playID)
    {
        PLAY_ReleasePort(m_playID);
        m_playID = -1;
    }

    return -1;
}

int
Dahualayer::open(HWND wnd, string filename, int poolSize)
{
    m_wnd = wnd;

    if (m_isOpen)
    {
        return 0;
    }

#ifdef _WIN32
    if (!PLAY_GetFreePort((long *)&m_playID))
#else
    if (!PLAY_GetFreePort((int*)&m_playID))
#endif
    {
        goto exit;
    }

    if (!PLAY_OpenFile(m_playID, (char *)filename.c_str()))
    {
        goto exit;
    }

    if (!PLAY_Play(m_playID, m_wnd))
    {
        goto exit;
    }

    m_speed = 0;
    LOGI_PLY("Dahualayer::open() PlayID:%d", m_playID);
    //opensound();
    m_status = S_PLAY;
    return 0;

exit:
    ERRE_SDK("dahua player opened fail.");

    return -1;
}

void
Dahualayer::close()
{
    LOGI_PLY("Dahualayer::close() PlayID:%d", m_playID);
    stopREC();

    m_status = S_STOP;  

    if (-1 != m_playID)
    {
        PLAY_Stop(m_playID);
        PLAY_CloseStream(m_playID);
        PLAY_ReleasePort(m_playID);
        PLAY_ResetBuffer(m_playID, BUF_VIDEO_SRC_DH);
        PLAY_ResetBuffer(m_playID, BUF_VIDEO_RENDER_DH);

        m_playID = -1;
    }

    m_listener = NULL;    

}

int
Dahualayer::inputData(void *hdr, uint32_t size)
{
    if (-1 == m_playID)
    {
        ERRE_SDK("Invalidate player ID.");
        return -1;
    }

    if (m_repos)
    {
        //ERRE_SDK("Waiting reposing.");
        //notifyError();

        return 0;
    }

    void *stream = ((ut_frame_s *)hdr)->buf;
    size = ((ut_frame_s *)hdr)->size;
    

    if (FALSE == PLAY_InputData(m_playID, (uint8_t *)stream, size))
    {
        DWORD errorCode = PLAY_GetLastError(m_playID);


        if (DH_PLAY_BUF_OVER == errorCode)
        {
            LOGD_SDK("Play buffer is full.");
            notifyFull(0);

            return 0;
        }

        ERRE_SDK("PlayM4_InputData err: %d", errorCode);
        //notifyError();

        return -1;
    }
    
    //m_status = S_PLAY;

    //uint32_t bufSize = PLAY_GetSourceBufferRemain(m_playID);
    //if (MAX_PBPOOL_MARK <= bufSize)
    //{
    //    notifyFull(bufSize);
    //}
    REC(stream, size);
    return 0;
}

int
Dahualayer::control(int code, void *val, int valSize)
{
    if (-1 == m_playID)
    {
        ERRE_SDK("Invalidate player ID.");
        return -1;
    }

    int res = 1;
    switch (code)
    {
    case PLAY_PAUSE:
        res = PLAY_Pause(m_playID, true);
        m_status = S_PAUSE;
        break;

    case PLAY_RESTART:
        res = PLAY_Pause(m_playID, false);
        m_status = S_PLAY;
        break;

    case PLAY_FAST:
        m_speed++;
        if (m_speed > 4)
        {
            m_speed = 4;
        }
        res = PLAY_Fast(m_playID);
        break;

    case PLAY_SLOW:
        m_speed--;
        if (m_speed < -4)
        {
            m_speed = -4;
        }
        res = PLAY_Slow(m_playID);
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
                res = PLAY_Slow(m_playID);
                m_speed--;
            }
            else
            {
                res = PLAY_Fast(m_playID);
                m_speed++;
            }
        }
        m_status = S_PLAY;
        break;
    }

    case PLAY_STARTREPOS:
        m_repos = true;
        res = PLAY_ResetSourceBuffer(m_playID);
        break;

    case PLAY_SETPOS:
    case PLAY_SETTIME:
        if ((LOCAL_FILE == m_streamMode) && val)
        {
            int pos = *((int *)val);
            res = PLAY_SetPlayPos(m_playID, pos / (float)100);
        }
        else
        {
            res = PLAY_ResetSourceBuffer(m_playID);
        }
        m_repos = false;
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
        ERRE_SDK("PlayM4_xxx call failed err: %d.", PLAY_GetLastError(m_playID));
    }

    return (res == 1) ? 0 : -1;
}

int
Dahualayer::capture(string filepath, int type)
{
    if (-1 == m_playID)
    {
        ERRE_SDK("Invalidate player ID.");
        return -1;
    }

    FILE *pic = NULL;
    pic = fopen(filepath.c_str(), "wb");
    if (!pic)
    {
        ERRE_SDK("open file: %s fail", filepath.c_str());
        return  -1;
    }

    /* sizeof(BITMAPFILEHEADER) + sizeof(BITMAPINFOHEADER) + size of 1010p bitmap */
    DWORD bufSize = 54 + 1920 * 1080 * 4;
    DWORD picSize = 0;

    uint8_t *buffer  = new uint8_t[bufSize];
    if (!buffer)
    {
        ERRE_SDK("new uint8_t[%d] err", bufSize);
        fclose(pic);
        return -1;
    }

    if (0 == type)
    {
        if (!PLAY_GetPicBMP(m_playID, buffer, bufSize, &picSize))
        {
            ERRE_SDK("PlayM4_GetBMP err: %d", PLAY_GetLastError(m_playID));
        }

        fwrite(buffer, picSize, 1, pic);
    }
    else if (1 == type)
    {
        if (!PLAY_GetPicJPEG(m_playID, buffer, bufSize, &picSize, 100))
        {
            ERRE_SDK("PlayM4_GetJPEG err: %d", PLAY_GetLastError(m_playID));
        }

        fwrite(buffer, picSize, 1, pic);
    }

    fclose(pic);
    delete []buffer;

    return 0;
}

int
Dahualayer::pos(datetime_s *start, datetime_s *end)
{
    if (m_repos)
    {
        //ERRE_SDK("Waiting reposing.");
        return -1;
    }

    if (-1 == m_playID)
    {
        //ERRE_SDK("Invalidate player ID.");
        return -1;
    }

    //uint32_t bufSize   = PLAY_GetSourceBufferRemain(m_playID);
    //uint32_t videoSize = PLAY_GetBufferValue(m_playID, BUF_VIDEO_RENDER_DH);

    //if (0 == bufSize && 0 == videoSize)
    //{
    //    ERRE_SDK("Play buffer is empty.");
    //    return -1;
    //}

    if (start && end)
    {
        tm tmStart = {0};
        tm tmEnd   = {0};

        tmStart.tm_year = start->year  - 1900;
        tmStart.tm_mon  = start->month - 1;
        tmStart.tm_mday = start->day;
        tmStart.tm_hour = start->hour;
        tmStart.tm_min  = start->minute;
        tmStart.tm_sec  = start->second;

        time_t sstart   = mktime(&tmStart);

        tmEnd.tm_year = end->year  - 1900;
        tmEnd.tm_mon  = end->month - 1;
        tmEnd.tm_mday = end->day;
        tmEnd.tm_hour = end->hour  ;
        tmEnd.tm_min  = end->minute;
        tmEnd.tm_sec  = end->second;
        time_t send   = mktime(&tmEnd);

        datetime_s DHTime = {0};
      
        if (0 == time(DHTime))

        {
            tm tmDH = { 0 };
            tmDH.tm_year = DHTime.year - 1900;
            tmDH.tm_mon  = DHTime.month - 1;
            tmDH.tm_mday = DHTime.day;
            tmDH.tm_hour = DHTime.hour;
            tmDH.tm_min  = DHTime.minute;
            tmDH.tm_sec  = DHTime.second;

            time_t current = mktime(&tmDH);

            /* m_pos = (int)((current - sstart) / float(send - sstart + 1) * 100 + 0.5); */
            m_pos = (int)((current - sstart) / float(send - sstart + 1) * 100);
        }

        //LOGI_SDK("play pos: %d", m_pos);

        return m_pos;
    }
    else
    {
        float pos = PLAY_GetPlayPos(m_playID);
        return (int)(pos * 100);
    }

    return m_pos;
}

int
Dahualayer::time(datetime_s &time)
{
    if (-1 == m_playID)
    {
        ERRE_SDK("Invalidate player ID.");
        return -1;
    }

    int len = 0;
    PLAY_QueryInfo(m_playID, PLAY_CMD_GetTime, (char*)&time, sizeof(time), &len);

    return 0;
}

int
Dahualayer::startREC(string filepath)
{
    filepath += ".dav";

    lock_guard<mutex> locker(m_wrmtx);
    m_recfile = fopen(filepath.c_str(), "wb");
    if (!m_recfile)
    {
        return -1;
    }

    return 0;
}

void       
Dahualayer::stopREC()
{
    lock_guard<mutex> locker(m_wrmtx);
    if (!m_recfile)
    {
        return;
    }

    fclose(m_recfile);
    m_recfile = NULL;
}

void
Dahualayer::REC(void *frame, size_t size)
{
    lock_guard<mutex> locker(m_wrmtx);
    if (!m_recfile)
    {
        return;
    }

    fwrite(frame, size, 1, m_recfile);
}


int
Dahualayer::opensound()
{
#if 1
    if (-1 == m_playID)
    {
        ERRE_SDK("Invalidate player ID.");
        return -1;
    }

    PLAY_SetVolume(m_playID, 0xFFFF);
    return PLAY_PlaySound(m_playID) ? 0 : -1;
#endif

    return -1;
}

int
Dahualayer::closesound()
{
#if 1
    if (-1 == m_playID)
    {
        ERRE_SDK("Invalidate player ID.");
        return -1;
    }

    return PLAY_StopSound() ? 0 : -1;
#endif

    return -1;
}

void
Dahualayer::enableDraw(bool enable)
{
#if 1
    if (enable)
    {
#ifdef _WIN32
        PLAY_RigisterDrawFun(m_playID, drawdatacb, this);
#else
        PLAY_RigisterDrawFun(m_playID, drawdatacb, this);
#endif
    }
    else
    {
#ifdef _WIN32
        PLAY_RigisterDrawFun(m_playID, NULL, 0);
#else
        PLAY_RigisterDrawFun(m_playID, NULL, NULL);
#endif
    }
#endif
}

void
Dahualayer::enableDecode(bool enable)
{
#if 1
    if (enable)
    {

#ifdef _WIN32
        PLAY_SetDecCallBackEx(m_playID, decdatacb, this);
#else
        PLAY_SetDecCallBackEx(m_playID, decdatacb, this);
#endif
    }
    else
    {
#ifdef _WIN32
        PLAY_SetDecCallBackEx(m_playID, NULL, NULL);
#else
        PLAY_SetDecCallBackEx(m_playID, NULL, NULL);
#endif
    }
#endif
}


void
Dahualayer::timerdriver()
{
    do
    {
        if (-1 == m_playID || S_STOP == m_status)
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

        uint32_t bufSize   = PLAY_GetSourceBufferRemain(m_playID);
        uint32_t videoSize = PLAY_GetBufferValue(m_playID, BUF_VIDEO_RENDER_DH);

        if (bufSize >= MAX_PBPOOL_MARK)
        {
            /* maximum threshold */
            printf("full timer\n");
            notifyFull(bufSize);
        }
        else if (bufSize <= MIN_PBPOOL_MARK)
        {
            /* minimun threshold */
            printf("empty timer\n");
            notifyEmpty(bufSize);
        }
    }
    while (0);
}


