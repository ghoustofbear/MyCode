#include "websock_realsession.h"
#include "websock_server.h"
#include "frame.h"
#include "bufferev.h"
#include "websock_frame.h"
#include "mempool.h"

WebSockRealSession::WebSockRealSession(Client *client, Bufferev* bev)
    : RealSession(client)
{
	assert(bev != NULL);
    m_bev = bev;
    m_bev->addBuffevListener(this);
}

WebSockRealSession::~WebSockRealSession()
{
    m_bev->delBuffevListener(this);
}

//void
//WebSockRealSession::stop()
//{
//    websockSV->delBuffevListener(m_bev, this);
//
//    RealSession::stop();
//}

void
WebSockRealSession::onRead(Bufferev *bev, void *data, uint32_t size)
{
    unsigned char * buf = (unsigned char *)data;

	for (uint32_t index = 0; index < size; ++index)
    {
        m_recvBuf.push_back(buf[index]);
    }

    do
    {
        uint8_t u8TwoChar[3] = { 0 };
        if (m_recvBuf.size() <= 1)
        {
            return;
        }

        memcpy((char*)u8TwoChar, (char*)&m_recvBuf[0], 2);

        int OpCode = u8TwoChar[0];

        //WebSocket Connection Close
        if (136 == OpCode)
        {
            RealSession::stop();
            bev->close();
        }
        else if (137 == OpCode)
        {
            //WebSocket Ping
            WebSockFrame dr = WebSockFrame("", 0);
            int nsize;
            char* msg = dr.GetBytes(nsize);
            if (NULL != msg)
            {
                bev->write(msg, nsize);
                delete[] msg;
            }
            m_recvBuf.clear();
        }
        else
        {
            char maskKey[4] = { 0 };
            char buffer[1024] = { 0 };
            int  pos = 0;

            int packetlen = 0;
            int len = u8TwoChar[1] & 0x7F;

            if (len < 126)
            {
                //memcpy(maskKey, (buf + 2), 4);
				memcpy(maskKey, (char*)&m_recvBuf[2], 4);
                pos = 6;
            }
            else if (len == 126)
            {
                len = buf[3] + (buf[2] << 8);
				memcpy(maskKey, (char*)(&m_recvBuf[4]), 4);
                //memcpy(maskKey, (buf + 4), 4);
                pos = 8;
            }
            else
            {
                return;
            }

            packetlen = len + pos;
            if (m_recvBuf.size() < packetlen)
            {
                break;
            }

            char u8PacketBuf[300 * 1024] = { 0 };
            memcpy(u8PacketBuf, (char*)&m_recvBuf[0], packetlen);

            m_recvBuf.erase(m_recvBuf.begin(), m_recvBuf.begin() + packetlen);

            for (int i = 0; i < len; i++)
            {
                int j = i % 4;
                buffer[i] = u8PacketBuf[i + pos] ^ maskKey[j];
            }

            std::string req = buffer;
        }
    } while (0);
}

void 
WebSockRealSession::onResponsePacket(Bufferev *bev, uint8_t * data, uint32_t inlen)
{
    ut_frame_s *hdr = (ut_frame_s *)data;
    if (hdr->tag == TAG_POSI ||
        hdr->tag == TAG_RPOS)
    {
        return;
    }

    WebSockFrame dr = WebSockFrame((char*)data, inlen);

    int nsize;
    char* msg = dr.GetBytes(nsize);
    if (NULL != msg)
    {
        if (bev)
        {
            bev->push((uint8_t*)msg, nsize);
        }
        
        delete[] msg;
    }	
}