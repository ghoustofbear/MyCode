/**
 * (C) Copyright 2013, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Nov 26 21:39:20 PST 2013
 *
 * stk_content_type.h  defines.
 */
#ifndef __STK_CONTENT_TYPE_H__
#define __STK_CONTENT_TYPE_H__

#include "stk_base.h"

namespace stk
{
class STK_API ContentType : public MsgField
{
public:
    ContentType();
    ContentType(const ContentType &src);
    ContentType(const char *buf);
    virtual ~ContentType();

public:
    ContentType & operator= (const ContentType& oSrc);

    virtual	void 	clear();
    virtual	bool	isvalid() const;
    virtual int32_t	to_str(string & str);
    virtual int32_t	parse(const char * szBuf, int nBufLen);

public:
    string		    m_strType;			/**< Type of attachement */
    string		    m_strSubType;		/**< Sub-Type of attachement */
    MsgParamList 	m_listParams;		/**< Content-Type parameters */
};
}

#endif
