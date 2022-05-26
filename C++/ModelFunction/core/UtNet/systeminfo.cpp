//#include "stdafx.h"
#include "systeminfo.h"
#define MALLOC(x) HeapAlloc(GetProcessHeap(), 0, (x))    
#define FREE(x) HeapFree(GetProcessHeap(), 0, (x))   


SystemInfo  g_SystemInfo;
SystemInfo *g_pSystemInfo = &g_SystemInfo;

SystemInfo::SystemInfo() 
{	
	m_dwSendIntf = -1;
	m_dwRecvIntf = -1;

	Init();
};

int SystemInfo::Init()
{
	PerfDataInitialize();
	GetAdaptInfo(NULL);

	return 0;
}
 
int SystemInfo::GetAdaptInfo(list<NETIFINFO> *listAdaptInfo)
{
	PIP_ADAPTER_INFO pAdapterInfo;   
    PIP_ADAPTER_INFO pAdapter = NULL;   
    DWORD dwRetVal = 0;   
   
    /* variables used to print DHCP time info */       
	NETIFINFO stNetInfo;

    ULONG ulOutBufLen = sizeof (IP_ADAPTER_INFO);   
    pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(sizeof (IP_ADAPTER_INFO));   
    if (pAdapterInfo == NULL) {   
        printf("Error allocating memory needed to call GetAdaptersinfo\n");   
        return 1;   
    } 
	memset(pAdapterInfo, 0, sizeof(IP_ADAPTER_INFO));
	
    // Make an initial call to GetAdaptersInfo to get    
    // the necessary size into the ulOutBufLen variable    
    if (GetAdaptersInfo(pAdapterInfo, &ulOutBufLen) == ERROR_BUFFER_OVERFLOW) 
	{   
        FREE(pAdapterInfo);   
        pAdapterInfo = (IP_ADAPTER_INFO *) MALLOC(ulOutBufLen);   
        if (pAdapterInfo == NULL) 
		{   
            //TRACE("Error allocating memory needed to call GetAdaptersinfo\n");   

            return 1;   
        }   
    }   
   
    if ((dwRetVal = GetAdaptersInfo(pAdapterInfo, &ulOutBufLen)) == NO_ERROR) 
	{  
		pAdapter = pAdapterInfo;   
        while (pAdapter) 
		{
			char szDes[2000] = {0};
			if(strcmp(pAdapter->Description, "") != NULL)
			{
				strncpy(szDes, pAdapter->Description, sizeof(szDes));
			}
			else
			{
				//strcpy(stNetInfo.Description, pAdapter->AdapterName);
				//描述为空，开始尝试从注册表获取  xzl 20141128
				
				if(!GetAdaptInfoFromReg(pAdapter->AdapterName, szDes))
				{
					//注册表也找不到了，没办法，只能用名字了
					strncpy(szDes, pAdapter->AdapterName, sizeof(szDes));
				}
			}
			snprintf(stNetInfo.Description, sizeof(stNetInfo.Description), "%s:%s",
					szDes, pAdapter->IpAddressList.IpAddress.String);
			
			strncpy(stNetInfo.szIp, pAdapter->IpAddressList.IpAddress.String, MAX_IP_STR_LEN);
			stNetInfo.Index = pAdapter->Index;
			stNetInfo.Type  = pAdapter->Type;
			if (listAdaptInfo)
				listAdaptInfo->push_back(stNetInfo);

			m_listAdapt.push_back(stNetInfo);

			pAdapter = pAdapter->Next; 
		}
    } 
	else 
	{ 
//		TRACE("GetAdaptersInfo failed with error: %d\n", dwRetVal);
    }   

    if (pAdapterInfo)   
        FREE(pAdapterInfo);   
   
	return 0;	
}

