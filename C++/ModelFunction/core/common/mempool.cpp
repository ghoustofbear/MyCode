#include "mempool.h"

Mempool::Mempool(int capacity)
{
    m_totalSize = 0;
    m_usedNum   = 0;
    m_capacity  = capacity;
}

Mempool::~Mempool()
{
    AutoLock locker(_mtx);
    for (multimap<uint32_t, mem_slice_s *>::const_iterator i = begin(); i != end(); ++i)
    {
        ::free((void *)(i->second));
    }
    clear();
}

void *
Mempool::alloc(uint32_t size)
{
    AutoLock locker(_mtx);

    multimap<uint32_t, mem_slice_s *>::const_iterator i = find(size);
    size_t   cnt = count(size);
    uint32_t k   = 0;

    if (0 != cnt)
    {
        for (k = 0; k < cnt; ++k, ++i)
        {
            if (0 == i->second->flag)
            {
                i->second->flag = 1;
                i->second->pts  = time(NULL);
                m_usedNum++;
                return i->second->buf;
            }
        }
    }

    /* There is no free slice in the same size, and matches a free one backward to N slices */
    if (k == cnt)
    {
        multimap<uint32_t, mem_slice_s *>::const_iterator i;
        int j = 0;
        /* for (i = lower_bound(size), j = 0; i != end() && j < 50; ++i, ++j) */
        for (i = upper_bound(size), j = 0; i != end() && j < 200; ++i, ++j)
        {
            if (0 == i->second->flag)
            {
                i->second->flag = 1;
                i->second->pts  = time(NULL);
                m_usedNum++;
                return i->second->buf;
            }
        }
    }

    if (size + m_totalSize > m_capacity)
    {
        LOGE_COM("the memory pool is full!");
        return NULL;
    }

    /* else we new a slice */
    mem_slice_s *slice = (mem_slice_s *)::malloc(sizeof(mem_slice_s) + size);
    if (!slice)
    {
        return NULL;
    }

    slice->flag = 1;
    slice->size = size;
    slice->pts  = time(NULL);

    insert(make_pair(size, slice));

    m_totalSize += size;
    m_usedNum++;

    return slice->buf;
}

void
Mempool::free(void *p)
{
    AutoLock locker(_mtx);
    mem_slice_s * slice = (mem_slice_s *)((uint8_t *)p - sizeof(mem_slice_s));
    size_t        cnt   = count(slice->size);

    if (0 == cnt)
    {
        LOGE_COM("slice does not exist!");
        return;
    }

    multimap<uint32_t, mem_slice_s *>::iterator i = find(slice->size);
    for (uint32_t k = 0; k < cnt; ++k, ++i)
    {
        if (p == i->second->buf)
        {
            i->second->flag = 0;
            m_usedNum--;
            return;
        }
    }

    LOGE_COM("free null!, cnt: %d", cnt);
}

uint32_t
Mempool::length(void *p)
{
    AutoLock locker(_mtx);
    mem_slice_s * slice = (mem_slice_s *)((uint8_t *)p - sizeof(mem_slice_s));

    return slice->size;
}

uint32_t
Mempool::capacity()
{
    return m_capacity;
}

uint32_t
Mempool::totalSize()
{
    AutoLock locker(_mtx);
    return m_totalSize;
}

uint32_t
Mempool::totalNumber()
{
    AutoLock locker(_mtx);
    return size();
}

uint32_t
Mempool::freeNumber()
{
    AutoLock locker(_mtx);
    return size() - m_usedNum;;
}

int
Mempool::start()
{
    m_collecThread.setEntry(threadEntry, this, 0);
    return m_collecThread.start();
}

void
Mempool::stop()
{
    m_collecThread.stop();
}

void
Mempool::collector()
{
    uint64_t cnt = 0;
    uint32_t len = 50;

    while (m_collecThread.active())
    {
        /* check every 5 second */
        if (0 == (cnt % 1000))
        {
            AutoLock locker(_mtx);
            for (auto i = begin(); i != end();)
            {
                if (((time(NULL) - i->second->pts) > MAX_UNUSED_SEC) && (0 == i->second->flag))
                {
                    m_totalSize -= i->second->size;
                    ::free((void *)(i->second));

                    auto ti = i++;
                    erase(ti);
                }
                else
                {
                    i++;
                }
            }

            //if (0 == (cnt % 30000))
            //{
            //    LOGI_COM("totalNum: %d, freeNum: %d, totalSize: %d kb", size(), size() - m_usedNum, m_totalSize / 1024);
            //}
        }

        sys->sleep(len);
        cnt += len;
    }
}

void *
Mempool::threadEntry(ThreadInfo *arg)
{
    Mempool * pool = (Mempool *)arg->data;
    pool->collector();

    return NULL;
}
