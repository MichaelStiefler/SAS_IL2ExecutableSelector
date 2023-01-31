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

#include "stdafx.h"
#ifdef _DEBUG
#define LOG_ENABLED
#pragma warning( disable : 4996 )
#define _CRT_SECURE_NO_WARNINGS
#include <stdlib.h>
#include <stdio.h>
void Proto(LPCTSTR szLine);

//************************************
// Method:    _trace
// FullName:  _trace
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: TCHAR * format
// Parameter: ...
//************************************
bool _trace(TCHAR *format, ...)
{
    TCHAR buffer[1000];
    va_list argptr;
    va_start(argptr, format);
    wvsprintf(buffer, format, argptr);
    va_end(argptr);
    _tprintf(buffer);
    OutputDebugString(buffer);
    Proto(buffer);
    return true;
}

//CRITICAL_SECTION	myCriticalSection;
extern TCHAR szSelectorPath[MAX_PATH];

//************************************
// Method:    Proto
// FullName:  Proto
// Access:    public 
// Returns:   void
// Qualifier:
// Parameter: LPCTSTR szLine
//************************************
void Proto(LPCTSTR szLine)
{
#ifndef LOG_ENABLED
    return;
#endif
    //EnterCriticalSection(&myCriticalSection);
    char* cLine = new char[MAX_PATH];
    char* cTimeStamp = new char[MAX_PATH];
    wcstombs(cLine, szLine, MAX_PATH);
    TCHAR szLogFile[MAX_PATH];
    _tcscpy(szLogFile, szSelectorPath);
    _tcscat(szLogFile, L"il2fb.log");
    /////////////////////////////////////////////////////////////////////////
    // Open Protocol file.
    /////////////////////////////////////////////////////////////////////////
    HANDLE hFile = CreateFile(szLogFile,
                              GENERIC_READ | GENERIC_WRITE,
                              FILE_SHARE_READ | FILE_SHARE_WRITE,
                              NULL,
                              OPEN_ALWAYS,
                              FILE_ATTRIBUTE_NORMAL,
                              NULL);

    if(hFile == INVALID_HANDLE_VALUE) {
        return;
    }

    /////////////////////////////////////////////////////////////////////////
    // Append to existing Protocol
    /////////////////////////////////////////////////////////////////////////
    SetFilePointer(hFile, 0, NULL, FILE_END);
    DWORD dwBytesWritten = 0;
    SYSTEMTIME stSystemTime;
    /////////////////////////////////////////////////////////////////////////
    // Generate Time Stamp
    /////////////////////////////////////////////////////////////////////////
    GetLocalTime(&stSystemTime);
    sprintf(cTimeStamp,
            "%02d.%02d.%04d %02d:%02d:%02d : ",
            stSystemTime.wDay,
            stSystemTime.wMonth,
            stSystemTime.wYear,
            stSystemTime.wHour,
            stSystemTime.wMinute,
            stSystemTime.wSecond);
    /////////////////////////////////////////////////////////////////////////
    // Write Time Stamp and Information and Close Protocol file afterwards
    /////////////////////////////////////////////////////////////////////////
    WriteFile(hFile, cTimeStamp, strlen(cTimeStamp), &dwBytesWritten, NULL);
    WriteFile(hFile, cLine, strlen(cLine), &dwBytesWritten, NULL);
    CloseHandle(hFile);
    delete [] cLine;
    delete [] cTimeStamp;
}
#endif