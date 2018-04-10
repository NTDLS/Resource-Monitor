#ifndef _MAINDLG_CPP
#define _MAINDLG_CPP
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Windows.H>
#include <CommCTRL.H>
#include <Stdio.H>
#include <Stdlib.H>

#include "../Source/Entry.H"
#include "../Source/Performance.H"

#include "../Dialogs/MainDlg.H"
#include "../Dialogs/NetLogoDlg.H"

#include "../../@Libraries/CGraph/CGraph.H"
#include "../../@Libraries/CStack/CStack.H"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

using namespace NSWFL::Windows;
using namespace NSWFL::File;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

//---------------------(Variable Declarations)
HWND hMainDialog    = NULL;

HWND hMemInstalled  = NULL;
HWND hMemUsed       = NULL;
HWND hMemUsedBar    = NULL;
HWND hMemFree       = NULL;
HWND hMemFreeBar    = NULL;

HWND hCPUInstalled  = NULL;
HWND hCPUUsage      = NULL;
HWND hCPUUsageBar   = NULL;
HWND hCPUIdle       = NULL;
HWND hCPUIdleBar    = NULL;

HWND hSysPerfBar    = NULL;
HWND hSysPerf       = NULL;

HWND hGraph         = NULL;

CGraph Graph;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void UpdatePerfInfo(void)
{
	char sFmt[255];

	if(hMainDialog)
	{
		sprintf_s(sFmt, sizeof(sFmt), "%.2f %%", SP.PercentMemoryUsed);
		Set_Text(hMemUsed, sFmt);
		SendMessage(hMemUsedBar, PBM_SETPOS, (DWORD) SP.PercentMemoryUsed, 0);
		sprintf_s(sFmt, sizeof(sFmt), "%.2f %%", SP.PercentMemoryFree);
		Set_Text(hMemFree, sFmt);
		SendMessage(hMemFreeBar, PBM_SETPOS, (DWORD) SP.PercentMemoryFree, 0);

		sprintf_s(sFmt, sizeof(sFmt), "%.2f %%", SP.PercentProcessorUsed);
		Set_Text(hCPUUsage, sFmt);
		SendMessage(hCPUUsageBar, PBM_SETPOS, (DWORD) SP.PercentProcessorUsed, 0);
		sprintf_s(sFmt, sizeof(sFmt), "%.2f %%", SP.PercentProcessorFree);
		Set_Text(hCPUIdle, sFmt);
		SendMessage(hCPUIdleBar, PBM_SETPOS, (DWORD) SP.PercentProcessorFree, 0);

		double iValues[2];
		iValues[0] = SP.PercentMemoryUsed;
		iValues[1] = SP.PercentProcessorUsed;
		Graph.Update(iValues);
		Graph.Invalidate();
	}

	SP.pMemSamples->Push((int)SP.PercentMemoryUsed);
	SP.pCPUSamples->Push((int)SP.PercentProcessorUsed);

	while(SP.pMemSamples->StackSize() > SP.MaxSamples)
	{
		SP.pMemSamples->Pop();
		SP.pCPUSamples->Pop();
	}

	double dOverall = ((SP.PercentMemoryUsed + SP.PercentProcessorUsed) / 2.0);

	int iIcon = SP.ZeroIcon + (int)dOverall;

	sprintf_s(sFmt, sizeof(sFmt),
		"Processor(%.2f) Memory(%.2f) Averall(%.2f)",
		SP.PercentProcessorUsed, SP.PercentMemoryUsed, dOverall);
	gTrayIcon.Update(iIcon, sFmt);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

BOOL CALLBACK MainDialog(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
    static DWORD BGColorRef = 0;

	static HWND hMemUsedLbl = NULL;
	static HWND hMemFreeLbl = NULL;

	static HWND hCPUUsageLbl = NULL;
	static HWND hCPUIdleLbl = NULL;

	//--------------------------------------------------------------------------

	if(uMsg == WM_INITDIALOG)
    {
        hMainDialog = hWnd;
        BGColorRef = GetSysColor(COLOR_3DFACE);

        HMENU hMenu = GetSystemMenu(hWnd, FALSE);
        AppendMenu(hMenu, MF_SEPARATOR, 0, 0);
        AppendMenu(hMenu, MF_STRING, MAINDIALOG_MENU_ABOUT, "About");

        SendMessage(hWnd, (UINT)WM_SETTEXT, (WPARAM)0, (LPARAM)gsTitleCaption);
        SendMessage(hWnd, WM_SETICON, TRUE, (LPARAM) LoadIcon(ghAppInstance, MAKEINTRESOURCE(IDI_MAIN)));

		hMemInstalled  = GetDlgItem(hWnd, IDC_MEMINSTALLED);
		hMemUsedLbl       = GetDlgItem(hWnd, IDC_MEMUSED_LBL);
		hMemUsed       = GetDlgItem(hWnd, IDC_MEMUSED);
		hMemUsedBar    = GetDlgItem(hWnd, IDC_MEMUSEDBAR);
		hMemFreeLbl       = GetDlgItem(hWnd, IDC_MEMFREE_LBL);
		hMemFree       = GetDlgItem(hWnd, IDC_MEMFREE);
		hMemFreeBar    = GetDlgItem(hWnd, IDC_MEMFREEBAR);

		hCPUInstalled  = GetDlgItem(hWnd, IDC_CPUINSTALLED);
		hCPUUsageLbl      = GetDlgItem(hWnd, IDC_CPUUSAGE_LBL);
		hCPUUsage      = GetDlgItem(hWnd, IDC_CPUUSAGE);
		hCPUUsageBar   = GetDlgItem(hWnd, IDC_CPUUSAGEBAR);
		hCPUIdleLbl       = GetDlgItem(hWnd, IDC_CPUIDLE_LBL);
		hCPUIdle       = GetDlgItem(hWnd, IDC_CPUIDLE);
		hCPUIdleBar    = GetDlgItem(hWnd, IDC_CPUIDLEBAR);

		hGraph		   = GetDlgItem(hWnd, IDC_GRAPH);

		Graph.Initialize(hGraph, RGB(210, 210, 210), RGB(150, 200, 150), 10, GRAPH_RIGHT, false);
		Graph.AddSeries(RGB(200, 0, 0), 100);
		Graph.AddSeries(RGB(0, 0, 200), 100);

		double iValues[2];

		EnterCriticalSection(&SP.CS);

		UpdatePerfInfo();

		for(unsigned int iSlot = SP.pMemSamples->StackSize() - 1; iSlot > 0; iSlot--)
		{
			int iSample;

			SP.pMemSamples->Peek(iSlot, &iSample);
			iValues[0] = iSample;

			SP.pCPUSamples->Peek(iSlot, &iSample);
			iValues[1] = iSample;

			Graph.Update(iValues);
		}
		Graph.Invalidate();
		LeaveCriticalSection(&SP.CS);

		char sFmt[64];
		char sText[1024];
		sprintf_s(sText, sizeof(sText), "Installed: %u CPUs @ %s", SP.NumberOfProcessors, SP.SpeedString);
		Set_Text(hCPUInstalled, sText);

		sprintf_s(sText, sizeof(sText), "Installed: %s",
			FileSizeFriendly(SP.TotalMemory, 1, sFmt, sizeof(sFmt)));
		Set_Text(hMemInstalled, sText);
		
		CenterWindow(hWnd);

        return TRUE; // Return TRUE to set the keyboard focus, Otherwise return FALSE.
    }

    //--------------------------------------------------------------------------

    if(uMsg == WM_CTLCOLORSTATIC)
	{
		if((HANDLE)lParam == hMemInstalled)
			return Set_Color(RGB(200, 0, 0), BGColorRef, wParam);
		else if((HANDLE)lParam == hMemUsedLbl)
			return Set_Color(RGB(200, 0, 0), BGColorRef, wParam);
		else if((HANDLE)lParam == hMemUsed)
			return Set_Color(RGB(200, 0, 0), BGColorRef, wParam);
		else if((HANDLE)lParam == hMemFreeLbl)
			return Set_Color(RGB(200, 0, 0), BGColorRef, wParam);
		else if((HANDLE)lParam == hMemFree)
			return Set_Color(RGB(200, 0, 0), BGColorRef, wParam);
		else if((HANDLE)lParam == hCPUInstalled)
			return Set_Color(RGB(0, 0, 200), BGColorRef, wParam);
		else if((HANDLE)lParam == hCPUUsageLbl)
			return Set_Color(RGB(0, 0, 200), BGColorRef, wParam);
		else if((HANDLE)lParam == hCPUUsage)
			return Set_Color(RGB(0, 0, 200), BGColorRef, wParam);
		else if((HANDLE)lParam == hCPUIdleLbl)
			return Set_Color(RGB(0, 0, 200), BGColorRef, wParam);
		else if((HANDLE)lParam == hCPUIdle)
			return Set_Color(RGB(0, 0, 200), BGColorRef, wParam);

        return FALSE;
	}

	//--------------------------------------------------------------------------

    else if(uMsg == WM_COMMAND)
    {
        if(wParam == IDC_CLOSE)
        {
			EnterCriticalSection(&SP.CS);
			hMainDialog = NULL;

			Graph.Destroy();
			hGraph = NULL;

            EndDialog(hWnd, 0);
			LeaveCriticalSection(&SP.CS);
			return TRUE;
        }
        else if(wParam == IDC_ALWAYSONTOP)
        {
            if(IsDlgButtonChecked(hWnd, IDC_ALWAYSONTOP))
			{
                SetWindowPos(hWnd, HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
			else {
				SetWindowPos(hWnd, HWND_NOTOPMOST, 0, 0, 0, 0, SWP_NOMOVE | SWP_NOSIZE);
			}
            return TRUE;
        }


        return FALSE;
    }

    //--------------------------------------------------------------------------

    else if(uMsg == WM_SYSCOMMAND) //- Received a system menu message.
    {
        if(LOWORD(wParam) == MAINDIALOG_MENU_ABOUT) //- About.
        {
            _AboutDialogInfo ADI;
    
            ADI.DisplayIcon  = LoadIcon(ghAppInstance, MAKEINTRESOURCE(IDI_MAIN));
            ADI.TitleCaption = gsTitleCaption;
            ADI.FileVersion  = gsFileVersion;
            ADI.BuildDate    = __DATE__;
            ADI.BuildTime    = __TIME__;
            ADI.CopyRight    = gsAppCopyRight;
            ADI.OwnerHandle  = hWnd;

            NetLogo(&ADI);

            return TRUE;
        }

        return FALSE;
    }

    //--------------------------------------------------------------------------

    else if(uMsg == WM_CLOSE) //- Received a close message.
    {
		EnterCriticalSection(&SP.CS);
		hMainDialog = NULL;

		Graph.Destroy();
		hGraph = NULL;
		
		EndDialog(hWnd, 0);
		LeaveCriticalSection(&SP.CS);

		return TRUE;
    }

    //--------------------------------------------------------------------------

    return FALSE;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif
