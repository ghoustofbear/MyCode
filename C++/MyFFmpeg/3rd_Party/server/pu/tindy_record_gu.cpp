#include "tindy_record_gu.h"
#include "tindy_pu.h"
#include "frame.h"
#include "tls_error.h"
#include "gumgr.h"


void CBK
TindyRecordGu::streamcb(unsigned int userID, unsigned char* data, int len, int flag, void* userData)
{
 
    ASSERT(data);

#if 0
    static FILE *fp = NULL;
    if (fp == NULL)
    {
        fp = fopen("tdServer.td", "wb");
    }

    //DWORD tc = GetTickCount();    
    //fprintf(fp, "%d :%d :%d\n",userID, flag,len);
    
    fwrite(data,len,1, fp);
        
    fflush(fp);
#endif

    TindyRecordGu *pGu = (TindyRecordGu *)userData;
    if (pGu->m_repos)
    {
        return;
    }

    pGu->notifyPrivStream((void*)data, len, ENCODE_H264, TAG_TIANDY);

    pGu->traffic(len);
    pGu->m_lastpts = sys->currentTimeMillis();
}


void CBK
TindyRecordGu::rawdatacb(unsigned int userID, unsigned char* data, int size, RAWFRAME_INFO*  frameInfo, void* userData)
{
    ASSERT(data);
  

#if 0
    static FILE *fp = NULL;
    if (fp == NULL)
    {
        fp = fopen("rawdatacb_std.h264", "wb");
    }

    fwrite(data, size, 1, fp);
    fflush(fp);
#endif
    TindyRecordGu   *gu = (TindyRecordGu *)userData;

    if (gu->m_repos)
    {
        return;
    }


    uint64_t pts = sys->currentTimeMillis();

    switch (frameInfo->nType)
    {
    case VI_FRAME:
    case VP_FRAME:
    {
        int iEncode = ENCODE_UNKNOW;
        char framType = 'I';

        if (frameInfo->nType == VI_FRAME)
        {
            framType = 'I';
        }
        else
        {
            framType = 'P';
        }


        switch (frameInfo->nEnCoder)
        {
        case RAW_VIDEO_H264:
            iEncode = ENCODE_H264;
            break;
        case RAW_VIDEO_H265:
            iEncode = ENCODE_HEVC;
            break;
        case RAW_VIDEO_MJPEG:
            iEncode = ENCODE_JPEG;
            break;
        default:
			return;
            break;
        }
        if (gu->m_streamProtoType == 0)
        {
            gu->notifyPrivStream(data, size, ENCODE_H264, TAG_STD);
        }
        else
        {
            //printf("pts:%lld\n", (frameInfo->nAbsStamp) * 1000);
            gu->notifyVideoStream(data, size, iEncode, framType, frameInfo->nWidth, frameInfo->nHeight, frameInfo->nFrameRate, (frameInfo->nAbsStamp) * 1000);
        }
	    

        break;
    }
    case  AUDIO_FRAME:
    {
        gu->notifyAudioStream(data, size, ENCODE_G711U, 8000, 1, 16, (frameInfo->nAbsStamp) * 1000);
        break;
    }
    default:
        break;
    }

    gu->traffic(size);
    gu->m_lastpts = pts;

}

TindyRecordGu::TindyRecordGu(Pu *pu, string filename, string subHost, int subPort, int streamType, int mode)
    : RecordGu(pu, filename, subHost, subPort, streamType, mode)
{
    m_recordID = -1;
}

TindyRecordGu::TindyRecordGu(Pu *pu, string filename, string addrcode, int streamType, int mode)
    : RecordGu(pu, filename, addrcode, streamType, mode)
{
    m_recordID = -1;
}

TindyRecordGu::TindyRecordGu(Pu *pu, int channelNo, int type, datetime_s &start, datetime_s &end, string subHost, int subPort, int streamType, int mode)
    : RecordGu(pu, channelNo, type, start, end, subHost, subPort, streamType, mode)
{
    m_recordID = -1;
}

TindyRecordGu::TindyRecordGu(Pu *pu, string addrcode, int type, datetime_s &start, datetime_s &end, int streamType, int mode)
    : RecordGu(pu, addrcode, type, start, end, streamType, mode)
{
    m_recordID = -1;
}

TindyRecordGu::~TindyRecordGu()
{

}


