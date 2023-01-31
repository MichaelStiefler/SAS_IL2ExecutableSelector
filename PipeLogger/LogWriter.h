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

#pragma once
#include "QueueConcurrent.h"

struct LogWriterParams {
	TCHAR *pchLogFile;
	TQueueConcurrent<std::string> *logQueue;
	HANDLE hNewData;
	HANDLE hEndLogWriter;
	DWORD  dwFlushTimeout;
	DWORD  dwIndex;
	LogWriterParams() {
		pchLogFile = NULL;
		logQueue = NULL;
		hNewData = INVALID_HANDLE_VALUE;
		hEndLogWriter = INVALID_HANDLE_VALUE;
		dwFlushTimeout = 0;
		dwIndex = 0;
	}
	LogWriterParams(LogWriterParams *lwp) {
		pchLogFile = lwp == NULL ? NULL : lwp->pchLogFile;
		logQueue = lwp == NULL ? NULL : lwp->logQueue;
		hNewData = lwp == NULL ? INVALID_HANDLE_VALUE : lwp->hNewData;
		hEndLogWriter = lwp == NULL ? INVALID_HANDLE_VALUE : lwp->hEndLogWriter;
		dwFlushTimeout = lwp == NULL ? 0 : lwp->dwFlushTimeout;
		dwIndex = lwp == NULL ? 0 : lwp->dwIndex;
	}
};

DWORD WINAPI LogWriterThread(LPVOID lpvParam);