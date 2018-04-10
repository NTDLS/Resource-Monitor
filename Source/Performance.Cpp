#ifndef _Performance_CPP
#define _Performance_CPP
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Windows.H>
#include <WindowsX.H>
#include <ShellAPI.H>
#include <Stdio.H>
#include <Stdlib.H>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Entry.H"
#include "Performance.H"

#include "../Dialogs/MainDlg.H"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace NSWFL::Registry;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

_SystemPerformance SP;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

bool StartSystemPerformanceThread(void)
{
	memset(&SP, 0, sizeof(SP));

	InitializeCriticalSection(&SP.CS);
	EnterCriticalSection(&SP.CS);

	SP.pMemSamples = new CStack;
	SP.pCPUSamples = new CStack;
	LeaveCriticalSection(&SP.CS);

	SP.ZeroIcon = (int) MAKEINTRESOURCE(IDI_PERF0);
	SP.MaxSamples = 600;

	if((SP.hEvent = CreateEvent(NULL, false, false, "PerMonPerfThread_01584")))
	{
		if(CreateThread(NULL, NULL, SystemPerformance_Thread, NULL, 0, NULL))
		{
			return (WaitForSingleObject(SP.hEvent, 5000) == 0);
		}
	}

	return false;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

DWORD WINAPI SystemPerformance_Thread(LPVOID pVoid)
{
	PROCNTQSI NtQuerySystemInformation;

    SYSTEM_PERFORMANCE_INFORMATION SysPerfInfo;
    SYSTEM_TIME_INFORMATION SysTimeInfo;
    SYSTEM_BASIC_INFORMATION SysBaseInfo;

    double dbIdleTime = 0;
    double dbSystemTime = 0;

    LONG Status = 0;

    LARGE_INTEGER liOldIdleTime   = {0, 0};
    LARGE_INTEGER liOldSystemTime = {0, 0};

    if(!(NtQuerySystemInformation = (PROCNTQSI) GetProcAddress( GetModuleHandle("ntdll"), "NtQuerySystemInformation")))
	{
		return 0;
	}

    // Get Number Of Processors In The System
    if((Status = NtQuerySystemInformation(SystemBasicInformation, &SysBaseInfo, sizeof(SysBaseInfo), NULL)) != NO_ERROR)
	{
		return 0;
	}

	DWORD dwCPUSpeed = 0;
	if(!Get_DWORDRegistryValue(HKEY_LOCAL_MACHINE, "HARDWARE\\DESCRIPTION\\System\\CentralProcessor\\0", "~MHz", dwCPUSpeed))
	{
		dwCPUSpeed = 0;
	}
	float fCPUSpeed = (float)dwCPUSpeed;

	if(fCPUSpeed > 1000)
	{
		sprintf_s(SP.SpeedString, sizeof(SP.SpeedString), "%.1f0 GHz", fCPUSpeed / 1000.0f);
	}
	else {
		if(fCPUSpeed > 0)
		{
			sprintf_s(SP.SpeedString, sizeof(SP.SpeedString), "%.2f MHz", fCPUSpeed);
		}
		else{
			strcpy_s(SP.SpeedString, sizeof(SP.SpeedString), "");
		}
	}
	
    MEMORYSTATUSEX MS;

    SP.NumberOfProcessors = SysBaseInfo.bKeNumberProcessors;

	while(true)
    {
		EnterCriticalSection(&SP.CS);

		memset(&MS, 0, sizeof(MS));
		MS.dwLength = sizeof(MEMORYSTATUSEX);
		GlobalMemoryStatusEx(&MS);

		SP.UsedMemory        = (double) MS.ullTotalPhys - MS.ullAvailPhys;
        SP.FreeMemory        = (double) MS.ullAvailPhys;
        SP.TotalMemory       = (double) MS.ullTotalPhys;

        SP.PercentMemoryUsed = (SP.UsedMemory / SP.TotalMemory) * 100.0;
        SP.PercentMemoryFree = (SP.FreeMemory / SP.TotalMemory) * 100.0;

        // Get New System Time
        if((Status = NtQuerySystemInformation(SystemTimeInformation, &SysTimeInfo, sizeof(SysTimeInfo), 0)) != NO_ERROR)
		{
			return 0;
		}

        // Get New CPU's Idle Time
        if((Status = NtQuerySystemInformation(SystemPerformanceInformation, &SysPerfInfo, sizeof(SysPerfInfo), NULL)) != NO_ERROR)
		{
			return 0;
		}

		if(liOldIdleTime.QuadPart != 0)
		{
			// CurrentValue = NewValue - OldValue
			dbIdleTime   = Li2Double(SysPerfInfo.liIdleTime)     - Li2Double(liOldIdleTime);
			dbSystemTime = Li2Double(SysTimeInfo.liKeSystemTime) - Li2Double(liOldSystemTime);

			// CurrentCpuIdle = IdleTime / SystemTime
			dbIdleTime = dbIdleTime / dbSystemTime;

			// CurrentCpuUsage% = 100 - (CurrentCpuIdle * 100) / NumberOfProcessors
			if((dbIdleTime = (100.0 - (((dbIdleTime * 100.0) / (double)SysBaseInfo.bKeNumberProcessors)) + 0.5)) < 0)
			{
				dbIdleTime = 0;
			}

			SP.PercentProcessorUsed = (float) dbIdleTime - 0.5f;
			SP.PercentProcessorFree = (float) (100.0 - dbIdleTime) + 0.5f;
		}

		// store new CPU's idle and system time
		liOldIdleTime   = SysPerfInfo.liIdleTime;
		liOldSystemTime = SysTimeInfo.liKeSystemTime;

		UpdatePerfInfo();

		if(SP.hEvent)
		{
			SetEvent(SP.hEvent);
			CloseHandle(SP.hEvent);
			SP.hEvent = NULL;
		}
		LeaveCriticalSection(&SP.CS);

		Sleep(1000);
	}

	delete SP.pMemSamples;
	delete SP.pCPUSamples;

	DeleteCriticalSection(&SP.CS);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
