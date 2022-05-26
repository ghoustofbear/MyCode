#include "hik_ehome_record_gu.h"
#include "hik_pu.h"
#include "frame.h"
#include "tls_error.h"
#include "gumgr.h"
#include "hik_ehome_pu.h"
#include "platform.h"
void CBK
HikEhomeRecordGu::streamcb(LONG recordID, DWORD type, uint8_t *data, DWORD size, void *userData)
{
    ASSERT(userData);

    HikEhomeRecordGu *gu    = (HikEhomeRecordGu *)userData;
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
    sys->sleep(gu->m_interval);
}

void CBK
HikEhomeRecordGu::rawdatacb(LONG recordID, NET_DVR_PACKET_INFO_EX *packet, void *userData)
{
    HikEhomeRecordGu *gu = (HikEhomeRecordGu *)userData;

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
            LOGT_MSV("type: %d, pts: %lld", type, pts);
        }
        else if (VIDEO_P_FRAME == type)
        {
            frame = 'P';
            gu->notifyVideoStream(packet->pPacketBuffer, size, ENCODE_H264, frame, width, height, fps, pts);
            LOGT_MSV("type: %d, pts: %lld", type, pts);
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
    sys->sleep(gu->m_interval);
}

HikEhomeRecordGu::HikEhomeRecordGu(Pu *pu, string filename, string subHost, int subPort, int streamType, int mode)
	: RecordGu(pu, filename, subHost, subPort, streamType, mode)
{
    m_recordID = -1;
    m_bStop = false;
}

HikEhomeRecordGu::HikEhomeRecordGu(Pu *pu, string filename, string addrcode, int streamType, int mode)
	: RecordGu(pu, filename, addrcode, streamType, mode)
{
    m_recordID = -1;
    m_bStop = false;
}

HikEhomeRecordGu::HikEhomeRecordGu(Pu *pu, int channelNo, int type, datetime_s &start, datetime_s &end, string subHost, int subPort, int streamType, int mode)
	: RecordGu(pu, channelNo, type, start, end, subHost, subPort, streamType, mode)
{
    m_recordID = -1;
    m_bStop = false;
}

HikEhomeRecordGu::HikEhomeRecordGu(Pu *pu, string addrcode, int type, datetime_s &start, datetime_s &end, int streamType, int mode)
	: RecordGu(pu, addrcode, type, start, end, streamType, mode)
{
    m_recordID = -1;
    m_bStop = false;
}

HikEhomeRecordGu::~HikEhomeRecordGu()
{

}


int
HikEhomeRecordGu::start(int offset)//值为0表示从文件开始位置播放， 值非0表示断网续传的文件位置
{
    if (-1 != m_recordID)
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

    if (-1 != m_recordID)
    {

        if (m_streamProtoType == 0)
        {
            NET_DVR_SetPlayDataCallBack_V40(m_recordID, streamcb, this);
        }
        else if (m_streamProtoType == 1)
        {
            NET_DVR_SetPlayBackESCallBack(m_recordID, rawdatacb, this);
        }

		if (0 == m_download)
		{
			play("1", "");
		}
		else
		{
			play("", "");
		}
               
        NET_DVR_PlayBackControl(m_recordID, NET_DVR_PLAYSTART, offset, NULL);
        NET_DVR_PlayBackControl(m_recordID, NET_DVR_PLAYNORMAL, 0, NULL);


        /* NET_DVR_PlayBackControl(m_recordID, NET_DVR_PLAYFAST, 0, NULL); */

        recordGuMgr->addGu(this);

        return 0;
    }

    return -1;
}


//注册回放码流的回调函数
BOOL CALLBACK 
PlayBackDataCallback(LONG lPlayBackLinkHandle, NET_EHOME_PLAYBACK_DATA_CB_INFO *pDataCBInfo, void *pUserData) 
{ 
	ASSERT(pUserData);
	if (NULL == pDataCBInfo)
	{
		return false;
	}
	HikEhomeRealGu *pGu = (HikEhomeRealGu *)pUserData;
	pGu->m_lRealHandle = lPlayBackLinkHandle;//保存回放句柄
	void* data = pDataCBInfo->pData;
	int size   = pDataCBInfo->dwDataLen;
	int type   = pDataCBInfo->dwType;
	return TRUE; 
}

