/**
 * (C) Copyright 2013, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Nov 26 21:39:20 PST 2013
 *
 * stk_content_length.h  defines.
 */
#ifndef __STK_CONTENT_LENGTH_H__
#define __STK_CONTENT_LENGTH_H__

#include "stk_base.h"

namespace stk
{
class STK_API ContentLength : public MsgField
{
public:
    ContentLength();
    ContentLength(const ContentLength &src);
    ContentLength(const char *buf, int len);
    virtual ~ContentLength();

public:
    ContentLength & operator = (const ContentLength &src);

    virtual	void 	clear();
    virtual	bool	isvalid() const;
    virtual int32_t	to_str(string &str);
    virtual int32_t	parse(const char *buf, int len);

public:
    string		    m_strValue;                /**< value for ContentLength (size of attachments) */
};
}

#endif

