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
#include <io.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include <commctrl.h>
#include "common.h"
#include "extern_globals.h"
#include "trace.h"
#include <Psapi.h>
#pragma comment(lib, "Psapi.lib")
#include <Tlhelp32.h>
#include "Crc32Static.h"

#include "winver.h"
#pragma comment(lib, "version.lib")

#pragma comment(lib, "comctl32.lib")
#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable : 4996 )

//************************************
// Method:    StartIl2Thread
// FullName:  StartIl2Thread
// Access:    public
// Returns:   UINT
// Qualifier:
// Parameter: LPVOID pParam
//************************************
UINT StartIl2Thread(LPVOID pParam)
{
    TRACE(L"start\r\n");
    STARTUPINFO si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));
    TCHAR szCommandLine[0x8000];

    if(_tcslen(g_lpCmdLine) > 0) {
        if((g_szModTypeExeParms[g_iModType] == NULL) || (_tcslen(g_szModTypeExeParms[g_iModType]) == 0)) {
            _stprintf(szCommandLine, L"\"%s\" %s", szLauncherFile, g_lpCmdLine);
        } else {
            _stprintf(szCommandLine, L"\"%s\" %s %s", szLauncherFile, g_lpCmdLine, g_szModTypeExeParms[g_iModType]);
        }
    } else {
        if(g_szModTypeExeParms[g_iModType] == NULL) {
            _stprintf(szCommandLine, L"\"%s\"", szLauncherFile);
        } else {
            _stprintf(szCommandLine, L"\"%s\" %s", szLauncherFile, g_szModTypeExeParms[g_iModType]);
        }
    }

    SetCurrentDirectory(szGamePath);
    TRACE(L"CreateProcess\r\n");

    if(CreateProcess(NULL, szCommandLine, NULL,
                     NULL, FALSE, CREATE_NEW_CONSOLE, NULL, szGamePath, &si, &pi)) {
        TRACE(L"Process created\r\n");
        TRACE(L"PostMessage\r\n");
        PostMessage(g_hWnd, WM_IL2_STARTED, 0, 0);
        TRACE(L"SetTimer\r\n");
        SetTimer(g_hWnd, TIMER_REENABLE_START, 5000, NULL);
        TRACE(L"CloseHandle\r\n");
        CloseHandle(pi.hThread);
        TRACE(L"WaitForSingleObject\r\n");
        WaitForSingleObject(pi.hProcess, INFINITE);
        TRACE(L"CloseHandle\r\n");
        CloseHandle(pi.hProcess);
        TRACE(L"PostMessage\r\n");
        PostMessage(g_hWnd, WM_IL2_STOPPED, 0, 0);
    } else {
        TRACE(L"Process not created\r\n");
        TRACE(L"PostMessage\r\n");
        PostMessage(g_hWnd, WM_IL2_START_ERROR, 0, 0);
        TRACE(L"ShowLastError\r\n");
        ShowLastError();
    }

    TRACE(L"finished.\r\n");
    return 0;
}

