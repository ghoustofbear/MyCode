#include "http_playback.h"
#include "stk/stk_message_http.h"
#include "socket.h"
#include "tls_error.h"
#include "cu.h"
#include "frame.h"

#include "hik_player.h"
/* #include "dahua_player.h" */

#if defined(_WIN32) || (__cplusplus >= 201103L)
#include "json/json.h"
#else
#include "json-0.10.6/json.h"
#endif


Httplayback::Httplayback(string filename, Cu *cu)
    : Playback(filename, cu)
{
    
}

Httplayback::Httplayback(datetime_s &start, datetime_s &end, Cu *cu)
    : Playback(start, end, cu)
{

}

Httplayback::~Httplayback()
{

}

int
Httplayback::open()
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

    value["userKey"] = m_cu->userKey();
    value["offset"] = m_offset;
    body = writer.write(value);

    stk::HttpMsg msg;
    msg.SetMethod("POST");
    msg.SetReqUrl("/playback");
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
        m_streamStatus = 1;
        
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
Httplayback::recv()
{
    int recvRet = 0;
    int ret = 0;
    while (m_recvThread.active() && m_cu->isLogin())
    {
        /* recv frame ok */
        recvRet = recvFrame();
        if (recvRet < 0)
        {
            return recvRet;
        }

        m_status = S_RECVING;

        ut_frame_s *hdr = (ut_frame_s *)m_frame;
        if (TAG_POSI == hdr->tag)
        {
            m_streamPos = *(uint32_t *)hdr->buf;

            //LOGI_SDK("stream pos: %d", m_streamPos);
            if (100 == m_streamPos)
            {
                ret = 1;//½áÊø
                break;
            }

            continue;
        }

        if (TAG_RPOS == hdr->tag)
        {
            //onReposEnd();
            continue;
        }
       
        onStream(m_frame, m_frameSize);
        m_offset += hdr->size;
    }

    return ret;
}

int
Httplayback::recvMsg(string &method, string &url, int &status, string &json)
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
Httplayback::recvFrame()
{
    memset(m_frame, 0, sizeof(ut_frame_s));
    m_frameSize = 0;
    ut_frame_s *hdr = (ut_frame_s *)m_frame;

    if (m_sock->read(hdr, sizeof(ut_frame_s)) < 0)
    {
        LOGE_SDK("header m_sock->read -1");
        return -1;
    }

    m_frameSize += sizeof(ut_frame_s);
    if (m_sock->read(hdr->buf, hdr->size) < 0)
    {
        LOGE_SDK("data m_sock->read -1, type:%c size:%d", hdr->type, hdr->size);
        return -1;
    }

    m_frameSize += hdr->size;

    return 0;
}

int
Httplayback::pause()
{
    if (S_RECVING != m_status)
    {
        return 0;
    }

    return streamCtrl(PLAY_PAUSE, NULL, 4);


    //if (m_playStatus == 2)
    //{
    //    return 0;
    //}

    //SHL_Json::FastWriter writer;
    //SHL_Json::Value      value;
    //string           body;

    //value["userKey"] = m_cu->userKey();
    //body = writer.write(value);

    //stk::HttpMsg msg;
    //msg.SetMethod("POST");
    //msg.SetReqUrl("/pause");
    //msg.SetVersion("HTTP/1.1");
    //msg.SetHost("localhost");
    //msg.SetContentType("application/json");
    //msg.SetBody(body.c_str());
    //string strMsg;
    //msg.to_str(strMsg);
    //if (m_sock->write(strMsg.c_str(), strMsg.size()) < 0)
    //{
    //    return -1;
    //}
    //m_playStatus = 2;
    //return 0;
}

int
Httplayback::continuex()
{
    if (S_RECVING != m_status)
    {
        return 0;
    }
    return streamCtrl(PLAY_RESTART, NULL, 4);


    //if (m_playStatus == 1)
    //{
    //    return 0;
    //}

    //SHL_Json::FastWriter writer;
    //SHL_Json::Value      value;
    //string           body;

    //value["userKey"] = m_cu->userKey();
    //body = writer.write(value);

    //stk::HttpMsg msg;
    //msg.SetMethod("POST");
    //msg.SetReqUrl("/continue");
    //msg.SetVersion("HTTP/1.1");
    //msg.SetHost("localhost");
    //msg.SetContentType("application/json");
    //msg.SetBody(body.c_str());

    //string strMsg;
    //msg.to_str(strMsg);
    //if (m_sock->write(strMsg.c_str(), strMsg.size()) < 0)
    //{
    //    return -1;
    //}
    //m_playStatus = 1;
    //return 0;
}

