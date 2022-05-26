#include "rtspclient.h"
#include "debug.h"
#include "osipparser2/sdp_message.h"

#ifdef __LINUX__
#include <strings.h>
#endif

#include <string.h>

#define CURL_EASY_SETOPT(A, B, C) \
    if ((res = curl_easy_setopt((A), (B), (C))) != CURLE_OK) \
    fprintf(stderr, "curl_easy_setopt(%s, %s, %s) failed: %d\n", \
            #A, #B, #C, res);

#define CURL_EASY_PERFORM(A) \
    if ((res = curl_easy_perform((A))) != CURLE_OK) \
    fprintf(stderr, "curl_easy_perform(%s) failed: %d\n", #A, res);

RtspClient::RtspClient()
{	
    m_s32ContentLen = 0;
    m_pUser = NULL;
}

bool RtspClient::start(const char *strUrl, void *pUser)
{
    m_strUrl = strUrl;
    m_pUser = pUser;
    CURLcode res = CURLE_OK;
    m_pCurl = curl_easy_init();
    if (NULL == m_pCurl)
    {
        LOGI_MSV("get a easy handle failed.\n");
        return false;
    }
    else
    {
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_VERBOSE, 0L);
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_NOPROGRESS, 1L);

        char *requrl = (char *)strchr(strUrl, '@');
        if (NULL == requrl)
        {
            return false;
        }
        char url[1024] = { 0 };
        strncpy(url, strUrl, (requrl - strUrl));

        CURL_EASY_SETOPT(m_pCurl, CURLOPT_URL, url);

        if (res != CURLE_OK)
        {
            curl_easy_cleanup(m_pCurl);
            m_pCurl = NULL;
        }
    }

    return true;
}

void RtspClient::stop()
{
    if (NULL != m_pCurl)
    {
        curl_easy_cleanup(m_pCurl);
        m_pCurl = NULL;
    }
}

RtspClient::~RtspClient()
{
    stop();
}

size_t option_callback(const char *ptr, size_t size, size_t nmemb, void *user)
{
    RtspClient *rtsp = (RtspClient *)user;
    if (rtsp)
    {
        rtsp->m_strResponse += ptr;
        if (strncasecmp(ptr, "Content-Length", 14) == 0)
        {
            const char *p = strstr(ptr, ":");
            rtsp->m_s32ContentLen = atoi(p + 1);
        }
        if ((strcmp("\r\n", ptr) == 0 &&
            rtsp->m_s32ContentLen == 0) ||
            (rtsp->m_s32ContentLen != 0 &&
            (int)(size*nmemb) == rtsp->m_s32ContentLen)
            )
        {
            stk::RtspMsg rtspMsg(rtsp->m_strResponse.c_str(), rtsp->m_strResponse.size());
            rtsp->onOptionRsp(rtspMsg, rtsp->m_pUser);
        }
    }

    return size*nmemb;
}

size_t describle_callback(const char *ptr, size_t size, size_t nmemb, void *user)
{
    RtspClient *rtsp = (RtspClient *)user;
    if (rtsp)
    {
        rtsp->m_strResponse += ptr;
        if (strncasecmp(ptr, "Content-Length", 14) == 0)
        {
            const char *p = strstr(ptr, ":");
            rtsp->m_s32ContentLen = atoi(p + 1);
        }
        if ((strcmp("\r\n", ptr) == 0 &&
            rtsp->m_s32ContentLen == 0) ||
            (rtsp->m_s32ContentLen != 0 &&
            (int)(size*nmemb) == rtsp->m_s32ContentLen)
            )
        {
            stk::RtspMsg rtspMsg(rtsp->m_strResponse.c_str(), rtsp->m_strResponse.size());
            rtsp->onDescribeRsp(rtspMsg, rtsp->m_pUser);
        }
    }

    return size*nmemb;
}

