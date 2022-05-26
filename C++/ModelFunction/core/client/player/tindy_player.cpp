#include "tindy_player.h"
#include "c2sdk.h"
#include "tls_error.h"
#include "frame.h"
#include "3rd/tindy/PLAYSDKM4_INTERFACE.cpp"


static uint8_t gbyTDHeader_H264[88] =
{
    0x19, 0x00, 0xc0, 0x02, 0x40, 0x02, 0x15, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x10, 0x40, 0x1f, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

//头第一个字节，由0x19 改为 0x1E --- chenjiaqi
static uint8_t gbyTDHeader_H264_ex[88] =
{
    0x1E, 0x00, 0x80, 0x07, 0x38, 0x04, 0x17, 0x00,
    0x01, 0x00, 0xE2, 0x07, 0x03, 0x00, 0x06, 0x00,
    0x17, 0x00, 0x13, 0x00, 0x04, 0x00, 0xFF, 0x00,
    0x53, 0x20, 0x4D, 0x55, 0x4C, 0x54, 0x49, 0x2D,
    0x4D, 0x45, 0x44, 0x49, 0x41, 0x20, 0x53, 0x54,
    0x52, 0x45, 0x41, 0x4D, 0x20, 0x28, 0x48, 0x2E,
    0x32, 0x36, 0x34, 0x29, 0x00, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x10, 0x40, 0x1F, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static uint8_t gbyTDHeader_H264_record[88] =
{
    0x19, 0x00, 0xC0, 0x02, 0x40, 0x02, 0x15, 0x00,
    0x01, 0x00, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF,
    0x53, 0x20, 0x4D, 0x55, 0x4C, 0x54, 0x49, 0x2D,
    0x4D, 0x45, 0x44, 0x49, 0x41, 0x20, 0x53, 0x54,
    0x52, 0x45, 0x41, 0x4D, 0x20, 0x28, 0x48, 0x2E,
    0x32, 0x36, 0x34, 0x29, 0x00, 0xFF, 0xFF, 0xFF,
    0xFF, 0xFF, 0xFF, 0xFF, 0x53, 0x5F, 0x01, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x24, 0x00, 0x01, 0x10, 0x40, 0x1F, 0x00, 0x00,
    0x1F, 0xEC, 0x29, 0x30, 0x53, 0x5F, 0x01, 0x00
};

static uint8_t gbyTDHeader_H265[88] =
{
    0x19, 0x00, 0xc0, 0x02, 0x40, 0x02, 0x17, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x10, 0x40, 0x1f, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

static uint8_t gbyTDHeader_mJpeg[88] =
{
    0x19, 0x00, 0xc0, 0x02, 0x40, 0x02, 0x15, 0x00,
    0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
    0x00, 0x00, 0x01, 0x10, 0x40, 0x1f, 0x00, 0x00,
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

typedef struct
{
    unsigned short  FrameRate;
    unsigned short  Width;
    unsigned short  Height;
} S_header;





void __cdecl TindyPlayer::decdatacb(unsigned int _ulID, unsigned char *_cData, int _iLen, const FRAME_INFO_TD *_pFrameInfo, void* _iUser)
{
    TindyPlayer *player = (TindyPlayer *)_iUser;
    if (player == NULL)
    {
        return;
    }

    if (_pFrameInfo->nType != T_YUV420_TD &&
        _pFrameInfo->nType != T_YUV422_TD)
    {
        return;
    }

    int type = 0;
    if (T_YUV420_TD == _pFrameInfo->nType)   ///chenjiqi   no used
    {
        type = 0;
    }

    player->notifyYUV(type, _pFrameInfo->nWidth, _pFrameInfo->nHeight, _cData, _iLen);

    player->m_picw = _pFrameInfo->nWidth;
    player->m_pich = _pFrameInfo->nHeight;
}


int __cdecl TindyPlayer::drawdatacb(long _lHandle, PLAYHDC _hDc, void* _lUserData)
{
    TindyPlayer *player = (TindyPlayer *)_lUserData;
    if (player == NULL)
    {
        return -1;
    }

    player->notifyHDC((HDC)_hDc, (HWND)_lHandle, player->m_picw, player->m_pich);

    return 0;
}

static bool s_TindyPlayerSdkInit = false;
TindyPlayer::TindyPlayer(PlayerListener *listener, bool isOld)
    : Player(listener)
{
    if (!s_TindyPlayerSdkInit)
    {
        if (PLAYSDK_INTERFACE::LoadPLAYSDK() != 0)
        {
            LOGI_PLY("tindy load sdk failed");
        }
        else
        {
            s_TindyPlayerSdkInit = true;
        }
    }

    m_playID    = -1;
    m_isOld     = isOld;
    m_wnd       = 0;
    m_bPlayBack = false;
    m_bstop     = false;
    m_recfile   = NULL;
}

TindyPlayer:: ~TindyPlayer()
{

}

int
TindyPlayer::open(HWND wnd, int streamMode, int encode, int poolSize, void *head, int size)
{
    m_wnd        = wnd;
    m_streamMode = streamMode;

    if (m_isOpen)
    {
        return 0;
    }


    if (m_encode != encode)
    {
        if (-1 != m_playID)
        {
            PLAYSDK_INTERFACE::TC_Stop(m_playID);
            PLAYSDK_INTERFACE::TC_DeletePlayer(m_playID);
            m_playID = -1;
        }

    }

    if (-1 == m_playID)
    {
        m_encode = encode;

        uint8_t tdHeader[88] = { 0 };

        switch (m_encode)
        {
        case 1:
        {
            memcpy(tdHeader, gbyTDHeader_H264_record, 88);
            break;
        }
        case 2:
        {
            memcpy(tdHeader, gbyTDHeader_H265, 88);
            break;
        }
        case 3:
        {
            memcpy(tdHeader, gbyTDHeader_mJpeg, 88);
            break;
        }
        default:
            ERRE_SDK("tindy play m_encode type error");
            return -1;
            break;
        }


        m_playID = PLAYSDK_INTERFACE::TC_CreatePlayerFromStreamEx((PLAYHWND)wnd, tdHeader, 88, poolSize);
        //PLAYSDK_INTERFACE::TC_SetStreamBufferSize(m_playID, 4*1024*1024);
        //PLAYSDK_INTERFACE::TC_SetFrameListBufferSize(m_playID, 10*1024*1024);
        PLAYSDK_INTERFACE::TC_SetModeRule(m_playID, MODERULE_AUTO_ADJUST_STREAM_PLAY_SPEED_YES);
        PLAYSDK_INTERFACE::TC_SetModeRule(m_playID, MODERULE_AUTO_ADJUST_DRAW_YES);

        if (m_playID >= 0)
        {            
            if (-1 == PLAYSDK_INTERFACE::TC_Play(m_playID))
            {
                ERRE_SDK("tindy play faild :[ TC_Play ]");
                PLAYSDK_INTERFACE::TC_DeletePlayer(m_playID);
                m_playID = -1;
                return -1;
            }

            //TC_PlayAudio(m_playID);
        }
        else
        {
            ERRE_SDK("tindy play faild :%d", m_playID);
            //PLAYSDK_INTERFACE::TC_Stop(m_playID);
            //PLAYSDK_INTERFACE::TC_DeletePlayer(m_playID);
            m_playID = -1;
            return -1;
        }

        m_speed = 0;
    }
    m_status = S_PLAY;
    LOGD_SDK("tindy play create success");
#ifdef _WIN32
    //TC_RegisterDrawFun(m_playID, drawdatacb, this);
    //TC_SetDecCallBack(m_playID, decdatacb, this);   
#else
#endif // _WIN32  

    return 0;
}

int
TindyPlayer::open(HWND wnd, string filename, int poolSize)
{
    m_wnd = wnd;

    if (m_isOpen)
    {
        return 0;
    }


    if (-1 == m_playID)
    {
        m_playID = PLAYSDK_INTERFACE::TC_CreatePlayerFromFile((PLAYHWND)m_wnd, (char*)(filename.c_str()), 0, 0, NULL, 0, NULL);

        if (m_playID < 0)
        {
            ERRE_SDK("tindy open file failed  m_playID : %d", m_playID);
            goto exit;
        }

        int ret = PLAYSDK_INTERFACE::TC_Play(m_playID);

        if (-1 == ret)
        {
            ERRE_SDK("tindy open file failed : [ TC_Play ]");
        }

        m_speed = 0;
    }
    m_status = S_PLAY;
    return 0;

exit:
    ERRE_SDK("hik player opened fail by filename.");

    return -1;
}

void
TindyPlayer::close()
{
    stopREC();

    if (-1 != m_playID)
    {
        m_bstop = true;
        m_thread.stop();

        PLAYSDK_INTERFACE::TC_Stop(m_playID);
        PLAYSDK_INTERFACE::TC_DeletePlayer(m_playID);
        m_playID = -1;
    }

    m_listener = NULL;

    m_status = S_STOP;
}

int
TindyPlayer::inputData(void *hdr, uint32_t size)
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



    int ret = 0;
    int retrycnt = 20;
    while (retrycnt > 0)
    {
        ret = PLAYSDK_INTERFACE::TC_PutStreamToPlayer(m_playID, (uint8_t*)stream, size);

        if (ret == -11)
        {
            LOGD_SDK("try input data to buffer,%d", retrycnt);
            notifyFull(0);

            sys->sleep(40);
            retrycnt--;
        }
        else
        {
            break;
        }
    }

    if (retrycnt <= 0)
    {
        ERRE_SDK("Play buffer is full, play end");
        return -2;
    }

    REC(stream, size);

    return 0;
}

int
TindyPlayer::control(int code, void *val, int valSize)
{
    if (-1 == m_playID)
    {
        ERRE_SDK("Invalidate player ID.");
        return -1;
    }

    int res = 0;
    switch (code)
    {
    case PLAY_PAUSE:
        res = PLAYSDK_INTERFACE::TC_Pause(m_playID);
        m_status = S_PAUSE;
        break;

    case PLAY_RESTART:
        res = PLAYSDK_INTERFACE::TC_Play(m_playID);
        m_status = S_PLAY;
        break;

    case PLAY_FAST:
        m_speed++;
        if (m_speed > 4)
        {
            m_speed = 4;
        }
        else if (m_speed < 0)
        {
            m_speed = 0;
        }

        LOGD_PLY("set speed:%\n", pow(2, m_speed));

        res = PLAYSDK_INTERFACE::TC_FastForward(m_playID, pow(2, m_speed));
        break;

    case PLAY_SLOW:
        m_speed--;
        if (m_speed < -4)
        {
            m_speed = -4;
        }
        else if (m_speed > 0)
        {
            m_speed = 0;
        }

        LOGD_PLY("set speed:%\n", pow(2, -m_speed));

        res = PLAYSDK_INTERFACE::TC_SlowForward(m_playID, pow(2, -m_speed));
        break;

    case PLAY_NORMAL:
    {
        int deta = m_speed - 2;

        for (int j = 0; j < abs(deta); ++j)
        {
            if (deta <= 0)
            {
                ++m_speed;
                res += PLAYSDK_INTERFACE::TC_FastForward(m_playID, m_speed);
            }
            else
            {
                --m_speed;
                res += PLAYSDK_INTERFACE::TC_SlowForward(m_playID, m_speed);
            }
        }

        m_status = S_PLAY;
        break;
    }

    case PLAY_STARTREPOS: 
    {
        m_repos = true;
        PLAYSDK_INTERFACE::TC_CleanStreamBuffer(m_playID);
        break;
    }
    case PLAY_SETPOS:
    case PLAY_SETTIME:
        if ((LOCAL_FILE == m_streamMode) && val)
        {
            int pos = *((int *)val);
            int totalFrames = PLAYSDK_INTERFACE::TC_GetFrameCount(m_playID);

            res = PLAYSDK_INTERFACE::TC_SeekEx(m_playID, pos / (float)100 * totalFrames);
        }
        else
        {
            res = PLAYSDK_INTERFACE::TC_CleanStreamBuffer(m_playID);
        }
        m_repos = false;
        break;
    case PLAY_OPENAUDIO:
        opensound();
        break;
    case PLAY_CLOSEAUDIO:
        closesound();
    default:
        ERRE_SDK("Unknow control commond.");
        return -1;
    }

    if (!res)
    {
        ERRE_SDK("tindy control call failed :%d.", code);
    }

    return (res == 0) ? 0 : -1;
}

int
TindyPlayer::capture(string filepath, int type)
{
    if (-1 == m_playID)
    {
        ERRE_SDK("Invalidate player ID.");
        return -1;
    }

    if (0 == type)
    {
        if (!PLAYSDK_INTERFACE::TC_CapturePicture(m_playID, CAPTURE_PICTURE_TYPE_BMP, (char*)filepath.c_str()))
        {
            ERRE_SDK("TC_CapturePicture [BMP] erro :%s", filepath.c_str());
            return -1;
        }
    }
    else if (1 == type)
    {
        if (!PLAYSDK_INTERFACE::TC_CapturePicture(m_playID, CAPTURE_PICTURE_TYPE_JPG, (char*)filepath.c_str()))
        {
            ERRE_SDK("TC_CapturePicture [JPEG] erro :%s", filepath.c_str());
            return -1;
        }
    }
    return 0;
}

int
TindyPlayer::pos(datetime_s *start, datetime_s *end)
{
    if (m_repos)
    {
        ERRE_SDK("Waiting reposing.");
        return -1;
    }

    if (-1 == m_playID)
    {
        ERRE_SDK("Invalidate player ID.");
        return -1;
    }

    if (start && end)
    {
        tm tmStart = {0};
        tm tmEnd   = {0};

        tmStart.tm_year = start->year - 1900 ;
        tmStart.tm_mon  = start->month - 1;
        tmStart.tm_mday = start->day;
        tmStart.tm_hour = start->hour;
        tmStart.tm_min  = start->minute;
        tmStart.tm_sec  = start->second;

        time_t sstart   = mktime(&tmStart);

        tmEnd.tm_year = end->year - 1900 ;
        tmEnd.tm_mon  = end->month - 1;
        tmEnd.tm_mday = end->day;
        tmEnd.tm_hour = end->hour  ;
        tmEnd.tm_min  = end->minute;
        tmEnd.tm_sec  = end->second;

        time_t send   = mktime(&tmEnd);

        time_t TindyTime = 0 ;
        if (0 == time(TindyTime))

        {
            m_pos = (int)((TindyTime) / float(send - sstart + 1) * 100);
        }

        LOGI_SDK("play pos: %d", m_pos);

        return m_pos;
    }
    else
    {
        float frameCnt = PLAYSDK_INTERFACE::TC_GetFrameCount(m_playID);
        float frameCur = PLAYSDK_INTERFACE::TC_GetCurFrameNo(m_playID);

        return frameCnt != 0 ? (int)(frameCur / frameCnt * 100) : 0;
    }

    return m_pos;
}

int
TindyPlayer::time(time_t& time)
{
    time = PLAYSDK_INTERFACE::TC_GetPlayTime(m_playID) / 1000;

    return 0;
}

int
TindyPlayer::startREC(string filepath)
{
    filepath += ".td";

    lock_guard<mutex> locker(m_wrmtx);
    m_recfile = fopen(filepath.c_str(), "wb");
    if (!m_recfile)
    {
        return -1;
    }

    return 0;
}

void
TindyPlayer::stopREC()
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
TindyPlayer::opensound()
{
    if (-1 == m_playID)
    {
        ERRE_SDK("Invalidate player ID.");
        return -1;
    }

    return (PLAYSDK_INTERFACE::TC_PlayAudio(m_playID) == 0 ? 0 : -1);

}

int
TindyPlayer::closesound()
{

    if (-1 == m_playID)
    {
        ERRE_SDK("Invalidate player ID.");
        return -1;
    }

    return (PLAYSDK_INTERFACE::TC_StopAudio(m_playID) == 0 ? 0 : -1);

}

void
TindyPlayer::enableDraw(bool enable)
{
    if (enable)
    {
        PLAYSDK_INTERFACE::TC_RegisterDrawFun(m_playID, drawdatacb, this);
    }
    else
    {
        PLAYSDK_INTERFACE::TC_RegisterDrawFun(m_playID, NULL, 0);
    }
}

void
TindyPlayer::enableDecode(bool enable)
{
    if (enable)
    {
        PLAYSDK_INTERFACE::TC_SetDecCallBack(m_playID, decdatacb, this);
    }
    else
    {
        PLAYSDK_INTERFACE::TC_SetDecCallBack(m_playID, NULL, 0);
    }
}

void
TindyPlayer::REC(void *frame, size_t size)
{
    lock_guard<mutex> locker(m_wrmtx);
    if (!m_recfile)
    {
        return;
    }

    fwrite(frame, size, 1, m_recfile);
}

void
TindyPlayer::timerdriver()
{
    do
    {
        if (-1 == m_playID || S_STOP == m_status)
        {
            break;
        }

        if (STREAM_FILE != m_streamMode)
        {
            break;
        }

        if (m_repos)
        {
            break;
        }

        int StreamBufferState = 0;
        int iRet = PLAYSDK_INTERFACE::TC_GetStreamPlayBufferState(m_playID, &StreamBufferState);

        if (0 >= iRet)
        {
            switch (StreamBufferState)
            {
            case -19:
            {
                notifyEmpty(0);
                break;
            }
            case  -11:
            case -18:
            {
                notifyFull(0);
                break;
            }
            default:
                break;
            }
        }

    }
    while (0);
}



//
//int TindyPlayer::recordProc()
//{
//    int bInput = true;
//
//    while (!m_bstop)
//    {
//        int stateCode = 0;
//
//        if (-1 != m_playID)
//        {
//            PLAYSDK_INTERFACE::TC_GetStreamPlayBufferState(m_playID, &stateCode);
//            if (-19 == stateCode )
//            {
//                bInput = true;
//                m_status = S_PLAY;
//
//                if (0 == m_listFrame.size())
//                {
//                    notifyEmpty(0);
//                    continue;
//                }
//
//                sys->sleep(5000);
//            }
//    
//        }
//
//        while (bInput && m_listFrame.size() > 0)
//        {   
//            {
//                AutoLock lock(m_listFrame._mtx);
//                list<frame_data*>::iterator it = m_listFrame.begin();
//
//                if (-1 != m_playID)
//                {                  
//                    int ret = PLAYSDK_INTERFACE::TC_PutStreamToPlayer(m_playID, (*it)->data, (*it)->len);
//            
//                    //ERRE_SDK("statecode = %d , framesize = %d , ret = %d", stateCode, m_listFrame.size(),ret);
//
//                    if (-11 == ret || -18 == stateCode)
//                    {
//                        m_status = S_PAUSE;
//                        bInput = false;
//                    }
//                }
//
//                delete[](*it)->data;
//                delete *it;
//                m_listFrame.erase(it);
//
//                if (!bInput)
//                {
//                    notifyFull(0);
//                    break;
//                }
//            }
//
//        }
//        sys->sleep(5000);
//
//    }
//
//    return 0;
//}


//void *
//TindyPlayer::threadEntry(ThreadInfo *arg)
//{
//    TindyPlayer * td = (TindyPlayer *)arg->data;
//
//    switch (arg->id)
//    {
//    case TD_RECORD:
//        td->recordProc();
//        break;
//
//    default:
//        break;
//    }
//
//    return NULL;
//}
