#include "hik_ps_player.h"
#ifndef __ANDROID__
#include "c2sdk.h"
#include "tls_error.h"
#include "frame.h"
#include "sys.h"

#if defined(__linux__)
void
HikPsPlayer::decdatacb(int port, char *data, int size, FRAME_INFO *info, void *reserved1, int reserved2)
#elif defined(_WIN64)
void CBK
HikPsPlayer::decdatacb(long port, char *data, long size, FRAME_INFO *info, void* reserved1, void* reserved2)
#elif defined(_WIN32)
void CBK
HikPsPlayer::decdatacb(long port, char *data, long size, FRAME_INFO *info, long reserved1, long reserved2)
#endif
{
    HikPsPlayer *player = (HikPsPlayer *)reserved1;
    if (player == NULL)
    {
        return;
    }

    /* hik audio decode data also callback from here */
    if (info->nType != T_YV12 &&
        info->nType != T_UYVY &&
        info->nType != T_RGB32)
    {
        return;
    }

    int type = 0;
    if (T_YV12 == info->nType)
    {
        type = 1;
    }
    else if (T_UYVY == info->nType)
    {
    }
    else if (T_RGB32 == info->nType)
    {
        type = 2;
    }
    else
    {
        type = 0;
    }

    player->notifyYUV(type, info->nWidth, info->nHeight, data, size);

    player->m_picw = info->nWidth;
    player->m_pich = info->nHeight;
}

#if defined(__linux__)
void
HikPsPlayer::drawdatacb(int handle, PLAYM4_HDC hdc, void *userData)
#elif defined(_WIN32)
void CBK
HikPsPlayer::drawdatacb(long handle, HDC hdc, HIK_USERDATA userData)
#endif
{
    HikPsPlayer *player = (HikPsPlayer *)userData;
    if (player == NULL)
    {
        return;
    }

    player->notifyHDC((HDC)hdc, (HWND)handle, player->m_picw, player->m_pich);
}

void psstreamdatacb(uint8_t *data, int size, void* userdata, int type, uint64_t pts, uint64_t frameTime)//�ص�������ȡ��װ���PS����������ʾ
{
    HikPsPlayer *player = (HikPsPlayer*)userdata;

    int retrycnt = 20;
    while (retrycnt > 0)
    {
        if (!PlayM4_InputData(player->m_playID, data, size))
        {
            LOGD_SDK("try input data to buffer,%d", retrycnt);
            player->notifyFull(0);

            sys->sleep(40);
            retrycnt--;
        }
        else
        {
            if (retrycnt != 20)
            {
                LOGD_SDK("input data to buffer succ,%d", retrycnt);
            }
            break;
        }
    }

    if (retrycnt <= 0)
    {
        PlayM4_ResetSourceBuffer(player->m_playID);
        ERRE_SDK("Play buffer is full, rest buffer");
    }
    //printf("ft %lld\n", frameTime);
    player->m_framePts = frameTime;


    //player->m_status = player->S_PLAY;

    

    //uint32_t bufSize = PlayM4_GetSourceBufferRemain(player->m_playID);
    //if (MAX_PBPOOL_MARK - 512 * 1024 <= bufSize)
    //{
    //    player->notifyFull(bufSize);
    //}
}

HikPsPlayer::HikPsPlayer(PlayerListener *listener)
: Player(listener)
{
    m_playID  = -1;
    m_bReverse= false;
    m_wnd     = 0;
    m_recfile = NULL;
    m_framePts = 0;
    pspacker = createpsinstance(PS_PACK);               //����PS��װ����
    setpsstreamcallback(pspacker, psstreamdatacb, this);//���ûص�����
}

HikPsPlayer::~HikPsPlayer()
{
	destorypsinstance(pspacker);                        //�ͷ�PS��װ���� 
}

int
HikPsPlayer::open(HWND wnd, int streamMode, int encode, int poolSize, void *head, int size)
{
	m_wnd = wnd;
	m_streamMode = streamMode;
	m_encode = encode;

	if (m_isOpen)
	{
		return 0;
	}

    do
    {
#ifdef _WIN32
        if (!PlayM4_GetPort((long *)&m_playID))
#else
        if (!PlayM4_GetPort((int*)&m_playID))
#endif
        {
            break;
        }

        if (!PlayM4_SetStreamOpenMode(m_playID, streamMode))
        {
            break;
        }

#if 0
        if (!PlayM4_OpenStream(m_playID, (unsigned char*)head, 40, poolSize))
        {
            break;
        }
#endif
        if (!PlayM4_OpenStream(m_playID, NULL, 0, poolSize))
        {
            break;
        }

        if (!PlayM4_Play(m_playID, m_wnd))
        {
            break;
        }
        m_speed = 0;
        m_status = S_PLAY;
        enableDraw(true);

        LOGI_PLY("HikPsPlayer::open() PlayID:%d", m_playID);

        return 0;
    } while (0);


    ERRE_SDK("hik player opened fail. playID:%d error:%d", m_playID, PlayM4_GetLastError(m_playID));
	if (-1 != m_playID)
	{
		PlayM4_FreePort(m_playID);
		m_playID = -1;
	}

	return -1;
}

