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

//************************************
// Method:    ReadIniSettings
// FullName:  ReadIniSettings
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void ReadIniSettings()
{
    TCHAR szSection[256];
    TCHAR szFiles[256];
    TCHAR szMods[256];
    TCHAR szIl2fbExe[256];
    TCHAR szWrapperDll[256];

    if(g_szModTypes != NULL) {
        for(int i = 0; i < g_iModTypes; i++) {
            delete[] g_szModTypes[i];
        }

        delete[] g_szModTypes;
    }

    if(g_szModTypeHelpText != NULL) {
        for(int i = 0; i < g_iModTypes; i++) {
            delete[] g_szModTypeHelpText[i];
        }

        delete[] g_szModTypeHelpText;
    }

    if(g_szModTypeExeParms != NULL) {
        for(int i = 0; i < g_iModTypes; i++) {
            delete[] g_szModTypeExeParms[i];
        }

        delete[] g_szModTypeExeParms;
    }

    g_iModTypes = GetPrivateProfileInt(L"Modtypes", L"Types", 1, szIniFile);
    g_szModTypeHelpText = new LPTSTR[g_iModTypes];
    g_szModTypes = new LPTSTR[g_iModTypes];
    g_szModTypeExeParms = new LPTSTR[g_iModTypes];
    g_szModTypeIl2fbExe = new LPTSTR[g_iModTypes];
    g_szModTypeWrapperDll = new LPTSTR[g_iModTypes];

    for(int iModType = 0; iModType < g_iModTypes; iModType++) {
        g_szModTypes[iModType] = new TCHAR[1024];
        g_szModTypeHelpText[iModType] = new TCHAR[1024];
        g_szModTypeExeParms[iModType] = new TCHAR[1024];
        g_szModTypeIl2fbExe[iModType] = new TCHAR[1024];
        g_szModTypeWrapperDll[iModType] = new TCHAR[1024];
        _stprintf(szSection, L"Modtype_%02d", iModType);
        GetPrivateProfileString(szSection, L"Name", NULL, g_szModTypes[iModType], 1024, szIniFile);
        GetPrivateProfileString(szSection, L"Help", NULL, g_szModTypeHelpText[iModType], 1024, szIniFile);
        GetPrivateProfileString(szSection, L"Files", NULL, szFiles, 256, szIniFile);
        GetPrivateProfileString(szSection, L"Mods", NULL, szMods, 256, szIniFile);
        GetPrivateProfileString(szSection, L"Il2fb", iModType==0 ? IL2FB_EXE_STOCK_NAME : IL2FB_EXE_MOD_NAME, szIl2fbExe, 256, szIniFile);
        GetPrivateProfileString(szSection, L"Wrapper", iModType == 0 ? WRAPPER_DLL_STOCK_NAME : WRAPPER_DLL_MOD_NAME, szWrapperDll, 256, szIniFile);
        TCHAR* slash_pos = _tcschr(szIl2fbExe, L'/');
        while (slash_pos) {
            *slash_pos = L'\\';
            slash_pos = _tcschr(slash_pos, L'/');
        }
        _stprintf(g_szModTypeIl2fbExe[iModType], L"%s%s%s", szSelectorPath, IL2_BASEFILES, szIl2fbExe);
        if (_tcslen(szWrapperDll) < 1) {
            g_szModTypeWrapperDll[iModType] = WRAPPER_DLL_STOCK_NAME;
        }
        else {
            slash_pos = _tcschr(szWrapperDll, L'/');
            while (slash_pos) {
                *slash_pos = L'\\';
                slash_pos = _tcschr(slash_pos, L'/');
            }
            _stprintf(g_szModTypeWrapperDll[iModType], L"%s%s%s", szSelectorPath, IL2_BASEFILES, szWrapperDll);
        }

        if((_tcsicmp(szFiles, L"none") == 0) && (_tcsicmp(szMods, L"none") == 0)) {
            _tcscpy(g_szModTypeExeParms[iModType], L"");
        } else {
            _stprintf(g_szModTypeExeParms[iModType], L"/f:%s /m:%s /lb:il2fb.exe", szFiles, szMods);
        }
    }

    g_iModType = GetPrivateProfileInt(L"Settings", L"ModType", 0, szIniFile);

    if(g_iModType > g_iModTypes - 1) {
        g_iModType = 0;
    }

    g_iRamSize = GetPrivateProfileInt(L"Settings", L"RamSize", 512, szIniFile);
    g_bExitWithIl2 = GetPrivateProfileInt(L"Settings", L"ExitWithIL2", 0, szIniFile) == 0 ? FALSE : TRUE;
    g_bExpertModeEnabled = GetPrivateProfileInt(L"Settings", L"ExpertMode", 0, szIniFile) == 0 ? FALSE : TRUE;
	g_iSplashScreenMode = GetPrivateProfileInt(L"Settings", L"SplashScreenMode", 3, szIniFile);
    if ( (g_iSplashScreenMode != 0) && (g_iSplashScreenMode != 1) && (g_iSplashScreenMode != 3) )
		g_iSplashScreenMode = 3;
    g_iDumpMode = GetPrivateProfileInt(L"Settings", L"DumpMode", 0, szIniFile);

    if(!g_bExpertModeEnabled) {
        g_iRamSize = stepRamSize(g_iRamSize);
        g_bEnableModFilesCache = FALSE;
        g_bMultipleInstancesEnabled = FALSE;
        g_iMemStrategy = MEM_STRATEGY_BALANCED;
    } else {
        g_bEnableModFilesCache = GetPrivateProfileInt(L"Settings", L"UseCachedFileLists", 0, szIniFile) == 0 ? FALSE : TRUE;
        g_bMultipleInstancesEnabled = GetPrivateProfileInt(L"Settings", L"MultipleInstances", 0, szIniFile) == 0 ? FALSE : TRUE;
        g_iMemStrategy = GetPrivateProfileInt(L"Settings", L"MemoryStrategy", 0, szIniFile);
    }
}
//************************************
// Method:    WritePrivateProfileInt
// FullName:  WritePrivateProfileInt
// Access:    public
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR lpAppName
// Parameter: LPCTSTR lpKeyName
// Parameter: int nInteger
// Parameter: LPCTSTR lpFileName
//************************************
BOOL WritePrivateProfileInt(LPCTSTR lpAppName, LPCTSTR lpKeyName, int nInteger, LPCTSTR lpFileName)
{
    TCHAR lpString[ 1024 ];
    wsprintf(lpString, L"%d", nInteger);
    return WritePrivateProfileString(lpAppName, lpKeyName, lpString, lpFileName);
}
//************************************
// Method:    WriteIniSettings
// FullName:  WriteIniSettings
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void WriteIniSettings()
{
//	SecureDeleteFile(szIniFile);
    WritePrivateProfileInt(L"Settings", L"ModType", g_iModType, szIniFile);
    WritePrivateProfileInt(L"Settings", L"RamSize", g_iRamSize, szIniFile);
    WritePrivateProfileInt(L"Settings", L"ExitWithIL2", g_bExitWithIl2, szIniFile);
    WritePrivateProfileInt(L"Settings", L"ExpertMode", g_bExpertModeEnabled, szIniFile);
    WritePrivateProfileInt(L"Settings", L"UseCachedFileLists", (BOOL)(g_bEnableModFilesCache && g_bExpertModeEnabled), szIniFile);
    WritePrivateProfileInt(L"Settings", L"MultipleInstances", (BOOL)(g_bMultipleInstancesEnabled && g_bExpertModeEnabled), szIniFile);
    WritePrivateProfileInt(L"Settings", L"MemoryStrategy", g_bExpertModeEnabled ? g_iMemStrategy : MEM_STRATEGY_BALANCED, szIniFile);
	WritePrivateProfileInt(L"Settings", L"SplashScreenMode", g_iSplashScreenMode, szIniFile);
    WritePrivateProfileInt(L"Settings", L"DumpMode", g_iDumpMode, szIniFile);
    WritePrivateProfileString(L"JVM2", NULL, NULL, szIniFile);
    TCHAR szJvmOptionsInFile[0xFFFF];
	ZeroMemory(szJvmOptionsInFile, sizeof(szJvmOptionsInFile));
    TCHAR szJvmOptions[0xFFFF];
	ZeroMemory(szJvmOptions, sizeof(szJvmOptions));
    GetDlgItemText(g_hWnd, IDC_EDIT_JVM_PARAMS, szJvmOptions, 0xFFFF);
    WritePrivateProfileString(L"JVM", NULL, NULL, szIniFile);
    LPTSTR lpJvmOptionsInFile = szJvmOptionsInFile;
    LPTSTR lpJvmOptionToken = _tcstok(szJvmOptions, L"\r\n");

    while(NULL != lpJvmOptionToken) {
        if(_tcslen(lpJvmOptionToken) > 0) {
            _tcscpy(lpJvmOptionsInFile, lpJvmOptionToken);
            lpJvmOptionsInFile += _tcslen(lpJvmOptionToken) + 1;
        }

        lpJvmOptionToken = _tcstok(NULL, L"\r\n");
    }

    WritePrivateProfileSection(L"JVM", szJvmOptionsInFile, szIniFile);

    try {
        if(IsDlgButtonChecked(g_hWnd, IDC_CHECK_SET_HYPERLOBBY)) {
            if(FileExists(szHyperlobbyIniFile)) {
                if(IsDlgButtonChecked(g_hWnd, IDC_RADIO_HYPERLOBBY_LAUNCHER)) {
                    WritePrivateProfileString(L"IL-2 Sturmovik 1946", L"gamepath", szGamePath, szHyperlobbyIniFile);
                } else if(IsDlgButtonChecked(g_hWnd, IDC_RADIO_HYPERLOBBY_SELECTOR)) {
                    WritePrivateProfileString(L"IL-2 Sturmovik 1946", L"gamepath", szSelectorPath, szHyperlobbyIniFile);
                }
            }
        }
    } catch(...) {}
}
