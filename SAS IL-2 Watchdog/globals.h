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

#include "StdAfx.h"
#include <set>
#include <string>
#include <stdarg.h>
#include <windows.h>
#include <atomic>
#include "gdiplus.h"

// Suppress compiler warnings about unsafe functions
#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable : 4996 )

#define LOGFILE_NAME		L"initlog.lst"

//*************************************************************************
// Type definitions
//*************************************************************************
typedef void (WINAPI *PROCSWITCHTOTHISWINDOW)(HWND, BOOL);

//*************************************************************************
// Globals
//*************************************************************************
extern PROCSWITCHTOTHISWINDOW MySwitchToThisWindow;
extern TCHAR LogFileName[MAX_PATH];
extern const TCHAR *	c_szSplashTitle;
extern const TCHAR *	c_szSplashClass;
extern const TCHAR *	c_szMsgWndTitle;
extern const TCHAR *	c_szMsgWndClass;
extern HWND g_hMsgWnd;
extern HWND g_hIl2MainWindow;
extern HANDLE g_hIl2ProcessHandle;
extern int g_iSplashScreenMode;
extern HWND g_hSplashWnd;
extern TCHAR g_szSplashImageParam[MAX_PATH];
extern TCHAR g_szSplashImage[MAX_PATH];