int
HikPsPlayer::openReverse(HWND wnd, int streamMode, int encode, int poolSize, void *head, int size)
{
	m_wnd = wnd;
	m_streamMode = streamMode;
	m_encode = encode;

	if (m_isOpen)
	{
		return 0;
	}

    do
    {
#ifdef _WIN32
        if (!PlayM4_GetPort((long *)&m_playID))
#else
        if (!PlayM4_GetPort((int*)&m_playID))
#endif
        {
            enableDraw(true);
        }

        if (!PlayM4_SetStreamOpenMode(m_playID, streamMode))
        {
            break;
        }

        if (!PlayM4_OpenStream(m_playID, (unsigned char*)head, 40, poolSize))
        {
            break;
        }

        if (!PlayM4_Play(m_playID, m_wnd))
        {
            break;
        }

        if (!PlayM4_ReversePlay(m_playID))//����
        {
            break;
        }

        m_speed = 0;


        enableDraw(true);
        m_status = S_PLAY;

        LOGI_PLY("HikPsPlayer::open() PlayID:%d", m_playID);


        return 0;
    } while (0);

    ERRE_SDK("hik player opened fail. playID:%d error:%d", m_playID, PlayM4_GetLastError(m_playID));
	if (-1 != m_playID)
	{
		PlayM4_FreePort(m_playID);
		m_playID = -1;
	}

	return -1;
}

int
HikPsPlayer::open(HWND wnd, string filename, int poolSize)
{
	m_wnd = wnd;
	m_streamMode = LOCAL_FILE;

	if (m_isOpen)
	{
		return 0;
	}

    do
    {
#ifdef _WIN32
        if (!PlayM4_GetPort((long *)&m_playID))
#else
        if (!PlayM4_GetPort((int*)&m_playID))
#endif
        {
            break;
        }

        if (!PlayM4_SetStreamOpenMode(m_playID, STREAM_FILE))
        {
            break;
        }


        if (!PlayM4_OpenFile(m_playID, (char *)filename.c_str()))
        {
            break;
        }

        if (!PlayM4_Play(m_playID, m_wnd))
        {
            break;
        }

        m_speed = 0;

        opensound();
        m_status = S_PLAY;
        LOGI_PLY("HikPsPlayer::open() PlayID:%d", m_playID);
        return 0;
    } while (0);


    ERRE_SDK("hik player opened fail. playID:%d error:%d", m_playID, PlayM4_GetLastError(m_playID));
    if (-1 != m_playID)
    {
        PlayM4_FreePort(m_playID);
        m_playID = -1;
    }
	return -1;
}

void
HikPsPlayer::close()
{
    LOGI_PLY("HikPsPlayer::close(), PlayID:%d", m_playID);
	stopREC();

	if (-1 != m_playID)
	{
		PlayM4_Stop(m_playID);
		if ((LOCAL_FILE == m_streamMode))
		{
			PlayM4_CloseFile(m_playID);
		}
		else
		{
			PlayM4_CloseStreamEx(m_playID);
		}
		PlayM4_FreePort(m_playID);
		m_playID = -1;
	}

	m_listener = NULL;
	m_status = S_STOP;

}

int
HikPsPlayer::inputData(void *hdr, uint32_t size)
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

	enableDecode(true);

	void *stream = ((ut_frame_s *)hdr)->buf;
	size         = ((ut_frame_s *)hdr)->size;
	int type     = ((ut_frame_s*)hdr)->type;
	uint64_t pts = ((ut_frame_s*)hdr)->pts;

    inputstreamdata(pspacker, (uint8_t*)stream, size, type, pts * 90, pts);

    REC(stream, size);

    return 0;
}