int
TindyRecordGu::start(int offset)
{
    if (-1 != m_recordID)
    {     
        NetClient_StopPlayBack(m_recordID);
        m_recordID = -1;
    }

	m_prevTime = sys->currentTimeMillis();

	int ret = 0;
    if (m_byname)
    {
        PlayerParam                        stParam = { 0 };
        stParam.iSize                              = sizeof(PlayerParam);
        stParam.iLogonID                           = ((TindyPu*)m_pu)->getUserID();
		stParam.tRawNotifyInfo.iForbidDecodeEnable = RAW_NOTIFY_FORBID_DECODE;
		stParam.iSpeed = 1;               
        memset(stParam.strFileName, 0, LEN_128);
#ifdef __linux__
        
        strcpy(stParam.strFileName, (char *)m_filename.c_str());
#else
        strcpy_s(stParam.strFileName, LEN_128, (char *)m_filename.c_str());        
#endif               
		if (m_streamProtoType == 1)
		{
			stParam.tRawNotifyInfo.pcbkRawFrameNotify = rawdatacb;
			stParam.tRawNotifyInfo.pUserData = this;
		}
		else
		{
            stParam.tRawNotifyInfo.pcbkRawFrameNotify = rawdatacb;
            stParam.tRawNotifyInfo.pUserData = this;
		}

        ret = NetClient_PlayBack((unsigned int *)&m_recordID, PLAYBACK_TYPE_FILE, &stParam, NULL);
    }
    else
    {
        NVS_FILE_TIME start =
        {
            static_cast<unsigned short>(m_start.year),
            static_cast<unsigned short>(m_start.month),
            static_cast<unsigned short>(m_start.day),
            static_cast<unsigned short>(m_start.hour),
            static_cast<unsigned short>(m_start.minute),
            static_cast<unsigned short>(m_start.second),
        };

        NVS_FILE_TIME end =
        {
            static_cast<unsigned short>(m_end.year),
            static_cast<unsigned short>(m_end.month),
            static_cast<unsigned short>(m_end.day),
            static_cast<unsigned short>(m_end.hour),
            static_cast<unsigned short>(m_end.minute),
            static_cast<unsigned short>(m_end.second),
        };


        PlayerParam stParm                        = { 0 };

        stParm.iSize                              = sizeof(PlayerParam);
        stParm.iLogonID                           = ((TindyPu*)m_pu)->getUserID();
        stParm.iChannNo                           = m_channelNo;
        stParm.tBeginTime                         = start;
        stParm.tEndTime                           = end;
        stParm.tRawNotifyInfo.pcbkRawFrameNotify  = rawdatacb;
        stParm.tRawNotifyInfo.pUserData           = this;
        stParm.tRawNotifyInfo.iForbidDecodeEnable = RAW_NOTIFY_FORBID_DECODE;
        stParm.iSpeed                             = 1;
		if (m_streamProtoType == 1)
		{
			stParm.tRawNotifyInfo.pcbkRawFrameNotify = rawdatacb;
			stParm.tRawNotifyInfo.pUserData = this;
		}
		else
		{
            stParm.tRawNotifyInfo.pcbkRawFrameNotify = rawdatacb;
            stParm.tRawNotifyInfo.pUserData = this;
		}

		ret = NetClient_PlayBack((unsigned int *)&m_recordID, PLAYBACK_TYPE_TIME, &stParm, NULL);
    }

	if (ret < 0 || m_recordID < 0)
	{
		ERRE_MSV("device ip: %s, filename: %s,%d NET_DVR_PlayBackByName [raw stream] fail.", m_pu->ip().c_str(), m_filename.c_str(),NetClient_GetLastError());
		return -1;
	}

	//if (m_streamProtoType == 1)
	//{

	//}
	//else
 //   {
 //       NetClient_SetDataPackCallBack(m_recordID, DTYPE_PS, streamcb, this);
 //       ret = NetClient_SetFullStreamNotify_V4(m_recordID, realdatacb2, this);
	//	ret = NetClient_SetNetFileDownloadFileCallBack(m_recordID, streamcb, this);
	//	if (ret)
	//	{
	//		ERRE_MSV("device ip: %s, filename: %s NET_DVR_PlayBackByName [private stream] fail.", m_pu->ip().c_str(), m_filename.c_str());
	//	}
	//}

	recordGuMgr->addGu(this);
	return 0;
}

void
TindyRecordGu::stop()
{
	recordGuMgr->delGu(this);
    if (-1 != m_recordID)
    {
		//NetClient_SetNetFileDownloadFileCallBack(m_recordID, NULL, NULL);
        NetClient_StopPlayBack(m_recordID);       		
        m_recordID = -1;		
    }
}

int
TindyRecordGu::pause()
{
    if (m_pause)
    {
        return 0;
    }


	//bao zheng zai settime/setpos qijian yizhi continue
	if (m_repos)
	{
		return 0;
	}

	printf("pause\n");

    if (-1 != m_recordID)
    {
        int outlen = 0;
        int result = NetClient_PlayBackControl(m_recordID, PLAY_CONTROL_PAUSE, NULL, 0, 0, &outlen);

        m_pause = true;
        return 0;
    }

    return -1;
}

