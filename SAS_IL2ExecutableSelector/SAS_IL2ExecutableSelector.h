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
HWND g_hWnd;
HINSTANCE g_hInst;
HWND g_hWndStatus;
LPWSTR g_lpCmdLine;
HICON g_hIconLarge, g_hIconSmall;
int g_iOperationMode = OPERATION_MODE_START;
int g_iNumIl2InstancesRunning = 0;
int g_iModType = 0;
int g_iRamSize = 0;
int g_iMemStrategy = 0;
int g_iSplashScreenMode = 0;
int g_iDumpMode = 0;
BOOL g_bExitWithIl2 = FALSE;
BOOL g_bExpertModeEnabled = FALSE;
BOOL g_bEnableModFilesCache = FALSE;
BOOL g_bMultipleInstancesEnabled = FALSE;
STATUSITEM g_itemStatus[3] = {};
HBRUSH g_hBrushYellow = NULL;
HBRUSH g_hBrushRed = NULL;
HBRUSH g_hBrushGreen = NULL;
HBRUSH g_hBrushOrange = NULL;
HFONT g_hListBoxFont = NULL;
HFONT g_hStatusFont = NULL;
HBRUSH g_hBrushYellowGreen = NULL;
int g_iMessageType = 0;
BOOL g_bRamToolTipVisible = FALSE;
BOOL g_bAdjustWindowPos = FALSE;
BOOL g_bOverrideHLSet = FALSE;
int g_iModTypes = 0;
LPTSTR* g_szModTypes = NULL;
LPTSTR* g_szModTypeHelpText = NULL;
LPTSTR* g_szModTypeExeParms = NULL;
LPTSTR* g_szModTypeIl2fbExe = NULL;
LPTSTR* g_szModTypeWrapperDll = NULL;
int g_iRamSizes[5] = {128, 256, 512, 768, 1024};
TCHAR szAppName[MAX_PATH];
TCHAR szSelectorPath[MAX_PATH];
TCHAR szGamePath[MAX_PATH];
TCHAR szIniFile[MAX_PATH];
TCHAR szLauncherFile[MAX_PATH];
TCHAR szLauncherModBaseFile[MAX_PATH];
TCHAR szLauncherStockBaseFile[MAX_PATH];
TCHAR szWrapperFile[MAX_PATH];
TCHAR szWrapperBaseFile[MAX_PATH];
TCHAR szDinputFile[MAX_PATH];
TCHAR szDinputBaseFile[MAX_PATH];
TCHAR szHyperlobbyIniFile[MAX_PATH];
TCHAR szWindowTitle[MAX_PATH];