#ifndef _Performance_H
#define _Performance_H
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../@Libraries/CStack/CStack.H"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define SystemBasicInformation       0
#define SystemPerformanceInformation 2
#define SystemTimeInformation        3

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#define Li2Double(x) ((double)((x).HighPart) * 4.294967296E9 + (double)((x).LowPart))

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct{
    DWORD   dwUnknown1;
    ULONG   uKeMaximumIncrement;
    ULONG   uPageSize;
    ULONG   uMmNumberOfPhysicalPages;
    ULONG   uMmLowestPhysicalPage;
    ULONG   uMmHighestPhysicalPage;
    ULONG   uAllocationGranularity;
    PVOID   pLowestUserAddress;
    PVOID   pMmHighestUserAddress;
    ULONG   uKeActiveProcessors;
    BYTE    bKeNumberProcessors;
    BYTE    bUnknown2;
    WORD    wUnknown3;
} SYSTEM_BASIC_INFORMATION;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct{
    LARGE_INTEGER   liIdleTime;
    DWORD           dwSpare[76];
} SYSTEM_PERFORMANCE_INFORMATION;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct{
    LARGE_INTEGER liKeBootTime;
    LARGE_INTEGER liKeSystemTime;
    LARGE_INTEGER liExpTimeZoneBias;
    ULONG         uCurrentTimeZoneId;
    DWORD         dwReserved;
} SYSTEM_TIME_INFORMATION;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

struct _SystemPerformance{
    double TotalMemory;
    double FreeMemory;
    double UsedMemory;

    double PercentMemoryUsed;
    double PercentMemoryFree;

    unsigned int NumberOfProcessors;
    double PercentProcessorUsed;
    double PercentProcessorFree;

	char SpeedString[100];

	CRITICAL_SECTION CS;

	CStack *pMemSamples;
	CStack *pCPUSamples;

	unsigned int MaxSamples;

	int ZeroIcon;

	HANDLE hEvent;
}; extern _SystemPerformance SP;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef LONG (WINAPI *PROCNTQSI)(UINT,PVOID,ULONG,PULONG);
DWORD WINAPI SystemPerformance_Thread(LPVOID pVoid);
bool StartSystemPerformanceThread(void);

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
