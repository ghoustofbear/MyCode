#include "wsys.h"
#include "wsocket.h"
#include "tls_error.h"
#include <time.h>
#include <sys/timeb.h>
#include <process.h>

#include <iphlpapi.h>
#include <psapi.h>


static DWORD key = -1;

static unsigned __stdcall start_address(void *arg)
{
    ThreadInfo *info = (ThreadInfo *)arg;
    info->func(info);

    if (-1 != key)
    {
        void * ptr = TlsGetValue(key);

        if (ptr)
        {
            LocalFree(ptr);
        }
    }

    return 0;
}

WSys::WSys()
{
    WSocket::init();

    SYSTEM_INFO SystemInfo;
    GetSystemInfo(&SystemInfo);
    m_processCount     = SystemInfo.dwNumberOfProcessors;
    m_curProcessHandle = GetCurrentProcess();
}

WSys::~WSys()
{
    if (-1 != key)
    {
        key = TlsFree(key);
    }
}

Socket *
WSys::createSocket()
{
    return new WSocket();
}

void
WSys::sleep(uint32_t ms)
{
    Sleep(ms);
}

int
WSys::startThread(ThreadInfo &info)
{
    unsigned int threadID;
    info.handle = (THREAD)_beginthreadex(NULL, 0, start_address, &info, 0, &threadID);

    if (info.handle == 0)
    {
        ERRE_COM("_beginthreadex fail");
        return -1;
    }

    return 0;
}

void
WSys::endThread(ThreadInfo &info, int timeout)
{

    if (info.handle)
    {
        unsigned int to = INFINITE;

        (0 == timeout) ? to = INFINITE : to = timeout;

        switch (WaitForSingleObject((void *)info.handle, to))
        {
        case WAIT_TIMEOUT:
            LOGE_COM("thread %ld end timeout\n", (long)info.handle);
            break;
        }

        ::CloseHandle(info.handle);
        info.handle = NULL;
    }
}

void
WSys::detachThread(ThreadInfo &info)
{
    if (info.handle)
    {
        ::CloseHandle(info.handle);
        info.handle = NULL;
    }
}

long
WSys::currentThreadID()
{
    return (long)GetCurrentThreadId();
}

void *
WSys::getTLSValue()
{
    if (-1 == key)
    {
        key = TlsAlloc();
    }

    return (void *)TlsGetValue(key);
}

void
WSys::setTLSValue(void *value)
{
    if (-1 == key)
    {
        key = TlsAlloc();
    }

    TlsSetValue(key, value);
}

uint64_t
WSys::currentTimeMillis()
{
    timeb tb;
    ftime(&tb);

    return (uint64_t)tb.time * 1000 + tb.millitm;
}

string      
WSys::currentTimeMillis2Str(int format)
{
    timeb tb;
    ftime(&tb);

    struct tm * tmPtr = NULL;
    tmPtr = localtime(&tb.time);
    char        szTime[128] = { 0 };

    switch (format)
    {
    case 0:
        snprintf(szTime, 40, "%04d-%02d-%02dT%02d:%02d:%02d.%03d",
            tmPtr->tm_year + 1900,
            tmPtr->tm_mon + 1,
            tmPtr->tm_mday,
            tmPtr->tm_hour,
            tmPtr->tm_min,
            tmPtr->tm_sec,
            tb.millitm);
        break;
    case 1:
        snprintf(szTime, 40, "%04d%02d%02d%02d%02d%02d%03d",
            tmPtr->tm_year + 1900,
            tmPtr->tm_mon + 1,
            tmPtr->tm_mday,
            tmPtr->tm_hour,
            tmPtr->tm_min,
            tmPtr->tm_sec,
            tb.millitm);
        break;
    default:
        snprintf(szTime, 40, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
            tmPtr->tm_year + 1900,
            tmPtr->tm_mon + 1,
            tmPtr->tm_mday,
            tmPtr->tm_hour,
            tmPtr->tm_min,
            tmPtr->tm_sec,
            tb.millitm);
        break;
    }

     string timeFmt = szTime;
    return timeFmt;
}