int
TindyRecordGu::continuex()
{
    if (!m_pause)
    {
        return 0;
    }

	printf("continuex\n");
    if (-1 != m_recordID)
    {
        int outlen = 0;
        int result = NetClient_PlayBackControl(m_recordID, PLAY_CONTROL_PLAY, NULL, 0, 0, &outlen);    

        m_pause = false;
    }  
        
    return m_pause == false ? 0 : -1;
}

int
TindyRecordGu::startRepos()
{
    m_repos = true;
	continuex();
    return 0;
}

int
TindyRecordGu::setPos(int pos)
{

#if 1
    int result = false;

    m_repos = true;
    if (-1 != m_recordID)
    {           
        if (pos < 0 || pos > 100)
        {
            ERRE_MSV("TindyRecordGu::setPos() Param error");
            return -1;
        }

        //if (-1 != m_recordID)
        //{
        //    //NetClient_StopRecv(m_recvID);
        //    m_recvID = -1;
        //}


        int outlen = 0;

        //if (m_pause)
        //{
        //    result = NetClient_PlayBackControl(m_recordID, PLAY_CONTROL_PLAY, NULL, 0, NULL, &outlen);
        //    sys->sleep(300);
        //}
        

        int frameNo = 0;
        result = NetClient_PlayBackControl(m_recordID, PLAY_CONTROL_SEEK, &pos, 0, NULL, &outlen);

        LOGI_MSV("[setPos] res : %d", pos);

    }

    m_repos = false;
	if (m_streamProtoType == 0)
	{
		notifyPrivStream(&pos, sizeof(pos), ENCODE_H264, TAG_RPOS);
	}
	else
	{
		notifyVideoStream(&pos, sizeof(pos), ENCODE_H264, 0, 0, 0, 0, 0, TAG_RPOS);
	}



    return result == 0 ? 0 : -1;
#endif

    
}



void AbsSecondsToNvsFileTime(NVS_FILE_TIME* fileTime, long _tTime)
{
    time_t ltime = (time_t)_tTime;
    struct tm* pTime = NULL;

#ifdef _WIN32
    gmtime_s(pTime, &ltime);
#else
 
    pTime = gmtime(&ltime);
#endif

    fileTime->iYear   = pTime->tm_year + 1900;
    fileTime->iMonth  = pTime->tm_mon  + 1;
    fileTime->iDay    = pTime->tm_mday;
    fileTime->iHour   = pTime->tm_hour;
    fileTime->iMinute = pTime->tm_min;
    fileTime->iSecond = pTime->tm_sec;
}


unsigned int NvsFileTimeToAbsSeconds(NVS_FILE_TIME * fileTime)
{
    tm tm_Begin = { 0 };
    tm_Begin.tm_year = fileTime->iYear  + 1900;
    tm_Begin.tm_mon  = fileTime->iMonth + 1;
    tm_Begin.tm_mday = fileTime->iDay;
    tm_Begin.tm_hour = fileTime->iHour;
    tm_Begin.tm_min  = fileTime->iMinute;
    tm_Begin.tm_sec  = fileTime->iSecond;

    time_t timenow, timeSpace;
    tm * pTimeg = NULL;
    time(&timenow);

#ifdef _WIN32
    gmtime_s(pTimeg, &timenow);
#else
    pTimeg = gmtime(&timenow);
#endif

    
    timeSpace = timenow - mktime(pTimeg);
    time_t begin = mktime(&tm_Begin) + timeSpace;
    unsigned int uiSeconds = (unsigned int)begin;
    return uiSeconds;
}




