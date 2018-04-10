#ifndef _ENTRY_CPP
#define _ENTRY_CPP
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include <Windows.H>
#include <Stdio.H>
#include <Stdlib.H>

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "Entry.H"
#include "Init.H"
#include "Performance.H"

#include "../Dialogs/MainDlg.H"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

HINSTANCE ghAppInstance = NULL;

char gsAppCopyRight[64];
char gsFileVersion[10];
char gsTitleCaption[64];
char gsRegistryKey[255];

CMemPool *pMem; //The global memory object.
CTrayIcon gTrayIcon;

#define ID_TRAY_DETAILS	WM_USER + 1
#define ID_TRAY_CANCEL	WM_USER + 2
#define ID_TRAY_CLOSE	WM_USER + 3

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

void ProgressTrayIconProc(CTrayIcon *lpTI, HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if(uMsg == WM_CTRAYICON_INITIALIZE)
	{
	}
	else if(uMsg == WM_COMMAND)
    {
		if(LOWORD(wParam) == ID_TRAY_CLOSE)
        {
			gTrayIcon.Destroy();
		}
		else if(LOWORD(wParam) == ID_TRAY_DETAILS)
        {
			if(hMainDialog)
			{
				SetForegroundWindow(hMainDialog);
				SetFocus(hMainDialog);
			}
			else{
				DialogBox(ghAppInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, MainDialog);
			}
        }
    }
    else if(uMsg == WM_SHELLNOTIFY)
    {
		if(lParam == WM_LBUTTONDBLCLK)
		{
			if(hMainDialog)
			{
				SetForegroundWindow(hMainDialog);
				SetFocus(hMainDialog);
			}
			else{
				DialogBox(ghAppInstance, MAKEINTRESOURCE(IDD_MAIN), NULL, MainDialog);
			}
		}
    }
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

int AppEntry(HINSTANCE hInstance, CCmdLine *lpCmdLine)
{
	HANDLE hMutex = NULL;

	if( (hMutex = OpenMutex(SYNCHRONIZE, FALSE, gsTitleCaption)) )
	{
		MessageBox(NULL, "Only one instance can run at a time!",
			gsTitleCaption, MB_ICONASTERISK | MB_SYSTEMMODAL);

		ReleaseMutex(hMutex);
		CloseHandle(hMutex);
		return 0;
	}
	else hMutex = CreateMutex(0, 0, gsTitleCaption);

	//--------------------------------------------------------------------------

	if(StartSystemPerformanceThread())
	{
		HMENU hPopupMenu = CreateMenu();
		AppendMenu(hPopupMenu, MF_STRING, ID_TRAY_DETAILS, "&Details");
		AppendMenu(hPopupMenu, MF_STRING, ID_TRAY_CANCEL, "Ca&ncel");
		AppendMenu(hPopupMenu, MF_SEPARATOR, MF_SEPARATOR, "");
		AppendMenu(hPopupMenu, MF_STRING, ID_TRAY_CLOSE, "&Close");

		gTrayIcon.Create(IDI_MAIN, gsTitleCaption, hPopupMenu, &ProgressTrayIconProc);
	}

	//--------------------------------------------------------------------------

	ReleaseMutex(hMutex);
	CloseHandle(hMutex);

	return 0;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef _DEBUG
int main(int argc, char *argv[])
#else
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevious, LPSTR CmdLine, int CmdShow)
#endif
{
    #ifdef _DEBUG
	ghAppInstance = GetModuleHandle(NULL);
    #else
	ghAppInstance = hInstance;
    #endif

	int iResult = 0;

	if(InitializeApp())
    {
		CCmdLine CmdLine;
		if(CmdLine.Initialize())
		{
			iResult = AppEntry(ghAppInstance, &CmdLine);
			CmdLine.Destroy();
		}
    }

    UninitializeApp();

	return iResult;
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif

