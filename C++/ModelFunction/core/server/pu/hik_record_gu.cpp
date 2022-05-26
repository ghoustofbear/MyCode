#include "hik_record_gu.h"
#include "hik_pu.h"
#include "frame.h"
#include "tls_error.h"
#include "gumgr.h"

void CBK
HikRecordGu::streamcb(LONG recordID, DWORD type, uint8_t *data, DWORD size, void *userData)
{
    ASSERT(userData);

    HikRecordGu *gu    = (HikRecordGu *)userData;
    HIK_DVR_CFG dvrCfg = ((HikPu      *)gu->m_pu)->getDvrCfg();

    /* is resposing */
    if (gu->m_repos)
    {
        return;
    }

    if (0x10 == dvrCfg.byDVRType)
    {
        gu->notifyPrivStream((void *)data, size, ENCODE_H264, TAG_HIK);
    }
    else if (0xbc == dvrCfg.byDVRType ||
             0x29 == dvrCfg.byDVRType)
    {
        gu->notifyPrivStream((void *)data, size, ENCODE_H264, TAG_HIK_N);
    }
    else
    {
        gu->notifyPrivStream((void *)data, size, ENCODE_H264, TAG_HIK_N);
    }

    gu->traffic(size);
    gu->m_lastpts = sys->currentTimeMillis();
    //sys->sleep(gu->m_interval);
}

void CBK
HikRecordGu::rawdatacb(LONG recordID, NET_DVR_PACKET_INFO_EX *packet, void *userData)
{
    HikRecordGu *gu = (HikRecordGu *)userData;

    /* is resposing */
    if (gu->m_repos)
    {
        return;
    }

    uint32_t type = packet->dwPacketType;
    uint32_t size = packet->dwPacketSize;

    switch (type)
    {
    case VIDEO_I_FRAME:
    case VIDEO_P_FRAME:
    case AUDIO_PACKET:
    {
        int width  = packet->wWidth;
        int height = packet->wHeight;
        if (65535 == width || 65535 == height)
        {
            return;
        }

        struct tm t = {0};
        t.tm_year = packet->dwYear  - 1900;
        t.tm_mon  = packet->dwMonth - 1;
        t.tm_mday = packet->dwDay;
        t.tm_hour = packet->dwHour;
        t.tm_min  = packet->dwMinute;
        t.tm_sec  = packet->dwSecond;

        uint64_t pts   = mktime(&t) * 1000 + packet->dwMillisecond;
        uint32_t fps   = packet->dwFrameRate;
        uint32_t frame = 'P';
        if (VIDEO_I_FRAME == type)
        {
            frame = 'I';
            gu->notifyVideoStream(packet->pPacketBuffer, size, ENCODE_H264, frame, width, height, fps, pts);
            //LOGT_MSV("type: %d, pts: %lld", type, pts);
        }
        else if (VIDEO_P_FRAME == type)
        {
            frame = 'P';
            gu->notifyVideoStream(packet->pPacketBuffer, size, ENCODE_H264, frame, width, height, fps, pts);
            //LOGT_MSV("type: %d, pts: %lld", type, pts);
        }
        else
        {
            gu->notifyAudioStream(packet->pPacketBuffer, size, ENCODE_G711U, 8000, 1, 16, pts);
        }
    }
    break;
    default:
        /* HIKGU_DEBUG("unkonw frame type.\n"); */
        break;
    }

    gu->traffic(size);
    gu->m_lastpts = sys->currentTimeMillis();
    //sys->sleep(gu->m_interval);
}

HikRecordGu::HikRecordGu(Pu *pu, string filename, string subHost, int subPort, int streamType, int mode)
	: RecordGu(pu, filename, subHost, subPort, streamType, mode)
{
    m_recordID = -1;
    m_bStop = false;
}

HikRecordGu::HikRecordGu(Pu *pu, string filename, string addrcode, int streamType, int mode)
	: RecordGu(pu, filename, addrcode, streamType, mode)
{
    m_recordID = -1;
    m_bStop = false;
}

