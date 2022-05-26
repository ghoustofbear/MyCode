#include "websock_recordsession.h"
#include "websock_server.h"
#include "websock_frame.h"
#include "bufferev.h"
#include "putree.h"
#include "pu.h"
#include "json/json.h"

WebsockRecordSession::WebsockRecordSession(Client *client, Bufferev *bev,int download)
	: RecordSession(client)
{
	assert(bev != NULL);
	m_bev = bev;
	m_bev->addBuffevListener(this);
}

WebsockRecordSession::~WebsockRecordSession()
{
	websockSV->delBuffevListener(m_bev, this);
}

/*void WebsockRecordSession::stop()
{
	websockSV->delBuffevListener(m_bev, this);

	RecordSession::stop();

	m_bev = NULL;
}*/

void WebsockRecordSession::onRead(Bufferev *bev, void *data, uint32_t size)
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
            RecordSession::stop();
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

            if (0 == len)
            {
                RecordSession::stop();
                bev->close();
                break;
            }
            else if (len < 126)
            {
               // memcpy(maskKey, (buf + 2), 4);
				memcpy(maskKey, (char*)&m_recvBuf[2], 4);
                pos = 6;
            }
            else if (len == 126)
            {
                len = buf[3] + (buf[2] << 8);
               // memcpy(maskKey, (buf + 4), 4);
				memcpy(maskKey, (char*)(&m_recvBuf[4]), 4);
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

            char u8PacketBuf[100 * 1024] = { 0 };
            memcpy(u8PacketBuf, (char*)&m_recvBuf[0], packetlen);

            m_recvBuf.erase(m_recvBuf.begin(), m_recvBuf.begin() + packetlen);

            for (int i = 0; i < len; i++)
            {
                int j = i % 4;
                buffer[i] = u8PacketBuf[i + pos] ^ maskKey[j];
            }

            Pu *pu = NULL;
            if (0 == m_obtainType || 1 == m_obtainType)
            {
                pu = putree->findPu(m_deviceHost, m_devicePort);
            }
            else
            {
                pu = putree->findPu(m_addrcode);
            }

            if (!pu)
            {
                return;
            }

            std::string req = buffer;
            SHL_Json::Reader reader;
            SHL_Json::Value  rvalue;
            if (!reader.parse(req, rvalue, false))
            {
                return;
            }

            string strMethod = rvalue["command"].asString();
            if ("pause" == strMethod)
            {
                //m_sendStatus = false;
                //pu->pauseRecord(this);
				setPausestatus(true);
            }
            else if ("continue" == strMethod)
            {
                //m_sendStatus = true;
               // pu->continueRecord(this);
				setPausestatus(false);
            }
            else if ("setPos" == strMethod)
            {
                int pos = rvalue["pos"].asInt();
                pu->setPosRecord(pos, this);
            }
			else if ("fast" == strMethod)
			{
				int speed = getspeed();
				if (speed < 4)
					speed++;
				setspeed(speed);
			}
			else if ("slow" == strMethod)
			{
				int speed = getspeed();
				if (speed > -4)
					speed--;
				setspeed(speed);
			}
           /* else if (("slow" == strMethod) ||
                ("fast" == strMethod))
            {
                string speed = rvalue["speed"].asString();
               // pu->playRecord(this, speed, "");
            }*/
        }
    }while (0);
}

void WebsockRecordSession::onResponsePacket(Bufferev *bev, uint8_t * data, uint32_t inlen)
{
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