string&
WSys::ascii2utf8(string& ascii)
{
    int       size = MultiByteToWideChar(CP_ACP, 0, ascii.c_str(), -1, NULL, 0);
    wchar_t * unicode = new wchar_t[size + 1];

    memset(unicode, 0, (size + 1) * sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, 0, ascii.c_str(), -1, (LPWSTR)unicode, size);

    size   = WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)unicode, -1, NULL, 0, NULL, NULL);
    BYTE * utf8 = new BYTE[size + 1];

    memset(utf8, 0, size + 1);
    WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)unicode, -1, (char *)utf8, size, NULL, NULL);

    ascii = (char *)utf8;

    delete []unicode;
    delete []utf8;

    return ascii;
}

string&
WSys::utf2ascii(string& utf8)
{
    int       size    = MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, NULL, 0);
    wchar_t * unicode = new wchar_t[size + 1];

    /* UTF-8 to nicode */
    memset(unicode, 0, (size + 1)*sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, utf8.c_str(), -1, unicode, size);

    /* Unicode to ASCII */
    size   = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)unicode, -1, NULL, 0, NULL, NULL);
    char * asc = new char[size + 1];

    memset(asc, 0, size + 1);
    WideCharToMultiByte(CP_ACP, 0, (LPWSTR)unicode, -1, asc, size, NULL, NULL);

    utf8 = asc;

    delete []unicode;
    delete []asc;

    return utf8;
}

string
WSys::ascii2utf8(const char* ascii)
{
    int       size = MultiByteToWideChar(CP_ACP, 0, ascii, -1, NULL, 0);
    wchar_t * unicode = new wchar_t[size + 1];

    memset(unicode, 0, (size + 1) * sizeof(wchar_t));
    MultiByteToWideChar(CP_ACP, 0, ascii, -1, (LPWSTR)unicode, size);

    size   = WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)unicode, -1, NULL, 0, NULL, NULL);
    BYTE * utf8 = new BYTE[size + 1];

    memset(utf8, 0, size + 1);
    WideCharToMultiByte(CP_UTF8, 0, (LPWSTR)unicode, -1, (char *)utf8, size, NULL, NULL);

    string str = (char *)utf8;

    delete []unicode;
    delete []utf8;

    return str;
}

string
WSys::utf2ascii(const char* utf8)
{
    int       size = MultiByteToWideChar(CP_UTF8, 0, utf8, -1, NULL, 0);
    wchar_t * unicode = new wchar_t[size + 1];

    /* UTF-8 to unicode */
    memset(unicode, 0, (size + 1)*sizeof(wchar_t));
    MultiByteToWideChar(CP_UTF8, 0, utf8, -1, unicode, size);

    /* Unicode to ASCII */
    size = WideCharToMultiByte(CP_ACP, 0, (LPWSTR)unicode, -1, NULL, 0, NULL, NULL);
    char * asc = new char[size + 1];

    memset(asc, 0, size + 1);
    WideCharToMultiByte(CP_ACP, 0, (LPWSTR)unicode, -1, asc, size, NULL, NULL);

    string str = asc;

    delete []unicode;
    delete []asc;

    return str;
}

#if 0
string&
WSys::currentTime(string &timeFmt)
{
    time_t      tt;
    struct tm * tmPtr = NULL;
    char        szTime[40];

    tt = time(NULL);
    tmPtr = localtime(&tt);

    snprintf(szTime, 40, "%04d-%02d-%02d %02d:%02d:%02d",
             tmPtr->tm_year + 1900,
             tmPtr->tm_mon + 1,
             tmPtr->tm_mday,
             tmPtr->tm_hour,
             tmPtr->tm_min,
             tmPtr->tm_sec);

    timeFmt = szTime;

    return timeFmt;
}
#endif

