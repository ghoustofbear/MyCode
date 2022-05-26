#include "avtalk.h"
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


AVTalk::AVTalk(Cu *cu)
{    
    m_cu         = cu;
    m_wnd        = 0;
    m_streamCB   = NULL;
    m_streamData = NULL;
    m_statusCB   = NULL;
    m_statusData = NULL;
    m_status = DTS_UNINIT;
    
}



AVTalk::~AVTalk()
{
    uninit();
}

int
AVTalk::init()
{
    m_sock = sys->createSocket();
    m_sock->setReadTimeout(10000);
    return 0;
}

void 
AVTalk::uninit()
{
    m_status = DTS_UNINIT;

    m_thread.stop();

    if (m_sock)
    {
        m_sock->close();
        delete m_sock;
        m_sock = NULL;
    }

}

int
AVTalk::start(string json)
{
    m_devListJson = json;


    m_thread.setEntry(threadProc, this, 0);
    return m_thread.start();
}

void
AVTalk::stop(string json)
{

}


int
AVTalk::open()
{
    SHL_Json::Value value;
    SHL_Json::Reader reader;
    SHL_Json::FastWriter writer;
    SHL_Json::Value  rspVal;

    if (!reader.parse(m_devListJson, value, false))
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

int
AVTalk::pause()
{
    m_status = DTS_WATTING_CLOSE;
    return 0;
}

int
AVTalk::resume()
{
    m_status = DTS_CONNECT_SUCCESS;
    return 0;
}

int
AVTalk::requestDualTalk(string devList)
{
    return 0;
}

int
AVTalk::disconnDualTalk(string devListJson)
{
    return 0;
}

int
AVTalk::initDualTalk()
{
    string rspJson = "";
    int ret = sendRequest("/initDualTalk", "server init dualtalk", rspJson);

    if (ret == 200)
    {
        ERRE_SDK("server init Dual Talk success");
        return 0;
    }


    SHL_Json::Reader reader;
    SHL_Json::Value  rspVal;
    reader.parse(rspJson, rspVal, false);
    ERRE_SDK("Code: %d, Reason: %s.", ret, rspVal["reason"].asString().c_str());
    return -1;
}

int
AVTalk::reqDualTalk(string devListJson)
{
    string rspJson = "";
    int ret = sendRequest("/requestDualTalk", devListJson, rspJson);

    if (ret == 200)
    {
        ERRE_SDK("client request Dual Talk success");
        return 0;
    }


    SHL_Json::Reader reader;
    SHL_Json::Value  rspVal;
    reader.parse(rspJson, rspVal, false);
    ERRE_SDK("Code: %d, Reason: %s.", ret, rspVal["reason"].asString().c_str());
    return -1;

}


int
AVTalk::sendRequest(string url, string body, string &rspJson)
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
AVTalk::recvHttpMsg(string &method, string &type, int &status, string &json)
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

    type   = httpMsg.GetReqUrl();
    method = httpMsg.GetMetthod();
    status = httpMsg.GetStatusCode();
    json = httpMsg.GetBody();
    return 0;
}

bool
AVTalk::isSame(Cu* cu)
{
    return cu == m_cu ? true:false;
}

void
AVTalk::doStream(void *stream, uint32_t size)
{  
    if (m_status == DTS_CONNECT_SUCCESS)
    {
        m_sock->write(stream, size);
    }
}

void
AVTalk::onStream(void *stream, uint32_t size)
{
    //play dual voice
}

void
AVTalk::onStatus(int code)
{
    STATUS(code);
}

int
AVTalk::recv()
{
    int   sRet = 0;
    while (m_thread.active())
    {
        sRet = m_sock->select(true, false, 1000);
        if (sRet > 0)
        {
            memset(m_frame, 0, sizeof(ut_frame_s));
            m_frameSize = 0;
            ut_frame_s *hdr = (ut_frame_s *)m_frame;
            if (m_sock->read(hdr, sizeof(ut_frame_s)) <= 0)
            {
                return -1;
            }
            m_frameSize += sizeof(ut_frame_s);
            if (m_sock->read(hdr->buf, hdr->size) <= 0)
            {
                return -1;
            }
            m_frameSize += hdr->size;

            m_status = DTS_TALKING;

            onStream(m_frame, m_frameSize);
        }
        else if (sRet < 0)
        {
            return -1;
        }
        else
        {
            
        }

        //break;
    }

    return 0;
}

void
AVTalk::recvLoop()
{
    while (m_thread.active())
    {
        if (!m_cu->isLogin())
        {
            m_status = DTS_WAITING_LOGIN;
            ERRD_SDK("Waiting for login.");
            sys->sleep(200);
            continue;
        }

        if (m_sock->open(m_cu->host()) < 0 ||
            m_sock->connect() < 0)
        {
            m_sock->close();

            m_status = DTS_CONNECTING;
            ERRD_SDK("Connect failed.");
            sys->sleep(2000);
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
            sys->sleep(2000);
            continue;
        }

        m_status = DTS_CONNECT_SUCCESS;

        if (recv() < 0)
        {
            if (!m_cu->isLogin())
            {
                /* cu first exit then remote close stream  */
                continue;
            }
            ERRD_SDK("recv dualtalk data failed.");
            m_status = DTS_WATTING_CLOSE;
            m_sock->close();
            sys->sleep(1000);
        }
        
    }
}



void *
AVTalk::threadProc(ThreadInfo *info)
{
    AVTalk * stream = (AVTalk*)(info->data);

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




/////////////////////////////////////////////////////////////////////////
Intercom::Intercom(HANDLE view)
{
    m_view = (Preview*)view;
    m_sock = sys->createSocket();
    m_sock->setReadTimeout(10000);
}

Intercom::~Intercom()
{
    if (m_sock)
    {
        m_sock->close();
        delete m_sock;
        m_sock = NULL;
    }


}

int 
Intercom::start()
{
    m_thread.setEntry(threadProc, this, 0);
    return m_thread.start();
}

int 
Intercom::stop()
{
    m_thread.stop();
    return 0;
}

void
Intercom::recvLoop()
{
    //while (m_thread.active())
    //{
    //    if (!m_cu->isLogin())
    //    {
    //        m_status = DTS_WAITING_LOGIN;
    //        ERRD_SDK("Waiting for login.");
    //        sys->sleep(200);
    //        continue;
    //    }
    //}
}

void* 
Intercom::threadProc(ThreadInfo *info)
{
    Intercom * intercom = (Intercom*)(info->data);

    switch (info->id)
    {
    case 0:
        intercom->recvLoop();
        break;

    default:
        break;
    }

    return NULL;
}


