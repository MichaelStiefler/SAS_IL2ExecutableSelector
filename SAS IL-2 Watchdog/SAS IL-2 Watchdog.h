//*****************************************************************
// IL-2 Watchdog.exe - il2fb.exe Watchdog
// Copyright (C) 2021 SAS~Storebror
//
// This file is part of IL-2 Watchdog.exe.
//
// IL-2 Watchdog.exe is free software.
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

#include <tchar.h>
#define SIGNATURE { 0xde63ee24, 0xa8fa, 0x40bb, { 0x95, 0x99, 0x1d, 0xbf, 0xce, 0x1a, 0xcb, 0xef } };
#define IL2FBEXE_WAIT_TIMEOUT				5000
#define WATCHDOG_TIMER_EVENT				1
#define WATCHDOG_TIMER_CHECKACTIVE_EVENT	2
#define WATCHDOG_TIMER_CHECKIL2WINDOW_EVENT	3
#define WATCHDOG_TIMER_TIMEOUT				1000
#define IL2_MAIN_WINDOW_CLASS				L"MaddoxRtsWndClassW"
#define WATCHDOG_DEFAULT_SPLASH_SCREEN_MODE	3


#pragma once
//*************************************************************************
// Globals
//*************************************************************************

// {DE63EE24-A8FA-40bb-9599-1DBFCE1ACBEF}
static const GUID Il2WatchdogGUID = SIGNATURE;


//*************************************************************************
// Forward Declarations
//*************************************************************************
void ShowLastError();
static LRESULT MsgWndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
void CheckIl2Termination();
void CALLBACK WinEventProc(
						   HWINEVENTHOOK hWinEventHook,
						   DWORD event,
						   HWND hwnd,
						   LONG idObject,
						   LONG idChild,
						   DWORD dwEventThread,
						   DWORD dwmsEventTime
						   );
void ActivateIl2MainWindow(HWND hWndIl2Main);
UINT SetForegroundWindowInternalThread(LPVOID pParam);
BOOL SetForegroundWindowInternal(HWND hWnd);
BOOL CheckActivated(HWND hWndIl2Main);

