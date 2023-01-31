//*****************************************************************
// il2fb.exe - SAS IL-2 Executable Selector
// Copyright (C) 2021 SAS~Storebror
//
// This file is part of il2fb.exe.
//
// il2fb.exe is free software.
// It is distributed under the DWTFYWTWIPL license:
//
// DO WHAT THE FUCK YOU WANT TO WITH IT PUBLIC LICENSE
// Version 1, March 2012
//
// Copyright (C) 2013 SAS~Storebror <mike@sas1946.com>
//
// Everyone is permitted to copy and distribute verbatim or modified
// copies of this license document, and changing it is allowed as long
// as the name is changed.
//
// DO WHAT THE FUCK YOU WANT TO WITH IT PUBLIC LICENSE
// TERMS AND CONDITIONS FOR COPYING, DISTRIBUTION AND MODIFICATION
//
// 0. You just DO WHAT THE FUCK YOU WANT TO WITH IT.
//
//*****************************************************************

#pragma once
#define SELECTOR_MUTEX				L"SAS IL-2 EXECUTABLE SELECTOR MUTEX"
#define WM_IL2_STARTED				WM_USER+101
#define WM_IL2_STOPPED				WM_USER+102
#define WM_IL2_LOADED				WM_USER+103
#define WM_IL2_START_ERROR			WM_USER+104
#define WM_IL2_INSUFFICIENT_RAM		WM_USER+105
#define OPERATION_MODE_START		0
#define OPERATION_MODE_SETTINGS		1
#define TIMER_REENABLE_START		2
#define FILE_OPERATION_RETRIES		50

#define RAM_MESSAGE_GOOD			0
#define RAM_MESSAGE_CRITICAL		1
#define RAM_MESSAGE_ERROR			2

#define MEM_STRATEGY_BALANCED		0
#define MEM_STRATEGY_CONSERVATIVE	1
#define MEM_STRATEGY_HEAPONLY		2

#define SPLASH_SCREEN_VISIBLE		1
#define SPLASH_SCREEN_TOPMOST		2

#define DUMP_MODE_SFS_ACCESS		1
#define DUMP_MODE_DUMP_FILES		2

#define IL2_WINDOW_CAPTION			L"Il2-Sturmovik Forgotten Battles"
#define IL2_WINDOW_CLASS			L"MaddoxRtsWndClassW"

#define IL2_EXE_NAME				L"il2fb.exe"
#define WRAPPER_DLL_NAME			L"wrapper.dll"
#define DINPUT_DLL_NAME				L"DINPUT.dll"
#define IL2_BASEFILES				L"basefiles\\"
#define IL2FB_EXE_MOD_NAME			L"mod\\il2fb.exe"
#define IL2FB_EXE_STOCK_NAME		L"stock\\il2fb.exe"
#define WRAPPER_DLL_MOD_NAME		L"mod\\wrapper.dll"
#define WRAPPER_DLL_STOCK_NAME		L""
#define DINPUT_BASE_NAME			L"basefiles\\DINPUT.dll"
#define HYPERLOBBY_INI				L"HyperLobby\\hyperlobby.ini"
#define EXPERT_KEY					L"40930"
#define CACHED_WRAPPER_KEY			L"89857"
#define EDIT_KEY_RESET				L"Auth. Code"

#define lengthof(a) (sizeof a / sizeof a[0])
#define RunWorker(ThreadStartRoutine) 	CloseHandle(CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadStartRoutine,NULL,0,NULL))

#ifndef _COMMON_H_
#define _COMMON_H_

#ifndef _QWORD_DEFINED
#define _QWORD_DEFINED
typedef __int64 QWORD, *LPQWORD;
#endif

#define MAKEQWORD(a, b)	\
	((QWORD)( ((QWORD) ((DWORD) (a))) << 32 | ((DWORD) (b))))

#define LODWORD(l) \
	((DWORD)(l))
#define HIDWORD(l) \
	((DWORD)(((QWORD)(l) >> 32) & 0xFFFFFFFF))

// Read 4K of data at a time (used in the C++ streams, Win32 I/O, and assembly functions)
#define MAX_BUFFER_SIZE	4096

// Map a "view" size of 10MB (used in the filemap function)
#define MAX_VIEW_SIZE	10485760

#endif

typedef struct STATUSITEM {
    BOOL		bEnabled;
    COLORREF	crForeground;
    COLORREF	crBackground;
    int			iBackgroundMode;
    LPTSTR		lpItemText;
} STATUSITEM, *LPSTATUSITEM;

//*************************************************************************
// Function Prototypes
//*************************************************************************
void FillDropdown();
void LaunchIl2();
void ShowLastError();
int stepRamSize(int baseRamSize);
void GetFilesAndPaths();
void ReadIniSettings();
void SettingsToControls();
void ControlsToSettings();
BOOL WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int nInteger, LPCTSTR lpFileName);
void WriteIniSettings();
void SetRamSliderTicks();
BOOL IsKeyDown(int vKey);
void CheckCreateFile(LPCTSTR lpCheckFile, LPCTSTR lpBaseFile);
void CreateIl2FbExe();
void EnableSettingChanges(BOOL bEnable);
BOOL SASES_OnInitDialog(HWND hwnd, HWND hwndFocus, LPARAM lParam);
bool SASES_OnCreate(HWND hwndDlg, LPCREATESTRUCT lpCS);
void SASES_OnDestroy(HWND hwndDlg);
void SASES_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify);
void SASES_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos);
HBRUSH SASES_OnCtlColorEdit(HWND hwnd, HDC hdc, HWND hwndChild, int type);
void SASES_OnTimer(HWND hwnd, UINT_PTR nIDEvent);
void SASES_OnDrawItem(HWND hwndDlg, const DRAWITEMSTRUCT* lpDIS);
void SASES_OnIl2Started();
void SASES_OnIl2Stopped();
void SASES_OnIl2Loaded();
void SASES_OnIl2StartError();
void SASES_OnIl2InsufficientRam(int iMessageType);
void ShowCurrentSettings();
void AfterIl2Stopped();
void BringToFront(HWND hwnd);
BOOL IsFolderWriteable();
BOOL IsFileWriteable(LPCTSTR szFilePath);
void CheckRamUsage();
void ShowModtypeHints(int iModType);
BOOL SetFileAttributesWithRetry(LPCTSTR lpFileName, DWORD dwFileAttributes, int iAttempts);
BOOL DeleteFileWithRetry(LPCTSTR lpFileName, int iAttempts);
BOOL MoveFileWithRetry(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, int iAttempts);
BOOL CopyMoveFileWithRetry(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, int iAttempts);
BOOL CopyFileWithRetry(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, int iAttempts);
BOOL SecureDeleteFile(LPCTSTR lpFileName);
BOOL SecureSetFileAttributes(LPCTSTR lpFileName, DWORD dwFileAttributes);
BOOL SecureMoveFile(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName);
BOOL SecureCopyMoveFile(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName);
BOOL SecureCopyFile(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName);
BOOL FileExists(LPCTSTR lpFileName);
void ShowAdditionalJvmParams();
void CreateStatusBar(int numParts, ...);
void EnableStatusBar(int iStatusBarIndex, BOOL bEnabled);
void SetStatusBarForegroundColor(int iStatusBarIndex, COLORREF crForegroundColor);
void SetStatusBarBackgroundColor(int iStatusBarIndex, COLORREF crBackgroundColor);
void SetStatusBarColor(int iStatusBarIndex, COLORREF crForegroundColor, COLORREF crBackgroundColor);
void SetStatusBarBackgroundMode(int iStatusBarIndex, int iBackgroundMode);
void SetStatusBarText(int iStatusBarIndex, LPCTSTR lpStatus);
void SetStatusBar(int iStatusBarIndex, BOOL bEnabled, COLORREF crForegroundColor, COLORREF crBackgroundColor, int iBackgroundMode, LPCTSTR lpStatus);
void SetRAMStatus();

INT_PTR CALLBACK SASES_DialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);

BOOL bShowExitOnIl2Quit();
BOOL bShowModFilesCache();
BOOL bShowMultipleInstances();
BOOL bShowAdjustHyperlobby();
BOOL bShowAdjustHyperlobbyOptions();
LRESULT APIENTRY EditSubclassProcExpert(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
LRESULT APIENTRY EditSubclassProcCachedWrapper(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