size_t setup_callback(const char *ptr, size_t size, size_t nmemb, void *user)
{
    RtspClient *rtsp = (RtspClient *)user;
    if (rtsp)
    {
        rtsp->m_strResponse += ptr;
        if (strncasecmp(ptr, "Content-Length", 14) == 0)
        {
            const char *p = strstr(ptr, ":");
            rtsp->m_s32ContentLen = atoi(p + 1);
        }
        if ((strcmp("\r\n", ptr) == 0 &&
            rtsp->m_s32ContentLen == 0) ||
            (rtsp->m_s32ContentLen != 0 &&
            (int)(size*nmemb) == rtsp->m_s32ContentLen)
            )
        {
            stk::RtspMsg rtspMsg(rtsp->m_strResponse.c_str(), rtsp->m_strResponse.size());
            rtsp->onSetupRsp(rtspMsg, rtsp->m_pUser);
        }
    }

    return size*nmemb;
}

size_t play_callback(const char *ptr, size_t size, size_t nmemb, void *user)
{
    RtspClient *rtsp = (RtspClient *)user;
    if (rtsp)
    {
        rtsp->m_strResponse += ptr;
        if (strncasecmp(ptr, "Content-Length", 14) == 0)
        {
            const char *p = strstr(ptr, ":");
            rtsp->m_s32ContentLen = atoi(p + 1);
        }
        if ((strcmp("\r\n", ptr) == 0 &&
            rtsp->m_s32ContentLen == 0) ||
            (rtsp->m_s32ContentLen != 0 &&
            (int)(size*nmemb) == rtsp->m_s32ContentLen)
            )
        {
            stk::RtspMsg rtspMsg(rtsp->m_strResponse.c_str(), rtsp->m_strResponse.size());
            rtsp->onPlayRsp(rtspMsg, rtsp->m_pUser);
        }
    }

    return size*nmemb;
}

size_t pause_callback(const char *ptr, size_t size, size_t nmemb, void *user)
{
    RtspClient *rtsp = (RtspClient *)user;
    if (rtsp)
    {
        rtsp->m_strResponse += ptr;
        if (strncasecmp(ptr, "Content-Length", 14) == 0)
        {
            const char *p = strstr(ptr, ":");
            rtsp->m_s32ContentLen = atoi(p + 1);
        }
        if ((strcmp("\r\n", ptr) == 0 &&
            rtsp->m_s32ContentLen == 0) ||
            (rtsp->m_s32ContentLen != 0 &&
            (int)(size*nmemb) == rtsp->m_s32ContentLen)
            )
        {
            stk::RtspMsg rtspMsg(rtsp->m_strResponse.c_str(), rtsp->m_strResponse.size());
            rtsp->onPauseRsp(rtspMsg, rtsp->m_pUser);
        }
    }

    return size*nmemb;
}

size_t teardown_callback(const char *ptr, size_t size, size_t nmemb, void *user)
{
    RtspClient *rtsp = (RtspClient *)user;
    if (rtsp)
    {
        rtsp->m_strResponse += ptr;
        if (strncasecmp(ptr, "Content-Length", 14) == 0)
        {
            const char *p = strstr(ptr, ":");
            rtsp->m_s32ContentLen = atoi(p + 1);
        }
        if ((strcmp("\r\n", ptr) == 0 &&
            rtsp->m_s32ContentLen == 0) ||
            (rtsp->m_s32ContentLen != 0 &&
            (int)(size*nmemb) == rtsp->m_s32ContentLen)
            )
        {
            stk::RtspMsg rtspMsg(rtsp->m_strResponse.c_str(), rtsp->m_strResponse.size());
            rtsp->onTeardownRsp(rtspMsg, rtsp->m_pUser);
        }
    }

    return size*nmemb;
}

