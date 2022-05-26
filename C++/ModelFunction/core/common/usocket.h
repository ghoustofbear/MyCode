/**
 * @file      usocket.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:29:35
 *   
 * @note
 *  usocket.h defines 
 */

#ifndef __USOCKET_H__
#define __USOCKET_H__

#include "socket.h"

class USocket : public Socket
{
public:
    USocket();
    virtual ~USocket();

    virtual int        open(Host &, int type = IPPROTO_TCP);
    virtual int        bind(Host &, int type = IPPROTO_TCP);
    virtual void       close();
    virtual int        connect();
    virtual Socket   * accept();

    virtual void       setReadTimeout (uint32_t ms);
    virtual void       setWriteTimeout(uint32_t ms);

    /* 0: timeout, 小于0: error, 大于0: ok */
    virtual int        select(bool r, bool w, int timeoMs);

    /* 0: timeout, 小于0: error, 大于0: ok */
    virtual int        checkTimeout(bool r, bool w);


    virtual void       setBlocking(bool);

    virtual int        read(void *buf, int size);
    virtual int        write(const void *buf, int size);

    virtual int        readFrom(void *buf, int size, sockaddr *sa, int *saLen);
    virtual int        writeTo(const void *buf, int size, sockaddr *sa = NULL, int saLen = 0);

    virtual string     getPeerIP();
    virtual int        getPeerPort();

    static  void       init();

private:
    int                m_flags;
};

#endif
