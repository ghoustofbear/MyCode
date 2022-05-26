/**
 * @file      tevqueue.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-11-30 08:06:58
 *
 * @note
 *  tevqueue.h defines
 */
#ifndef __TEVQUEUE_H__
#define __TEVQUEUE_H__
#include "def.h"
#include "concurrency.h"

template <class T1, class T2>
class tevent
{
public:
    tevent() : m_resped(false) {}

public:
    void               init(const T1 req, T2 & resp);
    bool               wait(int milliseconds);
    bool               notify(T1 req, T2 & resp);
    void               cancel();

public:
    bool volatile      m_resped;
    Event              m_ev;
    T1                 m_req;
    T2               * m_resp;
};

template <class T1, class T2>
void tevent<T1, T2>::init(const T1 req, T2 & resp)
{
    m_req  = req;
    m_resp = &resp;
}

template <class T1, class T2>
bool tevent<T1, T2>::wait(int milliseconds)
{
    m_ev.wait(milliseconds);

    return m_resped;
}

template <class T1, class T2>
bool tevent<T1, T2>::notify(T1 req, T2 &resp)
{
    if (m_req == req)
    {
        (*m_resp) = resp;
        m_resped  = true;
        m_ev.notify();

        return true;
    }

    return false;
}

template <class T1, class T2>
void tevent<T1, T2>::cancel()
{
}

template <class T1, class T2>
class tevqueue
{
    typedef typename  std::vector<tevent<T1, T2> *> _tvevqueue;
public:
    void               add(tevent<T1, T2> &event);
    void               response(T1 req, T2 &resp);

    void               clear(tevent<T1, T2> &event);
    void               clear();

protected:
    Mutex              m_mutex;
    _tvevqueue         m_evqu;
};

template <class T1, class T2>
void tevqueue<T1, T2>::add(tevent<T1, T2> &event)
{
    AutoLock lock(m_mutex);
    m_evqu.push_back(&event);
}

template <class T1, class T2>
void tevqueue<T1, T2>::response(T1 req, T2 &resp)
{
    AutoLock lock(m_mutex);

    for (size_t i = 0; i < m_evqu.size(); ++i)
    {
        if (m_evqu[i]->notify(req, resp))
        {
            break;
        }
    }
}

template <class T1, class T2>
void tevqueue<T1, T2>::clear(tevent<T1, T2> &event)
{
    AutoLock lock(m_mutex);

    /* #ifdef _WIN32 */
    /*     vector<tevent<T1, T2> *>::iterator i = find(m_evqu.begin(), m_evqu.end(), &event); */
    /* #else */
    /*     typename vector<tevent<T1, T2> *>::iterator i = find(m_evqu.begin(), m_evqu.end(), &event); */
    /* #endif */

    auto i = find(m_evqu.begin(), m_evqu.end(), &event);

    if (i != m_evqu.end())
    {
        m_evqu.erase(i);
        event.cancel();
    }
}

template <class T1, class T2>
void tevqueue<T1, T2>::clear()
{
    AutoLock lock(m_mutex);

    for (size_t i = 0; i < m_evqu.size(); ++i)
    {
        m_evqu[i]->cancel();
    }

    m_evqu.clear();
}
#endif
