#include "http_cu.h"
#include "socket.h"
#include "curl/curl.h"
#include "tls_error.h"
#include "c2types_json.h"
#include "utility.h"

#include "stk/stk_message_http.h"
#if defined(_WIN32) || (__cplusplus >= 201103L)
#include "json/json.h"
#else
#include "json-0.10.6/json.h"
#endif

using namespace stk;

typedef struct
{
    char   * readPtr;
    size_t 	 readSize;
} READ_INFO_S;

static size_t read_callback(void *ptr, size_t size, size_t nmemb, void *userp)
{
    READ_INFO_S *readInfo = (READ_INFO_S*)userp;

    if (size * nmemb < 1 || readInfo->readSize <= 0)
    {
        return 0;
    }

    if (readInfo->readSize <= size * nmemb)
    {
        size_t res = readInfo->readSize;
        memcpy(ptr, readInfo->readPtr, readInfo->readSize);
        readInfo->readSize = 0;

        return res;
    }

    memcpy(ptr, readInfo->readPtr, size * nmemb);
    readInfo->readPtr  += size * nmemb;
    readInfo->readSize -= size * nmemb;

    return size * nmemb;
}

static size_t post_resp_callback(const char *ptr, size_t size, size_t nmemb, void *userp)
{
    if (!userp)
    {
        return size * nmemb;
    }

    string * rsp = (string *)userp;

    /* HTTPCU_DEBUG("%s", ptr); */
    *rsp += ptr;

    return size * nmemb;
}

HttpCu::HttpCu(string host) : Cu(host)
{
    m_protocol = P_HTTP;
    m_response.clear();
}

HttpCu::~HttpCu()
{

}

int
HttpCu::init()
{
    if (Cu::init() < 0)
    {
        return -1;
    }

    return 0;
}

void
HttpCu::uninit()
{
    /* do something*/

    Cu::uninit();
}

int
HttpCu::login()
{
    HttpMsg msg;

    msg.SetMethod("POST");
    msg.SetReqUrl("/login");
    msg.SetVersion("HTTP/1.1");
    msg.SetHost("localhost");
    msg.SetConnection("Keep-Alive");
    msg.SetContentType("application/json");
    msg.SetBody(m_loginJson.c_str());

    string strMsg;
    msg.to_str(strMsg);

    if (m_sock->write(strMsg.c_str(), strMsg.size()) < 0)
    {
        return -1;
    }

    string method = "";
    int    type   = MSG_LOGIN;
    int    status = -1;

    if (recvMsg(method, type, status, m_rspJson) < 0)
    {
        return -1;
    }

    SHL_Json::Reader reader;
    SHL_Json::Value  rspVal;
    reader.parse(m_rspJson, rspVal, false);

    if (200 == status)
    {
        m_userKey = rspVal["key"].asString();
        m_isLogin = true;

        return 0;
    }

    switch (status)
    {
    case 401:
        ERRE_SDK("Code: %d, Reason: %s.", status, rspVal["reason"].asString().c_str());
        /* LOGIN_STATUS(CODE_LOGIN_NOUSER); */
        break;

    case 403:
        ERRE_SDK("Code: %d, Reason: %s.", status, rspVal["reason"].asString().c_str());
        /* LOGIN_STATUS(CODE_LOGIN_PSWDERR); */
        break;

    default:
        ERRE_SDK("Code: %d, Reason: %s.", status, rspVal["reason"].asString().c_str());
        /* LOGIN_STATUS(CODE_LOGIN_FAIL); */
        break;
    }

    return status;
}

void
HttpCu::logout(long &l)
{

}

