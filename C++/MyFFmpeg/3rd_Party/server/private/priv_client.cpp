#include "priv_client.h"
#include "priv_server.h"
#include "utility.h"
#include "stk/stk_message_http.h"

PrivClient:: PrivClient(Bufferev* bev, const string &name, const string &userId, const string &addrcode, uint64_t mright, uint64_t oright, string userKey)
    :Client(bev, name, userId, addrcode, mright, oright, userKey)
{
    m_type = Client::T_PRIVATE;
}

PrivClient::~PrivClient()
{

}

void
PrivClient::stop()
{
    //privSV->delBuffevListener(m_bev, this);

    Client::stop();
}

bool     
PrivClient::buildResponsePacket(string &json, char *stream, int &sz)
{
#if 0
    char buf[100] = { 0 };
    snprintf_s(buf, sizeof(buf), "HTTP/1.1 %d OK\r\n", 200);

    stk::HttpMsg http(buf, strlen(buf));

    if (json.size() > 0)
    {
        http.SetContentType("application/json");
        http.SetBody(json.c_str());
    }

    string sbuf;
    http.to_str(sbuf);

    memcpy(stream, sbuf.c_str(), sbuf.size());
    sz = sbuf.size();
#else



    stk::HttpMsg pushMsg;
    pushMsg.SetMethod("POST");
    pushMsg.SetReqUrl("/push");
    pushMsg.SetVersion("HTTP/1.1");
    pushMsg.SetHost("localhost");
    pushMsg.SetConnection("Keep-Alive");
    pushMsg.SetContentType("Content-Type: application/json");

    pushMsg.SetBody(json.c_str());

    string buf;
    pushMsg.to_str(buf);

	memcpy(stream, buf.c_str(), buf.size());
	sz = buf.size();
#endif
	return true;
}