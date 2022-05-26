#include "watchdog.h"
#include "process.h"


#if defined(_WIN32)
#ifndef _DEBUG
#pragma comment( linker, "/subsystem:windows /entry:mainCRTStartup" )
#endif
#endif

int main()
{
#ifdef _DEBUG
    return 0;
#endif

#ifdef UNIX
    initDemo();
#endif
	WatchDog dog;
	string WatchdogId = dog.getwatchdogid();
	if (isRuning(WatchdogId) != 0)
    {
        return 0;
    }

    //log4x::ilog4x::instance()->start();
   
    dog.start();

    while (true)
    {
        dog.timeDriver();
        sys->sleep(300);
    }

    dog.stop();

    return 0;
}