string
WSys::currentTime()
{
#if 0
    time_t      tt;
    struct tm * tmPtr = NULL;
    char        szTime[40];
    string      timeFmt;

    tt = time(NULL);
    tmPtr = localtime(&tt);

    snprintf(szTime, 40, "%04d-%02d-%02d %02d:%02d:%02d",
             tmPtr->tm_year + 1900,
             tmPtr->tm_mon + 1,
             tmPtr->tm_mday,
             tmPtr->tm_hour,
             tmPtr->tm_min,
             tmPtr->tm_sec);

    timeFmt = szTime;
    return timeFmt;
#endif

    time_t t;
    string s;
    tm     localtime;
    char   buffer[128] = {0};

    time(&t);

    if (localtime_s(&localtime, &t) == 0)
    {
        if (::strftime(buffer, 128, "%Y-%m-%d %H:%M:%S", &localtime) > 0)
        {
            s = buffer;
        }
    }

    return s;
}

int
WSys::setLocalTime(datetime_s &dt, const string &tzone)
{
    return 0;
}

string
WSys::generateUID()
{
    string uid;
    char   buf[64] = {0};

    GUID guid;

    if (S_OK == ::CoCreateGuid(&guid))
    {
        snprintf(buf, sizeof(buf)
                 , "%08X-%04X-%04x-%02X%02X-%02X%02X%02X%02X%02X%02X"
                 , guid.Data1
                 , guid.Data2
                 , guid.Data3
                 , guid.Data4[0], guid.Data4[1]
                 , guid.Data4[2], guid.Data4[3], guid.Data4[4], guid.Data4[5]
                 , guid.Data4[6], guid.Data4[7]
                );
    }

    uid = buf;
    return uid;
}

string
WSys::strftime(time_t now)
{
    string s;
    tm     lt;
    char   buffer[128] = {0};

    if (0 == now)
    {
        time(&now);
    }

    if (localtime_s(&lt, &now) == 0)
    {
        if (::strftime(buffer, 128, "%Y-%m-%dT%H:%M:%SZ", &lt) > 0)
        {
            s = buffer;
        }
    }

    return s;
}

string
WSys::strftime(datetime_s &dt)
{
    if (dt.year < 1970
            || dt.month < 1
            || dt.month > 12
            || dt.day < 1
            || dt.day > 31)
    {
        return "";
    }

    string s;
    char   buffer[128] = {0};
    tm     tt;
    memset(&tt, 0, sizeof(tm));

    tt.tm_year = dt.year - 1900;
    tt.tm_mon  = dt.month - 1;
    tt.tm_mday = dt.day;
    tt.tm_hour = dt.hour;
    tt.tm_min  = dt.minute;
    tt.tm_sec  = dt.second;

    if (::strftime(buffer, 128, "%Y-%m-%dT%H:%M:%SZ", &tt) > 0)
    {
        s = buffer;
    }

    return s;
}

int
WSys::strptime(string &s, datetime_s &dt)
{
    if ("" == s)
    {
        return -1;
    }

#if 0
    tm tt;
    memset(&tt, 0, sizeof(tm));
    ::strptime(s.c_str(), "%Y-%m-%d %H:%M:%S", &tt);

    dt.year   = tt.tm_year + 100;
    dt.month  = tt.tm_mon + 1;
    dt.day    = tt.tm_mday;
    dt.hour   = tt.tm_hour;
    dt.minute = tt.tm_min ;
    dt.second = tt.tm_sec ;
#endif

    sscanf_s(s.c_str(), "%d-%d-%dT%d:%d:%dZ",
             &dt.year, &dt.month, &dt.day, &dt.hour, &dt.minute, &dt.second);

    return 0;
}

int
WSys::strptime(uint64_t ti, datetime_s &time)
{

    time_t tt = ti / 1000;
    tm     lt;
    if (ti <= 0)
    {
        return -1;
    }

    if (localtime_s(&lt, &tt) == 0)
    {
        time.year = lt.tm_year + 1900;
        time.month = lt.tm_mon + 1;
        time.day = lt.tm_mday;
        time.hour = lt.tm_hour;
        time.minute = lt.tm_min;
        time.second = lt.tm_sec;
        return 0;
    }
    else
    {
        return -1;
    }
    
}