//回放连接请求的响应回调函数 
BOOL CALLBACK 
PlaybackNewlinkCallback(LONG lPlayBackLinkHandle, NET_EHOME_PLAYBACK_NEWLINK_CB_INFO  *pNewLinkCBMsg, void *pUserData)
{
	HikEhomeRecordGu *pGu = (HikEhomeRecordGu *)pUserData;
	pGu->m_lPlaybackLinkHandle = lPlayBackLinkHandle;   
	NET_EHOME_PLAYBACK_DATA_CB_PARAM struDataCB = {0};//回放数据的回调参数
	struDataCB.fnPlayBackDataCB = PlayBackDataCallback;//回放回调函数    
	struDataCB.byStreamFormat = 0;//封装格式 0：PS 1:RTP 
	if (!NET_ESTREAM_SetPlayBackDataCB(lPlayBackLinkHandle, &struDataCB))//SMS注册回调函数接收来自设备的实时码流
	{ 
		ERRE_MSV("NET_ESTREAM_SetPlayBackDataCB failed, error code: %d\n", NET_ESTREAM_GetLastError());
		return FALSE; 
	}    
	return TRUE;
}

//异常回调函数
void CALLBACK PlaybackExceptionCallback(DWORD dwType, LONG iUserID, LONG iHandle, void* pUser) 
{
	DWORD dwError = NET_ESTREAM_GetLastError();
	if (EHOME_PREVIEW_EXCEPTION == dwType)    
	{        
		//预览异常        
		printf("Preview exception, handle=%d, Error:%d", iHandle, dwError);    
	}    
	else if (EHOME_PLAYBACK_EXCEPTION == dwType)    
	{        
		//回放异常        
		printf("Playback exception, handle=%d, Error:%d", iHandle, dwError);    }    
	else if (EHOME_AUDIOTALK_EXCEPTION == dwType)    
	{        
		//语音对讲（转发）异常        
		printf("Stream VoiceTalk exception, handle=%d, Error:%d", iHandle, dwError);    
	}
}
int 
HikEhomeRecordGu::startPlayBack()
{
	HikEhomePu *pu = (HikEhomePu*)m_pu;
	tmap<string, LONG>::iterator pos = pu->m_deviceIDToUserId.find(m_name);
	if (pos == pu->m_deviceIDToUserId.end())
	{
		return -1;
	}
	LONG lUserId = pos->second;
    /*开启SMS监听服务接收来自设备的连接请求*/ 
	NET_EHOME_PLAYBACK_LISTEN_PARAM struListen = {0};//回放监听参数      
	string ip = platform->ip();//本地监听信息
	memcpy(struListen.struIPAdress.szIP, ip.c_str(), sizeof(struListen.struIPAdress.szIP));
	struListen.struIPAdress.wPort = 8000; //监听端口号    
	struListen.fnNewLinkCB = PlaybackNewlinkCallback;    
	struListen.pUserData   = this;    
	struListen.byLinkMode  = 0; //0-TCP, 1-UDP（保留）    
	m_lPlaybackListenHandle = NET_ESTREAM_StartListenPlayBack(&struListen);
	if (m_lPlaybackListenHandle < -1)
	{        
		ERRE_MSV("NET_ESTREAM_StartListenPlayBack failed, error code: %d\n", NET_ESTREAM_GetLastError());        
		return -1;    
	}

	//SMS注册异常回调函数
	NET_ESTREAM_SetExceptionCallBack(0, 0, PlaybackExceptionCallback, NULL);

	//CMS发送回放开始请求给设备     
	NET_EHOME_PLAYBACK_INFO_IN struPlayBackIn = {0};//回放请求的输入参数    
	struPlayBackIn.dwSize = sizeof(struPlayBackIn);    
	struPlayBackIn.dwChannel = 1; //通道号    
	struPlayBackIn.byPlayBackMode = 0; //回放模式：0-按文件名回放，1-按时间回放   
	struPlayBackIn.unionPlayBackMode.struPlayBackbyName.dwSeekType = 0; //0-按字节长度计算,1-按秒数计算   
	struPlayBackIn.unionPlayBackMode.struPlayBackbyName.dwFileOffset = 0;//当dwSeekType为0时，偏移量按字节计算；当dwSeekType等于1时，偏移量按秒数计算  
	memcpy(struPlayBackIn.unionPlayBackMode.struPlayBackbyName.szFileName, m_filename.c_str(), MAX_FILE_NAME_LEN);//文件名称
	memcpy(struPlayBackIn.struStreamSever.szIP,ip.c_str(), sizeof(struPlayBackIn.struStreamSever.szIP));   
	struPlayBackIn.struStreamSever.wPort = 8000;
	NET_EHOME_PLAYBACK_INFO_OUT struPlayBackOut = {0};//回放请求的输出参数
	if(!NET_ECMS_StartPlayBack(lUserId, &struPlayBackIn, &struPlayBackOut))    
	{        
		ERRE_MSV("NET_ECMS_StartPlayBack failed, error code: %d\n", NET_ECMS_GetLastError());                
		return 0;    
	}    
	m_lSessionID = struPlayBackOut.lSessionID;

	//CMS发送码流传输请求给设备
	NET_EHOME_PUSHPLAYBACK_IN struPushPlayBackIn = { 0 };	   //码流传输请求的输入参数    
	struPushPlayBackIn.dwSize = sizeof(struPushPlayBackIn);    
	struPushPlayBackIn.lSessionID = struPlayBackOut.lSessionID;//回放请求的会话ID    
	NET_EHOME_PUSHPLAYBACK_OUT struPushPlayBackOut = { 0 };	   //码流传输请求的输出参数   
	if(!NET_ECMS_StartPushPlayBack(lUserId, &struPushPlayBackIn, &struPushPlayBackOut))    
	{        
		ERRE_MSV("NET_ECMS_StartPushPlayBack failed, error code: %d\n", NET_ECMS_GetLastError());                
		return -1; 
	} 
	return 0;
}

