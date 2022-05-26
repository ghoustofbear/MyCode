/**
 * @file      http_cu.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     http protocol client unit defines
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:33:59
 *
 * @note
 *  http_cu.h defines
 */

#ifndef __HTTP_CU_H__
#define __HTTP_CU_H__

#include "cu.h"

#define CURL_EASY_SETOPT(A, B, C) \
    if ((res = curl_easy_setopt((A), (B), (C))) != CURLE_OK) \
    fprintf(stderr, "curl_easy_setopt(%s, %s, %s) failed: %d\n", \
#A, #B, #C, res);

#define CURL_EASY_PERFORM(A) \
    if ((res = curl_easy_perform((A))) != CURLE_OK) \
    fprintf(stderr, "curl_easy_perform(%s) failed: %d\n", #A, res);

class HttpCu : public Cu
{
public:
    HttpCu(string host);
    ~HttpCu();

    virtual int        init();
    virtual void       uninit();

    virtual int        login();
    virtual void       logout(long &);
    virtual long       getServerInfo(c2_servinfo_s *info);

    virtual int        heartbeat();
    virtual int        remoteCall(uint32_t call, string jin, string &jout);

protected:
    virtual int        recvMsg(string &method, int &type, int &status, string &json);

    virtual HANDLE     requestSesson();
    virtual void       releaseSession(HANDLE &sess);
    int                post(HANDLE sess, string url, string jin, string& jout);

private:
    string             m_response;
};

#endif
