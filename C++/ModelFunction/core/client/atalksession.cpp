#include "atalksession.h"
#include "socket.h"
#include "tls_error.h"
#include "cu.h"
#include "frame.h"
#include "c2_player.h"
#include "mempool.h"

#include "stk/stk_message_http.h"

#if defined(_WIN32) || (__cplusplus >= 201103L)
#include "json/json.h"
#else
#include "json-0.10.6/json.h"
#endif

#include "preview.h"


ATalkSession::ATalkSession(Cu *cu) :m_cu(cu), m_statusCB(NULL), m_statusData(NULL)
{
    m_status = DTS_UNINIT;
    m_laststatus = -1;
    m_sock = sys->createSocket();
    m_sock->setWriteTimeout(5000);
    m_sock->setReadTimeout(3000);
}

ATalkSession::~ATalkSession()
{
    stop("");

    if (m_sock)
    {
        delete m_sock;
        m_sock = NULL;
    }
}

void
ATalkSession::init(c2_status_cb cb, void* userdata)
{
    m_statusCB = cb;
    m_statusData = userdata;
}

int
ATalkSession::start(string json)
{
    m_thread.setEntry(threadProc, this, 0);
    m_thread.start();
    m_status = DTS_STARTING;
    return 0;
}

void
ATalkSession::stop(string json)
{
    m_thread.stop();

    if (m_sock)
    {
        m_sock->close();
    }
}

int
ATalkSession::talkTo(HANDLE view)
{

    

    SHL_Json::Value value;
    SHL_Json::FastWriter writer;
    
    value["version"] = "1.0";
    value["userKey"] = m_cu->userKey();
    value["host"] = "192.168.66.147";
    value["port"] = "9988";
    value["channel"] = 5;

    string body = writer.write(value);

    char alive[1024] = { 0 };

    ut_frame_s *hdr = (ut_frame_s *)alive;
    hdr->tag = TAG_STARTTALK;
    hdr->type = 'A';

    hdr->size = body.size();
    hdr->pts = sys->currentTimeMillis();
    hdr->time = hdr->pts;
    hdr->seq = 0;

    memcpy(hdr->buf, body.c_str(), body.size());

    m_sock->write(alive, sizeof(ut_frame_s) + body.size());

    return 0;
}

int
ATalkSession::endTalk(HANDLE view)
{
    SHL_Json::Value value;
    SHL_Json::FastWriter writer;

    value["version"] = "1.0";
    value["userKey"] = m_cu->userKey();
    value["host"] = "192.168.66.147";
    value["port"] = "9988";
    value["channel"] = 5;

    string body = writer.write(value);

    char alive[1024] = { 0 };

    ut_frame_s *hdr = (ut_frame_s *)alive;
    hdr->tag = TAG_STOPTALK;
    hdr->type = 'A';

    hdr->size = body.size();
    hdr->pts = sys->currentTimeMillis();
    hdr->time = hdr->pts;
    hdr->seq = 0;

    memcpy(hdr->buf, body.c_str(), body.size());

    m_sock->write(alive, sizeof(ut_frame_s) + body.size());
    return 0;
}

void
ATalkSession::statusCallback(int status)
{

    if (m_statusCB)
    {
        //防止频繁回调
        if (m_laststatus != status)
        {
            m_statusCB(status, m_statusData);
            m_laststatus = status;
        }
    }
}

int
ATalkSession::sendRequest(string url, string body, string &rspJson)
{
    stk::HttpMsg msg;

    msg.SetMethod("POST");
    msg.SetReqUrl(url.c_str());
    msg.SetVersion("HTTP/1.1");
    msg.SetHost("localhost");
    msg.SetConnection("Keep-Alive");
    msg.SetContentType("application/json");
    msg.SetBody(body.c_str());

    string strMsg;
    msg.to_str(strMsg);

    if (m_sock->write(strMsg.c_str(), strMsg.size()) < 0)
    {
        return -1;
    }

    string method = "";
    string type = "";
    int    status = -1;

    if (recvHttpMsg(method, type, status, rspJson) < 0)
    {
        return -1;
    }

    return status;
}

int
ATalkSession::heartbeat()
{
    char alive[128] = { 0 };

    ut_frame_s *hdr = (ut_frame_s *)alive;
    hdr->tag = TAG_HEARBEAT;
    hdr->type = 'A';
    hdr->encode = 0;
    hdr->channel = 0;
    hdr->encrypt = 0;

    hdr->size = 0;
    hdr->pts = sys->currentTimeMillis();
    hdr->time = hdr->pts;
    hdr->seq = 0;

    m_sock->write(alive, sizeof(ut_frame_s));

    return 0;
}

bool
ATalkSession::isSame(Cu* cu, string json)
{
    if (m_cu == cu && m_json == json)
    {
        return true;
    }
    else
    {
        return false;
    }
}