void
HikEhomeRecordGu::stop()
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

void
HikEhomeRecordGu::stopPlayBack()
{
	HikEhomePu *pu = (HikEhomePu*)m_pu;
	tmap<string, LONG>::iterator pos = pu->m_deviceIDToUserId.find(m_name);

	//SMS停止转发码流从而停止回放
	if (m_lPlaybackLinkHandle >= 0)
	{
		if (!NET_ESTREAM_StopPlayBack(m_lPlaybackLinkHandle))
		{ 
			ERRE_MSV("NET_ESTREAM_StopPlayBack failed, error code: %d\n", NET_ECMS_GetLastError()); 
		}
	}

	//CMS向设备发送回放停止请求
	if (pos != pu->m_deviceIDToUserId.end())
	{
		LONG lUserId = pos->second;
		if (!NET_ECMS_StopPlayBack(lUserId, m_lSessionID))
		{
			ERRE_MSV("NET_ECMS_StopPlayBack failed, error code: %d\n", NET_ECMS_GetLastError());
		}
	}


	//停止SMS回放监听服务并断开其余设备的连接
	if (m_lPlaybackListenHandle  >= 0)
	{        
		if (!NET_ESTREAM_StopListenPlayBack(m_lPlaybackListenHandle))
		{            
			printf("NET_ESTREAM_StopListenPlayBack failed, error code: %d\n", NET_ECMS_GetLastError());
		}   
	}    
	//释放被SMS占用的资源    
	NET_ESTREAM_Fini();
}

int
HikEhomeRecordGu::pause()
{
    if (m_pause || m_bStop)
    {
        return 0;
    }

    BOOL result = FALSE;
    if (-1 != m_recordID)
    {
        printf("HikEhomeRecordGu::pause\n");
        result = NET_DVR_PlayBackControl(m_recordID, NET_DVR_PLAYPAUSE, 0, NULL);
        if (!result)
        {
            return -1;
        }
        m_pause = true;
    }

    return 0;
}