long
HttpCu::getServerInfo(c2_servinfo_s *info)
{
    if (!m_isLogin)
    {
        ERRE_SDK("is not logined.");
        return -1;
    }

    SHL_Json::Reader reader;
    SHL_Json::Value  value;

    if (!reader.parse(m_rspJson, value, false))
    {
        return -1;
    }

    if (!value["version"].isNull())
    {
        strncpy_s(info->version, value["version"].asCString(), sizeof(info->version));
    }

    if (!value["msvcode"].isNull())
    {
        strncpy_s(info->msvcode, value["msvcode"].asCString(), sizeof(info->msvcode));
    }

    if (!value["msvname"].isNull())
    {
        strncpy_s(info->msvname, value["msvname"].asCString(), sizeof(info->msvname));
    }

    if (!value["usercode"].isNull())
    {
		strncpy(info->usercode, value["usercode"].asCString(), sizeof(info->usercode));
    }

    info->alarmBitmap  = value["alarmBitMap"].asUInt64();
    info->mgrBitmap    = value["mgrBitmap"].asInt64();
    info->optBitmap    = value["optBitmap"].asInt64();
    info->mgrLevel     = value["mgrLevel"].asInt();
    info->optLevel     = value["optLevel"].asInt();
    info->deviceNum    = value["deviceNum"].asInt();
    info->allowRelogin = value["allowRelogin"].asBool();
    info->suportTalk   = value["suportTalk"].asBool();
    info->suportPriv   = value["suportPriv"].asBool();

    return 0;
}

int
HttpCu::heartbeat()
{
    HttpMsg msg;
    msg.SetMethod("POST");
    msg.SetReqUrl("/heartbeat");
    msg.SetVersion("HTTP/1.1");
    msg.SetHost("localhost");
    msg.SetConnection("Keep-Alive");

    string strMsg;
    msg.to_str(strMsg);

    if (m_sock->write(strMsg.c_str(), strMsg.size()) < 0)
    {
        return -1;
    }

    return 0;
}

int
HttpCu::remoteCall(uint32_t call, string jin, string &jout)
{
    string url;
    switch (call)
    {
    case R_SUBSCRIBE:
        url = "/subscribe";
        break;

    case R_PTZ_CTRL:
        //url = "/ptzCtrlV1";
		url = "/ptzCtrl";
        break;

    case R_QUERY_DEVICE:
        url = "/queryDevice";
        break;

    case R_QUERY_PRESET:
        url = "/queryPreset";
        break;

    case R_SET_PRESET:
        url = "/setPreset";
        break;

    case R_QUERY_RECORD:
        url = "/queryRecord";
        break;

    case R_SYN_TIME:
        url = "/syncTime";
        break;

    case R_MATRIX_SWITCH:
        url = "/matrixSwitch";
        break;

    case R_CALL_DECODER:
        url = "/windowDecoder";
        break;

    case R_SET_FACE:
        url = "/setFace";
        break;

    case R_CUSTOM_COMMAND:
        url = "/customCommand";
        break;

    case R_GET_CONFIG:
        url = "/getConfig";
        break;

    case R_LOCK_FILE:
        url = "/lockFile";
        break;

    case R_PTZ_PATTERN_CTRL:
        url = "/ptzPatternCtrl";
        break;

    case R_SUBSCRIBE_SERVER_STATUS:
        url = "/subscribeStatus";
        break;

	case R_QUERY_USER:
		url = "/queryUser";
		break;

    case R_QUERY_CHANNEL:
        url = "/queryChannel";
        break;

    default:
        ERRE_SDK("remote call %d is not implemet.", call);
        return -1;
    }

    SHL_Json::Reader     reader;
    SHL_Json::FastWriter writer;
    SHL_Json::Value      jvalue;

    if (!m_isLogin)
    {
        ERRE_SDK("is not logined.");

        return -1;
    }

    if (!reader.parse(jin, jvalue, false))
    {
        return -1;
    }

    jvalue["userKey"] = m_userKey;

    jin = writer.write(jvalue);

    string tmpjin = sys->ascii2utf8(jin);

    HANDLE sess = requestSesson();

    int res = post(sess, url, tmpjin, jout);

    releaseSession(sess);

    return res;
}

int
HttpCu::recvMsg(string &method, int &type, int &status, string &json)
{
    string strMsg = "";
    int    cnt    = 0;

    while (m_mainThread.active())
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
            if (!length || length >= 102400)
            {
                break;
            }

            char buf[102400] = {0};
            if (m_sock->read(buf, length) <= 0)
            {
                return -1;
            }

            strMsg += buf;
            break;
        }
    }

    HttpMsg httpMsg(strMsg.c_str(), strMsg.size());

    string url = httpMsg.GetReqUrl();
	method = httpMsg.GetMetthod();
	status = httpMsg.GetStatusCode();
	json = httpMsg.GetBody();

    if ("/login" == url)
    {
        type = MSG_LOGIN;
    }
    else if ("/heartbeat" == url)
    {
        type = MSG_HEARTBEAT;
    }
    else if ("/push" == url)
    {
        type = MSG_PUSH;
    }
	else if (httpMsg.GetStatusCode() == 200 && json.size() > 130)
	{
		type = MSG_PUSH;
	}
	else
	{
		type = MSG_HEARTBEAT;
	}

    return 0;
}