bool RtspClient::doOption()
{
    CURLcode res = CURLE_OK;
    if (NULL != m_pCurl)
    {		
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_HEADER, 1L);
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_RTSP_STREAM_URI, m_strUrl.c_str());
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_RTSP_REQUEST, (long)CURL_RTSPREQ_OPTIONS);
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_WRITEFUNCTION, option_callback);
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_WRITEDATA, this);
        CURL_EASY_PERFORM(m_pCurl);

        /*
        * curl_easy_perform为阻塞函数，所以这里可以不用做事件等待相关操作
        */
        if (CURLE_OK == res)
        {
            long lResponse = 0;
            curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &lResponse);
            if (200 == lResponse)
            {
                return true;
            }
        }
    }

    return false;
}

bool RtspClient::doDescribe()
{
    if (NULL != m_pCurl)
    {
        CURLcode res = CURLE_OK;
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_HEADER, 1L);
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_RTSP_REQUEST, (long)CURL_RTSPREQ_DESCRIBE);

        m_strSdp = "";
        m_strResponse = "";
        m_s32ContentLen = 0;
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_WRITEFUNCTION, describle_callback);
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_WRITEDATA, this);
        CURL_EASY_PERFORM(m_pCurl);

        if (CURLE_OK == res)
        {
            long lResponse = 0;
            curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &lResponse);
            if (200 == lResponse)
            {
                return true;
            }
        }
    }

    return false;
}

bool RtspClient::doSetup(const char *ps8Proto, const char *ps8Cast, int32_t s32BasePort, int32_t &s32DestPort)
{
    if (NULL != m_pCurl)
    {
        CURLcode res = CURLE_OK;
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_HEADER, 1L);

        char s8Tranport[100] = { 0 };
        snprintf(s8Tranport, 100, "%s;%s;client_port=%d-%d",
            ps8Proto,
            ps8Cast,
            s32BasePort,
            s32BasePort + 1);

        sdp_message_t *sdp;
        sdp_attribute *attr = NULL;

        sdp_message_init(&sdp);
        sdp_message_parse(sdp, m_strSdp.c_str());

        int s32I = 0;
        for (s32I = 0; s32I < 2; s32I++)
        {
            char *media = sdp_message_m_media_get(sdp, s32I);
            if (0 == strncmp(media, "video", 5))
            {
                attr = sdp_message_attribute_get(sdp, s32I, 3);
                if (attr)
                {
                    break;
                }
            }
        }
        if (s32I >= 2)
        {
            return false;
        }

        string strCtrl = attr->a_att_value;

        sdp_message_free(sdp);

        string strUrl = m_strUrl + "/" + strCtrl;

        CURL_EASY_SETOPT(m_pCurl, CURLOPT_RTSP_STREAM_URI, strUrl.c_str());
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_RTSP_TRANSPORT, s8Tranport);
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_RTSP_REQUEST, (long)CURL_RTSPREQ_SETUP);

        m_strSdp = "";
        m_strResponse = "";
        m_s32ContentLen = 0;
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_WRITEFUNCTION, setup_callback);
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_WRITEDATA, this)
        CURL_EASY_PERFORM(m_pCurl);

        if (CURLE_OK == res)
        {
            long lResponse = 0;
            curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &lResponse);
            if (200 == lResponse)
            {
                int32_t s32BasePort1 = 0;
                int32_t s32RtcpPort = 0;

                char *ps8Transport = (char*)m_strTransport.c_str();
                char *ps8ServPort = strstr(ps8Transport, "server_port=");
                if (ps8ServPort)
                {
                    sscanf(ps8ServPort + 12, "%d-%d", &s32BasePort1, &s32RtcpPort);
                    s32DestPort = s32BasePort1;
                    return true;
                }
            }
        }
    }

    return false;
}

