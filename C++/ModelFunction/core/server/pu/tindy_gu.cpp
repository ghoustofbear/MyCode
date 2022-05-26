

#include "tindy_gu.h"
#include "tls_error.h"
#include "session.h"
#include "frame.h"
#include "gumgr.h"
#include "tindy_pu.h"

static FILE* ptrFile = NULL;


void CBK
TindyRealGu::realdatacb(unsigned int _ulID, unsigned int _ulStreamType, unsigned char *_cData, int _iLen, void* _iUser, void* _iUserData)
{
    ASSERT(_cData);

#if 0
    if (NULL == ptrFile)
    {
        ptrFile = fopen("realdatacb.log", "wb+");
    }

    if (_iLen == 88)
    {
        return;
    }
    fwrite(_cData + 32, sizeof(char), _iLen - 32 -320, ptrFile);
    fflush(ptrFile);
#endif

    TindyRealGu *pGu = (TindyRealGu *)_iUserData;

    pGu->notifyPrivStream((void*)(_cData), _iLen, ENCODE_H264, TAG_TIANDY);
    //pGu->notifyVideoStream(_cData + 32, _iLen - 32 - 320, ENCODE_H264, ENCODE_H264, 1280, 720, 25, sys->getTimeMillis());

    pGu->traffic(_iLen);
    pGu->m_lastpts = sys->currentTimeMillis();

}




void CBK
TindyRealGu::rawdatacb(unsigned int  realID, uint8_t * data, int  size, RAWFRAME_INFO * _pRawFrameInfo, void *userData)
{
    ASSERT(data);

#if 0
    static FILE *fp = NULL;
    if (fp == NULL)
    {
        fp = fopen("realdata_std.h264", "wb");
    }

    fwrite(data, size, 1, fp);
    fflush(fp);
#endif


    TindyRealGu   *gu = (TindyRealGu *)userData;

    uint64_t pts = sys->currentTimeMillis();
    switch (_pRawFrameInfo->nType)
    {
    case VI_FRAME:
    case VP_FRAME:
    {
        int iEncode   = ENCODE_UNKNOW;
        char framType = 'I';

        if (_pRawFrameInfo->nType == VI_FRAME)
        {
            framType = 'I';
        }
        else
        {
            framType = 'P';
        }


        switch (_pRawFrameInfo->nEnCoder)
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
            break;
        } 

        if (iEncode != ENCODE_UNKNOW)
        {

#if 0
            static FILE *fp = NULL;
            if (fp == NULL)
            {
                fp = fopen("realdata_std_me.h264", "wb");
            }

            fwrite(data, size, 1, fp);
            fflush(fp);
#endif

            gu->notifyVideoStream(data,
                size,
                iEncode,
                framType,
                _pRawFrameInfo->nWidth,
                _pRawFrameInfo->nHeight,
                _pRawFrameInfo->nFrameRate,
                pts);
        }
        
        break;
    }
    case  AUDIO_FRAME:
    {
        gu->notifyAudioStream(data, size, ENCODE_G711U, 8000, 1, 16, pts);
        break;
    }
    default:
        break;
    }
    gu->traffic(size);
    gu->m_lastpts = pts;
}


TindyRealGu::TindyRealGu(Pu * pu, int channelNo, string subHost, int subPort, int streamType, int streamProtoType)
    : RealGu(pu, channelNo, subHost, subPort, streamType, streamProtoType)
{
    m_realID = -1;
}

TindyRealGu::TindyRealGu(Pu *pu, string addrcode, int streamType, int streamProtoType)
    : RealGu(pu, addrcode, streamType, streamProtoType)
{
    m_realID = -1;
}

TindyRealGu::~TindyRealGu()
{
}

int
TindyRealGu::start(int offset)
{
    if (!m_pu->online())
    {
        ERRE_MSV("device ip: %s is not on-line.", m_pu->ip().c_str());

        return -1;
    }


    if (!m_pu->validate(m_channelNo))
    {
        ERRE_MSV("device ip: %s, chn: %d, stream:%d  is not exist.", m_pu->ip().c_str(), m_channelNo, m_streamType);
        return -1;
    }

    CLIENTINFO clientInfo;
    memset(&clientInfo, 0, sizeof(clientInfo));
    clientInfo.m_iChannelNo = m_channelNo;
    clientInfo.m_iNetMode   = NETMODE_TCP;
    clientInfo.m_iStreamNO  = m_streamType;//  SUB_STREAM:MAIN_STREAM;
    clientInfo.m_iServerID  = ((TindyPu*)m_pu)->getUserID();


    int ret = NetClient_StartRecv_V4((unsigned int *)&m_realID, &clientInfo, 0, (void *)this);
    if (ret < 0)
    {
        ERRE_MSV("NetClient_StartRecv_V4 faild");
        return -1;
    }


    if (m_streamProtoType == 0)
    {
        NetClient_SetFullStreamNotify_V4(m_realID, realdatacb, (void *)this);
    }
    else
    {
        NetClient_SetRawFrameCallBack(m_realID, rawdatacb, (void *)this);
    }

    NetClient_ForceIFrame((int)((TindyPu*)m_pu)->getUserID(), 0, m_streamType);

    realGuMgr->addGu(this);

    return 0;
}

void
TindyRealGu::stop()
{
    realGuMgr->delGu(this);

    if (-1 != m_realID)
    {
        NetClient_SetFullStreamNotify_V4(m_realID, NULL, 0);
        NetClient_SetRawFrameCallBack(m_realID, NULL, 0);
        NetClient_StopRecv(m_realID);
    }

    m_realID = -1;
}

void
TindyRealGu::timerdriver()
{
    Gu::onTimer();
}

void *
TindyRealGu::threadEntry(ThreadInfo * info)
{
    return NULL;

 //   TindyRealGu * gu = (TindyRealGu*)(info->data);

    //char buf[MAX_UT_CHANNEL_NUM * 1024];
 //   return NULL;
 //   while (gu->m_threadTest.active())
 //   {
 //       uint32_t r = sys->rand();
 //       r = r % (1 * 1024);
 //       if (r < 256)
 //       {
 //           r = 256;
 //       }

 //       gu->notifyPrivStream((void *)buf, r, ENCODE_H264, TAG_TIANDY);
 //       sys->sleep(80);
 //   }

 //   return NULL;
}