BOOL SystemInfo::GetAdaptInfoFromReg(char* name, char* Des)
{
	HKEY m_key,m_key2;
	LPTSTR m_SubKey = "SYSTEM\\CurrentControlSet\\Control\\Class\\{4D36E972-E325-11CE-BFC1-08002bE10318}\\"; 
    TCHAR m_name[200]={0},m_childName[200]={0},m_KeyName[200]={0};
    DWORD m_namelen=200;
    DWORD mIndex=0;
    DWORD m_KeyNamelen=200;
	DWORD m_keyType = 0;
	DWORD dwDataLen = 4;
    DWORD m_attr=REG_BINARY|REG_DWORD|REG_EXPAND_SZ|REG_MULTI_SZ|REG_NONE|REG_SZ;


	long ret = RegOpenKeyEx(HKEY_LOCAL_MACHINE,m_SubKey,0,KEY_ALL_ACCESS,&m_key);
    if(ret==ERROR_SUCCESS)
    {
        while(RegEnumKeyEx(m_key,mIndex,m_name,&m_namelen,0,NULL,NULL,0)!=ERROR_NO_MORE_ITEMS)//循环打开目录下的所有目录
        {
            mIndex++;
			if (strcmp(m_name, "") != NULL)
			{
				strcpy(m_childName,m_SubKey);
				strcat(m_childName,m_name);
				
				if(RegOpenKeyEx(HKEY_LOCAL_MACHINE,m_childName,0,KEY_ALL_ACCESS,&m_key2)==ERROR_SUCCESS)
				{
					dwDataLen = 4;
					m_keyType = 0;
					ret = RegQueryValueEx(m_key2,"Characteristics",0,&m_attr,(BYTE *)&m_keyType,&dwDataLen);
					if (((m_keyType & 0x04) == 0x04) || (m_keyType == 0x01))//0x01 NCF_VIRTUAL 说明是个虚拟适配器 ox04 NCF_PHYSICAL 说明是一个物理适配器
					{
						m_attr=REG_BINARY|REG_DWORD|REG_EXPAND_SZ|REG_MULTI_SZ|REG_NONE|REG_SZ;
						m_KeyNamelen=200;
						RegQueryValueEx(m_key2,"NetCfgInstanceId",0,&m_attr,LPBYTE(m_KeyName),&m_KeyNamelen);//取网卡名称
						if(strcmp(name, m_KeyName) == NULL)   //如果网卡名相同，则查找网卡描述
						{
							memset(m_KeyName,0,200);
							m_attr=REG_BINARY|REG_DWORD|REG_EXPAND_SZ|REG_MULTI_SZ|REG_NONE|REG_SZ;
							m_KeyNamelen=200;
							RegQueryValueEx(m_key2,"DriverDesc",0,&m_attr,LPBYTE(m_KeyName),&m_KeyNamelen);//取网卡描述
							sprintf(Des,"%s",m_KeyName);
							return TRUE;
						}
					}
				}
				RegCloseKey(m_key2);
				m_KeyNamelen = 200;
				memset(m_KeyName,0,200);
			}
			m_namelen=200;
			memset(m_name,0,200);
        }
    }
    RegCloseKey(m_key);
	return FALSE;
}


/**
 * 以下是获取当前进程CPU占用率
 *
 */
typedef S64 int64_t; 
typedef U64 uint64_t;
#include <psapi.h>

/// 时间转换
static uint64_t file_time_2_utc(const FILETIME* ftime)
{
	LARGE_INTEGER li;

	assert(ftime);
	li.LowPart = ftime->dwLowDateTime;
	li.HighPart = ftime->dwHighDateTime;
	return li.QuadPart;
}


/// 获得CPU的核数
static int get_processor_number()
{
	SYSTEM_INFO info;
	GetSystemInfo(&info);
	return (int)info.dwNumberOfProcessors;
}