HikRecordGu::HikRecordGu(Pu *pu, int channelNo, int type, datetime_s &start, datetime_s &end, string subHost, int subPort, int streamType, int mode)
	: RecordGu(pu, channelNo, type, start, end, subHost, subPort, streamType, mode)
{
    m_recordID = -1;
    m_bStop = false;
}

HikRecordGu::HikRecordGu(Pu *pu, string addrcode, int type, datetime_s &start, datetime_s &end, int streamType, int mode)
	: RecordGu(pu, addrcode, type, start, end, streamType, mode)
{
    m_recordID = -1;
    m_bStop = false;
}

HikRecordGu::~HikRecordGu()
{

}

int
HikRecordGu::start(int offset)
{
	m_prevTime = sys->currentTimeMillis();
	m_interval = 0;
    if (0 <= m_recordID)
    {
        NET_DVR_PlayBackControl_V40(m_recordID, NET_DVR_PLAYSTOP, 0, NULL);
        NET_DVR_StopPlayBack(m_recordID);
        m_recordID = -1;
    }

    if (m_byname)
    {
        if (getReverse())
        {
            m_recordID = NET_DVR_PlayBackReverseByName(((HikPu *)m_pu)->getUserID(), (char *)m_filename.c_str(), 0);
        }
        else
        {
            m_recordID = NET_DVR_PlayBackByName(((HikPu *)m_pu)->getUserID(), (char *)m_filename.c_str(), 0);
        }
        if (m_recordID < 0)
        {
            ERRE_MSV("device ip: %s, filename: %s NET_DVR_PlayBackByName fail, err %d.", m_pu->ip().c_str(), m_filename.c_str(), NET_DVR_GetLastError());
        }
    }
    else
    {
        NET_DVR_TIME start =
        {
            static_cast<DWORD>(m_start.year),
            static_cast<DWORD>(m_start.month),
            static_cast<DWORD>(m_start.day),
            static_cast<DWORD>(m_start.hour),
            static_cast<DWORD>(m_start.minute),
            static_cast<DWORD>(m_start.second),
        };

        NET_DVR_TIME end =
        {
            static_cast<DWORD>(m_end.year),
            static_cast<DWORD>(m_end.month),
            static_cast<DWORD>(m_end.day),
            static_cast<DWORD>(m_end.hour),
            static_cast<DWORD>(m_end.minute),
            static_cast<DWORD>(m_end.second),
        };

        m_recordID = NET_DVR_PlayBackByTime(((HikPu *)m_pu)->getUserID(), m_channelNo + 1, &start, &end, 0);
        if (m_recordID < 0)
        {
            ERRE_MSV("device ip: %s, chn: %d NET_DVR_PlayBackByTime fail, err %d.", m_pu->ip().c_str(), m_channelNo + 1, NET_DVR_GetLastError());
        }
    }

    if (0 <= m_recordID)
    {
		
        if (m_streamProtoType == 0)
        {
            NET_DVR_SetPlayDataCallBack_V40(m_recordID, streamcb, this);
        }
        else if (m_streamProtoType == 1)
        {
            NET_DVR_SetPlayBackESCallBack(m_recordID, rawdatacb, this);
        }

		NET_DVR_PlayBackControl(m_recordID, NET_DVR_PLAYSTART, offset, NULL);
		//NET_DVR_PlayBackControl(m_recordID, NET_DVR_PLAYNORMAL, 0, NULL);

		if (0 == m_download)
		{
			//play("1", "");
			NET_DVR_PlayBackControl_V40(m_recordID, NET_DVR_PLAYNORMAL, 0, NULL, 0, NULL);
			m_speed = 0;
		}
		else
		{
			//play("", "");
			m_speed = 4;
		}
               


		
        /* NET_DVR_PlayBackControl(m_recordID, NET_DVR_PLAYFAST, 0, NULL); */

        recordGuMgr->addGu(this);

        return 0;
    }

    return -1;
}