int
ATalkSession::recvHttpMsg(string &method, string &type, int &status, string &json)
{

    string strMsg = "";
    int    cnt = 0;

    while (m_thread.active())
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

            char buf[1024] = { 0 };
            if (m_sock->read(buf, length) <= 0)
            {
                return -1;
            }

            strMsg += buf;
            break;
        }
    }

    stk::HttpMsg httpMsg(strMsg.c_str(), strMsg.size());

    

    type = httpMsg.GetReqUrl();
    method = httpMsg.GetMetthod();
    status = httpMsg.GetStatusCode();
    json = httpMsg.GetBody();

    return 0;
}



void
ATalkSession::recvLoop()
{
    while (m_thread.active())
    {
        if (!m_cu->isLogin())
        {
            m_status = DTS_WAITING_LOGIN;
            ERRD_SDK("Waiting for login.");

            statusCallback(CODE_TALK_FAILED);

            sys->sleep(500);
            continue;
        }

        if (m_sock->open(m_cu->host()) < 0 ||
            m_sock->connect() < 0)
        {
            m_sock->close();

            m_status = DTS_CONNECTING;
            ERRD_SDK("Connect failed.");

            statusCallback(CODE_TALK_FAILED);

            sys->sleep(500);
            continue;
        }

        if (open() < 0)
        {
            if (!m_cu->isLogin())
            {
                /* cu first exit then remote close stream  */
                continue;
            }

            m_sock->close();
            ERRD_SDK("open dualtalk failed.");
            m_status = DTS_CONNECTING;

            statusCallback(CODE_TALK_BUSY);

            sys->sleep(300);
            continue;
        }

        m_status = DTS_CONNECT_SUCCESS;
        statusCallback(CODE_TALK_SUCCESS);

        string recvType = "";
        string recvMethod = "";
        int    recvStatus = 0;
        string recvJson = "";
        int    cnt = 0;

        while (m_thread.active())
        {
            bool timeout = false;
            int  result = -1;

            result = m_sock->select(true, false, 200);
            if (result > 0)
            {
                if (0 == recvHttpMsg(recvMethod, recvType, recvStatus, recvJson))
                {
                    cnt = 0;
                    if (recvType == "/heartbeat")
                    {
                        LOGD_SDK("heartbeat");
                    }
                    else if (recvType == "/ctrl")
                    {
                        //onCtrl(recvJson);
                    }
                    continue;
                }
            }
            else if (result < 0)
            {
                m_status = DTS_WAITING_LOGIN;
                m_sock->close();
                break;
            }
            else
            {
                timeout = true;
            }

            if (timeout)
            {
                if (++cnt < 20)
                {
                    LOGT_SDK("client send heartbeat!");
                    continue;
                }
            }

            m_status = DTS_WAITING_LOGIN;
            m_sock->close();
            break;
        }
    }
}


void *
ATalkSession::threadProc(ThreadInfo *info)
{
    ATalkSession * stream = (ATalkSession*)(info->data);

    switch (info->id)
    {
    case 0:
        stream->recvLoop();
        break;

    default:
        break;
    }

    return NULL;
}

///////////////////////////////////////////////////////////////////////////

HttpATalkSession::HttpATalkSession(Preview *view) : ATalkSession(view->getCu())
{
    m_json = view->getJson();
}

HttpATalkSession::HttpATalkSession(Cu *cu): ATalkSession(cu)
{
    m_json = "";
}

HttpATalkSession::~HttpATalkSession()
{

}


int
HttpATalkSession::open()
{
    SHL_Json::Value value;
    SHL_Json::Reader reader;
    SHL_Json::FastWriter writer;
    SHL_Json::Value  rspVal;

    if (!reader.parse(m_json, value, false))
    {
        ERRE_SDK("Invalidate json parameter.");
        return -1;
    }

    value["userKey"] = m_cu->userKey();
    string body = writer.write(value);

    string rspJson = "";
    int ret = sendRequest("/dualtalk", body, rspJson);

    if (ret == 200)
    {
        LOGI_SDK("dualtalk request Dual Talk success");
        return 0;
    }

    reader.parse(rspJson, rspVal, false);
    ERRE_SDK("Code: %d, Reason: %s.", ret, rspVal["reason"].asString().c_str());

    return -1;
}

void
HttpATalkSession::onAudioStream(void *stream, uint32_t size, int encode, int sps, int channels, int bps, uint64_t pts)
{
    static uint32_t    s_seq = 0;

    if (m_status == DTS_CONNECT_SUCCESS)
    {
#if 0
        memset(m_aframe, 0, 1024);
        memcpy(m_aframe, stream, size);
        m_sock->write(m_aframe, size);
#else
        memset(m_aframe, 0, 1024);
        ut_frame_s *hdr = (ut_frame_s *)m_aframe;
        memcpy(hdr->buf, stream, size);

        hdr->tag = TAG_STD;
        hdr->type = 'A';
        hdr->encode = encode;
        hdr->channel = 1;
        hdr->encrypt = 0;

        hdr->size = size;
        hdr->pts = pts;
        hdr->time = pts;
        hdr->seq = s_seq++;

        hdr->audio.bps = bps;
        hdr->audio.channels = channels;
        hdr->audio.packLen = size;
        hdr->audio.sps = sps;

        m_sock->write(m_aframe, size + sizeof(ut_frame_s));
#endif
        
    }
}











