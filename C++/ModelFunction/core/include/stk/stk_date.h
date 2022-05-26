/**
 * (C) Copyright 2013, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Nov 26 21:39:20 PST 2013
 *
 * stk_date.h  defines.
 */
#ifndef __STK_DATE_H__
#define __STK_DATE_H__

#include "stk_base.h"

namespace stk
{
class STK_API Date : public MsgField
{
public:
    Date();
    Date(const Date &src);
    Date(const char *buf);
    virtual ~Date();

public:
    Date & operator= (const Date& oSrc);

    virtual	void 	clear();
    virtual	bool	isvalid() const;
    virtual int32_t	to_str(string & str);
    virtual int32_t	parse(const char * szBuf, int nBufLen);

public:
    string		m_strMethod;    /**< Date method */
    string		m_strNumber;    /**< Date number */
};
}

#endif
