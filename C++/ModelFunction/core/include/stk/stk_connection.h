/**
 * (C) Copyright 2013, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Nov 26 21:39:20 PST 2013
 *
 * stk_connection.h  defines.
 */
#ifndef __STK_CONNECTION_H__
#define __STK_CONNECTION_H__

#include "stk_base.h"

namespace stk
{
class STK_API Connection : public MsgField
{
public:
    Connection();
    Connection(const Connection &src);
    Connection(const char *buf);
    virtual ~Connection();

public:
    Connection & operator= (const Connection &src);

    virtual	void 	clear();
    virtual	bool	isvalid() const;
    virtual int32_t	to_str(string &str);
    virtual int32_t	parse(const char *buf, int len);

public:
    string		    m_strValue;                /**< value for Connection (size of attachments) */
};
}

#endif

