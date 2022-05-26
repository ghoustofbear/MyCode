#include "http_realstream.h"
#include "stk/stk_message_http.h"
#include "socket.h"
#include "tls_error.h"
#include "cu.h"
#include "frame.h"

#if defined(_WIN32) || (__cplusplus >= 201103L)
#include "json/json.h"
#else
#include "json-0.10.6/json.h"
#endif

HttpRealStream::HttpRealStream(Cu *cu) : RealStream(cu)
{

}

HttpRealStream::~HttpRealStream()
{

}

int
HttpRealStream::open()
{
    SHL_Json::Reader     reader;
    SHL_Json::FastWriter writer;
    SHL_Json::Value      value;
    string           body;

    if (!reader.parse(m_json, value, false))
    {
        ERRE_SDK("Invalidate json parameter.");
        return -1;
    }

#ifdef UNIX_DRAW_RECT
	m_channelNo = value["channelNo"].asInt();
	m_devIp = value["deviceHost"].asString();
	m_devPort = value["devicePort"].asInt();
#endif

    value["userKey"] = m_cu->userKey();
    body = writer.write(value);

    stk::HttpMsg msg;
    msg.SetMethod("POST");
    msg.SetReqUrl("/preview");
    msg.SetVersion("HTTP/1.1");
    msg.SetHost("localhost");
    msg.SetContentType("application/json");
    msg.SetBody(body.c_str());

    string strMsg;
    msg.to_str(strMsg);
    if (m_sock->write(sys->ascii2utf8(strMsg).c_str(), strMsg.size()) < 0)
    {
        return -1;
    }

    string method;
    string url;
    string json;
    int    status;

    if (recvMsg(method, url, status, json) < 0)
    {
        return -1;
    }


    if (200 == status)
    {
        return 0;
    }

    SHL_Json::Value reason;
    reader.parse(json, reason, false);

    switch (status)
    {
    case 401:
        ERRE_SDK("Code: %d, Reason: %s.", status, reason["reason"].asString().c_str());
        break;

    default:
        ERRE_SDK("Code: %d, Reason: %s.", status, reason["reason"].asString().c_str());
        break;
    }

    return -1;
}

int
HttpRealStream::recv()
{
    while (m_recvThread.active() && m_cu->isLogin())
    {
        m_status = S_RECVING;
        /* recv frame ok */
        int ret = recvFrame();
        if (ret <= 0)
        {
            return -1;
        }

        ut_frame_s *hdr = (ut_frame_s *)m_frame;
        if (hdr->tag == TAG_RESPONSETALK)
        {
            int *ret = (int*)hdr->buf;

            if (*ret == 1)
            {
                notifyStatus(CODE_TALK_SUCC);
            }
            else
            {
                notifyStatus(CODE_TALK_FAIL);
            }

            continue;
        }

        notifyStream(m_frame, m_frameSize);
    }

    return 0;
}

int
HttpRealStream::recvMsg(string &method, string &url, int &status, string &json)
{
    string strMsg = "";
    int    cnt    = 0;

    while (m_recvThread.active())
    {
        char c;
        if (m_sock->read(&c, 1) <= 0)
        {
            return -1;
        }

        switch (c)
        {
        case '\r':
        case '\n':
            cnt++;
            break;

        default:
            cnt = 0;
        }

        strMsg += c;

        if (4 == cnt)
        {
            stk::HttpMsg httpMsg(strMsg.c_str(), strMsg.size());

            int length = httpMsg.GetContentLength_int();
            if (!length)
            {
                break;
            }

            char buf[1024] = {0};
            if (m_sock->read(buf, length) <= 0)
            {
                return -1;
            }

            strMsg += buf;
            break;
        }
    }

    stk::HttpMsg rspMsg(strMsg.c_str(), strMsg.size());

    method = rspMsg.GetMetthod();
    url    = rspMsg.GetReqUrl();
    status = rspMsg.GetStatusCode();
    json   = rspMsg.GetBody();

    return 0;
}

int
HttpRealStream::recvFrame()
{
    memset(m_frame, 0, sizeof(ut_frame_s));
    m_frameSize = 0;
    ut_frame_s *hdr = (ut_frame_s *)m_frame;

    int ret = m_sock->read(hdr, sizeof(ut_frame_s));
    if (ret <= 0)
    {
        return ret;
    }

    m_frameSize += sizeof(ut_frame_s);
	ret = m_sock->read(hdr->buf, hdr->size);
    if (ret <= 0)
    {
        return ret;
    }

    m_frameSize += hdr->size;

    return ret;
}
