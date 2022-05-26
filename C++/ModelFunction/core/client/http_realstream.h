/**
 * @file      http_realstream.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     http protocol implement real stream
 * @author    caiwang213@qq.com
 * @date      2017-06-09 11:48:27
 *   
 * @note
 *  http_realstream.h defines 
 */
#ifndef __HTTP_REALSTREAM_H__
#define __HTTP_REALSTREAM_H__

#include "realstream.h"

class HttpRealStream : public RealStream
{
public:
    HttpRealStream(Cu *cu);
    virtual ~HttpRealStream();

protected:
    virtual int        open();
    virtual int        recv();

protected:
    int                recvMsg(string &method, string &url, int &status, string &json);

    int                recvFrame();

private:
    

};

#endif /* __HTTP_REALSTREAM_H__ */
