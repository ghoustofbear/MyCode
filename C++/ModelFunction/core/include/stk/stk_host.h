/**
 * (C) Copyright 2013, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Nov 26 21:39:20 PST 2013
 *
 * stk_host.h  defines.
 */
#ifndef __STK_HOST_H__
#define __STK_HOST_H__

#include "stk_base.h"

namespace stk
{
class STK_API Host : public MsgField
{
public:
    Host();
    Host(const Host &src);
    Host(const char *buf, int len);
    virtual ~Host();

    Host & operator= (const Host &src);
public:

    virtual	void 	clear();
    virtual	bool	isvalid() const;
    virtual int32_t	to_str(string &str);
    virtual int32_t	parse(const char *buf, int len);

public:
    string		    m_strValue;                /**< value for Host (size of attachments) */
};
}

#endif
