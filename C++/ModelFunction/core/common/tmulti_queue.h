/**
 * @file      tmulti_queue.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-05-31 11:29:06
 *
 * @note
 *  tmulti_queue.h defines
 */

#ifndef __TMULTI_QUEUE_H__
#define __TMULTI_QUEUE_H__

#include "def.h"
#include "tlist.h"
#include "concurrency.h"

#define    MAX_ITEM_COUNT     100

struct READER_S
{
    uint32_t    id;
    uint32_t    runFlag;
    uint32_t    readPos;
    uint32_t    readCycle;

    READER_S()
    {
        id        = -1;
        runFlag   = 0;
        readPos   = -1;
        readCycle = -1;
    };

    bool operator==(const READER_S& reader) const
    {
        return (id == reader.id);
    }
};
template <class _T>
class tmulti_queue
{
public:
    enum
    {
        SET_CURPOS,
        SET_INCPOS,
    };

    enum
    {
        E_OK,
        E_PARAM,
        E_READER,
        E_TIMEOUT,
    };

    tmulti_queue()
    {
        m_writePos = 0;
        m_writeCycle = 0;
        m_seqReader = 0;
    };
    ~tmulti_queue() {};

    long            addReader();
    long   	        delReader(long &id);
    long            writeItem(_T item);
    long            readItem(long readerId, _T &item);

protected:
    long            getReader(long id, uint32_t &readPos, uint32_t &readCycle);
    long            setReader(long id, uint32_t type);

private:
    tlist<READER_S> m_listReader;
    _T              m_arrayItem[MAX_ITEM_COUNT];
    Mutex           m_mtxItem;

    uint32_t        m_writePos;
    uint32_t        m_writeCycle;

    uint32_t        m_seqReader;
};

template <class _T>
long
tmulti_queue<_T>::addReader()
{
    READER_S reader;

    {
        AutoLock locker(m_mtxItem);
        reader.readPos   = m_writePos;
        reader.readCycle = m_writeCycle;
        reader.id        = m_seqReader++;
    }

    {
        AutoLock locker(m_listReader._mtx);
        m_listReader.push_back(reader);

    }

    return reader.id;
}

template <class _T>
long
tmulti_queue<_T>::delReader(long &id)
{
    if (id < 0)
    {
        /* invalidate param */
        return E_PARAM;
    }

    {
        AutoLock locker(m_listReader._mtx);

        READER_S reader;
        reader.id = id;

        list<READER_S>::iterator iter = find(m_listReader.begin(), m_listReader.end(), reader);
        if (iter == m_listReader.end())
        {
            /* not find reader */
            return E_READER;
        }

        m_listReader.erase(iter);
    }

    id = -1;

    return 0;
}

template <class _T>
long
tmulti_queue<_T>::writeItem(_T item)
{
    AutoLock locker(m_mtxItem);

    if (m_writePos >= MAX_ITEM_COUNT)
    {
        m_writePos   = 0;
        m_writeCycle = 0;
    }

    m_arrayItem[m_writePos] = item;

    if (++m_writePos >= MAX_ITEM_COUNT)
    {
        m_writePos = 0;
        m_writeCycle++;
    }

    return 0;
}

template <class _T>
long
tmulti_queue<_T>::getReader(long id, uint32_t &readPos, uint32_t &readCycle)
{
    if (id < 0)
    {
        /* invalidate param */
        return E_PARAM;
    }

    AutoLock locker(m_listReader._mtx);

    READER_S reader;
    reader.id = id;

    list<READER_S>::const_iterator iter = find(m_listReader.begin(), m_listReader.end(), reader);;
    if (iter == m_listReader.end())
    {
        /* not find reader */
        return E_READER;
    }

    readPos   = iter->readPos;;
    readCycle = iter->readCycle;;

    return 0;
}

template <class _T>
long
tmulti_queue<_T>::setReader(long id, uint32_t type)
{
    if (id < 0 || type < SET_CURPOS)
    {
        /* invalidate param */
        return E_PARAM;
    }

    AutoLock locker(m_listReader._mtx);

    READER_S reader;
    reader.id = id;

    list<READER_S>::iterator iter = find(m_listReader.begin(), m_listReader.end(), reader);
    if (iter == m_listReader.end())
    {
        /* not find reader */
        return -2;
    }

    READER_S *rdr = &(*iter);
    if (SET_CURPOS == type)
    {
        AutoLock locker(m_mtxItem);
        rdr->readPos  = m_writePos;
        rdr->readCycle = m_writeCycle;
    }
    else if (SET_INCPOS == type)
    {
        if ((rdr->readPos + 1) >= MAX_ITEM_COUNT)
        {
            rdr->readPos = 0;
            rdr->readCycle++;
        }
        else
        {
            rdr->readPos++;
        }
    }
    else
    {
        return E_PARAM;
    }

    return 0;
}

template <class _T>
long
tmulti_queue<_T>::readItem(long readerId, _T &item)
{
    long     ret       = -1;
    uint32_t readPos   = 0;
    uint32_t readCycle = 0;
    uint32_t sleepMs   = 0;

    while (1)
    {
        ret = getReader(readerId, readPos, readCycle);

        if (ret < 0)
        {
            return ret;
        }

        if (readPos == m_writePos &&
                readCycle == m_writeCycle)
        {
            sys->sleep(1);
            sleepMs += 1;

            if (sleepMs >= 100)
            {
                /* no data return timeout */
                return E_TIMEOUT;
            }

            continue;
        }
        else if ((readPos < MAX_ITEM_COUNT) &&
                 ((readCycle == m_writeCycle && readPos < m_writePos) ||
                  (readCycle + 1 == m_writeCycle && readPos > m_writePos)))
        {
            {
                AutoLock locker(m_mtxItem);
                item = m_arrayItem[readPos];
            }

            setReader(readerId, SET_INCPOS);

            break;
        }
        else
        {
            if (setReader(readerId, SET_CURPOS) < 0)
            {
                break;
            }

            continue;
        }
    }

    return ret;
}

#endif