void
HikRecordGu::stop()
{
    recordGuMgr->delGu(this);
    if (-1 != m_recordID)
    {
        /* NOTE: it will block for a period of time on linux! */
        m_bStop = true;
        NET_DVR_SetPlayDataCallBack_V40(m_recordID, NULL, this);
        NET_DVR_SetPlayBackESCallBack(m_recordID, NULL, this);
        NET_DVR_StopPlayBack(m_recordID);
    }
    m_recordID = -1;
}

int
HikRecordGu::pause()
{
    if (m_pause || m_bStop)
    {
        return 0;
    }

	//bao zheng zai settime/setpos qijian yizhi continue
	if (m_repos)
	{
		return 0;
	}

    BOOL result = FALSE;
    if (-1 != m_recordID)
    {
		printf("HikRecordGu::pause\n");
        result = NET_DVR_PlayBackControl(m_recordID, NET_DVR_PLAYPAUSE, 0, NULL);
        if (!result)
        {
			int errorNo = NET_DVR_GetLastError();
			LOGI_MSV("NET_DVR_PlayBackControl failed ,errorNo = %d", errorNo);
            return -1;
        }
        m_pause = true;
    }

    return 0;
}

int
HikRecordGu::continuex()
{
    if (!m_pause || m_bStop)
    {
        return 0;
    }

    BOOL result = FALSE;
    if (-1 != m_recordID)
    {
        printf("HikRecordGu::continuex\n");
        result = NET_DVR_PlayBackControl_V40(m_recordID, NET_DVR_PLAYRESTART, 0, NULL);
        if (!result)
        {
			int errorNo = NET_DVR_GetLastError();
			LOGI_MSV("continuex ,errorNo = %d", errorNo);
            return -1;
        }
        m_pause = false;
    }

    return  0;
}

int
HikRecordGu::startRepos()
{
    m_repos = true;
    continuex();

    return 0;
}

int
HikRecordGu::setPos(int pos)
{
    if (m_repos == false)
    {
        ERRE_MSV("playback ctrol invalid\n");
        return -1;
    }

    BOOL result = FALSE;
    //continuex();
    if (-1 != m_recordID)
    {
        result = NET_DVR_PlayBackControl_V40(m_recordID, NET_DVR_PLAYSETPOS, &pos, sizeof(pos), 0, NULL);
    }
    //LOGD_MSV("set pos : %d, result:%d\n", pos, result);
    m_repos = false;
	if (m_streamProtoType == 0)
	{
		notifyPrivStream(&pos, sizeof(pos), ENCODE_H264, TAG_RPOS);
	}
	else
	{
		notifyVideoStream(&pos, sizeof(pos), ENCODE_H264, 0, 0, 0, 0, 0, TAG_RPOS);
	}      

    return result ? 0 : -1;
}