int get_cpu_usage()
{
	//cpu数量
	static int processor_count_ = -1;
	//上一次的时间
	static int64_t last_time_ = 0;
	static int64_t last_system_time_ = 0;


	FILETIME now;
	FILETIME creation_time;
	FILETIME exit_time;
	FILETIME kernel_time;
	FILETIME user_time;
	int64_t system_time;
	int64_t time;
	int64_t system_time_delta;
	int64_t time_delta;

	int cpu = -1;


	if(processor_count_ == -1)
	{
		processor_count_ = get_processor_number();
	}

	GetSystemTimeAsFileTime(&now);

	if (!GetProcessTimes(GetCurrentProcess(), &creation_time, &exit_time,
		&kernel_time, &user_time))
	{
		// We don't assert here because in some cases (such as in the Task Manager)
			// we may call this function on a process that has just exited but we have
			// not yet received the notification.
			return -1;
	}
	system_time = (file_time_2_utc(&kernel_time) + file_time_2_utc(&user_time))

		/
		processor_count_;
	time = file_time_2_utc(&now);

	if ((last_system_time_ == 0) || (last_time_ == 0))
	{
		// First call, just set the last values.
		last_system_time_ = system_time;
		last_time_ = time;
		return -1;
	}

	system_time_delta = system_time - last_system_time_;
	time_delta = time - last_time_;

	assert(time_delta != 0);

	if (time_delta == 0)
		return -1;

	// We add time_delta / 2 so the result is rounded.
	cpu = (int)((system_time_delta * 100 + time_delta / 2) / time_delta);
	last_system_time_ = system_time;
	last_time_ = time;
	return cpu;
}

/**
 * 以下是获取系统总CPU占用率
 *
 */

CRITICAL_SECTION    PerfDataCriticalSection;
CpuData             *pPerfDataOld = NULL; /* Older perf data (saved to establish delta values) */
CpuData             *pPerfData = NULL;    /* Most recent copy of perf data */
ULONG               ProcessCountOld = 0;
ULONG               ProcessCount = 0;
SYSTEM_BASIC_INFORMATION        SystemBasicInfo;
SYSTEM_PERFORMANCE_INFORMATION    SystemPerfInfo;
PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION SystemProcessorTimeInfo = NULL;
LARGE_INTEGER                    liOldIdleTime = {{0,0}};
double                            dbIdleTime;
double                            dbKernelTime;
double                            dbSystemTime;
double                            OldKernelTime = 0;
LARGE_INTEGER                    liOldSystemTime = {{0,0}};
long ( __stdcall *NtQuerySystemInformation )( DWORD, PVOID, DWORD, DWORD* );


BOOL SystemInfo::PerfDataInitialize(void)
{
	SID_IDENTIFIER_AUTHORITY NtSidAuthority = {SECURITY_NT_AUTHORITY};
	NTSTATUS    status;

	InitializeCriticalSection(&PerfDataCriticalSection);
	NtQuerySystemInformation = (long(__stdcall*)(DWORD,PVOID,DWORD,DWORD*))
		GetProcAddress(
				GetModuleHandle( "ntdll.dll" ),
				"NtQuerySystemInformation"
				);

	/*
	 * Get number of processors in the system
	 */
	status = NtQuerySystemInformation(0, &SystemBasicInfo, sizeof(SystemBasicInfo), NULL);
	if (status != NO_ERROR)
		return FALSE;

	/*
	 * Create the SYSTEM Sid
	 */
	return TRUE;
}

void PerfDataUninitialize(void)
{
	DeleteCriticalSection(&PerfDataCriticalSection);
}


