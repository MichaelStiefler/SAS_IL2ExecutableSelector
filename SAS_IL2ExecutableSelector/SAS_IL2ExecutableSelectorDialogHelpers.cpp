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

//*************************************************************************
// Includes
//*************************************************************************
#include "StdAfx.h"
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <commctrl.h>
#include <string>
#include <iostream>
#include <fstream>
#include "common.h"
#include "extern_globals.h"
#include "trace.h"

#pragma comment(lib, "comctl32.lib")
#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable : 4996 )

DWORD dwErrorNum = 0;

//************************************
// Method:    AfterIl2Stopped
// FullName:  AfterIl2Stopped
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void AfterIl2Stopped()
{
    if(!g_bMultipleInstancesEnabled) {
        ShowWindow(g_hWnd, SW_SHOW);
    }

    if(g_iNumIl2InstancesRunning == 0) {
        EnableSettingChanges(TRUE);

        if(g_bExitWithIl2) {
            PostMessage(g_hWnd, WM_CLOSE, 0, 0);
        }
    }

    if(!g_bMultipleInstancesEnabled) {
        EnableWindow(GetDlgItem(g_hWnd, IDC_BUTTON_START_NOW), TRUE);
    }
}
//************************************
// Method:    stepRamSize
// FullName:  stepRamSize
// Access:    public
// Returns:   int
// Qualifier:
// Parameter: int baseRamSize
//************************************
int stepRamSize(int baseRamSize)
{
    if(g_bExpertModeEnabled) {
        return baseRamSize;
    }

    int retVal = g_iRamSizes[0];

    for(int i = lengthof(g_iRamSizes) - 1; i > 0; i--) {
        if(baseRamSize >= (int)((g_iRamSizes[i] + g_iRamSizes[i - 1]) / 2)) {
            retVal = g_iRamSizes[i];
            break;
        }
    }

    if(!g_bExpertModeEnabled) {
        if(retVal > 1024) {
            retVal = 1024;
        }
    }

    return retVal;
}
//************************************
// Method:    SetRamSliderTicks
// FullName:  SetRamSliderTicks
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void SetRamSliderTicks()
{
    SendMessage(GetDlgItem(g_hWnd, IDC_SLIDER_RAMSIZE), TBM_SETRANGE, (WPARAM)TRUE, (LPARAM)MAKELONG(g_bExpertModeEnabled ? 64 : 128, g_bExpertModeEnabled ? 2048 : 1024));
    SetDlgItemText(g_hWnd, IDC_STATIC_RAM_MIN, (g_bExpertModeEnabled) ? L"64" : L"128");
    SetDlgItemText(g_hWnd, IDC_STATIC_RAM_MAX, (g_bExpertModeEnabled) ? L"2048" : L"1024");
    SendMessage(GetDlgItem(g_hWnd, IDC_SLIDER_RAMSIZE), TBM_CLEARTICS, (WPARAM)TRUE, 0);
    SendMessage(GetDlgItem(g_hWnd, IDC_SLIDER_RAMSIZE), TBM_SETTIC, (WPARAM)TRUE, 128);
    SendMessage(GetDlgItem(g_hWnd, IDC_SLIDER_RAMSIZE), TBM_SETTIC, (WPARAM)TRUE, 256);
    SendMessage(GetDlgItem(g_hWnd, IDC_SLIDER_RAMSIZE), TBM_SETTIC, (WPARAM)TRUE, 512);
    SendMessage(GetDlgItem(g_hWnd, IDC_SLIDER_RAMSIZE), TBM_SETTIC, (WPARAM)TRUE, 768);
    SendMessage(GetDlgItem(g_hWnd, IDC_SLIDER_RAMSIZE), TBM_SETTIC, (WPARAM)TRUE, 1024);

    if(g_bExpertModeEnabled) {
        SendMessage(GetDlgItem(g_hWnd, IDC_SLIDER_RAMSIZE), TBM_SETTIC, (WPARAM)TRUE, 1200);
        SendMessage(GetDlgItem(g_hWnd, IDC_SLIDER_RAMSIZE), TBM_SETTIC, (WPARAM)TRUE, 2048);
    }
}


//************************************
// Method:    FillDropdown
// FullName:  FillDropdown
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void FillDropdown()
{
    for(int i = 0; i < g_iModTypes; i++) {
        SendMessage(GetDlgItem(g_hWnd, IDC_COMBO_MODTYPES), CB_ADDSTRING, 0, (LPARAM)g_szModTypes[i]);
    }
}

//************************************
// Method:    SettingsToControls
// FullName:  SettingsToControls
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void SettingsToControls()
{
    PostMessage(GetDlgItem(g_hWnd, IDC_COMBO_MODTYPES), CB_SETCURSEL, g_iModType, 0);
    ShowModtypeHints(g_iModType);
    g_iRamSize = stepRamSize(g_iRamSize);
    PostMessage(GetDlgItem(g_hWnd, IDC_COMBO_MODTYPES), CB_SETEDITSEL, 0, MAKELPARAM(0, -1));
    SendMessage(GetDlgItem(g_hWnd, IDC_EDIT_RAM), EM_SETREADONLY, !g_bExpertModeEnabled, 0);

    if(GetFocus() != GetDlgItem(g_hWnd, IDC_EDIT_RAM)) {
        SetDlgItemInt(g_hWnd, IDC_EDIT_RAM, g_iRamSize, FALSE);
    }

    SetRamSliderTicks();
    SendMessage(GetDlgItem(g_hWnd, IDC_SLIDER_RAMSIZE), TBM_SETPOS, (WPARAM) TRUE, (LPARAM) g_iRamSize);
    SendMessage(GetDlgItem(g_hWnd, IDC_SLIDER_RAMSIZE), TBM_SETSEL, (WPARAM) TRUE, (LPARAM) MAKELONG(0, g_iRamSize));
    EnableWindow(GetDlgItem(g_hWnd, IDC_CHECK_EXIT_WITH_IL2), bShowExitOnIl2Quit());
    CheckDlgButton(g_hWnd, IDC_CHECK_EXIT_WITH_IL2, (g_bExitWithIl2 && bShowExitOnIl2Quit()) ? BST_CHECKED : BST_UNCHECKED);
    EnableWindow(GetDlgItem(g_hWnd, IDC_CHECK_SET_HYPERLOBBY), bShowAdjustHyperlobby());

    if(bShowAdjustHyperlobby()) {
        TCHAR szHyperlobbyIl2Path[MAX_PATH];
        GetPrivateProfileString(L"IL-2 Sturmovik 1946", L"gamepath", L"", szHyperlobbyIl2Path, MAX_PATH, szHyperlobbyIniFile);

        if(!IsDlgButtonChecked(g_hWnd, IDC_RADIO_HYPERLOBBY_LAUNCHER) && !IsDlgButtonChecked(g_hWnd, IDC_RADIO_HYPERLOBBY_SELECTOR)) {
            CheckDlgButton(g_hWnd, IDC_RADIO_HYPERLOBBY_LAUNCHER, (_tcsncmp(szHyperlobbyIl2Path, szGamePath, _tcslen(szHyperlobbyIl2Path)) == 0) ? BST_CHECKED : BST_UNCHECKED);
            CheckDlgButton(g_hWnd, IDC_RADIO_HYPERLOBBY_SELECTOR, (_tcsncmp(szHyperlobbyIl2Path, szSelectorPath, _tcslen(szHyperlobbyIl2Path)) == 0) ? BST_CHECKED : BST_UNCHECKED);
        }

        if(_tcsncmp(szHyperlobbyIl2Path, szGamePath, _tcslen(szHyperlobbyIl2Path)) == 0) {
            SetStatusBar(1, TRUE, GetSysColor(COLOR_BTNTEXT), GetSysColor(COLOR_BTNFACE), TRANSPARENT, L"HL = IL-2");
            if (!g_bOverrideHLSet) CheckDlgButton(g_hWnd, IDC_CHECK_SET_HYPERLOBBY, BST_CHECKED);
        } else if(_tcsncmp(szHyperlobbyIl2Path, szSelectorPath, _tcslen(szHyperlobbyIl2Path)) == 0) {
            SetStatusBar(1, TRUE, GetSysColor(COLOR_BTNTEXT), GetSysColor(COLOR_BTNFACE), TRANSPARENT, L"HL = Selector");
            if (!g_bOverrideHLSet) CheckDlgButton(g_hWnd, IDC_CHECK_SET_HYPERLOBBY, BST_CHECKED);
        } else {
            SetStatusBar(1, TRUE, GetSysColor(COLOR_BTNTEXT), GetSysColor(COLOR_BTNFACE), TRANSPARENT, L"HL = Other App.");
            if (!g_bOverrideHLSet) CheckDlgButton(g_hWnd, IDC_CHECK_SET_HYPERLOBBY, BST_UNCHECKED);
        }
    } else {
        SetStatusBar(1, FALSE, RGB(255, 0, 0), GetSysColor(COLOR_BTNFACE), TRANSPARENT, L"No HL found");
    }

    EnableWindow(GetDlgItem(g_hWnd, IDC_RADIO_HYPERLOBBY_LAUNCHER), bShowAdjustHyperlobbyOptions());
    EnableWindow(GetDlgItem(g_hWnd, IDC_RADIO_HYPERLOBBY_SELECTOR), bShowAdjustHyperlobbyOptions());
    CheckDlgButton(g_hWnd, IDC_CHECK_EXPERT, (g_bExpertModeEnabled) ? BST_CHECKED : BST_UNCHECKED);

    ShowWindow(GetDlgItem(g_hWnd, IDC_GB_EXPERT_MODE), g_bExpertModeEnabled ? SW_SHOW : SW_HIDE);
    ShowWindow(GetDlgItem(g_hWnd, IDC_CHECK_CACHED_WRAPPER), g_bExpertModeEnabled ? SW_SHOW : SW_HIDE);
    ShowWindow(GetDlgItem(g_hWnd, IDC_CHECK_MULTI), g_bExpertModeEnabled ? SW_SHOW : SW_HIDE);
    ShowWindow(GetDlgItem(g_hWnd, IDC_GB_MEM_METHOD), g_bExpertModeEnabled ? SW_SHOW : SW_HIDE);
    ShowWindow(GetDlgItem(g_hWnd, IDC_RADIO_MEM_BALANCED), g_bExpertModeEnabled ? SW_SHOW : SW_HIDE);
    ShowWindow(GetDlgItem(g_hWnd, IDC_RADIO_MEM_CONSERVATIVE), g_bExpertModeEnabled ? SW_SHOW : SW_HIDE);
    ShowWindow(GetDlgItem(g_hWnd, IDC_RADIO_MEM_HEAPONLY), g_bExpertModeEnabled ? SW_SHOW : SW_HIDE);
    ShowWindow(GetDlgItem(g_hWnd, IDC_STATIC_JVM_PARAMS), g_bExpertModeEnabled ? SW_SHOW : SW_HIDE);
    ShowWindow(GetDlgItem(g_hWnd, IDC_EDIT_JVM_PARAMS), g_bExpertModeEnabled ? SW_SHOW : SW_HIDE);

    if (g_bAdjustWindowPos && g_iOperationMode == OPERATION_MODE_SETTINGS) {
        RECT buttonSaveRect, buttonCancelRect, mainWindowRect, statusRect, screenRect;
        if (g_bExpertModeEnabled) {
            buttonSaveRect = { 7, 242, 7 + 74, 242 + 14 };
            buttonCancelRect = { 84, 242, 84 + 74, 242 + 14 };
            mainWindowRect = { 0, 0, 166, 276 };
        }
        else {
            buttonSaveRect = { 7, 137, 7 + 74, 137 + 14 };
            buttonCancelRect = { 84, 137, 84 + 74, 137 + 14 };
            mainWindowRect = { 0, 0, 166, 172 };
        }
        MapDialogRect(g_hWnd, &buttonSaveRect);
        MapDialogRect(g_hWnd, &buttonCancelRect);
        MoveWindow(GetDlgItem(g_hWnd, IDC_BUTTON_SAVE_SETTINGS), buttonSaveRect.left, buttonSaveRect.top, buttonSaveRect.right - buttonSaveRect.left, buttonSaveRect.bottom - buttonSaveRect.top, TRUE);
        MoveWindow(GetDlgItem(g_hWnd, IDC_BUTTON_CANCEL_SETTINGS), buttonCancelRect.left, buttonCancelRect.top, buttonCancelRect.right - buttonCancelRect.left, buttonCancelRect.bottom - buttonCancelRect.top, TRUE);

        MapDialogRect(g_hWnd, &mainWindowRect);


        GetWindowRect(g_hWnd, &screenRect);
        int left = screenRect.left;
        
        GetClientRect(GetDesktopWindow(), &screenRect);
        int width = mainWindowRect.right - mainWindowRect.left;
        int height = mainWindowRect.bottom - mainWindowRect.top;
        if (left == 0) left = (screenRect.right / 2) - (width / 2);
        int top = (screenRect.bottom / 2) - (height / 2);
        
        MoveWindow(g_hWnd, left, top, width, height, TRUE);

        GetWindowRect(g_hWndStatus, &statusRect);
        MoveWindow(g_hWndStatus, -100, -100, 10, 10, TRUE);

    }

    EnableWindow(GetDlgItem(g_hWnd, IDC_CHECK_CACHED_WRAPPER), bShowModFilesCache());
	EnableWindow(GetDlgItem(g_hWnd, IDC_EDIT_CACHED_WRAPPER), bShowModFilesCache());
    CheckDlgButton(g_hWnd, IDC_CHECK_CACHED_WRAPPER, (g_bEnableModFilesCache && bShowModFilesCache()) ? BST_CHECKED : BST_UNCHECKED);
    EnableWindow(GetDlgItem(g_hWnd, IDC_CHECK_MULTI), bShowMultipleInstances());
    CheckDlgButton(g_hWnd, IDC_CHECK_MULTI, (g_bMultipleInstancesEnabled && bShowMultipleInstances()) ? BST_CHECKED : BST_UNCHECKED);
     CheckDlgButton(g_hWnd, IDC_RADIO_MEM_BALANCED, (g_iMemStrategy == MEM_STRATEGY_BALANCED) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(g_hWnd, IDC_RADIO_MEM_CONSERVATIVE, (g_iMemStrategy == MEM_STRATEGY_CONSERVATIVE) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(g_hWnd, IDC_RADIO_MEM_HEAPONLY, (g_iMemStrategy == MEM_STRATEGY_HEAPONLY) ? BST_CHECKED : BST_UNCHECKED);
	CheckDlgButton(g_hWnd, IDC_CHECK_SPLASH_SHOW, (g_iSplashScreenMode & SPLASH_SCREEN_VISIBLE) ? BST_CHECKED : BST_UNCHECKED);
    EnableWindow(GetDlgItem(g_hWnd, IDC_CHECK_SPLASH_TOPMOST), IsDlgButtonChecked(g_hWnd, IDC_CHECK_SPLASH_SHOW));
    CheckDlgButton(g_hWnd, IDC_CHECK_SPLASH_TOPMOST, (g_iSplashScreenMode & SPLASH_SCREEN_TOPMOST) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(g_hWnd, IDC_CHECK_DUMP_FILES, (g_iDumpMode & DUMP_MODE_DUMP_FILES) ? BST_CHECKED : BST_UNCHECKED);
    CheckDlgButton(g_hWnd, IDC_CHECK_LOG_SFS_ACCESS, (g_iDumpMode & DUMP_MODE_SFS_ACCESS) ? BST_CHECKED : BST_UNCHECKED);
    ShowAdditionalJvmParams();
    InvalidateRect(g_hWnd, NULL, TRUE);
    g_bAdjustWindowPos = FALSE;
    g_bOverrideHLSet = FALSE;
}
//************************************
// Method:    trim
// FullName:  trim
// Access:    public
// Returns:   void
// Qualifier:
// Parameter: std::string & str
//************************************
void trim(std::string& str)
{
    std::string::size_type pos = str.find_last_not_of(' ');

    if(pos != std::string::npos) {
        str.erase(pos + 1);
        pos = str.find_first_not_of(' ');

        if(pos != std::string::npos) {
            str.erase(0, pos);
        }
    } else {
        str.erase(str.begin(), str.end());
    }
}

//************************************
// Method:    ShowAdditionalJvmParams
// FullName:  ShowAdditionalJvmParams
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void ShowAdditionalJvmParams()
{
    if(!g_bExpertModeEnabled) {
        SetDlgItemText(g_hWnd, IDC_EDIT_JVM_PARAMS, L"");
        return;
    }

    TCHAR szJvmOptionsInFile[0xFFFF];
	ZeroMemory(szJvmOptionsInFile, sizeof(szJvmOptionsInFile));
    TCHAR szJvmOptions[0xFFFF];
	ZeroMemory(szJvmOptions, sizeof(szJvmOptions));
    GetPrivateProfileSection(L"JVM", szJvmOptionsInFile, 0xFFFF, szIniFile);
    LPTSTR lpJvmOptionToken = szJvmOptionsInFile;

    while(_tcslen(lpJvmOptionToken) != 0) {
        _tcscat(szJvmOptions, lpJvmOptionToken);
        _tcscat(szJvmOptions, L"\r\n");
        lpJvmOptionToken += _tcslen(lpJvmOptionToken) + 1;
    }

    SetDlgItemText(g_hWnd, IDC_EDIT_JVM_PARAMS, szJvmOptions);
}

//************************************
// Method:    ControlsToSettings
// FullName:  ControlsToSettings
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void ControlsToSettings()
{
    g_iModType = SendMessage(GetDlgItem(g_hWnd, IDC_COMBO_MODTYPES), CB_GETCURSEL, 0, 0);

    if(g_iModType > g_iModTypes - 1) {
        g_iModType = 0;
    }

    g_iRamSize = GetDlgItemInt(g_hWnd, IDC_EDIT_RAM, NULL, FALSE);

    if(g_iRamSize < 64) {
        g_iRamSize = 64;
    }

    if(g_iRamSize > 2048) {
        g_iRamSize = 2048;
    }

    g_bExitWithIl2 = IsDlgButtonChecked(g_hWnd, IDC_CHECK_EXIT_WITH_IL2);
    g_bExpertModeEnabled = IsDlgButtonChecked(g_hWnd, IDC_CHECK_EXPERT);
    g_bEnableModFilesCache = IsDlgButtonChecked(g_hWnd, IDC_CHECK_CACHED_WRAPPER);
    g_bMultipleInstancesEnabled = IsDlgButtonChecked(g_hWnd, IDC_CHECK_MULTI);
	g_iSplashScreenMode = 0;
	if (IsDlgButtonChecked(g_hWnd, IDC_CHECK_SPLASH_SHOW)) {
		g_iSplashScreenMode |= 1;
		if (IsDlgButtonChecked(g_hWnd, IDC_CHECK_SPLASH_TOPMOST))
			g_iSplashScreenMode |= 2;
	}

    if(IsDlgButtonChecked(g_hWnd, IDC_RADIO_MEM_HEAPONLY)) {
        g_iMemStrategy = MEM_STRATEGY_HEAPONLY;
    } else if(IsDlgButtonChecked(g_hWnd, IDC_RADIO_MEM_CONSERVATIVE)) {
        g_iMemStrategy = MEM_STRATEGY_CONSERVATIVE;
    } else {
        g_iMemStrategy = MEM_STRATEGY_BALANCED;
    }

    if(!g_bExpertModeEnabled) {
        g_bEnableModFilesCache = FALSE;
        g_bMultipleInstancesEnabled = FALSE;
    }
}

//************************************
// Method:    EnableSettingChanges
// FullName:  EnableSettingChanges
// Access:    public
// Returns:   void
// Qualifier:
// Parameter: BOOL bEnable
//************************************
void EnableSettingChanges(BOOL bEnable)
{
    switch(g_iOperationMode) {
    case OPERATION_MODE_START:
        EnableWindow(GetDlgItem(g_hWnd, IDC_BUTTON_CHANGE_CONFIG), bEnable);

        if(bEnable == FALSE) {
            EnableWindow(GetDlgItem(g_hWnd, IDC_STATIC_START_COUNTDOWN), FALSE);
        }

        break;

    case OPERATION_MODE_SETTINGS:
        EnableWindow(GetDlgItem(g_hWnd, IDC_STATIC_GAME_TYPE), bEnable);
        EnableWindow(GetDlgItem(g_hWnd, IDC_COMBO_MODTYPES), bEnable);
        EnableWindow(GetDlgItem(g_hWnd, IDC_STATIC_RAM), bEnable);
        SendMessage(GetDlgItem(g_hWnd, IDC_EDIT_RAM), EM_SETREADONLY, !(bEnable && g_bExpertModeEnabled), 0);
        EnableWindow(GetDlgItem(g_hWnd, IDC_STATIC_RAM2), bEnable);
        EnableWindow(GetDlgItem(g_hWnd, IDC_STATIC_RAM_MIN), bEnable);
        EnableWindow(GetDlgItem(g_hWnd, IDC_SLIDER_RAMSIZE), bEnable);
        EnableWindow(GetDlgItem(g_hWnd, IDC_STATIC_RAM_MAX), bEnable);
        EnableWindow(GetDlgItem(g_hWnd, IDC_CHECK_EXIT_WITH_IL2), bEnable && bShowExitOnIl2Quit());
        EnableWindow(GetDlgItem(g_hWnd, IDC_CHECK_SET_HYPERLOBBY), bEnable && bShowAdjustHyperlobby());
        EnableWindow(GetDlgItem(g_hWnd, IDC_RADIO_HYPERLOBBY_LAUNCHER), bEnable && bShowAdjustHyperlobbyOptions());
        EnableWindow(GetDlgItem(g_hWnd, IDC_RADIO_HYPERLOBBY_SELECTOR), bEnable && bShowAdjustHyperlobbyOptions());
        EnableWindow(GetDlgItem(g_hWnd, IDC_CHECK_EXPERT), bEnable);
        EnableWindow(GetDlgItem(g_hWnd, IDC_CHECK_CACHED_WRAPPER), bEnable && bShowModFilesCache());
        EnableWindow(GetDlgItem(g_hWnd, IDC_CHECK_MULTI), bEnable && bShowMultipleInstances());
        break;

    default:
        break;
    }
}

//************************************
// Control Activation Helper Functions
//************************************
BOOL bShowExitOnIl2Quit()
{
    return !(g_bMultipleInstancesEnabled);
}
BOOL bShowModFilesCache()
{
    return ((g_iModType != 0) && g_bExpertModeEnabled);
}
BOOL bShowMultipleInstances()
{
    return ((!g_bExitWithIl2) && (g_bExpertModeEnabled));
}
BOOL bShowAdjustHyperlobby()
{
    return FileExists(szHyperlobbyIniFile);
}
BOOL bShowAdjustHyperlobbyOptions()
{
    return (bShowAdjustHyperlobby() && IsDlgButtonChecked(g_hWnd, IDC_CHECK_SET_HYPERLOBBY));
}
//************************************
// Method:    BringToFront
// FullName:  BringToFront
// Access:    public
// Returns:   void
// Qualifier:
// Parameter: HWND hwnd
//************************************
void BringToFront(HWND hwnd)
{
    SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 0, (LPVOID)0, SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);
    ShowWindowAsync(hwnd, SW_SHOWNORMAL);
    SetForegroundWindow(hwnd);
    SystemParametersInfo(SPI_SETFOREGROUNDLOCKTIMEOUT, 200000, (LPVOID)0, SPIF_SENDWININICHANGE | SPIF_UPDATEINIFILE);
}

//************************************
// Method:    CheckRamUsage
// FullName:  CheckRamUsage
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void CheckRamUsage()
{
    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    int iMegsFree = (int)(status.ullAvailVirtual / 0x100000);

    if(g_iRamSize <= 1200) {
        PostMessage(g_hWnd, WM_IL2_INSUFFICIENT_RAM, 0, RAM_MESSAGE_GOOD);
    } else if(g_iRamSize < iMegsFree) {
        PostMessage(g_hWnd, WM_IL2_INSUFFICIENT_RAM, 0, RAM_MESSAGE_CRITICAL);
    } else {
        PostMessage(g_hWnd, WM_IL2_INSUFFICIENT_RAM, 0, RAM_MESSAGE_ERROR);
    }
}

void ShowModtypeHints(int iModType)
{
    EDITBALLOONTIP editballoontip;
    editballoontip.cbStruct = sizeof(EDITBALLOONTIP);
    editballoontip.ttiIcon = TTI_INFO;
    editballoontip.pszTitle = g_szModTypes[iModType];
    editballoontip.pszText = g_szModTypeHelpText[iModType];
    SendMessage(GetDlgItem(g_hWnd, IDC_EDIT_MODTYPE_HELP), EM_SHOWBALLOONTIP, 0, (LPARAM)&editballoontip);
}

//************************************
// Method:    CreateStatusBar
// FullName:  CreateStatusBar
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void CreateStatusBar(int numParts, ...)
{
    int *status_parts = NULL;
    va_list argptr;
    va_start(argptr, numParts);

    if(numParts < 1) {
        va_end(argptr);
        return;
    }

    status_parts = new int[numParts];

    for(int i = 0; i < numParts; i++) {
        status_parts[i] = va_arg(argptr, int);
    }

    va_end(argptr);
    g_hWndStatus = CreateWindowEx(
                       0L,                              // no extended styles
                       STATUSCLASSNAME,                 // status bar
                       L"",                              // no text
                       WS_CHILD | WS_VISIBLE/* | SBARS_SIZEGRIP*/,  // styles
                       -100, -100, 10, 10,              // x, y, cx, cy
                       g_hWnd,                            // parent window
                       (HMENU)100,                      // window ID
                       g_hInst,                           // instance
                       NULL);                           // window data

    if(g_hWndStatus == NULL) {
        //MessageBox (NULL, L"Status Bar not created!", NULL, MB_OK );
        return;
    }

    SendMessage(g_hWndStatus, SB_SETPARTS, numParts, (LPARAM)status_parts);
    SendMessage(g_hWndStatus, WM_SETFONT, (WPARAM)g_hStatusFont, TRUE);
    delete [] status_parts;
}

//************************************
// Method:    EnableStatusBar
// FullName:  EnableStatusBar
// Access:    public
// Returns:   void
// Qualifier:
// Parameter: int iStatusBarIndex
// Parameter: BOOL bEnabled
//************************************
void EnableStatusBar(int iStatusBarIndex, BOOL bEnabled)
{
    g_itemStatus[iStatusBarIndex].bEnabled = bEnabled;
}
//************************************
// Method:    SetStatusBarForegroundColor
// FullName:  SetStatusBarForegroundColor
// Access:    public
// Returns:   void
// Qualifier:
// Parameter: int iStatusBarIndex
// Parameter: COLORREF crForegroundColor
//************************************
void SetStatusBarForegroundColor(int iStatusBarIndex, COLORREF crForegroundColor)
{
    g_itemStatus[iStatusBarIndex].crForeground = crForegroundColor;
}
//************************************
// Method:    SetStatusBarBackgroundColor
// FullName:  SetStatusBarBackgroundColor
// Access:    public
// Returns:   void
// Qualifier:
// Parameter: int iStatusBarIndex
// Parameter: COLORREF crBackgroundColor
//************************************
void SetStatusBarBackgroundColor(int iStatusBarIndex, COLORREF crBackgroundColor)
{
    g_itemStatus[iStatusBarIndex].crBackground = crBackgroundColor;
}
//************************************
// Method:    SetStatusBarColor
// FullName:  SetStatusBarColor
// Access:    public
// Returns:   void
// Qualifier:
// Parameter: int iStatusBarIndex
// Parameter: COLORREF crForegroundColor
// Parameter: COLORREF crBackgroundColor
//************************************
void SetStatusBarColor(int iStatusBarIndex, COLORREF crForegroundColor, COLORREF crBackgroundColor)
{
    SetStatusBarForegroundColor(iStatusBarIndex, crForegroundColor);
    SetStatusBarBackgroundColor(iStatusBarIndex, crBackgroundColor);
}
//************************************
// Method:    SetStatusBarBackgroundMode
// FullName:  SetStatusBarBackgroundMode
// Access:    public
// Returns:   void
// Qualifier:
// Parameter: int iStatusBarIndex
// Parameter: int iBackgroundMode
//************************************
void SetStatusBarBackgroundMode(int iStatusBarIndex, int iBackgroundMode)
{
    g_itemStatus[iStatusBarIndex].iBackgroundMode = iBackgroundMode;
}
//************************************
// Method:    SetStatusBarText
// FullName:  SetStatusBarText
// Access:    public
// Returns:   void
// Qualifier:
// Parameter: int iStatusBarIndex
// Parameter: LPCTSTR lpStatus
//************************************
void SetStatusBarText(int iStatusBarIndex, LPCTSTR lpStatus)
{
    if(g_itemStatus[iStatusBarIndex].lpItemText == NULL) {
        g_itemStatus[iStatusBarIndex].lpItemText = (LPTSTR)malloc(sizeof(TCHAR) * 256);
    }

    _tcscpy(g_itemStatus[iStatusBarIndex].lpItemText, lpStatus);
    PostMessage(g_hWndStatus, SB_SETTEXT, iStatusBarIndex | SBT_OWNERDRAW, (LPARAM)&g_itemStatus[iStatusBarIndex]);
}
//************************************
// Method:    SetStatusBar
// FullName:  SetStatusBar
// Access:    public
// Returns:   void
// Qualifier:
// Parameter: int iStatusBarIndex
// Parameter: BOOL bEnabled
// Parameter: COLORREF crForegroundColor
// Parameter: COLORREF crBackgroundColor
// Parameter: int iBackgroundMode
// Parameter: LPCTSTR lpStatus
//************************************
void SetStatusBar(int iStatusBarIndex, BOOL bEnabled, COLORREF crForegroundColor, COLORREF crBackgroundColor, int iBackgroundMode, LPCTSTR lpStatus)
{
    EnableStatusBar(iStatusBarIndex, bEnabled);
    SetStatusBarColor(iStatusBarIndex, crForegroundColor, crBackgroundColor);
    SetStatusBarBackgroundMode(iStatusBarIndex, iBackgroundMode);
    SetStatusBarText(iStatusBarIndex, lpStatus);
}


//************************************
// Method:    SetRAMStatus
// FullName:  SetRAMStatus
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void SetRAMStatus()
{
    if(g_iRamSize <= 512) {
        SetStatusBar(0, TRUE, GetSysColor(COLOR_BTNTEXT), RGB(0, 255, 0), TRANSPARENT, L"RAM GOOD");
        return;
    }

    if(g_iRamSize <= 1024) {
        SetStatusBar(0, TRUE, GetSysColor(COLOR_BTNTEXT), RGB(154, 205, 50), TRANSPARENT, L"RAM FEASIBLE");
        return;
    }

    if(g_iRamSize <= 1200) {
        SetStatusBar(0, TRUE, GetSysColor(COLOR_BTNTEXT), RGB(255, 255, 0), TRANSPARENT, L"RAM RISKY");
        return;
    }

    MEMORYSTATUSEX status;
    status.dwLength = sizeof(status);
    GlobalMemoryStatusEx(&status);
    int iMegsFree = (int)(status.ullAvailVirtual / 0x100000);

    if(g_iRamSize < iMegsFree) {
        SetStatusBar(0, TRUE, GetSysColor(COLOR_BTNTEXT), RGB(255, 140, 0), TRANSPARENT, L"RAM CRITICAL");
        return;
    }

    SetStatusBar(0, TRUE, GetSysColor(COLOR_BTNTEXT), RGB(255, 0, 0), TRANSPARENT, L"RAM ERROR");
}