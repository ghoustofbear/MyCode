#include "hik_gu.h"
#include "hik_pu.h"
#include "tls_error.h"
#include "session.h"
#include "frame.h"
#include "gumgr.h"
#include "hik_face_rgn_pu.h"
#include "hik_face_auth_pu.h"

void CBK
HikRealGu::realdatacb(LONG realID, DWORD type, BYTE *data, DWORD size, void* userData)
{
    ASSERT(userData);

    HikRealGu   *gu    = (HikRealGu *)userData;
    if (gu->m_streamProtoType != 0)
    {
        return;
    }
    //printf("p\n");
    HIK_DVR_CFG dvrCfg = ((HikPu    *)gu->m_pu)->getDvrCfg();
    if (0x10 == dvrCfg.byDVRType)
    {
        gu->notifyPrivStream((void *)data, size, ENCODE_H264, TAG_HIK);
    }
    else if (0x00 == dvrCfg.byDVRType ||
             0xbc == dvrCfg.byDVRType ||
             0x29 == dvrCfg.byDVRType ||
             0x5a == dvrCfg.byDVRType)
    {
        gu->notifyPrivStream((void *)data, size, ENCODE_H264, TAG_HIK_N);
    }
    else
    {
        gu->notifyPrivStream((void *)data, size, ENCODE_H264, TAG_HIK_N);
    }


    gu->traffic(size);
    gu->m_lastpts = sys->currentTimeMillis();
}

void CBK
HikRealGu::rawdatacb(LONG realID, DWORD type, uint8_t *data, DWORD  size, void *userData)
{
    ASSERT(userData);
    HikRealGu   *gu    = (HikRealGu *)userData;
    HIK_DVR_CFG dvrCfg = ((HikPu    *)gu->m_pu)->getDvrCfg();

    uint8_t encode = ENCODE_H264;
    if (0x5a == dvrCfg.byDVRType)
    {
        /* rtp packet */
        //gu->notifyVideoStream(data, size, encode, 'R', 0, 0, 0, sys->currentTimeMillis());

        //return;
    }

    switch (type)
    {
    case VIDEO_I_FRAME:
        LOGT_MSV("type: %02x%02x%02x%02x%02x", data[0], data[1], data[2], data[3], data[4]);
        encode = IS_HEVC_IFRAME(data) ? ENCODE_HEVC : ENCODE_H264;
        if (ENCODE_H264 == encode)
        {
            gu->getH264Resolution(data+4, size-4, gu->m_s32Height, gu->m_s32Width);
        }
        else
        {
            gu->getH265Resolution(data, size, gu->m_s32Height, gu->m_s32Width);
        }
        gu->notifyVideoStream(data, size, encode, 'I', gu->m_s32Width, gu->m_s32Height, 25, sys->currentTimeMillis());
        break;

    case VIDEO_P_FRAME:
        LOGT_MSV("type: %02x%02x%02x%02x%02x", data[0], data[1], data[2], data[3], data[4]);
        encode = IS_HEVC_PFRAME(data) ? ENCODE_HEVC : ENCODE_H264;
        gu->notifyVideoStream(data, size, encode, 'P', gu->m_s32Width, gu->m_s32Height, 25, sys->currentTimeMillis());
        break;

    case AUDIO_PACKET:
        gu->notifyAudioStream(data, size, ENCODE_G711A, 8000, 1, 16, sys->currentTimeMillis());
        break;

    default:
        /* HIKGU_DEBUG("unkonw frame type.\n"); */
        return;
        break;
    }
    gu->traffic(size);
    gu->m_lastpts = sys->currentTimeMillis();
}

