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

#include "stdafx.h"
#include <stdio.h>
#include <stdlib.h>
#include <share.h>
#include <tchar.h>
#include "globals.h"

#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable : 4996 )

//************************************
// Method:    traceLog
// FullName:  traceLog
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: LPCSTR output
//************************************
bool traceLog(LPCSTR output) {
#ifdef _DEBUG
	OutputDebugStringA(output);
#endif
	if (LogFileName != NULL && _tcslen(LogFileName) != 0) {
		SYSTEMTIME stSystemTime;
		/////////////////////////////////////////////////////////////////////////
		// Generate Time Stamp
		/////////////////////////////////////////////////////////////////////////
		GetLocalTime(&stSystemTime);
		FILE *log;

		while (true) {
			log = _wfsopen(LogFileName, L"ab", _SH_DENYWR);

			if (log != NULL) {
				break;
			}

			Sleep(rand() % 100);
		}

		fprintf(log, "%04d-%02d-%02d %02d:%02d:%02d:%03d (dinput.dll) : %s",
			stSystemTime.wYear,
			stSystemTime.wMonth,
			stSystemTime.wDay,
			stSystemTime.wHour,
			stSystemTime.wMinute,
			stSystemTime.wSecond,
			stSystemTime.wMilliseconds,
			output);
		fflush(log);
		fclose(log);
	}
	return true;
}


//************************************
// Method:    _trace
// FullName:  _trace
// Access:    public 
// Returns:   bool
// Qualifier:
// Parameter: TCHAR * format
// Parameter: ...
//************************************
bool _trace(const TCHAR *format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	int len = _vsctprintf(format, argptr); // _vscprintf doesn't count terminating '\0'
	TCHAR* buffer = (TCHAR*)calloc(len + 1, sizeof(TCHAR));
	_vsntprintf(buffer, len, format, argptr);
	va_end(argptr);
	CHAR* cBufLog = (CHAR*)calloc(len + 1, sizeof(CHAR));
	wcstombs(cBufLog, buffer, len + 1);
	free(buffer);
	bool retVal = traceLog(cBufLog);
	free(cBufLog);
	return retVal;
}

//************************************
// Method:    _trace
// FullName:  _trace
// Access:    public
// Returns:   bool
// Qualifier:
// Parameter: char * format
// Parameter: ...
//************************************
bool _trace(const char *format, ...)
{
	va_list argptr;
	va_start(argptr, format);
	int len = _vscprintf(format, argptr); // _vscprintf doesn't count terminating '\0'
	CHAR* buffer = (CHAR*)calloc(len + 1, sizeof(CHAR));
	_vsnprintf(buffer, len, format, argptr);
	va_end(argptr);
	bool retVal = traceLog(buffer);
	free(buffer);
	return retVal;
}