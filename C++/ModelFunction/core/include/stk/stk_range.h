/**
 * (C) Copyright 2013, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Nov 26 21:39:20 PST 2013
 *
 * stk_range.h  defines.
 */
#ifndef __STK_RANGE_H__
#define __STK_RANGE_H__

#include "stk_base.h"

namespace stk
{
class STK_API Range : public MsgField
{
public:
    Range();
    Range(const Range &src);
    Range(const char *buf, int len);
    virtual ~Range();

    Range & operator= (const Range &src);
public:

    virtual	void 	clear();
    virtual	bool	isvalid() const;
    virtual int32_t	to_str(string &str);
    virtual int32_t	parse(const char *buf, int len);

public:
    string		    m_strValue;                /**< value for Range (size of attachments) */
};
}

#endif
