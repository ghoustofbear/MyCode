/**
 * (C) Copyright 2013, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Nov 26 21:39:20 PST 2013
 *
 * stk_rtp_message.h  defines.
 */
#ifndef __STK_RTP_MESSAGE_H__
#define __STK_RTP_MESSAGE_H__

#include "stk_base.h"

namespace stk
{
class STK_API RtpMessage : public MsgField
{
public:
    RtpMessage();
    RtpMessage(const RtpMessage &src);
    RtpMessage(const char *buf, int len);
    virtual ~RtpMessage();

    RtpMessage & operator= (const RtpMessage &src);
public:

    virtual	void 	clear();
    virtual	bool	isvalid() const;
    virtual int32_t	to_str(string &str);
    virtual int32_t	parse(const char *buf, int len);

public:
    string		    m_strValue;                /**< value for RtpMessage (size of attachments) */
};
}

#endif
