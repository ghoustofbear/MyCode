/**
 * @file      excep.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:27:35
 *   
 * @note
 *  excep.h defines 
 */
#ifndef ___EXCEPTION__
#define ___EXCEPTION__

#if defined(_MSC_VER) && (_MSC_VER < 1400)
#define __FUNCTION__ "__FUNCTION__"
#endif

#define SHOW_NOT_IMPLE_FUNC printf("FUNC = %s not impletion!\n",  __FUNCTION__)
#include <string>
using namespace std;
class Exception
{
public:
    explicit Exception()
    {
    }

    Exception(const char* m)
    {
        info = m;
    }

    Exception(const char *m, int e)
    {
        info = m;
        err  = e;
    }

    ~Exception() {}

public:
    const char *what() const
    {
        return info.c_str();
    }

private:
    string info;
    int    err;
};

class StreamException : public Exception
{
public:
    StreamException(const char *m, int e = 0) : Exception(m, e) {}
};

#endif