int
HikPsPlayer::control(int code, void *val, int valSize)
{
	if (-1 == m_playID)
	{
		ERRE_SDK("Invalidate player ID.");
		return -1;
	}

	//if (m_status = S_PAUSE && (PLAY_STARTREPOS == code || PLAY_SETPOS == code || PLAY_SETTIME == code))
	//{
	//    ERRE_SDK("cant set pos when pause record");
	//    return -1;
	//}

	int res = 1;
	switch (code)
	{
	case PLAY_START:
	{
		if (m_bReverse)
		{
			m_bReverse = false;
			res = PlayM4_Play(m_playID, m_wnd);
            m_status = S_PLAY;
		}
	}
		break;
	case PLAY_PAUSE:
	{
		if (!m_bReverse)
		{
			res = PlayM4_Pause(m_playID, true);
			m_status = S_PAUSE;
		}
	}
		break;
	case PLAY_RESTART:
	{
		if (!m_bReverse)
		{
			res = PlayM4_Pause(m_playID, false);
			m_status = S_PLAY;
		}
	}
		break;
	case PLAY_FAST:
	{
		if (!m_bReverse)
		{
			m_speed++;
			if (m_speed > 4)
			{
				m_speed = 4;
			}
			res = PlayM4_Fast(m_playID);
		}
	}
		break;
	case PLAY_SLOW:
	{
		if (!m_bReverse)
		{
			m_speed--;
			if (m_speed < -4)
			{
				m_speed = -4;
			}
			res = PlayM4_Slow(m_playID);
		}
	}
		break;
	case PLAY_NORMAL:
	{
		/* note: PlayM4_SetPlayMode is no longer support in new hik sdk */
		/* res = PlayM4_SetPlayMode(m_playID, true); */
		if (!m_bReverse)
		{
			//int cnt = abs(m_speed);
			//for (int i = 0; i < cnt; ++i)
			//{
			//	if (m_speed > 0)
			//	{
			//		res = PlayM4_Slow(m_playID);
			//		m_speed--;
			//	}
			//	else
			//	{
			//		res = PlayM4_Fast(m_playID);
			//		m_speed++;
			//	}
			//}
            res = PlayM4_Play(m_playID, m_wnd);
			m_status = S_PLAY;
		}
	}
		break;
	case PLAY_STARTREPOS:
	{
		if (!m_bReverse)
		{
			m_repos = true;
			res = PlayM4_ResetSourceBuffer(m_playID);
		}
	}
		break;
	case PLAY_SETPOS:
	case PLAY_SETTIME:
	{
		if (!m_bReverse)
		{
			if ((LOCAL_FILE == m_streamMode) && val)
			{
				int pos = *((int *)val);
				res = PlayM4_SetPlayPos(m_playID, pos / (float)100);
			}
			else
			{
				res = PlayM4_ResetSourceBuffer(m_playID);
				//if (code == PLAY_SETPOS)
				//{
				//	int pos = *((int *)val);
				//	PlayM4_SetPlayPos(m_playID, pos / (float)100);
				//}
			}
            m_repos = false;
		}
	}
		break;
	case PLAY_OPENAUDIO:
		if (!m_bReverse)
		{
			res = opensound();
		}
		break;
	case PLAY_CLOSEAUDIO:
		if (!m_bReverse)
		{
			res = closesound();
		}
		break;
	case PLAY_STEP:
		if (!m_bReverse)
		{
			res = PlayM4_OneByOne(m_playID);
		}
		break;
	case PLAY_SETVOLUME:
	{
		if (!m_bReverse)
		{
			int volume = *((int *)val);
			res = PlayM4_SetVolume(m_playID, volume);
		}
	}
		break;
	case PLAY_FORWARD:
	{
		if (m_bReverse)
		{
			m_bReverse = false;
			res = PlayM4_Play(m_playID, m_wnd);
		}
	}
		break;
	case PLAY_REVERSE:
	{
		if (!m_bReverse)
		{
			m_bReverse = true;
			res = PlayM4_ReversePlay(m_playID);
		}
	}
		break;

	default:
		ERRE_SDK("Unknow control commond.");
		return -1;
	}

	if (!res)
	{
		ERRE_SDK("PlayM4_xxx call failed err: %d.", PlayM4_GetLastError(m_playID));
	}

	return (res == 1) ? 0 : -1;
}

