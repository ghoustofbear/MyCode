#include "thread.h"

Thread::Thread()
{
    _active = false;
}

Thread::~Thread()
{
    /* stop(); */
}

void
Thread::setEntry(THREAD_FUNC func, void * data, int id)
{
    _info.id   = id;
    _info.func = func;
    _info.data = data;
}

int
Thread::start()
{
    AutoLock locker(_mtx);
    _active = true;
    if (sys->startThread(_info) < 0)
    {
        _active = false;
        return -1;
    }
    return 0;
}

void
Thread::stop()
{
    AutoLock locker(_mtx);

    _active = false;
    sys->endThread(_info);
}

void
Thread::detach()
{
    sys->detachThread(_info);
}

bool
Thread::active()
{
    return _active;
}
