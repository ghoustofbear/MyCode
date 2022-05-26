#include "task.h"
#include "bufferev.h"

Task::Task()
{
    m_status = S_FREE;
    m_bev    = NULL;
}

Task::~Task()
{
}

int
Task::status()
{
    return m_status;
}

int
Task::start()
{
    m_thread.setEntry(threadProc, this, 0);
    return m_thread.start();
}

void
Task::stop()
{
    m_thread.stop();
}

void
Task::setbev(Bufferev *bev)
{
    m_bev = bev;
}

void *
Task::threadProc(ThreadInfo *info)
{
    Task * task = (Task *)(info->data);

    task->process();

    task->m_status = S_CLOSED;

    return NULL;
}
