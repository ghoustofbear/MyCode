/**
* @file      grsession.h
* @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
* @brief     brief
* @author    caiwang213@qq.com
* @date      2017-12-28 08:47:52
*
* @note
*  grsession.h defines
*/

#ifndef __RTSP_CLIENT_H__
#define __RTSP_CLIENT_H__

#include "curl/curl.h"
#include "stk/stk_message_rtsp.h"
#include <string>
#include "concurrency.h"

class RtspClient
{
public:

    friend size_t option_callback(const char *ptr, size_t size, size_t nmemb, void *user);
    friend size_t describle_callback(const char *ptr, size_t size, size_t nmemb, void *user);
    friend size_t setup_callback(const char *ptr, size_t size, size_t nmemb, void *user);
    friend size_t play_callback(const char *ptr, size_t size, size_t nmemb, void *user);
    friend size_t pause_callback(const char *ptr, size_t size, size_t nmemb, void *user);
    friend size_t teardown_callback(const char *ptr, size_t size, size_t nmemb, void *user);
    friend size_t get_parameter_callback(const char *ptr, size_t size, size_t nmemb, void *user);

    RtspClient(const char *ip, int port = 554);
    ~RtspClient();

    bool start(const char * strUrl, void *pUser);

    bool startex(const char * strUrl, void *pUser);

    void stop();

    bool doOption();

    bool doDescribe();

    bool doSetup(const char *ps8Proto, const char *ps8Cast, int32_t s32BasePort, int32_t &s32DestPort);

    bool doPlay(const char *ps8Scale, const char *ps8Range);

    bool doPause();

    bool doTeardown();

    bool doGetParameter();

    void onOptionRsp(stk::RtspMsg &rsp, void *pUser);

    void onDescribeRsp(stk::RtspMsg &rsp, void *pUser);

    void onSetupRsp(stk::RtspMsg &rsp, void *pUser);

    void onPlayRsp(stk::RtspMsg &rsp, void *pUser);

    void onPauseRsp(stk::RtspMsg &rsp, void *pUser);

    void onTeardownRsp(stk::RtspMsg &rsp, void *pUser);

    void onGetParameter(stk::RtspMsg &rsp, void *pUser);

private:
    CURL   *m_pCurl;
    std::string m_strUrl;
    std::string m_strResponse;
    std::string m_strSdp;
    std::string m_strTransport;
    int    m_s32ContentLen;
    void * m_pUser;
    std::string  m_strIp;
    int          m_rPort;

    Mutex        m_mtx;
};

#endif
