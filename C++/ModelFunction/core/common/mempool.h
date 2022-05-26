/**
 * @file      mempool.h
 * @copyright Copyright (c) 2017, Unitech Co., Ltd. All Rights Reserved.
 * @brief     brief
 * @author    caiwang213@qq.com
 * @date      2017-07-03 13:46:14
 *
 * @note
 *  mempool.h defines
 */
#ifndef __MEMPOOL_H__
#define __MEMPOOL_H__
#include "tmultimap.h"
#include "thread.h"

#define MEMPOOL
#define MAX_MEMPOOL_SIZE     (100*1024*1024)
#define MAX_UNUSED_SEC       (30)

typedef struct
{
    int                flag;
    uint32_t           size;
    uint64_t           pts;
    char               buf[0];
} mem_slice_s;

class CM_API Mempool : public tmultimap<uint32_t, mem_slice_s *>
{
public:
    Mempool(int capacity);
    virtual ~Mempool();

    int                start();
    void               stop();

    void             * alloc(uint32_t size);
    void               free(void *p);
    uint32_t           length(void *p);

    uint32_t           capacity();
    uint32_t           totalSize();

    uint32_t           totalNumber();
    uint32_t           freeNumber();

protected:
    void               collector();
    static void *      threadEntry(ThreadInfo *arg);

private:
    uint32_t           m_capacity;
    uint32_t           m_totalSize;

    uint32_t           m_usedNum;

    Thread             m_collecThread;
};

CM_API extern Mempool * mempool;
#endif
