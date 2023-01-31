//*****************************************************************
// PipeLogger.exe - Enhanced Logging Tool for IL-2 1946
// Copyright (C) 2021 SAS~Storebror
//
// This file is part of PipeLogger.exe.
//
// PipeLogger.exe is free software.
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

#include "globals.h"

// Suppress compiler warnings about unsafe functions
#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable : 4996 )

//*************************************************************************
// Globals
//*************************************************************************
TCHAR g_szLogFileName[MAX_PATH];
TCHAR g_szBuf[1024];
CHAR g_cBuf[1024];
CHAR g_cPipeBuf[PIPE_BUF_SIZE];
std::atomic<int> g_aiLogWriters = 0;
std::atomic<int> g_aiPendingMessages = 0;
HANDLE g_hTerminatePipeLogger;
DWORD g_dwCurInstance = 0;
BOOL g_bShowLogWarnings = FALSE;
unsigned long g_ulLogSizeThreshold = 10000000;
unsigned long g_ulExceptionThreshold = 10;
unsigned long g_ulErrorThreshold = 0;

void format_commas(unsigned long n, TCHAR* out)
{
	int c;
	TCHAR buf[20];
	TCHAR* p;

	_stprintf(buf, L"%d", n);
	c = 2 - _tcslen(buf) % 3;
	for (p = buf; *p != 0; p++) {
		*out++ = *p;
		if (c == 1) {
			*out++ = L',';
		}
		c = (c + 1) % 3;
	}
	*--out = 0;
}

