#include <sys/time.h>
#include <time.h>
#include <signal.h>
#include "usys.h"
#include "usocket.h"
#include "uuid.h"
#include <unistd.h>
#include "tls_error.h"
#include "utility.h"
#include "iconv.h"

static pthread_key_t  key  = -1;
static pthread_once_t once = PTHREAD_ONCE_INIT;

static void
sighandler(int signal)
{
    if (SIGPIPE == signal || 28 == signal || SIGALRM == signal)
    {
        LOGI_COM("Thread 0x%lu client received signal %d, ignore this signal.\n", (unsigned long)pthread_self(), signal);
        return;
    }

    LOGF_COM("Thread 0x%lu client received signal %d, exit.\n", (unsigned long)pthread_self(), signal);

    exit(1);
}

static void
TLS_free(void *ptr)
{
    free(ptr);
}

static void
TLS_key_create()
{
    pthread_key_create(&key, TLS_free);
}

USys::USys()
{
    for (int i = 0; i < 32; i ++)
    {
        if (SIGCHLD == i)
            continue;

        //signal(i, sighandler);

        if (i == 13)
        {
            signal(13, SIG_IGN);
        }
    }
}

USys::~USys()
{

}

Socket *
USys::createSocket()
{
    return new USocket();
}

void
USys::sleep(uint32_t ms)
{
    usleep(ms * 1000);
}

int
USys::startThread(ThreadInfo &info)
{
    typedef void* (*start_address)(void *);

    if (0 != pthread_create(&info.handle, NULL, (start_address)info.func, &info))
    {
        ERRE_COM("pthread_create called fail");

        return -1;
    }

    return 0;
}

void
USys::endThread(ThreadInfo &info, int timeout)
{
    if (info.handle)
    {
        pthread_join(info.handle, NULL);
        info.handle = 0;
    }
}

void
USys::detachThread(ThreadInfo &info)
{
    if (info.handle)
    {
        pthread_detach(info.handle);
        info.handle = 0;
    }
}

long USys::currentThreadID()
{
    return (long)pthread_self();
}

void *
USys::getTLSValue()
{
    pthread_once(&once, TLS_key_create);
    return pthread_getspecific(key);
}

void
USys::setTLSValue(void *value)
{
    pthread_once(&once, TLS_key_create);
    pthread_setspecific(key, value);
}

uint64_t
USys::currentTimeMillis()
{

    timeval tv;
    gettimeofday(&tv, 0);
    return (uint64_t)tv.tv_sec * 1000 + (uint64_t)tv.tv_usec/1000;


    //.autorelabeltime tb;
    //ftime(&tb);

    /* return (uint64_t)tb.time * 1000 + tb.millitm; */
}

string&
USys::ascii2utf8(string& ascii)
{
    iconv_t cd = iconv_open("utf-8", "gb2312");
	if (cd == 0)
    {
        ascii = "";
        return ascii;
    }
	
    size_t inlen = ascii.size();
    char* inbuf = (char*)ascii.c_str();
	
	char *outbuf = new char[1024*10];
	size_t outlen = 1024*10;
	memset(outbuf, 0, outlen);
	
    char *pin = inbuf;
    char *pout = outbuf;

	
    if (iconv(cd, &pin, &inlen, &pout, &outlen) == -1)
	{
        ascii = "";
	}
    else
    {
        ascii = outbuf;
    }

	iconv_close(cd);
	delete[] outbuf;

    return ascii;
}

string&
USys::utf2ascii(string& utf8)
{
    iconv_t cd = iconv_open("gb2312", "utf-8");
	if (cd == 0)
    {
        utf8 = "";
        return utf8;
    }
	size_t inlen = utf8.size();
	char* inbuf = (char*)utf8.c_str();
	char *outbuf = new char[1024*10];
	size_t outlen = 1024*10;
	memset(outbuf, 0, outlen);
	
    char *pin = inbuf;
    char *pout = outbuf;

    if (iconv(cd, &pin, &inlen, &pout, &outlen) == -1)
	{
        utf8 = "";
	}
    else
    {
        utf8 = outbuf;
    }
	iconv_close(cd);
	delete[] outbuf;

    return utf8;
}

string
USys::ascii2utf8(const char* ascii)
{
    iconv_t cd = iconv_open("utf-8", "gb2312");
	if (cd == 0)
		return "";
	
    size_t inlen = strlen(ascii);
	char* inbuf = (char*)ascii;
	
    char *outbuf = new char[1024*10];
    size_t outlen = 1024*10;
	memset(outbuf, 0, outlen);
	
    char *pin = inbuf;
    char *pout = outbuf;

    if (iconv(cd, &pin, &inlen, &pout, &outlen) == -1)
	{
		delete[] outbuf;
		return "";
	}
	iconv_close(cd);
	
    string ret = outbuf;

    delete[] outbuf;

	return ret;
}