//************************************
// Method:    GetFilesAndPaths
// FullName:  GetFilesAndPaths
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void GetFilesAndPaths()
{
    TCHAR szBuf[MAX_PATH];
	ZeroMemory(szSelectorPath, sizeof(szSelectorPath));
	ZeroMemory(szGamePath, sizeof(szGamePath));
    GetModuleFileName(NULL, szAppName, MAX_PATH);
    _tcscpy(szBuf, szAppName);
    _tcscpy(szSelectorPath, szAppName);
    // allocate a block of memory for the version info
    DWORD dummy;
    DWORD dwSize = GetFileVersionInfoSize(szAppName, &dummy);

    if(dwSize != 0) {
        BYTE *data = (BYTE*)malloc(dwSize);

        // load the version info
        if(GetFileVersionInfo(szAppName, NULL, dwSize, &data[0])) {
            // get the name and version strings
            LPVOID pvProductName = NULL;
            unsigned int iProductNameLen = 0;
            LPVOID pvProductVersion = NULL;
            unsigned int iProductVersionLen = 0;

            // replace "040904e4" with the language ID of your resources
            if(VerQueryValue(&data[0], _T("\\StringFileInfo\\000004b0\\ProductName"), &pvProductName, &iProductNameLen) &&
                    VerQueryValue(&data[0], _T("\\StringFileInfo\\000004b0\\ProductVersion"), &pvProductVersion, &iProductVersionLen)) {
                _tcscpy(szWindowTitle, L"IL-2 Selector ");
                _tcsncat(szWindowTitle, (LPCTSTR)pvProductVersion, iProductVersionLen);
                _tcscat(szWindowTitle, L" by SAS");
            }
        }
    }

    LPTSTR pszFileName = _tcsrchr(szBuf, '\\') + 1;
    *pszFileName = '\0';
    _tcscpy(szSelectorPath, szBuf);
    pszFileName = _tcsrchr(szBuf, '\\');
    *pszFileName = '\0';
    pszFileName = _tcsrchr(szBuf, '\\');
    *pszFileName = '\0';
    pszFileName = _tcsrchr(szBuf, '\\') + 1;
    *pszFileName = '\0';
    _tcscpy(szGamePath, szBuf);
    _tcscpy(szIniFile, szGamePath);
    _tcscat(szIniFile, L"il2fb.ini");
    _tcscpy(szLauncherFile, szGamePath);
    _tcscat(szLauncherFile, IL2_EXE_NAME);
    _tcscpy(szWrapperFile, szGamePath);
    _tcscat(szWrapperFile, WRAPPER_DLL_NAME);
    _tcscpy(szDinputFile, szGamePath);
    _tcscat(szDinputFile, DINPUT_DLL_NAME);
    _tcscpy(szLauncherModBaseFile, szSelectorPath);
    _tcscat(szLauncherModBaseFile, IL2_BASEFILES);
    _tcscat(szLauncherModBaseFile, IL2FB_EXE_MOD_NAME);
    _tcscpy(szLauncherStockBaseFile, szSelectorPath);
    _tcscat(szLauncherStockBaseFile, IL2_BASEFILES);
    _tcscat(szLauncherStockBaseFile, IL2FB_EXE_STOCK_NAME);
    _tcscpy(szWrapperBaseFile, szSelectorPath);
    _tcscat(szWrapperBaseFile, IL2_BASEFILES);
    _tcscat(szWrapperBaseFile, WRAPPER_DLL_MOD_NAME);
    _tcscpy(szDinputBaseFile, szSelectorPath);
    _tcscat(szDinputBaseFile, DINPUT_BASE_NAME);
    _stprintf(szHyperlobbyIniFile, L"%s\\%s", _wgetenv(L"AppData"), HYPERLOBBY_INI);
}

