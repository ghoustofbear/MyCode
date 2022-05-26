#include "watchdog.h"
#include "thread.h"
#include "socket.h"
#include "json/json.h"
#include "process.h"
#include "CELLProcessmgr.h"

#ifdef _WIN32
#include <direct.h>
#include "wsys.h"
#include <time.h>
#include <sys/timeb.h>
#else
#include "usys.h"
#include <unistd.h>
#include <sys/wait.h>
#include <sys/un.h>
#endif



WatchDog::WatchDog()
{
#ifdef _WIN32
    sys = new WSys();
#else
    sys = new USys();
#endif

    parseJson();
}

WatchDog::~WatchDog()
{
    AutoLock lock(m_programMapInfo._mtx);
    map<string, CIS*>::iterator i;
    for (i = m_programMapInfo.begin(); i != m_programMapInfo.end(); ++i)
    {
        delete i->second;
    }
    m_programMapInfo.clear();
}

int
WatchDog::start()
{

#ifdef _WIN32
	m_sock = sys->createSocket();
#else
	m_sockFd = socket(AF_LOCAL, SOCK_DGRAM, 0);
	unlink(C2WATCHDOG_SOCK_FILE);
#endif

	m_thread = new Thread();
	m_timeOut = 3;

#ifdef _WIN32
    Host rh("127.0.0.1", m_port);
    if (m_sock->bind(rh, IPPROTO_UDP) != 0)
    {
        exit(-1);
    }
#else
    struct sockaddr_un addr = {};
    addr.sun_family = AF_LOCAL;
    strcpy(addr.sun_path, C2WATCHDOG_SOCK_FILE);
    if(bind(m_sockFd, (struct sockaddr*)&addr, sizeof(addr)))
    {
        exit(-1);
    }
#endif

    m_thread->setEntry(threadProc, this, 0);
    m_thread->start();

    return 0;
}

int 
WatchDog::stop()
{
    m_thread->stop();
#ifdef _WIN32
    m_sock->close();
#else
    close(m_sockFd);
    m_sockFd = -1;
#endif
	if (m_thread)
	{
		delete m_thread;
		m_thread = NULL;
	}

#ifdef _WIN32
	if (m_sock)
	{
		delete m_sock;
		m_sock = NULL;
	}
#else
	if (m_sockFd != 1)
	{
		close(m_sockFd);
		m_sockFd = -1;
	}
	unlink(C2WATCHDOG_SOCK_FILE);
#endif

    return 0;
}

void
WatchDog::parseJson()
{
    FileStream jsonFile;
    jsonFile.openReadOnly("watchdog.json");
    if (!jsonFile.isopen())
    {
        exit(-1);
    }

    int length = jsonFile.length();
    char *json = new char[length+1];
    memset(json, 0, length + 1);

    jsonFile.read(json, length);

    SHL_Json::Reader reader;
    SHL_Json::Value  rvalue;

    if (!(reader.parse(json, rvalue, false) &&
        !rvalue.isNull() &&
        rvalue.isObject()))
    {
        delete[] json;
        return;
    }

    m_timeOut = rvalue["timeout"].asInt();
    string na = rvalue["name"].asString();
	m_port = rvalue["dogPort"].asInt();
	m_watchdogid = rvalue["dogID"].asString();
    int cs = rvalue["clients"].size();
	if (rvalue["reboottime"].isNull())
	{
		istimerboot = false;
	}
	else
	{
		istimerboot = true;
		string timeReboot = rvalue["reboottime"].asString();
#ifdef _WIN32
		sscanf_s(timeReboot.c_str(), "%d:%d:%d",
			&m_timeReboot.hour, &m_timeReboot.minute, &m_timeReboot.second);
#endif
	}
	
    string keyWords;
    string exeName;
    string argv;
    string workPath;
    for (int i = 0; i < cs; ++i)
    {
        keyWords = rvalue["clients"][i]["keyWords"].asString();
        exeName  = rvalue["clients"][i]["exeName"].asString();
        workPath = rvalue["clients"][i]["workPath"].asString();
        argv     = rvalue["clients"][i]["argv"].asString();

        CIS *cis = new CIS;
        cis->exeName  = exeName;
        cis->workPath = workPath;
        cis->argv     = argv;
        cis->key      = keyWords;
        cis->pid      = -1;

        m_programMapInfo[keyWords] = cis;     
        m_programMapLast[keyWords] = sys->currentTimeMillis();
    }

    delete[] json;
}

int
WatchDog::rebootApp(string key)
{
    string exeName;
    string argv;
    string workPath;

    {
        AutoLock lock(m_programMapInfo._mtx);
        exeName = m_programMapInfo[key]->exeName;
        argv = m_programMapInfo[key]->argv;
        workPath = m_programMapInfo[key]->workPath;
    }
    CELLProcessMgr promgr;

#ifdef _WIN32
#ifdef _DEBUG
	promgr.KillProcessByName((exeName + "_d.exe").c_str());
#else
	promgr.KillProcessByName((exeName + ".exe").c_str());
#endif
#else
    #ifdef _DEBUG
    promgr.KillProcessByName((exeName + "_d").c_str());
    #else
    promgr.KillProcessByName(exeName.c_str());
    #endif
#endif
    char curPath[1024] = { 0 };
#ifdef _WIN32
    //_getcwd(curPath, 1024);
#else
    //getcwd(curPath, 1024);
#endif

#ifdef _DEBUG
    createProcess((exeName + "_d").c_str(), argv.c_str(), workPath.c_str());
#else
    createProcess(exeName.c_str(), argv.c_str(), workPath.c_str());
#endif

    return 0;
}

void WatchDog::timeDriver()
{
    printf("timeDriver\n");
    uint64_t ret = 0;
    uint64_t cur = sys->currentTimeMillis();
	bool reboot = false;;
#ifdef _WIN32
	tm* time_t  = getcurrtime();
	if ((m_timeReboot.hour == time_t->tm_hour) && (m_timeReboot.minute == time_t->tm_min) && (m_timeReboot.second == time_t->tm_sec))
	{
		reboot = true;
	}
#endif
    map<string, uint64_t>::iterator i;
    AutoLock lock(m_programMapLast._mtx);
    for (i = m_programMapLast.begin(); i != m_programMapLast.end(); ++i)
    {

        ret = cur - i->second;

		if ((ret > m_timeOut * 1000) || (istimerboot && reboot))
        {
            i->second = cur;
            rebootApp(i->first);
        }
    }
}
#ifdef _WIN32
	struct tm*                 
	WatchDog::getcurrtime()
	{
		timeb tb;
		ftime(&tb);

		struct tm * tmPtr = NULL;
		tmPtr = localtime(&tb.time);
		return tmPtr;
	}
#endif





void
WatchDog::loop()
{
    char buf[128] = { 0 };
    int recvLen = 0;

    
    sockaddr sa;
    int saLen = sizeof(sa);

    uint64_t currTi;

    while (m_thread->active())
    {
#ifdef _WIN32
        recvLen = m_sock->readFrom(buf, 128, &sa, &saLen);
#else
        recvLen = read(m_sockFd, buf, sizeof(buf));
#endif
        if (recvLen < 0)
        {
            continue;
        }
        buf[recvLen] = '\0';
        //printf("recv:%s\n", buf);

        currTi = sys->currentTimeMillis();

        {
            AutoLock lock(m_programMapLast._mtx);
            map<string, uint64_t>::iterator i = m_programMapLast.find(buf);
            if (i != m_programMapLast.end())
            {
                m_programMapLast[buf] = currTi;
            }
        }

#ifdef UNIX
        int pid = waitpid(-1,NULL,WNOHANG);
        if (pid != 0)
        {
            //printf("children process finished, pid:%d\n", pid);
        }
#endif
        sys->sleep(10);
    }
}


void*
WatchDog::threadProc(ThreadInfo *info)
{
    WatchDog * dog = (WatchDog*)(info->data);

    switch (info->id)
    {
    case 0:
        dog->loop();
        break;

    default:
        break;
    }

    return NULL;
}
