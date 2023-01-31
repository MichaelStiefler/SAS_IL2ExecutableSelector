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

//*************************************************************************
// Includes
//*************************************************************************
#include "stdafx.h"
#include "PipeInstance.h"
#include "trace.h"
#include "globals.h"
#include "QueueConcurrent.h"
#include "LogWriter.h"
#include <string>

DWORD WINAPI PipeInstanceThread(LPVOID lpvParam)
{
	DWORD curInstance = ++g_dwCurInstance;
	HANDLE hHeap = GetProcessHeap();
	CHAR* pchRequest = (CHAR*)HeapAlloc(hHeap, 0, PIPE_BUF_SIZE * PIPE_BUF_NUM);
	TCHAR* pchLogFile = (TCHAR*)HeapAlloc(hHeap, 0, MAX_PATH * sizeof(TCHAR));
	TQueueConcurrent<std::string> *logQueue = NULL;
	LogWriterParams *logWriterParams = NULL;

	DWORD cbBytesRead = 0, cbReplyBytes = 0, cbWritten = 0, dwThreadId = 0;
	BOOL fSuccess = FALSE;
	HANDLE hPipe = NULL;
	HANDLE hWaitStart = NULL;
	HANDLE hLogWriterThread = NULL;

	// Do some extra error checking since the app will keep running even if this
	// thread fails.

	if (lpvParam == NULL)
	{
		TRACE("ERROR - Pipe Server Failure:\r\n");
		TRACE("   InstanceThread got an unexpected NULL value in lpvParam.\r\n");
		TRACE("   InstanceThread exiting.\r\n");
		if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
		if (pchLogFile != NULL) HeapFree(hHeap, 0, pchLogFile);
		return (DWORD)-1;
	}

	if (pchRequest == NULL)
	{
		TRACE("ERROR - Pipe Server Failure:\r\n");
		TRACE("   InstanceThread got an unexpected NULL heap allocation.\r\n");
		TRACE("   InstanceThread exiting.\r\n");
		if (pchLogFile != NULL) HeapFree(hHeap, 0, pchLogFile);
		return (DWORD)-2;
	}

	// The thread's parameter is a handle to a pipe object instance.

	BOOL bDuplicateHandles = DuplicateHandle(GetCurrentProcess(), ((PipeInstanceParams*)lpvParam)->hPipe, GetCurrentProcess(), &hPipe, 0, FALSE, DUPLICATE_SAME_ACCESS)
		&& DuplicateHandle(GetCurrentProcess(), ((PipeInstanceParams*)lpvParam)->hWaitStart, GetCurrentProcess(), &hWaitStart, 0, FALSE, DUPLICATE_SAME_ACCESS);

	if (!bDuplicateHandles) {
		TRACE("ERROR - Pipe Server Failure:\r\n");
		TRACE("   InstanceThread could not duplicate Pipe Handle\r\n");
		TRACE("   InstanceThread exiting.\r\n");
		if (pchRequest != NULL) HeapFree(hHeap, 0, pchRequest);
		if (pchLogFile != NULL) HeapFree(hHeap, 0, pchLogFile);
		return (DWORD)-3;
	}

	SetEvent(hWaitStart);
	CloseHandle(hWaitStart);

	// Print verbose messages. In production code, this should be for debugging only.
	TRACE(TEXT("InstanceThread No.%d created, receiving and processing messages on %s\r\n"), curInstance, PipeName);

	// Loop until done reading
	while (true)
	{
		// Read client requests from the pipe. This simplistic code only allows messages
		// up to BUFSIZE characters in length.
		fSuccess = ReadFile(
			hPipe,        // handle to pipe 
			pchRequest,    // buffer to receive data 
			PIPE_BUF_SIZE * PIPE_BUF_NUM, // size of buffer 
			&cbBytesRead, // number of bytes read 
			NULL);        // not overlapped I/O 
		pchRequest[cbBytesRead] = '\0';
		// Process Received Message
		if (logQueue == NULL) {
			if (cbBytesRead > 0) {
				size_t pathLen = strlen(pchRequest);

				TRACE("Logfile for instance No.%d is %s\r\n", curInstance, pchRequest);
				ZeroMemory(pchLogFile, sizeof(pchLogFile));
				mbstowcs(pchLogFile, pchRequest, pathLen + 1);

				DWORD dwFlushTimeout = INFINITE;
				if (pathLen < cbBytesRead - 1) {
					CHAR* pchTimeout = pchRequest + pathLen + 1;
					try {
						dwFlushTimeout = atoi(pchTimeout);
						if (dwFlushTimeout == INFINITE)
							TRACE("Instance No.%d uses (explicit) lazy flushing\r\n", curInstance);
						else if (dwFlushTimeout == 0)
							TRACE("Instance No.%d uses instant flushing\r\n", curInstance);
						else
							TRACE("Flush Timeout for instance No.%d is %d Milliseconds\r\n", curInstance, dwFlushTimeout);
					}
					catch (...) {}
				}
				else {
					TRACE("Instance No.%d uses lazy flushing\r\n", curInstance);
				}

				logQueue = new TQueueConcurrent<std::string>();

				logWriterParams = new LogWriterParams();
				logWriterParams->pchLogFile = pchLogFile;
				logWriterParams->logQueue = logQueue;
				logWriterParams->hNewData = CreateEvent(NULL, FALSE, FALSE, NULL);
				logWriterParams->hEndLogWriter = CreateEvent(NULL, FALSE, FALSE, NULL);
				logWriterParams->dwFlushTimeout = dwFlushTimeout;
				logWriterParams->dwIndex = curInstance;

				// Create a thread for this client. 
				hLogWriterThread = CreateThread(
					NULL,              // no security attribute 
					0,                 // default stack size 
					LogWriterThread,   // thread proc
					(LPVOID)logWriterParams,// thread parameter 
					0,                 // not suspended 
					&dwThreadId);      // returns thread ID 

				if (hLogWriterThread == NULL || hLogWriterThread == INVALID_HANDLE_VALUE)
				{
					TRACE("Pipe Instance No.%d CreateThread for Logfile Writer failed, GLE=%d.\r\n", curInstance, GetLastError());
					return -2;
				}
			}
		}
		else {
			if (hLogWriterThread != NULL && hLogWriterThread != INVALID_HANDLE_VALUE && WaitForSingleObject(hLogWriterThread, 0) != WAIT_OBJECT_0) {
				logQueue->emplace_back(std::string(pchRequest));
				SetEvent(logWriterParams->hNewData);
			}
		}

		if (!fSuccess || cbBytesRead == 0)
		{
			if (GetLastError() == ERROR_BROKEN_PIPE)
			{
				TRACE("InstanceThread No.%d: client disconnected.\r\n", curInstance, GetLastError());
			}
			else
			{
				TRACE("InstanceThread No.%d ReadFile failed, GLE=%d.\r\n", curInstance, GetLastError());
			}
			break;
		}

	}

	TRACE("InstanceThread No.%d terminating...\r\n", curInstance);

	if (logWriterParams != NULL) {
		SetEvent(logWriterParams->hEndLogWriter);
		if (hLogWriterThread != NULL && hLogWriterThread != INVALID_HANDLE_VALUE) {
			WaitForSingleObject(hLogWriterThread, MAX_THREAD_WAIT_TIME);
		}
		CloseHandle(logWriterParams->hEndLogWriter);
		CloseHandle(logWriterParams->hNewData);
		delete logWriterParams;
	}

	DisconnectNamedPipe(hPipe);
	CloseHandle(hPipe);
	if (hLogWriterThread != NULL && hLogWriterThread != INVALID_HANDLE_VALUE) {
		CloseHandle(hLogWriterThread);
		hLogWriterThread = NULL;
	}

	HeapFree(hHeap, 0, pchRequest);
	HeapFree(hHeap, 0, pchLogFile);

	TRACE("InstanceThread No.%d terminated\r\n", curInstance);

	return 0;
}