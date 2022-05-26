/**
 * (C) Copyright 2013, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Nov 26 21:39:20 PST 2013
 *
 * stk_message_base.h  defines.
 */
#ifndef __STK_MESSAGE_BASE_H__
#define __STK_MESSAGE_BASE_H__

#include "stk_base.h"
#include "stk_content_type.h"
#include "stk_content_length.h"
#include "stk_cseq.h"
#include "stk_date.h"
#include "stk_body.h"
#include "stk_server.h"
#include "stk_public.h"
#include "stk_content_base.h"
#include "stk_cache_control.h"
#include "stk_range.h"

#define   MAX_MESSAGE_SIZE      (2*1024*1024)
#define   NUMBER_OF_HEADERS     (23)
#define   HASH_TABLE_SIZE       (200)

namespace stk
{
class STK_API MsgBase
{
public:
    enum
    {
        MSG_TYPE_SIP = 0,
        MSG_TYPE_HTTP,
        MSG_TYPE_RTSP,
        MSG_TYPE_SMTP,
        MSG_TYPE_SOAP,
        MSG_TYPE_END
    };

    MsgBase();
    virtual ~MsgBase();

    MsgBase & operator= (const MsgBase &src);

    virtual int32_t        to_str(string &str) = 0;
    virtual int32_t        parse(const char *buf, int len) = 0;

    uint32_t               GetMsgType()
    {
        return m_iMsgType;
    }
    uint32_t               GetStatusCode()
    {
        return m_iStatusCode;
    }
    string                 GetMetthod()
    {
        return m_strMethod;
    }
    string                 GetReqUrl()
    {
        return m_strReqUrl;
    }
    string                 GetVersion()
    {
        return m_strVersion;
    }

public:
    /* for all */
    virtual int32_t    SetMethod(const char *szValue);
    virtual int32_t    SetReqUrl(const char *szValue);
    virtual int32_t    SetVersion(const char *szValue);
    virtual int32_t    SetBody(const char *szValue);
    virtual int32_t    SetBody(const Body &field);

    virtual int32_t    SetAccept(const char *szValue);
    virtual int32_t    SetSetAcceptEncoding(const char *szValue);
    virtual int32_t    SetAcceptLanguage(const char *szValue);
    virtual int32_t    SetAuthorization(const char *szValue);

    virtual int32_t    SetCSeq(const char *szValue);
    virtual int32_t    SetCSeq(const CSeq &field);

    virtual int32_t    SetContentEncoding(const char *szValue);
    virtual int32_t    SetContentType(const char *szValue);
    virtual int32_t    SetContentType(const ContentType &field);

    virtual int32_t    SetContentLength(const char *szValue);
    virtual int32_t    SetContentLength(const int32_t length);

    virtual int32_t    SetContentBase(const char *szValue);
    virtual int32_t    SetContentBase(const ContentBase &field);

    virtual int32_t    SetDate(const char *szValue);
    virtual int32_t    SetDate(const Date &field);

    virtual int32_t    SetServer(const char *szValue);
    virtual int32_t    SetServer(const Server &field);

    virtual int32_t    SetPublic(const char *szValue);
    virtual int32_t    SetPublic(const Public &field);

    virtual int32_t    SetCacheControl(const char *szValue);
    virtual int32_t    SetCacheControl(const CacheControl &field);

    virtual int32_t    SetRange(const char *szValue);
    virtual int32_t    SetRange(const Range &field);

    virtual int32_t    SetContentLanguage(const char *szValue);
    virtual int32_t    SetUserAgent(const char *szValue);

    virtual int32_t    SetWwwAuthenticate(const char *szValue);

    virtual string     GetCSeq();
    virtual string     GetContentType();
    virtual string     GetContentLength();
    virtual int32_t    GetContentLength_int();
    virtual string     GetContentBase();

    virtual string     GetDate();
    virtual string     GetServer();
    virtual string     GetPublic();
    virtual string     GetCacheControl();
    virtual string     GetRange();

    virtual string     GetBody();

    /**
     * @brief 获取Body结构
     *
     * @param [in] pos 指定第几个
     *             0 - 第一个
     *            -1 - 最后一个
     *             n - 第n个
     * @return 成功返回Body指针，失败返回NULL
     */
    virtual Body       * GetBody(int pos);

    /* for http */
    virtual int32_t    SetHost(const char *szValue)
    {
        return 0;
    };
    virtual int32_t    SetConnection(const char *szValue)
    {
        return 0;
    };
    virtual int32_t    SetLocation(const char *szValue)
    {
        return 0;
    };

    virtual string     GetConnection()
    {
        return 0;
    };

    /* for sip */

    /* for rtsp */
    virtual int32_t    SetTransport(const char *szValue)
    {
        return 0;
    };
    virtual string     GetTransport()
    {
        return 0;
    };

    virtual int32_t    SetSession(const char *szValue)
    {
        return 0;
    };
    virtual string     GetSession()
    {
        return 0;
    };

    virtual int32_t    SetScale(const char *szValue)
    {
        return 0;
    };
    virtual string     GetScale()
    {
        return 0;
    };

protected:
    void               init();
    void               clear();

    int32_t            MakeStartLineString(string &str);
    int32_t            MakeFieldString(MsgField &cField, const string strFieldHeader, string &strField);
    int32_t            MakeSingleLineFieldString(MsgFieldList &listField, const string strFieldHeader, string &strField);
    int32_t            MakeMultiLineFieldString(MsgFieldList &listField, const string strFieldHeader, string &strField);

    int32_t            msg_headers_parse(const char *start_of_header, const char **body);
    int32_t            msg_set_multiple_header(char *hname, char *hvalue);
    int32_t            msg_set_header(const char *hname, const char *hvalue);
    int32_t            msg_body_parse(const char *start_of_buf, const char **next_body, size_t length);
private:
    uint32_t           Hash(const char *str);

protected:
    uint32_t           m_iMsgType;
    string             m_strVersion;
    string             m_strMethod;
    string             m_strReqUrl;
    string             m_strReasonPhrase;

    bool               m_bValid;
    int32_t            m_iStatusCode;          /**< Status Code (SIP answer only) */
    int32_t            m_iContentLength;

    string             m_strMessage;
    int32_t            m_iMessageProperty;     /**< 1: structure and buffer "message" are identical.
                                                        *   2: buffer "message" is not up to date with the structure info (call to_str to update it).
                                                        */
    CSeq               cseqs;                  /**< CSeq header */
    ContentType        content_types;          /**< Content-Type field of header */
    ContentLength      content_lengths;        /**< Content-Length field of header */
    ContentBase        content_bases;          /**< Content-Base field of header */

    Date               dates;                  /**< Date header */
    Server             servers;                /**< Server header */
    CacheControl       cache_controls;         /**< Cache_Control header */
    MsgFieldList       publics;                /**< Public header */
    //MsgFieldList       Accepts;                /**< Accept header */
    //Vary               varys;                  /**< Vary header */
    Range              ranges;                 /**< Range header */
    //IfModifiedSince    if_modified_sinces      /**< If-Modified-Since header */


    MsgFieldList       bodies;                 /**< List of attachements */

private:
    static bool        m_config;
};
}
#endif
