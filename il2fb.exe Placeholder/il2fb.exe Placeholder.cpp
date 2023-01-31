//*****************************************************************
// il2fb.exe - Placeholder for IL-2 1946 executable il2fb.exe
// Copyright (C) 2021 SAS~Storebror
//
// This file is part of il2fb.exe
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
#include "stdafx.h"
#include <windows.h>

//*************************************************************************
// Suppress new style warning messages
//*************************************************************************
#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable : 4996 )

#define lengthof(a) (sizeof a / sizeof a[0])

//************************************
// Method:    _tmain
// FullName:  _tmain
// Access:    public 
// Returns:   int
// Qualifier:
// Parameter: int argc
// Parameter: _TCHAR * argv[]
//************************************
int _tmain(int argc, _TCHAR* argv[])
{
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	ZeroMemory( &si, sizeof(si) );
	si.cb = sizeof(si);
	ZeroMemory( &pi, sizeof(pi) );
	TCHAR szCmdLine[0x8000];
	ZeroMemory(szCmdLine, sizeof(szCmdLine));
	_tcscpy(szCmdLine, L"il2server.exe ");
	for (int i=1; i<lengthof(argv); i++) {
		_tcscat(szCmdLine, L" ");
		_tcscat(szCmdLine, argv[i]);
	}
	if (CreateProcess(NULL, szCmdLine, NULL,
		NULL, FALSE, 0, NULL, NULL, &si, &pi))
	{
		CloseHandle(pi.hThread);
		WaitForSingleObject(pi.hProcess, INFINITE);
		CloseHandle(pi.hProcess);
	} else {
		_tprintf(L"\r\n");
		_tprintf(L"IL-2 Server launch failed!\r\n");
		_tprintf(L"\r\n");
	}
	return 0;
}

