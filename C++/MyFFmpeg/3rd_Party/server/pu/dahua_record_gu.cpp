#include "dahua_record_gu.h"
#include "dahua_pu.h"
#include "frame.h"
#include "tls_error.h"
#include "gumgr.h"

#ifdef __linux__
int CBK DahuaRecordGu::RecordDataCallBack(
    LLONG      lRealHandle,
    DWORD       dwDataType,
    BYTE      * pData,
    DWORD       dwDataSize,
    LDWORD    dwUser)

#else
int CBK DahuaRecordGu::RecordDataCallBack(
    LLONG   lRealHandle,
    DWORD   dwDataType,
    BYTE    * pData,
    DWORD   dwDataSize,
    LDWORD   dwUser)
#endif
{
    ASSERT(dwUser);
    DahuaRecordGu   *gu = (DahuaRecordGu *)dwUser;

    if (dwDataType == EM_REAL_DATA_TYPE_PRIVATE)
    {
        //大华私有数据,不支持
        //if (gu->m_streamProtoType != 0)
        //{
        //    return 1;
        //}
        //printf("ty:%d da:%d\n", dwDataType, dwDataSize);
        //gu->notifyPrivStream(pData, dwDataSize, ENCODE_H264, TAG_DAHUA);
        return 1;
    }
    else if (dwDataType == NET_DATA_CALL_BACK_VALUE + EM_REAL_DATA_TYPE_GBPS)
    {
        //国标ps流
        //printf("ty:%d da:%d\n", dwDataType, dwDataSize);

        //if (gu->m_streamProtoType != 1)
        //{
        //    return 1;
        //}
        gu->notifyPrivStream(pData, dwDataSize, ENCODE_H264, TAG_DAHUA);
    }
    else if (dwDataType == NET_DATA_CALL_BACK_VALUE + EM_REAL_DATA_TYPE_H264)
    {
        //H264 裸流
        //printf("ty:%d da:%d\n", dwDataType, dwDataSize);
        gu->notifyVideoStream(pData, dwDataSize, ENCODE_H264, 'I', 1920, 1080, 25, sys->currentTimeMillis());
    }

    else
    {
        return 1;
    }

    gu->m_lastpts = sys->currentTimeMillis();
    gu->traffic(dwDataSize);
    return 1;
}



// 回放进度回调函数原形
// dwDownLoadSize == -1 表示用户回放或者下载进度完成
// dwDownLoadSize ==- 2 表示用户没有回放或者下载操作权限
void  CALLBACK DownLoadPosCallBack(
    LLONG  lPlayHandle,
    DWORD  dwTotalSize,
    DWORD  dwDownLoadSize,
    LDWORD dwUser)
{
    DahuaRecordGu* pGu = (DahuaRecordGu*)(dwUser);
    if (dwDownLoadSize == -1)
    {
        pGu->m_pos = 100;
    }
    else if (dwDownLoadSize == -2)
    {
        pGu->m_pos = 0;
    }
    else
    {
        pGu->m_totalSize = dwTotalSize;
        pGu->m_curSize = dwDownLoadSize;
        pGu->m_pos = ((dwDownLoadSize * 100) / dwTotalSize);

    }
    //printf("total:%d do:%d pos:%d\n", dwTotalSize, dwDownLoadSize, pGu->m_pos);
}

void
DahuaRecordGu::login()
{
    NET_DEVICEINFO_Ex devinfo;

    NET_PARAM netParam = { 0 };
    netParam.nWaittime            = 3000;
    netParam.nConnectTime         = 1500;
    netParam.nConnectTryNum       = 10;
    netParam.nSubConnectSpaceTime = 10;
    netParam.nPicBufSize          = 4 * 1024 * 1024;

    CLIENT_SetNetworkParam(&netParam);

    char           s8DvrIp[MAX_IPV4_STR_LEN] = { 0 };
    unsigned short u32Port = 0;
    string user = m_pu->userName();
    string pswd = m_pu->passWord();
    string ip   = m_pu->ip();
    u32Port     = m_pu->port();

    strncpy(s8DvrIp, ip.c_str(), MAX_IPV4_STR_LEN);

    int iErr = -1;
    m_userId = CLIENT_LoginEx2(
        s8DvrIp,
        u32Port,
        user.c_str(),
        pswd.c_str(),
        EM_LOGIN_SPEC_CAP_TCP,
        NULL,
        &devinfo,
        &iErr);

    if (m_userId <= 0 && iErr != 0)
    {
        LOGI_MSV("DahuaRecordGu login, Dahua ip:%s, port:%d, user:%s, password: %s, [ CLIENT_LoginEx ]failed , errorCode = %d\n",
                 ip.c_str(), u32Port, user.c_str(), pswd.c_str(), iErr);
    }
}

DahuaRecordGu::DahuaRecordGu(Pu *pu, string filename, string subHost, int subPort, int streamType, int mode)
    : RecordGu(pu, filename, subHost, subPort, streamType, mode)
{
    m_recordID = -1;
    m_bufstream = new char[10*1024*1024];
    m_pos     = 0;
    m_scale   = "1";
    m_bexit   = false;
    m_userId  = -1;
    //if (pu->linktype() == 1)
    //{
    //    login();
    //}
}

DahuaRecordGu::DahuaRecordGu(Pu *pu, string filename, string addrcode, int streamType, int mode)
	: RecordGu(pu, filename, addrcode, streamType, mode)
{
    m_recordID = -1;
    m_bufstream = new char[10*1024*1024];
    m_pos     = 0;
    m_scale   = "1";
    m_bexit   = false;
    m_userId  = -1;

    //if (pu->linktype() == 1)
    //{
    //    login();
    //}
}

DahuaRecordGu::DahuaRecordGu(Pu *pu, int channelNo, int type, datetime_s &start, datetime_s &end, string subHost, int subPort, int streamType, int mode)
	: RecordGu(pu, channelNo, type, start, end, subHost, subPort, streamType, mode)
{
    m_recordID = -1;
    m_bufstream = new char[10*1024*1024];
    m_pos     = 0;
    m_scale   = "1";
    m_bexit   = false;
    m_userId  = -1;

    //if (pu->linktype() == 1)
    //{
    //    login();
    //}
}

DahuaRecordGu::DahuaRecordGu(Pu *pu, string addrcode, int type, datetime_s &start, datetime_s &end, int streamType, int mode)
	: RecordGu(pu, addrcode, type, start, end, streamType, mode)
{
    m_recordID = -1;
    m_bufstream = new char[10*1024*1024];
    m_pos     = 0;
    m_scale   = "1";
    m_bexit   = false;
    m_userId = -1;
    //if (pu->linktype() == 1)
    //{
    //    login();
    //}
}

DahuaRecordGu::~DahuaRecordGu()
{
    ERRE_MSV("DahuaRecordGu::~DahuaRecordGu this: %0x !!!", this);

    m_bexit = true;

    if (NULL != m_bufstream)
    {
        delete []m_bufstream;
        m_bufstream = NULL;
    }
}



int
DahuaRecordGu::start(int offset)
{

    if (-1 != m_recordID)
    {
        CLIENT_StopPlayBack(m_recordID);
        m_recordID = -1;
    }

    NET_IN_PLAYBACK_BY_DATA_TYPE stIn = { sizeof(stIn) };
    NET_OUT_PLAYBACK_BY_DATA_TYPE stOut = { sizeof(stOut) };

    stIn.fDownLoadDataCallBack = RecordDataCallBack;
    stIn.dwDataUser = (LDWORD)this;

    stIn.cbDownLoadPos = DownLoadPosCallBack;
    stIn.dwPosUser = (LDWORD)this;

    stIn.hWnd = NULL;//GetSelfWindow();
    stIn.nChannelID = m_channelNo;
    stIn.emDataType = EM_REAL_DATA_TYPE_GBPS;

    if (m_streamProtoType == 0)
    {
        stIn.emDataType = EM_REAL_DATA_TYPE_GBPS;
    }
    else
    {
        stIn.emDataType = EM_REAL_DATA_TYPE_H264;
    }



    if (m_byname)
    {        
        NET_TIME tmStartTime = { 0 };
        NET_TIME tmEndTime = { 0 };

        getFileTimeFromFile(m_filename, tmStartTime, tmEndTime, m_channelNo);
        stIn.stStartTime = tmStartTime;
        stIn.stStopTime = tmEndTime;
        stIn.nChannelID = m_channelNo;
        
        m_start.year = tmStartTime.dwYear;
        m_start.month = tmStartTime.dwMonth;
        m_start.day = tmStartTime.dwDay;
        m_start.hour = tmStartTime.dwHour;
        m_start.minute = tmStartTime.dwMinute;
        m_start.second = tmStartTime.dwSecond;

        m_end.year = tmEndTime.dwYear;
        m_end.month = tmEndTime.dwMonth;
        m_end.day = tmEndTime.dwDay;
        m_end.hour = tmEndTime.dwHour;
        m_end.minute = tmEndTime.dwMinute;
        m_end.second = tmEndTime.dwSecond;

    }
    else
    {
        NET_TIME start1 =
        {
            static_cast<DWORD>(m_start.year),
            static_cast<DWORD>(m_start.month),
            static_cast<DWORD>(m_start.day),
            static_cast<DWORD>(m_start.hour),
            static_cast<DWORD>(m_start.minute),
            static_cast<DWORD>(m_start.second),
        };

        NET_TIME end1 =
        {
            static_cast<DWORD>(m_end.year),
            static_cast<DWORD>(m_end.month),
            static_cast<DWORD>(m_end.day),
            static_cast<DWORD>(m_end.hour),
            static_cast<DWORD>(m_end.minute),
            static_cast<DWORD>(m_end.second),
        };

        stIn.stStartTime = start1;
        stIn.stStopTime = end1;
        stIn.nChannelID = m_channelNo;
    }
    
    m_recordID = CLIENT_PlayBackByDataType(((DahuaPu *)m_pu)->getUserID(), &stIn, &stOut, 5000);


    if (m_recordID)
    {
        recordGuMgr->addGu(this);
        LOGD_MSV("dahua gu create success\n");
    }
    else
    {
        m_recordID = -1;
        return -1;
    }

    
}

void
DahuaRecordGu::stop()
{  
    recordGuMgr->delGu(this);
    if (-1 != m_recordID)
    {
        m_bexit = true;
        CLIENT_StopPlayBack(m_recordID);
    }
    m_recordID = -1;
}

int
DahuaRecordGu::pause()
{
    BOOL result = false;

    if (m_pause)
    {
        return 0;
    }

    //bao zheng zai settime/setpos qijian yizhi continue
    if (m_repos)
    {
        return 0;
    }

    if (-1 != m_recordID)
    {
        result = CLIENT_PausePlayBack(m_recordID, TRUE);
    }

    m_pause = true;
    LOGD_MSV("CLIENT_PausePlayBack:%d\n", result);
    return result ? 0 : -1;
}

int
DahuaRecordGu::continuex()
{
    BOOL result = false;
    if (!m_pause)
    {
        return 0;
    }

    if (-1 != m_recordID)
    {
        result = CLIENT_PausePlayBack(m_recordID, FALSE);
    }
    LOGD_MSV("CLIENT_PausePlayBack:%d\n", result);
    m_pause = false;

    return result ? 0 : -1;
}

int
DahuaRecordGu::startRepos()
{
    m_repos = true;
    continuex();
    return 0;
}

int
DahuaRecordGu::setPos(int pos)
{
    BOOL result = false;
    if (m_repos == false)
    {
        ERRE_MSV("playback ctrol invalid\n");
        return -1;
    }

    m_repos = true;
    if (-1 != m_recordID)
    {
        if (pos <= 100 || pos >= 0)
        {
            unsigned int offsetTM = (m_end.hour - m_start.hour) * 3600 + (m_end.minute - m_start.minute) * 60 + (m_end.second - m_start.second);
            offsetTM = offsetTM / 100 * pos;

            result = CLIENT_SeekPlayBack(m_recordID, offsetTM, 0xFFFFFFFF);
            //result = CLIENT_SeekPlayBack(m_recordID, 0xFFFFFFFF, m_totalSize / 100 * pos);
            //LOGD_MSV("set pos:%d off:%d", m_pos, offsetTM);
        }        
    }

    m_repos = false;
    notifyPrivStream(&pos, sizeof(pos), ENCODE_H264, TAG_RPOS);

    return result ? 0 : -1;
}

int
DahuaRecordGu::setTime(datetime_s &t)
{
    BOOL         result = false;

    int offsetSecond = (t.hour - m_start.hour) * 3600 + (t.minute - m_start.minute) * 60 + (t.second - m_start.second);
    
    m_repos = true;
    if (-1 != m_recordID)
    {
        result = CLIENT_SeekPlayBack(m_recordID, offsetSecond, 0xFFFFFFFF);
    }

    m_repos = false;



    notifyPrivStream(&m_pos, sizeof(m_pos), ENCODE_H264, TAG_RPOS);

    return result ? 0 : -1;

}


void
DahuaRecordGu::getFileTimeFromFile(string filename, NET_TIME& s, NET_TIME& e, int& c)
{
    /* 从文件名中解析通道、时间等信息 */
    char  s8StrTmp[10] = { 0 };
    int   s32I = 2;
    int   s32J = 0;
    while (m_filename[s32I] != '\0')
    {
        s8StrTmp[s32J] = m_filename[s32I];
        if (3 == s32I)
        {
            c = atoi(s8StrTmp);
            s32I += 2;
            s32J = 0;
            memset(s8StrTmp, 0, 10);
            continue;
        }
        else if (8 == s32I)
        {
            s.dwYear = atoi(s8StrTmp);
            s32I++;
            s32J = 0;
            memset(s8StrTmp, 0, 10);
            continue;
        }
        else if (10 == s32I)
        {
            s.dwMonth = atoi(s8StrTmp);
            s32I++;
            s32J = 0;
            memset(s8StrTmp, 0, 10);
            continue;
        }
        else if (12 == s32I)
        {
            s.dwDay = atoi(s8StrTmp);
            s32I += 2;
            s32J = 0;
            memset(s8StrTmp, 0, 10);
            continue;
        }
        else if (15 == s32I)
        {
            s.dwHour = atoi(s8StrTmp);
            s32I++;
            s32J = 0;
            memset(s8StrTmp, 0, 10);
            continue;
        }
        else if (17 == s32I)
        {
            s.dwMinute = atoi(s8StrTmp);
            s32I++;
            s32J = 0;
            memset(s8StrTmp, 0, 10);
            continue;
        }
        else if (19 == s32I)
        {
            s.dwSecond = atoi(s8StrTmp);
            s32I += 2;
            s32J = 0;
            memset(s8StrTmp, 0, 10);
            continue;
        }
        else if (24 == s32I)
        {
            e.dwYear = atoi(s8StrTmp);
            s32I++;
            s32J = 0;
            memset(s8StrTmp, 0, 10);
            continue;
        }
        else if (26 == s32I)
        {
            e.dwMonth = atoi(s8StrTmp);
            s32I++;
            s32J = 0;
            memset(s8StrTmp, 0, 10);
            continue;
        }
        else if (28 == s32I)
        {
            e.dwDay = atoi(s8StrTmp);
            s32I += 2;
            s32J = 0;
            memset(s8StrTmp, 0, 10);
            continue;
        }
        else if (31 == s32I)
        {
            e.dwHour = atoi(s8StrTmp);
            s32I++;
            s32J = 0;
            memset(s8StrTmp, 0, 10);
            continue;
        }
        else if (33 == s32I)
        {
            e.dwMinute = atoi(s8StrTmp);
            s32I++;
            s32J = 0;
            memset(s8StrTmp, 0, 10);
            continue;
        }
        else if (35 == s32I)
        {
            e.dwSecond = atoi(s8StrTmp);
            s32I++;
            s32J = 0;
            memset(s8StrTmp, 0, 10);
            continue;
        }
        s32I++;
        s32J++;
    }
}

void
DahuaRecordGu::onTimer()
{
    do
    {
        if (-1 != m_recordID)
        {
            uint64_t tmNow = sys->currentTimeMillis();            

            /* Check pos every 1000 milliseconds */
            if (tmNow - m_prevTime >= 1000)
            {
                
                m_prevTime = tmNow;
                printf("pos:%d\n", m_pos);

                notifyPrivStream(&m_pos, sizeof(m_pos), ENCODE_H264, TAG_POSI);
            }
        }
    }
    while (0);

    RecordGu::onTimer();
}

int
DahuaRecordGu::play(string scale, string range)
{

    LOGE_MSV("DahuaRecordGu::play:%s\n", scale.c_str());

    if ("1" == scale)
    {
        m_interval = 40;
    }
    else if ("2" == scale)
    {
        m_interval = 20;
    }
    else if ("4" == scale)
    {
        m_interval = 10;
    }
    else if ("8" == scale)
    {
        m_interval = 5;
    }
    else if ("16" == scale)
    {
        m_interval = 2;
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

    if (scale != "")
    {
        if (m_scale == scale)
        {
            return 0;
        }
        if (m_scale > scale)
        {
            LOGE_MSV("DahuaRecordGu::CLIENT_SlowPlayBack, %s-%s", m_scale.c_str(), scale.c_str());
            CLIENT_SlowPlayBack(m_recordID);
        }
        else
        {
            LOGE_MSV("DahuaRecordGu::CLIENT_FastPlayBack, %s-%s", m_scale.c_str(), scale.c_str());
            CLIENT_FastPlayBack(m_recordID);
        }
        m_scale = scale;
    }

    return 0;
}
