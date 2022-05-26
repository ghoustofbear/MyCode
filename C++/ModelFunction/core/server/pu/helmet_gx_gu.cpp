#include "helmet_gx_gu.h"
#include "helmet_interface.h"
#include "pu.h"
#include "helmet_gx_pu.h"
#include "frame.h"
#include "tls_error.h"
#include "gumgr.h"
#include "streamlistener.h"


HelmetGXGu::HelmetGXGu(Pu * pu, int channelNo, string subHost, int subPort, int streamType, int streamPotoType)
    : RealGu(pu, channelNo, subHost, subPort, streamType, streamPotoType)
{
    
    m_lRealHandle = NULL;
    m_lAudioHandle = NULL;
    m_dualtalkHandle = NULL;
}

HelmetGXGu::HelmetGXGu(Pu *pu, string addrcode, int streamType, int streamPotoType)
    : RealGu(pu, addrcode, streamType, streamPotoType)
{
    
    m_lRealHandle = NULL;
    m_lAudioHandle = NULL;
    m_dualtalkHandle = NULL;
}

HelmetGXGu::~HelmetGXGu()
{
    
}

int 
HelmetGXGu::start(int offset)
{ 	
    if (!m_pu->online())
    {
        ERRE_MSV("device ip: %s is not on-line.", m_pu->ip().c_str());
        return -1;
    }
    
    if (LiveStart() < 0)
    {
        return -1;
    }

    m_thread.setEntry(threadEntry, this, 0);
    m_thread.start();

    realGuMgr->addGu(this);
    
    return 0;
}

void 
HelmetGXGu::stop()
{	
    realGuMgr->delGu(this);
    LiveStop();
    m_thread.stop();
}

int
HelmetGXGu::reset()
{

    LiveStop();
    if (0 == LiveStart())
    {
        ERRE_MSV("HelmetGXGu success success success success");
        m_lastpts = sys->currentTimeMillis(); 
        m_status = StreamListener::S_GU_REONLINE;
    }
    else
    {
        ERRE_MSV("HelmetGXGu failed failed failed failed failed");
        
    }

    return 0;
}

int
HelmetGXGu::startDualTalk()
{
    HelmetGXPu* cpu = (HelmetGXPu*)m_pu;
    if (!cpu->validate(m_channelNo))
    {
        ERRE_MSV("HelmetGXGu device ip: %s, chn: %d, stream:%d  is not exist.", m_pu->ip().c_str(), m_channelNo, m_streamType);
        return -1;
    }

    int s32Ret = -1;
    IC_HANDLE thandle = cpu->getHandle(m_channelNo, "OA");
    if (thandle != NULL)
    {
        s32Ret = IC_StartTalk(thandle, &m_dualtalkHandle);
    }
    return s32Ret;
}

void
HelmetGXGu::stopDualTalk()
{
    if (m_dualtalkHandle)
    {
        IC_StopStream(m_dualtalkHandle);
        m_dualtalkHandle = NULL;
    }
}

int HelmetGXGu::LiveStart()
{
    HelmetGXPu* cpu = (HelmetGXPu*)m_pu;
    if (!cpu->validate(m_channelNo))
    {
        ERRE_MSV("HelmetGXGu device ip: %s, chn: %d, stream:%d  is not exist.", m_pu->ip().c_str(), m_channelNo, m_streamType);
        return -1;
    }

    int s32Ret = -1;
    
    
    IC_HANDLE vhandle = cpu->getHandle(m_channelNo, "IV");
    if (vhandle != NULL)
    {	
        s32Ret = IC_StartStream(vhandle, 0, &m_lRealHandle);
        if (0 != s32Ret)
        {
            return -1;
        }
        else
        {
            return 0;
        }
    }
    else
    {
        return -1;
    }

    //IC_HANDLE ahandle = cpu->getHandle(m_channelNo, "IA");
    //if (ahandle != NULL)
    //{
    //    s32Ret = IC_StartStream(ahandle, 0, &m_lAudioHandle);
    //}
}

int HelmetGXGu::LiveStop()
{
    int s32Ret = -1;
    
    if (NULL != m_lRealHandle)
    {
        IC_StopStream(m_lRealHandle);
        m_lRealHandle = NULL;
    }
    //if (NULL != m_lAudioHandle)
    //{
    //    IC_StopStream(m_lAudioHandle);
    //    m_lAudioHandle = NULL;
    //}

    return s32Ret;
}

void
HelmetGXGu::VideoGatherLoop()
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
            s32Ret = IC_ReceiveFrame(m_lRealHandle, s8FrameBuf, &nLen, &nFrmType, &nKeyFrmFlag);
            if (0 == s32Ret)
            {
                m_lastpts = sys->currentTimeMillis();
                RealDataPushBuffer(s8FrameBuf, nLen, nFrmType);
                RawDataPushBuffer(s8FrameBuf, nLen, nFrmType, nKeyFrmFlag);
            }
        }

        //if (NULL != m_lAudioHandle)
        //{
        //    s32Ret = IC_ReceiveFrame(m_lAudioHandle, s8FrameBuf, &nLen, &nFrmType, &nKeyFrmFlag);
        //    if (0 == s32Ret)
        //    {
        //        //RealDataPushBuffer(s8FrameBuf, nLen, nFrmType);
        //        RawDataPushBuffer(s8FrameBuf, nLen, nFrmType, nKeyFrmFlag);
        //        printf("a ba,size:%d\n", nLen);
        //    }
        //}

        if (NULL != m_dualtalkHandle)
        {
            s32Ret = IC_ReceiveFrame(m_dualtalkHandle, s8FrameBuf, &nLen, &nFrmType, &nKeyFrmFlag);
            if (0 == s32Ret)
            {
                //RealDataPushBuffer(s8FrameBuf, nLen, nFrmType);
                //RawDataPushBuffer(s8FrameBuf, nLen, nFrmType, nKeyFrmFlag);
                
            }
        }

        sys->sleep(1);
    }
    delete[]s8FrameBuf;
}

void *
HelmetGXGu::threadEntry(ThreadInfo * info)
{
    HelmetGXGu * hmgu = (HelmetGXGu *)info->data;

    switch (info->id)
    {
    case 0:
        hmgu->VideoGatherLoop();
        break;

    default:
        break;
    }

    return NULL;
}



void HelmetGXGu::RealDataPushBuffer(char * s8Buffer, uint32_t u32Len, uint32_t frameType)
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
    
void HelmetGXGu::RawDataPushBuffer(char * s8Buffer, uint32_t u32Len, uint32_t frameType, uint32_t nKeyFrmFlag)
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
            delete [] audiodata;
        }
    }	
    
}


void
HelmetGXGu::onTimer()
{

    Gu::onTimer();
}