void CBK
HikRealGu::rawdatacb1(LONG realID, NET_DVR_PACKET_INFO_EX *packet, void *userData)
{
    HikRealGu *gu = (HikRealGu *)userData;
    //printf("s\n");

    uint32_t type = packet->dwPacketType;
    uint32_t size = packet->dwPacketSize;

    struct tm t = {0};
    t.tm_year = packet->dwYear  - 1900;
    t.tm_mon  = packet->dwMonth - 1;
    t.tm_mday = packet->dwDay;
    t.tm_hour = packet->dwHour;
    t.tm_min  = packet->dwMinute;
    t.tm_sec  = packet->dwSecond;
    uint64_t pts   = mktime(&t) * 1000 + packet->dwMillisecond;

    switch (type)
    {
    case VIDEO_I_FRAME:
    case VIDEO_P_FRAME:
    {

        uint32_t fps   = packet->dwFrameRate;
        uint32_t frame = 'P';
        if (VIDEO_I_FRAME == type)
        {
            frame = 'I';
        }
        else
        {
            frame = 'P';
        }
        gu->notifyVideoStream(packet->pPacketBuffer, size, ENCODE_H264, frame, packet->wWidth, packet->wHeight, fps, pts);
        LOGT_MSV("type: %d, pts: %lld", type, pts);
    }
    break;

    case AUDIO_PACKET:
        gu->notifyAudioStream(packet->pPacketBuffer, size, ENCODE_G711A, 8000, 1, 16, pts);
        break;

    default:
        /* HIKGU_DEBUG("unkonw frame type.\n"); */
        return;
        break;
    }
    gu->traffic(size);
    gu->m_lastpts = sys->currentTimeMillis();
}

HikRealGu::HikRealGu(Pu * pu, int channelNo, string subHost, int subPort, int streamType ,int streamProtoType)
    : RealGu(pu, channelNo, subHost, subPort, streamType, streamProtoType)
{
    m_realID = -1;
}

HikRealGu::HikRealGu(Pu *pu, string addrcode, int streamType, int streamProtoType)
    : RealGu(pu, addrcode, streamType, streamProtoType)
{
    m_realID = -1;
}

HikRealGu::~HikRealGu()
{
}

int
HikRealGu::start(int offset)
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

    NET_DVR_PREVIEWINFO viewInfo;
    memset(&viewInfo, 0, sizeof(viewInfo));
    viewInfo.lChannel     = m_channelNo + 1;
    viewInfo.dwStreamType = m_streamType;
    viewInfo.dwLinkMode   = 0;
    viewInfo.byProtoType  = 0;
    viewInfo.bBlocked     = 0;
    viewInfo.hPlayWnd     = 0;

    if (m_streamProtoType == 0)
    {
        //private
        m_realID = NET_DVR_RealPlay_V40(((HikPu *)m_pu)->getUserID(), &viewInfo, realdatacb, this);
        if (m_realID < 0)
        {
            ERRE_MSV("device ip: %s, chn: %d NET_DVR_RealPlay_V40 fail, err %d.", m_pu->ip().c_str(), m_channelNo, NET_DVR_GetLastError());
            return -1;
        }
        //NET_DVR_SetESRealPlayCallBack( m_realID, rawdatacb1, this);
    }
    else
    {
        //standar
        m_realID = NET_DVR_RealPlay_V40(((HikPu *)m_pu)->getUserID(), &viewInfo, NULL, NULL);
        if (m_realID < 0)
        {
            ERRE_MSV("device ip: %s, chn: %d NET_DVR_RealPlay_V40 fail, err %d.", m_pu->ip().c_str(), m_channelNo, NET_DVR_GetLastError());
            return -1;
        }
        //NET_DVR_SetESCallBack(m_realID, rawdatacb, this);
        NET_DVR_SetESRealPlayCallBack( m_realID, rawdatacb1, this);
    }


    realGuMgr->addGu(this);
    return 0;
}

void
HikRealGu::stop()
{
    realGuMgr->delGu(this);
    if (-1 != m_realID)
    {
        NET_DVR_SetESRealPlayCallBack(m_realID, NULL, NULL);
        NET_DVR_StopRealPlay(m_realID);
    }
    m_realID = -1;
}

void
HikRealGu::onTimer()
{
    Gu::onTimer();
}
