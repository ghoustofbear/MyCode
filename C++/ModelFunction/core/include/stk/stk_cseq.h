/**
 * (C) Copyright 2013, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Nov 26 21:39:20 PST 2013
 *
 * stk_cseq.h  defines.
 */
#ifndef __STK_CSEQ_H__
#define __STK_CSEQ_H__

#include "stk_base.h"

namespace stk
{
class STK_API CSeq : public MsgField
{
public:
    CSeq();
    CSeq(const CSeq &src);
    CSeq(const char *buf);
    virtual ~CSeq();

public:
    CSeq & operator= (const CSeq& oSrc);

    virtual	void 	clear();
    virtual	bool	isvalid() const;
    virtual int32_t	to_str(string & str);
    virtual int32_t	parse(const char * szBuf, int nBufLen);

public:
    string		m_strMethod;    /**< CSeq method */
    string		m_strNumber;    /**< CSeq number */
};
}

#endif