int
TindyRecordGu::setTime(datetime_s &t)
{
    int result = false;
    
    NVS_FILE_TIME tmPos;

    tmPos.iYear   = t.year;
    tmPos.iMonth  = t.month;
    tmPos.iDay    = t.day;
    tmPos.iHour   = t.hour;
    tmPos.iMinute = t.minute;
    tmPos.iSecond = t.second;

    int  jumpTimePos = NvsFileTimeToAbsSeconds(&tmPos);

    m_repos = true;
    if (-1 != m_recordID)
    {
        int outLen = 0;

        //if (m_pause == true)
        //{
        //    result = NetClient_PlayBackControl(m_recordID, PLAY_CONTROL_PLAY, NULL, 0, NULL, &outLen);
        //    sys->sleep(100);
        //}

        result = NetClient_PlayBackControl(m_recordID, PLAY_CONTROL_SEEK, &jumpTimePos, 0, NULL, &outLen);
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
}

void
TindyRecordGu::onTimer()
{
    do
    {
        if (-1 != m_recordID)
        {
            int pos = 0;


			uint64_t tmNow = sys->currentTimeMillis();

			if (tmNow - m_prevTime >= 1000)
			{
				tPlaybackProcess tTmpProcess = { 0 };
				tTmpProcess.iSize = sizeof(tPlaybackProcess);
				tTmpProcess.iPlayByFileOrTime = PLAYBACK_TYPE_FILE;
				int res = NetClient_PlayBackControl(m_recordID, PLAY_CONTROL_GET_PROCESS, &tTmpProcess, tTmpProcess.iSize, NULL, NULL);
				pos = tTmpProcess.iProcess;

				if (m_streamProtoType == 0)
				{
					notifyPrivStream(&pos, sizeof(pos), ENCODE_H264, TAG_POSI);
				}
				else
				{
					notifyVideoStream(&pos, sizeof(pos), ENCODE_H264, 0, 0, 0, 0, 0, TAG_POSI);
				}

				m_prevTime = tmNow;
			}



#if 0			

            if (m_byname)
            {
#if 1
                tPlaybackProcess tTmpProcess = { 0 };
                tTmpProcess.iSize = sizeof(tPlaybackProcess);
                tTmpProcess.iPlayByFileOrTime = PLAYBACK_TYPE_FILE;

                int res = NetClient_PlayBackControl(m_recordID, PLAY_CONTROL_GET_PROCESS, &tTmpProcess, tTmpProcess.iSize, NULL, NULL);

                if (res < 0 )
                {
                    LOGE_MSV("get Pos fail");
                    // break;
                }

                //pos = tTmpProcess.iProcess;
#else
                int iDlSize = 0;
                NetClient_NetFileGetDownloadPos(m_recordID, &pos, &iDlSize);

                if (0 == (m_tmcnt * 25) % 500)
                {
                    if (pos >= 0 || pos <= 100)
                    {
                        notifyPrivStream(&pos, sizeof(pos), ENCODE_H264, TAG_POSI);
                        notifyVideoStream(&pos, sizeof(pos), ENCODE_H264, 0, 0, 0, 0, 0, TAG_POSI);

                    }
                }
#endif
            }
            else
            {
                int cmd  = PLAY_CONTROL_GETUSERINFO;
                int flag = GET_USERDATA_INFO_TIME;
                int size = sizeof(int);

                unsigned int absoluteFrameTime = 0;                

                NetClient_PlayBackControl(m_recordID, cmd, &flag, size, &absoluteFrameTime, &size);


                if (absoluteFrameTime > 0)
                {
                    NVS_FILE_TIME CurrentTime = { 0 };
                    AbsSecondsToNvsFileTime(&CurrentTime, absoluteFrameTime);

                    NVS_FILE_TIME tmFrom, tmTo;
                    tmFrom.iYear   = m_start.year;
                    tmFrom.iMonth  = m_start.month;
                    tmFrom.iDay    = m_start.day;
                    tmFrom.iHour   = m_start.hour;
                    tmFrom.iMinute = m_start.minute;
                    tmFrom.iSecond = m_start.second;

                    tmTo.iYear   = m_end.year;
                    tmTo.iMonth  = m_end.month;
                    tmTo.iDay    = m_end.day;
                    tmTo.iHour   = m_end.hour;
                    tmTo.iMinute = m_end.minute;
                    tmTo.iSecond = m_end.second;

                    unsigned int uiFrom = NvsFileTimeToAbsSeconds(&tmFrom);
                    unsigned int uiTo   = NvsFileTimeToAbsSeconds(&tmTo);

                    if (absoluteFrameTime >= uiFrom && absoluteFrameTime <= uiTo)
                    {
                        pos = (int)(absoluteFrameTime - uiFrom) * 100 / (int)(uiTo - uiFrom); 

                        if (100 == pos )
                        {
                            stop();
                        }


                        if (0 == (m_tmcnt * 25) % 500)
                        {
                            if (pos >= 0 || pos <= 100)
                            {
                                notifyPrivStream(&pos, sizeof(pos), ENCODE_H264, TAG_POSI);
                                notifyVideoStream(&pos, sizeof(pos), ENCODE_H264, 0, 0, 0, 0, 0, TAG_POSI);
                            }
                        }
                    }          
                    else
                    {
                        LOGE_MSV("get Pos fail");
                        break;
                    }

                }
            }
#endif

        }
    } while (0);

    //Gu::onTimer();
    RecordGu::onTimer();
}