string
USys::utf2ascii(const char* utf8)
{
    iconv_t cd = iconv_open("gbk", "utf-8");
	if (cd == 0)
		return "";
	//size_t inlen = sizeof(utf8);
	size_t inlen =  strlen(utf8); // modfity is wxr
	char* inbuf = (char*)utf8;
	
	char *outbuf = new char[1024*10];
	size_t outlen = 1024*10;
	memset(outbuf, 0, outlen);
	
    char *pin = inbuf;
    char *pout = outbuf;

    if (iconv(cd, &pin, &inlen, &pout, &outlen) == -1)
	{
		delete[] outbuf;
		return "";
	}
	iconv_close(cd);
    string ret = outbuf;
	delete[] outbuf;

	return ret;
}

#if 0
string&
USys::currentTime(string &timeFmt)
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
USys::currentTime()
{
    time_t t;
    string s;
    tm     localtime;
    char   buffer[128] = {0};

    time(&t);

    if (localtime_r(&t, &localtime))
    {
        if (::strftime(buffer, 128, "%Y-%m-%d %H:%M:%S", &localtime) > 0)
        {
            s = buffer;
        }
    }

    return s;
}

string
USys::currentTimeMillis2Str(int format)
{

    timeval tv;
    gettimeofday(&tv, 0);

    struct tm *tmPtr = localtime(&(tv.tv_sec));
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
            tv.tv_usec);
        break;
    default:
        snprintf(szTime, 40, "%04d-%02d-%02d %02d:%02d:%02d.%03d",
            tmPtr->tm_year + 1900,
            tmPtr->tm_mon + 1,
            tmPtr->tm_mday,
            tmPtr->tm_hour,
            tmPtr->tm_min,
            tmPtr->tm_sec,
            tv.tv_usec);
        break;
    }

     string timeFmt = szTime;
    return timeFmt;
}

int
USys::setLocalTime(datetime_s &dt, const string &tzone)
{
	struct tm tptr;
    struct timeval tv;

    tptr.tm_year = dt.year - 1900;
    tptr.tm_mon = dt.month - 1;
    tptr.tm_mday = dt.day;
    tptr.tm_hour = dt.hour;
    tptr.tm_min = dt.minute;
    tptr.tm_sec = dt.second;

    tv.tv_sec = mktime(&tptr);
    tv.tv_usec = 0;
	
    return settimeofday(&tv, NULL);
}

string
USys::generateUID()
{
    string uid;
    char   buf[64] = {0};

    uuid_t uuid = {0};
    uuid_generate(uuid);
    uuid_unparse(uuid, buf);

    uid = buf;

    return uid;
}

string
USys::strftime(time_t now)
{
    string s;
    tm     lt;
    char   buffer[128] = {0};

    if (0 == now)
    {
        time(&now);
    }

    if (localtime_r(&now, &lt))
    {
        if (::strftime(buffer, 128, "%Y-%m-%dT%H:%M:%SZ", &lt) > 0)
        {
            s = buffer;
        }
    }

    return s;
}

string
USys::strftime(datetime_s &dt)
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

    /* sorry, unimplemented: non-trivial designated initializers not supported for cpp suffix file */
    /* { */
    /*     .tm_year = datetime.year, */
    /*     .tm_mon  = datetime.month, */
    /*     .tm_mday = datetime.day, */
    /*     .tm_hour = datetime.hour, */
    /*     .tm_min  = datetime.minute, */
    /*     .tm_sec  = datetime.second, */
    /* }; */

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
USys::strptime(string &s, datetime_s &dt)
{
    if ("" == s)
    {
        return -1;
    }

    tm tt;
    memset(&tt, 0, sizeof(tm));
    ::strptime(s.c_str(), "%Y-%m-%dT%H:%M:%SZ", &tt);

    dt.year   = tt.tm_year + 1900;
    dt.month  = tt.tm_mon + 1;
    dt.day    = tt.tm_mday;
    dt.hour   = tt.tm_hour;
    dt.minute = tt.tm_min ;
    dt.second = tt.tm_sec ;

    return 0;
}

