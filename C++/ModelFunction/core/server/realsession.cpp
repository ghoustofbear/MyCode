#include "realsession.h"
#include "putree.h"
#include "pu.h"
#include "socket.h"
#include "bufferev.h"
#include "frame.h"
#include "mempool.h"

#include "json/json.h"

#include "su.h"


RealSession::RealSession(Client *client)
{
    m_client = client;
    m_pu = NULL;
    m_su = NULL;
    m_seq    = 0;
}

RealSession::~RealSession()
{
    //RealSession::stop();
}

int
RealSession::start(string json, int mode)
{
    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!reader.parse(json, rvalue, false))
    {
        return -1;
    }

    m_channelNo  = rvalue["channelNo"].asInt();
    m_obtainType = rvalue["obtainType"].asInt();
    m_streamType = rvalue["streamType"].asInt();
    m_transport  = rvalue["transport"].asInt();


    int result = -1;
    /*
    m_obtainType
    0:IP/PORT+Ë½ÓÐÂëÁ÷
    1:IP/PORT+±ê×¼ÂëÁ÷ 
    2:µØÖ·Âë+Ë½ÓÐÂëÁ÷, 
    3:µØÖ·Âë+±ê×¼ÂëÁ÷
    */

    
    if (m_obtainType == 0 || m_obtainType == 2)
    {
        m_streamProtoType = 0;
    }
    else
    {
        m_streamProtoType = 1;
    }

    if (0 == m_obtainType || 1 == m_obtainType)
    {
        m_deviceHost  = rvalue["deviceHost"].asString();
        m_devicePort  = rvalue["devicePort"].asInt();


        m_subServHost = rvalue["subServHost"].asString();
        m_subServPort = rvalue["subServPort"].asInt();

        if (m_subServPort != 0)
        {
			m_pu = putree->findPu(m_subServHost, m_subServPort);
        }
        else
        {
			m_pu = putree->findPu(m_deviceHost, m_devicePort);
			string chaCode = putree->findCode(m_deviceHost, m_devicePort, m_channelNo);
			if (chaCode == "")
			{
				return -1;
			}
        }


        if (!m_pu)
        {
            return -1;
        }

        if (Pu::PROTO_TYPE_UT == m_pu->protocol())
        {
            //m_addrcode = putree->findCode(m_deviceHost, m_devicePort, m_channelNo);
            result = m_pu->addRealGu(m_channelNo, m_deviceHost, m_devicePort, m_streamType, m_streamProtoType, this);
        }
        else
        {
            result = m_pu->addRealGu(m_channelNo, m_subServHost, m_subServPort, m_streamType, m_streamProtoType, this);
        }
    }
    else if (2 == m_obtainType || 3 == m_obtainType)
    {
        m_addrcode = rvalue["addrcode"].asString();
        m_pu = putree->findPu(m_addrcode);
        if (!m_pu)
        {
            return -1;
        }

        result = m_pu->addRealGu(m_addrcode, m_streamType, m_streamProtoType, this);
    }
    else
    {

    }

    if (0 == result)
    {
        m_time = sys->currentTime();
    }

    return result;
}

void
RealSession::stop()
{
    do
    {
        if (m_pu == NULL)
        {
            return ;
        }

        if (0 == m_obtainType || 1 == m_obtainType)
		{
			//if ((m_pu = putree->findPu(m_deviceHost, m_devicePort)) || (m_pu = putree->findPu(m_subServHost, m_subServPort)))
			//{
				if (Pu::PROTO_TYPE_UT == m_pu->protocol())
				{
                    m_pu->delRealGu(m_channelNo, m_deviceHost, m_devicePort, m_streamType, m_streamProtoType, this);
				}
				else
				{
                    m_pu->delRealGu(m_channelNo, m_subServHost, m_subServPort, m_streamType, m_streamProtoType, this);
				}
			//}
        }
        else if (2 == m_obtainType || 3 == m_obtainType)
        {
            //Pu *pu = putree->findPu(m_addrcode);
            //if (!pu)
            //{
            //    break;
            //}

            m_pu->delRealGu(m_addrcode, m_streamType, m_streamProtoType, this);
        }

        if (m_su)
        {
            m_pu->delDualTalkGu(m_su);
            m_su = NULL;
        }

    }
    while (0);
}

int
RealSession::stream()
{
    if (0 == m_obtainType || 2 == m_obtainType)
    {
        /* 0-private stream */
        return 0;
    }
    else if (1 == m_obtainType || 3 == m_obtainType)
    {
        /* 1-standard h.264 stream */
        return 1;
    }
    else
    {
        return -1;
    }
}

int
RealSession::transport()
{
    return m_transport;
}

string
RealSession::peerip()
{
    if (m_bev)
    {
        return m_bev->peerip();
    }

    return "";
}

int
RealSession::peerport()
{
    if (m_bev)
    {
        return m_bev->peerport();
    }

    return 0;
}

string
RealSession::peertime()
{
    return m_time;
}

int
RealSession::streamtype()
{
    return m_streamType;
}

void
RealSession::onVideoStream(void *stream, uint32_t size, int encode, int type, int w, int h, int fps, uint64_t pts, uint32_t tag)
{
    if (!(1 == m_obtainType || 3 == m_obtainType))
    {
         return;
    }

//#ifdef MEMPOOL
//    uint8_t *buf = (uint8_t *)mempool->alloc(size + sizeof(ut_frame_s));;
//#else
//    uint8_t *buf = (uint8_t *)malloc(size + sizeof(ut_frame_s));
//#endif
//    if (!buf)
//    {
//        return;
//    }
//
    ut_frame_s *hdr = (ut_frame_s *)m_sfrbuf;
    memcpy(hdr->buf, stream, size);

    hdr->tag          = tag;

    hdr->type         = type;
    hdr->encode       = encode;
    hdr->channel      = m_channelNo;
    hdr->encrypt      = 0;

    hdr->size         = size;
    hdr->pts          = pts;
    hdr->time         = pts;
    hdr->seq          = m_seq++;

    hdr->video.width  = w;
    hdr->video.height = h;
    hdr->video.fps    = fps;
    hdr->video.system = 'N';

    onResponsePacket(m_bev, m_sfrbuf, hdr->size + sizeof(ut_frame_s));

    //mempool->free(buf);
}

void
RealSession::onAudioStream(void *stream, uint32_t size, int encode, int sps, int channels, int bps, uint64_t pts)
{
    if (!(1 == m_obtainType || 3 == m_obtainType))
    {
        return;
    }

//#ifdef MEMPOOL
//    uint8_t *buf = (uint8_t *)mempool->alloc(size + sizeof(ut_frame_s));;
//#else
//    uint8_t *buf = (uint8_t *)malloc(size + sizeof(ut_frame_s));
//#endif
//    if (!buf)
//    {
//        return;
//    }

    ut_frame_s *hdr = (ut_frame_s *)m_sfrbuf;
    memcpy(hdr->buf, stream, size);

    hdr->tag            = TAG_STD;

    hdr->type           = 'A';
    hdr->encode         = encode;
    hdr->channel        = m_channelNo;
    hdr->encrypt        = 0;

    hdr->size           = size;
    hdr->pts            = pts;
    hdr->time           = pts;
    hdr->seq            = m_seq++;

    hdr->audio.sps      = sps;
    hdr->audio.channels = channels;
    hdr->audio.bps      = bps;
    hdr->audio.packLen  = size;

    onResponsePacket(m_bev, m_sfrbuf, hdr->size + sizeof(ut_frame_s));

    //mempool->free(buf);
}

void
RealSession::onPrivStream(void *stream, uint32_t size, int encode, uint32_t tag, int type)
{
    //if (!(0 == m_obtainType || 2 == m_obtainType))
    //{
    //    return;
    //}

//#ifdef MEMPOOL
//    uint8_t *buf = (uint8_t *)mempool->alloc(size + sizeof(ut_frame_s));
//#else
//    uint8_t *buf = (uint8_t *)malloc(size + sizeof(ut_frame_s));
//#endif
//    if (!buf)
//    {
//        return;
//    }

    ut_frame_s *hdr = (ut_frame_s *)m_pfrbuf;
    memcpy(hdr->buf, stream, size);

    hdr->tag          = tag;
    hdr->type         = type;
    hdr->encode       = encode;
    hdr->channel      = m_channelNo;
    hdr->encrypt      = 0;

    hdr->size         = size;
    hdr->pts          = sys->currentTimeMillis();
    hdr->time         = hdr->pts;
    hdr->seq          = m_seq++;

    hdr->video.width  = 0;
    hdr->video.height = 0;
    hdr->video.fps    = 0;
    hdr->video.system = 'N';

    onResponsePacket(m_bev, m_pfrbuf, hdr->size + sizeof(ut_frame_s));

    //mempool->free(buf);
}

void
RealSession::onRead(Bufferev *bev, void *data, uint32_t size)
{
    //m_bev = bev;
    ut_frame_s *hdr = (ut_frame_s *)data;

    if (!m_pu)
    {
        return;
    }

    if (hdr->tag == TAG_STD && hdr->type == 'A')
    {
        if (m_su)
        {
            m_pu->doAudioStream(m_channelNo, data, size);
        }
    }
    else if (hdr->tag == TAG_STARTTALK)
    {
        if (m_su)
        {
            m_pu->delDualTalkGu(m_su);
            m_su = NULL;
        }

        if (!m_su)
        {
            m_su = m_pu->addDualTalkGu(m_channelNo, "", 0);

            unsigned char buf[64] = {0};

            memcpy(buf, data, sizeof(ut_frame_s));
            ut_frame_s *hdr = (ut_frame_s *)buf;
            hdr->tag = TAG_RESPONSETALK;
            hdr->size = sizeof(int);

            int ret = 0;
            if (m_su)
            {
                ret = 1;
                memcpy(hdr->buf, &ret, sizeof(int));
                onResponsePacket(m_bev, buf, hdr->size + sizeof(ut_frame_s));
            }
            else
            {
                ret = 0;
                memcpy(hdr->buf, &ret, sizeof(int));
                onResponsePacket(m_bev, buf, hdr->size + sizeof(ut_frame_s));
            }
        }       
    }
    else if (hdr->tag == TAG_STOPTALK)
    {
        printf("hdr->tag == TAG_STOPTALK");
        if (m_su)
        {
            printf(" success\n");
            m_pu->delDualTalkGu(m_su);
            m_su = NULL;
        }
    }

}

void
RealSession::onClose(Bufferev *bev)
{
    m_status = S_SOK;
    m_active = false;
    
}

void
RealSession::onStatus(int code)
{
    m_status = code;
}
