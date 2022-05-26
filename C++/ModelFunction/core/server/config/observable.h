/**
 * @file      observable.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-06-20 11:24:57
 *   
 * @note
 *  observable.h defines 
 */
#ifndef __OBSERVABLE_H__
#define __OBSERVABLE_H__
#include "exports.h"

template <class T>
class tlist;

class Observer;

class MS_API Observable
{
public:

    Observable();
    virtual ~Observable();

    void               addObserver(Observer* o);
    void               deleteObserver(Observer* o);

protected:
    bool               hasChanged();
    void               setChanged();
    void               clearChanged();

    void               notifyObservers(const string& table, const string& json, int type);

private:
    tlist<Observer*> * m_obslst;

    bool               m_changed;
};

#endif
