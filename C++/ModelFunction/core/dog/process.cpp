#include "process.h"
#include "sys.h"


#ifdef _WIN32

#include <tlhelp32.h>

int isRuning(string ID)
{
    // 利用事件对象实现一个程序只允许运行一个进程
	HANDLE  hEvent = CreateEvent(NULL, FALSE, TRUE, ID.c_str());
    if (GetLastError() == ERROR_ALREADY_EXISTS)
    {
        CloseHandle(hEvent);
        return -1;
    }
    return 0;
}

int initDemo()
{
    return 0;
}

int getProcessNameByPid(IN const char* ProcessName)	//根据进程名获得PID,同GetProcessIDByName()
{
    HANDLE hSnapshot = ::CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    DWORD pid = -1;
    PROCESSENTRY32 pe = { 0 };
    pe.dwSize = sizeof(pe);

    bool bMore = ::Process32First(hSnapshot, &pe);
    while (bMore)
    {
        if (!strcmp(ProcessName, pe.szExeFile))
        {
            pid = pe.th32ProcessID;
            break;
        }
        else
        {
            bMore = ::Process32Next(hSnapshot, &pe);
        }
    }
    return pid;
}

int killProcess(const char* appName)
{
    DWORD pid = getProcessNameByPid(appName);
    if (pid < 0)
    {
        return -1;
    }
    __try
    {
        //if (!this->EnableDebugPriv(SE_DEBUG_NAME))		//提升本进程权限
        //{
        //    cout << "Debug Access Fuil" << endl;
        //    return false;
        //}
        HANDLE process;
        process = OpenProcess(PROCESS_ALL_ACCESS, false, pid);		//打开进程
        if (!process)
        {
            cout << "OpenProcess: " << pid << " Fail" << endl;
            CloseHandle(process);
            return false;
        }
        else
        {
            int flag = TerminateProcess(process, 0);	//结束进程API函数
            char processName[128] = { 0 };
            
            if (flag)
            {
                cout << "Kill Process PID: " << pid << " --- " << processName << " Success" << endl;
                //return true;
            }
            else
            {
                cout << "Kill Process PID: " << pid << " --- " << processName << " Fail" << endl;
                //return false;
            }
			CloseHandle(process);
        }
        
    }
    __except (1)
    {
        cout << "Unknown Error: " << GetLastError() << endl;
        return false;
    }
    return true;
}

int createProcess(const char* exeName, const char* appArgv, const char* workPath)
{
    STARTUPINFO si = { sizeof(si) };
    si.dwFlags |= STARTF_USESHOWWINDOW;
    si.wShowWindow = SW_SHOW;
    PROCESS_INFORMATION pi;

    killProcess("WerFault.exe");

    char tmp[128] = { 0 };
    memcpy(tmp, workPath, strlen(workPath));
    strcat(tmp, exeName);
    strcat(tmp, ".exe");
    strcat(tmp, "  ");
    strcat(tmp, appArgv);
    //HINSTANCE hInstance = ShellExecute(NULL, "open", exeName, appArgv, workPath, SW_SHOWNORMAL);



    BOOL callStatus = CreateProcess(NULL, tmp, NULL, NULL, false, 0, NULL, workPath, &si, &pi);
    if (callStatus == TRUE)
    {
        return 0;
    }

    //BOOL callStatus = CreateProcess(appPath, appArgv, NULL, NULL, false, 0, NULL, workPath, &si, &pi);

    return 0;
}

#else

#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <sys/param.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <syslog.h>

int isRuning(string ID)
{
	char dogpath[128];
	sprintf(dogpath, "/tmp/%s.pid", ID.c_str());
	int fd = open(dogpath, O_RDWR|O_CREAT, S_IRUSR|S_IWUSR|S_IRGRP|S_IROTH);
    if (fd < 0)
    {    
        exit(1);
    }

    struct flock fl;
    fl.l_type = F_WRLCK;
    fl.l_start = 0;
    fl.l_whence = SEEK_SET;
    fl.l_len = 0;

    if (fcntl(fd, F_SETLK, &fl) == -1)
    {
        if (errno == EACCES || errno == EAGAIN) {
            close(fd);
            return 1;
        }    
        exit(1);
    }

    char buf[16];
    ftruncate(fd, 0);
    sprintf(buf, "%ld", (long)getpid());
    write(fd, buf, strlen(buf) + 1);
    return 0;
}

int initDemo()
{
    pid_t pid = fork();
    if (pid > 0)
    {
        exit(0);
    }
    else if (pid < 0)
    {
        perror("initDemo, fork faild");
        exit(-1);
    }

    setsid();

//    pid=fork();
//    if( pid > 0)
//    {
//        exit(0);
//    }
//    else if( pid< 0)
//    {
//        perror("initDemo, fork faild");
//        exit(-1);
//    }

    //关闭所有从父进程继承的不再需要的文件描述符
    for(int i=0;i< NOFILE;close(i++));

    //改变工作目录，使得进程不与任何文件系统联系
    //chdir("/mnt/hgfs/lalala/test");

    //将文件当时创建屏蔽字设置为0
    umask(0);

    //忽略SIGCHLD信号
    signal(SIGCHLD,SIG_IGN);

    return 0;
}

int killProcess(const char* appName)
{
    return 0;
}

int createProcess(const char* exeName, const char* appArgv, const char* workPath)
{
    char tmp[512] = { 0 };
    getcwd(tmp, 512);

    strcat(tmp, "/");
    strcat(tmp, workPath);
    strcat(tmp, exeName);
//    strcat(tmp, "  ");
//    strcat(tmp, appArgv);

    pid_t pid = fork();
    if (pid < 0)
    {
        perror("fork failed");
        exit(1);
    }
    else if (pid == 0)
    {
        chdir(workPath);

        if (0 > execl(tmp, exeName, appArgv, NULL))
        {
            //LOGE_MSV("createProcess faild, path:%s exeName:%s arg:%s", tmp, exeName, appArgv);
            exit(1);
        }
    }
}

#endif