void GetAllProcCPUUsage()
{
	ULONG                             ulSize;
	LONG                          status;
	LPBYTE                            pBuffer;
	ULONG                             BufferSize;
	PSYSTEM_PROCESS_INFORMATION             pSPI;
	pCpuData                          pPDOld;
	ULONG                             Idx, Idx2;
	HANDLE                            hProcess;
	HANDLE                            hProcessToken;
	double                            CurrentKernelTime;
	SYSTEM_PERFORMANCE_INFORMATION    SysPerfInfo;
	SYSTEM_TIMEOFDAY_INFORMATION      SysTimeInfo;
	PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION SysProcessorTimeInfo;
	ULONG                             Buffer[64]; /* must be 4 bytes aligned! */



	/* Get new system time */
	status = NtQuerySystemInformation(3, &SysTimeInfo, sizeof(SysTimeInfo), 0);
	if (status != NO_ERROR)
		return;

	/* Get new CPU's idle time */
	status = NtQuerySystemInformation(2, &SysPerfInfo, sizeof(SysPerfInfo), NULL);
	if (status != NO_ERROR)
		return;

	/* Get processor time information */
	SysProcessorTimeInfo = (PSYSTEM_PROCESSOR_PERFORMANCE_INFORMATION)
		HeapAlloc(GetProcessHeap(), 
		0, 
		sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * SystemBasicInfo.NumberOfProcessors);

	status = NtQuerySystemInformation(
		8, 
		SysProcessorTimeInfo, 
		sizeof(SYSTEM_PROCESSOR_PERFORMANCE_INFORMATION) * SystemBasicInfo.NumberOfProcessors, 
		&ulSize);
	if (status != NO_ERROR)
		return;

	/* Get process information
	 * We don't know how much data there is so just keep
	 * increasing the buffer size until the call succeeds
	 */
	BufferSize = 0;
	do
	{
		BufferSize += 0x10000;
		pBuffer = (LPBYTE)HeapAlloc(GetProcessHeap(), 0, BufferSize);

		status = NtQuerySystemInformation(5, pBuffer, BufferSize, &ulSize);

		if (status == STATUS_INFO_LENGTH_MISMATCH) {
			HeapFree(GetProcessHeap(), 0, pBuffer);
		}

	} while (status == STATUS_INFO_LENGTH_MISMATCH);

	EnterCriticalSection(&PerfDataCriticalSection);

	/*
	 * Save system performance info
	 */
	memcpy(&SystemPerfInfo, &SysPerfInfo, sizeof(SYSTEM_PERFORMANCE_INFORMATION));

	/*
	 * Save system processor time info
	 */
	if (SystemProcessorTimeInfo)
	{
		HeapFree(GetProcessHeap(), 0, SystemProcessorTimeInfo);
	}
	SystemProcessorTimeInfo = SysProcessorTimeInfo;

	/*
	 * Save system handle info
	 */

	for (CurrentKernelTime=0, Idx=0; Idx<(ULONG)SystemBasicInfo.NumberOfProcessors; Idx++) {
		CurrentKernelTime += Li2Double(SystemProcessorTimeInfo[Idx].KernelTime);
		CurrentKernelTime += Li2Double(SystemProcessorTimeInfo[Idx].DpcTime);
		CurrentKernelTime += Li2Double(SystemProcessorTimeInfo[Idx].InterruptTime);
	}

	/* If it's a first call - skip idle time calcs */
	if (liOldIdleTime.QuadPart != 0) {
		/*  CurrentValue = NewValue - OldValue */
		dbIdleTime = Li2Double(SysPerfInfo.IdleProcessTime) - Li2Double(liOldIdleTime);
		dbKernelTime = CurrentKernelTime - OldKernelTime;
		dbSystemTime = Li2Double(SysTimeInfo.CurrentTime) - Li2Double(liOldSystemTime);

		/*  CurrentCpuIdle = IdleTime / SystemTime */
		dbIdleTime = dbIdleTime / dbSystemTime;
		dbKernelTime = dbKernelTime / dbSystemTime;

		/*  CurrentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors */
		dbIdleTime = 100.0 - dbIdleTime * 100.0 / (double)SystemBasicInfo.NumberOfProcessors; /* + 0.5; */
		dbKernelTime = 100.0 - dbKernelTime * 100.0 / (double)SystemBasicInfo.NumberOfProcessors; /* + 0.5; */
	}

	/* Store new CPU's idle and system time */
	liOldIdleTime = SysPerfInfo.IdleProcessTime;
	liOldSystemTime = SysTimeInfo.CurrentTime;
	OldKernelTime = CurrentKernelTime;

	/* Determine the process count
	 * We loop through the data we got from NtQuerySystemInformation
	 * and count how many structures there are (until RelativeOffset is 0)
	 */
	ProcessCountOld = ProcessCount;
	ProcessCount = 0;
	pSPI = (PSYSTEM_PROCESS_INFORMATION)pBuffer;
	while (pSPI) 
	{
		ProcessCount++;
		if (pSPI->NextEntryOffset == 0)
			break;
		pSPI = (PSYSTEM_PROCESS_INFORMATION)((LPBYTE)pSPI + pSPI->NextEntryOffset);
	}

	/* Now alloc a new PERFDATA array and fill in the data */
	if (pPerfDataOld) {
		HeapFree(GetProcessHeap(), 0, pPerfDataOld);
	}
	pPerfDataOld = pPerfData;
	pPerfData = (pCpuData)HeapAlloc(GetProcessHeap(), 0, sizeof(CpuData) * ProcessCount);
	pSPI = (PSYSTEM_PROCESS_INFORMATION)pBuffer;
	for (Idx=0; Idx<ProcessCount; Idx++) 
	{
		/* Get the old perf data for this process (if any) */
		/* so that we can establish delta values */
		pPDOld = NULL;
		for (Idx2=0; Idx2<ProcessCountOld; Idx2++) 
		{
			if (pPerfDataOld[Idx2].dwPID == pSPI->UniqueProcessId) 
			{
				pPDOld = &pPerfDataOld[Idx2];
				break;
			}
		}

		/* Clear out process perf data structure */
		memset(&pPerfData[Idx], 0, sizeof(CpuData));

		pPerfData[Idx].dwPID = pSPI->UniqueProcessId;

		if (pPDOld)   
		{
			double    CurTime = Li2Double(pSPI->KernelTime) + Li2Double(pSPI->UserTime);
			double    OldTime = Li2Double(pPDOld->KernelTime) + Li2Double(pPDOld->UserTime);
			double    CpuTime = (CurTime - OldTime) / dbSystemTime;
			CpuTime = CpuTime * 100.0 / (double)SystemBasicInfo.NumberOfProcessors; /* + 0.5; */
			pPerfData[Idx].cpuusage = (ULONG)CpuTime;
		}
		pPerfData[Idx].cputime.QuadPart = pSPI->UserTime.QuadPart + pSPI->KernelTime.QuadPart;

		if (pSPI->UniqueProcessId != NULL)
		{
			hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | READ_CONTROL, FALSE, PtrToUlong(pSPI->UniqueProcessId));
			if (hProcess)
			{
				/* don't query the information of the system process. It's possible but
				   returns Administrators as the owner of the process instead of SYSTEM */
				if (pSPI->UniqueProcessId != 0x4)
				{
					if (OpenProcessToken(hProcess, TOKEN_QUERY, &hProcessToken))
					{
						DWORD RetLen = 0;
						BOOL Ret;

						Ret = GetTokenInformation(hProcessToken, TokenUser, (LPVOID)Buffer, sizeof(Buffer), &RetLen);
						CloseHandle(hProcessToken);
					}

				}

				CloseHandle(hProcess);
			}
		}
		pPerfData[Idx].UserTime.QuadPart = pSPI->UserTime.QuadPart;
		pPerfData[Idx].KernelTime.QuadPart = pSPI->KernelTime.QuadPart;
		pSPI = (PSYSTEM_PROCESS_INFORMATION)((LPBYTE)pSPI + pSPI->NextEntryOffset);
	}
	HeapFree(GetProcessHeap(), 0, pBuffer);
	LeaveCriticalSection(&PerfDataCriticalSection);
}

