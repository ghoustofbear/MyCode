/**
 * (C) Copyright 2013, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Nov 26 21:39:20 PST 2013
 *
 * stk_server.h  defines.
 */
#ifndef __STK_SERVER_H__
#define __STK_SERVER_H__

#include "stk_base.h"

namespace stk
{
class STK_API Server : public MsgField
{
public:
    Server();
    Server(const Server &src);
    Server(const char *buf, int len);
    virtual ~Server();

    Server & operator= (const Server &src);
public:

    virtual	void 	clear();
    virtual	bool	isvalid() const;
    virtual int32_t	to_str(string &str);
    virtual int32_t	parse(const char *buf, int len);

public:
    string		    m_strValue;                /**< value for Server (size of attachments) */
};
}

#endif
