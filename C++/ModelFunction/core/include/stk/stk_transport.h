/**
 * (C) Copyright 2013, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Nov 26 21:39:20 PST 2013
 *
 * stk_transport.h  defines.
 */
#ifndef __STK_TRANSPORT_H__
#define __STK_TRANSPORT_H__

#include "stk_base.h"

namespace stk
{
class STK_API Transport : public MsgField
{
public:
    Transport();
    Transport(const Transport &src);
    Transport(const char *buf, int len);
    virtual ~Transport();

    Transport & operator= (const Transport &src);
public:

    virtual	void 	clear();
    virtual	bool	isvalid() const;
    virtual int32_t	to_str(string &str);
    virtual int32_t	parse(const char *buf, int len);
public:
    inline  void    SetMediaType(const char *szValue)
    {
        m_strMediaType = szValue;
    }
    inline  void    SetCastType(const char *szValue)
    {
        m_strCastType  = szValue;
    }

public:
    void            SetParamByName(const char *szName, const char *szValue);
    string          GetParamByName(const char *szName);

    void            SetMode(const char *szValue);
    void            SetDestination(const char *szValue);
    void            SetSource(const char *szValue);
    void            SetServerPort(const char *szValue);
    void            SetClientPort(const char *szValue);
    void            SetSSrc(const char *szValue);

    string          GetMode();
    string          GetDestination();
    string          GetSource();
    string          GetServerPort();
    string          GetClientPort();
    string          GetSSrc();

public:
    string          m_strMediaType;
    string          m_strCastType;
    MsgParamList 	m_listParams;		/**< Content-Type parameters */
};
}

#endif