int PerfGetIndexByProcessId(DWORD dwProcessId)
{
	int Index, FoundIndex = -1;

	EnterCriticalSection(&PerfDataCriticalSection);

	for (Index = 0; Index < (int)ProcessCount; Index++)
	{
		if ((DWORD)pPerfData[Index].dwPID == dwProcessId)
		{
			FoundIndex = Index;
			break;
		}
	}

	LeaveCriticalSection(&PerfDataCriticalSection);

	return FoundIndex;
}



ULONG PerfDataGetCPUUsage(DWORD dwProcessId)
{
	ULONG    CpuUsage;
	int Index, FoundIndex = -1;

	EnterCriticalSection(&PerfDataCriticalSection);


	for (Index = 0; Index < (int)ProcessCount; Index++)
	{
		if ((DWORD)pPerfData[Index].dwPID == dwProcessId)
		{
			FoundIndex = Index;
			break;
		}
	}

	if (Index < (int)ProcessCount)
		CpuUsage = pPerfData[Index].cpuusage;
	else
		CpuUsage = 0;

	LeaveCriticalSection(&PerfDataCriticalSection);

	return CpuUsage;
}

int SystemInfo::GetCpuUsage(DWORD *system, DWORD *process)
{
	ASSERT(system);
	ASSERT(process);

	GetAllProcCPUUsage();
	int sys_cpu = 100 - PerfDataGetCPUUsage(0);
	
	if (sys_cpu < 0)
		sys_cpu = 0;
	*system = (float)sys_cpu;

	int prog_cpu = get_cpu_usage();

	if (prog_cpu < 0)
		prog_cpu = 0;
	*process = (float)prog_cpu;

	return 0;
}


