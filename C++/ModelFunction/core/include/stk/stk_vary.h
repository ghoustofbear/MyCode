/**
 * (C) Copyright 2013, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Nov 26 21:39:20 PST 2013
 *
 * stk_vary.h  defines.
 */
#ifndef __STK_VARY_H__
#define __STK_VARY_H__

#include "stk_base.h"

namespace stk
{
class STK_API Vary : public MsgField
{
public:
    Vary();
    Vary(const Vary &src);
    Vary(const char *buf, int len);
    virtual ~Vary();

    Vary & operator= (const Vary &src);
public:

    virtual	void 	clear();
    virtual	bool	isvalid() const;
    virtual int32_t	to_str(string &str);
    virtual int32_t	parse(const char *buf, int len);

public:
    string		    m_strValue;                /**< value for Vary (size of attachments) */
};
}

#endif
