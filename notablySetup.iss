; Script generated by the Inno Setup Script Wizard.
; SEE THE DOCUMENTATION FOR DETAILS ON CREATING INNO SETUP SCRIPT FILES!

#define MyAppName "Notably"
#define MyAppVerName "Notable 0.2.1"
#define MyAppPublisher "Bernd Stramm"
#define MyAppURL "http://www.bernd-stramm.com/"
#define MyAppExeName "notably.exe"

[Setup]
; NOTE: The value of AppId uniquely identifies this application.
; Do not use the same AppId value in installers for other applications.
; (To generate a new GUID, click Tools | Generate GUID inside the IDE.)
AppId={{CEC18D25-9CD3-461B-91FD-CCF3A7E6DD57}
AppName={#MyAppName}
AppVerName={#MyAppVerName}
AppPublisher={#MyAppPublisher}
AppPublisherURL={#MyAppURL}
AppSupportURL={#MyAppURL}
AppUpdatesURL={#MyAppURL}
DefaultDirName={pf}\{#MyAppName}
DefaultGroupName={#MyAppName}
LicenseFile=D:\bernd\sample_codes\notetaker\LICENSE.txt
OutputBaseFilename=setupNotably
Compression=lzma
SolidCompression=true
OutputDir=D:\bernd\sample_codes\notetaker

[Languages]
Name: english; MessagesFile: compiler:Default.isl

[Tasks]
Name: desktopicon; Description: {cm:CreateDesktopIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked
Name: quicklaunchicon; Description: {cm:CreateQuickLaunchIcon}; GroupDescription: {cm:AdditionalIcons}; Flags: unchecked

[Files]
Source: D:\bernd\sample_codes\notetaker\viewer\release\notably.exe; DestDir: {app}; Flags: ignoreversion
; NOTE: Don't use "Flags: ignoreversion" on any shared system files
Source: C:\Qt\2010.02.1\qt\bin\QtCore4.dll; DestDir: {app}
Source: C:\Qt\2010.02.1\qt\bin\QtGui4.dll; DestDir: {app}
Source: C:\Qt\2010.02.1\qt\bin\QtSql4.dll; DestDir: {app}
Source: C:\Qt\2010.02.1\qt\bin\QtWebKit4.dll; DestDir: {app}
Source: C:\Qt\2010.02.1\qt\bin\mingwm10.dll; DestDir: {app}
Source: C:\Qt\2010.02.1\qt\bin\libgcc_s_dw2-1.dll; DestDir: {app}
Source: C:\Qt\2010.02.1\qt\bin\QtXmlPatterns4.dll; DestDir: {app}
Source: C:\Qt\2010.02.1\qt\bin\QtNetwork4.dll; DestDir: {app}
Source: C:\Qt\2010.02.1\qt\bin\phonon4.dll; DestDir: {app}
Source: img\nb.ico; DestDir: {app}
Source: C:\Qt\2010.02.1\qt\plugins\sqldrivers\qsqlite4.dll; DestDir: {app}\sqldrivers

[Icons]
Name: {group}\{#MyAppName}; Filename: {app}\{#MyAppExeName}; IconFilename: {app}\nb.ico
Name: {group}\{cm:UninstallProgram,{#MyAppName}}; Filename: {uninstallexe}; IconFilename: {app}\nb.ico
Name: {commondesktop}\{#MyAppName}; Filename: {app}\{#MyAppExeName}; Tasks: desktopicon; IconFilename: {app}\nb.ico
Name: {userappdata}\Microsoft\Internet Explorer\Quick Launch\{#MyAppName}; Filename: {app}\{#MyAppExeName}; Tasks: quicklaunchicon; IconFilename: {app}\nb.ico

[Run]
Filename: {app}\{#MyAppExeName}; Description: {cm:LaunchProgram,{#MyAppName}}; Flags: nowait postinstall skipifsilent
