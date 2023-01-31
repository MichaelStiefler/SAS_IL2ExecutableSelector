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
#include "common.h"
#include "extern_globals.h"
#include "trace.h"

#pragma comment(lib, "comctl32.lib")
#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable : 4996 )

WNDPROC wpOrigEditProcExpert = NULL;
WNDPROC wpOrigEditProcCachedWrapper = NULL;

//************************************
// Method:    SASES_DialogProc
// FullName:  SASES_DialogProc
// Access:    public
// Returns:   INT_PTR CALLBACK
// Qualifier:
// Parameter: HWND hwndDlg
// Parameter: UINT uMsg
// Parameter: WPARAM wParam
// Parameter: LPARAM lParam
//************************************
INT_PTR CALLBACK SASES_DialogProc(
    HWND hwndDlg,
    UINT uMsg,
    WPARAM wParam,
    LPARAM lParam)
{
    switch(uMsg) {
        HANDLE_MSG(hwndDlg, WM_INITDIALOG, SASES_OnInitDialog);
        HANDLE_MSG(hwndDlg, WM_COMMAND, SASES_OnCommand);
        HANDLE_MSG(hwndDlg, WM_HSCROLL, SASES_OnHScroll);
        HANDLE_MSG(hwndDlg, WM_TIMER, SASES_OnTimer);
        HANDLE_MSG(hwndDlg, WM_CTLCOLOREDIT, SASES_OnCtlColorEdit);
        HANDLE_MSG(hwndDlg, WM_DRAWITEM, SASES_OnDrawItem);
        HANDLE_MSG(hwndDlg, WM_CREATE, SASES_OnCreate);
        HANDLE_MSG(hwndDlg, WM_DESTROY, SASES_OnDestroy);

    case WM_IL2_STARTED:
        SASES_OnIl2Started();
        break;

    case WM_IL2_STOPPED:
        SASES_OnIl2Stopped();
        break;

    case WM_IL2_LOADED:
        SASES_OnIl2Loaded();
        break;

    case WM_IL2_START_ERROR:
        SASES_OnIl2StartError();
        break;

    case WM_IL2_INSUFFICIENT_RAM:
        SASES_OnIl2InsufficientRam((int)lParam);
        break;

    default:
        break;
    }

    return FALSE;
}

