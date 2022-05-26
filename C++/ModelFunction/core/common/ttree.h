/**
 * @file      ttree.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-12-30 04:33:47
 *
 * @note
 *  _K     ancestor(_K key); find out the first ancestor of the key bellow the root
 *  void * get_childata(_K key, int index, _K &child); index >= 0
 */
#ifndef __TTREE_H__
#define __TTREE_H__
#include <mutex>
#include <map>
#include <iostream>
#include "tls_error.h"
using namespace std;

template<class _K>
struct tnode
{
    _K             parent;
    _K             child;
    _K             sibling;
    _K             path;
    void         * data;

    tnode()
    {
        data = NULL;
    }

    ~tnode() {}
};

template< class _K>
class ttree : public map<_K, tnode<_K>>
{
    typedef void (*travf)(_K key, void *nodedata, void *arg);

public:
    ttree(_K key, void* data);

public:
    int                insert_child(_K parent, _K key, void *data);
    int                delete_sub(_K key);

    int                child_num(_K key);

    int                set_data(_K key, void* data);
    void             * get_data(_K key);
    void             * get_childata(_K key, int index, _K &child);

    _K                 ancestor(_K key);

    void               traversal(_K root, travf f = NULL, void *arg = NULL, bool trav_root = true);
    void               traversal_child(_K key, travf f = NULL, void *arg = NULL);

protected:
    bool               is_leaf(_K key);

public:
    /* It is not recommended to use a recursive lock */
    recursive_mutex   _mutex;
    /* mutex             _mutex; */
    _K                _root;
};

template<class _K>
ttree<_K>::ttree(_K key, void* data)
{
    lock_guard<recursive_mutex> locker(_mutex);
    /* lock_guard<mutex> locker(_mutex); */

    tnode<_K> &node = (*this)[key];
    node.path  = key;
    node.data  = data;

    _root = key;
}

template<class _K>
int
ttree<_K>::insert_child(_K parent, _K key, void *data)
{
    lock_guard<recursive_mutex> locker(_mutex);
    /* lock_guard<mutex> locker(_mutex); */
    if (this->find(parent) == this->end())
    {
        cout << "not find parent: " << parent << endl;
        ERRE_COM("not find parent: %s", parent.c_str());
        return -1;
    }

    if (this->find(key) != this->end())
    {
        cout << "the key: " << key << " is exist." << endl;
        ERRE_COM("the key: %s is exist.", key.c_str());
        return -1;
    }

    tnode<_K> &node = (*this)[key];
    node.parent = parent;
    node.path   = (*this)[parent].path + "-" +  key;
    node.data   = data;

    _K first = (*this)[parent].child;
    if (first.empty())
    {
        (*this)[parent].child = key;

        return 0;
    }

    _K prevous = first;
    _K sibling = (*this)[first].sibling;

    while (!sibling.empty())
    {
        prevous = sibling;
        sibling = (*this)[sibling].sibling;
    }

    (*this)[prevous].sibling = key;

    return 0;
}

template<class _K>
int
ttree<_K>::delete_sub(_K key)
{
    lock_guard<recursive_mutex> locker(_mutex);
    /* lock_guard<mutex> locker(_mutex); */
    if (this->find(key) == this->end())
    {
        cout << "the key: " << key << " is not exist" << endl;
        ERRW_COM("the key: %s is not exist.", key.c_str());
        return -1;
    }

    if (is_leaf(key) && (key == _root))
    {
        cout << "the key: " << key << " is the root, do not delete." << endl;
        ERRW_COM("the key: %s is the root, do not delete.", key.c_str());
        return 0;
    }
    else if (is_leaf(key) && (key != _root))
    {
        /* is the parent's frist child? */
        _K parent = (*this)[key].parent;
        if (key == (*this)[parent].child)
        {
            (*this)[parent].child = (*this)[key].sibling;
        }
        else
        {
            /* find prevous sibling */
            _K prevous = (*this)[parent].child;
            while (!(key == (*this)[prevous].sibling))
            {
                prevous = (*this)[prevous].sibling;
            }

            (*this)[prevous].sibling = (*this)[key].sibling;
        }

        this->erase(key);
    }
    else
    {
        /* post order delete child and siblings */
        _K first = (*this)[key].child;
        while (!first.empty())
        {
            /* find last sibling */
            _K last  = (*this)[key].child;
            while (!(*this)[last].sibling.empty())
            {
                last = (*this)[last].sibling;
            }

            delete_sub(last);
            first = (*this)[key].child;
        }

        /* delete itself */
        delete_sub(key);
    }

    return 0;
}

template<class _K>
int
ttree<_K>::child_num(_K key)
{
    lock_guard<recursive_mutex> locker(_mutex);
    /* lock_guard<mutex> locker(_mutex); */
    if (this->find(key) == this->end())
    {
        cout << "the key: " << key << " is not exist" << endl;
        ERRE_COM("the key: %s is not exist.", key.c_str());
        return -1;
    }

    if (is_leaf(key))
    {
        return 0;
    }

    int num = 0;
    _K sibling = (*this)[key].child;

    while (!sibling.empty())
    {
        num++;
        sibling = (*this)[sibling].sibling;
    }

    return num;
}

template<class _K>
bool
ttree<_K>::is_leaf(_K key)
{
    bool result = (*this)[key].child.empty();
//    if (result)
//    {
//        ERRW_COM("the key: %s is a leaf.", key.c_str());
//    }

    return result;
}

template<class _K>
int
ttree<_K>::set_data(_K key, void* data)
{
    lock_guard<recursive_mutex> locker(_mutex);
    /* lock_guard<mutex> locker(_mutex); */

    if (this->find(key) == this->end())
    {
        cout << "the key: " << key << " is not exist" << endl;
        ERRW_COM("the key: %s is not exist.", key.c_str());
        return -1;
    }

    (*this)[key].data = data;

    return 0;
}

template<class _K>
void *
ttree<_K>::get_data(_K key)
{
    lock_guard<recursive_mutex> locker(_mutex);
    /* lock_guard<mutex> locker(_mutex); */

    if (this->find(key) == this->end())
    {
        cout << "the key: " << key << " is not exist" << endl;
        ERRW_COM("the key: %s is not exist.", key.c_str());
        return NULL;
    }

    return (*this)[key].data;
}

template<class _K>
void *
ttree<_K>::get_childata(_K key, int index, _K &child)
{
    int num = child_num(key);
    if (index >= num)
    {
        ERRW_COM("index:%d > child_num(%s): %d", index, key.c_str(), num);
        return NULL;
    }

    lock_guard<recursive_mutex> locker(_mutex);
    /* lock_guard<mutex> locker(_mutex); */
    /* index: >= 0 */
    _K sibling = (*this)[key].child;
    int i = 0;
    while (!sibling.empty() && i++ < index)
    {
        sibling = (*this)[sibling].sibling;
    }

    child = sibling;
    return (*this)[sibling].data;
}

template<class _K>
_K
ttree<_K>::ancestor(_K key)
{
    lock_guard<recursive_mutex> locker(_mutex);
    /* lock_guard<mutex> locker(_mutex); */
    if (this->find(key) == this->end())
    {
        cout << "the key: " << key  << " is not exist" << endl;
        ERRW_COM("the key: %s is not exist.", key.c_str());
        return "";
    }

    _K ances = key;
    while ((*this)[ances].parent != _root)
    {
        ances = (*this)[ances].parent;
    }

    return ances;
}

template<class _K>
void
ttree<_K>::traversal(_K root, travf f, void *arg, bool trav_root)
{
    lock_guard<recursive_mutex> locker(_mutex);

    if (this->find(root) == this->end())
    {
        cout << "the root: " << root  << " is not exist" << endl;
        ERRW_COM("the key: %s is not exist.", root.c_str());
        return;
    }

    if (trav_root)
    {
        //cout << root << endl;
        if (f)
        {
            f(root, (*this)[root].data, arg);
        }
    }

    /* print child and siblings */
    _K sibling = (*this)[root].child;
    while (!sibling.empty())
    {
        //cout << (*this)[sibling].path << endl;
        if (f)
        {
            f(sibling, (*this)[sibling].data, arg);
        }
        sibling = (*this)[sibling].sibling;
    }

    /* recursive print child and siblings */
    sibling = (*this)[root].child;
    while (!sibling.empty())
    {
        traversal(sibling, f, arg, false);
        sibling = (*this)[sibling].sibling;
    }
}

template<class _K>
void
ttree<_K>::traversal_child(_K key, travf f, void *arg)
{
    lock_guard<recursive_mutex> locker(_mutex);

    if (this->find(key) == this->end())
    {
        cout << "the key: " << key  << " is not exist" << endl;
        ERRW_COM("the key: %s is not exist.", key.c_str());
        return;
    }

    if (is_leaf(key))
    {
        return;
    }

    _K sibling = (*this)[key].child;
    while (!sibling.empty())
    {
        f(sibling, (*this)[sibling].data, arg);
        sibling = (*this)[sibling].sibling;
    }
}
#endif
