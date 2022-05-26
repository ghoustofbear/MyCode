/*
 * @Author: liuxu
 * @Date: 2022-04-02 10:25:06
 * @LastEditors: liuxu
 * @LastEditTime: 2022-04-02 10:25:31
 * @FilePath: \C++\MyFFmpeg\src\MyFFmpeg\tls_error.cpp
 * @Description: 
 * 
 * Copyright (c) 2022 by liuxu/1056203382@qq.com, All Rights Reserved. 
 */
#include "tls_error.h"
#include "sys.h"
#include "utility.h"

long
setError(const char* fmt, ...)
{
    Error *error = (Error *)sys->getTLSValue();
    if (!error)
    {
#ifdef _WIN32
        error = (Error *)LocalAlloc(LPTR, sizeof(Error));
#else
        error = (Error *)malloc(sizeof(Error));
#endif
        sys->setTLSValue(error);
    }

    va_list ap;

    /* Ignore call if invalid format pointer was passed */
    if (fmt == NULL)
    {
        return -1;
    }

    /* Copy in the key, mark error as valid */
    error->code = 1;
    strncpy_s(error->key, fmt, sizeof(error->key));

    va_start(ap, fmt);
    error->argc = 0;

    while (*fmt)
    {
        if (*fmt++ == '%')
        {
            while (*fmt == '.' || (*fmt >= '0' && *fmt <= '9'))
            {
                ++fmt;
            }

            switch (*fmt++)
            {
            case 0:            /* Malformed format string.. */
                --fmt;
                break;

            case 'c':
            case 'i':
            case 'd':
            case 'u':
            case 'o':
            case 'x':
            case 'X':
                error->args[error->argc++].value_i = va_arg(ap, int);
                break;

            case 'f':
                error->args[error->argc++].value_f = va_arg(ap, double);
                break;

            case 'p':
                error->args[error->argc++].value_ptr = va_arg(ap, void *);
                break;

            case 's':
            {
                int i = error->argc;
                const char *str = va_arg(ap, const char *);
                if (str == NULL)
                {
                    str = "(null)";
                }

                strncpy_s(error->args[i].buf, str, ERR_MAX_STRLEN);
                error->argc++;
            }
            break;

            default:
                break;
            }

            if (error->argc >= ERR_MAX_ARGS)
            {
                break;
            }
        }
    }

    va_end(ap);

    return  0;
}

long
vsetError(const char* fmt, va_list ap)
{
    Error *error = (Error *)sys->getTLSValue();
    if (!error)
    {
#ifdef _WIN32
        error = (Error *)LocalAlloc(LPTR, sizeof(Error));
#else
        error = (Error *)malloc(sizeof(Error));
#endif
        sys->setTLSValue(error);
    }

    /* Ignore call if invalid format pointer was passed */
    if (fmt == NULL)
    {
        return -1;
    }

    /* Copy in the key, mark error as valid */
    error->code = 1;
    strncpy_s(error->key, fmt, sizeof(error->key));

    error->argc = 0;

    while (*fmt)
    {
        if (*fmt++ == '%')
        {
            while (*fmt == '.' || (*fmt >= '0' && *fmt <= '9'))
            {
                ++fmt;
            }

            switch (*fmt++)
            {
            case 0:            /* Malformed format string.. */
                --fmt;
                break;

            case 'c':
            case 'i':
            case 'd':
            case 'u':
            case 'o':
            case 'x':
            case 'X':
                error->args[error->argc++].value_i = va_arg(ap, int);
                break;

            case 'f':
                error->args[error->argc++].value_f = va_arg(ap, double);
                break;

            case 'p':
                error->args[error->argc++].value_ptr = va_arg(ap, void *);
                break;

            case 's':
            {
                int i = error->argc;
                const char *str = va_arg(ap, const char *);

                if (str == NULL)
                {
                    str = "(null)";
                }

                strncpy_s(error->args[i].buf, str, ERR_MAX_STRLEN);
                error->argc++;
            }
            break;

            default:
                break;
            }

            if (error->argc >= ERR_MAX_ARGS)
            {
                break;
            }
        }
    }

    return 0;
}

const char *
getError()
{
    Error *error = (Error *)sys->getTLSValue();
    if (!error)
    {
        return "No error";
    }

    char * errmsg = error->errmsg;
    int    maxlen = sizeof(error->errmsg);

    memset(errmsg, 0, maxlen);

    const char *fmt;
    char  *msg = errmsg;
    int   len;
    int   argi;

    fmt  = error->key;
    argi = 0;

    while (*fmt && (maxlen > 0))
    {
        if (*fmt == '%')
        {
            char tmp[128], *spot = tmp;
            *spot++ = *fmt++;

            while ((*fmt == '.' || (*fmt >= '0' && *fmt <= '9'))
                    && spot < (tmp + sizeof(tmp) - 2))
            {
                *spot++ = *fmt++;
            }

            *spot++ = *fmt++;
            *spot++ = '\0';

            switch (spot[-2])
            {
            case '%':
                *msg++ = '%';
                maxlen -= 1;
                break;

            case 'c':
            case 'i':
            case 'd':

            case 'u':
            case 'o':
            case 'x':
            case 'X':
                len = snprintf(msg, maxlen, tmp, error->args[argi++].value_i);
                if (len > 0)
                {
                    msg += len;
                    maxlen -= len;
                }

                break;

            case 'f':
                len = snprintf(msg, maxlen, tmp, error->args[argi++].value_f);
                if (len > 0)
                {
                    msg += len;
                    maxlen -= len;
                }

                break;

            case 'p':
                len = snprintf(msg, maxlen, tmp, error->args[argi++].value_ptr);
                if (len > 0)
                {
                    msg += len;
                    maxlen -= len;
                }

                break;

            case 's':
                len = snprintf(msg, maxlen, tmp, error->args[argi++].buf);
                if (len > 0)
                {
                    msg += len;
                    maxlen -= len;
                }

                break;
            }
        }
        else
        {
            *msg++ = *fmt++;
            maxlen -= 1;
        }
    }

    /* slide back if we've overshot the end of our buffer. */
    if (maxlen < 0)
    {
        msg -= (-maxlen) + 1;
    }

    *msg = 0;               /* NULL terminate the string */
    return (errmsg);
}
