/**
 * (C) Copyright 2013, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Nov 26 21:39:20 PST 2013
 *
 * stk_content_base.h  defines.
 */
#ifndef __STK_CONTENT_BASE_H__
#define __STK_CONTENT_BASE_H__

#include "stk_base.h"

namespace stk
{
class STK_API ContentBase : public MsgField
{
public:
    ContentBase();
    ContentBase(const ContentBase &src);
    ContentBase(const char *buf, int len);
    virtual ~ContentBase();

    ContentBase & operator= (const ContentBase &src);
public:

    virtual	void 	clear();
    virtual	bool	isvalid() const;
    virtual int32_t	to_str(string &str);
    virtual int32_t	parse(const char *buf, int len);

public:
    string		    m_strValue;                /**< value for ContentBase (size of attachments) */
};
}

#endif