HANDLE
HttpCu::requestSesson()
{
    CURLcode res = CURLE_OK;

    CURL * url = curl_easy_init();

    if (NULL == url)
    {
        ERRE_SDK("get a easy curl handle failed.");
        goto requestSessonExit;
    }

    CURL_EASY_SETOPT(url, CURLOPT_VERBOSE, 0L);
    CURL_EASY_SETOPT(url, CURLOPT_NOPROGRESS, 1L);
    CURL_EASY_SETOPT(url, CURLOPT_TIMEOUT, 3L);

    if (res != CURLE_OK)
    {
        goto requestSessonExit;
    }

    return (HANDLE)url;

requestSessonExit:
    return NULL;
}

void
HttpCu::releaseSession(HANDLE &sess)
{
    CURL * url = (CURL *)(sess);

    curl_easy_cleanup(url);

    sess = NULL;
}

int
HttpCu::post(HANDLE sess, string url, string jin, string& jout)
{
    if (!sess)
    {
        ERRE_SDK("sess == NULL");
        return -1;
    }

    CURL *curl = (CURL *)sess;

    if (!curl)
    {
        ERRE_SDK("curl == NULL");
        return -1;
    }

    string strUrl = "http://";
    strUrl += m_host.toStr();

    /* HTTPCU_DEBUG("\nHTTP: DoPost %s\n %s\n\n", url.c_str(), jin.c_str()); */

    CURLcode res = CURLE_OK;

    strUrl = strUrl + url;

    CURL_EASY_SETOPT(curl, CURLOPT_URL, strUrl.c_str());

    CURL_EASY_SETOPT(curl, CURLOPT_HEADER, 1L);
    CURL_EASY_SETOPT(curl, CURLOPT_POST, 1L);

    struct curl_slist *headers = NULL;
    headers = curl_slist_append(headers, "Content-Type: application/json");
    headers = curl_slist_append(headers, "Connection: Keep-Alive");
    CURL_EASY_SETOPT(curl, CURLOPT_HTTPHEADER, headers);

    CURL_EASY_SETOPT(curl, CURLOPT_POSTFIELDSIZE, jin.size());
    CURL_EASY_SETOPT(curl, CURLOPT_READFUNCTION, read_callback);

    READ_INFO_S readInfo;
    readInfo.readPtr = (char *)jin.c_str();
    readInfo.readSize = jin.size();

    CURL_EASY_SETOPT(curl, CURLOPT_READDATA, &readInfo);

    CURL_EASY_SETOPT(curl, CURLOPT_WRITEFUNCTION, post_resp_callback);
	string resBuf;
    //m_response.clear();
	CURL_EASY_SETOPT(curl, CURLOPT_WRITEDATA, &resBuf);
    CURL_EASY_PERFORM(curl);

    curl_slist_free_all(headers);

    /*
     * curl_easy_perform为阻塞函数，所以这里可以不用做事件等待相关操作
     */
    if (CURLE_OK != res)
    {
		ERRE_SDK("CURLE_OK != res,url = %s", url.c_str());
        return -1;
    }

    /* note: code type is long, otherwise it will abort on linux */
    long code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    if (200 != code)
    {
		HttpMsg rspMsg(resBuf.c_str(), resBuf.size());

        SHL_Json::Value  reason;
        SHL_Json::Reader reader;
        reader.parse(rspMsg.GetBody().c_str(), reason, false);
        ERRE_SDK("http post error code: %d, reason: %s. url = %s", code, reason["reason"].asCString(),url.c_str());

        return -1;
    }

	HttpMsg rspMsg(resBuf.c_str(), resBuf.size());

    jout = rspMsg.GetBody();
    /* jout = sys->utf2ascii(jout); */

    return 0;
}
