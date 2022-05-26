/**
 * @file      observer.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-06-16 17:52:02
 *   
 * @note
 *  observer.h defines 
 */
#ifndef __OBSERVER_H__
#define __OBSERVER_H__
#include "exports.h"

class MS_API Observer
{
public:
    friend class Observable;

    enum
    {
        O_ADD    = 1,
        O_DELETE = 2,
        O_MODIFY = 4,
        O_FLUSH  = 8,
        O_CLEAR  = 16,
    };

    int                getType() { return m_otype; }

protected:
    virtual void       update(const std::string &table, const std::string &json, int type) = 0;

protected:
    int                m_otype;
};

#endif
