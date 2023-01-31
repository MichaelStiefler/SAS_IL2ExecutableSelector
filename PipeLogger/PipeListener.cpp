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
#include "PipeListener.h"
#include "PipeInstance.h"
#include "trace.h"
#include "globals.h"

DWORD WINAPI PipeListenerThread(LPVOID lpvParam)
{
	DWORD dwThreadId;
	TRACE(TEXT("Listener thread awaiting client connection on %s\r\n"), PipeName);
	PipeInstanceParams *pipeInstanceParams = new PipeInstanceParams();
	pipeInstanceParams->hWaitStart = CreateEvent(NULL, FALSE, FALSE, NULL);
	if (pipeInstanceParams->hWaitStart == NULL || pipeInstanceParams->hPipe == INVALID_HANDLE_VALUE)
	{
		TRACE("Listener thread creating Pipe Instance Wait Handle failed, GLE=%d.\r\n", GetLastError());
		return -1;
	}
	pipeInstanceParams->hPipe = CreateNamedPipe(
		PipeName,						// pipe name
		PIPE_ACCESS_INBOUND,			// read access
		PIPE_TYPE_MESSAGE |				// message type pipe
		PIPE_READMODE_MESSAGE |			// message-read mode
		PIPE_WAIT,						// blocking mode
		PIPE_UNLIMITED_INSTANCES,		// max. instances
		PIPE_BUF_SIZE * PIPE_BUF_NUM,   // output buffer size
		PIPE_BUF_SIZE * PIPE_BUF_NUM,   // input buffer size
		NMPWAIT_USE_DEFAULT_WAIT,		// client time-out
		NULL);							// default security attribute

	if (pipeInstanceParams->hPipe == NULL || pipeInstanceParams->hPipe == INVALID_HANDLE_VALUE)
	{
		TRACE("CreateNamedPipe failed, GLE=%d.\r\n", GetLastError());
		CloseHandle(pipeInstanceParams->hWaitStart);
		return -2;
	}

	// Wait for the client to connect; if it succeeds, 
	// the function returns a nonzero value. If the function
	// returns zero, GetLastError returns ERROR_PIPE_CONNECTED. 

	BOOL fConnected = ConnectNamedPipe(pipeInstanceParams->hPipe, NULL) ?
		TRUE : (GetLastError() == ERROR_PIPE_CONNECTED);

	if (!fConnected) {
		// The client could not connect, so close the pipe. 
		TRACE(TEXT("ConnectNamedPipe failed, GLE=%d.\r\n"), GetLastError());
		CloseHandle(pipeInstanceParams->hPipe);
		CloseHandle(pipeInstanceParams->hWaitStart);
		return -3;
	}

	TRACE("Client connected, creating a new Listener Thread & new Instance Thread for this Client.\r\n");

	RunWorker(PipeListenerThread);

	// Create a thread for this client. 
	HANDLE hThread = CreateThread(
		NULL,              // no security attribute 
		0,                 // default stack size 
		PipeInstanceThread,// thread proc
		(LPVOID)pipeInstanceParams,     // thread parameter 
		0,                 // not suspended 
		&dwThreadId);      // returns thread ID 

	if (hThread == NULL)
	{
		TRACE(TEXT("Pipe Instance CreateThread failed, GLE=%d.\r\n"), GetLastError());
		CloseHandle(pipeInstanceParams->hPipe);
		CloseHandle(pipeInstanceParams->hWaitStart);
		return -2;
	}
	CloseHandle(hThread);
	TRACE("Waiting for Pipe Instance to initialize...\r\n");
	if (WaitForSingleObject(pipeInstanceParams->hWaitStart, MAX_THREAD_WAIT_TIME) == WAIT_OBJECT_0) {
		TRACE("Pipe Instance initialized successfully\r\n");
	}
	else {
		TRACE("Pipe Instance initialization timed out!\r\n");
	}
	CloseHandle(pipeInstanceParams->hPipe);
	CloseHandle(pipeInstanceParams->hWaitStart);
	delete pipeInstanceParams;
	return 0;
}