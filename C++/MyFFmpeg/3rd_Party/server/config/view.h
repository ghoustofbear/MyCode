/**
 * @file      view.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-11-13 10:14:25
 *   
 * @note
 *  view.h defines 
 */
#ifndef __VIEW_H__
#define __VIEW_H__
#include "observable.h"
#include "cfg.h"

class View: public Cfg, public Observable
{
public:
    View(int type) {};
    virtual ~View() {};

    virtual int        add(const string& json)    { return 0; };
    virtual int        del(const string& json)    { return 0; };
    virtual int        modify(const string& json) { return 0; };

    virtual int        flush(const string& json)  { return 0; };
    virtual int        clear(const string& json)  { return 0; };
};

#endif