bool RtspClient::doPlay(const char *ps8Scale, const char *ps8Range)
{
    if (NULL != m_pCurl)
    {
        CURLcode res = CURLE_OK;
        struct curl_slist *headers = NULL;
        if (ps8Scale)
        {
            string strScale = "Scale: ";
            strScale += ps8Scale;

            headers = curl_slist_append(headers, strScale.c_str());
            CURL_EASY_SETOPT(m_pCurl, CURLOPT_RTSPHEADER, headers);
        }

        CURL_EASY_SETOPT(m_pCurl, CURLOPT_HEADER, 1L);
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_RTSP_STREAM_URI, m_strUrl.c_str());
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_RANGE, ps8Range);
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_RTSP_REQUEST, (long)CURL_RTSPREQ_PLAY);

        m_strResponse = "";
        m_s32ContentLen = 0;
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_WRITEFUNCTION, play_callback);
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_WRITEDATA, this)
        CURL_EASY_PERFORM(m_pCurl);

        if (CURLE_OK == res)
        {
            long lResponse = 0;
            curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &lResponse);
            if (200 == lResponse)
            {
                return true;
            }
        }
    }

    return false;
}

bool RtspClient::doPause()
{
    if (NULL != m_pCurl)
    {
        CURLcode res = CURLE_OK;

        CURL_EASY_SETOPT(m_pCurl, CURLOPT_RTSP_STREAM_URI, m_strUrl.c_str());
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_RTSP_REQUEST, (long)CURL_RTSPREQ_PAUSE);

        m_strResponse = "";
        m_s32ContentLen = 0;
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_WRITEFUNCTION, pause_callback);
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_WRITEDATA, this)
        CURL_EASY_PERFORM(m_pCurl);

        if (CURLE_OK == res)
        {
            long lResponse = 0;
            curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &lResponse);
            if (200 == lResponse)
            {
                return true;
            }
        }
    }

    return false;
}

bool RtspClient::doTeardown()
{
    if (NULL != m_pCurl)
    {
        CURLcode res = CURLE_OK;

        CURL_EASY_SETOPT(m_pCurl, CURLOPT_RTSP_STREAM_URI, m_strUrl.c_str());
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_RTSP_REQUEST, (long)CURL_RTSPREQ_TEARDOWN);

        m_strResponse = "";
        m_s32ContentLen = 0;
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_WRITEFUNCTION, teardown_callback);
        CURL_EASY_SETOPT(m_pCurl, CURLOPT_WRITEDATA, this)
        CURL_EASY_PERFORM(m_pCurl);

        if (CURLE_OK == res)
        {
            long lResponse = 0;
            curl_easy_getinfo(m_pCurl, CURLINFO_RESPONSE_CODE, &lResponse);
            if (200 == lResponse)
            {
                return true;
            }
        }
    }

    return false;
}

void RtspClient::onOptionRsp(stk::RtspMsg &rsp, void *pUser)
{
    string strPublic = rsp.GetPublic();

    string strMsg;
    rsp.to_str(strMsg);
    printf("%s", strMsg.c_str());
}

void RtspClient::onDescribeRsp(stk::RtspMsg &rsp, void *pUser)
{
    string strSdp = rsp.GetBody();

    string strMsg;
    rsp.to_str(strMsg);
    printf("%s", strMsg.c_str());

    m_strSdp = rsp.GetBody();
}

void RtspClient::onSetupRsp(stk::RtspMsg &rsp, void *pUser)
{
    string strMsg;
    rsp.to_str(strMsg);
    printf("%s", strMsg.c_str());

    m_strTransport = rsp.GetTransport();
}

void RtspClient::onPlayRsp(stk::RtspMsg &rsp, void *pUser)
{
    string strMsg;
    rsp.to_str(strMsg);
    printf("%s", strMsg.c_str());
}

void RtspClient::onPauseRsp(stk::RtspMsg &rsp, void *pUser)
{
    string strMsg;
    rsp.to_str(strMsg);
    printf("%s", strMsg.c_str());
}

void RtspClient::onTeardownRsp(stk::RtspMsg &rsp, void *pUser)
{
    string strMsg;
    rsp.to_str(strMsg);
    printf("%s", strMsg.c_str());
}
