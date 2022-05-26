#include "preview.h"
#include "soundtalk.h"
#include "rtpsession.h"
#include "frame.h"
#include "gb28181_common.h"

#define GB28181_AUDIO_SDP_FORMAT \
    "v=0\r\n" \
    "o=%s 0 0 IN IP4 %s\r\n" \
    "s=Play\r\n" \
    "c=IN IP4 %s\r\n" \
    "t=0 0\r\n" \
    "m=audio %d RTP/AVP 8\r\n" \
    "a=rtpmap:8 PCMA/8000\r\n" \
    "a=sendrecv\r\n" \
    "y=%s\r\n" \
    "f=v/////a/1/8/1\r\n\r\n"

SoundTalk::SoundTalk(GB28181_SipUA *pSipUa, preview * stream)
{
    m_iConId     = -1;
    m_iDialId    = -1;
    m_hSound     = NULL;
    m_pSipUa     = pSipUa;
    m_preview    = stream;
    m_rtpSession = new RTPSessionEx();
}

SoundTalk::~SoundTalk()
{
    stop();
}

void cap_audio(int type, void *data, int size, uint64_t pts, void *userdata)
{
    SoundTalk *pTalk = (SoundTalk*)userdata;
    if (pTalk && 
        pTalk->m_rtpSession)
    {
        pTalk->m_rtpSession->sendByFU(ENCODE_G711A, 'A', data, size, 0);
    }
}

bool SoundTalk::start(const char *tohost, int toport)
{
    if (!m_pSipUa || !m_rtpSession 
        ||!m_preview)
    {
        return false;
    }
    string cameracode = m_preview->camCode();
    string ssrc = GenerateSSRC(m_pSipUa->addrcode());
    string resp;
    char sdp[1024] = { 0 };
    sprintf(sdp, GB28181_AUDIO_SDP_FORMAT, cameracode.c_str(),m_pSipUa->ip().c_str(), m_pSipUa->ip().c_str(), m_rtpSession->srcport(),ssrc.c_str());
    int ret = m_pSipUa->invite(cameracode, tohost, toport, sdp, resp);
    if (ret < 0)
    {
        return false;
    }
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;
    if (!reader.parse(resp, rvalue, false))
    {
        return  false;
    }
    
    m_iConId     = atoi(rvalue["cid"].asString().c_str());
    m_iDialId    = atoi(rvalue["did"].asString().c_str());
    int destport = rvalue["audioport"].asInt();
    m_rtpSession->setSsrc(rvalue["ssrc"].asString());

    ret = m_rtpSession->addDestination(tohost, destport, this);
    if (ret < 0)
    {
        m_pSipUa->bye(m_iConId, m_iDialId);
        m_iConId = -1;
        m_iDialId = -1;
        return false;
    }
    m_rtpSession->setstreamcallback(stream_cb, this);

    //start local sound capture
    cap_audio_s encode = { 0 };
    encode.sps = 8000;
    encode.channels = 1;
    encode.bps = 16;
    encode.ssize = 320;

    m_hSound = CAP_start_audio(NULL, &encode, 4, cap_audio, this);//m_mode:1-PCM,2-G711A,4-G711U
    if (NULL == m_hSound)
    {
        m_pSipUa->bye(m_iConId, m_iDialId);
        m_iConId = -1;
        m_iDialId = -1;
        return false;
    }

    return true;
}

void SoundTalk::stop()
{
    if (NULL != m_hSound)
    {
        CAP_stop_audio(m_hSound);
        m_hSound = NULL;
    }

    if (-1 != m_iConId || -1 != m_iDialId)
    {
        m_pSipUa->bye(m_iConId, m_iDialId);
        m_iConId = -1;
        m_iDialId = -1;
    }
    
    if (NULL != m_rtpSession)
    {
        m_rtpSession->stop();
        delete m_rtpSession;
        m_rtpSession = NULL;
    }
}

//FILE *file = fopen("ps.dat", "wb");
void SoundTalk::stream_cb(const uint8_t * stream, uint32_t size, uint8_t type, void *puser)
{
    SoundTalk * pthis = (SoundTalk*)puser;
    if (pthis)
    {
        if (0 == type)     //H264
        {

        }
        else if (1 == type)//G711
        {
        }
        else if (2 == type)//PS
        {
            preview::stream_cb(stream, size, type, pthis->m_preview);
            //fwrite(stream, size, 1, file);
        }
    }
}

void SoundTalk::onHeartbeat()
{

}

void SoundTalk::onClose()
{

}