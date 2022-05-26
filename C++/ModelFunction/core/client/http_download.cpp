#include "http_download.h"
#include "stk/stk_message_http.h"
#include "socket.h"
#include "tls_error.h"
#include "cu.h"
#include "frame.h"
#include "fsrw.h"

#if defined(_WIN32) || (__cplusplus >= 201103L)
#include "json/json.h"
#else
#include "json-0.10.6/json.h"
#endif

HttpDownload::HttpDownload(Cu *cu, string savepath, bool standard)
    : RecordStream(cu)
{
    m_savepath = savepath;
    m_standard = standard;
    m_savefile = NULL;
    if (standard)
    {
        m_saveStandardFile = NULL;
//        m_saveStandardFile = new RecordWriter();
//        string saveFileName = savepath + ".utv";
//        if (0 != m_saveStandardFile->open(saveFileName.c_str()))
//        {
//            delete m_saveStandardFile;
//            m_saveStandardFile = NULL;
//            ERRE_SDK("SDK can not open file %s, please check!!!", saveFileName.c_str());
//        }
    }
    else
    {
        m_saveStandardFile = NULL;
    }
}

HttpDownload::~HttpDownload()
{
    if (m_standard)
    {
        if (m_saveStandardFile)
        {
            delete m_saveStandardFile;
            m_saveStandardFile = NULL;
        }
    }
}

void
HttpDownload::init(c2_stream_cb streamCB, void *streamData, c2_status_cb statusCB, void *statusData)
{
    m_streamCB   = streamCB;
    m_streamData = streamData;
    m_statusCB   = statusCB;
    m_statusData = statusData;
}

int
HttpDownload::open()
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
    msg.SetReqUrl("/download");
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
HttpDownload::recv()
{
    while (m_recvThread.active() && m_cu->isLogin())
    {
        /* recv frame ok */
        if (recvFrame() < 0)
        {
            return -1;
        }

        ut_frame_s *hdr = (ut_frame_s *)m_frame;
        if (TAG_POSI == hdr->tag)
        {
            m_streamPos = *(uint32_t *)hdr->buf;

            LOGI_SDK("stream pos: %d\n", m_streamPos);

            if (100 == m_streamPos)
            {
                break;
            }

            continue;
        }

        m_status = S_RECVING;
        onStream(m_frame, m_frameSize);
    }

    return 0;
}

int
HttpDownload::recvMsg(string &method, string &url, int &status, string &json)
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
HttpDownload::recvFrame()
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

    return 0;
}

void
HttpDownload::stop()
{
    RecordStream::stop();

    if (m_standard)
    {
        if (m_saveStandardFile)
        {
            m_saveStandardFile->close();
        }
    }
    else
    {
        if (m_savefile)
        {
            fflush(m_savefile);
            fclose(m_savefile);
            m_savefile = NULL;
        }
    }
}

void
HttpDownload::onStream(void *stream, uint32_t size)
{
    ut_frame_s *hdr = (ut_frame_s *)stream;
    string savepath = m_savepath;
    if (!m_standard)
    {

        if (TAG_HIK == hdr->tag)
        {
            savepath += ".hkv";
        }
        else if (TAG_HIK_N == hdr->tag)
        {
            savepath += ".hkv";
        }
        else if (TAG_DAHUA == hdr->tag)
        {
            savepath += ".dav";
        }
        else if (TAG_STD == hdr->tag)
        {
            savepath += ".sdv";
        }
        if (!m_savefile)
        {
            m_savefile = fopen(savepath.c_str(), "wb");
        }

        if (!m_savefile)
        {
            return;
        }

        m_offset += hdr->size;

        fwrite(hdr->buf, hdr->size, 1, m_savefile);
    }
    else
    {
        if (!m_saveStandardFile)
        {
            m_saveStandardFile = new RecordWriter();
            savepath += ".utv";
            if (0 != m_saveStandardFile->open(savepath.c_str()))
            {
                ERRE_SDK("SDK can not open file %s, please check!!!", savepath.c_str());
                delete m_saveStandardFile;
                m_saveStandardFile = NULL;
				return;
            }
        }

        if (m_saveStandardFile)
        {
            m_saveStandardFile->write(stream);          
        }
    }
}

void
HttpDownload::onStatus(int code)
{
    STATUS(code);

    if (code == CODE_STREAM_FAIL)
    {
        if (m_standard && m_saveStandardFile)
        {
            string t = m_savepath+".utv";
            m_saveStandardFile->close();
            delete m_saveStandardFile;
            m_saveStandardFile = NULL;
            LOGI_SDK("download failed, remove file:%s\n", t.c_str());
            sys->removeFile(t.c_str());
        }
    }
}


int
HttpDownload::pause()
{
    return -1;
}

int
HttpDownload::continuex()
{
    return -1;
}

int
HttpDownload::heartbeat()
{
    return -1;
}

void
HttpDownload::onReposEnd()
{

}

int
HttpDownload::streamCtrl(int code, void *val, int valSize)
{
    return 0;
}

void
HttpDownload::timerdriver()
{
    RecordStream::timerdriver();
}