uint64_t
WSys::datetime2int(datetime_s &dt)
{
    tm tm_cur = { 0 };
    tm_cur.tm_year = dt.year - 1900;
    tm_cur.tm_mon = dt.month - 1;
    tm_cur.tm_mday = dt.day;
    tm_cur.tm_hour = dt.hour;
    tm_cur.tm_min = dt.minute;
    tm_cur.tm_sec = dt.second;

    time_t current = mktime(&tm_cur);

    return current;
}

string
WSys::toString(int value)
{
    char buf[20] = {0};
    snprintf_s(buf, sizeof(buf), "%s", value);

    return buf;
}

string
WSys::toString(uint32_t value)
{
    char buf[20] = {0};
    snprintf_s(buf, sizeof(buf), "%s", value);

    return buf;
}

string
WSys::toString(uint64_t value)
{
    char buf[20] = {0};
    snprintf_s(buf, sizeof(buf), "%s", value);

    return buf;
}

bool
WSys::copyFile(const char* source, const char* destination)
{
    return CopyFile(source, destination, TRUE)?true:false;
}

bool
WSys::removeFile(const char* path)
{
    return remove(path) == 0 ? true : false;
}

int
WSys::getPid(const char* process_name, const char* user)
{
    return -1;
}

bool
WSys::getCpuUsage(int pid, int &proUsed, int &sysUsed)
{
    return false;
}

bool
WSys::getMemUsage(int pid, int &pusedM, int &vused, int &totalM, int &freeM, int &virTotalM, int &virFreeM)
{
    PROCESS_MEMORY_COUNTERS pmc;
    if (GetProcessMemoryInfo(m_curProcessHandle, &pmc, sizeof(pmc)))
    {
        pusedM = pmc.WorkingSetSize >> 20;
        vused = pmc.PagefileUsage >> 20;
    }

    MEMORYSTATUSEX MemStatEx;
    MemStatEx.dwLength = sizeof(MEMORYSTATUSEX);
    if (GlobalMemoryStatusEx(&MemStatEx))
    {
        totalM = MemStatEx.ullTotalPhys >> 20;
        freeM = MemStatEx.ullAvailPhys >> 20;
        virTotalM = MemStatEx.ullTotalVirtual >> 20;
        virFreeM = MemStatEx.ullAvailVirtual >> 20;
    }

    return true;
}

