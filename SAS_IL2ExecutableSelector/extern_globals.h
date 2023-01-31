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
//*************************************************************************
// Globals
//*************************************************************************
extern HWND g_hWnd;
extern HINSTANCE g_hInst;
extern HWND g_hWndStatus;
extern LPWSTR g_lpCmdLine;
extern HICON g_hIconLarge, g_hIconSmall;
extern int g_iOperationMode;
extern int g_iNumIl2InstancesRunning;
extern int g_iModType;
extern int g_iRamSize;
extern int g_iMemStrategy;
extern int g_iSplashScreenMode;
extern int g_iDumpMode;
extern BOOL g_bExitWithIl2;
extern BOOL g_bExpertModeEnabled;
extern BOOL g_bEnableModFilesCache;
extern BOOL g_bMultipleInstancesEnabled;
extern BOOL g_bAdjustWindowPos;
extern BOOL g_bOverrideHLSet;
extern STATUSITEM g_itemStatus[3];
extern HBRUSH g_hBrushYellow;
extern HBRUSH g_hBrushRed;
extern HBRUSH g_hBrushGreen;
extern HBRUSH g_hBrushOrange;
extern HBRUSH g_hBrushYellowGreen;
extern HFONT g_hListBoxFont;
extern HFONT g_hStatusFont;
extern int g_iMessageType;
extern BOOL g_bRamToolTipVisible;
extern int g_iModTypes;
extern LPTSTR* g_szModTypes;
extern LPTSTR* g_szModTypeHelpText;
extern LPTSTR* g_szModTypeExeParms;
extern LPTSTR* g_szModTypeIl2fbExe;
extern LPTSTR* g_szModTypeWrapperDll;
extern int g_iRamSizes[5];
extern TCHAR szAppName[MAX_PATH];
extern TCHAR szSelectorPath[MAX_PATH];
extern TCHAR szGamePath[MAX_PATH];
extern TCHAR szIniFile[MAX_PATH];
extern TCHAR szLauncherFile[MAX_PATH];
extern TCHAR szLauncherModBaseFile[MAX_PATH];
extern TCHAR szLauncherStockBaseFile[MAX_PATH];
extern TCHAR szWrapperFile[MAX_PATH];
extern TCHAR szWrapperBaseFile[MAX_PATH];
extern TCHAR szDinputFile[MAX_PATH];
extern TCHAR szDinputBaseFile[MAX_PATH];
extern TCHAR szHyperlobbyIniFile[MAX_PATH];
extern TCHAR szWindowTitle[MAX_PATH];