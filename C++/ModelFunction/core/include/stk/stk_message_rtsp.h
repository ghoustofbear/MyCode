/**
 * (C) Copyright 2013, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Nov 26 21:39:20 PST 2013
 *
 * stk_message_http.h defines.
 */
#ifndef __RTSP_MESSAGE_BASE_H__
#define __RTSP_MESSAGE_BASE_H__

#include "stk_message_base.h"
#include "stk_host.h"
#include "stk_connection.h"
#include "stk_transport.h"
#include "stk_session.h"
#include "stk_scale.h"

namespace stk
{
class STK_API RtspMsg : public MsgBase
{
public:
    RtspMsg();
    RtspMsg(const char *str, int32_t len);
    RtspMsg(const RtspMsg &src);
    ~RtspMsg();

    RtspMsg &    operator= (const RtspMsg &src);

    int32_t	     to_str(string &str);
    int32_t      parse(const char *str, int32_t len);

public:
    int32_t      SetTransport(const char *szValue);
    int32_t      SetTransport(const Transport &field);
    string       GetTransport();
    Transport  * GetTransport(int32_t pos);

    int32_t      SetSession(const char *szValue);
    int32_t      SetSession(const Session &field);
    string       GetSession();

    int32_t      SetScale(const char *szValue);
    int32_t      SetScale(const Scale &field);
    string       GetScale();
protected:
    void         init();
    void         clear();

    int32_t      startline_parse(const char *buf, const char **headers);
    int32_t	     startline_parsereq(const char *buf, const char **headers);
    int32_t	     startline_parseresp(const char *buf, const char **headers);

private:
    MsgFieldList hosts;               /**< Host field of header */
    MsgFieldList connections;         /**< Transport field of header */
    Session      sessions;            /**< Session field of header */
    MsgFieldList transports;          /**< Transport field of header */
    //RtpMessage rtp_messages;        /**< RTP-MESSAGE field of header */
    Scale        scales;              /**< Scale field of header */
};
}


#endif