bool
WSys::getDiskUsage(unsigned int &totalM, unsigned int &usedM)
{
    totalM = 0;
    usedM = 0;

    /*²ÎÊý ÀàÐÍ¼°ËµÃ÷
    lpRootPathName String£¬²»°üÀ¨¾íÃûµÄ´ÅÅÌ¸ùÂ·¾¶Ãû
    lpFreeBytesAvailableToCaller LARGE_INTEGER£¬Ö¸¶¨Ò»¸ö±äÁ¿£¬ÓÃÓÚÈÝÄÉµ÷ÓÃÕß¿ÉÓÃµÄ×Ö½ÚÊýÁ¿
    lpTotalNumberOfBytes LARGE_INTEGER£¬        Ö¸¶¨Ò»¸ö±äÁ¿£¬ÓÃÓÚÈÝÄÉ´ÅÅÌÉÏµÄ×Ü×Ö½ÚÊý
    lpTotalNumberOfFreeBytes LARGE_INTEGER£¬    Ö¸¶¨Ò»¸ö±äÁ¿£¬ÓÃÓÚÈÝÄÉ´ÅÅÌÉÏ¿ÉÓÃµÄ×Ö½ÚÊý
    ÊÊÓÃÆ½Ì¨
    Windows 95 OSR2£¬Windows NT 4.0*/
    ULARGE_INTEGER FreeBytesAvailable, TotalNumberOfBytes, TotalNumberOfFreeBytes;

    if (GetDiskFreeSpaceEx("C:", &FreeBytesAvailable, &TotalNumberOfBytes, &TotalNumberOfFreeBytes))
    {
        totalM +=  (TotalNumberOfBytes.QuadPart) >> 20;
        usedM  += ((TotalNumberOfBytes.QuadPart) >> 20) - ((TotalNumberOfFreeBytes.QuadPart) >> 20);
    }

    if (GetDiskFreeSpaceEx("D:", &FreeBytesAvailable, &TotalNumberOfBytes, &TotalNumberOfFreeBytes))
    {
        totalM += (TotalNumberOfBytes.QuadPart) >> 20;
        usedM += ((TotalNumberOfBytes.QuadPart) >> 20) - ((TotalNumberOfFreeBytes.QuadPart) >> 20);
    }

    if (GetDiskFreeSpaceEx("E:", &FreeBytesAvailable, &TotalNumberOfBytes, &TotalNumberOfFreeBytes))
    {
        totalM += (TotalNumberOfBytes.QuadPart) >> 20;
        usedM += ((TotalNumberOfBytes.QuadPart) >> 20) - ((TotalNumberOfFreeBytes.QuadPart) >> 20);
    }

    if (GetDiskFreeSpaceEx("F:", &FreeBytesAvailable, &TotalNumberOfBytes, &TotalNumberOfFreeBytes))
    {
        totalM += (TotalNumberOfBytes.QuadPart) >> 20;
        usedM += ((TotalNumberOfBytes.QuadPart) >> 20) - ((TotalNumberOfFreeBytes.QuadPart) >> 20);
    }
    return 0;
}

bool
WSys::getInterfacUsage(const char* interfac, int upspeedKbs, int downspeedKbs)
{
    return false;
}

bool
WSys::getCpuTime(int pid, unsigned long long &totalTime, unsigned long long &pidUserTime, unsigned long long &sysUserTime)
{
    FILETIME idleTime;//¿ÕÏÐÊ±¼ä 
    FILETIME kernelTime;//ºËÐÄÌ¬Ê±¼ä 
    FILETIME userTime;//ÓÃ»§Ì¬Ê±¼ä 
    GetSystemTimes(&idleTime, &kernelTime, &userTime);
    totalTime = (userTime.dwHighDateTime << 32 | userTime.dwLowDateTime) + (kernelTime.dwHighDateTime << 32 | kernelTime.dwLowDateTime);
    sysUserTime = totalTime - (idleTime.dwHighDateTime << 32 | idleTime.dwLowDateTime);

    FILETIME creation_time;
    FILETIME exit_time;
    FILETIME kernel_time;
    FILETIME user_time;
    GetProcessTimes(m_curProcessHandle, &creation_time, &exit_time, &kernel_time, &user_time);
    pidUserTime = ((user_time.dwHighDateTime << 32 | user_time.dwLowDateTime) + (kernel_time.dwHighDateTime << 32 | kernel_time.dwLowDateTime)) / 4;

    return true;
}

bool
WSys::getInterfaceIO(const char* interfac, unsigned long long& recvKb, unsigned long long& sendKb)
{
    MIB_IFTABLE *pTable = new MIB_IFTABLE[sizeof(MIB_IFTABLE)];
    DWORD dwAdapters = 0;
    ULONG uRetCode = GetIfTable(pTable, &dwAdapters, TRUE);
    if (uRetCode == ERROR_INSUFFICIENT_BUFFER)
    {
        delete[]pTable;
        pTable = new MIB_IFTABLE[dwAdapters];
    }

    uRetCode = GetIfTable(pTable, &dwAdapters, TRUE);
    if (uRetCode == NO_ERROR)
    {
		for (unsigned long i = 0; i < (pTable->dwNumEntries); i++)
        {
            MIB_IFROW Row = pTable->table[i];

            recvKb = (Row.dwInOctets) >> 13;
            sendKb = (Row.dwOutOctets) >> 13;
        }       
    }
    delete[]pTable;

    return true;


}

