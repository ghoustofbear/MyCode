/**
 * (C) Copyright 2013, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Nov 26 21:39:20 PST 2013
 *
 * stk_public.h  defines.
 */
#ifndef __STK_PUBLIC_H__
#define __STK_PUBLIC_H__

#include "stk_base.h"

namespace stk
{
class STK_API Public : public MsgField
{
public:
    Public();
    Public(const Public &src);
    Public(const char *buf);
    virtual ~Public();

public:
    Public & operator= (const Public &src);

    virtual	void 	clear();
    virtual	bool	isvalid() const;
    virtual int32_t	to_str(string &str);
    virtual int32_t	parse(const char *buf, int len);

public:
    string		    m_strValue;                /**< value for Public (size of attachments) */
};
}

#endif