int
HikPsPlayer::capture(string filepath, int type)
{
    if (-1 == m_playID)
    {
        ERRE_PLY("Invalidate player ID.");
        return -1;
    }


    LONG w = 0, h = 0;


    if (m_picw == 0 || m_pich == 0)
    {

        ERRE_PLY("not decodec, w=h=0");
        return -2;
    }
    w = m_picw;
    h = m_pich;

#if 0
    //if (PlayM4_GetPictureSize(m_playID, &w, &h) == FALSE)
    //{
    //    ERRE_PLY("PlayM4_GetPictureSize faild play_ID:%d", m_playID);
    //    return -2;
    //}
#endif
    LOGI_PLY("w:%d h:%d", w, h);

    FILE *pic = NULL;
    pic = fopen(filepath.c_str(), "wb");
    if (!pic)
    {
        ERRE_PLY("open file: %s fail", filepath.c_str());
        return  -3;
    }

    int ret = 0;

    DWORD picSize = 0;
    DWORD bufSize = 0;
    uint8_t* buffer = NULL;
    bufSize = 60 + (w * h * 4);
    buffer = new uint8_t[bufSize];
    if (!buffer)
    {
        ERRE_PLY("new uint8_t[%d] err", bufSize);
        fclose(pic);
        return -4;
    }


    if (0 == type)
    {
        if (TRUE == PlayM4_GetBMP(m_playID, buffer, bufSize, &picSize))
        {
            ret = fwrite(buffer, picSize, 1, pic);
            LOGI_PLY("picName:%s, picsize:%d, ret%d", filepath.c_str(), picSize, ret);
        }
        else
        {
            ERRE_PLY("PlayM4_GetBMP err: %d", PlayM4_GetLastError(m_playID));
            ret = -5;
        }

    }
    else if (1 == type)
    {
        if (TRUE == PlayM4_GetJPEG(m_playID, buffer, bufSize, &picSize))
        {

            ret = fwrite(buffer, picSize, 1, pic);
            LOGI_PLY("picName:%s, picsize:%d, ret:%d", filepath.c_str(), picSize, ret);
        }
        else
        {
            ERRE_SDK("PlayM4_GetJPEG err: %d", PlayM4_GetLastError(m_playID));
            ret = -5;
        }

    }

    if (ret == 1)
    {
        ret = 0;
    }

    fclose(pic);
    delete[]buffer;
    return ret;
}

int
HikPsPlayer::pos(datetime_s *start, datetime_s *end)
{
	if (-1 == m_playID)
	{
		//ERRE_SDK("Invalidate player ID.");
		return -1;
	}

    if (m_repos)
    {
        //ERRE_SDK("Waiting reposing.");
        return -1;
    }

	//uint32_t bufSize = PlayM4_GetSourceBufferRemain(m_playID);
	//uint32_t videoSize = PlayM4_GetBufferValue(m_playID, BUF_VIDEO_DECODED);

	//if (0 == bufSize && 0 == videoSize)
	//{
	//	ERRE_SDK("Play buffer is empty.");
	//	return -1;
	//}

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

#if 0

        datetime_s hikTime = { 0 };
        if (0 == time(hikTime))
        //if (m_framePts > 0)
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

            //time_t current = m_framePts / 1000;
            m_pos = (int)((current - sstart) / float(send - sstart + 1) * 100);

            printf("get pos :%d\n", m_pos);
        }
#else
        
        
        if (m_framePts > 0)
        {           

            time_t current = m_framePts;
            m_pos = (int)((current - sstart) / float(send - sstart + 1) * 100);

            //printf("get pos :%d\n", m_pos);
        }

#endif

        //LOGI_SDK("play pos: %d", m_pos);

        return m_pos >= 0 ? m_pos : 0;
    }
    else
    {
        if (LOCAL_FILE == m_streamMode)
        {
            float pos = PlayM4_GetPlayPos(m_playID);
            m_pos = (int)(pos * 100);
        }
        //printf("f get pos :%d\n", m_pos);
        return m_pos;
    }
}

int
HikPsPlayer::time(datetime_s &time)
{
	if (-1 == m_playID)
	{
		ERRE_SDK("Invalidate player ID.");
		return -1;
	}


#if 1
    sys->strptime(m_framePts, time);

#else
    PLAYM4_SYSTEM_TIME hikTime = { 0 };
    if (!PlayM4_GetSystemTime(m_playID, &hikTime))
    {
        ERRE_SDK("PlayM4_GetSystemTime err: %d", PlayM4_GetLastError(m_playID));
        return -1;
    }

    time.year = hikTime.dwYear;
    time.month = hikTime.dwMon;
    time.day = hikTime.dwDay;
    time.hour = hikTime.dwHour;
    time.minute = hikTime.dwMin;
    time.second = hikTime.dwSec;
#endif

    return 0;
}

