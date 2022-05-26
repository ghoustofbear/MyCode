#include "helmet_gx_record_gu.h"
#include "helmet_interface.h"
#include "pu.h"
#include "helmet_gx_pu.h"
#include "frame.h"
#include "tls_error.h"
#include "gumgr.h"
#include "streamlistener.h"

HelmetGXRecordGu::HelmetGXRecordGu(Pu *pu, string filename, string subHost, int subPort, int streamType, int mode)
	: RecordGu(pu, filename, subHost, subPort, streamType, mode)
{
    m_lRealHandle = NULL;
    m_bGetFrame = false;
    m_bStop = false;
}

HelmetGXRecordGu::HelmetGXRecordGu(Pu *pu, string filename, string addrcode, int streamType, int mode)
	: RecordGu(pu, filename, addrcode, streamType, mode)
{
    m_lRealHandle = NULL;
    m_bGetFrame = false;
    m_bStop = false;
}

HelmetGXRecordGu::HelmetGXRecordGu(Pu *pu, int channelNo, int type, datetime_s &start, datetime_s &end, string subHost, int subPort, int streamType, int mode)
	: RecordGu(pu, channelNo, type, start, end, subHost, subPort, streamType, mode)
{
    m_lRealHandle = NULL;
    m_bGetFrame = false;
    m_bStop = false;
}

HelmetGXRecordGu::HelmetGXRecordGu(Pu *pu, string addrcode, int type, datetime_s &start, datetime_s &end, int streamType, int mode)
    : RecordGu(pu, addrcode, type, start, end, streamType, mode)
{
    m_lRealHandle = NULL;
    m_bGetFrame = false;
    m_bStop = false;
}


HelmetGXRecordGu::~HelmetGXRecordGu()
{
    
}

int 
HelmetGXRecordGu::start(int offset)
{ 	
    if (m_lRealHandle != NULL)
    {
        return -1;
    }

    HelmetGXPu *pPu = (HelmetGXPu*)m_pu;
    int ret = -1;

    if (m_byname)
    {
        size_t spos = m_filename.find_last_of(':');
        string storageId = m_filename.substr(spos+1);
        string fileName = m_filename.substr(0, spos);

        ret = IC_CSSVODFile(pPu->m_lUserID, storageId.c_str(), fileName.c_str(), 1, 0, 0, 32, m_szVodID, &m_lRealHandle);
    }
    else
    {
        struct tm stmTimeBegin;
        memset(&stmTimeBegin, 0, sizeof(stmTimeBegin));
        stmTimeBegin.tm_year = m_start.year - 1900;
        stmTimeBegin.tm_mon  = m_start.month - 1;
        stmTimeBegin.tm_mday = m_start.day;
        stmTimeBegin.tm_hour = m_start.hour;
        stmTimeBegin.tm_min  = m_start.minute;
        stmTimeBegin.tm_sec  = m_start.second;
        time_t tmTimeBegin = mktime(&stmTimeBegin);

        struct tm stmTimeEnd;
        memset(&stmTimeEnd, 0, sizeof(stmTimeEnd));
        stmTimeEnd.tm_year = m_end.year - 1900;
        stmTimeEnd.tm_mon  = m_end.month - 1;
        stmTimeEnd.tm_mday = m_end.day;
        stmTimeEnd.tm_hour = m_end.hour;
        stmTimeEnd.tm_min = m_end.minute;
        stmTimeEnd.tm_sec = m_end.second;
        time_t tmTimeEnd = mktime(&stmTimeEnd);


        IC_HANDLE hdl = pPu->getHandle(m_channelNo, "IV");
        if (hdl != NULL)
        {
            ret = IC_CSSVODTime(pPu->m_lUserID, hdl, tmTimeBegin, tmTimeEnd, 8, m_szVodID, &m_lRealHandle);
        }
    }

    if (0 == ret)
    {
        m_bGetFrame = true;
        m_thread.setEntry(threadEntry, this, 0);
        m_thread.start();
    }

    return ret;

}

void 
HelmetGXRecordGu::stop()
{	
    if (NULL != m_lRealHandle)
    {
        m_bGetFrame = false;
        IC_StopStream(m_lRealHandle);
        m_lRealHandle = NULL;
        m_thread.stop();
        m_bStop = true;
    }
}

int
HelmetGXRecordGu::pause()
{
    if (m_pause || m_bStop)
    {
        return 0;
    }



    m_pause = true;
    m_bGetFrame = false;
   

    //HelmetGXPu *pPu = (HelmetGXPu*)m_pu;
    //IC_CSSVODPause(pPu->m_lUserID, m_szVodID);
    return 0;
}

