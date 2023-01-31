//*****************************************************************
// DINPUT.dll - JVM Parameter parser and il2fb.exe modifier
// Copyright (C) 2021 SAS~Storebror
//
// This file is part of DINPUT.dll.
//
// DINPUT.dll is free software.
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
#include <stdlib.h>
#include <stdio.h>
#include <tchar.h>
#include "trace.h"
#include <share.h>
#include "_dinput.h"
#include "globals.h"

#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable : 4996 )


//************************************
// Method:    DllMain
// FullName:  DllMain
// Access:    public
// Returns:   BOOL APIENTRY
// Qualifier:
// Parameter: HMODULE hModule
// Parameter: DWORD ul_reason_for_call
// Parameter: LPVOID lpReserved
//************************************
BOOL APIENTRY DllMain(HMODULE hModule,
                      DWORD  ul_reason_for_call,
                      LPVOID lpReserved
                     )
{
    switch(ul_reason_for_call) {
    case DLL_PROCESS_ATTACH:
        hDInputSelf = hModule;
        if(hDInput == NULL && !g_bProcessAttached) {
			ZeroMemory(szCurDir, sizeof(szCurDir));
			ZeroMemory(cCurDir, sizeof(cCurDir));
			ZeroMemory(szIniFile, sizeof(szIniFile));
			ZeroMemory(szConfIniFile, sizeof(szConfIniFile));

			GetModuleFileName(NULL, szCurDir, MAX_PATH);
            LPTSTR pszFileName = NULL;
            pszFileName = _tcsrchr(szCurDir, '\\') + 1;
            *pszFileName = '\0';
			wcstombs(cCurDir, szCurDir, _tcslen(szCurDir));

            _stprintf(LogFileName, L"%s%s", szCurDir, LOGFILE_NAME);
            DeleteFile(LogFileName);

            if(IsServerExe()) {
                _stprintf(szIniFile, L"%s%s", szCurDir, SERVER_INI);
            } else {
                _stprintf(szIniFile, L"%s%s", szCurDir, CLIENT_INI);
				_stprintf(szConfIniFile, L"%s%s", szCurDir, CLIENT_CONF_INI);
			}

            wcstombs(cIniFile, szIniFile, MAX_PATH);
        }

        TRACE(L"JVM Parameters injector activated\r\n");

        if(!g_bProcessAttached) {
            g_bProcessAttached = TRUE;
			AdjustJvmParams();
        }

        break;

    case DLL_THREAD_ATTACH:
        break;

    case DLL_THREAD_DETACH:
        break;

    case DLL_PROCESS_DETACH:
        TRACE(L"JVM Parameters injector deactivated\r\n");

        if(hDInput != NULL) {
            FreeLibrary(hDInput);
            hDInput = NULL;
		}
		if (g_hJobObject != NULL) CloseHandle(g_hJobObject);
        break;
    }

    return TRUE;
}