int
USys::strptime(uint64_t ti, datetime_s &dt)
{
    if (ti <= 0)
    {
        return -1;
    }

    time_t t = ti/1000;
    tm     tt;
    if (localtime_r(&t, &tt))
    {
		dt.year   = tt.tm_year + 1900;
		dt.month  = tt.tm_mon + 1;
		dt.day    = tt.tm_mday;
		dt.hour   = tt.tm_hour;
		dt.minute = tt.tm_min ;
		dt.second = tt.tm_sec ;
		return 0;
    }
	else
	{
		return -1;
	}
}
uint64_t USys::datetime2int(datetime_s &dt)
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
USys::toString(int value)
{
    char buf[20] = {0};
    snprintf_s(buf, sizeof(buf), "%s", value);

    return buf;
}

string
USys::toString(uint32_t value)
{
    char buf[20] = {0};
    snprintf_s(buf, sizeof(buf), "%s", value);

    return buf;
}

string
USys::toString(uint64_t value)
{
    char buf[20] = {0};
    snprintf_s(buf, sizeof(buf), "%s", value);

    return buf;
}

bool
USys::copyFile(const char* source, const char* destination)
{
    return false;
}

bool USys::removeFile(const char *path)
{
    return remove(path) == 0 ? true : false;
}

int USys::getPid(const char *process_name, const char *user)
{
    if(user == NULL)
    {
        user = getlogin();
    }

    char cmd[128] = {0};
    if (user)
    {
        sprintf(cmd, "pgrep %s -u %s", process_name, user);
    }

    FILE *pstr = popen(cmd, "r");
    if(pstr == NULL)
    {
        return -1;
    }

    char buff[32] = {0};
    if(NULL == fgets(buff, 512, pstr))
    {
        pclose(pstr);
        return -1;
    }

    int ret = atoi(buff);
    pclose(pstr);

    return ret;
}

bool USys::getCpuUsage(int pid, int &proUsed, int &sysUsed)
{
    unsigned long long totalcputime1 = 0,totalcputime2 = 0;
    unsigned long long procputime1 = 0,procputime2 = 0;
    unsigned long long syscputime1 = 0,syscputime2 = 0;
    getCpuTime(pid, totalcputime1, procputime1, syscputime1);
    //延迟1000毫秒
    usleep(1000000);
    getCpuTime(pid, totalcputime2, procputime2, syscputime2);
    proUsed = (procputime2 - procputime1)/(totalcputime2 - totalcputime1);
    sysUsed = (syscputime2 - syscputime1)/(totalcputime2 - totalcputime1);
    return true;
}

bool USys::getMemUsage(int pid, int &pusedM, int &vusedM, int &totalM, int &freeM, int &virTotalM, int &virFreeM)
{
    char line_buff[256] = {0};
    char name[32]={0};
    int vmrss = 0;
    char file[64] = {0};
    sprintf(file,"/proc/%d/status", pid);
    FILE* fd = fopen (file, "r");
    if(fd)
    {
        while (!feof(fd))
        {
            memset(name, 0, sizeof(name));
            fgets (line_buff, sizeof(line_buff), fd);
            sscanf (line_buff, "%s %d", name, &vmrss);
            if (strcmp(name, "VmRSS:") == 0)
            {
                pusedM = vmrss >> 10;
                break;
            }
            else if (strcmp(name, "VmSize:") == 0)
            {
                vusedM = vmrss >> 10;
            }
        }
        fclose(fd);
        fd = NULL;
    }

    //获取物理内存使用情况
    fd = fopen ("/proc/meminfo", "r");
    int tmpval = 0;
    while (!feof(fd))
    {
        memset(name, 0, 32);
        fgets (line_buff, sizeof(line_buff), fd);
        sscanf (line_buff, "%s %d", name, &tmpval);
        if (strcmp(name, "MemTotal:") == 0)
        {
            totalM = tmpval >> 10;
        }
        else if (strcmp(name, "MemFree:") == 0)
        {
            freeM = tmpval >> 10;
        }
        else if (strcmp(name, "SwapTotal:") == 0)
        {
            virTotalM = tmpval >> 10;
        }
        else if (strcmp(name, "SwapFree:") == 0)
        {
            virFreeM =  tmpval >> 10;
            break;
        }
    }
    fclose(fd);
    return true;
}

bool USys::getDiskUsage(unsigned int &totalM, unsigned int &freeM)
{
    char currentDirectoryPath[128] = {0};
    getcwd(currentDirectoryPath, 128);
    char cmd[132]="df ";
    strcat(cmd,currentDirectoryPath);
    FILE* pipe = popen(cmd, "r");

    unsigned long blocks = 0;
    unsigned long used = 0;
    unsigned long free = 0;
    char fileSys[20] = {0};
    char percent[8] = {0};
    char moment[20] = {0};
    char buffer[256] = {0};
    fgets(buffer, sizeof(buffer), pipe);
    memset(buffer, 0, sizeof(buffer));
    if(fgets(buffer, sizeof(buffer), pipe)!=NULL)
    {
        char *ma = deleteSpace(buffer);
        sscanf(buffer,"%s %Lu %Lu %Lu %s %s",fileSys, &blocks, &used, &free, percent,moment);
        totalM = blocks >> 10;
        freeM = free >> 10;
        pclose(pipe);
        return true;
    }
    else
    {
        pclose(pipe);
        return false;
    }
}

