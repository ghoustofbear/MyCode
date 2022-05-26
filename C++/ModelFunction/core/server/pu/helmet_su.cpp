#include "helmet_su.h"
#include "helmet_gx_pu.h"

#include "tls_error.h"
#include "session.h"
#include "frame.h"
#include "encoder_g711a.h"
#include "sumgr.h"


HelmetSu::HelmetSu(Pu * pu, int channelNo)
    : Su(pu, channelNo)
{
    m_dualHandle = NULL;

    
    m_encBuf = new char[TMPBUFLEN];
    m_makeBuf = new char[TMPBUFLEN];
    m_useLen = 0;

    m_audio_encode = new G711AEncoder();
}

HelmetSu::HelmetSu(Pu *pu, string addrcode)
    : Su(pu, addrcode)
{
    m_dualHandle = NULL;

    m_encBuf = new char[TMPBUFLEN];
    m_makeBuf = new char[TMPBUFLEN];
    m_useLen = 0;

    m_audio_encode = new G711AEncoder();
}

HelmetSu::~HelmetSu()
{
    delete[] m_encBuf;
    m_encBuf = NULL;

    delete[] m_makeBuf;
    m_makeBuf = NULL;

    delete m_audio_encode;
    m_audio_encode = NULL;

    m_useLen = 0;
    LOGE_MSV("~HelmetSu~~");
#if 0
    if (fasdfas)
    {
        fclose(fasdfas);
        fasdfas = NULL;
    }
#endif
}

int
HelmetSu::start()
{
    HelmetGXPu* hpu = (HelmetGXPu*)m_pu;

    if (!hpu->online())
    {
        ERRE_MSV("dualtalk device ip: %s is not on-line.", m_pu->ip().c_str());
        return -1;
    }

    if (!hpu->validate(m_channelNo))
    {
        ERRE_MSV("dualtalk device ip: %s, chn: %d,   is not exist.", m_pu->ip().c_str(), m_channelNo);
        return -1;
    }

    
    IC_HANDLE aohandle = hpu->getHandle(m_channelNo, "OA");
    if (aohandle)
    {
        int ret = IC_StartTalk(aohandle, &m_dualHandle);
        if (0 != ret)
        {
            LOGE_MSV("start IC_StartTalk error code:%d", ret);
            return -1;
        }
    }

    m_thread.setEntry(threadEntry, this, 0);
    m_thread.start();

    dualtalkSuMgr->addSu(this);

    return 0;
}

void
HelmetSu::stop()
{
    if (NULL != m_dualHandle)
    {
        IC_StopStream(m_dualHandle);
        m_dualHandle = NULL;
    }

    if (m_audio_encode)
    {
        delete m_audio_encode;
        m_audio_encode = NULL;
    }

    m_thread.stop();

    dualtalkSuMgr->delSu(this);
}

int
HelmetSu::reset()
{
    if (NULL != m_dualHandle)
    {
        IC_StopStream(m_dualHandle);
        m_dualHandle = NULL;
    }

    HelmetGXPu* hpu = (HelmetGXPu*)m_pu;

    if (!hpu->online())
    {
        ERRE_MSV("dualtalk device ip: %s is not on-line.", m_pu->ip().c_str());
        return -1;
    }

    if (!hpu->validate(m_channelNo))
    {
        ERRE_MSV("dualtalk device ip: %s, chn: %d,   is not exist.", m_pu->ip().c_str(), m_channelNo);
        return -1;
    }


    IC_HANDLE aohandle = hpu->getHandle(m_channelNo, "OA");
    if (aohandle)
    {
        if (0 != IC_StartTalk(aohandle, &m_dualHandle))
        {
            LOGE_MSV("start IC_StartTalk error");
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
}

void
HelmetSu::doVideoStream(void *stream, uint32_t size)
{

}

void
HelmetSu::encode(void* srcData, int size, void* desData, int& desSize)
{
    uint64_t pts = 0;
    m_audio_encode->encode(srcData, size, desData, desSize, pts);

}

void
HelmetSu::doAudioStream(void *stream, uint32_t size, int flag)
{
    if (!m_dualHandle)
    {
        return;
    }

    m_lastpts = sys->currentTimeMillis();
        
    int tmpLen = TMPBUFLEN - m_useLen;
    if (0 == flag)
    {
        ut_frame_s* hdr = (ut_frame_s*)stream;
        ASSERT((int)(hdr->size) <= 2 * TMPBUFLEN);        
        encode(hdr->buf, hdr->size, m_encBuf + m_useLen, tmpLen);
    }
    else
    {
        ASSERT((int)(size) <= 2*TMPBUFLEN);   
        encode(stream, size, m_encBuf+m_useLen, tmpLen);
    }
    m_useLen += tmpLen;

    if (m_useLen < 480)
    {
        return;
    }

    int rest = m_useLen % 480;
    int enlen = m_useLen - rest;

    memset(m_makeBuf, 0, TMPBUFLEN);

    FM_AUDIO_INFO audioInfo = { 1, 16, 8000 };
    int offset = FM_MakeAudioFrame(m_lastpts, FM_AUDIO_ALG_G711A, m_encBuf, enlen, &audioInfo, m_makeBuf, TMPBUFLEN);
    if (offset > 0)
    {
        //printf("recv :%d\n", offset);
        offset = IC_SendFrame(m_dualHandle, m_makeBuf, offset, FRAME_TYPE_AUDIO);
        if (offset != 0)
            LOGE_MSV("IC_SendFrame error, ret %d\n", offset);
    }
    else
    {
        LOGE_MSV("FM_MakeAudioFrame error");
    }

    if (rest != 0)
    {
        memcpy(m_encBuf, m_encBuf + enlen, rest);
    }
    m_useLen = rest;
}

void
HelmetSu::doStatus(int code)
{

}



void
HelmetSu::onTimer()
{
    Su::onTimer();
}

void
HelmetSu::recvloop()
{
    int nFrmType;
    int nKeyFrmFlag;
    int nLen = 1024 * 1024;
    char *s8FrameBuf = new char[1024 * 1024];

    while (m_thread.active())
    {
        if (m_dualHandle)
        {
            while (1)
            {
                int s32Ret = IC_ReceiveFrame(m_dualHandle, s8FrameBuf, &nLen, &nFrmType, &nKeyFrmFlag);
                if (s32Ret != 0)
                {
                    if (4001 == s32Ret)
                    {
                        //LOGE_MSV("IC_ReceiveFrameIC_ReceiveFrameIC_ReceiveFrameIC_ReceiveFrame error, ret d4001\n");
                        break;
                    }
                    else
                    {
                        reset();
                        //LOGE_MSV("IC_StopStream(m_testDual); IC_StopStream(m_testDual); error, ret d4001\n");
                        break;
                    }
                }
                else
                {
                    continue;
                }
            }
            sys->sleep(100);
        }
    }
    delete[]s8FrameBuf;
}

void *
HelmetSu::threadEntry(ThreadInfo * info)
{
    HelmetSu * su = (HelmetSu*)(info->data);
    switch (info->id)
    {
    case 0:
        su->recvloop();
        break;
    default:
        break;
    }


    return NULL;
}
