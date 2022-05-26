/**
 * @file      utility.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:30:30
 *   
 * @note
 *  utility.h defines 
 */
#ifndef __UTILITY_H__
#define __UTILITY_H__

#include "def.h"

CM_API double util_round(double d);
CM_API double util_distance(double x1, double y1, double x2, double y2);

extern "C++"
{
    /**
     * High reliability and high security functions, and compatible with Linux and windows
     * for windows impliemnt we use the vc12.0 functions which with prefix "_s'.
     */
    template <size_t _size>
    inline
    char* util_strcpy(char(&dest)[_size], const char* src)
    {
        ASSERT(_size > strlen(src));

        if (_size <= strlen(src))
        {
            throw std::overflow_error(RPTLINE);
        }

        strcpy(dest, src);
        dest[_size - 1] = '\0';

        return dest;
    }

    template <size_t _size>
    inline
    char* util_strncpy(char(&dest)[_size], const char* src, size_t n)
    {
        ASSERT(!(_size < n && n <= strlen(src)));
        ASSERT(!(_size < n && n >= strlen(src)));
        ASSERT(!(_size == strlen(src) && n >= strlen(src)));
        ASSERT(!(_size == n && n < strlen(src)));

        strncpy(dest, src, n);

        if (n < _size)
        {
            dest[n] = '\0';
        }
        else
        {
            dest[_size - 1] = '\0';
        }

        return dest;
    }

    template <size_t _size>
    inline
    int util_sprintf(char(&dest)[_size], const char *format, ...)
    {
        va_list arg;
        int done;

        va_start(arg, format);

        done = vsprintf(dest, format, arg);
        va_end(arg);

#ifdef __linux
        ASSERT(_size > (size_t)done);
#endif
        return done;
    }

    template <size_t _size>
    inline
    int util_snprintf(char(&dest)[_size], size_t n, const char *format, ...)
    {
        va_list arg;
        int done;

        va_start(arg, format);

        /* note: The return value in vc12.0 is not the same as the Linux */
        done = vsnprintf(dest, n, format, arg);
        va_end(arg);

#ifdef __linux
        ASSERT(!(_size < n && n <= (size_t)done));
        ASSERT(!(_size < n && n >= (size_t)done));
        ASSERT(!(_size == done && n >= (size_t)done));
        ASSERT(!(_size == n && n < (size_t)done));
#endif
        return done;
    }
}

#endif
