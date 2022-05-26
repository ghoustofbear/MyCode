/**
 * (C) Copyright 2013, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Nov 26 21:39:20 PST 2013
 *
 * stk_accept.h  defines.
 */
#ifndef __STK_ACCEPT_H__
#define __STK_ACCEPT_H__

#include "stk_base.h"

namespace stk
{
class STK_API Accept : public MsgField
{
public:
    Accept();
    Accept(const Accept &src);
    Accept(const char *buf, int len);
    virtual ~Accept();

    Accept & operator= (const Accept &src);
public:

    virtual	void 	   clear();
    virtual	bool	   isvalid() const;
    virtual int32_t	   to_str(string &str);
    virtual int32_t	   parse(const char *buf, int len);

public:
    string		       m_value;                         /**< value for Accept (size of attachments) */
};
}

#endif