int
Httplayback::heartbeat()
{
    //SHL_Json::FastWriter writer;
    //SHL_Json::Value      value;
    //string           body;

    //value["userKey"] = m_cu->userKey();
    //body = writer.write(value);

    stk::HttpMsg msg;
    msg.SetMethod("POST");
    msg.SetReqUrl("/heartbeat");
    msg.SetVersion("HTTP/1.1");
    msg.SetHost("localhost");
    //msg.SetContentType("application/json");
    //msg.SetBody(body.c_str());

    string strMsg;
    msg.to_str(strMsg);
    if (m_sock->write(strMsg.c_str(), strMsg.size()) < 0)
    {
        return -1;
    }

    return 0;
}

int
Httplayback::streamCtrl(int code, void *val, int valSize)
{
    SHL_Json::FastWriter writer;
    SHL_Json::Value      value;
    string           body;
    stk::HttpMsg     msg;

    switch (code)
    {
    case PLAY_PAUSE:
        {
            if (m_streamStatus == 2)
            {
                return 0;
            }
            msg.SetReqUrl("/pause");
            m_streamStatus = 2;
            printf("sdk pause\n");
        }
        break;
    case PLAY_RESTART:
        {
            if (m_streamStatus == 1)
            {
                return 0;
            }
            msg.SetReqUrl("/continue");
            m_streamStatus = 1;
            printf("sdk continue\n");
        }
        break;
    case PLAY_FAST:
        {
            m_speed++;
            if (m_speed > 4)
            {
                m_speed = 4;
            }
            msg.SetReqUrl("/fast");
            value["speed"] = getSpeedStr(m_speed);
        }
        break;

    case PLAY_SLOW:
        {
            m_speed--;
            if (m_speed < -4)
            {
                m_speed = -4;
            }
            msg.SetReqUrl("/slow");
            value["speed"] = getSpeedStr(m_speed);
        }
        break;

    case PLAY_NORMAL:
        {
            m_speed = 0;
            value["speed"] = getSpeedStr(m_speed);
            msg.SetReqUrl("/normal");
            m_streamStatus = 1;
        }
        break;


    case PLAY_STARTREPOS:
        {
            msg.SetReqUrl("/startRepos");
        }
        break;
    case PLAY_STEP:
        {
            msg.SetReqUrl("/frame");
        }
        break;
    case PLAY_REVERSE:
        {
            if (val == NULL)
                return -1;
            datetime_s dt = *(datetime_s *)val;
            value["time"] = sys->strftime(dt);
            msg.SetReqUrl("/reverse");
        }
        break;
    case PLAY_FORWARD:
        {
            datetime_s dt = *(datetime_s *)val;
            value["time"] = sys->strftime(dt);
            msg.SetReqUrl("/forward");
        }
        break;
    case PLAY_SETPOS:
        {
            msg.SetReqUrl("/setPos");
            value["pos"] = *(int *)val;
        }
        break;
    case PLAY_SETTIME:
        {
            if (val == NULL)
                return -1;
            msg.SetReqUrl("/setTime");
            datetime_s dt = *(datetime_s *)val;
            value["time"] = sys->strftime(dt);

        }
        break;
    case PLAY_SETLABEL:
        {
            msg.SetReqUrl("/insertlabel");
            //char* labelName = new char[valSize + 1];
            //memcpy(labelName, val, valSize);
            value["name"] = (char*)val;
        }
        break;
    default:
        ERRE_SDK("Unknow control commond.");
        return -1;
    }

    value["userKey"] = m_cu->userKey();
    body = writer.write(value);

    msg.SetMethod("POST");
    msg.SetVersion("HTTP/1.1");
    msg.SetHost("localhost");
    msg.SetContentType("application/json");

    msg.SetBody(body.c_str());

    string strMsg;
    msg.to_str(strMsg);
    if (m_sock->write(strMsg.c_str(), strMsg.size()) < 0)
    {
        return -1;
    }

    return 0;
}
