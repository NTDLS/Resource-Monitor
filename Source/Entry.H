#ifndef _ENTRY_H
#define _ENTRY_H
///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#include "../../@Libraries/Debug/Debug.H"
#include "../../@Libraries/NSWFL/NSWFL.H"

#include "../../@Libraries/CMemPool/CMemPool.H"
#include "../../@Libraries/CCmdLine/CCmdLine.H"
#include "../../@Libraries/CTrayIcon/CTrayIcon.H"

#include "../Resources/Resource.H"

#include "Routines.H"

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

extern HINSTANCE ghAppInstance;

extern char gsAppCopyRight[64];
extern char gsFileVersion[10];
extern char gsTitleCaption[64];
extern char gsRegistryKey[255];

extern CMemPool *pMem;
extern CTrayIcon gTrayIcon;

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
#endif

