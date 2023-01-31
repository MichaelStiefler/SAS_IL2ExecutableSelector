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

#include "globals.h"

// Suppress compiler warnings about unsafe functions
#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable : 4996 )

//*************************************************************************
// Globals
//*************************************************************************
PROCSWITCHTOTHISWINDOW MySwitchToThisWindow = NULL;
TCHAR LogFileName[MAX_PATH];
const TCHAR *	c_szSplashTitle = _T("IL-2 Sturmovik 1946 Launcher ©SAS 2019 (http://www.sas1946.com)");
const TCHAR *	c_szSplashClass = _T("Il2SASLauncherWnd");
const TCHAR *	c_szMsgWndTitle = _T("IL-2 Sturmovik 1946 Watchdog ©SAS 2019 (http://www.sas1946.com)");
const TCHAR *	c_szMsgWndClass = _T("Il2SASWatchdogMsgWnd");
HWND g_hMsgWnd = NULL;
HWND g_hIl2MainWindow = NULL;
HANDLE g_hIl2ProcessHandle = NULL;
int g_iSplashScreenMode;
HWND g_hSplashWnd = NULL;
TCHAR g_szSplashImageParam[MAX_PATH];
TCHAR g_szSplashImage[MAX_PATH];
