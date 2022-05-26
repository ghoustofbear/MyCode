#include "websock_client.h"
#include "websock_server.h"
#include "websock_frame.h"
#include "json/json.h"
#include "bufferev.h"
#include "climgr.h"

WebSockClient::WebSockClient(Bufferev* bev, const string &name, const string &userId, const string &addrcode, uint64_t mright, uint64_t oright, string userKey)
    : Client(bev, name, userId, addrcode, mright, oright, userKey)
{
    m_type = Client::T_WEBSOCK;
}

WebSockClient::~WebSockClient()
{
    
}

void
WebSockClient::stop()
{
    //websockSV->delBuffevListener(m_bev, this);

    Client::stop();
}

void
WebSockClient::onRead(Bufferev *bev, void *data, uint32_t size)
{
    char maskKey[4] = { 0 };
    char buffer[1024] = { 0 };
    int  pos = 0;
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
            onClose(bev);
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
            int packetlen = 0;
            int len = u8TwoChar[1] & 0x7F;

            if (0 == len)
            {
                onClose(bev);
                break;
            }
            else if (len < 126)
            {
                memcpy(maskKey, (buf + 2), 4);
                pos = 6;
            }
            else if (len == 126)
            {
                len = buf[3] + (buf[2] << 8);
                memcpy(maskKey, (buf + 4), 4);
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

            SHL_Json::Reader reader;
            SHL_Json::Value  rvalue;

            if (!reader.parse(buffer, rvalue, false))
            {
                return;
            }

            string command = rvalue["command"].asString();
            if ("heartbeat" == command)
            {
                m_lastHeart = sys->currentTimeMillis();

                SHL_Json::FastWriter writer;
                SHL_Json::Value      value;

                value["version"] = "v1.0";
                value["msg"] = "heartbeat";
                value["code"] = 200;
                value["data"] = buffer;

                string body = writer.write(value);

                onHeartbeat(body, NULL, body.size());
            }
        }
    } while (0);
}


bool
WebSockClient::buildResponsePacket(string &json, char *stream, int &sz)
{
	//char buf[500*1024] = { 0 };
	char* buf = new char[500 * 1024];
	int len = int(json.size()) + 4;
	memcpy(buf, &len, sizeof(int));
	memcpy(buf + sizeof(int), (char*)json.c_str(), json.size());

	WebSockFrame dr = WebSockFrame(buf, len);
	if (buf != NULL)
	{
		delete[] buf;
		buf = NULL;
	}
	
    int nsize;
    char* msg = dr.GetBytes(nsize);
    if (NULL != msg)
    {
		memcpy(stream, msg, nsize);
		sz = nsize;
        delete[] msg;
        return true;
    }
    
	return false;
}

void WebSockClient::onAlarm(string &json, void *data, uint32_t size)
{
	SHL_Json::Value  value;
	value["version"] = "v1.0";
	value["msg"]     = "alarm";
	value["code"]    = 200;
	value["data"]    = json;

	SHL_Json::FastWriter writer;
	string alarmdata = writer.write(value);

	Client::onAlarm(alarmdata, data, alarmdata.size());
}

void WebSockClient::onHeartbeat(string &json, void *data, uint32_t size)
{
	SHL_Json::Value  value;
	value["version"] = "v1.0";
	value["msg"]  = "hearbeat";
	value["code"] = 200;
	value["data"] = json;

	SHL_Json::FastWriter writer;
	string alarmdata = writer.write(value);

	Client::onAlarm(alarmdata, data, alarmdata.size());
}