bool USys::getInterfacUsage(const char *interface, int upspeedKbs, int downspeedKbs)
{
    unsigned long long recvKb1 = 0, recvKb2 = 0;
    unsigned long long sendKb1 = 0, sendKb2 = 0;
    getInterfaceIO(interface, recvKb1, sendKb1);
    //延迟1000毫秒
    usleep(1000000);
    getInterfaceIO(interface, recvKb2, sendKb2);

    downspeedKbs = (recvKb2 - recvKb1)/1;
    upspeedKbs = (sendKb2 - sendKb1)/1;

    return false;
}

bool USys::getCpuTime(int pid, unsigned long long &totalTime, unsigned long long &pidUserTime, unsigned long long &sysUserTime)
{
    char line_buff[1024] = {0};
    char file[64] = {0};
    sprintf(file,"/proc/%d/stat", pid);
    FILE *fd = fopen (file, "r");
    if (fd)
    {
        unsigned int tpid = 0;
        unsigned int tutime = 0;
        unsigned int tstime = 0;
        unsigned int tcutime = 0;
        unsigned int tcstime = 0;

        fgets (line_buff, sizeof(line_buff), fd);
        sscanf(line_buff,"%u",&tpid);

        const char* q = getItems(line_buff,14);//取得从第14项开始的起始指针
        sscanf(q,"%u %u %u %u",&tutime,&tstime,&tcutime,&tcstime);//格式化第14,15,16,17项
        pidUserTime = tutime + tstime + tcutime + tcstime;
        fclose(fd);
        fd = NULL;
    }

    fd = fopen ("/proc/stat", "r");
    if (fd)
    {
        char name[16];
        unsigned int tuser = 0;
        unsigned int tnice = 0;
        unsigned int tsystem = 0;
        unsigned int tidle = 0;
        memset(line_buff, 0, sizeof(line_buff));

        fgets (line_buff, sizeof(line_buff), fd);
        sscanf (line_buff, "%s %u %u %u %u", name, &tuser, &tnice,&tsystem, &tidle);
        totalTime = tuser + tnice + tsystem + tidle;
        sysUserTime = tuser + tsystem;
        fclose(fd);
        fd = NULL;
    }
    return true;
}

const char* USys::getItems(const char *buffer, int ie)
{
    assert(buffer);
    char* p = (char*)buffer;//指向缓冲区
    int len = strlen(buffer);
    int count = 0;//统计空格数
    if (1 == ie || ie < 1)
    {
        return p;
    }
    int i;

    for (i=0; i<len; i++)
    {
        if (' ' == *p)
        {
            count++;
            if (count == ie-1)
            {
                p++;
                break;
            }
        }
        p++;
    }
    return p;
}

bool USys::getInterfaceIO(const char* interface, unsigned long long& recvKb, unsigned long long& sendKb)
{
    FILE * fd = fopen("/proc/net/dev", "r");
    char buffer[1024] = {0};
    char name[32];
    unsigned long long recvByte = 0;
    unsigned long long sendByte = 0;
    unsigned long long tmp = 0;
    int i = 0;
    while(!feof(fd))
    {
        if(fgets(buffer, sizeof(buffer), fd)!=NULL)
        {
            if(strstr(buffer,interface)!=NULL)
            {
                //char *ma = deleteSpace(buffer);
                sscanf(buffer,"%s %lu %lu %lu %lu %lu %lu %lu %lu %lu", name, &recvByte, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &tmp, &sendByte);
                break;
            }
        }
    }
    fclose(fd);

    recvKb = recvByte >> 10;
    sendKb = sendByte >> 10;
    return true;
}

char * USys::deleteSpace(char* str)
{
    char * tail = str;
    char * next = str;

    while(*next)
    {
        // 两个if可以合并到一块儿，这样写只是为了看着方便
        if(*next != ' ')
        {
            // 查找不是空格的字符
            *tail = *next;
            tail++;
        }
        if(*next == ' ' && *(next-1) != ' ')
        {
            // 只留一个空格的判断条件；当前字符为空格且前一个不为空格。
            *tail = *next;
            tail++;
        }
        next++;
    }
    // 字符串结束
    *tail='\0';
    return str;
}