int
HikPsPlayer::startREC(string filepath)
{
	filepath += ".hkv";

	lock_guard<mutex> locker(m_wrmtx);
	m_recfile = fopen(filepath.c_str(), "wb");
	if (!m_recfile)
	{
		return -1;
	}
	fwrite(hikNewHdr, 40, 1, m_recfile);
	return 0;
}

void
HikPsPlayer::stopREC()
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
HikPsPlayer::changeWndSize(int width, int height)
{
    int ret = 0;
#ifdef UNIX
    ret = PlayM4_WndResolutionChange(m_playID);
#endif
    return ret;
}

int HikPsPlayer::setDisplayRegion(float left, float right, float top, float bottom)
{
	if (-1 == m_playID)
	{
		return -1;
	}

	if (right <= left)
	{
		bool ret = PlayM4_SetDisplayRegion(m_playID, 0, NULL, NULL, true);
		return ret ? 0 : -1;
	}
	else
	{
#if defined(__linux__)
		int picW = 0;
		int picH = 0;
#else
		LONG picW = 0;
		LONG picH = 0;
#endif 

		PlayM4_GetPictureSize(m_playID, &picW, &picH);
		HKRECT rc;
		rc.left = left * picW;
		rc.right = right * picW;
		rc.top = top * picH;
		rc.bottom = bottom * picH;
		bool ret = PlayM4_SetDisplayRegion(m_playID, 0, &rc, NULL, true);
		return ret ? 0 : -1;
	}
}


void
HikPsPlayer::REC(void *frame, size_t size)
{
	lock_guard<mutex> locker(m_wrmtx);
	if (!m_recfile)
	{
		return;
	}

	fwrite(frame, size, 1, m_recfile);
}


int
HikPsPlayer::opensound()
{
	if (-1 == m_playID)
	{
		ERRE_SDK("Invalidate player ID.");
		return -1;
	}

	bool bRet = PlayM4_PlaySound(m_playID);
	if (bRet)
	{
		bRet = PlayM4_SetVolume(m_playID, 0xFFFF);
	}
	return bRet;
}

int
HikPsPlayer::closesound()
{
	if (-1 == m_playID)
	{
		ERRE_SDK("Invalidate player ID.");
		return -1;
	}

	return PlayM4_StopSound();
}

int
HikPsPlayer::setVolume(int vol)
{
	if (-1 == m_playID)
	{
		ERRE_SDK("Invalidate player ID.");
		return -1;
	}

	WORD v = 0xFFFF / 10 * vol;
	return PlayM4_SetVolume(m_playID, v);

}

void
HikPsPlayer::enableDraw(bool enable)
{
	if (enable)
	{
#ifdef _WIN32
		PlayM4_RegisterDrawFun(m_playID, drawdatacb, (HIK_USERDATA)this);
#else
		PlayM4_RegisterDrawFun(m_playID, drawdatacb, this);
#endif
	}
	else
	{
#ifdef _WIN32
		PlayM4_RegisterDrawFun(m_playID, NULL, 0);
#else
		PlayM4_RegisterDrawFun(m_playID, NULL, NULL);
#endif
	}
}

void
HikPsPlayer::enableDecode(bool enable)
{

	if (enable)
	{

#if defined(__linux__)
		PlayM4_SetDecCallBackExMend(m_playID, decdatacb, NULL, 0, (void*)this);
#elif defined(_WIN64)
		PlayM4_SetDecCallBackExMend(m_playID, decdatacb, NULL, 0, (void*)this);
#elif defined(_WIN32)
		PlayM4_SetDecCallBackExMend(m_playID, decdatacb, NULL, 0, (long)this);
#else
		PlayM4_SetDecCallBackExMend(m_playID, decdatacb, NULL, 0, this);
#endif
	}
	else
	{
#ifdef _WIN32
		PlayM4_SetDecCallBackExMend(m_playID, NULL, NULL, 0, 0);
#else
		PlayM4_SetDecCallBackExMend(m_playID, NULL, NULL, 0, NULL);
#endif
	}
}

void
HikPsPlayer::timerdriver()
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

        DWORD bufSize = PlayM4_GetSourceBufferRemain(m_playID);
        if (bufSize > MAX_PBPOOL_SIZE)
        {
            printf("PlayM4_GetSourceBufferRemain eerr\n");
            return;
        }

        if (bufSize >= MAX_PBPOOL_MARK)
		{
			/* maximum threshold */
			notifyFull(bufSize);
		}
		else if (bufSize <= MIN_PBPOOL_MARK)
		{
			/* minimun threshold */
			notifyEmpty(bufSize);
		}
	} while (0);
}

#endif
