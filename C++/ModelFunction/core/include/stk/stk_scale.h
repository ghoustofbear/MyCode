/**
 * (C) Copyright 2013, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Nov 26 21:39:20 PST 2013
 *
 * stk_scale.h  defines.
 */
#ifndef __STK_SCALE_H__
#define __STK_SCALE_H__

#include "stk_base.h"

namespace stk
{
class STK_API Scale : public MsgField
{
public:
    Scale();
    Scale(const Scale &src);
    Scale(const char *buf, int len);
    virtual ~Scale();

    Scale & operator= (const Scale &src);
public:

    virtual	void 	clear();
    virtual	bool	isvalid() const;
    virtual int32_t	to_str(string &str);
    virtual int32_t	parse(const char *buf, int len);

public:
    string		    m_strValue;                /**< value for Scale (size of attachments) */
};
}

#endif