#define DIV       (1024)
int SystemInfo::GetMemeryUsage(
				   DWORD *dwTotalPhysical, 
				   DWORD *dwLeftPhysical,
				   DWORD *dwTotalVirual,
				   DWORD *dwLeftVirual
		)
{
	MEMORYSTATUSEX MemStatEx;
	MemStatEx.dwLength = sizeof(MEMORYSTATUSEX);

	if (GlobalMemoryStatusEx(&MemStatEx))
	{
		*dwTotalPhysical = MemStatEx.ullTotalPhys/DIV;
		*dwLeftPhysical  = MemStatEx.ullAvailPhys/DIV;
		*dwTotalVirual   = MemStatEx.ullTotalVirtual/DIV;
		*dwLeftVirual    = MemStatEx.ullAvailVirtual/DIV;
			
		return 0;
	}

		
	return -1;
}

/**
 *  @dwIndex
 *  @dwTotalSpeed 该网卡总带宽，单位为Mbit
 *  @dwUp         当前上行速度，单位为Kbit
 *  @dwDown       当前下行速度，单位为Kbit
 *  @return       成功反回0，否则-1
 */
int SystemInfo::GetAdaptSpeed(DWORD dwIndex, DWORD *dwTotalSpeed, DWORD *dwUp, DWORD *dwDown)
{
	if (!dwTotalSpeed || !dwUp || !dwDown)
	{
		return -1;
	}

	PMIB_IFTABLE m_pTable = NULL; 
	DWORD m_dwAdapters    = 0; 
	ULONG uRetCode = GetIfTable(m_pTable, &m_dwAdapters, TRUE); 
	if (uRetCode == ERROR_NOT_SUPPORTED) 
	{ 
		return -1; 
	} 
	
	if (uRetCode == ERROR_INSUFFICIENT_BUFFER) 
	{ 
		// MIB_IFTABLE[m_dwAdapters];
		m_pTable = (PMIB_IFTABLE)new BYTE[65535]; 
	} 

	time_t now    = time(NULL);
	time_t elapse = (now - m_tLastTime);
	if (elapse <= 0)
	{
		if (m_pTable)
			delete []m_pTable; 
		return -1;
	}
	
	GetIfTable(m_pTable,  &m_dwAdapters, TRUE); 
	DWORD dwInOctets  = 0; 
	DWORD dwOutOctets = 0; 
	for   (UINT i = 0; i < m_pTable->dwNumEntries; i++) 
	{ 	
		MIB_IFROW Row = m_pTable->table[i];
		if (dwIndex == Row.dwIndex)
		{
			*dwTotalSpeed  = Row.dwSpeed/1000/1000;
			*dwDown        = ((Row.dwInOctets - m_dwLastInOctets) << 3)/1024/elapse;
			*dwUp          = ((Row.dwOutOctets - m_dwLastOutOctets) << 3)/1024/elapse;

			m_dwLastInOctets = Row.dwInOctets;
			m_dwLastOutOctets = Row.dwOutOctets;
		}
		
		dwInOctets  += Row.dwInOctets; 
		dwOutOctets += Row.dwOutOctets; 
	} 
	m_tLastTime = time(NULL);
	
	delete []m_pTable; 
	
	return 0;
}