//************************************
// Method:    LaunchIl2
// FullName:  LaunchIl2
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void LaunchIl2()
{
    EnableSettingChanges(FALSE);

    if(g_iNumIl2InstancesRunning == 0) {
        CreateIl2FbExe();
    }

    RunWorker(StartIl2Thread);
    EnableWindow(GetDlgItem(g_hWnd, IDC_BUTTON_START_NOW), FALSE);
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

//************************************
// Method:    IsKeyDown
// FullName:  IsKeyDown
// Access:    public
// Returns:   BOOL
// Qualifier:
// Parameter: int vKey
//************************************
BOOL IsKeyDown(int vKey)
{
    return (GetAsyncKeyState(vKey) & 0x8000);
}


//************************************
// Method:    CheckCreateFile
// FullName:  CheckCreateFile
// Access:    public
// Returns:   void
// Qualifier:
// Parameter: LPCTSTR lpCheckFile
// Parameter: LPCTSTR lpBaseFile
//************************************
void CheckCreateFile(LPCTSTR lpCheckFile, LPCTSTR lpBaseFile)
{
    CCrc32Static* theCRC = new CCrc32Static();
    BOOL bNeedNewFile = TRUE;
    DWORD dwCRCBaseFile = 0;
    DWORD dwCRCCheckFile = 0;

    do {
        if(theCRC->FileCrc32Win32(lpCheckFile, dwCRCCheckFile) != NO_ERROR) {
            break;
        }

        if(theCRC->FileCrc32Win32(lpBaseFile, dwCRCBaseFile) != NO_ERROR) {
            break;
        }

        if(dwCRCBaseFile == dwCRCCheckFile) {
            bNeedNewFile = FALSE;
        }
    } while(FALSE);

    if(bNeedNewFile) {
        SecureCopyFile(lpBaseFile, lpCheckFile);
    }

    theCRC->~CCrc32Static();
}
//************************************
// Method:    CreateIl2FbExe
// FullName:  CreateIl2FbExe
// Access:    public
// Returns:   void
// Qualifier:
//************************************
void CreateIl2FbExe()
{
    CheckCreateFile(szLauncherFile, g_szModTypeIl2fbExe[g_iModType]);
    CheckCreateFile(szDinputFile, szDinputBaseFile);
    CheckCreateFile(szWrapperFile, g_szModTypeWrapperDll[g_iModType]);
}

//************************************
// Method:    IsFolderWriteable
// FullName:  IsFolderWriteable
// Access:    public
// Returns:   BOOL
// Qualifier:
//************************************
BOOL IsFolderWriteable()
{
    return (IsFileWriteable(szIniFile));
}

//************************************
// Method:    IsFileWriteable
// FullName:  IsFileWriteable
// Access:    public
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR szFilePath
//************************************
BOOL IsFileWriteable(LPCTSTR szFilePath)
{
    BOOL bRetVal = TRUE;
    FILE* fTest;
    fTest = _wfopen(szFilePath, L"a+");

    if(fTest == NULL) {
        bRetVal = FALSE;
    } else {
        fclose(fTest);
    }

    return bRetVal;
}

//************************************
// Method:    SetFileAttributesWithRetry
// FullName:  SetFileAttributesWithRetry
// Access:    public
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR lpFileName
// Parameter: DWORD dwFileAttributes
// Parameter: int iAttempts
//************************************
BOOL SetFileAttributesWithRetry(LPCTSTR lpFileName, DWORD dwFileAttributes, int iAttempts)
{
    if(GetFileAttributes(lpFileName) == INVALID_FILE_ATTRIBUTES) {
        return FALSE;
    }

    while(!SetFileAttributes(lpFileName, dwFileAttributes) && (iAttempts > 0)) {
        iAttempts--;
        TRACE(L"SetFileAttributes(%s, %08X) failed, %d attempts left\r\n", lpFileName, dwFileAttributes, iAttempts);
        Sleep(100);
    }

    return (iAttempts > 0);
}

//************************************
// Method:    DeleteFileWithRetry
// FullName:  DeleteFileWithRetry
// Access:    public
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR lpFileName
// Parameter: int iAttempts
//************************************
BOOL DeleteFileWithRetry(LPCTSTR lpFileName, int iAttempts)
{
    if(GetFileAttributes(lpFileName) == INVALID_FILE_ATTRIBUTES) {
        return TRUE;
    }

    while(!DeleteFile(lpFileName) && (iAttempts > 0)) {
        iAttempts--;
        TRACE(L"DeleteFile(%s) failed, %d attempts left\r\n", lpFileName, iAttempts);
        Sleep(100);
    }

    return (iAttempts > 0);
}

//************************************
// Method:    MoveFileWithRetry
// FullName:  MoveFileWithRetry
// Access:    public
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR lpExistingFileName
// Parameter: LPCTSTR lpNewFileName
// Parameter: int iAttempts
//************************************
BOOL MoveFileWithRetry(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, int iAttempts)
{
    if(GetFileAttributes(lpExistingFileName) == INVALID_FILE_ATTRIBUTES) {
        return FALSE;
    }

    while(!MoveFile(lpExistingFileName, lpNewFileName) && (iAttempts > 0)) {
        iAttempts--;
        TRACE(L"MoveFile(%s, %s) failed, %d attempts left\r\n", lpExistingFileName, lpNewFileName, iAttempts);
        Sleep(100);
    }

    return (iAttempts > 0);
}

//************************************
// Method:    CopyMoveFileWithRetry
// FullName:  CopyMoveFileWithRetry
// Access:    public
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR lpExistingFileName
// Parameter: LPCTSTR lpNewFileName
// Parameter: int iAttempts
//************************************
BOOL CopyMoveFileWithRetry(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, int iAttempts)
{
    int iSafeAttempts = iAttempts;

    if(GetFileAttributes(lpExistingFileName) == INVALID_FILE_ATTRIBUTES) {
        return FALSE;
    }

    while(!CopyFile(lpExistingFileName, lpNewFileName, FALSE) && (iAttempts > 0)) {
        iAttempts--;
        TRACE(L"MoveFile(%s, %s) failed, %d attempts left\r\n", lpExistingFileName, lpNewFileName, iAttempts);
        Sleep(100);
    }

    iAttempts = iSafeAttempts;

    while(!DeleteFile(lpExistingFileName) && (iAttempts > 0)) {
        iAttempts--;
        TRACE(L"MoveFile(%s, %s) failed, %d attempts left\r\n", lpExistingFileName, lpNewFileName, iAttempts);
        Sleep(100);
    }

    return (iAttempts > 0);
}
//************************************
// Method:    CopyFileWithRetry
// FullName:  CopyFileWithRetry
// Access:    public
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR lpExistingFileName
// Parameter: LPCTSTR lpNewFileName
// Parameter: int iAttempts
//************************************
BOOL CopyFileWithRetry(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName, int iAttempts)
{
    if(GetFileAttributes(lpExistingFileName) == INVALID_FILE_ATTRIBUTES) {
        return FALSE;
    }

    while(!CopyFile(lpExistingFileName, lpNewFileName, FALSE) && (iAttempts > 0)) {
        iAttempts--;
        TRACE(L"CopyFile(%s, %s) failed, %d attempts left\r\n", lpExistingFileName, lpNewFileName, iAttempts);
        Sleep(100);
    }

    return (iAttempts > 0);
}

//************************************
// Method:    SecureDeleteFile
// FullName:  SecureDeleteFile
// Access:    public
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR lpFileName
//************************************
BOOL SecureDeleteFile(LPCTSTR lpFileName)
{
    if(GetFileAttributes(lpFileName) == INVALID_FILE_ATTRIBUTES) {
        return TRUE;
    }

    SecureSetFileAttributes(lpFileName, FILE_ATTRIBUTE_NORMAL);
    return DeleteFileWithRetry(lpFileName, FILE_OPERATION_RETRIES);
}

//************************************
// Method:    SecureSetFileAttributes
// FullName:  SecureSetFileAttributes
// Access:    public
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR lpFileName
// Parameter: DWORD dwFileAttributes
//************************************
BOOL SecureSetFileAttributes(LPCTSTR lpFileName, DWORD dwFileAttributes)
{
    if(GetFileAttributes(lpFileName) == INVALID_FILE_ATTRIBUTES) {
        return FALSE;
    }

    return SetFileAttributesWithRetry(lpFileName, dwFileAttributes, FILE_OPERATION_RETRIES);
}

//************************************
// Method:    SecureMoveFile
// FullName:  SecureMoveFile
// Access:    public
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR lpExistingFileName
// Parameter: LPCTSTR lpNewFileName
//************************************
BOOL SecureMoveFile(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName)
{
    if(GetFileAttributes(lpExistingFileName) == INVALID_FILE_ATTRIBUTES) {
        return FALSE;
    }

    SecureSetFileAttributes(lpNewFileName, FILE_ATTRIBUTE_NORMAL);
    SecureDeleteFile(lpNewFileName);
    return MoveFileWithRetry(lpExistingFileName, lpNewFileName, FILE_OPERATION_RETRIES);
}

//************************************
// Method:    SecureCopyMoveFile
// FullName:  SecureCopyMoveFile
// Access:    public
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR lpExistingFileName
// Parameter: LPCTSTR lpNewFileName
//************************************
BOOL SecureCopyMoveFile(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName)
{
    if(GetFileAttributes(lpExistingFileName) == INVALID_FILE_ATTRIBUTES) {
        return FALSE;
    }

    SecureSetFileAttributes(lpNewFileName, FILE_ATTRIBUTE_NORMAL);
    SecureDeleteFile(lpNewFileName);
    return CopyMoveFileWithRetry(lpExistingFileName, lpNewFileName, FILE_OPERATION_RETRIES);
}
//************************************
// Method:    SecureCopyFile
// FullName:  SecureCopyFile
// Access:    public
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR lpExistingFileName
// Parameter: LPCTSTR lpNewFileName
//************************************
BOOL SecureCopyFile(LPCTSTR lpExistingFileName, LPCTSTR lpNewFileName)
{
    if(GetFileAttributes(lpExistingFileName) == INVALID_FILE_ATTRIBUTES) {
        return FALSE;
    }

    SecureSetFileAttributes(lpNewFileName, FILE_ATTRIBUTE_NORMAL);
    SecureDeleteFile(lpNewFileName);
    return CopyFileWithRetry(lpExistingFileName, lpNewFileName, FILE_OPERATION_RETRIES);
}

//************************************
// Method:    FileExists
// FullName:  FileExists
// Access:    public
// Returns:   BOOL
// Qualifier:
// Parameter: LPCTSTR lpFileName
//************************************
BOOL FileExists(LPCTSTR lpFileName)
{
    return (GetFileAttributes(lpFileName) != (DWORD) - 1);
}
