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

#include "StdAfx.h"
#include <set>
#include <string>
#include <stdarg.h>
#include <windows.h>
#include <atomic>

// Suppress compiler warnings about unsafe functions
#define _CRT_SECURE_NO_WARNINGS
#pragma warning( disable : 4996 )

#define RunWorker(ThreadStartRoutine) 	CloseHandle(CreateThread(NULL,0,(LPTHREAD_START_ROUTINE)ThreadStartRoutine,NULL,0,NULL))
#define PIPE_BUF_SIZE 4096
#define PIPE_BUF_NUM  128
#define PipeName L"\\\\.\\pipe\\SAS_PIPE_LOGGER"
#define LOGFILE_NAME		L"initlog.lst"
#define MAX_THREAD_WAIT_TIME 5000
#define MAX_ALL_WAIT_TIME 10000

void format_commas(unsigned long n, TCHAR* out);

//*************************************************************************
// Type definitions
//*************************************************************************

//*************************************************************************
// Globals
//*************************************************************************
extern TCHAR g_szLogFileName[MAX_PATH];
extern TCHAR g_szBuf[1024];
extern CHAR g_cBuf[1024];
extern CHAR g_cPipeBuf[PIPE_BUF_SIZE];
extern std::atomic<int> g_aiLogWriters;
extern std::atomic<int> g_aiPendingMessages;
extern HANDLE g_hTerminatePipeLogger;
extern DWORD g_dwCurInstance;
extern BOOL g_bShowLogWarnings;
extern unsigned long g_ulLogSizeThreshold;
extern unsigned long g_ulExceptionThreshold;
extern unsigned long g_ulErrorThreshold;