int
HikEhomeRecordGu::continuex()
{
    if (!m_pause || m_bStop)
    {
        return 0;
    }

    BOOL result = FALSE;
    if (-1 != m_recordID)
    {
        printf("HikEhomeRecordGu::continuex\n");
        result = NET_DVR_PlayBackControl_V40(m_recordID, NET_DVR_PLAYRESTART, 0, NULL);
        if (!result)
        {
            return -1;
        }
        m_pause = false;
    }

    return  0;
}

int
HikEhomeRecordGu::startRepos()
{
    m_repos = true;
    continuex();

    return 0;
}

int
HikEhomeRecordGu::setPos(int pos)
{
    BOOL result = FALSE;

    //continuex();

    m_repos = true;
    if (-1 != m_recordID)
    {
        result = NET_DVR_PlayBackControl_V40(m_recordID, NET_DVR_PLAYSETPOS, &pos, sizeof(pos), 0, NULL);
    }
    LOGE_MSV("set pos : %d, result:%d\n", pos, result);
    m_repos = false;
    notifyPrivStream(&pos, sizeof(pos), ENCODE_H264, TAG_RPOS);
    notifyVideoStream(&pos, sizeof(pos), ENCODE_H264, 0, 0, 0, 0, 0, TAG_RPOS);

    return result ? 0 : -1;
}

int
HikEhomeRecordGu::setTime(datetime_s &t)
{
    //continuex();

    BOOL         result = FALSE;
    NET_DVR_TIME dt     = {0};

    dt.dwYear   = t.year;
    dt.dwMonth  = t.month;
    dt.dwDay    = t.day;
    dt.dwHour   = t.hour;
    dt.dwMinute = t.minute;
    dt.dwSecond = t.second;

    m_repos = true;

#if 1
    if (-1 != m_recordID)
    {
        result = NET_DVR_PlayBackControl_V40(m_recordID, NET_DVR_PLAYSETTIME, &dt, sizeof(NET_DVR_TIME), 0, NULL);
    }
    m_repos = false;
    notifyPrivStream(&m_repos, sizeof(m_repos), ENCODE_H264, TAG_RPOS);
    notifyVideoStream(&m_repos, sizeof(m_repos), ENCODE_H264, 0, 0, 0, 0, 0, TAG_RPOS);
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
HikEhomeRecordGu::insertRecordLabel(string name)
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
HikEhomeRecordGu::playFrame()
{
    BOOL res = NET_DVR_PlayBackControl_V40(m_recordID, NET_DVR_PLAYFRAME, 0, NULL);
    return res ? 0 : -1;
}

int HikEhomeRecordGu::playNormal()
{
    BOOL res = NET_DVR_PlayBackControl_V40(m_recordID, NET_DVR_PLAYNORMAL, 0, NULL);
    return res ? 0 : -1;
}

int
HikEhomeRecordGu::playReverse(datetime_s &t)
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
HikEhomeRecordGu::playFoward(datetime_s &t)
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
HikEhomeRecordGu::onTimer()
{
    if (-1 != m_recordID)
    {
        if (m_repos || m_bStop)
        {
            return;
        }

        time_t tmNow = time(NULL);
        DWORD pos = 0;
        /* Check pos every 1000 milliseconds */
        if (tmNow -m_prevTime >= 1)
        {
            NET_DVR_PlayBackControl(m_recordID, NET_DVR_PLAYGETPOS, 0, &pos);
            m_prevTime = tmNow;
        
            notifyPrivStream(&pos, sizeof(pos), ENCODE_H264, TAG_POSI);
            notifyVideoStream(&pos, sizeof(pos), ENCODE_H264, 0, 0, 0, 0, 0, TAG_POSI);
        }
    }

    //Gu::onTimer();
    RecordGu::onTimer();
}

int
HikEhomeRecordGu::play(string scale, string range)
{
   
    LOGE_MSV("HikEhomeRecordGu::play:%s\n", scale.c_str());

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

    return 0;
}