int
HelmetGXRecordGu::continuex()
{
    if (!m_pause || m_bStop)
    {
        return 0;
    }

    m_bGetFrame = true;
    m_pause = false;


    //HelmetGXPu *pPu = (HelmetGXPu*)m_pu;
    //IC_CSSVODResume(pPu->m_lUserID, m_szVodID);

    return 0;
}

int
HelmetGXRecordGu::startRepos()
{
    return -1;
}

int
HelmetGXRecordGu::setPos(int pos)
{
    return -1;
}

int
HelmetGXRecordGu::setTime(datetime_s &t)
{
    return -1;
}

void
HelmetGXRecordGu::VideoGatherLoop()
{
    int s32Ret = 0;
    int nLen = 1024 * 1024;
    char  *s8FrameBuf = new char[1024 * 1024];
    memset(s8FrameBuf, 0, nLen);

    while (m_thread.active())
    {
        int nFrmType = 0;
        int nKeyFrmFlag = 0;

        if (NULL != m_lRealHandle)
        {
            if (!m_bGetFrame)
            {
                sys->sleep(100);
                continue;
            }
                

            s32Ret = IC_ReceiveFrame(m_lRealHandle, s8FrameBuf, &nLen, &nFrmType, &nKeyFrmFlag);
            if (0 == s32Ret)
            {
                m_lastpts = sys->currentTimeMillis();
                RealDataPushBuffer(s8FrameBuf, nLen, nFrmType);
                RawDataPushBuffer(s8FrameBuf, nLen, nFrmType, nKeyFrmFlag);
                //printf("helmet len:%d\n", nLen);
            }
        }

        sys->sleep(1);
    }
    delete[]s8FrameBuf;
}


void HelmetGXRecordGu::RealDataPushBuffer(char * s8Buffer, uint32_t u32Len, uint32_t frameType)
{
    if (FRAME_TYPE_VIDEO == frameType)
    {

        notifyPrivStream(s8Buffer, u32Len, ENCODE_H264, TAG_HELMET_GX, 'I');
    }
    else
    {
        notifyPrivStream(s8Buffer, u32Len, ENCODE_H264, TAG_HELMET_GX, 'A');
    }
}

void HelmetGXRecordGu::RawDataPushBuffer(char * s8Buffer, uint32_t u32Len, uint32_t frameType, uint32_t nKeyFrmFlag)
{
    if (FRAME_TYPE_VIDEO == frameType)
    {
        int w = FM_GetVideoWidth(s8Buffer);
        int h = FM_GetVideoHeight(s8Buffer);

        int ftype;
        if (1 == nKeyFrmFlag)
        {
            ftype = 'I';
        }
        else
        {
            ftype = 'P';
        }

        notifyVideoStream((void *)(s8Buffer + 40), u32Len - 40, ENCODE_H264, ftype, w, h, 25, m_lastpts);
        traffic(u32Len - 40);
    }
    else if (FRAME_TYPE_AUDIO == frameType)
    {
        FM_AUDIO_INFO audiohead;
        FM_GetAudioInfo(s8Buffer, &audiohead);
        int audiolen = FM_GetAudioDataLen(s8Buffer, 0);
        char * audiodata = new char[audiolen];
        if (NULL != audiodata)
        {
            FM_GetAudioData(s8Buffer, u32Len, audiodata, audiolen);
            notifyAudioStream(audiodata, audiolen, ENCODE_G711A, audiohead.simple, audiohead.channel, audiohead.bits, sys->currentTimeMillis());
            delete[] audiodata;
        }
    }

}



void* 
HelmetGXRecordGu::threadEntry(ThreadInfo *arg)
{
    HelmetGXRecordGu * hmgu = (HelmetGXRecordGu *)arg->data;

    switch (arg->id)
    {
    case 0:
        hmgu->VideoGatherLoop();
        break;

    default:
        break;
    }

    return NULL;
}

void
HelmetGXRecordGu::onTimer()
{
    if (NULL != m_lRealHandle)
    {
        if (m_repos || m_bStop)
        {
            return;
        }

        time_t tmNow = time(NULL);
        int pos = 0;
        /* Check pos every 1000 milliseconds */
        if (tmNow - m_prevTime >= 1)
        {
            m_prevTime = tmNow;

            notifyPrivStream(&pos, sizeof(pos), ENCODE_H264, TAG_POSI);
            notifyVideoStream(&pos, sizeof(pos), ENCODE_H264, 0, 0, 0, 0, 0, TAG_POSI);
        }
    }

    //Gu::onTimer();
    RecordGu::onTimer();
}