LRESULT APIENTRY EditSubclassProcExpert(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{ 
	if (uMsg == WM_LBUTTONDOWN) {
		TCHAR szEditText[MAX_PATH];
		SendMessage(hwnd, WM_GETTEXT, MAX_PATH, (LPARAM)&szEditText[0]);
		if (_tcscmp(szEditText, EDIT_KEY_RESET) == 0) {
			SetFocus(hwnd);
			SendMessage(hwnd, EM_SETSEL, 0, -1);
			return TRUE;
		}
	}
	return CallWindowProc(wpOrigEditProcExpert, hwnd, uMsg, wParam, lParam); 
} 

LRESULT APIENTRY EditSubclassProcCachedWrapper(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) 
{ 
	if (uMsg == WM_LBUTTONDOWN) {
		TCHAR szEditText[MAX_PATH];
		SendMessage(hwnd, WM_GETTEXT, MAX_PATH, (LPARAM)&szEditText[0]);
		if (_tcscmp(szEditText, EDIT_KEY_RESET) == 0) {
			SetFocus(hwnd);
			SendMessage(hwnd, EM_SETSEL, 0, -1);
			return TRUE;
		}
	}
	return CallWindowProc(wpOrigEditProcCachedWrapper, hwnd, uMsg, wParam, lParam); 
} 


//************************************
// Method:    SASES_OnCreate
// FullName:  SASES_OnCreate
// Access:    public
// Returns:   bool
// Qualifier:
// Parameter: HWND hwndDlg
// Parameter: LPCREATESTRUCT lpCS
//************************************
bool SASES_OnCreate(HWND hwndDlg, LPCREATESTRUCT lpCS)
{
    InitCommonControls();
    INITCOMMONCONTROLSEX icex;
    icex.dwSize = sizeof(INITCOMMONCONTROLSEX);
    icex.dwICC   = ICC_ANIMATE_CLASS | ICC_COOL_CLASSES | ICC_BAR_CLASSES | ICC_LISTVIEW_CLASSES | ICC_PROGRESS_CLASS | ICC_STANDARD_CLASSES | ICC_TAB_CLASSES | ICC_USEREX_CLASSES;
    InitCommonControlsEx(&icex);
    return FALSE;
}
//************************************
// Method:    SASES_OnInitDialog
// FullName:  SASES_OnInitDialog
// Access:    public
// Returns:   BOOL
// Qualifier:
// Parameter: HWND hwnd
// Parameter: HWND
// Parameter: LPARAM
//************************************
BOOL SASES_OnInitDialog(HWND hwnd, HWND /*hwndFocus*/, LPARAM /*lParam*/)
{
    g_hWnd = hwnd;
    SendMessage(hwnd, WM_SETICON, ICON_SMALL, (LPARAM) g_hIconSmall);
    SendMessage(hwnd, WM_SETICON, ICON_BIG, (LPARAM) g_hIconLarge);

    if(g_iOperationMode == OPERATION_MODE_START) {
        CreateStatusBar(3, 150, 300, 1000);
        ShowCurrentSettings();
    } else {
        CreateStatusBar(3, 150, 300, 1000);
    }

    FillDropdown();
    g_bAdjustWindowPos = TRUE;
    SettingsToControls();
    SetRAMStatus();

    if(g_iOperationMode != OPERATION_MODE_START) {
        SetStatusBar(2, TRUE, GetSysColor(COLOR_BTNTEXT), RGB(0, 255, 0), TRANSPARENT, L"Settings Saved");
    }

    SetWindowText(hwnd, szWindowTitle);
	wpOrigEditProcExpert = (WNDPROC) SetWindowLong(GetDlgItem(hwnd, IDC_EDIT_EXPERT), GWL_WNDPROC, (LONG) EditSubclassProcExpert); 
	wpOrigEditProcCachedWrapper = (WNDPROC) SetWindowLong(GetDlgItem(hwnd, IDC_EDIT_CACHED_WRAPPER), GWL_WNDPROC, (LONG) EditSubclassProcCachedWrapper); 
    BringToFront(hwnd);

    return FALSE;
}

//************************************
// Method:    SASES_OnDestroy
// FullName:  SASES_OnDestroy
// Access:    public
// Returns:   void
// Qualifier:
// Parameter: HWND hwndDlg
//************************************
void SASES_OnDestroy(HWND hwndDlg)
{
    return;
}
//************************************
// Method:    ShowCurrentSettings
// FullName:  ShowCurrentSettings
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void ShowCurrentSettings()
{
    TCHAR szBuf[MAX_PATH];
    SendDlgItemMessage(g_hWnd, IDC_LIST_SETTINGS, WM_SETFONT, (WPARAM)g_hListBoxFont, TRUE);
    SendDlgItemMessage(g_hWnd, IDC_LIST_SETTINGS, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Current Game Settings:");
    SendDlgItemMessage(g_hWnd, IDC_LIST_SETTINGS, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"");

    if(g_iModType < g_iModTypes) {
        SendDlgItemMessage(g_hWnd, IDC_LIST_SETTINGS, LB_ADDSTRING, (WPARAM)0, (LPARAM)g_szModTypes[g_iModType]);
    } else {
        SendDlgItemMessage(g_hWnd, IDC_LIST_SETTINGS, LB_ADDSTRING, (WPARAM)0, (LPARAM)g_szModTypes[1]);
    }

    _stprintf(szBuf, L"Maximum RAM size: %d MB", g_iRamSize);
    SendDlgItemMessage(g_hWnd, IDC_LIST_SETTINGS, LB_ADDSTRING, (WPARAM)0, (LPARAM)szBuf);

    switch(g_iMemStrategy) {
    case MEM_STRATEGY_BALANCED:
        SendDlgItemMessage(g_hWnd, IDC_LIST_SETTINGS, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Memory Strategy: Balanced");
        break;

    case MEM_STRATEGY_CONSERVATIVE:
        SendDlgItemMessage(g_hWnd, IDC_LIST_SETTINGS, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Memory Strategy: Conservative");
        break;

    case MEM_STRATEGY_HEAPONLY:
        SendDlgItemMessage(g_hWnd, IDC_LIST_SETTINGS, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Memory Strategy: Heap only");
        break;

    default:
        SendDlgItemMessage(g_hWnd, IDC_LIST_SETTINGS, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Memory Strategy: Unknown");
        break;
    }

    if(g_bExitWithIl2) {
        SendDlgItemMessage(g_hWnd, IDC_LIST_SETTINGS, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"This Selector Exits with IL-2");
    }

    if(g_bEnableModFilesCache) {
        SendDlgItemMessage(g_hWnd, IDC_LIST_SETTINGS, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Mod Files Cache enabled");
    }

    if(g_bMultipleInstancesEnabled) {
        SendDlgItemMessage(g_hWnd, IDC_LIST_SETTINGS, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Multiple IL-2 Instances enabled");
    }

    if (g_iDumpMode & DUMP_MODE_DUMP_FILES) {
        SendDlgItemMessage(g_hWnd, IDC_LIST_SETTINGS, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"File Dump enabled");
    }

    if (g_iDumpMode & DUMP_MODE_SFS_ACCESS) {
        SendDlgItemMessage(g_hWnd, IDC_LIST_SETTINGS, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Log SFS File access");
    }

    if(_tcslen(g_lpCmdLine) > 0) {
        SendDlgItemMessage(g_hWnd, IDC_LIST_SETTINGS, LB_SETHORIZONTALEXTENT, (WPARAM)(_tcslen(g_lpCmdLine) * 10), (LPARAM)0);
        SendDlgItemMessage(g_hWnd, IDC_LIST_SETTINGS, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"");
        SendDlgItemMessage(g_hWnd, IDC_LIST_SETTINGS, LB_ADDSTRING, (WPARAM)0, (LPARAM)L"Additional Parameters:");
        SendDlgItemMessage(g_hWnd, IDC_LIST_SETTINGS, LB_ADDSTRING, (WPARAM)0, (LPARAM)g_lpCmdLine);
    }

    if(FileExists(szHyperlobbyIniFile)) {
        TCHAR szHyperlobbyIl2Path[MAX_PATH];
        GetPrivateProfileString(L"IL-2 Sturmovik 1946", L"gamepath", L"", szHyperlobbyIl2Path, MAX_PATH, szHyperlobbyIniFile);

        if(_tcsncmp(szHyperlobbyIl2Path, szGamePath, _tcslen(szHyperlobbyIl2Path)) == 0) {
            SetStatusBar(1, TRUE, GetSysColor(COLOR_BTNTEXT), GetSysColor(COLOR_BTNFACE), TRANSPARENT, L"HL = IL-2");
        } else if(_tcsncmp(szHyperlobbyIl2Path, szSelectorPath, _tcslen(szHyperlobbyIl2Path)) == 0) {
            SetStatusBar(1, TRUE, GetSysColor(COLOR_BTNTEXT), GetSysColor(COLOR_BTNFACE), TRANSPARENT, L"HL = Selector");
        } else {
            SetStatusBar(1, TRUE, GetSysColor(COLOR_BTNTEXT), GetSysColor(COLOR_BTNFACE), TRANSPARENT, L"HL = Other App.");
        }
    } else {
        SetStatusBar(1, FALSE, RGB(255, 0, 0), GetSysColor(COLOR_BTNFACE), TRANSPARENT, L"No HL found");
    }
}

//************************************
// Method:    SASES_OnCommand
// FullName:  SASES_OnCommand
// Access:    public
// Returns:   void
// Qualifier:
// Parameter: HWND hwnd
// Parameter: int id
// Parameter: HWND hwndCtl
// Parameter: UINT codeNotify
//************************************
void SASES_OnCommand(HWND hwnd, int id, HWND hwndCtl, UINT codeNotify)
{
    switch(id) {
    case IDOK:
        LaunchIl2();
        break;

    case IDCANCEL:
        EndDialog(hwnd, IDCANCEL);
        break;

    case IDC_BUTTON_START_NOW:
        LaunchIl2();
        break;

    case IDC_BUTTON_CHANGE_CONFIG:
        EndDialog(hwnd, IDC_BUTTON_CHANGE_CONFIG);
        break;

    case IDC_BUTTON_SAVE_SETTINGS:
        ControlsToSettings();
        WriteIniSettings();
        CreateIl2FbExe();
        SetStatusBar(2, TRUE, GetSysColor(COLOR_BTNTEXT), RGB(0, 255, 0), TRANSPARENT, L"Settings Saved");
        EndDialog(hwnd, IDC_BUTTON_SAVE_SETTINGS);
        break;

    case IDC_BUTTON_CANCEL_SETTINGS:
        ReadIniSettings();
        EndDialog(hwnd, IDC_BUTTON_CANCEL_SETTINGS);
        break;

    case IDC_CHECK_EXIT_WITH_IL2:
        g_bExitWithIl2 = IsDlgButtonChecked(g_hWnd, IDC_CHECK_EXIT_WITH_IL2);
        SettingsToControls();
        SetStatusBar(2, TRUE, GetSysColor(COLOR_BTNTEXT), RGB(255, 255, 0), TRANSPARENT, L"Settings Changed");
        break;

    case IDC_CHECK_EXPERT:
        g_bExpertModeEnabled = IsDlgButtonChecked(g_hWnd, IDC_CHECK_EXPERT);
        g_bAdjustWindowPos = TRUE;
        SettingsToControls();
        SetStatusBar(2, TRUE, GetSysColor(COLOR_BTNTEXT), RGB(255, 255, 0), TRANSPARENT, L"Settings Changed");
        break;

    case IDC_CHECK_CACHED_WRAPPER:
        g_bEnableModFilesCache = IsDlgButtonChecked(g_hWnd, IDC_CHECK_CACHED_WRAPPER);
        SettingsToControls();
        SetStatusBar(2, TRUE, GetSysColor(COLOR_BTNTEXT), RGB(255, 255, 0), TRANSPARENT, L"Settings Changed");
        break;

    case IDC_CHECK_MULTI:
        g_bMultipleInstancesEnabled = IsDlgButtonChecked(g_hWnd, IDC_CHECK_MULTI);
        SettingsToControls();
        SetStatusBar(2, TRUE, GetSysColor(COLOR_BTNTEXT), RGB(255, 255, 0), TRANSPARENT, L"Settings Changed");
        break;

    case IDC_COMBO_MODTYPES: {
        switch(codeNotify) {
        case CBN_SELCHANGE:
            g_iModType = SendMessage(GetDlgItem(g_hWnd, IDC_COMBO_MODTYPES), CB_GETCURSEL, 0, 0);
            SettingsToControls();
            ShowModtypeHints(g_iModType);
            break;
        }

        SetStatusBar(2, TRUE, GetSysColor(COLOR_BTNTEXT), RGB(255, 255, 0), TRANSPARENT, L"Settings Changed");
        break;
    }

    case IDC_EDIT_RAM: {
        switch(codeNotify) {
        case EN_UPDATE: {
            if(GetFocus() == GetDlgItem(hwnd, IDC_EDIT_RAM)) {
                int iValue = GetDlgItemInt(g_hWnd, IDC_EDIT_RAM, FALSE, FALSE);

                if((g_bExpertModeEnabled && (iValue >= 64) && (iValue <= 2048)) || ((iValue >= 128) && (iValue <= 1024))) {
                    g_iRamSize = iValue;
                    SettingsToControls();
                }

                CheckRamUsage();
            }
        }
        break;
        }

        SetStatusBar(2, TRUE, GetSysColor(COLOR_BTNTEXT), RGB(255, 255, 0), TRANSPARENT, L"Settings Changed");
        break;
    }

    case IDC_CHECK_SET_HYPERLOBBY:
        g_bOverrideHLSet = TRUE;
        SettingsToControls();
        break;

    case IDC_RADIO_HYPERLOBBY_SELECTOR:
        SetStatusBar(2, TRUE, GetSysColor(COLOR_BTNTEXT), RGB(255, 255, 0), TRANSPARENT, L"Settings Changed");
        break;

    case IDC_RADIO_MEM_BALANCED:
        g_iMemStrategy = MEM_STRATEGY_BALANCED;
        SetStatusBar(2, TRUE, GetSysColor(COLOR_BTNTEXT), RGB(255, 255, 0), TRANSPARENT, L"Settings Changed");
        break;

    case IDC_RADIO_MEM_CONSERVATIVE:
        g_iMemStrategy = MEM_STRATEGY_CONSERVATIVE;
        SetStatusBar(2, TRUE, GetSysColor(COLOR_BTNTEXT), RGB(255, 255, 0), TRANSPARENT, L"Settings Changed");
        break;

    case IDC_RADIO_MEM_HEAPONLY:
        g_iMemStrategy = MEM_STRATEGY_HEAPONLY;
        SetStatusBar(2, TRUE, GetSysColor(COLOR_BTNTEXT), RGB(255, 255, 0), TRANSPARENT, L"Settings Changed");
        break;

	case IDC_CHECK_SPLASH_SHOW:
		if (IsDlgButtonChecked(g_hWnd, IDC_CHECK_SPLASH_SHOW))
			g_iSplashScreenMode |= SPLASH_SCREEN_VISIBLE;
		else {
			g_iSplashScreenMode &= ~SPLASH_SCREEN_VISIBLE;
		}
		SettingsToControls();
		break;

	case IDC_CHECK_SPLASH_TOPMOST:
		if (IsDlgButtonChecked(g_hWnd, IDC_CHECK_SPLASH_TOPMOST))
			g_iSplashScreenMode |= SPLASH_SCREEN_TOPMOST;
		else
			g_iSplashScreenMode &= ~SPLASH_SCREEN_TOPMOST;
		SettingsToControls();
		break;

    case IDC_CHECK_DUMP_FILES:
        if (IsDlgButtonChecked(g_hWnd, IDC_CHECK_DUMP_FILES))
            g_iDumpMode |= DUMP_MODE_DUMP_FILES;
        else
            g_iDumpMode &= ~DUMP_MODE_DUMP_FILES;
        SettingsToControls();
        break;

    case IDC_CHECK_LOG_SFS_ACCESS:
        if (IsDlgButtonChecked(g_hWnd, IDC_CHECK_LOG_SFS_ACCESS))
            g_iDumpMode |= DUMP_MODE_SFS_ACCESS;
        else
            g_iDumpMode &= ~DUMP_MODE_SFS_ACCESS;
        SettingsToControls();
        break;

    default:
        break;
    }
}

//************************************
// Method:    SASES_OnTimer
// FullName:  SASES_OnTimer
// Access:    public
// Returns:   void
// Qualifier:
// Parameter: HWND hwnd
// Parameter: UINT_PTR nIDEvent
//************************************
void SASES_OnTimer(HWND hwnd, UINT_PTR nIDEvent)
{
    switch(nIDEvent) {
    case TIMER_REENABLE_START: {
        KillTimer(hwnd, TIMER_REENABLE_START);

        if(g_bMultipleInstancesEnabled) {
            EnableWindow(GetDlgItem(g_hWnd, IDC_BUTTON_START_NOW), TRUE);
        }
    }
    break;

    default:
        break;
    }
}
//************************************
// Method:    SASES_OnHScroll
// FullName:  SASES_OnHScroll
// Access:    public
// Returns:   void
// Qualifier:
// Parameter: HWND hwnd
// Parameter: HWND hwndCtl
// Parameter: UINT code
// Parameter: int pos
//************************************
void SASES_OnHScroll(HWND hwnd, HWND hwndCtl, UINT code, int pos)
{
    if(hwndCtl == GetDlgItem(g_hWnd, IDC_SLIDER_RAMSIZE)) {
        switch LOWORD(code) {
        case TB_PAGEUP:
        case TB_PAGEDOWN:
            pos = SendMessage(GetDlgItem(g_hWnd, IDC_SLIDER_RAMSIZE), TBM_GETPOS, 0, 0);

        case TB_LINEUP:
        case TB_LINEDOWN:
        case TB_THUMBPOSITION:
        case TB_THUMBTRACK:
        case TB_TOP:
        case TB_BOTTOM:
        {
            g_iRamSize = stepRamSize(pos);
            SendMessage(GetDlgItem(g_hWnd, IDC_SLIDER_RAMSIZE), TBM_SETPOS, (WPARAM) TRUE, (LPARAM) g_iRamSize);
            SendMessage(GetDlgItem(g_hWnd, IDC_SLIDER_RAMSIZE), TBM_SETSEL, (WPARAM) TRUE, (LPARAM) MAKELONG(0, g_iRamSize));

            if(GetFocus() == GetDlgItem(hwnd, IDC_EDIT_RAM)) {
                break;
            }

            SetDlgItemInt(g_hWnd, IDC_EDIT_RAM, g_iRamSize, FALSE);
        }
        break;

        case TB_ENDTRACK:
            CheckRamUsage();
            break;

        default:
            TRACE(L"code %d, pos %d\r\n", code, pos);
            break;
        }
    }
}

//************************************
// Method:    SASES_OnCtlColorEdit
// FullName:  SASES_OnCtlColorEdit
// Access:    public
// Returns:   HBRUSH
// Qualifier:
// Parameter: HWND hwnd
// Parameter: HDC hdc
// Parameter: HWND hwndChild
// Parameter: int type
//************************************
HBRUSH SASES_OnCtlColorEdit(HWND hwnd, HDC hdc, HWND hwndChild, int type)
{
    if(hwndChild != GetDlgItem(g_hWnd, IDC_EDIT_RAM)) {
        return NULL;
    }

    SetRAMStatus();

    switch(type) {
    case CTLCOLOR_EDIT: {
        SetTextColor(hdc, RGB(0, 0, 0));

        if(g_iRamSize <= 512) {
            SetBkColor(hdc, RGB(0, 255, 0));
            return g_hBrushGreen;
        }

        if(g_iRamSize <= 1024) {
            SetBkColor(hdc, RGB(154, 205, 50));
            return g_hBrushYellowGreen;
        }

        if(g_iRamSize <= 1200) {
            SetBkColor(hdc, RGB(255, 255, 0));
            return g_hBrushYellow;
        }

        MEMORYSTATUSEX status;
        status.dwLength = sizeof(status);
        GlobalMemoryStatusEx(&status);
        int iMegsFree = (int)(status.ullAvailVirtual / 0x100000);

        if(g_iRamSize < iMegsFree) {
            SetBkColor(hdc, RGB(255, 140, 0));
            return g_hBrushOrange;
        }

        SetBkColor(hdc, RGB(255, 0, 0));
        return g_hBrushRed;
    }

    default:
        break;
    }

    return NULL;
}

//************************************
// Method:    SASES_OnDrawItem
// FullName:  SASES_OnDrawItem
// Access:    public
// Returns:   void
// Qualifier:
// Parameter: HWND hwndDlg
// Parameter: const DRAWITEMSTRUCT * lpDIS
//************************************
void SASES_OnDrawItem(HWND hwndDlg, const DRAWITEMSTRUCT* lpDIS)
{
    PTSTR ptStr;
    RECT itemRect;
    LPSTATUSITEM pstatusItem;
    pstatusItem = (LPSTATUSITEM)lpDIS->itemData;
    ptStr = (PTSTR)pstatusItem->lpItemText;
    SendMessage(g_hWndStatus, SB_GETRECT, (WPARAM)lpDIS->itemID, (LPARAM)&itemRect);

    if(pstatusItem->bEnabled) {
        SetTextColor(lpDIS->hDC, pstatusItem->crForeground);
        SetBkMode(lpDIS->hDC, pstatusItem->iBackgroundMode);
        SetBkColor(lpDIS->hDC, pstatusItem->crBackground);
    } else {
        SetTextColor(lpDIS->hDC, GetSysColor(COLOR_GRAYTEXT));
        SetBkMode(lpDIS->hDC, TRANSPARENT);
        SetBkColor(lpDIS->hDC, GetSysColor(COLOR_BTNFACE));
    }

    ExtTextOut(lpDIS->hDC, itemRect.left, itemRect.top, ETO_OPAQUE, &lpDIS->rcItem , ptStr, _tcslen(ptStr), NULL);
}

//************************************
// Method:    SASES_OnIl2Started
// FullName:  SASES_OnIl2Started
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void SASES_OnIl2Started()
{
    g_iNumIl2InstancesRunning++;

    if(g_bExitWithIl2 || !g_bMultipleInstancesEnabled) {
        ShowWindow(g_hWnd, SW_HIDE);
    }
}

//************************************
// Method:    SASES_OnIl2Stopped
// FullName:  SASES_OnIl2Stopped
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void SASES_OnIl2Stopped()
{
    if(g_iNumIl2InstancesRunning > 0) {
        g_iNumIl2InstancesRunning--;
    }

    AfterIl2Stopped();
}

//************************************
// Method:    SASES_OnIl2Loaded
// FullName:  SASES_OnIl2Loaded
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void SASES_OnIl2Loaded()
{
    //PostMessage(g_hWnd, WM_IL2_STARTED, 0, 0);
    //DeleteFile(szIl2CreatedFilePath);
}
//************************************
// Method:    SASES_OnIl2StartError
// FullName:  SASES_OnIl2StartError
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void SASES_OnIl2StartError()
{
    AfterIl2Stopped();
}

//************************************
// Method:    SASES_OnIl2InsufficientRam
// FullName:  SASES_OnIl2InsufficientRam
// Access:    public
// Returns:   void
// Qualifier:
// Parameter: int iMessageType
//************************************
void SASES_OnIl2InsufficientRam(int iMessageType)
{
    SetRAMStatus();

    switch(iMessageType) {
    case RAM_MESSAGE_GOOD: {
        if(!g_bRamToolTipVisible) {
            break;
        }

        Edit_HideBalloonTip(GetDlgItem(g_hWnd, IDC_EDIT_RAM));
        g_bRamToolTipVisible = FALSE;
    }
    break;

    case RAM_MESSAGE_CRITICAL: {
        if(g_bRamToolTipVisible) {
            if(g_iMessageType == RAM_MESSAGE_CRITICAL) {
                break;
            }

            Edit_HideBalloonTip(GetDlgItem(g_hWnd, IDC_EDIT_RAM));
            Sleep(0);
        }

        EDITBALLOONTIP editballoontip;
        editballoontip.cbStruct = sizeof(EDITBALLOONTIP);
        editballoontip.ttiIcon = TTI_WARNING;
        editballoontip.pszTitle = L"! WARNING !";
        editballoontip.pszText = L"Your RAM usage selection exceeds\r\n"
                                 L"critical limits. Please be prepared that\r\n"
                                 L"IL-2 might not launch successfully with\r\n"
                                 L"this setting.";
        SendMessage(GetDlgItem(g_hWnd, IDC_EDIT_RAM), EM_SHOWBALLOONTIP, 0, (LPARAM)&editballoontip);
        g_bRamToolTipVisible = TRUE;
    }
    break;

    case RAM_MESSAGE_ERROR: {
        if(g_bRamToolTipVisible) {
            if(g_iMessageType == RAM_MESSAGE_ERROR) {
                break;
            }

            Edit_HideBalloonTip(GetDlgItem(g_hWnd, IDC_EDIT_RAM));
            Sleep(0);
        }

        EDITBALLOONTIP editballoontip;
        editballoontip.cbStruct = sizeof(EDITBALLOONTIP);
        editballoontip.ttiIcon = TTI_WARNING;
        editballoontip.pszTitle = L"*** !!! ERROR !!! ***";
        editballoontip.pszText = L"Your RAM usage selection exceeds\r\n"
                                 L"the currently available virtual memory\r\n"
                                 L"on your system. Please be prepared that\r\n"
                                 L"IL-2 will not launch successfully with\r\n"
                                 L"this setting.";
        SendMessage(GetDlgItem(g_hWnd, IDC_EDIT_RAM), EM_SHOWBALLOONTIP, 0, (LPARAM)&editballoontip);
        g_bRamToolTipVisible = TRUE;
    }
    break;

    default:
        break;
    }

    g_iMessageType = iMessageType;
}

