#include "observable.h"
#include "observer.h"
#include "tlist.h"

Observable::Observable()
{
    m_obslst = new tlist<Observer *>;
    m_changed = false;
}

Observable::~Observable()
{
    {
        AutoLock locker(m_obslst->_mtx);
        m_obslst->clear();
    }

    delete m_obslst;
    m_obslst = NULL;
}

void
Observable::addObserver(Observer* o)
{
    AutoLock locker(m_obslst->_mtx);

    list<Observer *>::const_iterator iter = find(m_obslst->begin(), m_obslst->end(), o);
    if (iter != m_obslst->end())
    {
        return;
    }

    m_obslst->push_back(o);
}

void
Observable::deleteObserver(Observer* o)
{
    AutoLock locker(m_obslst->_mtx);

    list<Observer *>::iterator iter = find(m_obslst->begin(), m_obslst->end(), o);
    if (iter != m_obslst->end())
    {
        m_obslst->erase(iter);
    }
}

bool
Observable::hasChanged()
{
    return m_changed;
}

void
Observable::setChanged()
{
    m_changed = true;
}

void
Observable::clearChanged()
{
    m_changed = false;
}

void
Observable::notifyObservers(const string &table, const string& json, int type)
{
    if (hasChanged())
    {
        AutoLock locker(m_obslst->_mtx);

        list<Observer *>::iterator i;

        for (i = m_obslst->begin(); i != m_obslst->end(); i++)
        {
            Observer *o = *i;

            if (type & o->getType())
            {
                o->update(table, json, type);
            }
        }
    }

    clearChanged();
}
