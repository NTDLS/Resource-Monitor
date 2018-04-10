[Setup]
;-- Main Setup Information
 AppName                = Resource Monitor
 AppVerName             = Resource Monitor 1.0.0.2
 AppCopyright           = Copyright © 1995-2009 NetworkDLS.
 DefaultDirName         = {pf}\NetworkDLS\Resource Monitor
 DefaultGroupName       = NetworkDLS\Resource Monitor
 UninstallDisplayIcon   = {app}\ResMon.Exe
 WizardImageFile        = \..\..\@Resources\Setup\LgSetup.bmp
 WizardSmallImageFile   = \..\..\@Resources\Setup\SmSetup.bmp
 PrivilegesRequired     = PowerUser
 Uninstallable          = Yes
 AppMutex               = Resource Monitor
 LicenseFile            = \..\..\@Resources\Setup\EULA.txt
 Compression            = ZIP/9
 OutputBaseFilename     = Setup
 MinVersion             = 0.0,5.0

;-- Windows 2000 & XP (Support Dialog)
 AppPublisher    = NetworkDLS
 AppPublisherURL = http://www.NetworkDLS.com/
 AppUpdatesURL   = http://www.NetworkDLS.com/
 AppVersion      = 1.0.0.2

[Tasks]
 Name: RunAtStart;        Description: "Run at &startup?"; GroupDescription: "Additional icons:"; Flags: unchecked
 Name: RunAtStart\common; Description: "For all &users";   GroupDescription: "Additional icons:"; Flags: unchecked exclusive
 Name: RunAtStart\user;   Description: "For me &only";     GroupDescription: "Additional icons:"; Flags: unchecked exclusive

[Files]
 Source: "..\Release\ResMon.Exe"; DestDir: "{app}";

[Icons]
 Name: "{group}\Resource Monitor";         Filename: "{app}\ResMon.Exe";
 Name: "{commonstartup}\Resource Monitor"; Filename: "{app}\ResMon.Exe"; Tasks: RunAtStart\common
 Name: "{userstartup}\Resource Monitor";   Filename: "{app}\ResMon.Exe"; Tasks: RunAtStart\User

[Registry]
 Root: HKLM; Subkey: "Software\NetworkDLS\Resource Monitor"; Flags: uninsdeletekey noerror;

[Run]
 Filename: "{app}\ResMon.Exe"; Description: "Launch application"; Flags: postinstall nowait skipifsilent unchecked

