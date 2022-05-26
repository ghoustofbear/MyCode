/**
 * (C) Copyright 2013, XXX Technology  Co., Ltd.
 * Zhong caiwang <caiwang213@163.com>
 *
 * Tue Nov 26 21:39:20 PST 2013
 *
 * stk_message_http.h defines.
 */
#ifndef __HTTP_MESSAGE_BASE_H__
#define __HTTP_MESSAGE_BASE_H__

#include "stk_message_base.h"
#include "stk_host.h"
#include "stk_connection.h"
namespace stk
{
class STK_API HttpMsg : public MsgBase
{
public:
    HttpMsg();
    HttpMsg(const char *str, int32_t len);
    HttpMsg(const HttpMsg &src);
    ~HttpMsg();

    HttpMsg &    operator= (const HttpMsg &src);

    int32_t	     to_str(string &str);
    int32_t      parse(const char *str, int32_t len);

public:
    int32_t      SetHost(const char *szValue);
    int32_t      SetHost(const Host &field);
    string       GetHost();

    /**
     * @brief 获取Host结构
     *
     * @param [in] pos 指定第几个
     *             0 - 第一个
     *            -1 - 最后一个
     *             n - 第n个
     * @return 成功返回Host指针，失败返回NULL
     */
    Host       * GetHost(int32_t pos);

    int32_t      SetConnection(const char *szValue);
    int32_t      SetConnection(const Connection &field);
    string       GetConnection();
    Connection * GetConnection(int32_t pos);

    int32_t      SetLocation(const char *szValue);

protected:
    void         init();
    void         clear();

    int32_t      startline_parse(const char *buf, const char **headers);
    int32_t	     startline_parsereq(const char *buf, const char **headers);
    int32_t	     startline_parseresp(const char *buf, const char **headers);

private:
    MsgFieldList hosts;               /**< Host field of header */
    MsgFieldList connections;         /**< Connection field of header */
};
}


#endif
