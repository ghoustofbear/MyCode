#include "CELLProcessmgr.h"
#include <stdio.h>
#include <string.h>
#include <iostream>

#ifdef _WIN32
#include <windows.h>
#include <tlhelp32.h>
#else
//exec或popen执行ps的命令行，然后运用某几个字符串匹配函数。
#include <unistd.h>
#include <signal.h>
#endif // _WIN32

#ifdef _WIN32
char* wchar2char(const wchar_t *src)
{
    char *dest;
    int len_src = WideCharToMultiByte(CP_ACP, 0, src, wcslen(src), NULL, 0, NULL, NULL);
    dest = new char[len_src + 1];
    WideCharToMultiByte(CP_ACP, 0, src, wcslen(src), dest, len_src, NULL, NULL);
    dest[len_src] = '\0';
    return dest;
}
#endif // _WIN32

std::list<uint64_t> CELLProcessMgr::QueryProcessIDByName(const char *strName)
{
    m_listProcessID.clear();
#ifdef _WIN32
    //static FILE* fp = fopen("C:/Users/zhangshenyu/Desktop/log.txt", "wb");
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (INVALID_HANDLE_VALUE == hSnapshot)
    {
        return m_listProcessID;
    }
    PROCESSENTRY32 pe = { sizeof(pe) };

    for (BOOL ret = Process32First(hSnapshot, &pe); ret; ret = Process32Next(hSnapshot, &pe))
    {
        char *cstr = NULL;
#ifdef UNICODE
        cstr = wchar2char((const wchar_t *)pe.szExeFile);
#else
        cstr = pe.szExeFile;
#endif
        //char logbuf[256] = { 0 };
        //sprintf_s(logbuf, "cstr=%s,cstr.len=%d,strName=%s,strName.len=%d\n", cstr, strlen(cstr), strName, strlen(strName));
        //fputs(logbuf, fp);
        //fflush(fp);
        if (strcmp(cstr, strName) == 0)
        {
            m_listProcessID.push_back(pe.th32ProcessID);
        }
#ifdef UNICODE
        delete[]cstr;
#endif
    }


    CloseHandle(hSnapshot);
#else
    FILE *pstr;
    char cmd[128];
    char buff[512];
    char *p;
    pid_t pID;
    int pidnum;
    int ret = 3;
    memset(cmd, 0, sizeof(cmd));

    sprintf(cmd, "ps -ef|grep %s ", strName);
    pstr = popen(cmd, "r");//

    if (pstr == NULL)
    {
        return m_listProcessID;
    }

    while (!feof(pstr))
    {
        memset(buff, 0, sizeof(buff));
        fgets(buff, 512, pstr);
        p = strtok(buff, " ");  //字符串分割，linux内核2.6以上已用strsep替换
        p = strtok(NULL, " "); //这句是否去掉，取决于当前系统中ps后，进程ID号是否是第一个字段
        if (p == NULL)
        {
            break;
        }
        if (strlen(p) == 0)
        {
            break;
        }
        if ((pidnum = atoi(p)) == 0)
        {
            break;
        }
        pID = (pid_t)pidnum;
        ret = kill(pID, 0);//这里不是要杀死进程，而是验证一下进程是否真的存在，返回0表示真的存在
        if (0 == ret)
        {
            m_listProcessID.push_back(pidnum);
            //printf("process: %s exist!\n", name);
        }
    }

    pclose(pstr);

    return m_listProcessID;
#endif // _WIN32
    return m_listProcessID;
}

void CELLProcessMgr::KillProcessByName(const char *strName)
{
    QueryProcessIDByName(strName);
    std::list<uint64_t>::iterator i;
    for (i = m_listProcessID.begin(); i != m_listProcessID.end(); i++)
    {
        KillProcessByPID(*i);
    }
}

bool CELLProcessMgr::KillProcessByPID(const uint64_t pid)
{
#ifdef _WIN32
    //printf("Kill进程Pid = %d\n", pID); 
    //关闭进程
    HANDLE killHandle = OpenProcess(PROCESS_TERMINATE | PROCESS_QUERY_INFORMATION |   // Required by Alpha
        PROCESS_CREATE_THREAD |   // For CreateRemoteThread
        PROCESS_VM_OPERATION |   // For VirtualAllocEx/VirtualFreeEx
        PROCESS_VM_WRITE,             // For WriteProcessMemory);
        FALSE, pid);
    if (killHandle == NULL)
        return false;
    TerminateProcess(killHandle, 0);
    CloseHandle(killHandle);
    return true;
#else
    kill(pid, 9);
#endif // _WIN32
}