int
HikRecordGu::setTime(datetime_s &t)
{
    //continuex();
    if (m_repos == false)
    {
        ERRE_MSV("playback ctrol invalid\n");
        return -1;
    }

    BOOL         result = FALSE;
    NET_DVR_TIME dt     = {0};

    dt.dwYear   = t.year;
    dt.dwMonth  = t.month;
    dt.dwDay    = t.day;
    dt.dwHour   = t.hour;
    dt.dwMinute = t.minute;
    dt.dwSecond = t.second;

#if 1
    if (-1 != m_recordID)
    {
        result = NET_DVR_PlayBackControl_V40(m_recordID, NET_DVR_PLAYSETTIME, &dt, sizeof(NET_DVR_TIME), 0, NULL);

    }
    m_repos = false;

	if (m_streamProtoType == 0)
	{
		notifyPrivStream(&m_repos, sizeof(m_repos), ENCODE_H264, TAG_RPOS);
	}
	else
	{
		notifyVideoStream(&m_repos, sizeof(m_repos), ENCODE_H264, 0, 0, 0, 0, 0, TAG_RPOS);
	}
    
    return result ? 0 : -1;
#else

    stop();

    NET_DVR_TIME start =
    {
        static_cast<DWORD>(dt.dwYear),
        static_cast<DWORD>(dt.dwMonth),
        static_cast<DWORD>(dt.dwDay),
        static_cast<DWORD>(dt.dwHour),
        static_cast<DWORD>(dt.dwMinute),
        static_cast<DWORD>(dt.dwSecond),
    };

    NET_DVR_TIME end =
    {
        static_cast<DWORD>(m_end.year),
        static_cast<DWORD>(m_end.month),
        static_cast<DWORD>(m_end.day),
        static_cast<DWORD>(m_end.hour),
        static_cast<DWORD>(m_end.minute),
        static_cast<DWORD>(m_end.second),
    };

    m_recordID = NET_DVR_PlayBackByTime(((HikPu *)m_pu)->getUserID(), m_channelNo + 1, &start, &end, 0);
    if (m_recordID < 0)
    {
        ERRE_MSV("device ip: %s, chn: %d NET_DVR_PlayBackByTime fail, err %d.", m_pu->ip().c_str(), m_channelNo + 1, NET_DVR_GetLastError());
    }

    if (-1 != m_recordID)
    {
        NET_DVR_SetPlayDataCallBack_V40(m_recordID, streamcb, this);
        NET_DVR_SetPlayBackESCallBack(m_recordID, rawdatacb, this);

        NET_DVR_PlayBackControl_V40(m_recordID, NET_DVR_PLAYSTART, 0, NULL);
        NET_DVR_PlayBackControl_V40(m_recordID, NET_DVR_PLAYNORMAL, 0, NULL);
        /* NET_DVR_PlayBackControl_V40(m_recordID, NET_DVR_PLAYFAST, 0, NULL); */
    }
#endif


   //
}

int 
HikRecordGu::insertRecordLabel(string name)
{
    NET_DVR_RECORD_LABEL struRecordLabel = { 0 };
    memset(&struRecordLabel, 0, sizeof(struRecordLabel));
    struRecordLabel.dwSize = sizeof(struRecordLabel);

    NET_DVR_GetPlayBackOsdTime(m_recordID, &struRecordLabel.struTimeLabel);
    if (name.length() <= 0)
    {
        struRecordLabel.byQuickAdd = TRUE;
    }
    else
    {
        struRecordLabel.byQuickAdd = FALSE;
        memcpy(struRecordLabel.sLabelName, name.c_str(), name.length());
    }

    NET_DVR_LABEL_IDENTIFY struLabelIdentiff = { 0 };
    memset(&struLabelIdentiff, 0, sizeof(struLabelIdentiff));

    BOOL res = NET_DVR_InsertRecordLabel(m_recordID, &struRecordLabel, &struLabelIdentiff);
    return res ? 0 : -1;
}

int
HikRecordGu::playFrame()
{
    BOOL res = NET_DVR_PlayBackControl_V40(m_recordID, NET_DVR_PLAYFRAME, 0, NULL);
    return res ? 0 : -1;
}

int HikRecordGu::playNormal()
{
    BOOL res = NET_DVR_PlayBackControl_V40(m_recordID, NET_DVR_PLAYNORMAL, 0, NULL);
    return res ? 0 : -1;
}

int
HikRecordGu::playReverse(datetime_s &t)
{
    NET_DVR_TIME dt     = {0};
    dt.dwYear   = t.year;
    dt.dwMonth  = t.month;
    dt.dwDay    = t.day;
    dt.dwHour   = t.hour;
    dt.dwMinute = t.minute;
    dt.dwSecond = t.second;

    BOOL res = NET_DVR_PlayBackControl_V40(m_recordID, NET_DVR_PLAY_REVERSE, &dt, sizeof(NET_DVR_TIME));
    if (!res)
    {
        ERRE_MSV("NET_DVR_PlayBackControl_V40 NET_DVR_PLAY_REVERSE, erron:%d", NET_DVR_GetLastError());
        return -1;
    }
    else
    {
        return 0;
    }
}

