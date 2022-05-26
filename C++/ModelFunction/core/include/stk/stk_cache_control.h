/**
 * (C) Copyright 2013, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Nov 26 21:39:20 PST 2013
 *
 * stk_cache_control.h  defines.
 */
#ifndef __STK_CACHE_CONTROL_H__
#define __STK_CACHE_CONTROL_H__

#include "stk_base.h"

namespace stk
{
class STK_API CacheControl : public MsgField
{
public:
    CacheControl();
    CacheControl(const CacheControl &src);
    CacheControl(const char *buf, int len);
    virtual ~CacheControl();

    CacheControl & operator= (const CacheControl &src);
public:

    virtual	void 	clear();
    virtual	bool	isvalid() const;
    virtual int32_t	to_str(string &str);
    virtual int32_t	parse(const char *buf, int len);

public:
    string		    m_strValue;                /**< value for CacheControl (size of attachments) */
};
}

#endif
