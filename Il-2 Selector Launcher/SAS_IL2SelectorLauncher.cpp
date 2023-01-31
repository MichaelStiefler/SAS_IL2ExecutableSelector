//*****************************************************************
// IL-2 Selector.exe - IL-2 Selector Launcher
// Copyright (C) 2021 SAS~Storebror
//
// This file is part of IL-2 Selector.exe.
//
// IL-2 Selector.exe is free software.
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
#include "SAS_IL2SelectorLauncher.h"
#include "trace.h"

#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable : 4996 )

void ShowLastError();

//************************************
// Method:    _tWinMain
// FullName:  _tWinMain
// Access:    public
// Returns:   int WINAPI
// Qualifier:
// Parameter: HINSTANCE hInstance
// Parameter: HINSTANCE hPrevInstance
// Parameter: LPWSTR lpCmdLine
// Parameter: int nShowCmd
//************************************
int WINAPI _tWinMain(
    HINSTANCE hInstance,
    HINSTANCE hPrevInstance,
    LPWSTR lpCmdLine,
    int nShowCmd)
{
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    TCHAR szSelectorPath[MAX_PATH];
    TCHAR szSelectorFile[MAX_PATH];
	ZeroMemory(szSelectorPath, sizeof(szSelectorPath));
	ZeroMemory(szSelectorFile, sizeof(szSelectorFile));
    GetModuleFileName(NULL, szSelectorPath, MAX_PATH);
    LPTSTR pszFileName = _tcsrchr(szSelectorPath, '\\') + 1;
    *pszFileName = '\0';
    _tcscat(szSelectorPath, L"bin\\selector");
    _tcscpy(szSelectorFile, szSelectorPath);
    _tcscat(szSelectorFile, L"\\il2fb.exe");
    SetCurrentDirectory(szSelectorPath);

    if(CreateProcess(NULL, szSelectorFile, NULL,
                     NULL, FALSE, CREATE_NEW_CONSOLE, NULL, szSelectorPath, &si, &pi)) {
        CloseHandle(pi.hThread);
        CloseHandle(pi.hProcess);
    } else {
        TRACE(L"Process not created\r\n");
        TRACE(L"ShowLastError\r\n");
        ShowLastError();
    }

    return 0;
}

//************************************
// Method:    ShowLastError
// FullName:  ShowLastError
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void ShowLastError()
{
    LPVOID lpMsgBuf;
    FormatMessage(
        FORMAT_MESSAGE_ALLOCATE_BUFFER |
        FORMAT_MESSAGE_FROM_SYSTEM |
        FORMAT_MESSAGE_IGNORE_INSERTS,
        NULL,
        GetLastError(),
        0, // Default language
        (LPTSTR) &lpMsgBuf,
        0,
        NULL
    );
    // Process any inserts in lpMsgBuf.
    // ...
    // Display the string.
    MessageBox(NULL, (LPCTSTR)lpMsgBuf, L"SAS IL-2 Executable Selector Error", MB_OK | MB_ICONINFORMATION | MB_TOPMOST);
    // Free the buffer.
    LocalFree(lpMsgBuf);
}
