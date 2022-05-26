/**
 * (C) Copyright 2013, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Nov 26 21:39:20 PST 2013
 *
 * stk_session.h  defines.
 */
#ifndef __STK_SESSION_H__
#define __STK_SESSION_H__

#include "stk_base.h"

namespace stk
{
class STK_API Session : public MsgField
{
public:
    Session();
    Session(const Session &src);
    Session(const char *buf, int len);
    virtual ~Session();

    Session & operator= (const Session &src);
public:

    virtual	void 	clear();
    virtual	bool	isvalid() const;
    virtual int32_t	to_str(string &str);
    virtual int32_t	parse(const char *buf, int len);

public:
    string		    m_strValue;                /**< value for Session (size of attachments) */
};
}

#endif