int
HikRecordGu::playFoward(datetime_s &t)
{
    NET_DVR_TIME dt     = {0};
    dt.dwYear   = t.year;
    dt.dwMonth  = t.month;
    dt.dwDay    = t.day;
    dt.dwHour   = t.hour;
    dt.dwMinute = t.minute;
    dt.dwSecond = t.second;

    BOOL res = NET_DVR_PlayBackControl_V40(m_recordID, NET_DVR_PLAY_FORWARD, &dt, sizeof(NET_DVR_TIME));
    if (!res)
    {
        ERRE_MSV("NET_DVR_PlayBackControl_V40 NET_DVR_PLAY_FORWARD, erron:%d", NET_DVR_GetLastError());
        return -1;
    }
    else
    {
        return 0;
    }
}

void
HikRecordGu::onTimer()
{
	if (0 <= m_recordID && m_bStop == false)
    {
		uint64_t tmNow = sys->currentTimeMillis();
        DWORD pos = 0;
        /* Check pos every 1000 milliseconds */
        if (tmNow -m_prevTime >= 1000)
        {
			m_prevTime = tmNow;
			if (m_byname)
			{
				NET_DVR_PlayBackControl(m_recordID, NET_DVR_PLAYGETPOS, 0, &pos);
			}
			else
			{
				pos = 0;
			}

			if (m_streamProtoType == 0)
			{
				notifyPrivStream(&pos, sizeof(pos), ENCODE_H264, TAG_POSI);
			}
			else
			{
				notifyVideoStream(&pos, sizeof(pos), ENCODE_H264, 0, 0, 0, 0, 0, TAG_POSI);
			}
          
        }
    }

    //Gu::onTimer();
    RecordGu::onTimer();
}

int
HikRecordGu::play(string scale, string range)
{
#if 0   
    LOGE_MSV("HikRecordGu::play:%s\n", scale.c_str());

    if ("1" == scale)
    {
        m_interval = 40;
        m_interval = 20;
    }
    else if ("2" == scale)
    {
        m_interval = 20;
        m_interval = 10;
    }
    else if ("4" == scale)
    {
        m_interval = 10;
        m_interval = 5;
    }
    else if ("8" == scale)
    {
        m_interval = 5;
        m_interval = 0;
    }
    else if ("16" == scale)
    {
        m_interval = 2;
        m_interval = 0;
    }
    else if ("0.5" == scale)
    {
        m_interval = 80;
    }
    else if ("0.25" == scale)
    {
        m_interval = 160;
    }
    else if ("0.125" == scale)
    {
        m_interval = 320;
    }
    else if ("0.0625" == scale)
    {
        m_interval = 640;
    }
	else if ("" == scale)
	{
		m_interval = 0;
	}
#endif
	
#if 0

	BOOL res;

	if (range == "+")
	{
		res = NET_DVR_PlayBackControl_V40(m_recordID, NET_DVR_PLAYFAST);
		if (!res)
		{
			ERRE_MSV("NET_DVR_PlayBackControl_V40 NET_DVR_PLAYFAST, erron:%d", NET_DVR_GetLastError());
			return -1;
		}
		else
		{
			m_speed++;
			if (m_speed > 4)
			{
				m_speed = 4;
			}
			return 0;
		}
	}
	else
	{
		res = NET_DVR_PlayBackControl_V40(m_recordID, NET_DVR_PLAYSLOW, NULL, 0);
		if (!res)
		{
			ERRE_MSV("NET_DVR_PlayBackControl_V40 NET_DVR_PLAYSLOW, erron:%d", NET_DVR_GetLastError());
			return -1;
		}
		else
		{
			m_speed--;
			if (m_speed < -4)
			{
				m_speed = -4;
			}
			return 0;
		}
	}

#endif
    return 0;
}
