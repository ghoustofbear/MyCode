
/**
 * (C) Copyright 2013, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Nov 26 21:39:20 PST 2013
 *
 * stk_body.h  defines.
 */
#ifndef __STK_BODY_H__
#define __STK_BODY_H__

#include "stk_content_type.h"
namespace stk
{
class STK_API Body : public MsgField
{
public:
    Body();
    Body(const Body &src);
    Body(const char *buf, int len);
    virtual ~Body();

    Body & operator= (const Body &src);
public:

    virtual	void 	clear();
    virtual	bool	isvalid() const;
    virtual int32_t	to_str(string &str);
    virtual int32_t	parse(const char *buf, int len);

public:
    string		    m_strBody;                /**< value for Body (size of attachments) */

    //MsgFieldList    m_listMime;			      /**< List of headers (when mime is used) */
    ContentType 	m_ContentType;	     	  /**< Content-Type (when mime is used) */
};
}

#endif

